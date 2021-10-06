/*
 * gps_data.c
 *
 *  Created on: 26-Jan-2021
 *      Author: ABC
 */

#include <msp430.h>
#include "inc/projectopts.h"

ST_DATE stdate = {
		30,11,20
};

ST_DATE hebrewdate;

gps_val gps_reading;


void fndms(unsigned int *time, char *arr)
{
	//char ASCII[] = {'0','1','2','3'}
	int hour = 0, minute = 0;
	//int __time_in_minutes = *time;

	hour = *time / 60;

	minute = *time % 60;
	/*
	minute = *time - (hour*60);

	minute *= 60;
	minute /= 100;
*/
	arr[0] = (hour / 10) + 0x30;
	arr[1] = (hour %10) + 0x30;

	arr[3] = (minute / 10) + 0x30;
	arr[4] = (minute %10) + 0x30;
}

inline char* findToken(char *inBuf)
{
    unsigned int cnt1;

    for(cnt1 =0; ;cnt1++)
    {
        if((inBuf[cnt1] == ',') || (inBuf[cnt1] == 0))
            break;
    }
    cnt1++;
    return &inBuf[cnt1];
}

float strFloat(char *inData, int longitude)
{
	int afterDeci, beforeDeci;
	char index;
	int *ptrData;
	int tenth,pos, sizeData;
	float data;
	if(longitude)
	{
		tenth = 10000;
		sizeData = 10;
	}
	else
	{
		tenth = 1000;
		sizeData = 9;
	}

	beforeDeci = 0;
	afterDeci = 0;
	ptrData = &beforeDeci;
	for(index = 0; index < sizeData; index++)
	{
		if((inData[index] < 0x3A) && (inData[index] > 0x2f))
		{
			pos = inData[index] - 0x30;
			pos *= tenth;
			*ptrData += pos;
			tenth /= 10;
		}

		// decimal point
		if(inData[index] == 0x2e)
		{

			tenth = 1000;
			ptrData = &afterDeci;
		}
	}

	data = afterDeci;
	data /= 10000;
	data += beforeDeci;
	return data;
}

int getDate(char *inData)
{
	//int afterDeci, beforeDeci;
	char index;
	int *ptrData;
	int tenth,pos, sizeData;

	ptrData = &stdate.day;
	tenth = 10;
	sizeData = 1;
	*ptrData = 0;
	for(index = 0; index < 6; index++)
	{
		if((inData[index] < 0x3A) && (inData[index] > 0x2f))
		{
			pos = inData[index] - 0x30;
			pos *= tenth;
			*ptrData += pos;
			tenth /= 10;
		}
		if(index == sizeData)
		{
			sizeData += 2;
			tenth = 10;
			ptrData += 1;
			*ptrData = 0;
		}
	}
	sizeData = 1;
	//validate date
	if((stdate.day > 31) || (stdate.mon > 12) || (stdate.yr > 99))
	{
		sizeData = 0;
	}
	return sizeData;
}

//  					lat			long					date
// $GPRMC,104749.000,A,3146.1846,N,03513.9256,E,0.16,48.45,230516,,,A*5D<CR><LF>
uint16_t decryptData(char *data)
{
	uint16_t ret_val = 1;
	char *ret1, *ret2;
//	if(rxComplete)
//	{
		/* get the first token */
		//ret1 = strstr(data, ",");
		ret1 = findToken(data);
		if(ret1 != NULL)
		{
		    // read time from GPS

			ret2 = findToken(ret1);
			if(ret2 != NULL)
			{
//				ret2 += 1;
				if(*ret2 == 'A') // valid GPS data
				{
//					if(dataLatch == 0)
//					{
						ret2 += 2;
						gps_reading.latitude = (float)strFloat(ret2, 0);
						gps_reading.latitude /= 100;
						ret1 = findToken(ret2);
						//ret1 = strstr(ret2 , ",");
						ret2 = findToken(ret1);
						//ret2 = strstr(ret1 + 1, ","); // skip N/S
//						longitude = (float)atof(ret2);
						gps_reading.longitude = strFloat(ret2, 1);
						gps_reading.longitude /= 100;

						ret1 = findToken(ret2 );
						//ret1 = strstr(ret2 + 1 , ",");// skip E/W
						//ret1 += 1;
						ret2 = findToken(ret1);
						//ret2 = strstr(ret1, ","); // skip num1
						//ret2 += 1;
						//ret1 = strstr(ret2  , ","); // skip num2
//						ret1 += 1;
						ret1 = findToken(ret2);
						//strcpy(temparr, ret1);
						//ret2 = strstr(ret1, ","); // datE
//						ret2 += 1;
						ret2 = findToken(ret1);
						//strcpy(temparr, ret2);
						//temparr[6] = 0;
						if(getDate(ret2)){
							ret_val = 0;
						}
//						memset(data,0, 128);
//					}

				}		else{
					ret_val = 3;
				}
			}		else{
				ret_val = 2;
			}
		}
		else{
			ret_val = 1;
		}
//		rxComplete = 0;
//	}
	return ret_val;
}



