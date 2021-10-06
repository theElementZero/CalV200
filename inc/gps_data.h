/*
 * gps_data.h
 *
 *  Created on: 26-Jan-2021
 *      Author: ABC
 */

#ifndef GPS_DATA_H_
#define GPS_DATA_H_

#include <stdint.h>

typedef struct ST_DATE_TAG
{
	int day;
	int mon;
	int yr;
}ST_DATE;

typedef struct __gps_val
{
	float longitude;
	float latitude;
}gps_val;

extern gps_val gps_reading;
extern  ST_DATE stdate;

uint16_t decryptData(char *data);
void fndms(unsigned int *time, char *arr);

#endif /* GPS_DATA_H_ */
