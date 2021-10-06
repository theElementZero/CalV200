/*
 * lw_utils.c
 *
 *  Created on: 05-Sep-2021
 *      Author: ABC
 */


#include <msp430.h>
#include <stdint.h>
#include <string.h>


void set_bit_mask(uint16_t *reg_val, uint16_t mask)
{
   *reg_val |= mask;
}

void clr_bit_mask(uint16_t *reg_val, uint16_t mask)
{
   *reg_val &= ~mask;
}

uint16_t get_bit_status(uint16_t *reg_val, uint16_t mask)
{
   return ((*reg_val) & mask) ? 1 : 0;
}

