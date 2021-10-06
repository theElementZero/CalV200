/*
 * menu.c
 *
 *  Created on: 24-Aug-2021
 *      Author: Sushant Naik , india(+91-9930554126)
 *
 *
 *
 */


#include <msp430.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "inc/projectopts.h"


enum key_code{
    menu = 0x40,
    down = 0x80,
    enter = 0x8,
};

typedef struct __menu_setting
{
    uint16_t offset;
    int16_t *number;
    int16_t max;
    int16_t r1_item;
    uint16_t enable;
    uint16_t modify;
    uint16_t blink;
    uint16_t show_digit;
}menu_setting;

typedef struct __time_setting
{
    int16_t GMT_OFF;            // OFF set according GMT
    int16_t summer_time;
    int16_t location;
    int16_t candle;
}time_setting;

const char *msg_r1[] =
                   {
                    "GMT Offset:",
                    "Summer     ",
                    "location   ",
                    "Jerusalem  "
                   };


const char *msg_r2[] = {
                     "no ",
                     "yes"
};


const int16_t time_off[] = {
                            -1200,
                            -1100,
                            -1000,
                            -900,
                            -800,
                            -700,
                            -600,
                            -500,
                            -430,
                            -400,
                            -330,
                            -300,
                            -200,
                            -100,
                            0,
                            100,
                            200,
                            300,
                            330,
                            400,
                            430,
                            500,
                            530,
                            545,
                            600,
                            630,
                            700,
                            800,
                            845,
                            900,
                            930,
                            1000,
                            1030,
                            1100,
                            1130,
                            1200,
                            1245,
                            1300,
                            1400
};
time_setting time_k;

menu_setting menu_items;

int16_t *reg_items[] = {
                        &time_k.GMT_OFF,
                        &time_k.summer_time,
                        &time_k.location,
                        &time_k.candle,
};

const int16_t reg_max[] = {
                           39,
                           1,
                           1,
                           1
};


typedef void (* menu_callback) (void );

static menu_callback main_page_cb[3] = {0};
//static menu_callback nxt_page_cb[4] = {0};
//static menu_callback n_page_cb[4] = {0};

static char menu_display[48];

void page1();
void page2();
void page3();
void page4();
void page0();

void page1_modify();
void page2_modify();
void page3_modify();
void page4_modify();

void page1_save();
void page2_save();
void page3_save();
void page4_save();

void dec_reg();

void menu_selection(uint16_t key)
{
    switch(key)
    {
    case menu:  // in/ out of menu

        menu_items.enable ^= 1;
        if (menu_items.enable )
            {
                SET_MENU_FLAG();
                select_font(font_Times_New_Roman19x20);

            }else{
                CLR_MENU_FLAG();
                select_font(font_Times_New_Roman42x44);
                menu_items.modify = 0;
            }
        (*main_page_cb[0])();
        break;
    case down: // list menu
        if(menu_items.enable){
            (*main_page_cb[1])();
        }
        break;
    case enter: // edit item
        if(menu_items.enable){
            dec_reg();
            menu_items.blink = 1;
        }
        break;
    }
}

void init_menu()
{
    menu_items.offset = 1;
    menu_items.max = 0;
    menu_items.r1_item = 0;
    menu_items.number = 0;

    time_k.GMT_OFF = 21;
    main_page_cb[0] = page1;
    main_page_cb[1] = page0;
    main_page_cb[2] = page0;
//    main_page_cb[3] = page4;
}


void page0(){}

void page1()
{
    float tmp = 0.0;
    /* Display page1 info
     * */
    strcpy(menu_display, msg_r1[0]);
    tmp =(float)time_off[ time_k.GMT_OFF];
    tmp /= 100.0;

    set_xy(0, 0);
    write_directly_disp(&menu_display[0]);
    sprintf(menu_display , "%.2f",  tmp);
    set_xy(0, 21);
    append_directly_disp(&menu_display[0]);

    main_page_cb[1] = page2;
    main_page_cb[2] = page1_modify;

    menu_items.number = reg_items[0];
    menu_items.show_digit = 1;
    menu_items.max = reg_max[0];
    menu_items.r1_item = 0;
    menu_items.offset = 30;
    menu_items.modify = 1;
    menu_items.blink = 1;
}


void page2()
{
    /* Display page2 info
     * */
    strcpy(menu_display, msg_r1[1]);
    set_xy(0, 0);
    write_directly_disp(&menu_display[0]);

    strcpy(menu_display, msg_r2[0] );
    set_xy(0, 21);
    append_directly_disp(&menu_display[0]);

    main_page_cb[1] = page3;
    main_page_cb[2] = page2_modify;

    menu_items.number = reg_items[1];
    menu_items.show_digit = 0;
    menu_items.max = reg_max[1];
    menu_items.r1_item = 1;
    menu_items.modify = 1;
    menu_items.blink = 1;
}

void page3()
{
    /* Display page3 info
     * */
    strcpy(menu_display, msg_r1[2]);
    set_xy(0, 0);
    write_directly_disp(&menu_display[0]);

    strcpy(menu_display, msg_r2[0] );
    set_xy(0, 21);
    append_directly_disp(&menu_display[0]);

    main_page_cb[1] = page4;
    main_page_cb[2] = page3_modify;

    menu_items.number = reg_items[2];
    menu_items.show_digit = 0;
    menu_items.max = reg_max[2];
    menu_items.r1_item = 2;
    menu_items.modify = 1;
    menu_items.blink = 1;
}

void page4()
{
    /* Display page4 info
     * */
    strcpy(menu_display, msg_r1[3]);
    set_xy(0, 0);
    write_directly_disp(&menu_display[0]);

    strcpy(menu_display, msg_r2[0] );
    set_xy(0, 21);
    append_directly_disp(&menu_display[0]);

    main_page_cb[1] = page1;
    main_page_cb[2] = page4_modify;

    menu_items.number = reg_items[3];
    menu_items.show_digit = 0;
    menu_items.max = reg_max[3];
    menu_items.r1_item = 3;
    menu_items.modify = 1;
    menu_items.blink = 1;
}


void page1_modify()
{
    menu_items.modify = 1;
#if 0
    main_page_cb[1] = dec_reg;
#endif
    main_page_cb[2] = dec_reg;

}

void page2_modify()
{
    menu_items.modify = 1;
#if 0
    main_page_cb[1] = dec_reg;
#endif
    main_page_cb[2] = dec_reg;


}

void page3_modify()
{
    menu_items.modify = 1;
#if 0
    main_page_cb[1] = dec_reg;
#endif
    main_page_cb[2] = dec_reg;

}

void page4_modify()
{
    menu_items.modify = 1;
#if 0
    main_page_cb[1] = dec_reg;
#endif
    main_page_cb[2] = dec_reg;

}

void page1_save()
{
    menu_items.modify = 0;
    main_page_cb[1] = page2;
    main_page_cb[2] = page1_modify;
    menu_items.offset = 1;
}

void page2_save()
{
    menu_items.modify = 0;
    main_page_cb[1] = page3;
    main_page_cb[2] = page2_modify;
}

void page3_save()
{
    menu_items.modify = 0;
    main_page_cb[1] = page4;
    main_page_cb[2] = page3_modify;
}

void page4_save()
{
    menu_items.modify = 0;
    main_page_cb[1] = page1;
    main_page_cb[2] = page4_modify;
}


void dec_reg()
{
    float tmp = 0.0;
    if(menu_items.modify ){
        ++(*menu_items.number);
        if(*menu_items.number > menu_items.max)
        {
            *menu_items.number = 0;
        }
    }
    set_xy(0, 0);
    write_directly_disp( (char *) msg_r1[menu_items.r1_item]);

    if(menu_items.show_digit){
        tmp =(float) time_off[*menu_items.number];
        tmp /= 100.0;

        sprintf(menu_display , "%.2f",  tmp);
    }else {
        strcpy(menu_display, msg_r2[*menu_items.number] );
    }

    set_xy(0, 21);
    append_directly_disp(&menu_display[0]);
}

void blinker(){
    float tmp = 0.0;

    if(menu_items.modify){

        menu_items.blink ^= 1;

        if(menu_items.blink){
            clear_screen();
        }else{

            set_xy(0, 0);
            write_directly_disp( (char *) msg_r1[menu_items.r1_item]);

            if(menu_items.show_digit){
                tmp =(float) time_off[*menu_items.number];
                tmp /= 100.0;

                sprintf(menu_display , "%.2f",  tmp);
            }else {
                strcpy(menu_display, msg_r2[*menu_items.number] );
            }

            set_xy(0, 21);
            append_directly_disp(&menu_display[0]);
        }
    }
}


int16_t is_summer_time()
{
    return time_k.summer_time;
}

int16_t get_GMT_offset_in_minutes()
{
    int hour = time_off[time_k.GMT_OFF];

    int minutes = hour % 100;
    hour /= 100;

    minutes += hour * 60;
    return minutes;
}
