#include "GPIO.h"

#define GPIO_STA_FLOAT 0x0
#define GPIO_STA_PP 0x1
#define GPIO_STA_OD 0x2
#define GPIO_STA_DEF 0x3

void GPIO_SET(UINT8 Port, UINT8 IO, UINT8 STA){
  if(IO > 7)return;

  if(Port == 1){
    if(STA & 0x1)
      P1_DIR_PU |= 1<<IO;
    else
      P1_DIR_PU &= ~(1<<IO);
    
    if(STA & 0x2)
      P1_MOD_OC |= 1<<IO;
    else
      P1_MOD_OC &= ~(1<<IO);
  }
  
  if(Port == 3){
    if(STA & 0x1)
      P3_DIR_PU |= 1<<IO;
    else
      P3_DIR_PU &= ~(1<<IO);
    
    if(STA & 0x2)
      P3_MOD_OC |= 1<<IO;
    else
      P3_MOD_OC &= ~(1<<IO);
  }
}

void InitIO(void)
{
    GPIO_SET(3, 2, GPIO_STA_FLOAT);
    GPIO_SET(1, 4, GPIO_STA_FLOAT);
    GPIO_SET(1, 5, GPIO_STA_FLOAT);
    GPIO_SET(1, 6, GPIO_STA_FLOAT);
    GPIO_SET(1, 7, GPIO_STA_FLOAT);
    GPIO_SET(1, 0, GPIO_STA_FLOAT);
    GPIO_SET(1, 1, GPIO_STA_FLOAT);
    GPIO_SET(3, 1, GPIO_STA_FLOAT);

    IN1 = IN2 = IN3 = IN4 = IN5 = IN6 = IN7 = IN8 = 0;

    GPIO_SET(1, 2, GPIO_STA_PP);
    GPIO_SET(1, 3, GPIO_STA_PP);

    LED_ERR = LED_USB = 0;

    GPIO_SET(3, 5, GPIO_STA_PP);
    GPIO_SET(3, 4, GPIO_STA_DEF);
    GPIO_SET(3, 3, GPIO_STA_PP);

    SPI_CS = 0;
    SPI_MISO = 1;
    SPI_SCL = 1;
}

UINT8 READ_KEY(UINT8 ID)
{
    switch (ID)
    {
        // 		case 0:return (!IN1);
        // 		case 1:return (!IN2);
        // 		case 2:return (!IN3);
        // 		case 3:return (!IN4);
        // 		case 4:return (!IN5);
        // 		case 5:return (!IN6);
        // 		case 6:return (!IN7);
        // 		case 7:return (!IN8);
    case 0:
        return (IN1);
    case 1:
        return (IN2);
    case 2:
        return (IN3);
    case 3:
        return (IN4);
    case 4:
        return (IN5);
    case 5:
        return (IN6);
    case 6:
        return (IN7);
    case 7:
        return (IN8);
    }
    return 0;
}

extern UINT8 IO_ErrFlag;

#define HAND_TIME 2000
#define ERROR_TIME 20000
#define FILTER_TIME 2

UINT16 ReadIO(void)
{
    static UINT16 IO_COUNT[8] = {0};
    static UINT16 IO_COUNT_FILTER_UP[8] = {0};
    static UINT16 IO_COUNT_FILTER_DN[8] = {0};
    static UINT16 IODATA = 0;
    UINT8 i;

    for (i = 0; i < 8; i++)
    {
        if (READ_KEY(i))
        {
            IO_COUNT_FILTER_DN[i] = 0;
            if (IO_COUNT_FILTER_UP[i] < FILTER_TIME)
                IO_COUNT_FILTER_UP[i]++;
            if (IO_COUNT[i] < ERROR_TIME)
                IO_COUNT[i]++;

            if (IO_COUNT_FILTER_UP[i] >= FILTER_TIME)
            {
                IODATA |= 0x01 << i;
            }
            if (IO_COUNT[i] >= HAND_TIME)
            {
                IODATA &= ~(0x01 << i);
            }
            if (IO_COUNT[i] >= ERROR_TIME)
            {
                IO_ErrFlag |= 0x01 << i;
            }
        }
        else
        {
            IO_COUNT_FILTER_UP[i] = 0;
            if (IO_COUNT_FILTER_DN[i] < FILTER_TIME)
                IO_COUNT_FILTER_DN[i]++;

            if (IO_COUNT_FILTER_DN[i] >= FILTER_TIME)
            {
                IODATA &= ~(0x01 << i);
                IO_ErrFlag &= ~(0x01 << i);
                IO_COUNT[i] = 0;
            }
        }
    }

    IODATA &= 0xFF;
    IODATA |= ReadIOSlow() << 8;

    return IODATA;
}

UINT8 ReadIOSlow(void)
{
    UINT8 i;
    static UINT8 step = 0;

    static UINT8 IODATA = 0;
    static UINT8 IODATA_Bak = 0;
    static UINT8 IODATA_Out = 0;

    switch (step)
    {
    case 0:
        step++;
        SPI_CS = 1;
        IODATA = 0;
        break;

    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
        i = step - 1;
        if (SPI_SCL)
        {
            if (SPI_MISO == 0)
            {
                IODATA |= 0x01 << i;
            }
            else
            {
                IODATA &= ~(0x01 << i);
            }
            SPI_SCL = 0;
        }
        else
        {
            step++;
            SPI_SCL = 1;
        }
        break;

    case 9:
        IODATA_Bak = IODATA;
		IODATA_Out = IODATA_Bak | IODATA;
        SPI_CS = 0;
        step = 0;
        break;

    default:
        SPI_CS = 0;
        step = 0;
        break;
    }

    return IODATA_Out;
}

//#define IO_FILTER_NUM 14    //50ms

//UINT8 IOFilter(UINT8 temp)
//{
//    static UINT8 io_data[IO_FILTER_NUM] = {0};
//    static UINT8 index = 0;
//    static UINT8 temp_bak = 0;

//    UINT8 i;
//    UINT8 data_xor = 0;
//    UINT8 data_nor = 0xFF;

//    io_data[index++] = temp;

//    if (index >= IO_FILTER_NUM)
//        index = 0;

//    for (i = 0; i < IO_FILTER_NUM; i++)
//    {
//        data_xor |= io_data[i];
//        data_nor &= io_data[i];
//    }

//    temp_bak &= data_xor;
//    temp_bak |= data_nor;

//    return temp_bak;
//}
