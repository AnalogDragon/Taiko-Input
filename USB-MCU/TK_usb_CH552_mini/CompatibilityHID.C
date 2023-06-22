#include "./Public/CH552.H"
#include "./Public/Debug.H"
#include <stdio.h>
#include <string.h>
#include "GPIO.h"

#define VID 0x0f0d  //used HORI vid & pid
#define PID 0x00f0

#define VID1 0xDEAD
#define PID1 0xFF05


#define THIS_ENDP0_SIZE MAX_PACKET_SIZE
#define KEY_SIZE 8

#define Keyboard_a 4  // Keyboard a and A
#define Keyboard_b 5  // Keyboard b and B
#define Keyboard_c 6  // Keyboard c and C
#define Keyboard_d 7  // Keyboard d and D
#define Keyboard_e 8  // Keyboard e and E
#define Keyboard_f 9  // Keyboard f and F
#define Keyboard_g 10 // Keyboard g and G
#define Keyboard_h 11 // Keyboard h and H
#define Keyboard_i 12 // Keyboard i and I
#define Keyboard_j 13 // Keyboard j and J
#define Keyboard_k 14 // Keyboard k and K
#define Keyboard_l 15 // Keyboard l and L
#define Keyboard_m 16 // Keyboard m and M
#define Keyboard_n 17 // Keyboard n and N
#define Keyboard_o 18 // Keyboard o and O
#define Keyboard_p 19 // Keyboard p and P
#define Keyboard_q 20 // Keyboard q and Q
#define Keyboard_r 21 // Keyboard r and R
#define Keyboard_s 22 // Keyboard s and S
#define Keyboard_t 23 // Keyboard t and T
#define Keyboard_u 24 // Keyboard u and U
#define Keyboard_v 25 // Keyboard v and V
#define Keyboard_w 26 // Keyboard w and W
#define Keyboard_x 27 // Keyboard x and X
#define Keyboard_y 28 // Keyboard y and Y
#define Keyboard_z 29 // Keyboard z and Z

#define Keyboard_OEM3 53 // Keyboard ` and ~
#define Keyboard_1 30    // Keyboard 1 and !
#define Keyboard_2 31    // Keyboard 2 and @
#define Keyboard_3 32    // Keyboard 3 and #
#define Keyboard_4 33    // Keyboard 4 and $
#define Keyboard_5 34    // Keyboard 5 and %
#define Keyboard_6 35    // Keyboard 6 and ^
#define Keyboard_7 36    // Keyboard 7 and &
#define Keyboard_8 37    // Keyboard 8 and *
#define Keyboard_9 38    // Keyboard 9 and (
#define Keyboard_0 39    // Keyboard 0 and )

#define Keyboard_ENTER 40     // Keyboard ENTER
#define Keyboard_ESCAPE 41    // Keyboard ESCAPE
#define Keyboard_Backspace 42 // Keyboard Backspace
#define Keyboard_Tab 43       // Keyboard Tab
#define Keyboard_Space 44     // Keyboard Spacebar
#define Keyboard_CapsLock 57  // Keyboard Caps Lock

#define Keyboard_MINUS 45         // Keyboard - and _
#define Keyboard_PLUS 46          // Keyboard = and +
#define Keyboard_LeftBrackets 47  // Keyboard [ and {
#define Keyboard_RightBrackets 48 // Keyboard ] and }
#define Keyboard_OEM5 49          // Keyboard \ and |
#define Keyboard_OEM1 51          // Keyboard ; and :
#define Keyboard_OEM7 52          // Keyboard ‘ and “
#define Keyboard_COMMA 54         // Keyboard, and <
#define Keyboard_PERIOD 55        // Keyboard . and >
#define Keyboard_DIV 56           // Keyboard / and ?

#define Keyboard_F1 58  // Keyboard F1
#define Keyboard_F2 59  // Keyboard F2
#define Keyboard_F3 60  // Keyboard F3
#define Keyboard_F4 61  // Keyboard F4
#define Keyboard_F5 62  // Keyboard F5
#define Keyboard_F6 63  // Keyboard F6
#define Keyboard_F7 64  // Keyboard F7
#define Keyboard_F8 65  // Keyboard F8
#define Keyboard_F9 66  // Keyboard F9
#define Keyboard_F10 67 // Keyboard F10
#define Keyboard_F11 68 // Keyboard F11
#define Keyboard_F12 69 // Keyboard F12

#define Keyboard_PrintScreen 70 // Keyboard PrintScreen
#define Keyboard_ScrollLock 71  // Keyboard Scroll Lock
#define Keyboard_Pause 72       // Keyboard Pause

#define Keyboard_Insert 73   // Keyboard Insert
#define Keyboard_Home 74     // Keyboard Home
#define Keyboard_PageUp 75   // Keyboard PageUp
#define Keyboard_Delete 76   // Keyboard Delete
#define Keyboard_End 77      // Keyboard End
#define Keyboard_PageDown 78 // Keyboard PageDown

#define Keyboard_RightArrow 79 // Keyboard RightArrow
#define Keyboard_LeftArrow 80  // Keyboard LeftArrow
#define Keyboard_DownArrow 81  // Keyboard DownArrow
#define Keyboard_UpArrow 82    // Keyboard UpArrow

#define Keypad_NumLock 83 // Keypad Num Lock and Clear
#define Keypad_DIV 84     // Keypad /
#define Keypad_MULT 85    // Keypad *
#define Keypad_SUB 86     // Keypad -
#define Keypad_ADD 87     // Keypad +
#define Keypad_ENTER 88   // Keypad ENTER
#define Keypad_1 89       // Keypad 1 and End
#define Keypad_2 90       // Keypad 2 and Down Arrow
#define Keypad_3 91       // Keypad 3 and PageDn
#define Keypad_4 92       // Keypad 4 and Left Arrow
#define Keypad_5 93       // Keypad 5
#define Keypad_6 94       // Keypad 6 and Right Arrow
#define Keypad_7 95       // Keypad 7 and Home
#define Keypad_8 96       // Keypad 8 and Up Arrow
#define Keypad_9 97       // Keypad 9 and PageUp
#define Keypad_0 98       // Keypad 0 and Insert
#define Keypad_DEC 99     // Keypad . and Delete

UINT8X Ep0Buffer[(MAX_PACKET_SIZE)*3 + 2] _at_ 0x0000;		//端点0 OUT&IN缓冲区，必须是偶地址
UINT8X Ep2Buffer[(MAX_PACKET_SIZE)*2 + 2] _at_ 0x00C4;		//端点2 IN&OUT缓冲区,必须是偶地址

UINT8   SetupReq,SetupLen,Ready,HID_Ready,Count,UsbConfig;
UINT16 shutdown_count = 0;

bit STA_NumLock = 0;
bit STA_CapLock = 0;
bit STA_ScrLock = 0;
PUINT8  pDescr;                                                                    //USB配置标志
USB_SETUP_REQ   SetupReqBuf;                                                       //暂存Setup包
#define UsbSetupBuf     ((PUSB_SETUP_REQ)Ep0Buffer)  

volatile UINT8  KeyCountTime_A[4] = {0};
volatile UINT8  KeyCountTime_B[4] = {0};

#define MODE_NS_GP 		0
#define MODE_KEYBOARD 	1
#define MODE_KB_LMT  	2
#define MODE_STARTUP  	MODE_KEYBOARD

UINT8 start_up_mode = MODE_STARTUP;

volatile UINT16 KEY_OUT = 0;

#pragma  NOAREGS
/*设备描述符*/
UINT8C DevDesc_Gamepad[18] = {0x12,0x01,0x00,0x02,0x00,0x00,0x00,THIS_ENDP0_SIZE,
                      VID,VID>>8,PID,PID>>8,0x00,0x02,0x01,0x02,
                      0x03,0x01};

UINT8C DevDesc_Keyboard[18] = {0x12,0x01,0x00,0x02,0x00,0x00,0x00,THIS_ENDP0_SIZE,
                      VID1,VID1>>8,PID1,PID1>>8,0x00,0x02,0x01,0x02,
                      0x03,0x01};

UINT8C HIDRepDesc_Gamepad[80] =
{
0x05, 0x01,  //Usage Page (Generic Desktop)
0x09, 0x05,  //Usage (Game Pad)
0xA1, 0x01,  //Collection (Application)	
0x15, 0x00,  //    Logical Minimum (0)	
0x25, 0x01,  //    Logical Maximum (1)	
0x35, 0x00,  //    Physical Minimum (0)	
0x45, 0x01,  //    Physical Maximum (1)	
0x75, 0x01,  //    Report Size (1)	
0x95, 0x0E,  //    Report Count (14)	
0x05, 0x09,  //    Usage Page (Button)	
0x19, 0x01,  //    Usage Minimum (Button 1)	
0x29, 0x0E,  //    Usage Maximum (Button 14)	
0x81, 0x02,  //    Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)	
0x95, 0x02,  //    Report Count (2)	
0x81, 0x01,  //    Input (Cnst,Ary,Abs)	
0x05, 0x01,  //    Usage Page (Generic Desktop)	
0x25, 0x07,  //    Logical Maximum (7)	
0x46, 0x3B, 0x01,  //    Physical Maximum (315)	
0x75, 0x04,  //    Report Size (4)	
0x95, 0x01,  //    Report Count (1)	
0x65, 0x14,  //    Unit (Eng Rot: Degree)	
0x09, 0x39,  //    Usage (Hat Switch)	
0x81, 0x42,  //    Input (Data,Var,Abs,NWrp,Lin,Pref,Null,Bit)	
0x65, 0x00,  //    Unit (None)	
0x95, 0x01,  //    Report Count (1)	
0x81, 0x01,  //    Input (Cnst,Ary,Abs)	
0x26, 0xFF, 0x00,  //    Logical Maximum (255)	
0x46, 0xFF, 0x00,  //    Physical Maximum (255)	
0x09, 0x30,  //    Usage (X)	
0x09, 0x31,  //    Usage (Y)	
0x09, 0x32,  //    Usage (Z)	
0x09, 0x35,  //    Usage (3)	
0x75, 0x08,  //    Report Size (8)	
0x95, 0x04,  //    Report Count (4)	
0x81, 0x02,  //    Input (Data,Var,Abs,NWrp,Lin,Pref,NNul,Bit)	
0x75, 0x08,  //    Report Size (8)	
0x95, 0x01,  //    Report Count (1)
0x81, 0x01,  //    Input (Cnst,Ary,Abs)
0xC0,  //End Collection	

};

UINT8C HIDRepDesc_Keyboard[63] = {
    0x05,
    0x01, // USAGE_PAGE (Generic Desktop)
    0x09,
    0x06, // USAGE (Keyboard)
    0xa1,
    0x01, // COLLECTION (Application)
    //
    0x75,
    0x01, // REPORT_SIZE (1)
    0x95,
    0x08, // REPORT_COUNT (8)

    0x05,
    0x07, // USAGE_PAGE (Keyboard)
    0x19,
    0xE0, // USAGE_MINIMUM (Keyboard LeftControl)
    0x29,
    0xE7, // USAGE_MAXIMUM (Keyboard Right GUI)
    0x15,
    0x00, // LOGICAL_MINIMUM (0)
    0x25,
    0x01, // LOGICAL_MAXIMUM (1)

    0x81,
    0x02, // INPUT (Data,Var,Abs)
    //
    0x75,
    0x08, // REPORT_SIZE (8)
    0x95,
    0x01, // REPORT_COUNT (1)
    0x81,
    0x03, // INPUT (Cnst,Var,Abs)
    //
    0x75,
    0x08, // REPORT_SIZE (8)
    0x95,
    KEY_SIZE, // REPORT_COUNT (16)

    0x05,
    0x07, // USAGE_PAGE (Keyboard)
    0x19,
    0x00, // USAGE_MINIMUM (Reserved (no event indicated))
    0x29,
    0x65, // USAGE_MAXIMUM (Keyboard Application)
    0x15,
    0x00, // LOGICAL_MINIMUM (0)
    0x25,
    0xFF, // LOGICAL_MAXIMUM (255)

    0x81,
    0x00, // INPUT (Data,Ary,Abs)
    //
    0x75,
    0x01, // REPORT_SIZE (1)
    0x95,
    0x05, // REPORT_COUNT (5)

    0x05,
    0x08, // USAGE_PAGE (LEDs)
    0x19,
    0x01, // USAGE_MINIMUM (Num Lock)
    0x29,
    0x05, // USAGE_MAXIMUM (Kana)

    0x91,
    0x02, // OUTPUT (Data,Var,Abs)
    //
    0x75,
    0x03, // REPORT_SIZE (3)
    0x95,
    0x01, // REPORT_COUNT (1)

    0x91,
    0x03, // OUTPUT (Cnst,Var,Abs)	//填充位
    //
    0xc0,

};

UINT8C CfgDesc_Gamepad[34] =
{
	0x09, /* bLength: Configuration Descriptor size */
	0x02, /* bDescriptorType: Configuration */
	sizeof(CfgDesc_Gamepad),
	/* wTotalLength: Bytes returned */
	0x00,
	0x01,         /*bNumInterfaces: 1 interface*/
	0x01,         /*bConfigurationValue: Configuration value*/
	0x00,         /*iConfiguration: Index of string descriptor describing
	the configuration*/
	0x80,         /*bmAttributes: bus powered and Support Remote Wake-up */
	0x32,         /*MaxPower 100 mA: this current is used for detecting Vbus*/
	
	/************** Descriptor of Joystick Mouse interface ****************/
	/* 09 */
	0x09,         /*bLength: Interface Descriptor size*/
	0x04,/*bDescriptorType: Interface descriptor type*/
	0x00,         /*bInterfaceNumber: Number of Interface*/
	0x00,         /*bAlternateSetting: Alternate setting*/
	0x01,         /*bNumEndpoints*/
	0x03,         /*bInterfaceClass: HID*/
	0x00,         /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
	0x00,         /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
	0,            /*iInterface: Index of string descriptor*/
	/******************** Descriptor of Joystick Mouse HID ********************/
	/* 18 */
	0x09,         /*bLength: HID Descriptor size*/
	0x21, /*bDescriptorType: HID*/
	0x00,         /*bcdHID: HID Class Spec release number*/
	0x01,
	0x00,         /*bCountryCode: Hardware target country*/
	0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
	0x22,         /*bDescriptorType*/
	sizeof(HIDRepDesc_Gamepad),/*wItemLength: Total length of Report descriptor*/
	0x00,
	/******************** Descriptor of Mouse endpoint ********************/
	/* 27 */
	0x07,          /*bLength: Endpoint Descriptor size*/
	0x05, /*bDescriptorType:*/
	
	0x82,     /*bEndpointAddress: Endpoint Address (IN)*/
	0x03,          /*bmAttributes: Interrupt endpoint*/
	0x40, /*wMaxPacketSize: 4 Byte max */
	0x00,
	8,          /*bInterval: Polling Interval */
};

UINT8C CfgDesc_Keyboard[34] =
{
    0x09, /* bLength: Configuration Descriptor size */
    0x02, /* bDescriptorType: Configuration */
    sizeof(CfgDesc_Keyboard),
    /* wTotalLength: Bytes returned */
    0x00, 0x01, /*bNumInterfaces: 1 interface*/
    0x01,       /*bConfigurationValue: Configuration value*/
    0x00,       /*iConfiguration: Index of string descriptor describing the configuration*/
    0xE0,       /*bmAttributes: bus powered and Support Remote Wake-up */
    0x32,       /*MaxPower 100 mA: this current is used for detecting Vbus*/

    /************** Descriptor of Joystick Mouse interface ****************/
    /* 09 */
    0x09, /*bLength: Interface Descriptor size*/
    0x04, /*bDescriptorType: Interface descriptor type*/
    0x00, /*bInterfaceNumber: Number of Interface*/
    0x00, /*bAlternateSetting: Alternate setting*/
    0x01, /*bNumEndpoints*/
    0x03, /*bInterfaceClass: HID*/
    0x01, /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
    0x01, /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
    0,    /*iInterface: Index of string descriptor*/
    /******************** Descriptor of Joystick Mouse HID ********************/
    /* 18 */
    0x09,               /*bLength: HID Descriptor size*/
    0x21,               /*bDescriptorType: HID*/
    0x11,               /*bcdHID: HID Class Spec release number*/
    0x01, 0x00,         /*bCountryCode: Hardware target country*/
    0x01,               /*bNumDescriptors: Number of HID class descriptors to follow*/
    0x22,               /*bDescriptorType*/
    sizeof(HIDRepDesc_Keyboard), /*wItemLength: Total length of Report descriptor*/
    0x00,
    /******************** Descriptor of Mouse endpoint ********************/
    /* 27 */
    0x07, /*bLength: Endpoint Descriptor size*/
    0x05, /*bDescriptorType:*/

    0x82,    /*bEndpointAddress: Endpoint Address (IN)*/
    0x03,    /*bmAttributes: Interrupt endpoint*/
    0x40,    /*wMaxPacketSize: 4 Byte max */
    0x00, 1, /*bInterval: Polling Interval */
};


UINT8C LangDes[]={0x04,0x03,0x09,0x04}; //语言描述符

UINT8C SerDes1[13] = "Analog Dragon";

UINT8C SerDes2_DS_GP[21] = "Fake Taiko Controller";
UINT8C SerDes2_Keyboard[20] = "Taiko Input Keyboard";
UINT8C SerDes2_KB_LMT[24] = "Taiko Input Keyboard LMT";

UINT8 SerDes3[15] = "0000000000_TKIO"; 

UINT8X UserBuf[64];
UINT8 EndInBusy = 0;

UINT8 SerDes[64] = {0};

typedef struct{
  UINT8C* DevDesc;
  UINT8 DevDesc_size;
  UINT8C* HIDRepDesc;
  UINT8 HIDRepDesc_size;
  UINT8C* CfgDesc;
  UINT8 CfgDesc_size;
}StrDef;

const StrDef USB_Str[4]={
{
  DevDesc_Gamepad,
  sizeof(DevDesc_Gamepad),
  HIDRepDesc_Gamepad,
  sizeof(HIDRepDesc_Gamepad),
  CfgDesc_Gamepad,
  sizeof(CfgDesc_Gamepad),
},
{
  DevDesc_Keyboard,
  sizeof(DevDesc_Keyboard),
  HIDRepDesc_Keyboard,
  sizeof(HIDRepDesc_Keyboard),
  CfgDesc_Keyboard,
  sizeof(CfgDesc_Keyboard),
},
{
  DevDesc_Keyboard,
  sizeof(DevDesc_Keyboard),
  HIDRepDesc_Keyboard,
  sizeof(HIDRepDesc_Keyboard),
  CfgDesc_Keyboard,
  sizeof(CfgDesc_Keyboard),
},
};

UINT8 GetSerDes(UINT8* str,UINT8 len){
  UINT8 i;
  if(len > 30)len = 30;
  
  memset(SerDes,0,sizeof(SerDes));
  SerDes[0] = len * 2 + 2;
  SerDes[1] = 0x03;
  for(i=0;i<len;i++){
    SerDes[2 + i*2] = str[i];
  }
  return SerDes[0];
}


void GetID(void){
  UINT8 i = 0;
  const UINT8C trsTable[16] = "0123456789ABCDEF";
  UINT8C* ID;
  UINT16 IDA;
  
  IDA = 0x3FFF;
  ID = (UINT8*)IDA;
  SerDes3[i++] = trsTable[*ID & 0x0F];
  SerDes3[i++] = trsTable[(*ID >> 4) & 0x0F];
  
  IDA = 0x3FFE;
  ID = (UINT8*)IDA;
  SerDes3[i++] = trsTable[*ID & 0x0F];
  SerDes3[i++] = trsTable[(*ID >> 4) & 0x0F];
  
  IDA = 0x3FFD;
  ID = (UINT8*)IDA;
  SerDes3[i++] = trsTable[*ID & 0x0F];
  SerDes3[i++] = trsTable[(*ID >> 4) & 0x0F];
  
  IDA = 0x3FFC;
  ID = (UINT8*)IDA;
  SerDes3[i++] = trsTable[*ID & 0x0F];
  SerDes3[i++] = trsTable[(*ID >> 4) & 0x0F];
  
  IDA = 0x3FFA;
  ID = (UINT8*)IDA;
  SerDes3[i++] = trsTable[*ID & 0x0F];
  SerDes3[i++] = trsTable[(*ID >> 4) & 0x0F];
  
}

/*******************************************************************************
* Function Name  : USBDeviceInit()
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USBDeviceInit()
{
	IE_USB = 0;
	USB_CTRL = 0x00;                                                           // 先设定USB设备模式
	UDEV_CTRL = bUD_PD_DIS;                                                    // 禁止DP/DM下拉电阻	

	UDEV_CTRL &= ~bUD_LOW_SPEED;                                               //选择全速12M模式，默认方式
	USB_CTRL &= ~bUC_LOW_SPEED;

	UEP2_DMA = Ep2Buffer;                                                      //端点2数据传输地址
	UEP2_3_MOD |= bUEP2_TX_EN | bUEP2_RX_EN;                                   //端点2发送接收使能
	UEP2_3_MOD &= ~bUEP2_BUF_MOD;                                              //端点2收发各64字节缓冲区
	UEP2_CTRL = bUEP_AUTO_TOG | UEP_T_RES_NAK | UEP_R_RES_ACK;                 //端点2自动翻转同步标志位，IN事务返回NAK，OUT返回ACK
	UEP0_DMA = Ep0Buffer;                                                      //端点0数据传输地址
	UEP4_1_MOD &= ~(bUEP4_RX_EN | bUEP4_TX_EN);                                //端点0单64字节收发缓冲区
	UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;                                 //OUT事务返回ACK，IN事务返回NAK
		
	USB_DEV_AD = 0x00;
	USB_CTRL |= bUC_DEV_PU_EN | bUC_INT_BUSY | bUC_DMA_EN;                     // 启动USB设备及DMA，在中断期间中断标志未清除前自动返回NAK
	UDEV_CTRL |= bUD_PORT_EN;                                                  // 允许USB端口
	USB_INT_FG = 0xFF;                                                         // 清中断标志
	USB_INT_EN = bUIE_SUSPEND | bUIE_TRANSFER | bUIE_BUS_RST;    // 挂起中断
	IE_USB = 1;
}


UINT8 COUNT_NUM_D = 4;
UINT8 COUNT_NUM_K = 2;

volatile UINT8 input_count[4] = {0};


void get_input(UINT16 DATA_){
	static UINT8 back = 0;
	UINT8 temp = DATA_ & 0x0F;
	UINT8 out = (temp ^ back) & temp;
	back = temp;
	
	if((out & 0x01) && (input_count[0] < COUNT_NUM_K))input_count[0]++;
	if((out & 0x02) && (input_count[1] < COUNT_NUM_D))input_count[1]++;
	if((out & 0x04) && (input_count[2] < COUNT_NUM_D))input_count[2]++;
	if((out & 0x08) && (input_count[3] < COUNT_NUM_K))input_count[3]++;
}


/*----------------------------keyboard limit------------------------*/
bit player_num_flag = 0;
//虹专用键盘
#define KB_LMT_COUNT	40
#define KB_LMT_TRIG		20

UINT8 kb_lmt_count[4] = {0};

void kb_lmt_get(UINT16 DATA_){
	//咚交叉，咔不交叉
	//1 = LK
	if(input_count[0] && kb_lmt_count[0] == 0){
		input_count[0]--;
		kb_lmt_count[0] = KB_LMT_COUNT;
		if(kb_lmt_count[3] == 0)kb_lmt_count[3] = KB_LMT_TRIG;
	}
	//4 = RK
	if(input_count[3] && kb_lmt_count[3] == 0){
		input_count[3]--;
		kb_lmt_count[3] = KB_LMT_COUNT;
		if(kb_lmt_count[0] == 0)kb_lmt_count[0] = KB_LMT_TRIG;
	}
	//2 = LD
	if(input_count[1] && kb_lmt_count[1] == 0){
		input_count[1]--;
		kb_lmt_count[1] = KB_LMT_COUNT;
		if(kb_lmt_count[2] == 0)kb_lmt_count[2] = KB_LMT_TRIG;
	}
	//3 = RD
	if(input_count[2] && kb_lmt_count[2] == 0){
		input_count[2]--;
		kb_lmt_count[2] = KB_LMT_COUNT;
		if(kb_lmt_count[1] == 0)kb_lmt_count[1] = KB_LMT_TRIG;
	}
	//
	if(input_count[1] && kb_lmt_count[2] == 0){
		input_count[1]--;
		kb_lmt_count[2] = KB_LMT_COUNT;
		if(kb_lmt_count[1] == 0)kb_lmt_count[1] = KB_LMT_TRIG;
	}
	if(input_count[2] && kb_lmt_count[1] == 0){
		input_count[2]--;
		kb_lmt_count[1] = KB_LMT_COUNT;
		if(kb_lmt_count[2] == 0)kb_lmt_count[2] = KB_LMT_TRIG;
	}
	
	//输出
	KEY_OUT = DATA_ & 0xFF00;
	if(kb_lmt_count[0] > KB_LMT_TRIG)KEY_OUT |= 0x01;
	if(kb_lmt_count[1] > KB_LMT_TRIG)KEY_OUT |= 0x02;
	if(kb_lmt_count[2] > KB_LMT_TRIG)KEY_OUT |= 0x04;
	if(kb_lmt_count[3] > KB_LMT_TRIG)KEY_OUT |= 0x08;

	
	//锁定输出
	if(kb_lmt_count[0])kb_lmt_count[0]--;
	if(kb_lmt_count[1])kb_lmt_count[1]--;
	if(kb_lmt_count[2])kb_lmt_count[2]--;
	if(kb_lmt_count[3])kb_lmt_count[3]--;
}

void kb_lmt_do(UINT16 DATA_){
	UINT8C InputData_1[8] = {
        Keyboard_d, Keyboard_f, Keyboard_j, Keyboard_k, Keyboard_1, Keyboard_2, Keyboard_3, Keyboard_4,
    };
	UINT8C InputData_2[8] = {
        Keyboard_z, Keyboard_x, Keyboard_c, Keyboard_v, Keyboard_1, Keyboard_2, Keyboard_3, Keyboard_4,
    };
    UINT8 i,temp;
    
    temp = DATA_ & 0x0F;

    switch(DATA_ & 0xFF00){
        case 0x0100:temp |= 0x10;player_num_flag = 0;break;
        case 0x0200:temp |= 0x20;player_num_flag = 1;break;
        case 0x0300:temp |= 0x40;break;
        case 0x0400:temp |= 0x80;break;
    }
	
	if(player_num_flag){
		for (i = 0; i < 8; i++)
		{
			if (temp & (1 << i))
				UserBuf[2 + i] = InputData_2[i];
			else
				UserBuf[2 + i] = 0;
		}
	}
	else{
		for (i = 0; i < 8; i++)
		{
			if (temp & (1 << i))
				UserBuf[2 + i] = InputData_1[i];
			else
				UserBuf[2 + i] = 0;
		}
	}

    memcpy(Ep2Buffer + MAX_PACKET_SIZE, UserBuf, sizeof(UserBuf));
    UEP2_T_LEN = KEY_SIZE + 2;
    UEP2_CTRL = UEP2_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK;
}
/*------------------------------------------------------------------*/



/*----------------------------keyboard------------------------------*/
//普通键盘
#define KEYBOARD_COUNT_DOWN_FULL 	20
#define KEYBOARD_COUNT_DOWN_MIN		5
#define KEYBOARD_COUNT_DOWN_TRIG	2

UINT8 keyboard_count[4] = {0};


void keyboard_get(UINT16 DATA_){
	UINT8 i;
	
	KEY_OUT = DATA_ & 0xFF00;
	
	for(i=0;i<4;i++){
		//
		if(input_count[i] && keyboard_count[i] == 0){
			input_count[i]--;
			keyboard_count[i] = KEYBOARD_COUNT_DOWN_FULL;
		}
		if(input_count[i] && keyboard_count[i] > KEYBOARD_COUNT_DOWN_MIN){
			keyboard_count[i] = KEYBOARD_COUNT_DOWN_MIN;
		}
		if(keyboard_count[i] > KEYBOARD_COUNT_DOWN_TRIG)KEY_OUT |= (1<<i);
	}
}

void keyboard_do(UINT16 DATA_){
	UINT8C InputData_1[8] = {
        Keyboard_d, Keyboard_f, Keyboard_j, Keyboard_k, Keyboard_1, Keyboard_2, Keyboard_3, Keyboard_4,
    };
	UINT8C InputData_2[8] = {
        Keyboard_z, Keyboard_x, Keyboard_c, Keyboard_v, Keyboard_1, Keyboard_2, Keyboard_3, Keyboard_4,
    };
    UINT8 i,temp;
    
    temp = DATA_ & 0x0F;

    switch(DATA_ & 0xFF00){
        case 0x0100:temp |= 0x10;player_num_flag = 0;break;
        case 0x0200:temp |= 0x20;player_num_flag = 1;break;
        case 0x0300:temp |= 0x40;break;
        case 0x0400:temp |= 0x80;break;
    }
	
	if(player_num_flag){
		for (i = 0; i < 8; i++)
		{
			if (temp & (1 << i))
				UserBuf[2 + i] = InputData_2[i];
			else
				UserBuf[2 + i] = 0;
		}
	}
	else{
		for (i = 0; i < 8; i++)
		{
			if (temp & (1 << i))
				UserBuf[2 + i] = InputData_1[i];
			else
				UserBuf[2 + i] = 0;
		}
	}
	if(keyboard_count[0])keyboard_count[0]--;
	if(keyboard_count[1])keyboard_count[1]--;
	if(keyboard_count[2])keyboard_count[2]--;
	if(keyboard_count[3])keyboard_count[3]--;

    memcpy(Ep2Buffer + MAX_PACKET_SIZE, UserBuf, sizeof(UserBuf));
    UEP2_T_LEN = KEY_SIZE + 2;
    UEP2_CTRL = UEP2_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK;
}

/*------------------------------------------------------------------*/




/*---------------------Nintendo Switch------------------------------*/
//NS手柄
#define NS_GP_COUNT_DOWN_FULL 	100
#define NS_GP_COUNT_DOWN_MIN 	80
#define NS_GP_COUNT_DOWN_HALF 	40
#define NS_GP_COUNT_DOWN_HALF2 	20
#define NS_GP_COUNT_DOWN_TRIG	40

//NS PRO
#define NS_KEY_U_NUM2   0
#define NS_KEY_UR_NUM2  1
#define NS_KEY_R_NUM2   2
#define NS_KEY_RD_NUM2  3
#define NS_KEY_D_NUM2   4
#define NS_KEY_DL_NUM2  5
#define NS_KEY_L_NUM2   6
#define NS_KEY_LU_NUM2  7

#define NS_KEY_Y_NUM0  0
#define NS_KEY_B_NUM0  1
#define NS_KEY_A_NUM0  2
#define NS_KEY_X_NUM0  3
#define NS_KEY_L_NUM0  4
#define NS_KEY_R_NUM0  5
#define NS_KEY_Lz_NUM0	6
#define NS_KEY_Rz_NUM0	7

#define NS_KEY_SUB_NUM1   0
#define NS_KEY_ADD_NUM1   1
#define NS_KEY_L3_NUM1    2
#define NS_KEY_R3_NUM1    3
#define NS_KEY_HOME_NUM1  4
#define NS_KEY_PrtSc_NUM1 5

//don
volatile UINT8	ns_gp_count_B = 0;
volatile UINT8	ns_gp_count_Y = 0;
volatile UINT8	ns_gp_count_D = 0;
volatile UINT8	ns_gp_count_R = 0;

//ka
volatile UINT8	ns_gp_count_A = 0;
volatile UINT8	ns_gp_count_X = 0;
volatile UINT8	ns_gp_count_U = 0;
volatile UINT8	ns_gp_count_L = 0;


void ns_gp_get(UINT16 DATA_){
	KEY_OUT = DATA_ & 0xFF00;
	
	
	if(input_count[1]){
		if(ns_gp_count_R == 0){
			input_count[1]--;
			ns_gp_count_R = NS_GP_COUNT_DOWN_FULL;
			if(ns_gp_count_D < NS_GP_COUNT_DOWN_HALF)ns_gp_count_D = NS_GP_COUNT_DOWN_HALF;
			if(ns_gp_count_B < NS_GP_COUNT_DOWN_HALF2)ns_gp_count_B = NS_GP_COUNT_DOWN_HALF2;
			if(ns_gp_count_Y < NS_GP_COUNT_DOWN_HALF2)ns_gp_count_Y = NS_GP_COUNT_DOWN_HALF2;
		}
	}
	
	if(input_count[1]){
		if(ns_gp_count_D == 0){
			input_count[1]--;
			ns_gp_count_D = NS_GP_COUNT_DOWN_FULL;
			if(ns_gp_count_R < NS_GP_COUNT_DOWN_HALF)ns_gp_count_R = NS_GP_COUNT_DOWN_HALF;
			if(ns_gp_count_B < NS_GP_COUNT_DOWN_HALF2)ns_gp_count_B = NS_GP_COUNT_DOWN_HALF2;
			if(ns_gp_count_Y < NS_GP_COUNT_DOWN_HALF2)ns_gp_count_Y = NS_GP_COUNT_DOWN_HALF2;
		}
	}
	
	if(input_count[2]){
		if(ns_gp_count_B == 0){
			input_count[2]--;
			ns_gp_count_B = NS_GP_COUNT_DOWN_FULL;
			if(ns_gp_count_Y < NS_GP_COUNT_DOWN_HALF)ns_gp_count_Y = NS_GP_COUNT_DOWN_HALF;
			if(ns_gp_count_R < NS_GP_COUNT_DOWN_HALF2)ns_gp_count_R = NS_GP_COUNT_DOWN_HALF2;
			if(ns_gp_count_D < NS_GP_COUNT_DOWN_HALF2)ns_gp_count_D = NS_GP_COUNT_DOWN_HALF2;
		}
	}
	
	if(input_count[2]){
		if(ns_gp_count_Y == 0){
			input_count[2]--;
			ns_gp_count_Y = NS_GP_COUNT_DOWN_FULL;
			if(ns_gp_count_B < NS_GP_COUNT_DOWN_HALF)ns_gp_count_B = NS_GP_COUNT_DOWN_HALF;
			if(ns_gp_count_R < NS_GP_COUNT_DOWN_HALF2)ns_gp_count_R = NS_GP_COUNT_DOWN_HALF2;
			if(ns_gp_count_D < NS_GP_COUNT_DOWN_HALF2)ns_gp_count_D = NS_GP_COUNT_DOWN_HALF2;
		}
	}
	
	if(input_count[0]){
		if(ns_gp_count_L == 0){
			input_count[0]--;
			ns_gp_count_L = NS_GP_COUNT_DOWN_FULL;
			if(ns_gp_count_U < NS_GP_COUNT_DOWN_HALF)ns_gp_count_U = NS_GP_COUNT_DOWN_HALF;
		}
		else if(ns_gp_count_U == 0){
			input_count[0]--;
			ns_gp_count_U = NS_GP_COUNT_DOWN_FULL;
			if(ns_gp_count_L < NS_GP_COUNT_DOWN_HALF)ns_gp_count_L = NS_GP_COUNT_DOWN_HALF;
		}
	}
	
	if(input_count[3]){
		if(ns_gp_count_A == 0){
			input_count[3]--;
			ns_gp_count_A = NS_GP_COUNT_DOWN_FULL;
			if(ns_gp_count_X  < NS_GP_COUNT_DOWN_HALF)ns_gp_count_X = NS_GP_COUNT_DOWN_HALF;
		}
		else if(ns_gp_count_X == 0){
			input_count[3]--;
			ns_gp_count_X = NS_GP_COUNT_DOWN_FULL;
			if(ns_gp_count_A < NS_GP_COUNT_DOWN_HALF)ns_gp_count_A = NS_GP_COUNT_DOWN_HALF;
		}
	}
	
	if(input_count[0]){
		if(ns_gp_count_L > NS_GP_COUNT_DOWN_MIN)ns_gp_count_L = NS_GP_COUNT_DOWN_MIN;
		if(ns_gp_count_U > NS_GP_COUNT_DOWN_MIN)ns_gp_count_U = NS_GP_COUNT_DOWN_MIN;
	}
	
	if(input_count[1]){
		if(ns_gp_count_B > NS_GP_COUNT_DOWN_MIN)ns_gp_count_B = NS_GP_COUNT_DOWN_MIN;
		if(ns_gp_count_Y > NS_GP_COUNT_DOWN_MIN)ns_gp_count_Y = NS_GP_COUNT_DOWN_MIN;
	}
	
	if(input_count[2]){
		if(ns_gp_count_R > NS_GP_COUNT_DOWN_MIN)ns_gp_count_R = NS_GP_COUNT_DOWN_MIN;
		if(ns_gp_count_D > NS_GP_COUNT_DOWN_MIN)ns_gp_count_D = NS_GP_COUNT_DOWN_MIN;
	}
	
	if(input_count[3]){
		if(ns_gp_count_A > NS_GP_COUNT_DOWN_MIN)ns_gp_count_A = NS_GP_COUNT_DOWN_MIN;
		if(ns_gp_count_X > NS_GP_COUNT_DOWN_MIN)ns_gp_count_X = NS_GP_COUNT_DOWN_MIN;
	}
	
	if(ns_gp_count_A)ns_gp_count_A--;
	if(ns_gp_count_B)ns_gp_count_B--;
	if(ns_gp_count_X)ns_gp_count_X--;
	if(ns_gp_count_Y)ns_gp_count_Y--;
	if(ns_gp_count_U)ns_gp_count_U--;
	if(ns_gp_count_D)ns_gp_count_D--;
	if(ns_gp_count_L)ns_gp_count_L--;
	if(ns_gp_count_R)ns_gp_count_R--;
}


void ns_gp_do(UINT16 DATA_){
	UINT8 RecData[8] = {0x00,0x00,0x0F,0x80,0x80,0x80,0x80,0x00};
	UINT8 max_avlue = 0,max_key = 0x0F;

	//ABXY按键的输出
	if(ns_gp_count_A > NS_GP_COUNT_DOWN_TRIG)RecData[0] |= 1 << NS_KEY_A_NUM0;
	if(ns_gp_count_B > NS_GP_COUNT_DOWN_TRIG)RecData[0] |= 1 << NS_KEY_B_NUM0;
	if(ns_gp_count_X > NS_GP_COUNT_DOWN_TRIG)RecData[0] |= 1 << NS_KEY_X_NUM0;
	if(ns_gp_count_Y > NS_GP_COUNT_DOWN_TRIG)RecData[0] |= 1 << NS_KEY_Y_NUM0;
  
	//一个排序
	if(ns_gp_count_U >= ns_gp_count_D){
		max_avlue = ns_gp_count_U;
		max_key = NS_KEY_U_NUM2;
	}
	else{
		max_avlue = ns_gp_count_D;
		max_key = NS_KEY_D_NUM2;
	}
	
	if(max_avlue < ns_gp_count_L){
		max_avlue = ns_gp_count_L;
		max_key = NS_KEY_L_NUM2;
	}
	
	if(max_avlue < ns_gp_count_R){
		max_avlue = ns_gp_count_R;
		max_key = NS_KEY_R_NUM2;
	}
	
	//根据排序选出方向键
	if(max_avlue > NS_GP_COUNT_DOWN_TRIG){
		switch(max_key){
			
			case NS_KEY_U_NUM2:
				if(ns_gp_count_L > ns_gp_count_R){
					if(ns_gp_count_L > NS_GP_COUNT_DOWN_TRIG)
						max_key = NS_KEY_LU_NUM2;
				}
				else{
					if(ns_gp_count_R > NS_GP_COUNT_DOWN_TRIG)
						max_key = NS_KEY_UR_NUM2;
				}
				break;
				
			case NS_KEY_D_NUM2:
				if(ns_gp_count_L > ns_gp_count_R){
					if(ns_gp_count_L > NS_GP_COUNT_DOWN_TRIG)
						max_key = NS_KEY_DL_NUM2;
				}
				else{
					if(ns_gp_count_R > NS_GP_COUNT_DOWN_TRIG)
						max_key = NS_KEY_RD_NUM2;
				}
				break;
				
			case NS_KEY_L_NUM2:
				if(ns_gp_count_U > ns_gp_count_D){
					if(ns_gp_count_U > NS_GP_COUNT_DOWN_TRIG)
						max_key = NS_KEY_LU_NUM2;
				}
				else{
					if(ns_gp_count_D > NS_GP_COUNT_DOWN_TRIG)
						max_key = NS_KEY_DL_NUM2;
				}
				break;
			
			case NS_KEY_R_NUM2:
				if(ns_gp_count_U > ns_gp_count_D){
					if(ns_gp_count_U > NS_GP_COUNT_DOWN_TRIG)
						max_key = NS_KEY_UR_NUM2;
				}
				else{
					if(ns_gp_count_D > NS_GP_COUNT_DOWN_TRIG)
						max_key = NS_KEY_RD_NUM2;
				}
				break;
		}
		
		RecData[2] = max_key;
	}


	switch((DATA_ >> 8) & 0xFF){

		case 0x01:RecData[2] = NS_KEY_U_NUM2;              break;//1 press
		case 0x02:RecData[0] |= 1 << NS_KEY_X_NUM0;        break;//2 press
		case 0x03:RecData[0] |= 1 << NS_KEY_X_NUM0;        break;//3 press
		case 0x04:RecData[2] = NS_KEY_D_NUM2;              break;//4 press

		case 0x11:RecData[1] |= 1 << NS_KEY_SUB_NUM1;      break;//1+2 press
		case 0x12:RecData[1] |= 1 << NS_KEY_HOME_NUM1;     break;//2+3 press
		case 0x13:RecData[1] |= 1 << NS_KEY_ADD_NUM1;      break;//3+4 press
		case 0x1F:RecData[0] |= (1 << NS_KEY_L_NUM0) | (1 << NS_KEY_R_NUM0);break;//1+4 press  

		case 0x21://3 keys press
		case 0x31:RecData[1] |= 1 << NS_KEY_PrtSc_NUM1;    break;//4 keys press

	}

	memcpy(Ep2Buffer + MAX_PACKET_SIZE, RecData, KEY_SIZE + 2);
	UEP2_T_LEN = sizeof(RecData);
	UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;
}
/*------------------------------------------------------------------*/



void Taiko_task(UINT16 DATA_){
	
	get_input(DATA_);
	
	if((DATA_ & 0xFF00) == 0x3100){
		shutdown_count++;
		if(shutdown_count > 3000){
			LED_USB = 0;
			while (1){
				if(KEY1 && KEY2 && KEY3 && KEY4)break;
				WDOG_COUNT = 0;
				mDelaymS(10);
			}
			CH554SoftReset();
		}
	}
	else{
		shutdown_count = 0;
	}
	
	switch(start_up_mode){
		case MODE_NS_GP:
			ns_gp_get(DATA_);
			break;
		
		case MODE_KEYBOARD:
			keyboard_get(DATA_);
			break;
		
		case MODE_KB_LMT:
			kb_lmt_get(DATA_);
			break;
		
		default:
			while(1);
	}
}


void HIDIn(UINT16 DATA_)
{
	
	switch(start_up_mode){
		case MODE_NS_GP:
			ns_gp_do(DATA_);
			break;
		
		case MODE_KEYBOARD:
			keyboard_do(DATA_);
			break;
		
		case MODE_KB_LMT:
			kb_lmt_do(DATA_);
			break;
		
		default:
			while(1);
	}
}


/*******************************************************************************
* Function Name  : DeviceInterrupt()
* Description    : CH559USB中断处理函数
*******************************************************************************/
void    DeviceInterrupt( void ) interrupt INT_NO_USB using 1                    //USB中断服务程序,使用寄存器组1
{
    UINT8 len;
    if(UIF_TRANSFER)                                                            //USB传输完成标志
    {
        switch (USB_INT_ST & (MASK_UIS_TOKEN | MASK_UIS_ENDP))
        {
        case UIS_TOKEN_IN | 2:   
			
//            UEP1_CTRL ^= bUEP_T_TOG; 				
            if ( U_TOG_OK ){			 
            }
            else{		 
              //transf failed
            }						 
            UEP2_T_LEN = 0;
            EndInBusy = 0 ;									   
            UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;
            break;
			
        case UIS_TOKEN_IN | 4:    
			//端点4仅支持手动反翻转
            UEP4_CTRL ^= bUEP_T_TOG; 				
            if ( U_TOG_OK ){			 
            }
            else{		 
              //transf failed
            }
            UEP4_T_LEN = 0;
            EndInBusy = 0 ;									   
            UEP4_CTRL = UEP4_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;
            break;
			
        case UIS_TOKEN_OUT | 3:                                                  //endpoint 3# j
			
            if ( U_TOG_OK )
            {
              //get rec data
            }
            break;
        
        case UIS_TOKEN_OUT | 2:                                                 //endpoint 2# 
            
            if ( U_TOG_OK )
            {
              //get rec data
            }
            break;
            
        case UIS_TOKEN_SETUP | 0:                                               //SETUP事务
            UEP0_CTRL ^= bUEP_T_TOG; 
            len = USB_RX_LEN;
            if(len == (sizeof(USB_SETUP_REQ)))
            {
                if(UsbSetupBuf->wLengthH)
					SetupLen = 0xFF;
                else
					SetupLen = UsbSetupBuf->wLengthL;
                len = 0;
                SetupReq = UsbSetupBuf->bRequest;
                if ( ( UsbSetupBuf->bRequestType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD )/*HID类命令*/
                {
                  switch( SetupReq )                                             
                  {
                    case 0x01:                                                  //GetReport
                        pDescr = UserBuf;
                        if (SetupLen >= THIS_ENDP0_SIZE)
                        {
                            len = THIS_ENDP0_SIZE;
                        }
                        else
                        {
                            len = SetupLen;
                        }
                        break;
                    case 0x02:                                                   //GetIdle
						break;	
                    case 0x03:                                                   //GetProtocol
						break;				
                    case 0x09:                                                   //SetReport
						break; 
                    case 0x0A:                                     				//SetIdle
						break;	
                    case 0x0B:    												//SetProtocol 
						break;
                    default:
						len = 0xFF;  				                              /*命令不支持*/					
						break;
                  }
                  if( SetupLen > len ){
					SetupLen = len;    //限制总长度
                  }
                  len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen;   //本次传输长度
                  memcpy(Ep0Buffer,pDescr,len);                                     //加载上传数据
                  SetupLen -= len;
                  pDescr += len;									
                }
                else                                                             //标准请求
                {
					//here
                    switch(SetupReq)                                             //请求码
                    {
                    case USB_GET_DESCRIPTOR:
                        switch(UsbSetupBuf->wValueH)
                        {
                        case 1:                                                  //锟借备锟斤拷锟斤拷锟斤拷
                            pDescr = USB_Str[start_up_mode].DevDesc;                                    //锟斤拷锟借备锟斤拷锟斤拷锟斤拷锟酵碉拷要锟斤拷锟酵的伙拷锟斤拷锟斤拷
                            len = USB_Str[start_up_mode].DevDesc_size;
                            break;
                        case 2:                                                  //配置描述符
                            pDescr = USB_Str[start_up_mode].CfgDesc;                                    //把设备描述符送到要发送的缓冲区
                            len = USB_Str[start_up_mode].CfgDesc_size;
                            break;
                        case 0x22:                                               //报表描述符
							HID_Ready = 1;
                            pDescr = USB_Str[start_up_mode].HIDRepDesc;                                 //数据准备上传
                            len = USB_Str[start_up_mode].HIDRepDesc_size;;                            
                            break;
                        case 0x03:                                                  //设备名称
                            
                            switch(UsbSetupBuf->wValueL){
                              
                              case 0:
                                pDescr = LangDes;
                                len = sizeof(LangDes);   
                                break;
                              
                              case 1:
                                pDescr = SerDes;
								len = GetSerDes(SerDes1,sizeof(SerDes1)); 
                                break;
                              
                              case 2:
                                pDescr = SerDes;
								if(start_up_mode == MODE_KEYBOARD)
									len = GetSerDes(SerDes2_Keyboard,sizeof(SerDes2_Keyboard)); 
								else if(start_up_mode == MODE_KB_LMT)
									len = GetSerDes(SerDes2_KB_LMT,sizeof(SerDes2_KB_LMT)); 
								else
									len = GetSerDes(SerDes2_DS_GP,sizeof(SerDes2_DS_GP)); 
                                break;
                              
                              case 3:
                                GetID();
                                pDescr = SerDes;
                                len = GetSerDes(SerDes3,sizeof(SerDes3)); 
                                break;
                              
                              default:
                                  len = 0xff;                                    //不支持的命令或者出错
                                  break;
                            }
//                            if(len > SetupLen)SerDes[0] = SetupLen;
                            break;
							
						case 0x06:
						case 0x07:
                            len = 0xff;                                          //不支持的命令或者出错
                            break;
                        default:
                            len = 0xff;                                          //不支持的命令或者出错
                            break;
                        }
                        if ( SetupLen > len )
                        {
                            SetupLen = len;    //限制总长度
                        }
                        len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen;//本次传输长度
                        memcpy(Ep0Buffer,pDescr,len);                            //加载上传数据
                        SetupLen -= len;
                        pDescr += len;
                        break;
                    case USB_SET_ADDRESS:
                        if(UsbSetupBuf->wValueH)SetupLen = 0xFF;                         //锟捷达拷USB锟借备锟斤拷址
                        else SetupLen = UsbSetupBuf->wValueL;                         //锟捷达拷USB锟借备锟斤拷址
                        break;
                    case USB_GET_CONFIGURATION:
                        Ep0Buffer[0] = UsbConfig;
                        if ( SetupLen >= 1 )
                        {
                            len = 1;
                        }
                        break;
                    case USB_SET_CONFIGURATION:	  
						//here
                        UsbConfig = UsbSetupBuf->wValueL;
                        if(UsbConfig)
                        {
                          Ready = 1;                                            //set config命令一般代表usb枚举完成的标志
                          EndInBusy = 0;
                        }
                        break;
                    case USB_GET_INTERFACE:
                        break;
                    case USB_CLEAR_FEATURE:                                      //Clear Feature
                        if ( ( UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )// 端点
                        {
                            switch( UsbSetupBuf->wIndexL )
                            {
                            case 0x82:
                                UEP2_CTRL = UEP2_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
                                break;
                            case 0x02:
                                UEP2_CTRL = UEP2_CTRL & ~ ( bUEP_R_TOG | MASK_UEP_R_RES ) | UEP_R_RES_ACK;
                                break;
                            case 0x84:
                                UEP4_CTRL = UEP4_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
                                break;
                            case 0x03:
                                UEP3_CTRL = UEP3_CTRL & ~ ( bUEP_R_TOG | MASK_UEP_R_RES ) | UEP_R_RES_ACK;
                                break;
                            default:
                                len = 0xFF;                                       // 不支持的端点
                                break;
                            }
                        }
                        else
                        {
                            len = 0xFF;                                           // 不是端点不支持
                        }
                        break;
                    case USB_SET_FEATURE:                                         /* Set Feature */
                        if( ( UsbSetupBuf->bRequestType & 0x1F ) == 0x00 )        /* 设置设备 */
                        {
                            if( ( ( ( UINT16 )UsbSetupBuf->wValueH << 8 ) | UsbSetupBuf->wValueL ) == 0x01 )
                            {
                                if( USB_Str[start_up_mode].CfgDesc[ 7 ] & 0x20 )
                                {
                                    /* 设置唤醒使能标志 */
                                }
                                else
                                {
                                    len = 0xFF;                                    /* 操作失败 */
                                }
                            }
                            else
                            {
                                len = 0xFF;                                        /* 操作失败 */
                            }
                        }
                        else if( ( UsbSetupBuf->bRequestType & 0x1F ) == 0x02 )    /* 设置端点 */
                        {
                            if( ( ( ( UINT16 )UsbSetupBuf->wValueH << 8 ) | UsbSetupBuf->wValueL ) == 0x00 )
                            {
                                switch( ( ( UINT16 )UsbSetupBuf->wIndexH << 8 ) | UsbSetupBuf->wIndexL )
                                {
                                case 0x82:
                                    UEP2_CTRL = UEP2_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;/* 设置端点2 IN STALL */
                                    break;
                                case 0x02:
                                    UEP2_CTRL = UEP2_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL;/* 设置端点2 OUT Stall */
                                    break;
                                case 0x84:
                                    UEP4_CTRL = UEP4_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;/* 设置端点2 IN STALL */
                                    break;
                                case 0x03:
                                    UEP3_CTRL = UEP3_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL;/* 设置端点2 OUT Stall */
                                    break;
                                default:
                                    len = 0xFF;                                     /* 操作失败 */
                                    break;
                                }
                            }
                            else
                            {
                                len = 0xFF;                                         /* 操作失败 */
                            }
                        }
                        else
                        {
                            len = 0xFF;                                             /* 操作失败 */
                        } 
                        break;
                    case USB_GET_STATUS:
                        Ep0Buffer[0] = 0x00;
                        Ep0Buffer[1] = 0x00;
                        if ( SetupLen >= 2 )
                        {
                            len = 2;
                        }
                        else
                        {
                            len = SetupLen;
                        }
                        break;
                    default:
                        len = 0xff;                                                  //操作失败
                        break;
                    }
                }
            }
            else
            {
                len = 0xff;                                                          //包长度错误
            }
            if(len == 0xff)
            {
                SetupReq = 0xFF;
                UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL;//STALL
            }
            else if(len <= THIS_ENDP0_SIZE)                                         //上传数据或者状态阶段返回0长度包
            {
                UEP0_T_LEN = len;
                UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;//默认数据包是DATA1，返回应答ACK
            }
            else
            {
                UEP0_T_LEN = 0;  //虽然尚未到状态阶段，但是提前预置上传0长度数据包以防主机提前进入状态阶段
                UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;//默认数据包是DATA1,返回应答ACK
            }
            break;
        case UIS_TOKEN_IN | 0:                                                      //endpoint0 IN
            switch(SetupReq)
            {
            case USB_GET_DESCRIPTOR:
            case HID_GET_REPORT:							
                len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen;     //本次传输长度
                memcpy( Ep0Buffer, pDescr, len );                                   //加载上传数据
                SetupLen -= len;
                pDescr += len;
                UEP0_T_LEN = len;
                UEP0_CTRL ^= bUEP_T_TOG;                                            //同步标志位翻转
                break;
            case USB_SET_ADDRESS:
                USB_DEV_AD = USB_DEV_AD & bUDA_GP_BIT | SetupLen;
                UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
                break;
            default:
                UEP0_T_LEN = 0;                                                      //状态阶段完成中断或者是强制上传0长度数据包结束控制传输
                UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
                break;
            }
            break;
			
        case UIS_TOKEN_OUT | 0:  // endpoint0 OUT
            len = USB_RX_LEN;
            UEP0_CTRL ^= bUEP_R_TOG;                                     //同步标志位翻转
            if (SetupReq == 0x09)
            {
                // state LED
                STA_NumLock = !!(Ep0Buffer[0] & 0x01);
                STA_CapLock = !!(Ep0Buffer[0] & 0x02);
                STA_ScrLock = !!(Ep0Buffer[0] & 0x04);
            }
            break;
		
        default:
            break;
        }
        UIF_TRANSFER = 0;                                                           //写0清空中断
    }
    else if(UIF_BUS_RST)                                                                 //设备模式USB总线复位中断
    {
        UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
		UEP2_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;
        USB_DEV_AD = 0x00;
        UIF_SUSPEND = 0;                                                         //清中断标志
        UIF_TRANSFER = 0;
        UIF_BUS_RST = 0;    
        EndInBusy = 0;
    }
    else if (UIF_SUSPEND)                                                                 //USB总线挂起/唤醒完成
    {
        UIF_SUSPEND = 0;
        if ( USB_MIS_ST & bUMS_SUSPEND )                                             //挂起
        {
          Ready = 0;
        }
        else{
          Ready = 1;
		  EndInBusy = 0;
        }
    }
    else {                                                                             //意外的中断,不可能发生的情况
        USB_INT_FG = 0xFF;                                                             //清中断标志
    }
}


#define TIMER2_DATA 	(65535-1363)
				  
UINT8 ErrFlag = 0;
UINT8 IO_ErrFlag = 0;

void main(void)				   
{
    UINT16 KEY_TEMP = 0;
    UINT8 WAIT_COUNT = 0;

    //set watch dog before system up
    WDOG_COUNT = 0;	  
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    GLOBAL_CFG |= bWDOG_EN;
    SAFE_MOD = 0x00;

    InitIO();
    CfgFsys();                                                           //CH559时钟选择配置
    mDelaymS(5);                                                          //修改主频等待内部晶振稳定,必加	
  
	//start up delay1 wait 1s
	start_up_mode = MODE_STARTUP;
    while (1){
		if(!KEY1)start_up_mode = MODE_NS_GP;
		if(!KEY2)start_up_mode = MODE_KEYBOARD;
		if(!KEY3)start_up_mode = MODE_KB_LMT;
		if(!KEY4)start_up_mode = MODE_STARTUP;
		WDOG_COUNT = 0;
		WAIT_COUNT++;
		mDelaymS(10);
		if(WAIT_COUNT > 100){
			WAIT_COUNT = 0;
			LED_USB = 0;
			break;
		}
		if((WAIT_COUNT%3)==0)
			LED_USB = ~LED_USB;
	}
	
	if(start_up_mode == MODE_NS_GP){
		UINT8 COUNT_NUM_D = 2;
		UINT8 COUNT_NUM_K = 2;
	}
	
	TR2 = 0;
	C_T2=0;
	CP_RL2=0;
	RCLK=0;
	TCLK=0;
	TL2 = TIMER2_DATA%0xFF;
	TH2 = TIMER2_DATA/0xFF;
	RCAP2L = TIMER2_DATA%0xFF;
	RCAP2H = TIMER2_DATA/0xFF;
	TR2 = 1;
	
    memset(UserBuf,0,sizeof(UserBuf));
    USBDeviceInit();                                                      //USB设备模式初始化
    EA = 1;                                                               //允许单片机中断		
    UEP1_T_LEN = 0;                                                       //预使用发送长度一定要清空
    UEP2_T_LEN = 0;                                                       //预使用发送长度一定要清空
    Ready = 0;
    
	
	//wait ready
    while (Ready == 0 || HID_Ready == 0){
		WAIT_COUNT++;
		mDelaymS(10);
		if((WAIT_COUNT%5)==0)
			LED_USB = ~LED_USB;
	}
	
	//wair 500ms
	WAIT_COUNT = 0;
    while (1){
		WDOG_COUNT = 0;
		WAIT_COUNT++;
		mDelaymS(10);
		if(WAIT_COUNT > 50){
			WAIT_COUNT = 0;
			LED_USB = 0;
			break;
		}
		if((WAIT_COUNT%10)==0)
			LED_USB = ~LED_USB;
	}
	WAIT_COUNT = 0;
    
	//wair 500ms
    while(1)
    {				 
        WDOG_COUNT = 0;	//WDOG reload

        Taiko_task(ReadIO());
		
        if(Ready == 0){
			CH554SoftReset();
			while(1);
        }
        
        if(Ready){
            if(EndInBusy == 0){
                WAIT_COUNT = 0;
                EndInBusy = 1;
                HIDIn(KEY_OUT);
            }
            else{
                WAIT_COUNT++;
                if(WAIT_COUNT >= 1000)
                	Ready = 0;
            }
        }
		
		LED_USB = 0;
		while(TF2 == 0){
		}
		TF2 = 0;
        if(Ready && ((ErrFlag | IO_ErrFlag) == 0))
          LED_USB = 1;
		
    }
}

