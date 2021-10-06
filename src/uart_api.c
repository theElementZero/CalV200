/*
 * uart_api.c
 *
 *  Created on: 26-Jan-2021
 *      Author: ABC
 */

#include <msp430.h>
#include <stdint.h>
#include <string.h>
#include "inc/lw_rngbuf.h"
#include "inc/uart_api.h"

LW_RING_BUFFER_T uart_rb;

uint8_t TxIntStat = 0;

const char LOC_MATCH[] = /*{"GPTXT"};*/{"GPRMC"};

const char ANT_MATCH2[] = {"ANTSTATUS="};

typedef struct __uart_process
{
	uint16_t state;
	uint16_t antenna_status;
	uint16_t counter;
	uint16_t reading_counter;
	uint16_t ant_status_counter;
	uint16_t flag;
	char ant_status[32];
	char reading[64];
}uart_process;

enum GPS_PROTO
{
	MATCH_HEADER,
	MATCH_IDENTIFY,
	MATCH_ANTENNA_STATUS_PRO,
	MATCH_GPS_READING_PRO,
	MATCH_PUSH_LCD
};

enum GPS_PROTO_STATUS_FLAG
{
	flag_capture_none,
	flag_capture_ant_status,
	flag_got_reading,
	flag_lock_reading
};

static uart_process uart_var = {0};
void reset_uart_rx_buffer();


void uart_task()
{
	uint8_t tmp_char = 0;
	char *ret = 0;
	tmp_char = get_uart0_char();

	if(uart_var.flag && flag_lock_reading)
	{
	    return;
	}

	switch(uart_var.state){
	case MATCH_HEADER:
		if(tmp_char == '$')
		{
			// goto next match to differentiate between protocol
			uart_var.state = MATCH_IDENTIFY;
			uart_var.counter = 0;
			uart_var.reading_counter = 0;
		}
		break;
	case MATCH_IDENTIFY:
		if(tmp_char == LOC_MATCH[uart_var.counter]){
			uart_var.counter++;
			// packet is of location
			if(uart_var.counter > 3){
				uart_var.state = MATCH_GPS_READING_PRO;
			}
		}else if(tmp_char == ANT_MATCH2[uart_var.counter]){
			uart_var.counter++;
			if(uart_var.counter > 3)
			{
				uart_var.state = MATCH_ANTENNA_STATUS_PRO;
				uart_var.ant_status_counter = 0;
			}
		}else{
			uart_var.state = MATCH_HEADER;
			__BUF_RESET(uart_rb.rx_head);
			__BUF_RESET(uart_rb.rx_tail);

			uart_var.counter = 0;

		}

		break;
	case MATCH_GPS_READING_PRO:
		// get reading
		uart_var.reading[uart_var.reading_counter++] = tmp_char;
		if(tmp_char == 0x0d)
		{
			uart_var.flag |= flag_got_reading;
			uart_var.state = MATCH_HEADER;
			reset_uart_rx_buffer();
		}
		break;
	case MATCH_ANTENNA_STATUS_PRO:
		// read antenna status
		uart_var.ant_status[uart_var.ant_status_counter++] = tmp_char;
		if(tmp_char == 0x0d)
		{
			ret = strstr((char *)uart_var.ant_status, ANT_MATCH2);
			if(ret)
			{
				ret += strlen(ANT_MATCH2);
				memset(uart_var.reading, 0, 8);
				memcpy(uart_var.reading, ret, 4);
			}
			uart_var.state = MATCH_HEADER;
		}
		break;
	}
}


void init_uart()
{
    TxIntStat = 0;

	// Reset ring buf head and tail idx
	__BUF_RESET(uart_rb.rx_head);
	__BUF_RESET(uart_rb.rx_tail);
	__BUF_RESET(uart_rb.tx_head);
	__BUF_RESET(uart_rb.tx_tail);

}

void UART_int_control(uint8_t lu8_status)
{
	if(lu8_status){
		UCA0IE |= UCTXIE ;
	}else{
		UCA0IE &= ~UCTXIE;
	}
}


void UARTRX_int_control(uint8_t lu8_status)
{
	if(lu8_status){
		UCA0IE |= UCRXIE ;
	}else{
		UCA0IE &= ~UCRXIE;
	}
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=EUSCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(EUSCI_A0_VECTOR))) USCI_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{
	uint8_t temp_rx = 0;
    switch(__even_in_range(UCA0IV, USCI_UART_UCTXCPTIFG))
    {
        case USCI_NONE: break;
        case USCI_UART_UCRXIFG:
            while(!(UCA0IFG&UCTXIFG));
            temp_rx = UCA0RXBUF;
            /* Check if buffer is more space
			 * If no more space, remaining character will be trimmed out
			 */
			if (!__BUF_IS_FULL(uart_rb.rx_head,uart_rb.rx_tail)){
				uart_rb.rx[uart_rb.rx_head] = temp_rx;
				__BUF_INCR(uart_rb.rx_head);
			}
            __no_operation();
            break;
        case USCI_UART_UCTXIFG:

        	while (!__BUF_IS_EMPTY(uart_rb.tx_head,uart_rb.tx_tail))
            {
                /* Move a piece of data into the transmit FIFO */
        		UCA0TXBUF = uart_rb.tx[uart_rb.tx_tail];
        		 /* Update transmit ring FIFO tail pointer */
        		__BUF_INCR(uart_rb.tx_tail);
            }

            /* If there is no more data to send, disable the transmit
               interrupt - else enable it or keep it enabled */
        	if (__BUF_IS_EMPTY(uart_rb.tx_head, uart_rb.tx_tail)) {
        		UART_int_control(0);
            	// Reset Tx Interrupt state
            	TxIntStat = 0;
            }
            else{
              	// Set Tx Interrupt state
        		TxIntStat = 1;
        		UART_int_control(1);
            }

        	break;
        case USCI_UART_UCSTTIFG: break;
        case USCI_UART_UCTXCPTIFG: break;
        default: break;
    }
}


/*********************************************************************//**
 * @brief		UART transmit function for interrupt mode (using ring buffers)

 * @param[out]	txbuf Pointer to Transmit buffer
 * @param[in]	buflen Length of Transmit buffer
 * @return 		Number of bytes actually sent to the ring buffer
 **********************************************************************/
void UARTSend( uint8_t *txbuf, uint8_t buflen)
{
	uint8_t *data = (uint8_t *) txbuf;
	uint16_t bytes = 0;
	UART_int_control(0);
	/* Loop until transmit run buffer is full or until n_bytes
	   expires */
	while ((buflen > 0) && (!__BUF_IS_FULL(uart_rb.tx_head, uart_rb.tx_tail)))
	{
		/* Write data from buffer into ring buffer */
		uart_rb.tx[uart_rb.tx_head] = *data;
		data++;

		/* Increment head pointer */
		__BUF_INCR(uart_rb.tx_head);

		/* Increment data count and decrement buffer size count */
		bytes++;
		buflen--;
	}
	/*
	 * Check if current Tx interrupt enable is reset,
	 * that means the Tx interrupt must be re-enabled
	 * due to call UART_IntTransmit() function to trigger
	 * this interrupt type
	 */
	if (TxIntStat == 0) {
		// kick start transmission
		/* Move a piece of data into the transmit FIFO */
		UCA0TXBUF = uart_rb.tx[uart_rb.tx_tail];
		 /* Update transmit ring FIFO tail pointer */
		__BUF_INCR(uart_rb.tx_tail);
	}
	/*
	 * Otherwise, re-enables Tx Interrupt
	 */
	else {
		UART_int_control(1);
	}
}

char get_uart0_char()
{
	char temp = 0;
	if(__BUF_COUNT(uart_rb.rx_head, uart_rb.rx_tail) > 0)
	{
		temp = uart_rb.rx[uart_rb.rx_tail];
		__BUF_INCR(uart_rb.rx_tail);
	}
	return temp;
}

void reset_uart_rx_buffer()
{
	__BUF_RESET(uart_rb.rx_tail);
	__BUF_RESET(uart_rb.rx_head);
}

void clr_uart_flag_status()
{
	uart_var.flag &= ~flag_got_reading;
}

uint16_t get_reading_flag_status()
{
	return (uart_var.flag && flag_got_reading);
}

void lock_gps_reading_memory()
{
    uart_var.flag |= flag_lock_reading;
}

void unlock_gps_reading_memory()
{
    uart_var.flag &= ~flag_lock_reading;
}
char * get_uart_reading_ptr()
{
	return uart_var.reading;
}
