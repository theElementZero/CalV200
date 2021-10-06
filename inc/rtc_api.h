/*
 * rtc_api.h
 *
 *  Created on: 30-Sep-2021
 *      Author: ABC
 */

#ifndef INC_RTC_API_H_
#define INC_RTC_API_H_



typedef struct __rtc_cfg
{
    uint16_t year;
    uint16_t month;
    uint16_t day;

    uint16_t dow;

    uint16_t hour;
    uint16_t minute;
    uint16_t seconds;
}rtc_cfg;


void init_RTC();

void start_RTC();

void stop_RTC();

void set_rtc(rtc_cfg rtc_in);

void get_rtc(rtc_cfg *rtc_in);

#endif /* INC_RTC_API_H_ */
