#include "GPIO.h"

#define GPIO_STA_FLOAT  0x0
#define GPIO_STA_PP     0x1
#define GPIO_STA_OD     0x2
#define GPIO_STA_DEF    0x3

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

void InitIO(void){
  
  GPIO_SET(3,2,GPIO_STA_FLOAT);
  GPIO_SET(1,4,GPIO_STA_FLOAT);
  GPIO_SET(1,5,GPIO_STA_FLOAT);
  GPIO_SET(1,6,GPIO_STA_FLOAT);
  
  IN1 = IN2 = IN3 = IN4 = 0;
  
  GPIO_SET(1,3,GPIO_STA_PP);
  
  LED_USB = 0;
  
  GPIO_SET(1,1,GPIO_STA_DEF);
  GPIO_SET(3,3,GPIO_STA_DEF);
  GPIO_SET(3,4,GPIO_STA_DEF);
  GPIO_SET(3,5,GPIO_STA_DEF);
  
  KEY1 = KEY2 = KEY3 = KEY4 = 1;
  
  
}

UINT8 READ_KEY(UINT8 ID){
	switch(ID){
// 		case 0:return (!IN1);
// 		case 1:return (!IN2);
// 		case 2:return (!IN3);
// 		case 3:return (!IN4);
    case 0:return (IN1);
    case 1:return (IN2);
    case 2:return (IN3);
    case 3:return (IN4);
	}
	return 0;
}

UINT8 READ_KEY2(UINT8 ID){
	switch(ID){
		case 0:return (!KEY1);
 		case 1:return (!KEY2);
		case 2:return (!KEY3);
 		case 3:return (!KEY4);
	}
	return 0;
}

extern UINT8 IO_ErrFlag;

#define HAND_TIME 2000
#define ERROR_TIME 20000
#define FILTER_TIME 2

UINT16 ReadIO(void){
	
	static UINT16 IO_COUNT[4] = {0};
	static UINT16 IO_COUNT_FILTER_UP[4] = {0};
	static UINT16 IO_COUNT_FILTER_DN[4] = {0};
	static UINT16 IODATA = 0;
	UINT8 i;

	for(i=0;i<4;i++){
		if(READ_KEY(i)){
			IO_COUNT_FILTER_DN[i] = 0;
			if(IO_COUNT_FILTER_UP[i] < FILTER_TIME)IO_COUNT_FILTER_UP[i]++;
			if(IO_COUNT[i] < ERROR_TIME)IO_COUNT[i]++;

			if(IO_COUNT_FILTER_UP[i] >= FILTER_TIME){
				IODATA |= 0x01<<i;
			}
			if(IO_COUNT[i] >= HAND_TIME){
				IODATA &= ~(0x01<<i);
			}
			if(IO_COUNT[i] >= ERROR_TIME){
				IO_ErrFlag |= 0x01<<i;
			}
		}
		else{
			IO_COUNT_FILTER_UP[i] = 0;
			if(IO_COUNT_FILTER_DN[i] < FILTER_TIME)IO_COUNT_FILTER_DN[i]++;

			if(IO_COUNT_FILTER_DN[i] >= FILTER_TIME){	 
				IODATA &= ~(0x01<<i);	 
				IO_ErrFlag &= ~(0x01<<i);
				IO_COUNT[i] = 0;
			}
		}
	}
  
  IODATA &= 0xFF;
  IODATA |= ReadIOSlow()<<8;
  
  return IODATA;
}


#define HAND_TIME2     6000
#define FILTER_TIME2   10

#define LONG_PRESS_TIME  2000

#define TRIGGER_TIME  (100)	//ms

UINT8 ReadIOSlow(void){
	
	static UINT16 IO_COUNT_FILTER_UP[4] = {0};
	static UINT16 IO_COUNT_FILTER_DN[4] = {0};
	static UINT8 IODATA = 0;
	static UINT8 IODATA_bak = 0;
	static KeyOut = 0;
	static UINT16 IODATA_count = 0;
	UINT8 i;

  UINT8C kEYtABLE[0x10] = {
//	#	|	1	|	2	|	3	|	4
	0x00, 
			0x01,	
			0x02,
					0x11,
			0x03,
					0x00,
					0x12,
							0x21, 
			0x04,
					0x1F,
					0x00, 
							0x00,
					0x13,
							0x00, 
		  				0x21,
				  				0x31, 
  };



	for(i=0;i<4;i++){
		if(READ_KEY2(i)){
			IO_COUNT_FILTER_DN[i] = 0;
			if(IO_COUNT_FILTER_UP[i] < FILTER_TIME2)IO_COUNT_FILTER_UP[i]++;

			if(IO_COUNT_FILTER_UP[i] >= FILTER_TIME2){
				IODATA |= 0x01<<i;
			}
		}
		else{
			IO_COUNT_FILTER_UP[i] = 0;
			if(IO_COUNT_FILTER_DN[i] < FILTER_TIME2)IO_COUNT_FILTER_DN[i]++;

			if(IO_COUNT_FILTER_DN[i] >= FILTER_TIME2){	 
				IODATA &= ~(0x01<<i);	 
			}
		}
	}
  
  if(IODATA){
    IODATA_count++;
  }
  else{
    IODATA_count = 0;
  }
  
  
//   if(KeyOut){
//     //当前有按键
//     if(IODATA == 0){
//       KeyOut = 0;
//     }
//   }
//   else{
//     if(IODATA_bak && IODATA == 0){
//       //按下并弹起
//       if(IODATA_bak == 1)KeyOut = 1;
//       else if(IODATA_bak == 2)KeyOut = 2;
//       else if(IODATA_bak == 4)KeyOut = 3;
//       else if(IODATA_bak == 8)KeyOut = 4;
//       else KeyOut = 0xFF;
//     }
//     else if(IODATA && IODATA_count > LONG_PRESS_TIME){
//       //长按
//       if(IODATA == 1)KeyOut = 0x81;
//       else if(IODATA == 2)KeyOut = 0x82;
//       else if(IODATA == 4)KeyOut = 0x83;
//       else if(IODATA == 8)KeyOut = 0x84;
//       else KeyOut = 0xFF;
//     }
//     else if(IODATA == 0x03){
//       KeyOut = 0x05;
//     }
//     else if(IODATA == 0x0C){
//       KeyOut = 0x06;
//     }
//   }

  if(KeyOut){
    //当前有按键
    if(IODATA == 0){
      KeyOut = 0;
    }
  }
  else{
    if(IODATA != 0 && IODATA_count >= TRIGGER_TIME){
      KeyOut = kEYtABLE[IODATA];
    }
  }

  IODATA_bak = IODATA;
  return KeyOut;
}



/*------------------------------data flash-----------------------------------*/

#define DEF_FLASH_OP_CHECK1     0xAA
#define DEF_FLASH_OP_CHECK2     0x55   

UINT8 Flash_Op_Check_Byte1 = 0x00;
UINT8 Flash_Op_Check_Byte2 = 0x00;


UINT8 Flash_Op_Unlock( UINT8 flash_type )
{
    bit ea_sts;
    
    /* Check the Flash operation flags to prevent Flash misoperation. */
    if( ( Flash_Op_Check_Byte1 != DEF_FLASH_OP_CHECK1 ) ||
        ( Flash_Op_Check_Byte2 != DEF_FLASH_OP_CHECK2 ) )
    {
        return 0xFF;   /* Flash Operation Flags Error */
    }
    
    /* Disable all INTs to prevent writing GLOBAL_CFG from failing in safe mode. */
    ea_sts = EA;                                
    EA = 0;
    
    /* Enable Flash writing operations. */
    SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG |= flash_type;
    SAFE_MOD = 0x00;

    /* Restore all INTs. */
    EA = ea_sts;
    
    return 0x00;
}


void Flash_Op_Lock( UINT8 flash_type )
{
    bit ea_sts;
    
    /* Disable all INTs to prevent writing GLOBAL_CFG from failing in safe mode. */
    ea_sts = EA;                                
    EA = 0;
    
    /* Disable Flash writing operations. */
    SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG &= ~flash_type;
    SAFE_MOD = 0x00;
    
    /* Restore all INTs. */
    EA = ea_sts;
}


bit clear_data_flash(void){
    UINT8 i;
    
    if( Flash_Op_Unlock( bDATA_WE ) )
        return 1;
    
    ROM_ADDR_H = DATA_FLASH_ADDR >> 8;
    for(i=0;i<128;i++)
	{
        ROM_ADDR_L = i*2;
        ROM_DATA_L = 0xFF;			
        if ( ROM_STATUS & bROM_ADDR_OK ) { 
           ROM_CTRL = ROM_CMD_WRITE; 
        }
        if((ROM_STATUS ^ bROM_ADDR_OK) > 0) return 1;
	}
    Flash_Op_Lock( bDATA_WE );
    
    return 0;
}

UINT8 load_mode(void){
	//從前往後找
	UINT8 i;
	UINT8 dat;
	bit data_err = 0;
	UINT8 last_addr = 0xFF;
	UINT8 last_data = 0xFF;

	//檢查是否有非法數據（不等於255且大於最大mode）
	ROM_ADDR_H = DATA_FLASH_ADDR >> 8;
	for(i=0;i<128;i++){
		ROM_ADDR_L = i*2;
		ROM_CTRL = ROM_CMD_READ;
		dat = ROM_DATA_L;
		if(dat != 0xFF && dat > MODE_MAX){
			//裏面有奇怪的數據
			data_err = 1;
		}
		if(dat <= MODE_MAX){
			last_addr = i;
			last_data = dat;
		}
	}
	
	if(last_data <= MODE_MAX){
		return last_data;
	}
	
	return MODE_STARTUP;
}

bit save_mode(UINT8 mode){	
	//從前往後存
	UINT8 i;
	UINT8 dat;
	bit data_err = 0;
	bit no_data = 1;
	UINT8 last_addr = 0;
	UINT8 last_data = 0xFF;
	
	if(mode > MODE_MAX)
		return 1;
	
	//檢查是否有非法數據（不等於255且大於最大mode）
	ROM_ADDR_H = DATA_FLASH_ADDR >> 8;
	for(i=0;i<128;i++){
		ROM_ADDR_L = i*2;
		ROM_CTRL = ROM_CMD_READ;
		dat = ROM_DATA_L;
		if(dat != 0xFF && dat > MODE_MAX){
			//裏面有奇怪的數據
			data_err = 1;
		}
		if(dat <= MODE_MAX){
			last_addr = i;
			last_data = dat;
			no_data = 0;
		}
	}
	
	if(data_err || last_addr >= 127){
		last_addr = 0;
		if(clear_data_flash()){
			mDelaymS(5);
			if(clear_data_flash()){
				return 1;
			}
		}
	}
	else{
		if(!no_data)
		last_addr++;
	}
	
	if(mode == last_data){
		return 0;
	}
	
	mDelaymS(5);
	
    if( Flash_Op_Unlock( bDATA_WE ) )
        return 1;
    
    ROM_ADDR_H = DATA_FLASH_ADDR >> 8;

	ROM_ADDR_L = last_addr*2;
	ROM_DATA_L = mode;
	if ( ROM_STATUS & bROM_ADDR_OK ) { 
	   ROM_CTRL = ROM_CMD_WRITE; 
	}
	if((ROM_STATUS ^ bROM_ADDR_OK) > 0) return 1;
	
    Flash_Op_Lock( bDATA_WE );
    
    return 0;
	
}



void test(void){
	UINT16 i;
	
	for(i=0;i<6000;i++){
		save_mode(i%3);
		if((i%3) != load_mode()){
			while(1){
				WDOG_COUNT = 0;
				LED_USB = 0;
			}
		}
		WDOG_COUNT = 0;
	}
	
}






