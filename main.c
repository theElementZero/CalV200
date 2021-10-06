#include <msp430.h> 
#include "inc/projectopts.h"
#include "inc/main.h"


#define TEST_DEBUG_PACKECT 0

#define MAIN_TIMEOUT0 BIT0
#define MAIN_STATE_LOCK BIT1
#define MAIN_MENU_LOCK BIT2


uint16_t u16Led_counter;
uint16_t u16_disp_counter;
uint16_t u16_delay1_counter;
uint16_t u16_shut_counter;

//! brief main variable to store process data
static main_var vMain_Process;
//! brief
static LW_RING_BUFFER_T rg_main_state;

const char LCD_TEMPLATE1[] = "GPS v2.01 is unreachable  ";

static char lcd_data_live_reading[128];// = "SUNRISE: 00:00, SUNSET: 00:00, Date: dd-mm-yy ";

const char test_gps_template[] = "$GPRMC,150030.000,A,19.1196976,N,72.8464205,E,0.16,48.45,051021,,,A*5D";

const char err_code_msg[] = "GPS Err=0 ";


void init_gpio()
{
    // Configure GPIO
	P2SEL1 &= ~(BIT0 | BIT1);
	P2SEL0 |= (BIT0 | BIT1);				// USCI_A0` UART operation

	// 2 led, gps en pin
    // Configure GPIO
    P3DIR |= BIT7 | BIT3 | BIT6;
    P3OUT &=  ~(BIT7 | BIT6);

    P4DIR &= ~( BIT6 | BIT7 | BIT3);

}

void set_clock_and_uart()
{

    // Startup clock system with max DCO setting ~8MHz
    CSCTL0_H = CSKEY_H;                     // Unlock CS registers
    CSCTL1 = DCOFSEL_3 | DCORSEL;           // Set DCO to 8MHz
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;   // Set all dividers
    CSCTL0_H = 0;                           // Lock CS registers

    // Configure USCI_A0 for UART mode
    UCA0CTLW0 = UCSWRST;                    // Put eUSCI in reset
    UCA0CTLW0 |= UCSSEL__SMCLK;             // CLK = SMCLK
    // Baud Rate calculation
    // 8000000/(16*9600) = 52.083
    // Fractional portion = 0.083
    // User's Guide Table 21-4: UCBRSx = 0x04
    // UCBRFx = int ( (52.083-52)*16) = 1
    UCA0BRW = 52;                           // 8000000/16/9600
    UCA0MCTLW |= UCOS16 | UCBRF_1 | 0x4900;
    UCA0CTLW0 &= ~UCSWRST;                  // Initialize eUSCI
    UCA0IE |= UCRXIE;                       // Enable USCI_A0 RX interrupt

    //Timer A0
    TA0CCTL0 = CCIE;                        // TACCR0 interrupt enabled
    TA0CCR0 = 50000;
    TA0CTL = TASSEL__SMCLK | MC__CONTINOUS; // SMCLK, continuous mode

    //Timer B0
    TB0CCTL0 = CCIE;                        // TBCCR0 interrupt enabled
    TB0CCR0 = 50000;
    TB0CTL = TBSSEL__SMCLK | MC__CONTINOUS; // SMCLK, continuous mode


    __bis_SR_register( GIE);

}

void init_main_variables()
{
	u16_delay1_counter = 0;
	u16_shut_counter = 0;
	u16_disp_counter = __TIMER_COUNT1__;
	vMain_Process.state = state_main_boot_gps;
	vMain_Process.key1_status = (P4IN && BIT6);
	vMain_Process.local_rtc.year = 0x2021;
	vMain_Process.local_rtc.month = 0x9;
	vMain_Process.local_rtc.day = 0x30;
	vMain_Process.local_rtc.hour = 0x15;
	vMain_Process.local_rtc.minute = 0x30;
	vMain_Process.local_rtc.seconds = 0x00;
}


void displayDate(ST_DATE *date, char *arr)
{
	arr[0] = (date->day / 10) + 0x30;
	arr[1] = (date->day %10) + 0x30;

	arr[3] = (date->mon / 10) + 0x30;
	arr[4] = (date->mon %10) + 0x30;

	arr[6] = (date->yr / 10) + 0x30;
	arr[7] = (date->yr %10) + 0x30;
}


#define LOCK_MAIN_STATE_QUEUE() set_bit_mask(&vMain_Process.status_flag, MAIN_STATE_LOCK)

#define UNLOCK_MAIN_STATE_QUEUE() clr_bit_mask(&vMain_Process.status_flag, MAIN_STATE_LOCK)

#define IS_MAIN_FLAG_SET() get_bit_status(&vMain_Process.status_flag, MAIN_TIMEOUT0)

#define SET_MAIN_FLAG() set_bit_mask(&vMain_Process.status_flag, MAIN_TIMEOUT0)

#define CLR_MAIN_FLAG() clr_bit_mask(&vMain_Process.status_flag, MAIN_TIMEOUT0)

#define IS_MENU_FLAG_SET() get_bit_status(&vMain_Process.status_flag, MAIN_MENU_LOCK)

void SET_MENU_FLAG() { set_bit_mask(&vMain_Process.status_flag, MAIN_MENU_LOCK) ; }

void CLR_MENU_FLAG() { clr_bit_mask(&vMain_Process.status_flag, MAIN_MENU_LOCK); }

#define   is_main_state_full()   __BUF_IS_FULL(rg_main_state.rx_head, rg_main_state.rx_tail)

#define is_main_state_empty()  __BUF_IS_EMPTY(rg_main_state.rx_head, rg_main_state.rx_tail)

void insert_main_state(uint16_t state)
{
    /*wait if statusflag is set*/
    while(get_bit_status(&vMain_Process.status_flag, MAIN_STATE_LOCK) == 1);

    /*take access of flag*/
    LOCK_MAIN_STATE_QUEUE();

    /*insert state in ring buffer aka queue*/
    insert_lw_rngbuf(&rg_main_state, state);

    /*release access of flag*/
    UNLOCK_MAIN_STATE_QUEUE();
}

void pop_main_state(uint16_t *state)
{
    /*wait if statusflag is set*/
    while(get_bit_status(&vMain_Process.status_flag, MAIN_STATE_LOCK) == 1);

    /*take access of flag*/
    LOCK_MAIN_STATE_QUEUE();

    /*insert state in ring buffer aka queue*/
    *state = pop_lw_rngbuf(&rg_main_state);

    /*release access of flag*/
    UNLOCK_MAIN_STATE_QUEUE();
}



void machine_state()
{
#if !TEST_DEBUG_PACKECT
	char tmp_v = 0;
#endif
	float tmp = 0.0;
	long int calc_jd = 0;

	calc_jd = is_main_state_full();
	/*is queue full/empty*/
//	if(is_main_state_full() /*) && ((vMain_Process.status_flag & MAIN_MENU_LOCK ) == 0) */)
	if(!is_main_state_empty())
	{

	    pop_main_state(&vMain_Process.state);

        switch(vMain_Process.state)
        {
        case state_main_boot_gps:
            //P3OUT |= BIT3;
            select_font(font_Times_New_Roman42x44);
            enable_rolling_display(1);

            unlock_gps_reading_memory();
            UARTRX_int_control(1); // enable UART
//            vMain_Process.state = state_main_wait_gps_coordinate;
            insert_main_state(state_main_wait_gps_coordinate);
            break;
        case state_main_shut_gps:
            //P3OUT &= ~BIT3;

            insert_main_state(state_main_idle);
            break;
        case state_main_display_err:

#if TEST_DEBUG_PACKECT
            memcpy(lcd_data_live_reading, get_uart_reading_ptr(), 48);
            update_rolling_display(0, &lcd_data_live_reading[0], strlen(lcd_data_live_reading), 0);
#else

            //strcpy(err_code_msg, "GPS Err=0 ");
            tmp_v = vMain_Process.err_code + 0x30;

            clr_rolling_display();
            update_rolling_display( 0,(char *) err_code_msg, strlen(err_code_msg), 0);
            update_rolling_display(strlen("GPS Err="), &tmp_v, 1, 1);
#endif

            u16_delay1_counter = 2000;

            if(++u16_shut_counter > 5){
                u16_shut_counter = 0;
                insert_main_state(state_main_shut_gps);
            }else{
                insert_main_state(state_main_idle);
            }
            break;
        case state_main_wait_gps_coordinate:
//            if(get_reading_flag_status())
            {
                lock_gps_reading_memory();

                clr_uart_flag_status()  ;

                UARTRX_int_control(0);

                memcpy(lcd_data_live_reading,/* */ test_gps_template, 128); //get_uart_reading_ptr(), 128);

                vMain_Process.err_code = decryptData(lcd_data_live_reading ); //get_uart_reading_ptr() );

                if( vMain_Process.err_code == 0 )
    //			if( decryptData((char *)test_gps_template) == 0)
                {
                    //P3OUT |= BIT7 ;
                    insert_main_state(state_compute_reading);
                }else{
                    //P3OUT &= ~BIT7 ;
                    insert_main_state(state_main_display_err);
                }
            }
            break;
        case state_display_reading:
#if TEST_DEBUG_PACKECT
		memcpy(lcd_data_live_reading, get_uart_reading_ptr(), 48);
		update_rolling_display(0, &lcd_data_live_reading[0], strlen(lcd_data_live_reading), 0);
#else
            memcpy(lcd_data_live_reading, "SUNRISE: 00:00, SUNSET: 00:00, Date: dd-mm-yy ", 46);

            set_xy(0, 5);

            fndms(&vMain_Process.riseTime, &lcd_data_live_reading[9]);

            fndms(&vMain_Process.setTime, &lcd_data_live_reading[24]);
            //displayDate(&stdate, &lcd_data_live_reading[37]);

            if( dayofweek(&stdate) == 5 )
            {
                is_it_friday(1, &lcd_data_live_reading[24], &vMain_Process.setTime);
            }else{

            }

            calc_jd = gregorian_calendar_to_jd(stdate.yr +2000, stdate.mon, stdate.day);

            CALENDER_N hebrew_date = jdToHebrew(calc_jd);

            displayHebrew(&hebrew_date, &lcd_data_live_reading[37]);

            get_hebrew_holiday(hebrew_date, lcd_data_live_reading + strlen(lcd_data_live_reading), &vMain_Process.setTime);

            update_rolling_display(0, &lcd_data_live_reading[0], strlen(lcd_data_live_reading), 0);
#endif
            insert_main_state(state_main_shut_gps);
            break;
        case state_display_reading_hebrew:
            break;
        case state_compute_reading:

            tmp = computeSunrise( 1);

            vMain_Process.riseTime =  (unsigned int)tmp;

            // add GMT off set
            vMain_Process.riseTime += get_GMT_offset_in_minutes();

            if (is_summer_time()) {
                vMain_Process.riseTime += 120; // 2HR israel to UT conversion
            }

            tmp = computeSunrise( 0);

            vMain_Process.setTime =  (unsigned int)tmp;

            // add GMT off set
            vMain_Process.setTime += get_GMT_offset_in_minutes();

            if (is_summer_time()) {
                vMain_Process.setTime += 120;
            }
            insert_main_state(state_display_reading);
            break;

        case state_main_read_rtc:

            get_rtc(&vMain_Process.local_rtc);

            /*compare gps & local date*/
            if( (vMain_Process.local_rtc.year == stdate.yr) && (vMain_Process.local_rtc.month) )
            {
                //Display time
            }else{

            }
            break;

        case state_main_write_rtc:
                break;

        }// switch
    }// if
}
/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	// Disable the GPIO power-on default high-impedance mode to activate
	// previously configured port settings
	PM5CTL0 &= ~LOCKLPM5;

    init_gpio();
    set_clock_and_uart();

    init_uart();
    init_main_variables();
    init_rolling_disp();

    init(); // glcd
    init_menu();

    init_RTC();

    set_rtc(vMain_Process.local_rtc);

    start_RTC();

    update_rolling_display(0,(char *) LCD_TEMPLATE1, strlen(LCD_TEMPLATE1), 0);

    insert_main_state(state_main_boot_gps);

    while(1)
	{
		if( IS_MAIN_FLAG_SET() ){
			machine_state();
			CLR_MAIN_FLAG();
		}
		uart_task();

		if((vMain_Process.status_flag && MAIN_MENU_LOCK ) == 0)
		rolling_display();
	}
//	return 0;
}



// Timer0_A0 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer0_A0_ISR (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer0_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{
	if(++u16Led_counter > 20){
#if 0
	    P3OUT ^= BIT6;
#endif
	    u16Led_counter = 0;
	    update_rolling_disp_state(rolling_disp_change);

	    if(u16_disp_counter){
	    	--u16_disp_counter;
	    	if(!u16_disp_counter)
	    	{
	    		u16_disp_counter = __TIMER_COUNT1__;
	    		SET_MAIN_FLAG();
	    	}
	    }
	    blinker();
	}

	if (u16_delay1_counter)
	{
		--u16_delay1_counter;
		if (!u16_delay1_counter)
		{
			insert_main_state(state_main_boot_gps);
		}
	}
	menu_selection( filter_keypad() );
}


// Timer0_B0 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_B0_VECTOR
__interrupt void Timer0_B0_ISR (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_B0_VECTOR))) Timer0_B0_ISR (void)
#else
#error Compiler not supported!
#endif
{
    TB0CCR0 += 50000;                       // Add Offset to TBCCR0
}
