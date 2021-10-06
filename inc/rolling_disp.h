/*
 * rolling_disp.h
 *
 *  Created on: 24-Jan-2021
 *      Author: ABC
 */

#ifndef ROLLING_DISP_H_
#define ROLLING_DISP_H_


#define MAX_LCD_DATA_SIZE 64

typedef struct __rolling_disp
{
    uint8_t x;
    uint8_t y;
    uint16_t enable;
	uint16_t state;
	uint16_t size;
	char lcd_live_data[MAX_LCD_DATA_SIZE];
}rolling_disp;

enum rolling_disp_event{
	rolling_disp_idle,
	rolling_disp_show,
	rolling_disp_change,
	rolling_disp_update
};


enum font_select{
    font_Times_New_Roman42x44,
    font_Times_New_Roman34x36,
    font_Times_New_Roman29x31,
    font_Times_New_Roman27x28,
    font_Times_New_Roman40x41,
    font_Times_New_Roman26x28,
    font_Times_New_Roman25x26,
    font_Times_New_Roman19x20
};


void init_rolling_disp();

void update_rolling_disp_state(uint16_t state);

void update_rolling_display(uint16_t index, char *data, uint16_t size, uint16_t append);
void rolling_display();
void clr_rolling_display();
void set_xy(uint16_t x, uint16_t y);
void select_font(uint16_t key);
void write_directly_disp(char * data);
void append_directly_disp(char * data);
void clear_screen();

void enable_rolling_display(uint16_t state);


#endif /* ROLLING_DISP_H_ */
