/*
 * hebrew.c
 *
 *  Created on: Mar 15, 2020
 *      Author: sushant21
 *      Source: http://dzucconi.github.io/calendrical/docs/calendrical.calendar.conversions.html
 */


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "inc/hebrew.h"

#include <msp430f5438.h>


const HEBREW_DATA hebrew ={
      .EPOCH = 347995.5 ,
      .MONTHS =  {
           /* 0         1       2                   4       5   */
           "Nisan" , "Iyyar" , "Sivan" , "Tammuz" , "Av" , "Elul" ,
           /*6          7               8                       10          11      12      */
           "Tishri" , "Marẖeshvan" , "Kislev" , "Teveth" , "Shevat" , "Adar" , "Veadar" , ""}
    };


const hebrew_month_days[14] = {
/*                          0       2       4       6       8       10      12*/
                             30, 29, 30, 29, 30, 29, 30, 29, 30, 29, 30, 29, 30, 29,
};
int leapGregorian (year) {
    return ((year % 4) == 0) &&
      (!(((year % 100) == 0) && ((year % 400) != 0)));
  }
long int gregorian_calendar_to_jd(int yr, int m, int d)
    {
	long int jd_num = 0;
	long int y = yr;
		y+=8000;
		if(m<3) { y--; m+=12; }

		jd_num = (y * 365);
		jd_num += (y/4);
		jd_num -= (y/100);
		jd_num += (y/400);
		jd_num -= 1200820;
		jd_num += (m*153+3)/5;
		jd_num += -92;
		jd_num += d;
		jd_num += -1;
		return jd_num;
//	return (long int)((y*365) +(y/4) -(y/100) +(y/400) -1200820
//              +(m*153+3)/5-92
//              +d-1)
//	;
    }
long int gregorianToJd(int year, int month,int day)
{
    long int time_1;
    float lfdata1;
    int lidatay, lidatam;
//    struct tm timeinfo;
    // time_t is arithmetic time type
//	time_t now;

	// Obtain current time
	// time() returns the current time of the system as a time_t value
//	time(&now);
/*
	struct tm epoch_time;

    memcpy(&epoch_time, localtime(&now), sizeof (struct tm));

    year = epoch_time.tm_year + 1900;
    month = epoch_time.tm_mon + 1;
    day = epoch_time.tm_mday;
*/
	#if CAL_DEBUG1
	printf("[gregoriantoJD]EPOCH %ld\r\n", now);
	#endif // CAL_DEBUG1

	time_1 = GREGORIAN_EPOCH - 1;
	lidatay = year - 1;
	//lidatay *= 365;

	time_1 += (lidatay * 365);
	lfdata1 = (float)lidatay / 4 ;
	time_1 += (long int)lfdata1;
	lfdata1 = (float)lidatay / 100;
	time_1 += -(long int)lfdata1;
	lfdata1 = (float)lidatay / 400;
	time_1 += (long int)lfdata1;
	lidatam = month;
	lidatam *= 367;
	lfdata1 = lidatam - 362;
	lfdata1 /= 12;
	//time_1 += (long int)lfdata1;
	lidatam = (month <= 2) ? 0 : (leapGregorian(year) ? -1 : -2);

	lidatam += day;
	lfdata1 += lidatam;
	time_1 += (long int)lfdata1;

//	printf("{gregtojd}JD %ld\r\n", time_1);
	return (int)time_1;
}

long int astro_mod(long int a, long int b) {
	long int b_temp;
	b_temp = a;
	b_temp /= b;
	b_temp *= b;
	b = a - b_temp;
	return b;
//    return a - (b * (long int)(a / b));
  }

int jdToGregorianYear (jd) {
    int wjd, depoch, quadricent, dqc, cent, dcent, quad, dquad,
      yindex, year;
    time_t now;

	// Obtain current time
	// time() returns the current time of the system as a time_t value
	time(&now);
    wjd        = (int)(jd - 0.5) + 0.5;
    depoch     = wjd - GREGORIAN_EPOCH;
    quadricent = (int)(depoch / 146097);
    dqc        = astro_mod(depoch, 146097);
    cent       = (int)(dqc / 36524);
    dcent      = astro_mod(dqc, 36524);
    quad       = (int)(dcent / 1461);
    dquad      = astro_mod(dcent, 1461);
    yindex     = (int)(dquad / 365);
    year       = (quadricent * 400) + (cent * 100) + (quad * 4) + yindex;

    if (!((cent == 4) || (yindex == 4))) { year++; }

    return year;
}

int hebrewLeap (int year) {
	long int temp = year;
	temp *= 7;
	temp += 1;
    return astro_mod(temp, 19) < 7;
  }

int
hebrewYearMonths (int year) {
    return hebrewLeap(year) ? 13 : 12;
  }


long  int
hebrewDelay1 (int year) {
    long int months, parts;
    volatile long int  day;

    months =  year;
    months *= 235;
    months -= 234;
    months /= 19;

    //months = (long int)(((235 * year) - 234) / 19);

    //parts  = 12084 + (13753 * months);
    parts = months;
    parts *= 13753;
    parts += 12084;

    //day    = (months * 29) + (long int)(parts / 25920);
    day = parts;
    day /= 25920;
    day += (months * 29);

    // temp use parts
    months = day;
    months += 1;
    months *= 3;
//    if (astro_mod((3 * (day + 1)), 7) < 3) {
    if (astro_mod(months, 7) < 3) {
      day++;
    }

    return day;
  }

//Check for delay in start of new year due to length of adjacent years

int
  hebrewDelay2 (int year) {
    long int last, present, next;

    last    = hebrewDelay1(year - 1);
    present = hebrewDelay1(year);
    next    = hebrewDelay1(year + 1);

    return ((next - present) == 356) ? 2 :
      (((present - last) == 382) ? 1 : 0);
  }
//¶
//How many days are in a Hebrew year?


  int hebrewYearDays (year) {
    return
            hebrewToJd(year + 1, 7, 1) - hebrewToJd(year, 7, 1);
  }

  int hebrewMonthDays (year, month) {
//First of all, dispose of fixed-length 29 day months


    if (month == 2 || month == 4 || month == 6 ||
      month == 10 || month == 13) {
      return 29;
    }
//If it's not a leap year, Adar has 29 days


    if (month == 12 && !hebrewLeap(year)) {
      return 29;
    }
//If it's Heshvan, days depend on length of year


    if (month == 8 && !(astro_mod(hebrewYearDays(year), 10) == 5)) {
      return 29;
    }
//Similarly, Kislev varies with the length of year


    if (month == 9 && (astro_mod(hebrewYearDays(year), 10) == 3)) {
      return 29;
    }

//Nope, it's a 30 day month


    return 30;
  }

long int
hebrewToJd (int year, int month, int day) {
    long int jd;
    int mon, months;
//    time_t now;

	// Obtain current time
	// time() returns the current time of the system as a time_t value
	//time(&now);

    months = hebrewYearMonths(year);

    jd = hebrew.EPOCH ;
    jd += hebrewDelay1(year);
    jd +=
      hebrewDelay2(year);
    jd += day;
    jd  += 1;

    if (month < 7) {
      for (mon = 7; mon <= months; mon++) {
        jd += hebrewMonthDays(year, mon);
      }
      for (mon = 1; mon < month; mon++) {
        jd += hebrewMonthDays(year, mon);
      }
    } else {
      for (mon = 7; mon < month; mon++) {
        jd += hebrewMonthDays(year, mon);
      }
    }

    return jd;
  }


CALENDER_N  jdToHebrew (long int jd) {
    long int count, count1, i;
    int year, month, day,  first;
    //time_t now;
    CALENDER_N temp;

	// Obtain current time
	// time() returns the current time of the system as a time_t value
	//time(&now);

    jd    = (long int)(jd + 0.5);

    count = (long int)(((jd - hebrew.EPOCH) * 98496.0) / 35975351.0);
    year  = count - 1;


    for (i = count; jd >= (long int)hebrewToJd(i, 7, 1); i++) {

      year++;
    }

    first = (jd < hebrewToJd(year, 1, 1)) ? 7 : 1;
    month = first;

    for (i = first; jd > hebrewToJd(year, i, hebrewMonthDays(year, i)); i++) {
      month++;
    }

    day = (jd - hebrewToJd(year, month, 1)) + 1;

     temp.day = day;
     temp.month = month;
     temp.year = year;

     return temp;
//    return [year, month, day];
  }



void displayHebrew(CALENDER_N *date, char *arr)
{
	arr[0] = (date->day / 10) + 0x30;
	arr[1] = (date->day %10) + 0x30;

	sprintf(arr + 2, "-%s-%d", hebrew.MONTHS[date->month - 1], date->year);
}


void
  updateHebrew (jd) {
    CALENDER_N temp = jdToHebrew(jd);



   // printf("Hebrew Date = %d, %s, %d\r\n", temp.year, hebrew.MONTHS[temp.month - 1], temp.day);
    //return data.hebrew;
  }


