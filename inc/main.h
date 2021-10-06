/*
 * main.h
 *
 *  Created on: 24-Mar-2021
 *      Author: ABC
 */

#ifndef INC_MAIN_H_
#define INC_MAIN_H_
#include "inc/rtc_api.h"
#include "inc/gps_data.h"

enum main_process_state
{
	state_main_idle,
	state_main_boot_gps,
	state_main_shut_gps,
	state_main_update_lcd,
	state_main_wait_gps_coordinate,
	state_main_get_reading,
	state_main_evaluate_gps,
	state_display_reading,
	state_display_reading_hebrew,
	state_compute_reading,
	state_main_display_err,
	state_main_delay1_wait,
	state_main_read_rtc,
	state_main_write_rtc
};

#define __TIMER_COUNT1__ 20

typedef struct __main_var
{
	uint16_t key1_status;
	uint16_t err_code;
	uint16_t state;
	uint16_t state_flag;
	uint16_t status_flag;
	unsigned int riseTime;
	unsigned int setTime;

	rtc_cfg local_rtc;

	ST_DATE hebrew_date;
}main_var;

void SET_MENU_FLAG();

void CLR_MENU_FLAG();

#endif /* INC_MAIN_H_ */
