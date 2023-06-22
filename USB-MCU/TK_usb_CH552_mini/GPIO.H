#ifndef _GPIO_H_
#define _GPIO_H_
#include "./Public/CH552.H"

sbit IN4 = P3^2;
sbit IN3 = P1^4;
sbit IN2 = P1^5;
sbit IN1 = P1^6;

sbit LED_USB = P1^3;

sbit KEY1 = P1^1;
sbit KEY2 = P3^3;
sbit KEY3 = P3^4;
sbit KEY4 = P3^5;



void InitIO(void);
UINT16 ReadIO(void);
UINT8 ReadIOSlow(void);








#endif
