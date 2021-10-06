/*
 * menu.h
 *
 *  Created on: 30-Aug-2021
 *      Author: ABC
 */

#ifndef INC_MENU_H_
#define INC_MENU_H_

void init_menu();

void menu_selection(uint16_t key);

void blinker();

int16_t is_summer_time();

int16_t get_GMT_offset_in_minutes();

#endif /* INC_MENU_H_ */
