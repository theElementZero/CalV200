/*
 * uart_api.h
 *
 *  Created on: 26-Jan-2021
 *      Author: ABC
 */

#ifndef UART_API_H_
#define UART_API_H_


void init_uart();

char get_uart0_char();

void uart_task();
void clr_uart_flag_status();
uint16_t get_reading_flag_status();
void UARTRX_int_control(uint8_t lu8_status);
char * get_uart_reading_ptr();
void lock_gps_reading_memory();
void unlock_gps_reading_memory();

#endif /* UART_API_H_ */
