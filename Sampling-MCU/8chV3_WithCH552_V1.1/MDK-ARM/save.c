#include "save.h"
#include "stdlib.h"


extern uint8_t SystemError;

const struct SystemSet_DEF SystemSetDefault = {
	
  OUT_LENG_DEF_H,OUT_LENG_DEF_L,

  {0, 1, 2, 3, 4, 5, 6, 7,},
//  {4,7,5,6,0,1,2,3,},
	
	{ADC_DIV_DEF, ADC_DIV_DEF, ADC_DIV_DEF, ADC_DIV_DEF,
	 ADC_DIV_DEF, ADC_DIV_DEF, ADC_DIV_DEF, ADC_DIV_DEF},

  ENABLE,
  ENABLE,
  WAIT_MUTE_TIMES,
  
  {0xCC, },
		
  {ADC_TRIGGER_LEVEL, ADC_TRIGGER_LEVEL, ADC_TRIGGER_LEVEL, ADC_TRIGGER_LEVEL,
   ADC_TRIGGER_LEVEL, ADC_TRIGGER_LEVEL, ADC_TRIGGER_LEVEL, ADC_TRIGGER_LEVEL,},
  
  {0xCCCC, },
  
  0xCCCCCCCCCCCCCCCC,
  
};


const struct SystemSet_DEF SystemSetTest = {
	
  50,50,

  {0, 1, 2, 3, 4, 5, 6, 7,},
	
	{100, 100, 100, 100, 100, 100, 100, 100, },

  0,
  0,
  10,
  
  {0xCC, },
		
  {100, 100, 100, 100, 100, 100, 100, 100,},
  
  {0xCCCC, },
  
  0xCCCCCCCCCCCCCCCC,
  
};


struct SystemSet_DEF SystemSet;
struct SystemSet_DEF SystemSetTemp;


void LoadSave(void){
  uint32_t ADDR = FLASH_ADDR_SAVE; 
  memcpy(&SystemSet,(uint8_t*)ADDR,sizeof(SystemSet));
}

int CmpSave(void){
  uint32_t ADDR = FLASH_ADDR_SAVE; 
  return memcmp(&SystemSet,(uint8_t*)ADDR,sizeof(SystemSet));
}


uint8_t CheckSave(struct SystemSet_DEF* Set){
  if(Set->OutHiLength > OUT_LENG_MAX || Set->OutHiLength < OUT_LENG_MIN)return 1;
  if(Set->OutLoLength > OUT_LENG_MAX || Set->OutLoLength < OUT_LENG_MIN)return 1;
	
  for(uint8_t i=0;i<8;i++){
    uint8_t count = 0;
    for(uint8_t j=0;j<8;j++){
      if(Set->ADC_ChannelRemap[j] == i)count++;
    }
    if(count != 1)return 1;
		if(Set->ADC_Mul[i] < 10)return 1;
  }
  if(Set->MuteEnable > 1)return 1;
  if(Set->ADC_Mute_Side > 1)return 1;
  if(Set->RES2 == 0xFFFFFFFFFFFFFFFF)return 1;
  if(Set->RES2 == 0x0)return 1;
  
  return 0;
}



void SaveInit(void){
  
  LoadSave();
  if(CheckSave(&SystemSet)){
    SystemSet = SystemSetDefault;
    CloseIrqAndSave();
    LoadSave();
    if(CheckSave(&SystemSet)){
      SystemSet = SystemSetDefault;
      SystemError = 1;
    }
  }
}


uint8_t DoSave(void){
  uint32_t PageError = 0; 
  uint64_t* pSystemSet = (uint64_t*)&SystemSet; 
  
  FLASH_EraseInitTypeDef FLASH_EraseInitType = {
	FLASH_TYPEERASE_PAGES,0,FLASH_PAGE_SAVE,1
	};
  
  if(CheckSave(&SystemSet))return 1;
  if(CmpSave() == 0)return 0;
  
  HAL_FLASH_Unlock();
  
  HAL_FLASHEx_Erase(&FLASH_EraseInitType, &PageError);
  
  if(0xFFFFFFFF != PageError){
    //Erase Failed!
    SystemError = 1;
		HAL_FLASH_Lock();
    return 1;
  }
  
  for(uint16_t i=0;i<(sizeof(SystemSet)/8);i++){
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, FLASH_ADDR_SAVE + (i*8), pSystemSet[i]);
  }
  
  HAL_FLASH_Lock();
  return 0;
}



const uint16_t CRCVALUE[256]=
{
  0x0000,0xC1C0,0x81C1,0x4001,0x01C3,0xC003,0x8002,0x41C2,0x01C6,0xC006,0x8007,0x41C7,
	0x0005,0xC1C5,0x81C4,0x4004,0x01CC,0xC00C,0x800D,0x41CD,0x000F,0xC1CF,0x81CE,0x400E,
	0x000A,0xC1CA,0x81CB,0x400B,0x01C9,0xC009,0x8008,0x41C8,0x01D8,0xC018,0x8019,0x41D9,
	0x001B,0xC1DB,0x81DA,0x401A,0x001E,0xC1DE,0x81DF,0x401F,0x01DD,0xC01D,0x801C,0x41DC,
	0x0014,0xC1D4,0x81D5,0x4015,0x01D7,0xC017,0x8016,0x41D6,0x01D2,0xC012,0x8013,0x41D3,
	0x0011,0xC1D1,0x81D0,0x4010,0x01F0,0xC030,0x8031,0x41F1,0x0033,0xC1F3,0x81F2,0x4032,
	0x0036,0xC1F6,0x81F7,0x4037,0x01F5,0xC035,0x8034,0x41F4,0x003C,0xC1FC,0x81FD,0x403D,
	0x01FF,0xC03F,0x803E,0x41FE,0x01FA,0xC03A,0x803B,0x41FB,0x0039,0xC1F9,0x81F8,0x4038,
	0x0028,0xC1E8,0x81E9,0x4029,0x01EB,0xC02B,0x802A,0x41EA,0x01EE,0xC02E,0x802F,0x41EF,
	0x002D,0xC1ED,0x81EC,0x402C,0x01E4,0xC024,0x8025,0x41E5,0x0027,0xC1E7,0x81E6,0x4026,
	0x0022,0xC1E2,0x81E3,0x4023,0x01E1,0xC021,0x8020,0x41E0,0x01A0,0xC060,0x8061,0x41A1,
	0x0063,0xC1A3,0x81A2,0x4062,0x0066,0xC1A6,0x81A7,0x4067,0x01A5,0xC065,0x8064,0x41A4,
	0x006C,0xC1AC,0x81AD,0x406D,0x01AF,0xC06F,0x806E,0x41AE,0x01AA,0xC06A,0x806B,0x41AB,
	0x0069,0xC1A9,0x81A8,0x4068,0x0078,0xC1B8,0x81B9,0x4079,0x01BB,0xC07B,0x807A,0x41BA,
	0x01BE,0xC07E,0x807F,0x41BF,0x007D,0xC1BD,0x81BC,0x407C,0x01B4,0xC074,0x8075,0x41B5,
	0x0077,0xC1B7,0x81B6,0x4076,0x0072,0xC1B2,0x81B3,0x4073,0x01B1,0xC071,0x8070,0x41B0,
	0x0050,0xC190,0x8191,0x4051,0x0193,0xC053,0x8052,0x4192,0x0196,0xC056,0x8057,0x4197,
	0x0055,0xC195,0x8194,0x4054,0x019C,0xC05C,0x805D,0x419D,0x005F,0xC19F,0x819E,0x405E,
	0x005A,0xC19A,0x819B,0x405B,0x0199,0xC059,0x8058,0x4198,0x0188,0xC048,0x8049,0x4189,
	0x004B,0xC18B,0x818A,0x404A,0x004E,0xC18E,0x818F,0x404F,0x018D,0xC04D,0x804C,0x418C,
	0x0044,0xC184,0x8185,0x4045,0x0187,0xC047,0x8046,0x4186,0x0182,0xC042,0x8043,0x4183,
	0x0041,0xC181,0x8180,0x4040,
};

   
uint16_t CRCCheck(uint8_t *tempMsg, uint8_t tempLength){
  uint16_t tempCrcHigh = 0xFF;
  uint16_t tempCrcLow = 0xFF;  
  uint16_t tempIndex = 0; 

  for(uint8_t i = 0; i < tempLength; i++){
    tempIndex = tempCrcLow ^ (tempMsg[i]);
    tempCrcLow = tempCrcHigh ^ (CRCVALUE[tempIndex] >>8);  
    tempCrcHigh = CRCVALUE[tempIndex] & 0xff;
  }
  return (tempCrcHigh | (tempCrcLow<<8));
}



#define CMD_FAULT             0x10
#define CMD_ACK             	0x20

#define CMD_SYS_RESET         0xA0
#define CMD_SYS_SAVE          0xA1

#define CMD_ACK_DEF_SET       0xB0
#define CMD_ACK_SAVE_SET      0xB1
#define CMD_ACK_NOW_SET       0xB2

#define CMD_SEND_SET          0xC0
#define CMD_CALL_DEF          0xC1

#define CMD_DEBUG_LOG         0xD1


uint8_t TXBuffer[200]; 
uint8_t DEBUG_EN = 0;



void SendDone(uint8_t CMD,uint8_t DATA){
  uint8_t i = 0;
  uint16_t CRC_DATA;
  TXBuffer[i++] = CMD;
  TXBuffer[i++] = DATA;
  
  CRC_DATA = CRCCheck(TXBuffer,i);
  TXBuffer[i++] = CRC_DATA>>8;
  TXBuffer[i++] = CRC_DATA;
  HAL_UART_Transmit_DMA(&huart1, TXBuffer, i);
}

void ACK_SET(uint8_t CMD){
  uint8_t i = 0;
  uint16_t CRC_DATA;
  
  TXBuffer[i++] = CMD;
  if(CMD == CMD_ACK_DEF_SET){
    memcpy(&TXBuffer[i], &SystemSetDefault, sizeof(SystemSetDefault));
    i+= sizeof(SystemSetDefault);
  }
  else if(CMD == CMD_ACK_SAVE_SET){
    memcpy(&TXBuffer[i], (uint8_t*)FLASH_ADDR_SAVE, sizeof(SystemSetDefault));
    i+= sizeof(SystemSetDefault);
  }
  else if(CMD == CMD_ACK_NOW_SET){
    memcpy(&TXBuffer[i], &SystemSet, sizeof(SystemSetDefault));
    i+= sizeof(SystemSetDefault);
  }
  else
    TXBuffer[i++] = 0;
  
  CRC_DATA = CRCCheck(TXBuffer,i);
  TXBuffer[i++] = CRC_DATA>>8;
  TXBuffer[i++] = CRC_DATA;
  HAL_UART_Transmit_DMA(&huart1, TXBuffer, i);
  
}


void SciDoCMD(uint8_t* data, uint8_t len){
  uint8_t temp = 0;
  
  if(data[0] != CMD_SEND_SET){
    if(len != 4){
      SendDone(CMD_FAULT,data[0]);
      return;
    }
  }
  
	
  switch(data[0]){
		
		case CMD_ACK:
      SendDone(CMD_ACK,0x01);
			break;
    
    case CMD_SYS_RESET:
      HAL_NVIC_SystemReset();
      break;
    
    case CMD_ACK_DEF_SET:
    case CMD_ACK_SAVE_SET:
    case CMD_ACK_NOW_SET:
      ACK_SET(data[0]);
      break;
    
    case CMD_SYS_SAVE:
      if(CloseIrqAndSave() == 0){
        SendDone(CMD_SYS_SAVE,0x01);
      }
      else{
        SendDone(CMD_SYS_SAVE,0x00);
      }
      break;
    
    case CMD_CALL_DEF:
      SystemSet = SystemSetDefault;
      SendDone(CMD_CALL_DEF,0x01 | temp);
      break;
    
    case CMD_SEND_SET:
      
      if(len == (sizeof(SystemSet) + 3)){
        memcpy(&SystemSetTemp, &data[1], sizeof(SystemSet));
        if(CheckSave(&SystemSetTemp) == 0){
          SystemSet = SystemSetTemp;
          SendDone(CMD_SEND_SET,0x01);
        }
        else
          SendDone(CMD_SEND_SET,0x00);
      }
      else{
        SendDone(CMD_SEND_SET,0x00);
      }
      break;
			
		case CMD_DEBUG_LOG:
			DEBUG_EN = data[1] != 0;
			UartDebug();
			break;
    
    default:
      SendDone(CMD_FAULT,data[0]);
    
  }
  
}



extern uint16_t InputLevel_MAX[8];
extern uint16_t InputLevel_Now[8];

void UartDebug(void){
	uint8_t i=0;
	
	if(DEBUG_EN){
		uint16_t CRC_DATA;
		TXBuffer[i++] = CMD_DEBUG_LOG;
		
		memcpy(&TXBuffer[i],(uint8_t*)InputLevel_MAX,sizeof(InputLevel_MAX));
		i += sizeof(InputLevel_MAX);
		
		CRC_DATA = CRCCheck(TXBuffer,i);
		TXBuffer[i++] = CRC_DATA>>8;
		TXBuffer[i++] = CRC_DATA;
		HAL_UART_Transmit_DMA(&huart1, TXBuffer, i);
	}
}






