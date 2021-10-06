/*
 * lcd_api.h
 *
 *  Created on: 18-Jan-2021
 *      Author: ABC
 */

#ifndef LCD_API_H_
#define LCD_API_H_


#define LCD_RES_HIGH() P4OUT |= BIT4
#define LCD_SDA_HIGH() P1OUT |= BIT6
#define LCD_SCL_HIGH() P1OUT |= BIT7

#define LCD_RES_LOW() P4OUT &= ~BIT4
#define LCD_SDA_LOW() P1OUT &= ~BIT6
#define LCD_SCL_LOW() P1OUT &= ~BIT7

#define LCD_RES_OUT() P4DIR |= BIT4
#define LCD_SDA_OUT() P1DIR |= BIT6
#define LCD_SCL_OUT() P1DIR |= BIT7

#define LCD_RES_IN() P4DIR &= ~BIT4
#define LCD_SDA_IN() P1DIR &= ~BIT6
#define LCD_SCL_IN() P1DIR &= ~BIT7


void init();



#endif /* LCD_API_H_ */
