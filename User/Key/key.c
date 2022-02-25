#include "Key/key.h"
#include "lpc177x_8x_gpio.h"
#include "GlobalValue.h"
volatile unsigned long SysTickCnt;
Key_TypeDef Keyboard;//����ṹ��

static u8 LastKeyVal;//�ϴμ�ֵ
static u8 LastKeyTicks;//��������
static u8 ContinueKeyTicks;//������������
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
void Wlan_Rst(void)//���ڸ�λ
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
    GPIO_SetDir(0, (1<<26), GPIO_DIRECTION_INPUT);//��������
	GPIO_SetDir(2, (1<<15), GPIO_DIRECTION_INPUT);//��λ����
	GPIO_SetDir(1, (1<<15), GPIO_DIRECTION_OUTPUT);//������//
	GPIO_SetDir(1, (1<<14), GPIO_DIRECTION_OUTPUT);//��PLC//
	//GPIO_SetDir(0, (1<<22), GPIO_DIRECTION_OUTPUT);//��Դ�̵���
    GPIO_SetDir(1, (1<<10), GPIO_DIRECTION_OUTPUT);//���ϸ�LPC//


}
void GPIO_Led_Configuration(void)
{
	GPIO_SetDir(0, (1<<10), GPIO_DIRECTION_OUTPUT);//������

	GPIO_SetDir(1, (1<<6), GPIO_DIRECTION_OUTPUT);//�ϸ��
	GPIO_SetDir(4, (1<<26), GPIO_DIRECTION_OUTPUT);//���ϸ��
    
    
	GPIO_SetDir(2, (1<<0), GPIO_DIRECTION_OUTPUT);//START��
	GPIO_SetDir(1, (1<<7), GPIO_DIRECTION_OUTPUT);//��ѹ��
	GPIO_SetDir(1, (1<<11), GPIO_DIRECTION_OUTPUT);//������
	GPIO_SetDir(2, (1<<1), GPIO_DIRECTION_OUTPUT);//����
	
	GPIO_SetDir(0, (1<<3), GPIO_DIRECTION_OUTPUT);//��ѹDC����


}
void GPIO_Key_Configuration(void)
{
	GPIO_SetDir(2, (1<<10), GPIO_DIRECTION_OUTPUT);//74LS164����
	GPIO_SetDir(4, (1<<27), GPIO_DIRECTION_OUTPUT);//74LS164�����ݶ�
	GPIO_SetDir(2, (1<<26), GPIO_DIRECTION_OUTPUT);//74LS164��cp
	
	GPIO_SetDir(2, (1<<14), GPIO_DIRECTION_INPUT);//4���������˿�
	GPIO_SetDir(1, (1<<5), GPIO_DIRECTION_INPUT);
//	GPIO_SetDir(0, (1<<25), GPIO_DIRECTION_INPUT);
//	GPIO_SetDir(0, (1<<26), GPIO_DIRECTION_INPUT);
	
	
//	GPIO_SetDir(0, (1<<27), GPIO_DIRECTION_INPUT);//F��ݼ�
//	GPIO_SetDir(0, (1<<14), GPIO_DIRECTION_INPUT);
//	GPIO_SetDir(5, (1<<2),  GPIO_DIRECTION_INPUT);
//	GPIO_SetDir(4, (1<<26), GPIO_DIRECTION_INPUT);
//	GPIO_SetDir(0, (1<<19), GPIO_DIRECTION_INPUT);//���ؼ�
//	GPIO_SetDir(1, (1<<15), GPIO_DIRECTION_INPUT);//LOCK
///*key1(P4_26)  key2(P2_22)  key3(P0_10)  */	
	GPIO_SetValue(2, (1<<10));//HC164���ƽ�
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
	Keyboard.value=KEY_NONE;//��ֵΪ��
	Keyboard.state=FALSE;	//������Ч
	Keyboard.continuance=FALSE;//����������Ч
	
	LastKeyVal=KEY_NONE;//�ϴμ�ֵ
	LastKeyTicks=0;//��������
	ContinueKeyTicks=0;//������������
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
	if (Keyboard.state)	//�м�����
	{
		Keyboard.state=FALSE;
		//Beep_One();	//��������һ��
		return (Keyboard.value);
	}
	//��ֱ�Ӵ����Դ��ͨ�ð���
	return (KEY_NONE);	
}

u8 Key_Read_WithTimeOut(u8 ticks)
{
//	if (ticks)//��ʱ�ȴ�����
//		{
			SetSoftTimer(KEYBOARD_SOFTTIMER,ticks);//���ð�����ʱ����(����ʱ������,50ms����)
				
			while ((!GetSoftTimerOut(KEYBOARD_SOFTTIMER))&&(!Keyboard.state)) //��ʱδ�������ް�������
				{
//					RunOnKeyFree();//�ڰ����ȴ������д�������ҵ��
				}
//		}
//	else//���޵ȴ�����
//		{
//			while (!Keyboard.state) 
//				{
//					RunOnKeyFree();//�ڰ����ȴ������д�������ҵ��
//				}			
//		}
	return Key_Read();//��������
}
//==========================================================
//�������ƣ�Key_Scan
//�������ܣ�����ɨ��
//��ڲ�������
//���ڲ�������
//�������ڣ�2014.09.28 
//�޸����ڣ�2014.09.28 11:01
//��ע˵������10mS�жϵ���
//==========================================================
void Key_Scan(void)
{
	u8 keyval;
	
	keyval = HW_KeyScsn1();//����ʶ�𣬷��ؼ�ֵ

	//��������ʶ��
	if (keyval==KEY_NONE)//�޼�
	{
		if (LastKeyVal)	//�ϴ��м����̰����б�
		{
			if ( (LastKeyTicks>SHORTKEYCOUNT) && (LastKeyTicks<LONGKEYCOUNT) )
			{
				Keyboard.value=LastKeyVal;	//��ֵ
				Keyboard.state=TRUE;		//�а���
			}
		}
		LastKeyVal=KEY_NONE;
		LastKeyTicks=0;
		Keyboard.continuance=FALSE; //��������	
	}
	else	//�м�
	{
		if (LastKeyVal==keyval)//�ϴΰ����ͱ��ΰ�����ͬ
		{
			if (LastKeyTicks<LONGKEYCOUNT+1)//������������100x10mS
				LastKeyTicks++;
			
			if (LastKeyTicks==LONGKEYCOUNT)//���ڳ�����������
			{
				ContinueKeyTicks=0;//������������
				keyval|=LONG_PRESS;	//������
				Keyboard.value=keyval;	//��ֵ
				Keyboard.state=TRUE;	//�а���
//				Keyboard.continuance=FALSE; //��������
				Keyboard.continuance=TRUE; //��������
			}
			else if (LastKeyTicks>LONGKEYCOUNT)//���ڳ�����������
			{
				if(HW_KEYBOARD_CONTINUE_SUPPORT)//����������Ч�б�
				{
					keyval|=LONG_PRESS;	//������
					Keyboard.value=keyval;//��ֵ
//					Keyboard.state=TRUE;//�а���
					Keyboard.continuance=TRUE; //��������
					ContinueKeyTicks++;
					if(ContinueKeyTicks>CONTINUEKEYCOUNT)//��������������
					{
						ContinueKeyTicks=0;//������������
						if(Keyboard.state==FALSE)//�����Ѷ�ȡ
							Keyboard.state=TRUE;//�а���
					}
				}
			}					
		}
		else//�ϴΰ����ͱ��ΰ�����ͬ
		{
			LastKeyVal=keyval;
			LastKeyTicks=0;
		}
	}
}


