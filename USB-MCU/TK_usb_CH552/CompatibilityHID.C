/*
太鼓IO hid程序
v1.0
2022年2月7日
*/

#include "./Public/CH552.H"
#include "./Public/Debug.H"
#include "GPIO.h"
#include <stdio.h>
#include <string.h>

//#define WITHOUT_OUTPUT_ASSIGN

#define VID 0xAABB
#define PID 0xCCDD

#define THIS_ENDP0_SIZE MAX_PACKET_SIZE
#define KEY_SIZE 16

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

UINT8X Ep0Buffer[THIS_ENDP0_SIZE + 2] _at_ 0x0000;     //端点0 OUT&IN缓冲区，必须是偶地址
UINT8X Ep2Buffer[2 * MAX_PACKET_SIZE + 4] _at_ 0x0044; //端点2 IN&OUT缓冲区,必须是偶地址
UINT8 SetupReq, SetupLen, Ready, Count, UsbConfig;
bit STA_NumLock = 0;
bit STA_CapLock = 0;
bit STA_ScrLock = 0;
PUINT8 pDescr;             // USB配置标志
USB_SETUP_REQ SetupReqBuf; //暂存Setup包
#define UsbSetupBuf ((PUSB_SETUP_REQ)Ep0Buffer)

UINT16 KEY_OUT = 0;
UINT16 KEY_OUT_bak = 0;

#pragma NOAREGS
/*设备描述符*/
const UINT8C DevDesc[18] = {0x12,     0x01, 0x00, 0x02, 0x00, 0x00, 0x00, THIS_ENDP0_SIZE, VID, VID >> 8, PID,
                            PID >> 8, 0x05, 0x01, 0x01, 0x02, 0x03, 0x01};

const UINT8C HIDRepDesc[63] = {
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

const UINT8C CfgDesc[34] = {
    0x09, /* bLength: Configuration Descriptor size */
    0x02, /* bDescriptorType: Configuration */
    sizeof(CfgDesc),
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
    0x00, /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
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
    sizeof(HIDRepDesc), /*wItemLength: Total length of Report descriptor*/
    0x00,
    /******************** Descriptor of Mouse endpoint ********************/
    /* 27 */
    0x07, /*bLength: Endpoint Descriptor size*/
    0x05, /*bDescriptorType:*/

    0x82,    /*bEndpointAddress: Endpoint Address (IN)*/
    0x03,    /*bmAttributes: Interrupt endpoint*/
    0x40,    /*wMaxPacketSize: 4 Byte max */
    0x00, 
	1, /*bInterval: Polling Interval */
};

const UINT8C LangDes[] = {0x04, 0x03, 0x09, 0x04}; //语言描述符

const UINT8C SerDes1[] = "Analog Dragon";
const UINT8C SerDes2[] = "Taiko Input";

const UINT8C SerDes1S[] = "A.D.";
const UINT8C SerDes2S[] = "T.K.";

UINT8 SerDes3[] = "0000000000_V15";

UINT8X UserEp2Buf[64];
UINT8 Endp2Busy = 0;

UINT8 SerDes[64] = {0};

UINT8 GetSerDes(UINT8 *str, UINT8 len)
{
    UINT8 i;
    if (len > 30)
        len = 30;

    memset(SerDes, 0, sizeof(SerDes));
    SerDes[0] = len * 2 + 2;
    SerDes[1] = 0x03;
    for (i = 0; i < len; i++)
    {
        SerDes[2 + i * 2] = str[i];
    }
    return SerDes[0];
}

void GetID(void)
{
    UINT8 i = 0;
    const UINT8C trsTable[16] = "0123456789ABCDEF";
    UINT8C *ID;
    UINT16 IDA;

    IDA = 0x3FFF;
    ID = (UINT8 *)IDA;
    SerDes3[i++] = trsTable[*ID & 0x0F];
    SerDes3[i++] = trsTable[(*ID >> 4) & 0x0F];

    IDA = 0x3FFE;
    ID = (UINT8 *)IDA;
    SerDes3[i++] = trsTable[*ID & 0x0F];
    SerDes3[i++] = trsTable[(*ID >> 4) & 0x0F];

    IDA = 0x3FFD;
    ID = (UINT8 *)IDA;
    SerDes3[i++] = trsTable[*ID & 0x0F];
    SerDes3[i++] = trsTable[(*ID >> 4) & 0x0F];

    IDA = 0x3FFC;
    ID = (UINT8 *)IDA;
    SerDes3[i++] = trsTable[*ID & 0x0F];
    SerDes3[i++] = trsTable[(*ID >> 4) & 0x0F];

    IDA = 0x3FFA;
    ID = (UINT8 *)IDA;
    SerDes3[i++] = trsTable[*ID & 0x0F];
    SerDes3[i++] = trsTable[(*ID >> 4) & 0x0F];
}

/*******************************************************************************
 * Function Name : USBDeviceInit()
 * Description : USB设备模式配置,设备模式启动，收发端点配置，中断开启
 * Input : None
 * Output : None
 * Return : None
 *******************************************************************************/
void USBDeviceInit()
{
    IE_USB = 0;
    USB_CTRL = 0x00;        // 先设定USB设备模式
    UDEV_CTRL = bUD_PD_DIS; // 禁止DP/DM下拉电阻

    UDEV_CTRL &= ~bUD_LOW_SPEED; //选择全速12M模式，默认方式
    USB_CTRL &= ~bUC_LOW_SPEED;

    UEP2_DMA = Ep2Buffer;                                      //端点2数据传输地址
    UEP2_3_MOD |= bUEP2_TX_EN | bUEP2_RX_EN;                   //端点2发送接收使能
    UEP2_3_MOD &= ~bUEP2_BUF_MOD;                              //端点2收发各64字节缓冲区
    UEP2_CTRL = bUEP_AUTO_TOG | UEP_T_RES_NAK | UEP_R_RES_ACK; //端点2自动翻转同步标志位，IN事务返回NAK，OUT返回ACK
    UEP0_DMA = Ep0Buffer;                                      //端点0数据传输地址
    UEP4_1_MOD &= ~(bUEP4_RX_EN | bUEP4_TX_EN);                //端点0单64字节收发缓冲区
    UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;                 // OUT事务返回ACK，IN事务返回NAK

    USB_DEV_AD = 0x00;
    USB_CTRL |= bUC_DEV_PU_EN | bUC_INT_BUSY | bUC_DMA_EN; // 启动USB设备及DMA，在中断期间中断标志未清除前自动返回NAK
    UDEV_CTRL |= bUD_PORT_EN;                              // 允许USB端口
    USB_INT_FG = 0xFF;                                     // 清中断标志
    USB_INT_EN = bUIE_SUSPEND | bUIE_TRANSFER | bUIE_BUS_RST; // 挂起中断
    IE_USB = 1;
}

/*----------------------*/
#ifdef WITHOUT_OUTPUT_ASSIGN
#define COUNT_NUM_D 	2
#define COUNT_NUM_K 	2
#else
#define COUNT_NUM_D 	4
#define COUNT_NUM_K 	2
#endif


UINT8 input_count[8] = {0};

/*----------------------*/

#define EXCHANGE_DATA	90//92
//1402/1422

void Taiko_task(UINT16 DATA_){
	
	//获取输入
	static UINT8 back = 0;
	static UINT8 exchange_count[8] = {0};
	UINT8 temp = DATA_ & 0xFF;
	UINT8 out = (temp ^ back) & temp;
	back = temp;
	
	if((out & 0x01) && (input_count[0] < COUNT_NUM_K))input_count[0]++;
	if((out & 0x02) && (input_count[1] < COUNT_NUM_D))input_count[1]++;
	if((out & 0x04) && (input_count[2] < COUNT_NUM_D))input_count[2]++;
	if((out & 0x08) && (input_count[3] < COUNT_NUM_K))input_count[3]++;
	if((out & 0x10) && (input_count[4] < COUNT_NUM_K))input_count[4]++;
	if((out & 0x20) && (input_count[5] < COUNT_NUM_D))input_count[5]++;
	if((out & 0x40) && (input_count[6] < COUNT_NUM_D))input_count[6]++;
	if((out & 0x80) && (input_count[7] < COUNT_NUM_K))input_count[7]++;
	
	//咚交叉，咔不交叉
	//1 = LK
	if(input_count[0] && exchange_count[0] == 0){
		input_count[0]--;
		exchange_count[0] = EXCHANGE_DATA;
		if(exchange_count[3] == 0)exchange_count[3] = EXCHANGE_DATA / 2;
	}
	if(input_count[4] && exchange_count[4] == 0){
		input_count[4]--;
		exchange_count[4] = EXCHANGE_DATA;
		if(exchange_count[7] == 0)exchange_count[7] = EXCHANGE_DATA / 2;
	}
	//4 = RK
	if(input_count[3] && exchange_count[3] == 0){
		input_count[3]--;
		exchange_count[3] = EXCHANGE_DATA;
		if(exchange_count[0] == 0)exchange_count[0] = EXCHANGE_DATA / 2;
	}
	if(input_count[7] && exchange_count[7] == 0){
		input_count[7]--;
		exchange_count[7] = EXCHANGE_DATA;
		if(exchange_count[4] == 0)exchange_count[4] = EXCHANGE_DATA / 2;
	}
	//2 = LD
	if(input_count[1] && exchange_count[1] == 0){
		input_count[1]--;
		exchange_count[1] = EXCHANGE_DATA;
		if(exchange_count[2] == 0)exchange_count[2] = EXCHANGE_DATA / 2;
	}
	if(input_count[5] && exchange_count[5] == 0){
		input_count[5]--;
		exchange_count[5] = EXCHANGE_DATA;
		if(exchange_count[6] == 0)exchange_count[6] = EXCHANGE_DATA / 2;
	}
	//3 = RD
	if(input_count[2] && exchange_count[2] == 0){
		input_count[2]--;
		exchange_count[2] = EXCHANGE_DATA;
		if(exchange_count[1] == 0)exchange_count[1] = EXCHANGE_DATA / 2;
	}
	if(input_count[6] && exchange_count[6] == 0){
		input_count[6]--;
		exchange_count[6] = EXCHANGE_DATA;
		if(exchange_count[5] == 0)exchange_count[5] = EXCHANGE_DATA / 2;
	}
	
#ifdef WITHOUT_OUTPUT_ASSIGN
	//
#else
	//咚允许交叉加入
	if(input_count[1] && exchange_count[2] == 0){
		input_count[1]--;
		exchange_count[2] = EXCHANGE_DATA;
		if(exchange_count[1] == 0)exchange_count[1] = EXCHANGE_DATA / 2;
	}
	if(input_count[2] && exchange_count[1] == 0){
		input_count[2]--;
		exchange_count[1] = EXCHANGE_DATA;
		if(exchange_count[2] == 0)exchange_count[2] = EXCHANGE_DATA / 2;
	}
	if(input_count[5] && exchange_count[6] == 0){
		input_count[5]--;
		exchange_count[6] = EXCHANGE_DATA;
		if(exchange_count[5] == 0)exchange_count[5] = EXCHANGE_DATA / 2;
	}
	if(input_count[6] && exchange_count[5] == 0){
		input_count[6]--;
		exchange_count[5] = EXCHANGE_DATA;
		if(exchange_count[6] == 0)exchange_count[6] = EXCHANGE_DATA / 2;
	}
	
#endif
	
	//获取输出
	KEY_OUT = DATA_ & 0xFF00;
	if(exchange_count[0] > EXCHANGE_DATA / 2)KEY_OUT |= 0x01;
	if(exchange_count[1] > EXCHANGE_DATA / 2)KEY_OUT |= 0x02;
	if(exchange_count[2] > EXCHANGE_DATA / 2)KEY_OUT |= 0x04;
	if(exchange_count[3] > EXCHANGE_DATA / 2)KEY_OUT |= 0x08;
	if(exchange_count[4] > EXCHANGE_DATA / 2)KEY_OUT |= 0x10;
	if(exchange_count[5] > EXCHANGE_DATA / 2)KEY_OUT |= 0x20;
	if(exchange_count[6] > EXCHANGE_DATA / 2)KEY_OUT |= 0x40;
	if(exchange_count[7] > EXCHANGE_DATA / 2)KEY_OUT |= 0x80;

	
	//倒计时，60Hz或者30Hz
	if(exchange_count[0])exchange_count[0]--;
	if(exchange_count[1])exchange_count[1]--;
	if(exchange_count[2])exchange_count[2]--;
	if(exchange_count[3])exchange_count[3]--;
	if(exchange_count[4])exchange_count[4]--;
	if(exchange_count[5])exchange_count[5]--;
	if(exchange_count[6])exchange_count[6]--;
	if(exchange_count[7])exchange_count[7]--;
}


/*******************************************************************************
 * Function Name : Enp2BlukIn()
 * Description : USB设备模式端点2的批量上传
 * Input : None
 * Output : None
 * Return : None
 *******************************************************************************/
void Enp2BlukIn(void)
{
//    UINT8C InputData[8] = {
//        Keyboard_1, Keyboard_2, Keyboard_3, Keyboard_4, Keyboard_5, Keyboard_6, Keyboard_7, Keyboard_8,
//    };
//    UINT8C CtrlData[8] = {
//        Keyboard_q, Keyboard_w, Keyboard_e, Keyboard_r, Keyboard_t, Keyboard_y, Keyboard_u, Keyboard_i,
//    };
	UINT8C InputData[8] = {
        Keyboard_d, Keyboard_f, Keyboard_j, Keyboard_k, Keyboard_z, Keyboard_x, Keyboard_c, Keyboard_v,
    };
    UINT8C CtrlData[8] = {
        Keyboard_1, Keyboard_2, Keyboard_3, Keyboard_4, Keyboard_5, Keyboard_6, Keyboard_7, Keyboard_8,
    };
    UINT8 i;
	
	if(KEY_OUT == KEY_OUT_bak){
		Endp2Busy = 0;
		return;
	}
	
	KEY_OUT_bak = KEY_OUT;
	
    for (i = 0; i < 8; i++){
        if (KEY_OUT & (1 << i))
            UserEp2Buf[2 + i] = InputData[i];
		else
			UserEp2Buf[2 + i] = 0;

        if (KEY_OUT & (1 << (i + 8)))
            UserEp2Buf[2 + 8 + i] = CtrlData[i];
		else
			UserEp2Buf[2 + 8 + i] = 0;
    }

    memcpy(Ep2Buffer + MAX_PACKET_SIZE, UserEp2Buf, sizeof(UserEp2Buf));
    UEP2_T_LEN = KEY_SIZE + 2;
    UEP2_CTRL = UEP2_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK;
}

/*******************************************************************************
 * Function Name : DeviceInterrupt()
 * Description : CH559USB中断处理函数
 *******************************************************************************/
void DeviceInterrupt(void) interrupt INT_NO_USB using 1 // USB中断服务程序,使用寄存器组1
{
    UINT8 len;
    if (UIF_TRANSFER) // USB传输完成标志
    {
        switch (USB_INT_ST & (MASK_UIS_TOKEN | MASK_UIS_ENDP))
        {
        case UIS_TOKEN_IN | 2: // endpoint 2#

            // UEP1_CTRL ^= bUEP_T_TOG;
            if (U_TOG_OK)
            {
            }
            else
            {
                // transf failed
            }
            UEP2_T_LEN = 0;
            Endp2Busy = 0;
            UEP2_CTRL = UEP2_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_NAK;
            break;

        case UIS_TOKEN_OUT | 2: // endpoint 2#

            if (U_TOG_OK)
            {
                // get rec data
            }
            break;

        case UIS_TOKEN_SETUP | 0: // SETUP事务
            len = USB_RX_LEN;
            if (len == (sizeof(USB_SETUP_REQ)))
            {
                if (UsbSetupBuf->wLengthH)
                    SetupLen = 0xFF;
                else
                    SetupLen = UsbSetupBuf->wLengthL;
                len = 0; // 默锟斤拷为锟缴癸拷锟斤拷锟斤拷锟较达拷0锟斤拷锟斤拷
                SetupReq = UsbSetupBuf->bRequest;
                if ((UsbSetupBuf->bRequestType & USB_REQ_TYP_MASK) != USB_REQ_TYP_STANDARD) /*HID类命令*/
                {
                    switch (SetupReq)
                    {
                    case 0x01:                           // GetReport
                        pDescr = UserEp2Buf;             //控制端点上传输据
                        if (SetupLen >= THIS_ENDP0_SIZE) //大于端点0大小，需要特殊处理
                        {
                            len = THIS_ENDP0_SIZE;
                        }
                        else
                        {
                            len = SetupLen;
                        }
                        break;
                    case 0x02: // GetIdle
                        break;
                    case 0x03: // GetProtocol
                        break;
                    case 0x09: // SetReport
                        break;
                    case 0x0A: // SetIdle
                        break;
                    case 0x0B: // SetProtocol
                        break;
                    default:
                        len = 0xFF; /*命令不支持*/
                        break;
                    }
                    if (SetupLen > len)
                    {
                        SetupLen = len; //限制总长度
                    }
                    len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen; //本次传输长度
                    memcpy(Ep0Buffer, pDescr, len);                                 //加载上传数据
                    SetupLen -= len;
                    pDescr += len;
                }
                else //标准请求
                {
                    // here
                    switch (SetupReq) //请求码
                    {
                    case USB_GET_DESCRIPTOR:
                        switch (UsbSetupBuf->wValueH)
                        {
                        case 1: //锟借备锟斤拷锟斤拷锟斤拷
                            // twice
                            pDescr = DevDesc; //锟斤拷锟借备锟斤拷锟斤拷锟斤拷锟酵碉拷要锟斤拷锟酵的伙拷锟斤拷锟斤拷
                            len = sizeof(DevDesc);
                            break;
                        case 2:               //配置描述符
                            pDescr = CfgDesc; //把设备描述符送到要发送的缓冲区
                            len = sizeof(CfgDesc);
                            break;
                        case 0x22:               //报表描述符
                            pDescr = HIDRepDesc; //数据准备上传
                            len = sizeof(HIDRepDesc);
                            break;
                        case 3: //设备名称

                            switch (UsbSetupBuf->wValueL)
                            {

                            case 0:
                                pDescr = LangDes;
                                len = sizeof(LangDes);
                                break;

                            case 1:
                                pDescr = SerDes;
                                if (SetupLen <= 0x0A)
                                {
                                    len = GetSerDes(SerDes1S, sizeof(SerDes1S));
                                }
                                else
                                {
                                    len = GetSerDes(SerDes1, sizeof(SerDes1));
                                }
                                break;

                            case 2:
                                pDescr = SerDes;
                                if (SetupLen <= 0x0A)
                                {
                                    len = GetSerDes(SerDes2S, sizeof(SerDes2S));
                                }
                                else
                                {
                                    len = GetSerDes(SerDes2, sizeof(SerDes2));
                                }
                                break;

                            case 3:
                                GetID();
                                pDescr = SerDes;
                                len = GetSerDes(SerDes3, sizeof(SerDes3));
                                break;

                            default:
                                len = 0xff; //不支持的命令或者出错
                                break;
                            }

                            if (len > SetupLen)
                                SerDes[0] = SetupLen;
                            break;

                        default:
                            len = 0xff; //不支持的命令或者出错
                            break;
                        }
                        if (SetupLen > len)
                        {
                            SetupLen = len; //限制总长度
                        }
                        len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen; //本次传输长度
                        memcpy(Ep0Buffer, pDescr, len);                                 //加载上传数据
                        SetupLen -= len;
                        pDescr += len;
                        break;
                    case USB_SET_ADDRESS:
                        if (UsbSetupBuf->wValueH)
                            SetupLen = 0xFF; //锟捷达拷USB锟借备锟斤拷址
                        else
                            SetupLen = UsbSetupBuf->wValueL; //锟捷达拷USB锟借备锟斤拷址
                        break;
                    case USB_GET_CONFIGURATION:
                        Ep0Buffer[0] = UsbConfig;
                        if (SetupLen >= 1)
                        {
                            len = 1;
                        }
                        break;
                    case USB_SET_CONFIGURATION:
                        // here
                        UsbConfig = UsbSetupBuf->wValueL;
                        if (UsbConfig)
                        {
                            Ready = 1; // set config命令一般代表usb枚举完成的标志
                            Endp2Busy = 0;
                        }
                        break;
                    case USB_GET_INTERFACE:
                        break;
                    case USB_CLEAR_FEATURE:                                                         // Clear Feature
                        if ((UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP) // 端点
                        {
                            switch (UsbSetupBuf->wIndexL)
                            {
                            case 0x82:
                                UEP2_CTRL = UEP2_CTRL & ~(bUEP_T_TOG | MASK_UEP_T_RES) | UEP_T_RES_NAK;
                                break;
                            case 0x81:
                                UEP1_CTRL = UEP1_CTRL & ~(bUEP_T_TOG | MASK_UEP_T_RES) | UEP_T_RES_NAK;
                                break;
                            case 0x02:
                                UEP2_CTRL = UEP2_CTRL & ~(bUEP_R_TOG | MASK_UEP_R_RES) | UEP_R_RES_ACK;
                                break;
                            default:
                                len = 0xFF; // 不支持的端点
                                break;
                            }
                        }
                        else
                        {
                            len = 0xFF; // 不是端点不支持
                        }
                        break;
                    case USB_SET_FEATURE:                               /* Set Feature */
                        if ((UsbSetupBuf->bRequestType & 0x1F) == 0x00) /* 设置设备 */
                        {
                            if ((((UINT16)UsbSetupBuf->wValueH << 8) | UsbSetupBuf->wValueL) == 0x01)
                            {
                                if (CfgDesc[7] & 0x20)
                                {
                                    /* 设置唤醒使能标志 */
                                }
                                else
                                {
                                    len = 0xFF; /* 操作失败 */
                                }
                            }
                            else
                            {
                                len = 0xFF; /* 操作失败 */
                            }
                        }
                        else if ((UsbSetupBuf->bRequestType & 0x1F) == 0x02) /* 设置端点 */
                        {
                            if ((((UINT16)UsbSetupBuf->wValueH << 8) | UsbSetupBuf->wValueL) == 0x00)
                            {
                                switch (((UINT16)UsbSetupBuf->wIndexH << 8) | UsbSetupBuf->wIndexL)
                                {
                                case 0x82:
                                    UEP2_CTRL = UEP2_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL; /* 设置端点2 IN STALL */
                                    break;
                                case 0x02:
                                    UEP2_CTRL = UEP2_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL; /* 设置端点2 OUT Stall */
                                    break;
                                case 0x81:
                                    UEP1_CTRL = UEP1_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL; /* 设置端点1 IN STALL */
                                    break;
                                default:
                                    len = 0xFF; /* 操作失败 */
                                    break;
                                }
                            }
                            else
                            {
                                len = 0xFF; /* 操作失败 */
                            }
                        }
                        else
                        {
                            len = 0xFF; /* 操作失败 */
                        }
                        break;
                    case USB_GET_STATUS:
                        Ep0Buffer[0] = 0x00;
                        Ep0Buffer[1] = 0x00;
                        if (SetupLen >= 2)
                        {
                            len = 2;
                        }
                        else
                        {
                            len = SetupLen;
                        }
                        break;
                    default:
                        len = 0xff; //操作失败
                        break;
                    }
                }
            }
            else
            {
                len = 0xff; //包长度错误
            }
            if (len == 0xff)
            {
                SetupReq = 0xFF;
                UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL; // STALL
            }
            else if (len <= THIS_ENDP0_SIZE) //上传数据或者状态阶段返回0长度包
            {
                UEP0_T_LEN = len;
                UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK; //默认数据包是DATA1，返回应答ACK
            }
            else
            {
                UEP0_T_LEN = 0; //虽然尚未到状态阶段，但是提前预置上传0长度数据包以防主机提前进入状态阶段
                UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK; //默认数据包是DATA1,返回应答ACK
            }
            break;
        case UIS_TOKEN_IN | 0: // endpoint0 IN
            switch (SetupReq)
            {
            case USB_GET_DESCRIPTOR:
            case HID_GET_REPORT:
                len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen; //本次传输长度
                memcpy(Ep0Buffer, pDescr, len);                                 //加载上传数据
                SetupLen -= len;
                pDescr += len;
                UEP0_T_LEN = len;
                UEP0_CTRL ^= bUEP_T_TOG; //同步标志位翻转
                break;
            case USB_SET_ADDRESS:
                USB_DEV_AD = USB_DEV_AD & bUDA_GP_BIT | SetupLen;
                UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
                break;
            default:
                UEP0_T_LEN = 0; //状态阶段完成中断或者是强制上传0长度数据包结束控制传输
                UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
                break;
            }
            break;
        case UIS_TOKEN_OUT | 0: // endpoint0 OUT
            len = USB_RX_LEN;
            if (SetupReq == 0x09)
            {
                // state LED
                STA_NumLock = !!(Ep0Buffer[0] & 0x01);
                STA_CapLock = !!(Ep0Buffer[0] & 0x02);
                STA_ScrLock = !!(Ep0Buffer[0] & 0x04);
            }
            UEP0_CTRL ^= bUEP_R_TOG; //同步标志位翻转
            break;
        default:
            break;
        }
        UIF_TRANSFER = 0; //写0清空中断
    }
    else if (UIF_BUS_RST) //设备模式USB总线复位中断
    {
        UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
        UEP1_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK;
        UEP2_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;
        USB_DEV_AD = 0x00;
        UIF_SUSPEND = 0; //清中断标志
        UIF_TRANSFER = 0;
        UIF_BUS_RST = 0;
        Endp2Busy = 0;
    }
    else if (UIF_SUSPEND) // USB总线挂起/唤醒完成
    {
        UIF_SUSPEND = 0;
        if (USB_MIS_ST & bUMS_SUSPEND) //挂起
        {
            Ready = 0;
        }
        else
        {
            Ready = 1;
            Endp2Busy = 0;
        }
    }
    else
    {                      //意外的中断,不可能发生的情况
        USB_INT_FG = 0xFF; //清中断标志
    }
}

UINT8 ErrFlag = 0;
UINT8 IO_ErrFlag = 0;


void main(void)
{
    UINT16 KEY_TEMP = 0;
    UINT16 WAIT_COUNT = 0;

    // set watch dog before system up
    WDOG_COUNT = 0;
    SAFE_MOD = 0x55;
    SAFE_MOD = 0xAA;
    GLOBAL_CFG |= bWDOG_EN;
    SAFE_MOD = 0x00;

    InitIO();
    CfgFsys();   // CH559时钟选择配置
	
	//wait 1s
    while (1){
		WDOG_COUNT = 0;
		WAIT_COUNT++;
		mDelaymS(10);
		if(WAIT_COUNT > 100){
			WAIT_COUNT = 0;
			LED_USB = 0;
			break;
		}
		if((WAIT_COUNT%5)==0)
			LED_USB = ~LED_USB;
	}

    memset(UserEp2Buf, 0, 64);
    USBDeviceInit(); // USB设备模式初始化
    EA = 1;          //允许单片机中断
    UEP1_T_LEN = 0;  //预使用发送长度一定要清空
    UEP2_T_LEN = 0;  //预使用发送长度一定要清空
    Ready = 0;
	
	//wait ready
    while (Ready == 0){
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
		if((WAIT_COUNT%5)==0)
			LED_USB = ~LED_USB;
	}
	//wait 1s ready keep
	
	WAIT_COUNT = 0;
    while (1){
		WDOG_COUNT = 0;
		WAIT_COUNT++;
		mDelaymS(10);
		if(Ready == 0){
			CH554SoftReset();
			while(1);
		}
		if(WAIT_COUNT > 100){
			WAIT_COUNT = 0;
			LED_USB = 0;
			break;
		}
		if((WAIT_COUNT%5)==0)
			LED_USB = ~LED_USB;
	}

    while (1)
    {
        WDOG_COUNT = 0; // WDOG reload

        Taiko_task(ReadIO());

		if(Ready == 0){
			CH554SoftReset();
			while(1);
		}
		
        if (Ready)
        {
            if (Endp2Busy == 0)
            {
                WAIT_COUNT = 0;
                Endp2Busy = 1;
                Enp2BlukIn();
            }
            else
            {
                WAIT_COUNT++;
                if (WAIT_COUNT >= 1000)
                    Ready = 0;
            }
        }
		
		if(Ready){
			LED_USB = ~LED_USB;
		}
		else{
			LED_USB = 0;
		}
        LED_ERR = ((ErrFlag | IO_ErrFlag) != 0);
    }
}
