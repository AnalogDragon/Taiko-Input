/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "save.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

IWDG_HandleTypeDef hiwdg;

TIM_HandleTypeDef htim14;

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_tx;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_IWDG_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM14_Init(void);
static void MX_ADC1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

#ifdef IS_4CH_MODE

#define ADC_BUFFER_SIZE     10
#define ADC_LIST_SIZE       6
#define ADC_CHANNEL_NUM     8
#define CHANNEL_NUM     		4

#else

#define ADC_BUFFER_SIZE     10
#define ADC_LIST_SIZE       6
#define ADC_CHANNEL_NUM     8
#define CHANNEL_NUM     		8

#endif


uint8_t POWER_ON_EN = 0;

//
void GPIO_Output(uint8_t IO, uint8_t state){
  switch(IO){
    case 0:if(state)OUT1_GPIO_Port->BSRR = (uint32_t)OUT1_Pin;else OUT1_GPIO_Port->BRR = (uint32_t)OUT1_Pin;break;
    case 1:if(state)OUT2_GPIO_Port->BSRR = (uint32_t)OUT2_Pin;else OUT2_GPIO_Port->BRR = (uint32_t)OUT2_Pin;break;
    case 2:if(state)OUT3_GPIO_Port->BSRR = (uint32_t)OUT3_Pin;else OUT3_GPIO_Port->BRR = (uint32_t)OUT3_Pin;break;
    case 3:if(state)OUT4_GPIO_Port->BSRR = (uint32_t)OUT4_Pin;else OUT4_GPIO_Port->BRR = (uint32_t)OUT4_Pin;break;
#ifndef IS_4CH_MODE
    case 4:if(state)OUT5_GPIO_Port->BSRR = (uint32_t)OUT5_Pin;else OUT5_GPIO_Port->BRR = (uint32_t)OUT5_Pin;break;
    case 5:if(state)OUT6_GPIO_Port->BSRR = (uint32_t)OUT6_Pin;else OUT6_GPIO_Port->BRR = (uint32_t)OUT6_Pin;break;
    case 6:if(state)OUT7_GPIO_Port->BSRR = (uint32_t)OUT7_Pin;else OUT7_GPIO_Port->BRR = (uint32_t)OUT7_Pin;break;
    case 7:if(state)OUT8_GPIO_Port->BSRR = (uint32_t)OUT8_Pin;else OUT8_GPIO_Port->BRR = (uint32_t)OUT8_Pin;break;
#endif
  }
}

uint16_t OutListFerq = 1000; //us

volatile uint16_t I_Buffer = 0;
volatile uint16_t O_Buffer = 0;
volatile uint16_t O_CountDown[CHANNEL_NUM] = {0};

volatile uint16_t ADC_Buffer[ADC_BUFFER_SIZE][ADC_CHANNEL_NUM];
volatile uint16_t ADC_List[ADC_CHANNEL_NUM][ADC_LIST_SIZE];
volatile uint16_t ADC_Now[ADC_CHANNEL_NUM];

volatile uint16_t ADC_Zero[ADC_CHANNEL_NUM] = {0};

volatile uint8_t InputCount[CHANNEL_NUM] = {0};
volatile uint16_t InputLevel_MAX[CHANNEL_NUM] = {0};
volatile uint16_t InputLevel_Now[CHANNEL_NUM] = {0};


uint8_t SystemError = 0;
uint8_t SystemTest = 0;

#ifndef IS_4CH_MODE
uint8_t SET_1 = 0;
uint8_t SET_2 = 0;
uint8_t SET_3 = 0;
uint8_t SET_4 = 0;
#endif
//输出函数
void IO_OutputList(void){
 
  for(uint8_t i = 0; i < CHANNEL_NUM; i ++){
    if(O_Buffer & (1 << i)){
			
      if(POWER_ON_EN)
        GPIO_Output(i, SystemSet.OutLoLength < O_CountDown[i]);
			
      O_CountDown[i]--;
			
      if(O_CountDown[i] == 0){
        O_Buffer &= ~(1 << i);
				I_Buffer &= ~(1 << i);
      }
    }
    else{
      O_CountDown[i] = 0;
      GPIO_Output(i, 0);
    }
  }
}


//清除操作
void DoMute(uint8_t ID){
	I_Buffer &= ~(1 << ID);
	InputCount[ID] = 0;
	InputLevel_MAX[ID] = 0;
}

//操作输出
void DoOutput(uint8_t ID){
  
  UartDebug();
  
	//清除整组异端
  if(SystemSet.ADC_Mute_Side){
    
    if(ID > 3){
      DoMute(4);
      DoMute(5);
      DoMute(6);
      DoMute(7);
    }
    else{
      DoMute(0);
      DoMute(1);
      DoMute(2);
      DoMute(3);
    }
    
  }
  else{
    
    if(ID == 0 || ID == 3){
      DoMute(1);
      DoMute(2);
    }
    else if(ID == 1 || ID == 2){
      DoMute(0);
      DoMute(3);
    }
    else if(ID == 4 || ID == 7){
      DoMute(5);
      DoMute(6);
    }
    else if(ID == 5 || ID == 6){
      DoMute(4);
      DoMute(7);
    }
    else return;
    
    DoMute(ID);
    
  }
	
  //allow output
  O_Buffer |= 1 << ID;
  O_CountDown[ID] = SystemSet.OutHiLength + SystemSet.OutLoLength;
	//here need to make all channel mute
}


void REG_Output(uint8_t ID,uint16_t Level){
	
	//首次登记
	if(((I_Buffer>>ID)&1) == 0){
		I_Buffer |= 1<<ID;
		InputCount[ID] = 0;
	}
	
	//记录最大值
	if(Level > InputLevel_MAX[ID]){
		InputLevel_MAX[ID] = Level;
	}
	
}

//查找最大值，仅用于登记后
uint8_t CheckAdcMax(uint8_t ID, uint16_t* pValueList){
	
  uint16_t ADC_Value = pValueList[ID];
	
  if(SystemSet.ADC_Mute_Side){    
    
    switch(ID){
      //1P
      case 0:
        if(
          ADC_Value < pValueList[1]
         ||ADC_Value < pValueList[2]
         ||ADC_Value < pValueList[3]
          ){
          return 0;
        }
        else{
          return 1;
        }
        
      case 1:
        if(
          ADC_Value < pValueList[0]
         ||ADC_Value < pValueList[2]
         ||ADC_Value < pValueList[3]
          ){
          return 0;
        }
        else{
          return 1;
        }
        
      case 2:
        if(
          ADC_Value < pValueList[1]
         ||ADC_Value < pValueList[0]
         ||ADC_Value < pValueList[3]
          ){
          return 0;
        }
        else{
          return 1;
        }
        
      case 3:
        if(
          ADC_Value < pValueList[1]
         ||ADC_Value < pValueList[2]
         ||ADC_Value < pValueList[0]
          ){
          return 0;
        }
        else{
          return 1;
        }
        
        //2P
      case 4:
        if(
          ADC_Value < pValueList[5]
         ||ADC_Value < pValueList[6]
         ||ADC_Value < pValueList[7]
          ){
          return 0;
        }
        else{
          return 1;
        }
        
      case 5:
        if(
          ADC_Value < pValueList[4]
         ||ADC_Value < pValueList[6]
         ||ADC_Value < pValueList[7]
          ){
          return 0;
        }
        else{
          return 1;
        }
        
      case 6:
        if(
          ADC_Value < pValueList[5]
         ||ADC_Value < pValueList[4]
         ||ADC_Value < pValueList[7]
          ){
          return 0;
        }
        else{
          return 1;
        }
        
      case 7:
        if(
          ADC_Value < pValueList[5]
         ||ADC_Value < pValueList[6]
         ||ADC_Value < pValueList[4]
          ){
          return 0;
        }
        else{
          return 1;
        }
    }

  }
  
  else{    
    
    switch(ID){
      //1P
      case 0:
        if(
          ADC_Value < pValueList[1]
         ||ADC_Value < pValueList[2]
          ){
          return 0;
        }
        else{
          return 1;
        }
        
      case 1:
        if(
          ADC_Value < pValueList[0]
         ||ADC_Value < pValueList[3]
          ){
          return 0;
        }
        else{
          return 1;
        }
        
      case 2:
        if(
         ADC_Value < pValueList[0]
         ||ADC_Value < pValueList[3]
          ){
          return 0;
        }
        else{
          return 1;
        }
        
      case 3:
        if(
          ADC_Value < pValueList[1]
         ||ADC_Value < pValueList[2]
          ){
          return 0;
        }
        else{
          return 1;
        }
        
        //2P
      case 4:
        if(
          ADC_Value < pValueList[5]
         ||ADC_Value < pValueList[6]
          ){
          return 0;
        }
        else{
          return 1;
        }
        
      case 5:
        if(
          ADC_Value < pValueList[4]
         ||ADC_Value < pValueList[7]
          ){
          return 0;
        }
        else{
          return 1;
        }
        
      case 6:
        if(
         ADC_Value < pValueList[4]
         ||ADC_Value < pValueList[7]
          ){
          return 0;
        }
        else{
          return 1;
        }
        
      case 7:
        if(
          ADC_Value < pValueList[5]
         ||ADC_Value < pValueList[6]
          ){
          return 0;
        }
        else{
          return 1;
        }
    }

  }
	
	return 0;
  
}


/*
原理：收到自己被触发后，等待设置时间到
在时间段内有高于自己值的就屏蔽自己
*/

void CalPxMute_V2(uint8_t ID){
	
	//查找组内最大是否为本通道
	if(CheckAdcMax(ID, (uint16_t*)InputLevel_MAX)){
		DoOutput(ID);
	}
	else{
		DoMute(ID);
	}
	
}


#define AUTO_ZERO_LEN		2048

void AutoZero(void){
	static uint32_t ADC_ADD[CHANNEL_NUM] = {0}; 
	static uint16_t Count = 0;
	 
	if(I_Buffer){
		memset(ADC_ADD, 0, sizeof(ADC_ADD));
		Count = 0;
	}
	else{
		for(uint8_t i=0; i<CHANNEL_NUM; i++){
			ADC_ADD[i] += ADC_Now[i];
		}
		Count++;
		
		if(Count >= AUTO_ZERO_LEN){
			for(uint8_t i=0; i<CHANNEL_NUM; i++){
				ADC_Zero[i] = ADC_ADD[i] / Count;
			}
			memset(ADC_ADD, 0, sizeof(ADC_ADD));
			Count = 0;
      if(POWER_ON_EN == 0){
        memset((uint8_t*)InputLevel_MAX, 0, sizeof(InputLevel_MAX));
        POWER_ON_EN = 1;
      }
		}
	}
}


 
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
  UNUSED(hadc);
  uint32_t temp;
	
	//calc adc value
	for(uint8_t i=0;i<CHANNEL_NUM;i++){

		//move list
		for(uint8_t j=0;j < (ADC_LIST_SIZE - 1);j++){
			ADC_List[i][(ADC_LIST_SIZE - 1) - j] = ADC_List[i][(ADC_LIST_SIZE - 1) - j - 1];
		}
	
		//cal avg
		temp = 0;
		for(uint8_t j=0;j<ADC_BUFFER_SIZE;j++){
			temp += ADC_Buffer[j][SystemSet.ADC_ChannelRemap[i]];
		}
		ADC_Now[i] = (uint16_t)(temp * SystemSet.ADC_Mul[i] / ADC_BUFFER_SIZE / ADC_DIV_DEF);
		
		//Set Zero
		if(ADC_Zero[i] < ADC_Now[i])
			ADC_List[i][0] = ADC_Now[i] - ADC_Zero[i];
		else
			ADC_List[i][0] = 0;
		InputLevel_Now[i] = ADC_List[i][0];
		
	}
  
	
	//cale trigger
  if(SystemSet.MuteEnable == DISABLE){
		//without mute func
    
    for(uint8_t i=0;i<CHANNEL_NUM;i++){
      
      //cal trigger
      for(uint8_t j=0;j<(ADC_LIST_SIZE - 1);j++){
        if(ADC_List[i][0] > (ADC_List[i][j+1] + SystemSet.ADC_Trigger[i])){
          
          //REG OUTPUT
          I_Buffer |= 1<<i;
          if(((O_Buffer>>i)&1) == 0){
						UartDebug();
            O_CountDown[i] = SystemSet.OutHiLength + SystemSet.OutLoLength;
            O_Buffer |= 1 << i;
          }
          
          break;
        }
      }
    }
		
  }
	
	else if(SystemSet.ADC_WaitMuteTime == 0){
		//mute and time first
		uint16_t max_dt = 0;
		uint8_t max_id = 0;
		
		if(O_Buffer == 0){
			for(uint8_t i=0;i<CHANNEL_NUM;i++){
				for(uint8_t j=0;j<(ADC_LIST_SIZE - 1);j++){
					if(ADC_List[i][0] > ADC_List[i][j+1]){
						if(ADC_List[i][0] > (ADC_List[i][j+1] + SystemSet.ADC_Trigger[i])){	//do trigger
							
							if(max_dt < ADC_List[i][0] - ADC_List[i][j+1]){
								max_dt = ADC_List[i][0] - ADC_List[i][j+1];
								max_id = i;
							}
						}
					}
				}
			}
		}
		if(max_dt != 0){
			DoOutput(max_id);
		}
		
		//end of time first
	}
	
	else{
		//With Mute
		
		for(uint8_t i=0;i<CHANNEL_NUM;i++){
			//cal trigger
			if((I_Buffer>>i) & 1){	//This channel has been trigged , GetMax
					REG_Output(i, ADC_List[i][0]);
			}
			else{
				//寻找高脉冲边缘
				for(uint8_t j=0;j<(ADC_LIST_SIZE - 1);j++){
					
					if(ADC_List[i][0] > ADC_List[i][j+1]){
						if(ADC_List[i][0] > (ADC_List[i][j+1] + SystemSet.ADC_Trigger[i])){//do trigger
							
							//REG OUTPUT
							if(CheckAdcMax(i,(uint16_t*)InputLevel_Now))
								if(((O_Buffer>>i) & 1) == 0)  //This channel no output Now
									REG_Output(i, ADC_List[i][0]);//GetMax
								
						}
					}
					
				}
			}
		}
		
		//calculate output
		for(uint8_t i=0;i<CHANNEL_NUM;i++){
			if((I_Buffer >> i) & 1){
				InputCount[i]++;
				if(InputCount[i] >= SystemSet.ADC_WaitMuteTime){
					//time out
					CalPxMute_V2(i);
				}
			}
		}
		
		//end of mute
		
	}
	
	AutoZero();
	
  
}

#ifndef IS_4CH_MODE
 
void ReadSet(void){
  SET_1 = HAL_GPIO_ReadPin(SET1_GPIO_Port,SET1_Pin) == 0; //Debug EN
  SET_2 = HAL_GPIO_ReadPin(SET2_GPIO_Port,SET2_Pin) == 0; //load Set = Default
  SET_3 = HAL_GPIO_ReadPin(SET3_GPIO_Port,SET3_Pin) == 0; //Test IO
  SET_4 = HAL_GPIO_ReadPin(SET4_GPIO_Port,SET4_Pin) == 0; //timebase 2X
}

#endif



uint8_t SciBuffer[200];
uint8_t SciLength = 0;
uint8_t SciCount = 0;
uint8_t SciTime = 0;

void UsartStartRec(void){
  HAL_UART_AbortReceive_IT(&huart1);
  HAL_UART_Receive_IT(&huart1, SciBuffer, sizeof(SciBuffer));
}


uint8_t SciCheck(uint8_t* data, uint8_t len){
  if(len < 3)return 1;
  uint16_t CRCData1;
  uint16_t CRCData2;
  
  CRCData1 = CRCCheck(data, len-2);
  CRCData2 = data[len-1] | (data[len-2]<<8);
  
  if(CRCData1 == CRCData2)return 0;
  return 1;
}



void GetSciData(UART_HandleTypeDef *huart){
  
  SciCount++;
  SciLength = huart->RxXferSize - huart->RxXferCount;
  
  if(0 == SciCheck(SciBuffer,SciLength)){
    //check data cplt
    //do cmd
    SciDoCMD(SciBuffer, SciLength);
  }
  
  UsartStartRec();
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  GetSciData(huart);
}


uint8_t CloseIrqAndSave(void){
  uint8_t temp;
	
  HAL_TIM_Base_Stop_IT(&htim14);
  HAL_ADC_Stop_DMA(&hadc1);
	HAL_Delay(5);
	
  temp = DoSave();
	
  HAL_TIM_Base_Start_IT(&htim14);
  HAL_ADC_Start_DMA(&hadc1,(uint32_t *)ADC_Buffer,ADC_BUFFER_SIZE*ADC_CHANNEL_NUM);
	
  return temp;
}

//改为软件断帧
#define UART_FREAM_TIME		2

void UsartTask(void){
  static uint16_t RxXferCount_buf = 0xFF;
  
  if(huart1.RxXferCount != huart1.RxXferSize){
    
    if(RxXferCount_buf != huart1.RxXferCount){
      RxXferCount_buf = huart1.RxXferCount;
      SciTime = 0;
    }
    else{
      SciTime ++;
      if(SciTime > UART_FREAM_TIME){
        GetSciData(&huart1);
        SciTime = 0;
        RxXferCount_buf = 0xFF;
      }
    }
  }
  
}


//开机动画
void ON_Play(void){
  for(uint8_t i = 0; i < CHANNEL_NUM; i ++){
    GPIO_Output(i, 1);
    HAL_Delay(30);
  }
  for(uint8_t i = 0; i < CHANNEL_NUM; i ++){
    GPIO_Output(i, 0);
    HAL_Delay(30);
  }
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  uint16_t time = 0;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_IWDG_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_TIM14_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  
  HAL_FLASH_Lock();
	
  HAL_Delay(10);
	
#ifdef IS_4CH_MODE

  SaveInit();
	
#else

  ReadSet();
  
  if(SET_4){
    OutListFerq = 2000;
    MX_TIM14_Init();
  }
  
  if(SET_2){
    SystemSet = SystemSetDefault;
  }
  else{
    SaveInit();
  }
	
#endif

	
#ifndef IS_4CH_MODE
	
	if(SET_3)
	{
		SystemSet = SystemSetTest;
		SystemTest = 1;
	}
	
#endif
	
  HAL_IWDG_Refresh(&hiwdg);
  ON_Play();
	
	HAL_ADCEx_Calibration_Start(&hadc1);
  HAL_TIM_Base_Start_IT(&htim14);
  HAL_ADC_Start_DMA(&hadc1,(uint32_t *)ADC_Buffer,ADC_BUFFER_SIZE*ADC_CHANNEL_NUM);
  HAL_Delay(500);

#ifndef IS_4CH_MODE
  if(SET_1)
#endif
	{
    HAL_UART_Transmit_DMA(&huart1, (uint8_t*)"Power On\r\n",sizeof("Power On\r\n")-1);
    UsartStartRec();
  }
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    
    HAL_IWDG_Refresh(&hiwdg);
    UsartTask();
    
    time++;
    if(time >= 100){
      time = 0;
      if(!SystemError && POWER_ON_EN){
				//开机自检成功闪灯
        HAL_GPIO_TogglePin(LED_STA_GPIO_Port,LED_STA_Pin);
      }
      else{
        HAL_GPIO_WritePin(LED_STA_GPIO_Port,LED_STA_Pin,GPIO_PIN_SET);
      }
    }
    
    HAL_Delay(1);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the peripherals clocks
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_ADC;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_SYSCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.LowPowerAutoPowerOff = DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.NbrOfConversion = 8;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_160CYCLES_5;
  hadc1.Init.SamplingTimeCommon2 = ADC_SAMPLETIME_160CYCLES_5;
  hadc1.Init.OversamplingMode = DISABLE;
  hadc1.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_HIGH;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = ADC_REGULAR_RANK_3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = ADC_REGULAR_RANK_4;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = ADC_REGULAR_RANK_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = ADC_REGULAR_RANK_6;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_6;
  sConfig.Rank = ADC_REGULAR_RANK_7;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_7;
  sConfig.Rank = ADC_REGULAR_RANK_8;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief IWDG Initialization Function
  * @param None
  * @retval None
  */
static void MX_IWDG_Init(void)
{

  /* USER CODE BEGIN IWDG_Init 0 */

  /* USER CODE END IWDG_Init 0 */

  /* USER CODE BEGIN IWDG_Init 1 */

  /* USER CODE END IWDG_Init 1 */
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_128;
  hiwdg.Init.Window = 4095;
  hiwdg.Init.Reload = 4095;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IWDG_Init 2 */

  /* USER CODE END IWDG_Init 2 */

}

/**
  * @brief TIM14 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM14_Init(void)
{

  /* USER CODE BEGIN TIM14_Init 0 */

  /* USER CODE END TIM14_Init 0 */

  /* USER CODE BEGIN TIM14_Init 1 */

  /* USER CODE END TIM14_Init 1 */
  htim14.Instance = TIM14;
  htim14.Init.Prescaler = 32 - 1;
  htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim14.Init.Period = 2 * OutListFerq - 1;
  htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim14.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim14) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM14_Init 2 */
//  htim14.Init.Period = 2 * OutListFerq - 1;
  /* USER CODE END TIM14_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 38400;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  /* DMA1_Channel2_3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);

}

/* USER CODE BEGIN 4 */

#ifndef IS_4CH_MODE

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, OUT1_Pin|OUT8_Pin|OUT7_Pin|OUT6_Pin
                          |OUT5_Pin|OUT4_Pin|OUT3_Pin|OUT2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_STA_GPIO_Port, LED_STA_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : OUT1_Pin OUT8_Pin OUT7_Pin OUT6_Pin
                           OUT5_Pin OUT4_Pin OUT3_Pin OUT2_Pin */
  GPIO_InitStruct.Pin = OUT1_Pin|OUT8_Pin|OUT7_Pin|OUT6_Pin
                          |OUT5_Pin|OUT4_Pin|OUT3_Pin|OUT2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : SET3_Pin SET4_Pin */
  GPIO_InitStruct.Pin = SET3_Pin|SET4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : SET2_Pin SET1_Pin */
  GPIO_InitStruct.Pin = SET2_Pin|SET1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : LED_STA_Pin */
  GPIO_InitStruct.Pin = LED_STA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_STA_GPIO_Port, &GPIO_InitStruct);

}

#else

static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, OUT2_Pin|OUT3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED_STA_Pin|OUT1_Pin|OUT4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : OUT2_Pin OUT3_Pin */
  GPIO_InitStruct.Pin = OUT2_Pin|OUT3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_STA_Pin OUT1_Pin OUT4_Pin */
  GPIO_InitStruct.Pin = LED_STA_Pin|OUT1_Pin|OUT4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

#endif


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
