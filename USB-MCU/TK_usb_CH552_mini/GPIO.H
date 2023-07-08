#ifndef _GPIO_H_
#define _GPIO_H_
#include "./Public/CH552.H"
#include "./Public/Debug.H"

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


/* Check of Flash Operation */ 
#define DEF_FLASH_OP_CHECK1     0xAA
#define DEF_FLASH_OP_CHECK2     0x55   

/* Flash Operation Flags */    
extern UINT8 Flash_Op_Check_Byte1;
extern UINT8 Flash_Op_Check_Byte2;


UINT8 load_mode(void);
bit save_mode(UINT8 mode);
void test(void);

#define MODE_NS_GP 		0
#define MODE_KEYBOARD 	1
#define MODE_KB_LMT  	2

#define MODE_STARTUP  	MODE_KEYBOARD
#define MODE_MAX  	MODE_KB_LMT



#endif
