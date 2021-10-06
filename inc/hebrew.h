/*
 * hebrew.h
 *
 *  Created on: Mar 15, 2020
 *      Author: sushant21
 */

#ifndef HEBREW_H_
#define HEBREW_H_



#define CAL_DEBUG1 0


#define       GREGORIAN_EPOCH 1721425.5



typedef struct __HEBREW_DATA
{
    float EPOCH;
    char *MONTHS[14];
}HEBREW_DATA;

typedef struct __CALENDER
{
    int year;
    int month;
    int day;
}CALENDER_N;

CALENDER_N  jdToHebrew (long int jd) ;

long int gregorian_calendar_to_jd(int y, int m, int d);
void displayHebrew(CALENDER_N *hebrew, char *arr);

#endif /* HEBREW_H_ */
