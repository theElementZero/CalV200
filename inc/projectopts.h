/*
 * projectopts.h
 *
 *  Created on: 18-Jan-2021
 *      Author: ABC
 */

#ifndef PROJECTOPTS_H_
#define PROJECTOPTS_H_

#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "main.h"

#define GLCD_DEVICE_MSP430

#define GLCD_CONTROLLER_ST7565R

#include "inc/glcd.h"
#include "inc/glcd_devices.h"
#include "inc/glcd_controllers.h"
#include "inc/glcd_graphics.h"
#include "inc/glcd_graphs.h"
#include "inc/glcd_text_tiny.h"
#include "inc/glcd_text.h"

#include "inc/lw_rngbuf.h"
#include "inc/lcd_api.h"
#include "inc/rolling_disp.h"
#include "uart_api.h"
#include "inc/gps_data.h"
#include "inc/algo.h"

#include "inc/key.h"
#include "inc/menu.h"
#include "inc/rtc_api.h"
#include "inc/lw_utils.h"

#include "inc/main.h"
#include "inc/hebrew.h"
#include "inc/calender_holiday.h"

#endif /* PROJECTOPTS_H_ */
