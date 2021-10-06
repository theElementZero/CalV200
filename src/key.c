/*
 * key.c
 *
 *  Created on: 24-Aug-2021
 *      Author: Sushant Naik, India (+91-993055426)
 */


#include <msp430.h>
#include <stdint.h>
#include <string.h>

#include "inc/key.h"

enum key_val{
    key_menu = 1,
    key_up = 2,
    key_dn = 4,
    key_ent = 8,
    key_tst = 16
};

static uint16_t last_val = 0;           // 0 means no buttons pushed
static uint16_t count = 0;              // debounce filter count


static uint16_t read_keypad()
{
    return (P4IN & 0xC8 );
}

uint16_t filter_keypad(void)
{
  uint16_t val;                           // raw val this time
  uint16_t filtered_val = 0;              // our filtered return val

  val = read_keypad();              // get raw keypad value

  if (val != 0)                     // have a keypad button push
  {
    if (val == last_val)            // we're filtering this val
    {
      if ((count != 0) && (--count == 0))  // continue filtering & check if done
      {
        filtered_val = val;         // found enough consecutive values to return as valid
      }
    }
    else
    {
      count = FILTER_COUNT;         // start filtering a new val
    }
  }

  last_val = val;
  return filtered_val;
}



