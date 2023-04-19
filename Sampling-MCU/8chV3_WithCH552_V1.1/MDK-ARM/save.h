#ifndef _SAVE_H_
#define _SAVE_H_
#include "main.h"


#define FLASH_PAGE_SAVE   0x0F
#define FLASH_ADDR_SAVE   (0x08000000 + FLASH_PAGE_SAVE * 0x800)

#define FLASH_PAGE_ACTIVE   0x01
#define FLASH_ADDR_ACTIVE   (0x08000000 + FLASH_PAGE_ACTIVE * 0x800)


/*  system save define   */

#define OUT_LENG_MAX   50
#define OUT_LENG_MIN   2
#define OUT_LENG_DEF_H   3
#define OUT_LENG_DEF_L   3

#define ADC_TRIGGER_LEVEL	   	15

#define WAIT_MUTE_TIMES  13


#define ADC_DIV_DEF			100



struct SystemSet_DEF{
	
	uint8_t OutHiLength;
	uint8_t OutLoLength;
  
  uint8_t ADC_ChannelRemap[8];    //Input Channel Order
  
  uint8_t ADC_Mul[8];        		//
  
  uint8_t MuteEnable;
  uint8_t ADC_Mute_Side;        //相同侧屏蔽倍数
  uint8_t ADC_WaitMuteTime;      //time to wait mute
	
	uint8_t RES0[1];
	
  uint16_t ADC_Trigger[8];        //
  
  uint16_t RES1[1];
  
  uint64_t RES2;                   //对齐
  
};




extern struct SystemSet_DEF SystemSet;
extern const struct SystemSet_DEF SystemSetDefault;
extern const struct SystemSet_DEF SystemSetTest;




void SaveInit(void);


uint8_t DoSave(void);
uint16_t CRCCheck(uint8_t *tempMsg, uint8_t tempLength);
void SciDoCMD(uint8_t* data, uint8_t len);


void UartDebug(void);







#endif
