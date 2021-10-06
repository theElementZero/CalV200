
/*
 * algo.c
 *
 *  Created on: Feb 14, 2018
 *      Author: Sushant
 */

#include "math.h"
#include <stdio.h>
#include "string.h"
#include <stdlib.h>
#include "inc/gps_data.h"

#define MATH_PI 3.141

float timeRise;

static const unsigned int days[13] = {
        0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
    };

unsigned int checkYear(unsigned int year)
{
// Return true if year is a multiple
// 0f 4 and not multiple of 100.
// OR year is multiple of 400.

    // If a year is multiple of 400,
    // then it is a leap year
    if (year % 400 == 0)
        return 1;

    // Else If a year is muliplt of 100,
    // then it is not a leap year
    if (year % 100 == 0)
        return 0;

    // Else If a year is muliplt of 4,
    // then it is a leap year
    if (year % 4 == 0)
        return 1;
    return 0;
//return (((year % 4 == 0) && (year % 100 != 0)) ||
//        (year % 400 == 0));
}

int dayofweek(ST_DATE *date)
{
    static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
    date->yr -= date->mon < 3;
    return ( date->yr + date->yr/4 - date->yr/100 + date->yr/400 + t[date->mon-1] + date->day) % 7;
}

unsigned int get_yday(ST_DATE *date)
{
    int leap;


	leap = checkYear(date->yr);

    return days[date->mon] + date->day + leap;
}

//
//unsigned int computeSunrise(unsigned int sunrise) {
//
//    /*Sunrise/Sunset Algorithm taken from
//        http://williams.best.vwh.net/sunrise_sunset_algorithm.htm
//        inputs:
//            day = day of the year
//            sunrise = true for sunrise, false for sunset
//        output:
//            time of sunrise/sunset in hours */
//
//    //lat, lon for Berlin, Germany
////    float longitude = 13.408056;
////    float latitude = 52.518611;
//#define zenith  90.83333333333333
//
//#define D2R  (MATH_PI / 180)
//#define R2D ( 180 / MATH_PI)
//
//    // convert the longitude to hour value and calculate an approximate time
//    float lnHour = longitude / 15;
//    float t, M, L, RA;
//    unsigned int day;
//    day = get_yday(&stdate);
//
//    if (sunrise) {
//        t = day + ((6 - lnHour) / 24);
//    } else {
//        t = day + ((18 - lnHour) / 24);
//    };
//
//    //calculate the Sun's mean anomaly
//    M = (0.9856 * t) - 3.289;
//
//    //calculate the Sun's true longitude
//    L = M + (1.916 * sin(M * D2R)) + (0.020 * sin(2 * M * D2R)) + 282.634;
//    if (L > 360) {
//        L = L - 360;
//    } else if (L < 0) {
//        L = L + 360;
//    };
//
//    //calculate the Sun's right ascension
//    RA = R2D * asin(0.91764 * (sin(L * D2R)/cos(L * D2R)));
//    RA = RA / acos(0.91764 * (sin(L * D2R)/cos(L * D2R)));
//    if (RA > 360) {
//        RA = RA - 360;
//    } else if (RA < 0) {
//        RA = RA + 360;
//    };
//
//    //right ascension value needs to be in the same qua
//    M = (floor(L / (90))) * 90;  // Lquadrant
//    lnHour = (floor(RA / 90)) * 90;  // RAquadrant
//    RA = RA + (M  - lnHour);
//
//    //right ascension value needs to be converted into hours
//    RA = RA / 15;
//
//    //calculate the Sun's declination
//    M = 0.39782 * sin(L * D2R);
//    lnHour = cos(asin(M));
//
//    //calculate the Sun's local hour angle
//    M = (cos(zenith * D2R) - (M * sin(latitude * D2R))) / (lnHour * cos(latitude * D2R));
//    //var H;
//    if (sunrise) {
//    	lnHour = 360 - R2D * acos(M);
//    } else {
//    	lnHour = R2D * acos(M);
//    }
//    lnHour = lnHour / 15;
//
//    //calculate local mean time of rising/setting
//    M = lnHour + RA - (0.06571 * t) - 6.622;
//
//
//    lnHour = longitude / 15; // added by sushant reducig overload
//    //adjust back to UTC
//    RA = M - lnHour;
//    if (RA > 24) {
//        RA = RA - 24;
//    } else if (RA < 0) {
//        RA = RA + 24;
//    }
//
//    //convert UT value to local time zone of latitude/longitude
//    lnHour = RA + 1;
//
//    //convert to Milliseconds
//    return (lnHour * 3600 * 1000);
//}

float computeSunrise( unsigned int sunrise) {

/*Sunrise/Sunset Algorithm taken from
        http://williams.best.vwh.net/sunrise_sunset_algorithm.htm
        inputs:
            day = day of the year
            sunrise = true for sunrise, false for sunset
        output:
            time of sunrise/sunset in hours */

    //lat, lon for Berlin, Germany
//    float longitude = 13.408056;
//    float latitude = 52.518611;
    float zenith = 90.83333333333333;
    float D2R = 3.141/ 180;
    float R2D = 180 / 3.141;
    float M, L;
    unsigned int day = get_yday(&stdate);

    // convert the longitude to hour value and calculate an approximate time
    float lnHour = gps_reading.longitude / 15;
    float t;
    if (sunrise) {
        t = day + ((6 - lnHour) / 24);
    } else {
        t = day + ((18 - lnHour) / 24);
    };

    //calculate the Sun's mean anomaly
    M = (0.9856 * t) - 3.289;

    //calculate the Sun's true longitude
    L = M + (1.916 * sin(M * D2R)) + (0.020 * sin(2 * M * D2R)) + 282.634;
    if (L > 360) {
        L = L - 360;
    } else if (L < 0) {
        L = L + 360;
    };

    float RA;
    //calculate the Sun's right ascension
    RA = R2D * (atan(0.91764 * (sin(L * D2R)/cos(L * D2R)))); /**<  / acos(0.91764 * (sin(L * D2R)/cos(L * D2R)))); */
    if (RA > 360) {
        RA = RA - 360;
    } else if (RA < 0) {
        RA = RA + 360;
    };

    float Lquadrant, RAquadrant;
    //right ascension value needs to be in the same qua
    Lquadrant = (floor(L / (90))) * 90;
    RAquadrant = (floor(RA / 90)) * 90;
    RA = RA + (Lquadrant - RAquadrant);

    //right ascension value needs to be converted into hours
    RA = RA / 15;

    float sinDec, cosDec;
    //calculate the Sun's declination
    sinDec = 0.39782 * sin(L * D2R);
    cosDec = cos(asin(sinDec));

    float cosH;
    //calculate the Sun's local hour angle
    cosH = (cos(zenith * D2R) - (sinDec * sin(gps_reading.latitude * D2R))) / (cosDec * cos(gps_reading.latitude * D2R));
    float H;
    if (sunrise) {
        H = 360 - R2D * acos(cosH);
    } else {
        H = R2D * acos(cosH);
    }
    H = H / 15;

    float T,UT;
   // localT = 0;
    //calculate local mean time of rising/setting
    T = H + RA - (0.06571 * t) - 6.622;

    //adjust back to UTC
    UT = T - lnHour;
    if (UT > 24) {
        UT = UT - 24;
    } else if (UT < 0) {
        UT = UT + 24;
    }

    //convert UT value to local time zone of latitude/longitude
   // localT = UT + 1;

    //convert to Minutes
    return UT * 60;
}
