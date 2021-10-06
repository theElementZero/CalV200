/*
 * calender_holiday.c
 *
 *  Created on: 05-Sep-2021
 *      Author: ABC
 */
#include <stdio.h>
#include "string.h"
#include <stdlib.h>
#include "msp430.h"

#include "inc/projectopts.h"


const char *holiday_msg[] = {
                        ", Candle hh:mm",               /*0*/
                        ", Yaaleh Veyavo",              /*1*/
                        ", Shana Tova",                 /*2*/
                        ", Tishrei ",
                        ", Hal-Nissim",                 /*4*/
                        ", Hayom nn Baomer",            /*5*/
                        ", Nisan ",                     /*6*/
                        ", Sivan ",                     /*7*/
                        "- Candle ",               /*8*/
                        " BaOmer"
};

void fndms(unsigned int *time, char *arr);

void is_it_friday(int _true, char *message , unsigned int *time)
{
    if(_true)
    {
        strcat(message, holiday_msg[8] );

        fndms(time, message + strlen(message));
    }
}


void get_hebrew_holiday(CALENDER_N hebrew_date, char *message, unsigned int *time)
{
    int day_count = 0;
    int i = 0;

    /* check if its friday */


    switch(hebrew_date.month)
    {
    /*
     * 1-2 Tishrei  2 days
                    Message 1 : Shana Tova
                    Message 2 : Day before 1 - Candle #
                    Message 3 : On 1 Tishrei Candle  #
     *
     * */
    case 6:
        switch(hebrew_date.day)
        {
        case 9:
        case 14:

        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
            strcat(message, holiday_msg[1]);
            strcat(message, holiday_msg[0]);
            break;
        }
        break;
     case 8:
        day_count = hebrew_date.day - 25;

        if(day_count > 0)
        {
            for(i = 0; i < day_count ; i++)
            {
                strcat(message, "i");
            }
        }

        strcat(message, holiday_msg[1]);
        strcat(message, holiday_msg[4]);
        break;
     case 9:
         if(hebrew_date.day < 4)
         {
             day_count = hebrew_date.day + 5;
             for(i = 0; i < day_count ; i++)
             {
                 strcat(message, "i");
             }
         }

         strcat(message, holiday_msg[1]);
         strcat(message, holiday_msg[4]);
         break;
         /*
          * 15-21 Nisan
          * Message 1:On 14 Nisan - Candle  #
            Message 2:On 20 Nisan - Candle  #
            Message 3 :During all 8 days –
            “Yaaleh Veyavo”
          *
          * */
     case 0:
         switch(hebrew_date.day)
         {
         case 14:
             break;
         case 15:
         case 16:
         case 17:
         case 18:
         case 19:
         case 20:
             strcat(message, holiday_msg[6]);
             strcat(message, holiday_msg[8] );

             fndms(time, message + strlen(message));
         case 21:
             strcat(message, holiday_msg[1]);
             break;


         }

         if(hebrew_date.day > 14)
         {
             day_count = hebrew_date.day - 14;

             sprintf(message,", %d" , day_count);
             strcat(message, holiday_msg[9]);
         }
         break;
     case 1:
         day_count = 15 + hebrew_date.day;
         sprintf(message,", %d" , day_count);
         strcat(message, holiday_msg[9]);

         break;
     case 2:
        day_count = 44 + hebrew_date.day;
        sprintf(message,", %d" , day_count);
        strcat(message, holiday_msg[9]);
        strcat(message, holiday_msg[1]);
         break;


    }
}


