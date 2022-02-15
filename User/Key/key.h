#ifndef __Key_H_
#define __Key_H_
//typedef unsigned char u8;
////typedef unsigned char u8;
//typedef unsigned short     int u16;
//typedef unsigned           int u32;
#include "GlobalValue.h"

extern volatile unsigned long SysTickCnt;

#define _HW_CONFIG_TRUE		(1)

#define TICKS_PER_SEC 100	//系统时钟每秒节拍数，100x10mS(T0)=1S
#define TICKS_PER_SEC_SOFTTIMER 100	//软定时器时钟每秒节拍数，100x10mS=1S

//==========================================================
//按键常数定义
#define LONGKEYCOUNT 	(TICKS_PER_SEC)	//长按键节拍数(1s)
//#define SHORTKEYCOUNT 	(TICKS_PER_SEC/20)	//短按键节拍数(50ms)
#define SHORTKEYCOUNT 	(TICKS_PER_SEC/30)	//短按键节拍数(33ms)
#define CONTINUEKEYCOUNT (TICKS_PER_SEC/7)	//持续按键节拍数(100ms)
#define HW_KEYBOARD_CONTINUE_SUPPORT	(_HW_CONFIG_TRUE)
//按键结构体声明
typedef struct 
{
	u8 value;	//键值
	u8 state;	//键有按下=TRUE
	u8 continuance;//持续按键有效=TRUE
}Key_TypeDef;
extern Key_TypeDef Keyboard;//键码结构体
#define LONG_PRESS		((u8)0x80)
#define 	Key_F1  	(0x25)
#define		Key_F2  	(0x20)
#define 	Key_F3  	(0x15)
#define 	Key_F4  	(0x0a)
#define		Key_F5 		(0x12)
#define		Key_Disp 	 (0x14)//9
#define		Key_SETUP	 (0x21)
#define		Key_FAST	(0x11)//OK
#define		Key_LEFT	(0X16)//LEFT
#define		Key_RIGHT  	(0X18)//RIGHT
#define		Key_UP		(0x26)//UP
#define		Key_DOWN	(0X29)//DOWN
#define		Key_NUM7	('7')//
#define		Key_NUM8	('8')//
#define		Key_COMP	(0x0D)//
#define		Key_NUM9	('9')
#define		Key_NUM4	('4')//
#define		Key_NUM5	('5')//退出  从新定义按键
#define		Key_NUM6	('6')
#define		Key_NUM1	('1')
#define		Key_NUM2	('2')
#define		Key_NUM3	('3')
#define		Key_NUM0	('0')
#define		Key_DOT		(0x2e)
#define		Key_BACK	(0x27)
#define		Key_LOCK	(0x23)
#define		Key_SYST	(0x19)
#define		Key_SAVE	(0x13)
#define		Key_FILE	(0x22)

#define 	L_Key_F1  	(LONG_PRESS | Key_F1)
#define		L_Key_F2  	(LONG_PRESS | Key_F2)
#define 	L_Key_F3  	(LONG_PRESS | Key_F3)
#define 	L_Key_F4  	(LONG_PRESS | Key_F4)

#define		Key_ADJ0	(0X0F)
#define		Key_BIAS	(23)
#define		Key_STOP	(0X06)
#define		Key_REST	(60)
#define		Key_TRIG	(7)
//#define		Key_POWER	(55)
#define		Key_PAGE	(0X05)//5520
#define		Key_START	(0X07)
#define		Key_VSET	(21)
#define		Key_EXIT	(100)//退出

#define KEY_NONE		((u8)0xFF)

#define _Hc164CpL GPIO_ClearValue(2, (1<<26))
#define _Hc164CpH GPIO_SetValue(2, (1<<26))
#define _Hc164DL GPIO_ClearValue(4, (1<<27))
#define _Hc164DH GPIO_SetValue(4, (1<<27))
void HW_Sendvalueto164(u8 value);
void HW_keyInt(void);
u8 HW_KeyScsn(void);
void Delay_Key(void);
u8 HW_KeyScsn1(void);
void SysTick_Handler (void);
void Delay (unsigned long tick);
void GPIO_Led_Configuration(void);

void Beep_on(void);
void Beep_Off(void);
void Pass_Led(void);
void Fail_led(void);

void Lock_LedOn(void);
void Lock_LedOff(void);
void Lock_Control_Off(void);
void Lock_Control_On(void);

void Bais_LedOff(void);
void Bais_LedOn(void);

void Beep_Off(void);
void Beep_on(void);

void Pro_PlcOn(void);
void Pro_Plcoff(void);

void Power_Off_led(void);
void Power_On_led(void);
void All_LedOff(void);
void All_LedOff1(void);
	
void Turnoff_backlight(void);
void Turnon_backlight(void);

void Key_Beep(void);
void Start_Lowdc(void);
void BeepCon(void);
void BeepGap(void);

void GPIO_LPC_Configration(void);

void Key_Scan(void);
void GPIO_WAN_Config(void);
void Wlan_Rst(void);
void Wlan_Cfg_L(void);
void HV_Ledon(void);
void HV_Ledoff(void);
void Touch_beep(void);
u8 Key_Read(void);
void Delay (unsigned long tick);
#endif

