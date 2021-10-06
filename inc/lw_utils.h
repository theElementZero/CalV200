/*
 * lw_utils.h
 *
 *  Created on: 05-Sep-2021
 *      Author: ABC
 */

#ifndef INC_LW_UTILS_H_
#define INC_LW_UTILS_H_

void set_bit_mask(uint16_t *reg_val, uint16_t mask);

void clr_bit_mask(uint16_t *reg_val, uint16_t mask);

uint16_t get_bit_status(uint16_t *reg_val, uint16_t mask);

#endif /* INC_LW_UTILS_H_ */
