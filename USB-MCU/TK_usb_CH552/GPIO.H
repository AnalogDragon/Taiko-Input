#ifndef _GPIO_H_
#define _GPIO_H_
#include "./Public/CH552.H"

sbit IN1 = P3 ^ 2;
sbit IN2 = P1 ^ 4;
sbit IN3 = P1 ^ 5;
sbit IN4 = P1 ^ 6;
sbit IN5 = P1 ^ 7;
sbit IN6 = P1 ^ 0;
sbit IN7 = P1 ^ 1;
sbit IN8 = P3 ^ 1;

sbit LED_ERR = P1 ^ 3;
sbit LED_USB = P1 ^ 2;

sbit SPI_CS = P3 ^ 5;
sbit SPI_MISO = P3 ^ 4;
sbit SPI_SCL = P3 ^ 3;

void InitIO(void);
UINT16 ReadIO(void);
UINT8 ReadIOSlow(void);
UINT8 IOFilter(UINT8 temp);

#endif
