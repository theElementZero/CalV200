/*
 * lcd_api.c
 *
 *  Created on: 18-Jan-2021
 *      Author: ABC
 */
#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "inc/lcd_api.h"
void init();
void init1();
void clealddram();
void font1();
void font2();
void showpic(char *k);
void showpic1(char *k);
void write_com(char para);
void write_com1(char para1, char para2);
void write_data(int para);
void start();
void stop();
void sendbyte(int temp);
void check_ack();
char keyscan();
void delay_ms(int t);
void delayus(int t);

//========================================================
void init()
{
	LCD_RES_OUT();
	LCD_SDA_OUT();
	LCD_SCL_OUT();


	LCD_RES_HIGH();//	RES=1;
    	delay_ms(2000);
    LCD_RES_LOW();//	RES=0;
    	delay_ms(200);
    LCD_RES_HIGH();// 	RES=1;
    	delay_ms(2000);



    //	write_com(0xa0);		//Frame??
    ////	write_com(0xe8);		//e8 1/6 Bias  ea 1/7Bias
    //	write_com(0x81);		//Set VOP
    //	write_com(0xb5);		// 0xb5=7.0V
    //	write_com(0xc0);		//MX\MY
    //	write_com(0x84);
    //	write_com(0xf1);		//Duty
    //	write_com(0x1f);		// 1/32

/*
 *
    // Horizontal output direction (ADC segment driver selection)
    	glcd_command(GLCD_CMD_HORIZONTAL_NORMAL);
 *
 * */
    	write_com(0x40);     //SCOLL LINE=0     ST7539    set duty

    	write_com(0x84);     //SET PARTIAL SCREEN MODE

    	write_com1(0xf1,0x2F);  //SET COM END=16
       // write_com(0x1F);  //SET COM END=16

        write_com1(0xf2,0x00);  //SET PARTIAL START ADDRESS
       // write_com(0x00);  //SET PARTIAL START ADDRESS

        write_com1(0xf3,0x2F);  //SET PARTIAL END ADDRESS
        //write_com(0x1F);  //SET PARTIAL END ADDRESS

        write_com(0xa6);     //NORMAL DISPLAY
        write_com(0x89);     //SET RAM ADDRESS CONTROL
        write_com(0xa3);     //FRAME RATE=76Hz  a3
        write_com(0xea);     //SET BIAS=1/8

        write_com1(0x81,0x52);//SET VOP=0x52=7.8V; 0x40=7.5V


        write_com(0xC4);     //SET MX/MY NORMAL
        write_com(0xaf);     //DISPLAY ON

}
//========================================================
void clealddram()			//??
{
	int i,j;
	for(i=0Xb0;i<0Xb6;i++)
	{
	    write_com(i);
	    write_com1(0x10,0x00);
	    for(j=0;j<160;j++)
	{
	    write_data(0x00);
	}
	}
}
//========================================================
void font(char ph, char pl)
{
	int i,j;
	for(i=0xb0;i<0xb6;i++)
	{
	    write_com(i);
	    write_com1(0x10,0x00);
	    for(j=0;j<80;j++)
	{
	    write_data(ph);
        write_data(pl);
	}
	}
}

//========================================================
void showpic(char *p)			//????
{
    int i,j;

     for(i=0;i<6;i++)
	{
	write_com(0x40);
	write_com(i+0xb0);
	write_com(0x10);
	write_com(0x00);
	for(j=0;j<160;j++)
	 {
	write_data(p[i*160+j]);
	 }
        }
}
//========================================================
void write_com(char para)
{
	start();
  	sendbyte(0x78);
  	//check_ack();
  	sendbyte(para);
  	//check_ack();
 	stop();
}
//========================================================
void write_com1(char para1, char para2)
{
	start();
  	sendbyte(0x78);
  	//check_ack();
  	sendbyte(para1);
  	sendbyte(para2);
  	//check_ack();
  	stop();
}
//========================================================
void write_data(int para)
{
	start();
  	sendbyte(0x7A);
  	//check_ack();
  	sendbyte(para);
  	//check_ack();
  	stop();
}
//========================================================
void start()
{
	LCD_SDA_HIGH(); //SI=1;
	LCD_SCL_HIGH(); // SCL=1;
	delayus(20);
	LCD_SDA_LOW(); //SI=0;
	LCD_SCL_LOW(); //SCL=0;
}
//========================================================
void stop()
{
	LCD_SDA_LOW(); //SI=0;
	LCD_SCL_HIGH(); //SCL=1;
	delayus(20);
  	LCD_SDA_HIGH(); //SI=1;
  	LCD_SCL_LOW(); //SCL=1;
}
//========================================================
void sendbyte(int temp)
{
 	int i;
 	for(i=0;i<8;i++)
	{
	if((temp&0x80)==0x80) LCD_SDA_HIGH(); //SI=1;
	else LCD_SDA_LOW(); //SI=0;
	delayus(20);
	LCD_SCL_HIGH(); //SCL=1;
	delayus(20);
	LCD_SCL_LOW();//SCL=0;
	temp=temp<<1;
	}
 	LCD_SCL_HIGH();//	SCL=1;
 	LCD_SDA_LOW(); //SI=0;
	LCD_SCL_LOW();//SCL=0;
}
//========================================================
//void check_ack()
//{
//LL:
//	SCL=0;
//	SI=1;
//
//	if(P1^6==0) goto KK;
//	else goto LL;
//KK:
//	SCL=1;
//	SCL=0;
//	;
//}
//=======================================================
void display_off()
{
	write_com(0xae);	//DISPLAY OFF
	LCD_RES_LOW();//RES=0;	//power down BACKLIGHT
}
//======================================================
void display_on()
{
	init();
//	RES=1;	//power down BACKLIGHT
}
//========================================================
void delay_ms(int t)
{
	register int i,j;
	for(i=0;i<t;i++)
	for(j=0;j<21250;j++);
}

void delayus(int t){
	register int i,j;
		for(i=0;i<t;i++)
		for(j=0;j<100;j++);

}


