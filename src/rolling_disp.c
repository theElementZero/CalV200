/*
 * rolling_disp.c
 *
 *  Created on: 24-Jan-2021
 *      Author: ABC
 */


#include "inc/projectopts.h"

#include "fonts/Times_New_Roman42x44.h"
#if EXTRA_FONT
#include "fonts/Times_New_Roman29x31.h"
#include "fonts/Times_New_Roman34x36.h"
#include "fonts/Times_New_Roman27x28.h"
#include "fonts/Times_New_Roman40x41.h"
#include "fonts/Times_New_Roman25x26.h"
#include "fonts/Times_New_Roman26x28.h"
#endif
#include "fonts/Times_New_Roman19x20.h"

uint16_t glcd_flipped;

static rolling_disp var_rolling_disp;

void select_font(uint16_t key)
{
    switch (key) {
        case font_Times_New_Roman42x44:
            glcd_set_font(Times_New_Roman42x44, 42, 44, 32, 127);
            break;
#if EXTRA_FONT
        case font_Times_New_Roman34x36:
            glcd_set_font(Times_New_Roman34x36, 34, 36, 32, 127);
            break;
        case font_Times_New_Roman29x31:
            glcd_set_font(Times_New_Roman29x31, 29, 31, 32, 127);
            break;
        case font_Times_New_Roman27x28:
            glcd_set_font(Times_New_Roman27x28, 27, 38, 32, 127);
            break;
        case font_Times_New_Roman40x41:
            glcd_set_font(Times_New_Roman40x41, 40, 41, 32, 127);
            break;
        case font_Times_New_Roman25x26:
            glcd_set_font(Times_New_Roman25x26, 25, 26, 32, 127);
            break;
        case font_Times_New_Roman26x28:
            glcd_set_font(Times_New_Roman26x28, 26, 28, 32, 127);
            break;
#endif
        case font_Times_New_Roman19x20:
            glcd_set_font(Times_New_Roman19x20, 19, 20, 32, 127);
            break;
    }
}

void init_rolling_disp()
{
	glcd_select_screen(glcd_buffer, &glcd_bbox);
	glcd_flipped = 0;

	select_font(font_Times_New_Roman42x44);

	var_rolling_disp.enable = 1;
	var_rolling_disp.state = rolling_disp_show;

	memcpy(var_rolling_disp.lcd_live_data , __TIME__ , strlen(__TIME__) );

	var_rolling_disp.size = 16;
}


void set_xy(uint16_t x, uint16_t y)
{
    var_rolling_disp.x = x;
    var_rolling_disp.y = y;
}

void write_directly_disp(char * data)
{
    glcd_clear_buffer();
    glcd_draw_string_xy(  var_rolling_disp.x , var_rolling_disp.y , data);
    glcd_write();
}


void append_directly_disp(char * data)
{
    glcd_draw_string_xy(  var_rolling_disp.x , var_rolling_disp.y , data);
    glcd_write();
}

void update_rolling_display(uint16_t index, char *data, uint16_t size, uint16_t append)
{
	var_rolling_disp.state = rolling_disp_show;
	var_rolling_disp.size = append ? (var_rolling_disp.size ) : size;
	memcpy(var_rolling_disp.lcd_live_data + index, data, size);
}

void clr_rolling_display()
{
	memset(var_rolling_disp.lcd_live_data, 0, MAX_LCD_DATA_SIZE);
}

void update_rolling_disp_state(uint16_t state)
{
    if (var_rolling_disp.enable && BIT0) {
        var_rolling_disp.state = state;
    }
}

void rolling_display()
{
	char tmp = 0;

	switch(var_rolling_disp.state){
	case rolling_disp_idle:

		break;
	case rolling_disp_show:
		glcd_clear_buffer();
		glcd_draw_string_xy(var_rolling_disp.x , var_rolling_disp.y ,var_rolling_disp.lcd_live_data);
		glcd_write();
		var_rolling_disp.state = rolling_disp_idle; // change state
		break;
	case rolling_disp_change:
		tmp = var_rolling_disp.lcd_live_data[0];
		memcpy(var_rolling_disp.lcd_live_data, var_rolling_disp.lcd_live_data + 1, var_rolling_disp.size);
		var_rolling_disp.lcd_live_data[var_rolling_disp.size] = tmp;
		// change state
		var_rolling_disp.state = rolling_disp_show;
		break;
	case rolling_disp_update:
		var_rolling_disp.state = rolling_disp_show; // change state
		break;
	}
}

void enable_rolling_display(uint16_t state)
{
    if(state) var_rolling_disp.enable |= BIT0; else var_rolling_disp.enable &= ~BIT0;
}

void clear_screen()
{
    glcd_clear_buffer();
    glcd_write();
}
