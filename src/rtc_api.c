/*
 * rtc_api.c
 *
 *  Created on: 30-Sep-2021
 *      Author: ABC
 */


#include <msp430.h>
#include <stdint.h>
#include <string.h>
#include "msp430.h"
#include "inc/rtc_api.h"

void init_RTC(){
    PJSEL0 = BIT4 | BIT5;                   // Initialize LFXT pins

    // Configure LFXT 32kHz crystal
    CSCTL0_H = CSKEY_H;                     // Unlock CS registers
    CSCTL4 &= ~LFXTOFF;                     // Enable LFXT
    do
    {
      CSCTL5 &= ~LFXTOFFG;                  // Clear LFXT fault flag
      SFRIFG1 &= ~OFIFG;
    } while (SFRIFG1 & OFIFG);              // Test oscillator fault flag
    CSCTL0_H = 0;                           // Lock CS registers

    // Configure RTC_C
    RTCCTL0_H = RTCKEY_H;                   // Unlock RTC
    RTCCTL0_L = RTCTEVIE_L | RTCRDYIE_L;    // enable RTC read ready interrupt
                                            // enable RTC time event interrupt

    RTCCTL13 = RTCBCD | RTCHOLD | RTCMODE;  // RTC enable, BCD mode, RTC hold

}

void start_RTC()
{
    RTCCTL13 &= ~(RTCHOLD);                 // Start RTC
}

void stop_RTC()
{
    RTCCTL13 |= (RTCHOLD);                 // Stop RTC
}

void set_rtc(rtc_cfg rtc_in)
{
    RTCYEAR = rtc_in.year;                       // Year = 0x2010
    RTCMON = rtc_in.month;                           // Month = 0x04 = April
    RTCDAY = rtc_in.day;                          // Day = 0x05 = 5th
    RTCDOW = rtc_in.dow;                          // Day of week = 0x01 = Monday
    RTCHOUR = rtc_in.hour;                         // Hour = 0x10
    RTCMIN = rtc_in.minute;                          // Minute = 0x32
    RTCSEC = rtc_in.seconds;                          // Seconds = 0x45
}


void get_rtc(rtc_cfg *rtc_in)
{
    rtc_in->year = RTCYEAR;                       // Year = 0x2010
    rtc_in->month = RTCMON;                           // Month = 0x04 = April
    rtc_in->day = RTCDAY;                          // Day = 0x05 = 5th
    rtc_in->dow = RTCDOW;                          // Day of week = 0x01 = Monday
    rtc_in->hour = RTCHOUR;                         // Hour = 0x10
    rtc_in->minute = RTCMIN;                          // Minute = 0x32
    rtc_in->seconds = RTCSEC;                          // Seconds = 0x45
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=RTC_C_VECTOR
__interrupt void RTC_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(RTC_C_VECTOR))) RTC_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(RTCIV, RTCIV__RT1PSIFG))
    {
        case RTCIV__NONE:      break;       // No interrupts
        case RTCIV__RTCOFIFG:  break;       // RTCOFIFG
        case RTCIV__RTCRDYIFG:              // RTCRDYIFG
            __no_operation();
            break;
        case RTCIV__RTCTEVIFG:              // RTCEVIFG
            __no_operation();               // Interrupts every minute - SET BREAKPOINT HERE
            break;
        case RTCIV__RTCAIFG:   break;       // RTCAIFG
        case RTCIV__RT0PSIFG:  break;       // RT0PSIFG
        case RTCIV__RT1PSIFG:  break;       // RT1PSIFG
        default: break;
    }
}
