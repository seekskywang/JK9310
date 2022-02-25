#include "Key/key.h"
#include "lpc177x_8x_gpio.h"
#include "GlobalValue.h"
volatile unsigned long SysTickCnt;
Key_TypeDef Keyboard;//键码结构体

static u8 LastKeyVal;//上次键值
static u8 LastKeyTicks;//按键节拍
static u8 ContinueKeyTicks;//持续按键节拍
void SysTick_Handler (void)
{
	SysTickCnt++;
}
void Beep_on(void)
{
	GPIO_SetValue(3, (1<<28));

}
void Start_Lowdc(void)
{
	GPIO_ClearValue(0, (1<<3));

}
void Beep_Off(void)
{
	GPIO_ClearValue(3, (1<<28));

}
void Pro_PlcOn(void)
{
//    GPIO_SetValue(2, (1<<10));
    GPIO_SetValue(1, (1<<15));
}
void Pro_Plcoff(void)
{
 //   GPIO_ClearValue(2, (1<<10));
	if(Tft_5520.Sys_Set.I_O)
		GPIO_ClearValue(1, (1<<15));
}

//void Stop_Plc(void)
//{
//    if(!(GPIO_ReadValue(0) & (1<<21)))
//        ;

//}
void Bais_LedOn(void)
{
	//GPIO_SetValue(1, (1<<14));

}

void Bais_LedOff(void)
{
	//GPIO_ClearValue(1, (1<<14));
}
void Lock_LedOn(void)
{
	GPIO_SetValue(1, (1<<0));

}
void Lock_LedOff(void)
{
	GPIO_ClearValue(1, (1<<0));
}
void Lock_Control_On(void)
{
	GPIO_ClearValue(1, (1<<9));
	

}
void Lock_Control_Off(void)
{
	GPIO_SetValue(1, (1<<9));

}
void Pass_Led(void)
{
	
    
    GPIO_SetValue(1, (1<<6));
	GPIO_ClearValue(4, (1<<26));
//  
	if(Tft_5520.Sys_Set.I_O)
	{
		GPIO_SetValue(1, (1<<14));
		GPIO_ClearValue(1, (1<<10));
	}
    
    

}

void Fail_led(void)
{	
    GPIO_SetValue(4, (1<<26));
	GPIO_ClearValue(1, (1<<6));
	if(Tft_5520.Sys_Set.I_O)
	{
		GPIO_SetValue(1, (1<<10));//
		GPIO_ClearValue(1, (1<<14));
	}
    
}

void All_LedOff(void)
{
	GPIO_ClearValue(1, (1<<6));
	GPIO_ClearValue(4, (1<<26));
	GPIO_ClearValue(2,(1<<0));
	GPIO_ClearValue(1, (1<<7));
	//	Turnoff_backlight();
	//GPIO_ClearValue(1, (1<<9));


}
void HV_Ledon(void)
{
	GPIO_SetValue(1, (1<<7));

}

void HV_Ledoff(void)
{
	GPIO_ClearValue(1, (1<<7));

}

void Power_On_led(void)
{
//	GPIO_SetValue(0, (1<<10));
//	GPIO_ClearValue(2, (1<<26));

}
void Power_Off_led(void)
{
//	GPIO_SetValue(2, (1<<26));
//	GPIO_ClearValue(0, (1<<10));

}
void Turnon_backlight(void)
{
	GPIO_SetValue(2, (1<<1));

}
void Turnoff_backlight(void)
{
	GPIO_ClearValue(2, (1<<1));


}
/*-------------------------PRIVATE FUNCTIONS------------------------------*/
/*********************************************************************//**
 * @brief		Delay function
 * @param[in]	tick - number milisecond of delay time
 * @return 		None
 **********************************************************************/
void Delay (unsigned long tick)
{
	unsigned long systickcnt;

	systickcnt = SysTickCnt;
	while ((SysTickCnt - systickcnt) < tick);
}
void Wlan_Rst(void)//网口复位
{
	GPIO_ClearValue(1, (1<<8));
	Delay(250);
	GPIO_SetValue(1, (1<<8));

}
void Wlan_Cfg_L(void)
{
	GPIO_ClearValue(1, (1<<9));

}
void Wlan_Cfg_H(void)
{
	GPIO_SetValue(1, (1<<9));

}
void GPIO_WAN_Config(void)
{
	GPIO_SetDir(1, (1<<9), GPIO_DIRECTION_OUTPUT);//cfg
	GPIO_SetDir(1, (1<<8), GPIO_DIRECTION_OUTPUT);//rst
	GPIO_SetValue(1, (1<<8));
	GPIO_SetValue(1, (1<<9));


}
void GPIO_LPC_Configration(void)
{
    GPIO_SetDir(0, (1<<26), GPIO_DIRECTION_INPUT);//启动按键
	GPIO_SetDir(2, (1<<15), GPIO_DIRECTION_INPUT);//复位按键
	GPIO_SetDir(1, (1<<15), GPIO_DIRECTION_OUTPUT);//测试中//
	GPIO_SetDir(1, (1<<14), GPIO_DIRECTION_OUTPUT);//合PLC//
	//GPIO_SetDir(0, (1<<22), GPIO_DIRECTION_OUTPUT);//电源继电器
    GPIO_SetDir(1, (1<<10), GPIO_DIRECTION_OUTPUT);//不合格LPC//


}
void GPIO_Led_Configuration(void)
{
	GPIO_SetDir(0, (1<<10), GPIO_DIRECTION_OUTPUT);//开机灯

	GPIO_SetDir(1, (1<<6), GPIO_DIRECTION_OUTPUT);//合格灯
	GPIO_SetDir(4, (1<<26), GPIO_DIRECTION_OUTPUT);//不合格灯
    
    
	GPIO_SetDir(2, (1<<0), GPIO_DIRECTION_OUTPUT);//START灯
	GPIO_SetDir(1, (1<<7), GPIO_DIRECTION_OUTPUT);//高压灯
	GPIO_SetDir(1, (1<<11), GPIO_DIRECTION_OUTPUT);//蜂鸣器
	GPIO_SetDir(2, (1<<1), GPIO_DIRECTION_OUTPUT);//背光
	
	GPIO_SetDir(0, (1<<3), GPIO_DIRECTION_OUTPUT);//低压DC启动


}
void GPIO_Key_Configuration(void)
{
	GPIO_SetDir(2, (1<<10), GPIO_DIRECTION_OUTPUT);//74LS164清零
	GPIO_SetDir(4, (1<<27), GPIO_DIRECTION_OUTPUT);//74LS164的数据端
	GPIO_SetDir(2, (1<<26), GPIO_DIRECTION_OUTPUT);//74LS164的cp
	
	GPIO_SetDir(2, (1<<14), GPIO_DIRECTION_INPUT);//4个读按键端口
	GPIO_SetDir(1, (1<<5), GPIO_DIRECTION_INPUT);
//	GPIO_SetDir(0, (1<<25), GPIO_DIRECTION_INPUT);
//	GPIO_SetDir(0, (1<<26), GPIO_DIRECTION_INPUT);
	
	
//	GPIO_SetDir(0, (1<<27), GPIO_DIRECTION_INPUT);//F快捷键
//	GPIO_SetDir(0, (1<<14), GPIO_DIRECTION_INPUT);
//	GPIO_SetDir(5, (1<<2),  GPIO_DIRECTION_INPUT);
//	GPIO_SetDir(4, (1<<26), GPIO_DIRECTION_INPUT);
//	GPIO_SetDir(0, (1<<19), GPIO_DIRECTION_INPUT);//开关键
//	GPIO_SetDir(1, (1<<15), GPIO_DIRECTION_INPUT);//LOCK
///*key1(P4_26)  key2(P2_22)  key3(P0_10)  */	
	GPIO_SetValue(2, (1<<10));//HC164控制脚
//	GPIO_SetDir(5, (1<<4), GPIO_DIRECTION_INPUT);
//	GPIO_SetDir(2, (1<<22), GPIO_DIRECTION_INPUT);
//	GPIO_SetDir(0, (1<<10), GPIO_DIRECTION_INPUT);	
}void _164Delaay(void)
{
	u8 i;
	for(i=0;i<10;i--)
	;

};
void HW_Sendvalueto164(u8 value)
{
	uint8_t i;
	_Hc164CpL;
	for(i=0;i<8;i++)
	{
		if((value&0x80)==0x80)
			_Hc164DH;
		else
			_Hc164DL;
		_Hc164CpH;
		_164Delaay();
		_Hc164CpL;
		value<<=1;
	}

}
void Key_Init(void)
{
	Keyboard.value=KEY_NONE;//键值为空
	Keyboard.state=FALSE;	//按键无效
	Keyboard.continuance=FALSE;//持续按键无效
	
	LastKeyVal=KEY_NONE;//上次键值
	LastKeyTicks=0;//按键节拍
	ContinueKeyTicks=0;//持续按键节拍
}
void HW_keyInt(void)
{
	GPIO_Key_Configuration();
	HW_Sendvalueto164(0xff);
	Key_Init();

}
const u8 Scan_Value[]={0x7f,0xbf,0xdf,0xef,0xf7,0xfb,0xfd,0xfe};
u8 HW_KeyScsn(void)
{
	static u8 keynum,keynum1;
	keynum=HW_KeyScsn1();
//	if(keynum!=0xff)
//	{
//		Delay(10);
//		keynum1=HW_KeyScsn1();
//		
//		if(keynum1==keynum)
//		{
//			//Key_Beep();
//			return keynum;
//			
//		}
//		else
//			return 0xff;
//	
//	
//	}
	return 0xff;

}
u8 HW_KeyScsn1(void)
{
	
	
//	u8 scanvalue=0;
	u8 key_value=0xff;
	u8 value=0xff;
	key_value=key_x;
	return key_value;

}
void Delay_Key(void)
{
	vu8 key;
	do{
		key=HW_KeyScsn();
	}while(key!=0xff);


}
void Key_Beep(void)
{
	Beep_on();
	Delay(500);
	Beep_Off();

}
void BeepCon(void)
{
	Beep_on();
//	Delay(50000);
//	Beep_Off();

}
void BeepGap(void)
{
	Beep_on();
	Delay(1500);
	Beep_Off();
	Delay(10000);
	Beep_on();
	Delay(1500);
	Beep_Off();
	Delay(10000);
	Beep_on();
	Delay(1500);
	Beep_Off();
	Delay(10000);
	Beep_on();
	Delay(1500);
	Beep_Off();
	Delay(10000);
	Beep_on();
	Delay(1500);
	Beep_Off();
}
void Touch_beep(void)
{
	Beep_on();
	Delay(200);
	Beep_Off();

}

u8 Key_Read(void)
{
	if (Keyboard.state)	//有键按下
	{
		Keyboard.state=FALSE;
		//Beep_One();	//蜂鸣器响一声
		return (Keyboard.value);
	}
	//可直接处理电源等通用按键
	return (KEY_NONE);	
}

u8 Key_Read_WithTimeOut(u8 ticks)
{
//	if (ticks)//定时等待按键
//		{
			SetSoftTimer(KEYBOARD_SOFTTIMER,ticks);//设置按键延时周期(即软定时器设置,50ms周期)
				
			while ((!GetSoftTimerOut(KEYBOARD_SOFTTIMER))&&(!Keyboard.state)) //计时未到并且无按键按下
				{
//					RunOnKeyFree();//在按键等待过程中处理其他业务
				}
//		}
//	else//无限等待按键
//		{
//			while (!Keyboard.state) 
//				{
//					RunOnKeyFree();//在按键等待过程中处理其他业务
//				}			
//		}
	return Key_Read();//按键更新
}
//==========================================================
//函数名称：Key_Scan
//函数功能：按键扫描
//入口参数：无
//出口参数：无
//创建日期：2014.09.28 
//修改日期：2014.09.28 11:01
//备注说明：由10mS中断调用
//==========================================================
void Key_Scan(void)
{
	u8 keyval;
	
	keyval = HW_KeyScsn1();//按键识别，返回键值

	//按键长短识别
	if (keyval==KEY_NONE)//无键
	{
		if (LastKeyVal)	//上次有键，短按键判别
		{
			if ( (LastKeyTicks>SHORTKEYCOUNT) && (LastKeyTicks<LONGKEYCOUNT) )
			{
				Keyboard.value=LastKeyVal;	//键值
				Keyboard.state=TRUE;		//有按键
			}
		}
		LastKeyVal=KEY_NONE;
		LastKeyTicks=0;
		Keyboard.continuance=FALSE; //持续按键	
	}
	else	//有键
	{
		if (LastKeyVal==keyval)//上次按键和本次按键相同
		{
			if (LastKeyTicks<LONGKEYCOUNT+1)//长按键节拍数100x10mS
				LastKeyTicks++;
			
			if (LastKeyTicks==LONGKEYCOUNT)//等于长按键节拍数
			{
				ContinueKeyTicks=0;//持续按键节拍
				keyval|=LONG_PRESS;	//长按键
				Keyboard.value=keyval;	//键值
				Keyboard.state=TRUE;	//有按键
//				Keyboard.continuance=FALSE; //持续按键
				Keyboard.continuance=TRUE; //持续按键
			}
			else if (LastKeyTicks>LONGKEYCOUNT)//大于长按键节拍数
			{
				if(HW_KEYBOARD_CONTINUE_SUPPORT)//持续按键有效判别
				{
					keyval|=LONG_PRESS;	//长按键
					Keyboard.value=keyval;//键值
//					Keyboard.state=TRUE;//有按键
					Keyboard.continuance=TRUE; //持续按键
					ContinueKeyTicks++;
					if(ContinueKeyTicks>CONTINUEKEYCOUNT)//持续按键节拍数
					{
						ContinueKeyTicks=0;//持续按键节拍
						if(Keyboard.state==FALSE)//按键已读取
							Keyboard.state=TRUE;//有按键
					}
				}
			}					
		}
		else//上次按键和本次按键不同
		{
			LastKeyVal=keyval;
			LastKeyTicks=0;
		}
	}
}


