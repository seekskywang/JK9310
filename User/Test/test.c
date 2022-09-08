#include "test.h"
#include "LCD/lpc177x_8x_lcd.h"
#include "Globalvalue/GlobalValue.h"
#include "use_disp.h"
#include "EX_SDRAM/EX_SDRAM.H"
#include "stdint.h"
#include "key/key.h"
//#include "uart.h"
#include "timer.h"
#include "lpc177x_8x_eeprom.h"
#include "math.h"
#include  "usbhost_fat.h"
#include "lpc177x_8x_clkpwr.h"
#include "LCD/logo.h"
#include "stdlib.h"
#include "debug_frmwrk.h"
#include "lpc177x_8x_gpio.h"
#include <string.h>
#include "spi_ad_da/spi_da_ad.h"
#include "lpc177x_8x_rtc.h"
#include  "ff.h"
#include  "touch.h"
#include "GUI.h"
#include    "cpld.h"
#include    "stdbool.h"
#include    "AT45DB.h"
#include "scpi/scpi.h"
#include "scpi-def.h"
//#ifdef ZC5520 
// #include "5520.h"
// #else
// #include "2683.h"
// #endif
 

#define POWERON_DISP_TIME (200)	//������ʾ������ʱ20*100mS=2s









//#include "open.h"
//#include "lpc177x_8x_rtc.h"
 FATFS fs;         /* Work area (file system object) for logical drive */
FIL fsrc;         /* file objects */   
FRESULT res;
UINT br;
char sendbuff[20];
char sendbuff1[20];
char sendbuff2[20];
char sendbuff3[20];
const u8 IR_Range_dot[5]={3,2,1,0,0};
uint8_t inputselnum;
uint8_t clearflag;
uint8_t clearstart;
uint8_t osgetstart;
uint8_t osgetflag;
uint8_t testflag;
const uint8_t USB_dISPVALUE[][9]=
{
	"RH_FAIL ",
	"RL_FAIL ",
	"VH_FAIL ",
	"VL_FAIL ",
	"ALL_FAIL",
	"ALL_PASS"
};

uint32_t keynum;
uint8_t pageflag;
uint16_t rangedelay;
size_t SCPI_Write(scpi_t * context, const char * data, size_t len) {
    (void) context;	
	if (len > 0)
	{	
		for(int i = 0;i < len;i++)
		{
			UARTPutChar(LPC_UART3,*(data+i) );
		}
//		for (int i = 0;i < len;i++)
//		{
//			osMessageQueuePut(cmdTxQueueHandle,&data[i],NULL,0U);
//		}
//		LL_USART_EnableIT_TXE(USART1);
	}
	return len;
}

scpi_result_t SCPI_Flush(scpi_t * context) {
    (void) context;

    return SCPI_RES_OK;
}

int SCPI_Error(scpi_t * context, int_fast16_t err) {
    (void) context;

//    printf("**ERROR: %d, \"%s\"\r\n", (int16_t) err, SCPI_ErrorTranslate(err));
    return 0;
}

scpi_result_t SCPI_Control(scpi_t * context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val) {
    (void) context;

    if (SCPI_CTRL_SRQ == ctrl) {
//        printf("**SRQ: 0x%X (%d)\r\n", val, val);
    } else {
//        printf("**CTRL %02x: 0x%X (%d)\r\n", ctrl, val, val);
    }
    return SCPI_RES_OK;
}

scpi_result_t SCPI_Reset(scpi_t * context) {
    (void) context;

//    printf("**Reset\r\n");
    return SCPI_RES_OK;
}

scpi_result_t SCPI_SystemCommTcpipControlQ(scpi_t * context) {
    (void) context;

    return SCPI_RES_ERR;
}

const u8 DOT_POS[6]=
{	
	2,
	1,
	3,
	2,
	0
};
//#include "debug_frmwrk.h"
Cal_TypeDef Cal[9];
LPC_RTC_TypeDef RtcSet;
const vu8 Uart_Ordel[]={0x60,0x70,0x71,0x80,0x90,0xa0,0xb0,0xc0,0xe0};
const vu8 READDATA[7]={0xAB,0x01,0x06,0x03,0x08,0xbf,'\0'};
const vu8 Disp_Main_Ord[][3]={
	{1,1,0},
	{1,1,1},
	{1,1,2},
	{1,1,3},//Cp
	
	{0,1,0},
	{0,1,1},
	{0,1,3},//Cs
	{1,0,0},
	{1,0,0},
	{1,0,0},
	{1,0,1},//Lp
	{1,0,3},
	{1,0,4},
	{1,0,0},
	{0,0,0},//Ls
	{0,0,1},
	{0,0,5},
	{0,0,4},
	{0,3,6},//Z
	{0,3,7},
	{0,4,6},//Y
	{0,4,7},
	{0,2,8},//R
	{1,2,1},
	{0,2,1},
	{1,1,0},
	{1,5,9},//GB
	//{1,1,0},
	};
union U1
{
    uint8_t s[4];
    float sendI;
};

union U1 USENDI;
const u32 RANGE_RATE[6]=
{
	1300,1300,45000,450000	
};
//==========================================================
//�������ƣ�Power_Process
//�������ܣ��ϵ紦��
//��ڲ�������
//���ڲ�������
//�������ڣ�2015.10.26
//�޸����ڣ�2015.10.26 08:53
//��ע˵������������SET����У׼����ģʽ
//==========================================================

void Power_Process(void)
{

//	u8 rc;
	u32 i;
	u8 temp[4];
		u32 FlashID;
		Pc_Sendflag=0;
	HW_keyInt();
	 //Key_Init();
	GPIO_Led_Configuration();
    CPLD_GPIO();
//	SPI_ADDA_GPIO();
//	GPIO_Key_Configuration();
//	HV_Out_Gpioint();//��ѹ�����������
	debug_frmwrk_init();
	Uart3_init(9600);
	SDRAM_Init();
//	Uart3_init(0);
//	GLCD_Ctrl (FALSE);
//	GLCD_Init (LogoPic.pPicStream, NULL);
//	GLCD_Ctrl (TRUE);
//	Beep_on();
//	Disp_open();
//	lcd_image((uint8_t *)gImage_5520);
    GPIO_Led_Configuration();
    GPIO_LPC_Configration();
     V_DA_out(0);
    Sing_out_C(0);
    Short_out(0);
    FRB_out(0);
	
	Beep_on();
    Sing_out_C(0);
//	HW_Sendvalueto164(0);
	Bais_LedOff();
	Lock_LedOff();
	Pass_Led();
	init_timer ( 1,10 ) ;//10MS��ʱ
	init_timer ( 0,10 ) ;//10MS��ʱ
	enable_timer(0);
	enable_timer(1);
	NVIC_SetPriority(TIMER0_IRQn, 2);
	NVIC_SetPriority(TIMER1_IRQn, 2);
	InitGlobalValue();//��ʼ��ȫ�ֱ���
	Uart_Send_Flag=0;
	EEPROM_Init();
    save_flag=0;
	ReadSavedata();
//    SSPIx_Init();
//    AT45DBXX_Read_ID(temp);
//    FlashID = SPI_FLASH_ReadID();
//    Read_FileDate(0);
    SetDate_Comp();
	Delay(100);
	ReadCal();
	SetData_High_Low_Comp();//����


	Host_Init(); 
    Host_EnumDev();
	ARC_out(1);
	ARC_out(0);
//	if(usb_oenflag)	
//	{
//		res=f_mount(0,&fs);
//		Host_DelayMS(20);
//		res=f_mkdir("0:/ZC");
//		res=f_mkdir("0:/ZC");
//		res = f_open( &fsrc , "0:/ZC/ZC5520.xls" , FA_CREATE_NEW | FA_WRITE);//FA_CREATE_NEW | FA_WRITE);		

//		if ( res == FR_OK )
//		{ 
//			f_lseek(&fsrc,fsrc.fsize);
//			res = f_write(&fsrc, "���	��ѹ	����	ʱ��	��ѡ\t\n", sizeof("���	��ѹ	����	ʱ��	��ѡ\t\n"), &br);     
//		  f_close(&fsrc); 
//		}
//		
//	}


	
	
	
//	i=0;
	if(CONFIG_TIME!=RTC_ReadGPREG (LPC_RTC, 0))//���Ĵ������ݽ����ж�
	{
		RTC_Init(LPC_RTC);

	/* Enable rtc (starts increase the tick counter and second counter register) */
	
		
		LPC_RTC->SEC=0;
		LPC_RTC->MIN=0;
		LPC_RTC->HOUR=0;
		LPC_RTC->DOY=3;
		LPC_RTC->DOW=8;
		LPC_RTC->DOM=1;
		
		LPC_RTC->MONTH=1;
		LPC_RTC->YEAR=2018;
		RTC_WriteGPREG (LPC_RTC,0 ,CONFIG_TIME);
		RTC_ResetClockTickCounter(LPC_RTC);
		RTC_Cmd(LPC_RTC, ENABLE);
		//RTC_SetFullTime (LPC_RTC, &RtcSet);
	
	}
//	LPC_GPIOINT->IO0IntEnR |= (0<<5);//ʹ��IO�������ش���
//	GPIO_ClearInt(0, 1<<5);
//	LPC_GPIOINT->IO0IntEnR |= (0<<6);//ʹ��IO�������ش���
//	GPIO_ClearInt(0, 1<<6);
//	LPC_GPIOINT->IO0IntEnR |= (0<<7);//ʹ��IO�������ش���
//	GPIO_ClearInt(0, 1<<7);
	GPIO_IntCmd(0, 1<<5, 0);//p0_29 ???��???D??
	GPIO_ClearInt(0, 1<<5);
    GPIO_IntCmd(0, 1<<6, 0);//p0_29 ???��???D??
	GPIO_ClearInt(0, 1<<6);
    GPIO_IntCmd(0, 1<<7, 0);//p0_29 ???��???D??
	GPIO_ClearInt(0, 1<<7);
	NVIC_SetPriority(GPIO_IRQn, 0);
////	NVIC_DisableIRQ(GPIO_IRQn);
	NVIC_EnableIRQ(GPIO_IRQn);
//	tp_dev.init();	
	Beep_Off();
    Delay(20);
//    SDRAM_Init();
	GUI_Init();
	GUI_SetBkColor(LCD_COLOR_TEST_BACK);//GUI_DARKBLUE
	GUI_SetColor(GUI_WHITE);
	GUI_Clear();
    GUI_SetFont(GUI_FONT_32B_ASCII);//
    GUI_UC_SetEncodeUTF8();
    
    FMC_Int();
	SCPI_Init(&scpi_context,
					scpi_commands,
					&scpi_interface,
					scpi_units_def,
					SCPI_IDN1, SCPI_IDN2, SCPI_IDN3, SCPI_IDN4,
					scpi_input_buffer, SCPI_INPUT_BUFFER_LENGTH,
					scpi_error_queue_data, SCPI_ERROR_QUEUE_SIZE);
    i=0;
//	memset(&U9001_Save_sys.U9001_Testconfg,0,sizeof(U9001_Save_sys.U9001_Testconfg));
	while(GetSystemStatus()==SYS_STATUS_POWER)
	{

		i++;
		if(i>POWERON_DISP_TIME)//��ʱ20*100mS=2s���Զ��˳�
		{
			SetSystemStatus(SYS_STATUS_IDEM);//����״̬

			
		}
//        if(Key_Read()==10)
//		SetSystemStatus(SYS_STATUS_USERDEBUG);
        Delay(100);
     
//        key=Uart_Process();
//        if(keyvalue!=0xff)
//        {

//            GUI_DispHexAt(keyvalue,10,10,3);
//        
//        }
		switch(keyvalue)
		{
//			case KEY_SET:		//���ü�
			case Key_UP:		//�������ü�
				SetSystemStatus(SYS_STATUS_USERDEBUG);//����״̬
//				Beep_One();//��������һ��
				break;
	
//			case KEY_UP:		//�ϼ�
//			case L_KEY_UP:		//�����ϼ�
//				break;
//	
			case Key_DOWN:		//�¼�
//			case L_KEY_DOWN:	//�����¼�
				Tft_5520.Touch_Save.passeord=0X00;
				break;
//	
//			case KEY_LEFT:		//���
//			case L_KEY_LEFT:	//�������
//				break;
//
//			case KEY_RIGHT:		//�Ҽ�
//			case L_KEY_RIGHT:	//�����Ҽ�
//				break;
//
//			case KEY_ENTER:		//ȷ�ϼ�
//			case L_KEY_ENTER:	//����ȷ�ϼ�
//				break;
//	
//			case KEY_START:		//������
//			case L_KEY_START:	//����������
//				break;
//	
//			case KEY_RESET:		//��λ��
//			case L_KEY_RESET:	//������λ��
//				i=POWERON_DISP_TIME;//��ʾ��ʱ����
//				break;
//			
			default:
				break;
		}
		
		//Delay_1ms(50);
		//Range_Control(3);
	}
	

}
u16 ParameterLimit[][2]=
{
    {2,999},
    {0,999},
    {0,999},
    {0,1},
    {0,1},
    {0,1},
    {0,50},
    {0,1},
    {0,1},
    {0,1},
    {0,3},
    {0,1},
    
    {0,1},
    {0,2},
    {0,10},
};
//�������
void Idem_Process(void)
{

	 vu16 i;
//	u8 list=1;//��
//    u8 line=0;//��
//    u8 item=0;
//    u8 disp_all=1;
//	vu8 key;
	DISP_FLAG=TRUE;
//    GUI_SetColor(LCD_COLOR_TEST_BACK);
	GUI_SetBkColor(LCD_COLOR_TEST_BACK);
	GUI_Clear();
//	Sin_out(1);//Ƶ��ѡ��
//    V_DA_out(0xfff);
	//IdelButton_Tab
//    _4094_databuff[0]=0x80;
//    Out_4094(_4094_databuff);
//    PLC_OutProg();
    Range_Control(0);
    for(i=0;i<10;i++)//����Ĳ��Խ��
    {
        Save_TestValue[i].text_flag=0;
        Save_TestValue[i].Text_value=0;
        Save_TestValue[i].Text_vot=0;
        Save_TestValue[i].Text_time=0;
			  Save_TestValue[i].text_unit=0;
    }
    keyvalue=KEY_NONE;
	if(osgetstart == 0)
	{
		if(U9001_Save_sys.U9001_Testconfg.start_setp == 0)
		{
			U9001_Save_sys.U9001_save.current_step=1;
		}else{
			U9001_Save_sys.U9001_save.current_step = U9001_Save_sys.U9001_save.start_step;
		}
	}
	
    Test_mid.set_item=U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter;//��ǰ����
	pageflag = 0;//������ʾ
	Uart0_Send(0x02);//�ص�
	PLC_OutOff();//��PLC����
	PLC_CompOff();//��PLC��ѡ
	while(GetSystemStatus()==SYS_STATUS_IDEM)
	{
        Disp_Time( );
		if(Pc_Sendflag)//�����λ�������������ݣ��ౣ�沢ת������λ��
        {
            Pc_Sendflag=0;
            Savetoeeprom();//������������
           DISP_FLAG=TRUE;
			
        
        }
		if(DISP_FLAG==TRUE)
		{
			
              
			GUI_SetColor(LCD_COLOR_TEST_BACK);
			GUI_FillRect(0,24,380,270);
			GUI_SetColor(GUI_WHITE);
			if(U9001_Save_sys.U9001_save.disp)
			{
				pageflag = 1;//�б���ʾ
        Disp_Idel_list();
        disp_TestMSG(GetSystemMessage());
			}
			else
			{
					pageflag = 0;//������ʾ
					Disp_Idel_Item();
					Disp_Testvalue(0);   
			}
			
			Disp_IdelButton();
             
			DISP_FLAG=FALSE;

		}
        if(READ_START()==0 || clearstart==1 || osgetstart == 1)
        {
            SetSystemStatus(SYS_STATUS_START);
            SetSystemMessage(MSG_RAMP);
            Uart0_Send(0xa1);
						PLC_OutProg();//��PLC����
						clearstart=0;
						osgetstart=0;
        }
        if(READ_STOP()==0)
        {
            
            V_DA_out(0);
            Sing_out_C(0);
            Short_out(0);
            FRB_out(0);
            Uart0_Send(0xa0);
        }
        
//	  key=Key_Read();
//        keyvalue=Key_Read();;
		if(keyvalue!=KEY_NONE)
//		if(key!=KEY_NONE)
		{	
			
			
			switch(keyvalue)
			{
                case Key_LOCK:
//                   SetSystemStatus(SYS_STATUS_USERDEBUG);
                    break;
                case Key_SAVE://����
					Key_Beep();
                    if(save_flag)
                    {
                        save_flag=0;
                        Uart0_Send(0xa7);
                    }
                    else
                    {
                        save_flag=1;
                         Uart0_Send(0xa3);
                    }
                    
                    break;
				case Key_FAST:
				
				break;

				case Key_ADJ0:
                   
				break;
				case Key_RIGHT:
//					Key_Beep();
//                    DISP_FLAG=TRUE;
                    
				break;
				case Key_LEFT:
//					Key_Beep();
//                     DISP_FLAG=TRUE;
                   
				break;
				case Key_DOWN:
				
				break;
				case Key_UP:

				break;
				case Key_PAGE://��ҳ
					Key_Beep();
                    SetSystemStatus(SYS_STATUS_SETUPTEST);
				break;
				case Key_STOP:
                    
                    break;
				case Key_Disp:
//					list=0;
                    //SetSystemStatus(SYS_STATUS_IDEM);
				break;
                case Key_START:
                    
//                    SetSystemStatus(SYS_STATUS_TEST);
//					SetSystemMessage(MSG_TEST);
                
                break;

                
                case Key_COMP://7 8
					SetSystemStatus(SYS_STATUS_TEST);
					DISP_FLAG=TRUE;

                    
                break;
				case Key_SYST://
//					SetSystemStatus(SYS_STATUS_SYSSET);
					break;
                case Key_FILE://
//					SetSystemStatus(SYS_STATUS_FILE);
					break;
				case Key_SETUP://
					Key_Beep();
					SetSystemStatus(SYS_STATUS_SETUPTEST);
					break;
                case Key_F1://
					Key_Beep();
                    DISP_FLAG=TRUE;
					U9001_Save_sys.U9001_save.disp=0;
					break;
                case Key_F2://
					Key_Beep();
                    DISP_FLAG=TRUE;
					U9001_Save_sys.U9001_save.disp=1;
					break;
				case Key_F5://
//					SetSystemStatus(SYS_STATUS_RANGE);
					break;
				default:
				break;
					
			}
		keyvalue=KEY_NONE;
            key_x=KEY_NONE;
		}
        
	}


}

//��ͣ����
void Abroat_Process(void)
{

//	vu32 keynum=0;
	vu8 key;
	Delay_Key();
	Disp_big_HZ(GetSystemMessage());
	clear_flag=0;
    ARC_out(0);
	while(GetSystemStatus()==SYS_STATUS_ABORT)
	{
		Disp_Time( );
		Colour.black=LCD_COLOR_TEST_MID;
//		Uart_Process();
		Colour.Fword=LCD_COLOR_WHITE;
                if(READ_STOP()==0)
        {
//            V_DA_out(0);
//            Sing_out_C(0);
//            Short_out(0);
//            FRB_out(0);
				SetSystemMessage(MSG_IDLE);//ϵͳ��Ϣ-��ͣ����
				SetSystemStatus(SYS_STATUS_IDEM);//ϵͳ״̬-��ͣ����
        }
	   key=Key_Read();
		
		if(key!=KEY_NONE)
		{	
			
			Key_Beep();
			switch(key)
			{
				
				case Key_Disp:
                    SetSystemStatus(SYS_STATUS_TEST);
				break;
				case Key_SETUP:
                    //SetSystemStatus(SYS_STATUS_SETUPTEST);
				break;
                
                case Key_COMP:
                   
                    
                    
                    break;
                case Key_VSET:
                   
                    break;
				case Key_EXIT:
                    
                    
				break;
				case Key_RIGHT:
					

				break;
				case Key_LEFT:
					

						
				break;
				case Key_DOWN:
					
					
				break;
				case Key_UP:
					
				break;
				

				case Key_PAGE://��ҳ
                   
					
				break;
		
				case Key_STOP:
                    SetSystemStatus(SYS_STATUS_IDEM);
					SetSystemMessage(MSG_IDLE);
					
					
				break;
                case Key_START:
                    SetSystemStatus(SYS_STATUS_TEST);
                    
                break;
				case Key_REST:
					Power_Off_led();
				break;

				default:
				break;
					
			}
		
		
		}
	
	
	}

}
//���������ʾ
void Finsh_Process(void)
{

//	vu32 keynum=0;
//	float ddd,eee;
	vu8 key;

	clear_flag=0;
	
	//res=f_mkdir("0:/ZC");
//	res = f_open( &fsrc , "0:/ZC/ZC5520.xls" , FA_CREATE_NEW);//FA_CREATE_NEW | FA_WRITE);		

//    if ( res == FR_OK )
//    { 
//		f_lseek(&fsrc,fsrc.fsize);
//		res = f_write(&fsrc, "���	��ѹ	����	ʱ��	��ѡ\t\n", sizeof("���	��ѹ	����	ʱ��	��ѡ\t\n"), &br);     
//      f_close(&fsrc); 
//    }
//	else if(res == FR_EXIST)
    FRB_out(0);//���������
//	Disp_Usbflag(usb_oenflag);
//	if(Tft_5520.Sys_Set.U_Switch)
//	{
//		res = f_open( &fsrc , "0:/ZC/ZC5520.xls" ,  FA_WRITE|FA_OPEN_ALWAYS);
//		if(res == FR_OK)
//		{
//			f_lseek (&fsrc,fsrc.fsize);
//			res = f_write(&fsrc, U_Storebuff, strlen(U_Storebuff)+1, &br);  
//			//UserBuffer
//			f_close(&fsrc);
//			usb_oenflag=1;
//		}
//		else
//			usb_oenflag=2;
//	
//	}
//	else
//		usb_oenflag=0;
//	else
//		f_close(&fsrc);
	
	Disp_big_HZ(GetSystemMessage());
	while(GetSystemStatus()==SYS_STATUS_FINISH)
	{
		
	
		Disp_Time( );
//		Colour.black=LCD_COLOR_TEST_BACK;
		

//		Uart_Process();

		if(READ_START()==0)
        {
            U9001_Save_sys.U9001_save.current_step=1;
            SetSystemStatus(SYS_STATUS_START);
            SetSystemMessage(MSG_TEST);
            Uart0_Send(0xa1);
						PLC_OutProg();//��PLC����
        }
        if(READ_STOP()==0)
        {
			Beep_Off();
            SetSystemStatus(SYS_STATUS_IDEM);
			SetSystemMessage(MSG_IDLE);
            V_DA_out(0);
            Sing_out_C(0);
            Short_out(0);
            FRB_out(0);
            Uart0_Send(0xa0);
        }
		
	  
	
	
	}
 
    All_LedOff();

}
const u16 Range_comp[][2]=
{
	3000,300,
    3000,300,
//    2500,200,
//    2500,200,
    4200,460,

};
//==========================================================
//�������ƣ�Test_Process
//�������ܣ�����������
//��ڲ�������
//���ڲ�������
//�������ڣ�2015.01.25 
//�޸����ڣ�2015.09.06 15:06
//��ע˵������ֵˢ��8.4ms��״̬ˢ��3.6ms��12MHz/12����
//��ע˵����ȱ�ٵ绡�жϡ������жϴ���
//==========================================================
void Test_Process(void)
{
	vu8 key;
    u8 Range_IR=0;
	vu8 max;//�������
    u8 min;
	vu16 sortT;//��ѡ��ʱ
	vu8 stepT;//����ʱ���ʱ
	vu8 blinkT;//��˸ʱ���ʱ
//	char sendbuff[20];
	vu32 dat;
	vu32 temp;
    float value;
    float F_x;

	bool f_disp=FALSE;//��ʾ���±�־
	bool f_msgdisp=FALSE;//��Ϣ��ʾ��־

	bool f_sort=FALSE;//��ѡ��־
	bool f_upper=FALSE;//���̹�ѹ��־
	bool f_below=FALSE;//����Ƿѹ��־
	bool f_switch=FALSE;//�����л���־
	testflag=1;//���Ա�־


//	Check_Parameter_Limit(StepIndex);//��ǰ����Ĳ�������ֵ���

	//��������ֵ
	Test_mid.set_item=U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter;//��ǰ����
	if(Test_mid.set_item == OS_SETUP)
	{
		Test_mid.set_high=(float)U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last*
		((float)U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper/100.0);;//���õ�����
		
		Test_mid.set_low=(float)U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last*
		((float)U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower/100.0);//���õ�����
	}else{
		Test_mid.set_high=U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper;//���õ�����
		Test_mid.set_low=U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower;//���õ�����
	}
    Test_mid.set_ramp=U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].rise_time;
    Test_mid.set_time=U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time;
    Test_mid.set_drop=U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].drop_time;
	//��������ֵ

	//����ֵ����
	switch(Test_mid.set_item)//��Ŀ
	{
		case IR_SETUP://��Ե�������
            rangr_limit_high=Range_comp[2][0];
            rangelomit_low=Range_comp[2][1];
            Range_IR=1;
			max=5;//�������-4�� Range_comp
            min=1;
            
			break;

		case ACW_SETUP://��ѹ����
		
			if(U9001_Save_sys.U9001_Testconfg.Autorange == 1)
			{
				max=1;//�������-2��
				rangr_limit_high=Range_comp[0][0];
				rangelomit_low=Range_comp[0][1];
				min=0;
				Range=0;
			}else if(U9001_Save_sys.U9001_Testconfg.Autorange == 0){
				if(Test_mid.set_high < 3000)
				{
					Range=1;
					max=1;
					min=1;
				}else{
					Range=0;
					max=0;
					min=0;
				}
			}
			break;

		case DCW_SETUP://
			if(U9001_Save_sys.U9001_Testconfg.Autorange == 1)
			{
				rangr_limit_high=Range_comp[1][0];
				rangelomit_low=Range_comp[1][1];
				Range=0;
				max=1;//�������-2��
				min=0;
			}else if(U9001_Save_sys.U9001_Testconfg.Autorange == 0){
				if(Test_mid.set_high < 3000)
				{
					Range=1;
					max=1;
					min=1;
				}else{
					Range=0;
					max=0;
					min=0;
				}
			}
			break;

		case OS_SETUP://
            rangr_limit_high=Range_comp[2][0];
            rangelomit_low=Range_comp[2][1];
//            Test_mid.set_high=Test_mid.set_high*U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last/100;
//            Test_mid.set_low=Test_mid.set_low*U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last/100;
            Range_IR=1;
            max=5;
            min=0;
			break;

		case PA_SETUP://�޲�����Ŀ
            max=0;
			break;

		default:
			SetSystemStatus(SYS_STATUS_TEST_PAUSE);//ϵͳ״̬-��ͣ����
			return;
			
	}		
	
	Test_Init();//���Գ�ʼ��
	

	stepT=0;//����ʱ���ʱ
	sortT=0;//��ѡ��ʱ
	blinkT=0;//��˸ʱ���ʱ
	Test_Time=0;
//	Test_value.Test_Time=0;
    
//	TestOut=0;
//	Range=2;
//	Range_Control(Range);
    Test_Value.Time=0;
	
	while(GetSystemStatus()==SYS_STATUS_TEST)
	{
        if(READ_STOP()==0)
        {
            
            V_DA_out(0);
            Sing_out_C(0);
            Short_out(0);	
            FRB_out(0);
            SetSystemStatus(SYS_STATUS_TEST_PAUSE);
        }
        Read_Ad();//��ȡADֵ
		Ad_Filter();//ADֵ�˲�
       
        
		Get_Result();//�������ֵ	
		//��ѡ�б�
		if(f_switch==FALSE)//�������л���־
		{
			if(Test_mid.set_item==IR_SETUP || Test_mid.set_item==OS_SETUP)//��Ե����
			{
                if(GetSystemMessage()==MSG_TEST )
                {
                    if(Test_Value.Time>=Test_mid.set_time)//�б���ʱ
					{
						f_sort=TRUE;//��ѡ��־
					}
//                        f_sort=TRUE;//��ѡ��־
                    
                }
			}
			else
			{
//                 if(GetSystemMessage()==MSG_TEST )
//                {
//                    if(Test_Value.Time>=SORT_TIME_MIN)//�б���ʱ
//                        f_sort=TRUE;//��ѡ��־
//                    
//                }
//				if(sortT>=SORT_TIME_MIN)//������Сʱ���ſ�ʼ��ѡ
//					f_sort=TRUE;//��ѡ��־
				if(sortT>=SORT_TIME_MIN)//�б���ʱ
				{
					f_sort=TRUE;//��ѡ��־
				}
			}
		}//�������б�
		if(F_100ms==TRUE)//100ms��ʱ����
		{
			
			F_100ms=FALSE;
			if(GetSystemMessage()!=MSG_DROP)
			{
				f_disp=TRUE;//��ʾ����
			}

			if(sortT<9999)sortT++;
			
//			if(Test_Value.Time > 3)
//			{
//				NVIC_EnableIRQ(GPIO_IRQn);
//			}
			if(GetSystemMessage()==MSG_RAMP )//ϵͳ��Ϣ-����
			{
				stepT++;//����ʱ���ʱ
				if(stepT>=RampStepTime)//��������ʱ��
				{
					stepT=0;//����ʱ���ʱ
					TestOut+=RampStepFd;//�������ֵ����
 
				}
				

				if(Test_Value.Time>=Test_mid.set_ramp)//����ʱ���б�
				{
					Test_Value.Time=0;//����ʱ������
					TestOut=FullOut;//�������ֵ����
					SetSystemMessage(MSG_TEST);//ϵͳ��Ϣ-���ز���
					f_msgdisp=TRUE;//��Ϣ��ʾ��־
				}
				Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].text_flag=GetSystemMessage();
//				if(Test_mid.set_item==IR_SETUP)
//				{
//					if(Current)//����ֵ����
//					{
//						temp=Voltage;//��ѹֵ
////						//���ֵ�б�
//						if((Range==5)&&(Resistance>IR_RESISTANCE_MAX))//���ֵ�б�
//							Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].Text_value=TEST_VALUE_OVER;//�������
//					}
//					else
//					{
//							Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].Text_value=TEST_VALUE_OVER;//�������
//					}
////							Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].Text_value=Resistance;
//					Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].text_unit=test_value.uint;
//				}else{
//					Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].Text_value=Current;
//				}
//					Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].Text_vot=Test_Value.Vol;
//					Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].Text_time=Test_Value.Time;
			}
			else if(GetSystemMessage()==MSG_TEST )
			{
				TestOut=FullOut;//�������ֵ����
				
				Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].text_flag=GetSystemMessage();
				if(Test_mid.set_item==IR_SETUP)
				{
					if(Current)//����ֵ����
					{
						temp=Voltage;//��ѹֵ
//						//���ֵ�б�
						if((Range==5)&&(Resistance>IR_RESISTANCE_MAX))//���ֵ�б�
							Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].Text_value=TEST_VALUE_OVER;//�������
					}
					else
					{
							Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].Text_value=TEST_VALUE_OVER;//�������
					}
//							Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].Text_value=Resistance;
					Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].text_unit=test_value.uint;
				}else{
					Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].Text_value=Current;
				}
					Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].Text_vot=Test_Value.Vol;
					Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].Text_time=Test_Value.Time;
				if(Test_mid.set_time>0)//����ʱ��Ϊ0����������
				{
					
					if(Test_Value.Time>=Test_mid.set_time)//����ʱ���б�
					{
						if(clearflag == 1)
						{
							U9001_Save_sys.U9001_save.clearvalue[U9001_Save_sys.U9001_save.current_step-1]=Test_Value.I_R;
						}
						if(osgetflag == 1)
						{
							if(Range <= 2)
							{
								U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last=Test_Value.I_R*10;
							}else{
								U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last=Test_Value.I_R;
							}
						}
//                        if(Test_mid.set_item==OS_SETUP)
//                        {
//                                dat=Test_Value.I_R;
//                            if(dat>Test_mid.set_high)//������
//                                SetSystemMessage(MSG_HIGH);
//                            if(dat<Test_mid.set_low)//������
//                                SetSystemMessage(MSG_LOW);
//                        }
//                        else
                            
                    
            SetSystemMessage(MSG_PASS);
						PLC_OutPass();
            Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].text_flag=GetSystemMessage();
						if(Test_mid.set_item==IR_SETUP)
            {
							if(Current)//����ֵ����
							{
								temp=Voltage;//��ѹֵ
		//						//���ֵ�б�
								if((Range==5)&&(Resistance>IR_RESISTANCE_MAX))//���ֵ�б�
									Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].Text_value=TEST_VALUE_OVER;//�������
							}
							else
							{
									Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].Text_value=TEST_VALUE_OVER;//�������
							}
//							Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].Text_value=Resistance;
							Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].text_unit=test_value.uint;
						}else{
							Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].Text_value=Current;
						}
              Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].Text_vot=Test_Value.Vol;
							Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].Text_time=Test_Value.Time;
                        Test_Value.Time=0;
                        
                        if(Test_mid.set_drop)//�½�ʱ��
                        {
//                            f_sort=TRUE;//��ѡ��־
                            stepT++;//����ʱ���ʱ
                            if(stepT>=RampStepTime)//��������ʱ��
                            {
                                stepT=0;//����ʱ���ʱ
                                if(TestOut>DropStepFd)
                                TestOut-=DropStepFd;//�������ֵ����
 
                            }
                            SetSystemMessage(MSG_DROP);
                        }else
                        {
                            SetSystemStatus(SYS_STATUS_TEST_PAUSE);//������ͣ״̬
//                            SetSystemMessage(MSG_PASS);//ϵͳ��Ϣ-���Ժϸ�
                            f_msgdisp=TRUE;//��Ϣ��ʾ��־
                            V_DA_out(0);
                            Sing_out_C(0);
                            Short_out(0);
                            FRB_out(0);
														testflag=0;
//                            Uart0_Send(0xa4);
                        }
					}
				}
			}
            else if(GetSystemMessage()==MSG_DROP )
            {  
//                 if(U9001_Save_sys.U9001_save.U9001_Setup[1].parameter!=OS)//OS״̬��Ҫ
//                 {
//                    Test_Value.Time=Test_mid.set_drop;
//                 }
//							Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].text_flag=GetSystemMessage();
//							if(Test_mid.set_item==IR_SETUP)
//							{
//								if(Current)//����ֵ����
//								{
//									temp=Voltage;//��ѹֵ
//			//						//���ֵ�б�
//									if((Range==5)&&(Resistance>IR_RESISTANCE_MAX))//���ֵ�б�
//										Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].Text_value=TEST_VALUE_OVER;//�������
//								}
//								else
//								{
//										Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].Text_value=TEST_VALUE_OVER;//�������
//								}
//	//							Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].Text_value=Resistance;
//								Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].text_unit=test_value.uint;
//							}else{
//								Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].Text_value=Current;
//							}
//								Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].Text_vot=Test_Value.Vol;
//								Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].Text_time=Test_Value.Time;
                if(Test_mid.set_drop)//�½�ʱ��
                {
                    stepT++;//����ʱ���ʱ
                    if(stepT>=DropStepTime)//��������ʱ��
                    {
                        stepT=0;//����ʱ���ʱ
                        if(TestOut>DropStepFd)
                        TestOut-=DropStepFd;//�������ֵ����

                    }
                    SetSystemMessage(MSG_DROP);
                }else
                {
                    SetSystemStatus(SYS_STATUS_TEST_PAUSE);//������ͣ״̬
                    SetSystemMessage(MSG_PASS);//ϵͳ��Ϣ-���Ժϸ�
										PLC_OutPass();
                    f_msgdisp=TRUE;//��Ϣ��ʾ��־
                    V_DA_out(0);
                    Sing_out_C(0);
                    Short_out(0);
                    FRB_out(0);
										testflag=0;
//                    Uart0_Send(0xa4);
                }
                if(Test_Value.Time>=Test_mid.set_drop)//����ʱ���б�
                {
										Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].Text_time+=Test_Value.Time;
                    Test_Value.Time=0;//����ʱ������
//                    Test_Value.Vol=0;
                    SetSystemStatus(SYS_STATUS_TEST_PAUSE);//������ͣ״̬
                    SetSystemMessage(MSG_PASS);//ϵͳ��Ϣ-���Ժϸ�
										PLC_OutPass();
                    f_msgdisp=TRUE;//��Ϣ��ʾ��־
                    V_DA_out(0);
                    Sing_out_C(0);
                    Short_out(0);
                    FRB_out(0);
										testflag=0;
//                    Uart0_Send(0xa4);
                }
            
  
            }
			
			//�ź��������
			V_DA_out(TestOut);//�ź�D/A���
//            V_DA_out(200);
			
			//���Ե���˸����
			blinkT++;//��˸ʱ���ʱ
			blinkT%=9;
			
		}
		if(rangedelay>0)
		{
			rangedelay --;
		}
		
		if(Current>rangr_limit_high/* && rangedelay == 0*/)//������������
		{
			f_upper=TRUE;//���̹�ѹ��־
			f_below=FALSE;//����Ƿѹ��־
		}
		else if(Current<rangelomit_low/* && rangedelay == 0*/)//������������
		{
			f_below=TRUE;//���̵�ѹ��־
			f_upper=FALSE;//����Ƿѹ��־
		}
		else
		{
			f_upper=FALSE;//���̹�ѹ��־
			f_below=FALSE;//����Ƿѹ��־
		}

		f_switch=FALSE;//�����л���־
		if((Range<max)&&(f_below==TRUE))//���̷�����ҵ�ѹ
		{
			Range++;
			f_switch=TRUE;//�����л���־
		}
		if((Range>min)&&(f_upper==TRUE))//���̷�����ҹ�ѹ
		{
			Range--;
			f_switch=TRUE;//�����л���־
		}
		
		
		if(f_switch==TRUE)//�����л���־
		{
			AdCount=0;//ADֵ�������
			for(key=0;key<(AD_BUF_LENGTH);key++)
	 		{
				Ibuf[key]=0;//AD���������
			}
			Range_Control(Range);//���̿���
			rangedelay = 10;

		}
		else
		{
			//��ֵ����ͼ������ֵ
			switch(Test_mid.set_item)//������
			{
				case ACW_SETUP://��ѹ
				case DCW_SETUP:
					//��λ����
//					if(Range==1)
						Test_Value.I_R=Current;
//                    if(Range==0)//������� �Ƚ����ֵ
//                    {
                        if(Test_mid.set_item==1)
                            dat=DCW_CURRENT_MAX;
                        else
                            dat=ACW_CURRENT_MAX;
//                    }
					if(Test_Value.I_R>dat)//���ֵ�б�
						Test_Value.I_R=TEST_VALUE_OVER;//�������

//					if(f_switch==FALSE)
                    if(GetSystemMessage()==MSG_TEST )
                    {
                        if(Test_Value.Time>=SORT_TIME_MIN)//�б���ʱ
						{
							if(U9001_Save_sys.U9001_Testconfg.comp_setp != 0)
							{
								if(U9001_Save_sys.U9001_save.current_step < U9001_Save_sys.U9001_Testconfg.comp_setp)
								{
									f_sort=TRUE;//��ѡ��־
								}
							}else{
								f_sort=TRUE;//��ѡ��־
							}
						}
                    }
					break;
	
                case IR_SETUP:
					//��λ����
                   
					if(Current)//����ֵ����
					{
						temp=Voltage;//��ѹֵ
//						//���ֵ�б�
						if((Range==5)&&(Resistance>IR_RESISTANCE_MAX))//���ֵ�б�
							Resistance=TEST_VALUE_OVER;//�������
						Test_Value.I_R=Resistance;
					}
					else
					{
							Resistance=TEST_VALUE_OVER;//�������
              Test_Value.I_R=Resistance;
					}
                     switch(Range)
                    {
                        case 1:
                            
                            Test_Value.I_R/=100;
                            break;
                        case 2:
                            Test_Value.I_R/=100;
                            break;
                        case 3:
                            Test_Value.I_R/=10;
                            break;
                        case 4:
//                            Test_Value.I_R*=10;
                            break;
                        case 5:
                            Test_Value.I_R*=10;
                            break;
                        default:
                            break;
                    
                    }
					break;
                case PA_SETUP:

					break;
                case OS_SETUP:
                    if(Current)//����ֵ����
					{
						temp=Voltage;//��ѹֵ
//						//���ֵ�б�
						if((Range==5)&&(Resistance>IR_RESISTANCE_MAX))//���ֵ�б�
							Resistance=TEST_VALUE_OVER;//�������
					}
					else
					{
						Resistance=0;//�������
                        Test_Value.I_R=0;
					}

//                    F_x=Test_Value.I_R;
//                     switch(Range)
//                    {
//                         case 0:
//                             
//                             break;
//                        case 1:
//                            
//                            F_x*=10;
//                            break;
//                        case 2:
//                            F_x*=100;
//                            break;
//                        case 3:
//                            F_x*=1000;
//                            break;
//                        case 4:
//                            F_x*=100000;
//                            break;
//                        case 5:
//                            F_x*=1000000;
//                            break;
//                        default:
//                            break;
//                    
//                    }

//                    value=1.0/(376.8*F_x);
//                    value/=1.45f;
//					Test_Value.I_R=value*1e11;
////                    Test_Value.I_R=value*1e15;
//                    Test_Value.I_R*=10;
//                    Test_Value.I_R/=14;
//					value=F_x;
//					Test_Value.I_R=value;
//					Test_Value.I_R/=23;
//                    Resistance=Test_Value.I_R;
                    break;
				default:
					break;
			}
		}
        
        //��ѡ�б�
//		if(f_switch==FALSE)//�������л���־
//		{
//			if(Test_mid.set_item==IR_SETUP)//��Ե����
//			{
//                if(GetSystemMessage()==MSG_TEST )
//                {
//                    if(Test_Value.Time>=Test_mid.set_time)//�б���ʱ
//                        f_sort=TRUE;//��ѡ��־
//                    
//                }
//			}
//			else
//			{
////                 if(GetSystemMessage()==MSG_TEST )
////                {
////                    if(Test_Value.Time>=SORT_TIME_MIN)//�б���ʱ
////                        f_sort=TRUE;//��ѡ��־
////                    
////                }
//				if(sortT>=SORT_TIME_MIN)//������Сʱ���ſ�ʼ��ѡ
//					f_sort=TRUE;//��ѡ��־
//			}
//		}//�������б�
//		if((f_sort==TRUE) && (AdCount>=AD_BUF_LENGTH))//�ǻ���
        if(f_sort==TRUE)
		{
			switch(Test_mid.set_item)//������
			{
				case ACW_SETUP:
				case DCW_SETUP:
					dat=Current;
					if(Range == 1)
					{
						if(dat>Test_mid.set_high)//������
							SetSystemMessage(MSG_HIGH);
						if(GetSystemMessage()==MSG_TEST )
						if(dat<Test_mid.set_low)//������
							SetSystemMessage(MSG_LOW);
					}else if(Range == 0){
						if(dat*10>Test_mid.set_high)//������
							SetSystemMessage(MSG_HIGH);
						if(GetSystemMessage()==MSG_TEST )
						if(dat*10<Test_mid.set_low)//������
							SetSystemMessage(MSG_LOW);
					}
					break;
		
				case IR_SETUP:
//                    Test_Value.I_R*=pow(10,Range-1);
//					Test_Value.I_R/=100;
					dat=Resistance;
					dat=Test_Value.I_R;
					if((dat>Test_mid.set_high)&&(Test_mid.set_high!=0))//������
						SetSystemMessage(MSG_HIGH);
					if(dat<Test_mid.set_low)//������
						SetSystemMessage(MSG_LOW);
					break;

				case OS_SETUP:
					if(Range <= 2)
					{
						dat=Test_Value.I_R*10;
					}else{
						dat=Test_Value.I_R;
					}
					if(dat>Test_mid.set_high)//������
						SetSystemMessage(MSG_SHORT);
					if(dat<Test_mid.set_low)//������
						SetSystemMessage(MSG_OPEN);
                   
					break;
			
				default:
					break;
			}
		}

		//�����ޱ����͹�������
		if((GetSystemMessage()==MSG_HIGH)||(GetSystemMessage()==MSG_LOW)||
				(GetSystemMessage()==MSG_ARC)||(GetSystemMessage()==MSG_OVER)||(GetSystemMessage()==MSG_SHORT)
        ||(GetSystemMessage()==MSG_GIF)||(GetSystemMessage()==MSG_ARC)||(GetSystemMessage()==MSG_OPEN))
		{
			V_DA_out(0);
			Sing_out_C(0);
			Short_out(0);
			FRB_out(0);
      Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].text_flag=GetSystemMessage();
      Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].Text_value=dat;
			Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].text_unit=test_value.uint;
			Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].Text_time=Test_Value.Time;
      Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].Text_vot=Test_Value.Vol;
			F_Fail=TRUE;//����ʧ�ܱ�־
			PLC_OutFail();
			SetSystemStatus(SYS_STATUS_TEST_PAUSE);//ϵͳ״̬-������ͣ
			f_disp=TRUE;//������ʾ���Խ��
			f_msgdisp=TRUE;//��Ϣ��ʾ��־
			testflag=0;
//            Uart0_Send(0xa5);//ʧ�ܵ�
		}

		if(f_disp==TRUE)//��ʾ����
		{
            if(U9001_Save_sys.U9001_save.disp)
                Disp_Test_List(U9001_Save_sys.U9001_save.current_step);
            else
            Disp_Testvalue(1); 
			f_disp=FALSE;//��ʾ����

			//��ʾ��Ϣ
			if(f_msgdisp==TRUE)//��Ϣ��ʾ��־
			{
//				f_msgdisp=FALSE;//��Ϣ��ʾ��־
//				LcdAddr.x=MSG_ADDR_X;
//				Disp_SysMessage();//��ʾ��Ϣ
			}
		}
		
//		Uart_Process();//���ڴ���
        if(READ_STOP()==0)
        {
						Beep_Off();
            V_DA_out(0);
            Sing_out_C(0);
            Short_out(0);
            FRB_out(0);
            SetSystemMessage(MSG_PAUSE);//ϵͳ��Ϣ-��ͣ����
            SetSystemStatus(SYS_STATUS_ABORT);//ϵͳ״̬-��ͣ����
            Uart0_Send(0xa0);
        }

	}

}
//�ļ��������
void File_Process(void)
{
	 vu32 keynum=0;
	 vu8 key;
//	lcd_Clear(LCD_COLOR_TEST_BACK);
	Delay_Key();
  	while(GetSystemStatus()==SYS_STATUS_FILE)
	{
	 key=HW_KeyScsn();
		if(key==0xff)
		{
			keynum=0;
		}
		else
			keynum++;
		if(keynum==2)
		{
			switch(key)
			{
				case Key_F1:
				break;
				case Key_F2:
				break;
				case Key_F3:
				break;
				case Key_F4:
				break;
				case Key_F5:
				break;
				case Key_Disp:
				break;
				case Key_SETUP:
				break;
				case Key_FAST:
				break;
				case Key_LEFT:
				break;
				case Key_RIGHT:
				break;
				case Key_UP:
				break;
				case Key_DOWN:
				break;
				case Key_NUM1:
				break;
				case Key_NUM2:
				break;
				case Key_NUM3:
				break;
				case Key_NUM4:
				break;
				case Key_NUM5:
				break;
				case Key_NUM6:
				break;
				case Key_NUM7:
				break;
				case Key_NUM8:
				break;
				case Key_NUM9:
				break;
				case Key_NUM0:
				break;
				case Key_DOT:
				break;
				case Key_BACK:
				break;
				
				default:
				break;
					
			}
		
		
		}
	
	
	
	}

}
const u16 Hign_Vout[]={5000,6000,1000};
const u16 High_Upper[]={12000,600,9999};
const u8 ITEM_S[][12]=
{
    
    {5,5,5,0,2,1,2,1,2,1,5,5},//AC
    {5,5,5,0,2,1,2,1,2,1,1,2},//DC
    {5,5,5,0,3,1,3,1,5,1,5,5},//IR
    {5,5,5,0,4,1,3,1,5,1,5,5},//os
	{5,5,5,0,1,1,3,1,5,1,5,5}//PA
};

//�������ó���

void Setup_Process(void)
{
    vu16 *pt;
	u8 list=1;//���ұ���0  �����1��
    u8 line=0;//��
    u8 item=0;
    u8 disp_all=1;
    u8 key_count;
//    u8 lenth=5;
    u8 Disp_buff[8];
    u8 input_flag=0;
    u8 select_num=0;
	u16 presscount=0;//��������
    float setvalue;
//	vu8 key;
	DISP_FLAG=TRUE;
    keyvalue=KEY_NONE;
//    GUI_SetColor(LCD_COLOR_TEST_BACK);
	GUI_SetBkColor(LCD_COLOR_TEST_BACK);
	GUI_Clear();
    

    DISP_FLAG=TRUE;
	pageflag = 2;//��������
 	while(GetSystemStatus()==SYS_STATUS_SETUPTEST)
	{
	    
		if(DISP_FLAG==TRUE)
		{
//             item=list*(MAX_SETP[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter]+1)/2+line;
//            if(line==0)
//                item=0;
//            else
//                item=2*line-list;
            
//            if(item>=MAX_SETP[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter])
//            {
//                item=MAX_SETP[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter];
//                
////                list=item/((MAX_SETP[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter]+1)/2);
////                line=item-list*(MAX_SETP[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter]+1)/2;
//                
//            }
			inputselnum=ITEM_S[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter][item];
            if(disp_all)
            {
//                
               
                GUI_SetColor(LCD_COLOR_TEST_BACK);
                GUI_FillRect(0,24,380,270);
                GUI_SetColor(GUI_WHITE);
                Disp_Test_Set_Item();
                disp_all=0;
            }
            if(input_flag)
            {
                disp_Inputnum((u8*)Disp_buff);
                
            }else
            {
                DispSet_value(item);
                memset(Disp_buff,0,10);

                GUI_SetColor(LCD_COLOR_TEST_BACK);
                GUI_FillRect(20,214,350,242);

            }
            
			DISP_FLAG=FALSE;
		
		}

//        keyvalue=Key_Read_WithTimeOut(TICKS_PER_SEC_SOFTTIMER/10);
		if(keyvalue!=KEY_NONE)
		{	
            
			
			
            if(input_flag==0)
            {
                key_count=0;
                switch(keyvalue)
                {
                    case Key_NUM0:
                    case Key_NUM1:
                    case Key_NUM2:
                    case Key_NUM3:
                    case Key_NUM4:
                    case Key_NUM5:
                    case Key_NUM6:
                    case Key_NUM7:
                    case Key_NUM8:
                    case Key_NUM9:
                    case Key_DOT:  
						Key_Beep();
                        if((item<=2)/*||(item==8)*/)
                        {
                        }
                        else
                        {
							if(item == 8)
							{
								if(U9001_Save_sys.U9001_Testconfg.ARC_mode)
								{
									
								}else{
									memset(Disp_buff,0,6);
									if(key_count<5)
									{
										Disp_buff[key_count]=keyvalue;
										key_count++;

									}
									input_flag=1;
									Disp_Pre_Uint(inputselnum);
									DISP_FLAG=TRUE;
								}
							}else{
								if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter == OS_SETUP)
								{
									if(item == 4)
									{
										memset(Disp_buff,0,6);
										if(key_count<5)
										{
											Disp_buff[key_count]=keyvalue;
											key_count++;

										}
										input_flag=1;
										Disp_Pre_Uint(inputselnum);
										DISP_FLAG=TRUE;
									}
								}else if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter == PA_SETUP){
									if(item == 5)
									{
										memset(Disp_buff,0,6);
										if(key_count<5)
										{
											Disp_buff[key_count]=keyvalue;
											key_count++;

										}
										input_flag=1;
										Disp_Pre_Uint(inputselnum);
										DISP_FLAG=TRUE;
									}
								}else{
									memset(Disp_buff,0,6);
									if(key_count<5)
									{
										Disp_buff[key_count]=keyvalue;
										key_count++;

									}
									input_flag=1;
									Disp_Pre_Uint(inputselnum);
									DISP_FLAG=TRUE;
								}
							}
                        }
                        break;
                    case Key_BACK:
//                        DISP_FLAG=TRUE;
                        break;
                    case Key_F1:
						Key_Beep();
						presscount=0;
						DISP_FLAG=TRUE;
                        switch(item)//�ڼ���
                        {
//							case 0:
//								
//								
//								break;
                            case 1:
                                if(U9001_Save_sys.U9001_save.all_step<10)
                                {
                                    U9001_Save_sys.U9001_save.all_step++;
                                    U9001_Save_sys.U9001_save.current_step++;
                                    disp_all=1;
                                    SetDate_Comp();
                                }
                                break;
                            case 2:
                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter=0;
								U9001_Save_sys.U9001_save.clearvalue[U9001_Save_sys.U9001_save.current_step-1]=0;
                                SetDate_Comp();
                                disp_all=1;
                                break;
                            case 3:
                                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==OS_SETUP)
                                {
                                    
                                    U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower+=10;
                                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower>=100)
                                        U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower=100;
                                }
                                else
                                {
                                    U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out+=100;
                                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out>Hign_Vout[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter])//
                                        U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out=Hign_Vout[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter];
                                }
                                break;
                            case 4:
                                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==IR_SETUP)
								{
									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper < 10000)
									{
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=100;
									}else{
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=1000;
									}
								}else if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==OS_SETUP){
									U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last=0;
								}else if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==PA_SETUP){
									U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check=0;
								}
								else{
                                    U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=100;
								}
								SetDate_Comp();
//                                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>High_Upper[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter])//
//                                    U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=High_Upper[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter];
                                break;
                            case 5:
                                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==OS_SETUP)
                                {
                                    
                                    U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=100;
                                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>=500)
                                        U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=500;
                                }
                                else{
									U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time+=10;
									SetDate_Comp();
								}
								
    //                            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper.num>High_Upper[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter])//
    //                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper.num=High_Upper[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter];
                                break;
                            case 6:
								if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==IR_SETUP)
								{
									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower < 10000)
									{
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower+=100;
									}else{
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower+=1000;
									}
									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower>
									   U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper)
									{
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper
									   =U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower;
									}
								}else{
									U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower+=100;
									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower>
									   U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper)
									{
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower=
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper;
									}
								}
								SetDate_Comp();
                                break;
                            case 7:
                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].rise_time+=10;
								SetDate_Comp();
                                break;
                            case 8: 
                                 if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==IR)
                                {
                                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc<MAX_R_RANGE)
                                        U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc++;
                                }
                                else
                                 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc++;
								SetDate_Comp();
                                break;
                            case 9:
                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].drop_time+=10;
								SetDate_Comp();
                                break;
                            case 10:
                                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==DC)
                                {
                                   U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last+=10; 
                                }else
                                {
                                    U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check=0;
                                }
								SetDate_Comp();
                                break;
                            case 11:
                                
                                   U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check+=10;
									SetDate_Comp();
                                break;
                            default:
                                break;
                            
                        }
                        break;
					case L_Key_F1:
						presscount++;
                        switch(item)//�ڼ���
                        {
                            case 0:
                                
                                
                                break;
                            case 1:
                                if(U9001_Save_sys.U9001_save.all_step<10)
                                {
									if(presscount>LKEY1)//����������
									{
										U9001_Save_sys.U9001_save.all_step++;
										U9001_Save_sys.U9001_save.current_step++;
										DISP_FLAG=TRUE;
									}
                                    disp_all=1;
                                    SetDate_Comp();
                                }
                                break;
                            case 2:
//                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter=0;
//								U9001_Save_sys.U9001_save.clearvalue[U9001_Save_sys.U9001_save.current_step-1]=0;
//                                SetDate_Comp();
//								DISP_FLAG=TRUE;
//                                disp_all=1;
                                break;
                            case 3:
                                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==OS_SETUP)
                                {
                                    
									if(presscount>LKEY3)//����������
									{
										Key_Beep();
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower+=30;
										DISP_FLAG=TRUE;
									}
									else if(presscount>LKEY2)//����������
									{
										Key_Beep();
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower+=20;
										DISP_FLAG=TRUE;
									}
									else if(presscount>LKEY1)//����������
									{
										Key_Beep();
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower+=10;
										DISP_FLAG=TRUE;
									}
//                                    U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower+=10;
                                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower>=100)
                                        U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower=100;
                                }
                                else
                                {
									if(presscount>LKEY3)//����������
									{
										Key_Beep();
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out+=300;
										DISP_FLAG=TRUE;
									}
									else if(presscount>LKEY2)//����������
									{
										Key_Beep();
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out+=200;
										DISP_FLAG=TRUE;
									}
									else if(presscount>LKEY1)//����������
									{
										Key_Beep();
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out+=100;
										DISP_FLAG=TRUE;
									}
//                                    U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out+=100;
                                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out>Hign_Vout[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter])//
                                        U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out=Hign_Vout[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter];
                                }
                                break;
                            case 4:
                                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==IR_SETUP)
								{
									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper < 10000)
									{
										if(presscount>LKEY3)//����������
										{
											Key_Beep();
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=300;
											DISP_FLAG=TRUE;
										}
										else if(presscount>LKEY2)//����������
										{
											Key_Beep();
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=200;
											DISP_FLAG=TRUE;
										}
										else if(presscount>LKEY1)//����������
										{
											Key_Beep();
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=100;
											DISP_FLAG=TRUE;
										}
//										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=100;
									}else{
										if(presscount>LKEY3)//����������
										{
											Key_Beep();
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=3000;
											DISP_FLAG=TRUE;
										}
										else if(presscount>LKEY2)//����������
										{
											Key_Beep();
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=2000;
											DISP_FLAG=TRUE;
										}
										else if(presscount>LKEY1)//����������
										{
											Key_Beep();
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=1000;
											DISP_FLAG=TRUE;
										}
//										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=1000;
									}
								}
//								else if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==OS_SETUP){
//									U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last=0;
//									DISP_FLAG=TRUE;
//								}else if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==PA_SETUP){
//									U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check=0;
//									DISP_FLAG=TRUE;
//								}
								else{
									if(presscount>LKEY3)//����������
									{
										Key_Beep();
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=300;
										DISP_FLAG=TRUE;
									}
									else if(presscount>LKEY2)//����������
									{
										Key_Beep();
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=200;
										DISP_FLAG=TRUE;
									}
									else if(presscount>LKEY1)//����������
									{
										Key_Beep();
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=100;
										DISP_FLAG=TRUE;
									}
//                                    U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=100;
								}
								SetDate_Comp();
//                                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>High_Upper[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter])//
//                                    U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=High_Upper[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter];
                                break;
                            case 5:
                                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==OS_SETUP)
                                {
									
                                    if(presscount>LKEY3)//����������
									{
										Key_Beep();
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=300;
										DISP_FLAG=TRUE;
									}
									else if(presscount>LKEY2)//����������
									{
										Key_Beep();
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=200;
										DISP_FLAG=TRUE;
									}
									else if(presscount>LKEY1)//����������
									{
										Key_Beep();
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=100;
										DISP_FLAG=TRUE;
									}
//                                    U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=100;
                                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>=500)
                                        U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=500;
                                }
                                else{
									if(presscount>LKEY3)//����������
									{
										Key_Beep();
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time+=30;
										DISP_FLAG=TRUE;
									}
									else if(presscount>LKEY2)//����������
									{
										Key_Beep();
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time+=20;
										DISP_FLAG=TRUE;
									}
									else if(presscount>LKEY1)//����������
									{
										Key_Beep();
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time+=10;
										DISP_FLAG=TRUE;
									}
//									U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time+=10;
									SetDate_Comp();
								}
								
    //                            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper.num>High_Upper[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter])//
    //                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper.num=High_Upper[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter];
                                break;
                            case 6:
								if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==IR_SETUP)
								{
									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower < 10000)
									{
										if(presscount>LKEY3)//����������
										{
											Key_Beep();
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower+=300;
											DISP_FLAG=TRUE;
										}
										else if(presscount>LKEY2)//����������
										{
											Key_Beep();
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower+=200;
											DISP_FLAG=TRUE;
										}
										else if(presscount>LKEY1)//����������
										{
											Key_Beep();
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower+=100;
											DISP_FLAG=TRUE;
										}
//										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower+=100;
									}else{
										if(presscount>LKEY3)//����������
										{
											Key_Beep();
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower+=3000;
											DISP_FLAG=TRUE;
										}
										else if(presscount>LKEY2)//����������
										{
											Key_Beep();
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower+=2000;
											DISP_FLAG=TRUE;
										}
										else if(presscount>LKEY1)//����������
										{
											Key_Beep();
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower+=1000;
											DISP_FLAG=TRUE;
										}
//										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower+=1000;
									}
									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower>
									   U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper)
									{
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper
									   =U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower;
									}
								}else{
									if(presscount>LKEY3)//����������
									{
										Key_Beep();
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower+=300;
										DISP_FLAG=TRUE;
									}
									else if(presscount>LKEY2)//����������
									{
										Key_Beep();
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower+=200;
										DISP_FLAG=TRUE;
									}
									else if(presscount>LKEY1)//����������
									{
										Key_Beep();
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower+=100;
										DISP_FLAG=TRUE;
									}
//									U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower+=100;
									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower>
									   U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper)
									{
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower=
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper;
									}
								}
								SetDate_Comp();
                                break;
                            case 7:
								if(presscount>LKEY3)//����������
								{
									Key_Beep();
									U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].rise_time+=30;
									DISP_FLAG=TRUE;
								}
								else if(presscount>LKEY2)//����������
								{
									Key_Beep();
									U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].rise_time+=20;
									DISP_FLAG=TRUE;
								}
								else if(presscount>LKEY1)//����������
								{
									Key_Beep();
									U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].rise_time+=10;
									DISP_FLAG=TRUE;
								}
//                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].rise_time+=10;
								SetDate_Comp();
                                break;
                            case 8: 
                                 if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==IR)
                                {
									if(presscount>LKEY1)//����������
									{
										Key_Beep();
										if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc<MAX_R_RANGE)
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc++;
										DISP_FLAG=TRUE;
									}
                                }
                                else{
									if(presscount>LKEY1)//����������
									{
										Key_Beep();
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc++;
										DISP_FLAG=TRUE;
									}
								}
								SetDate_Comp();
                                break;
                            case 9:
								if(presscount>LKEY1)//����������
								{
									DISP_FLAG=TRUE;
									Key_Beep();
									U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].drop_time+=10;
								}
								SetDate_Comp();
                                break;
                            case 10:
                                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==DC)
                                {
									if(presscount>LKEY1)//����������
									{
										DISP_FLAG=TRUE;
										Key_Beep();
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last+=10; 
									}
                                }
//								else
//                                {
//                                    U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check=0;
//                                }
								SetDate_Comp();
                                break;
                            case 11:
								  if(presscount>LKEY1)//����������
								  {
									  DISP_FLAG=TRUE;
									  Key_Beep();
									U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check+=10;
								  }
									SetDate_Comp();
                                break;
                            default:
                                break;
                            
                        }
                        break;
                    case Key_F2:
						Key_Beep();
						presscount=0;
                        switch(item)//�ڼ���
                        {
                            case 0:
                                SetSystemStatus(SYS_STATUS_CLEAR);
                                break;
                            case 1:
                                if(U9001_Save_sys.U9001_save.all_step>1)
                                {
									DISP_FLAG=TRUE;
                                    U9001_Save_sys.U9001_save.all_step--;
                                    if(U9001_Save_sys.U9001_save.current_step>1)
                                        U9001_Save_sys.U9001_save.current_step--;
                                    disp_all=1;
                                
                                }
                                break;
                            case 2:
//																DISP_FLAG=TRUE;
//                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter=1;
//                                SetDate_Comp();
//                                disp_all=1;
                                break;
                            case 3:
                                 if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==OS_SETUP)
                                {
                                    
                                    U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower++;
                                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower>=100)
                                        U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower=100;
									else
										DISP_FLAG=TRUE;
                                }
                                else
								{
                                    U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out+=1;
                                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out>Hign_Vout[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter])//
                                        U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out=Hign_Vout[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter];
									else
										DISP_FLAG=TRUE;
                                }
                                


                                break;
                            case 4:
								if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==IR_SETUP)
								{
									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper < 10000)
									{
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=1;
										DISP_FLAG=TRUE;
									}else{
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=100;
										DISP_FLAG=TRUE;
									}
									
								}else if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==OS_SETUP){
									osgetflag = 1;
									osgetstart = 1;
									SetSystemStatus(SYS_STATUS_IDEM);
									
								}else if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==PA_SETUP){
									U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check=1;
									DISP_FLAG=TRUE;
								}else{
									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper < 2000)
									{
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=1;
										DISP_FLAG=TRUE;
									}else{
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=10;
										DISP_FLAG=TRUE;
									}
								}
                                 
								SetDate_Comp();
    //                            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>High_Upper[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter])//
    //                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=High_Upper[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter];
                                break;
                            case 5:
                                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==OS_SETUP)
                                {
                                    
                                    U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=10;
                                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper<100)
                                        U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=100;
									else
										DISP_FLAG=TRUE;
                                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>=500)
                                        U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=500;
									else
										DISP_FLAG=TRUE;
                                }
                                else{
									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter == ACW_SETUP
									 ||U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter == IR_SETUP
									 ||U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter == PA_SETUP)
									{
										if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time == 0)
										{
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time = 3;
											DISP_FLAG=TRUE;
										}else{
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time+=1;
											DISP_FLAG=TRUE;
										}
									}else if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter == DCW_SETUP){
										if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time == 0)
										{
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time = 2;
											DISP_FLAG=TRUE;
										}else{
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time+=1;
											DISP_FLAG=TRUE;
										}
									}
									
									SetDate_Comp();
								}
    //                            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper.num>High_Upper[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter])//
    //                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper.num=High_Upper[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter];
                                break;
                            case 6:
								if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==IR_SETUP)
								{
									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower < 10000)
									{
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower+=1;
										DISP_FLAG=TRUE;
									}else{
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower+=100;
										DISP_FLAG=TRUE;
									}
									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower>
									   U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper)
									{
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper
									   =U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower;
									}
								}else{
									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower < 2000)
									{
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower+=1;
										DISP_FLAG=TRUE;
									}else{
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower+=10;
										DISP_FLAG=TRUE;
									}
									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower>
									   U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper)
									{
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower=
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper;
									}
								}
								SetDate_Comp();
                                break;
                            case 7:
                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].rise_time+=1;
								DISP_FLAG=TRUE;
								SetDate_Comp();
                                break;
                            case 8:
//                                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==IR)
//                                {
									DISP_FLAG=TRUE;
                                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc)
                                        U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc--;
//									else
										
//                                }
//                                else
//                                 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc++;
									SetDate_Comp();
                                break;
                            case 9:
								DISP_FLAG=TRUE;
                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].drop_time+=1;
								SetDate_Comp();
                                break;
                            case 10:
                                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==DC)
                                {
                                   U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last+=1; 
									DISP_FLAG=TRUE;
                                }else
                                {
                                    U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check=1;
									DISP_FLAG=TRUE;
                                }
								SetDate_Comp();
                                break;
                            case 11:
                                
                                   U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check+=1;
									DISP_FLAG=TRUE;
									SetDate_Comp();
                                break;
                            default:
                                break;
                            
                        }
                        break;
					case L_Key_F2:
						presscount++;
                        switch(item)//�ڼ���
                        {
                            case 0:
                                SetSystemStatus(SYS_STATUS_CLEAR);
                                break;
//                            case 1:
//                                if(U9001_Save_sys.U9001_save.all_step>1)
//                                {
//                                    U9001_Save_sys.U9001_save.all_step--;
//                                    if(U9001_Save_sys.U9001_save.current_step>1)
//                                        U9001_Save_sys.U9001_save.current_step--;
//                                    disp_all=1;
//                                
//                                }
//                                break;
//                            case 2:
//                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter=1;
//                                SetDate_Comp();
//                                disp_all=1;
//                                break;
                            case 3:
                                 if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==OS_SETUP)
                                {
                                    if(presscount>LKEY2)//����������
									{
										Key_Beep();
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower+=10;
										
									}
									else if(presscount>LKEY1)//����������
									{
										Key_Beep();
										 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower++;
									}
//                                    U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower++;
                                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower>=100)
                                        U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower=100;
									else
										DISP_FLAG=TRUE;
                                }
                                else
								{
									if(presscount>LKEY2)//����������
									{
										Key_Beep();
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out+=10;
									}
									else if(presscount>LKEY1)//����������
									{
										Key_Beep();
										 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out++;
									}
//                                    U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out+=1;
                                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out>Hign_Vout[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter])//
                                        U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out=Hign_Vout[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter];
									else
										DISP_FLAG=TRUE;
                                }
                                


                                break;
                            case 4:
								if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==IR_SETUP)
								{
									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper < 10000)
									{
										if(presscount>LKEY2)//����������
										{
											Key_Beep();
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=10;
											DISP_FLAG=TRUE;
										}
										else if(presscount>LKEY1)//����������
										{
											Key_Beep();
											 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper++;
											DISP_FLAG=TRUE;
										}
//										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=1;
									}else{
										if(presscount>LKEY2)//����������
										{
											Key_Beep();
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=1000;
											DISP_FLAG=TRUE;
										}
										else if(presscount>LKEY1)//����������
										{
											Key_Beep();
											 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=100;
											DISP_FLAG=TRUE;
										}
//										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=100;
									}
									
								}else if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==OS_SETUP){
									osgetflag = 1;
									osgetstart = 1;
									SetSystemStatus(SYS_STATUS_IDEM);
									
								}else if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==PA_SETUP){
									U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check=1;
									
								}else{
									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper < 2000)
									{
										if(presscount>LKEY2)//����������
										{
											Key_Beep();
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=10;
											DISP_FLAG=TRUE;
										}
										else if(presscount>LKEY1)//����������
										{
											Key_Beep();
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=1;
											DISP_FLAG=TRUE;
										}
//										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=1;
									}else{
										if(presscount>LKEY2)//����������
										{
											Key_Beep();
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=100;
											DISP_FLAG=TRUE;
										}
										else if(presscount>LKEY1)//����������
										{
											Key_Beep();
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=10;
											DISP_FLAG=TRUE;
										}
//										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=10;
									}
								}
                                 
								SetDate_Comp();
    //                            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>High_Upper[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter])//
    //                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=High_Upper[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter];
                                break;
                            case 5:
                                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==OS_SETUP)
                                {
                                    if(presscount>LKEY2)//����������
									{
										Key_Beep();
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=100;
									}
									else if(presscount>LKEY1)//����������
									{
										Key_Beep();
										 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=10;
									}
//                                    U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=10;
//                                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper<100)
//                                        U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=100;
                                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>=500)
                                        U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=500;
									else
										DISP_FLAG=TRUE;
                                }
                                else{
									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter == ACW_SETUP
									 ||U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter == IR_SETUP
									 ||U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter == PA_SETUP)
									{
										if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time == 0)
										{
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time = 3;
											DISP_FLAG=TRUE;
										}else{
											if(presscount>LKEY1)//����������
											{
												Key_Beep();
												 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time+=1;
												DISP_FLAG=TRUE;
											}
//											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time+=1;
										}
									}else if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter == DCW_SETUP){
										if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time == 0)
										{
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time = 2;
											DISP_FLAG=TRUE;
										}else{
											if(presscount>LKEY1)//����������
											{
												Key_Beep();
												 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time+=1;
												DISP_FLAG=TRUE;
											}
//											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time+=1;
										}
									}
									
									SetDate_Comp();
								}
    //                            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper.num>High_Upper[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter])//
    //                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper.num=High_Upper[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter];
                                break;
                            case 6:
								if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==IR_SETUP)
								{
									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower < 10000)
									{
										if(presscount>LKEY2)//����������
										{
											Key_Beep();
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower+=10;
											DISP_FLAG=TRUE;
										}
										else if(presscount>LKEY1)//����������
										{
											Key_Beep();
											 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower+=1;
											DISP_FLAG=TRUE;
										}
//										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower+=1;
									}else{
										if(presscount>LKEY2)//����������
										{
											Key_Beep();
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower+=1000;
											DISP_FLAG=TRUE;
										}
										else if(presscount>LKEY1)//����������
										{
											Key_Beep();
											 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower+=100;
											DISP_FLAG=TRUE;
										}
//										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower+=100;
									}
									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower>
									   U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper)
									{
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper
									   =U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower;
										DISP_FLAG=TRUE;
									}
								}else{
									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper < 2000)
									{
										if(presscount>LKEY2)//����������
										{
											Key_Beep();
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower+=10;
											DISP_FLAG=TRUE;
										}
										else if(presscount>LKEY1)//����������
										{
											Key_Beep();
											 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower+=1;
											DISP_FLAG=TRUE;
										}
//										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=1;
									}else{
										if(presscount>LKEY2)//����������
										{
											Key_Beep();
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower+=100;
											DISP_FLAG=TRUE;
										}
										else if(presscount>LKEY1)//����������
										{
											Key_Beep();
											 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower+=10;
											DISP_FLAG=TRUE;
										}
//										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper+=10;
									}
									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower>
									   U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper)
									{
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower=
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper;
										DISP_FLAG=TRUE;
									}
								}
								SetDate_Comp();
                                break;
                            case 7:
								if(presscount>LKEY1)//����������
								{
									Key_Beep();
									 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower+=1;
									DISP_FLAG=TRUE;
								}
//                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].rise_time+=1;
								SetDate_Comp();
                                break;
                            case 8:
//                                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==IR)
//                                {
								if(presscount>LKEY1)//����������
								{
									DISP_FLAG=TRUE;
									Key_Beep();
                                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc)
                                        U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc--;
										
								}
//                                }
//                                else
//                                 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc++;
									SetDate_Comp();
                                break;
                            case 9:
								if(presscount>LKEY1)//����������
								{
									Key_Beep();
									U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].drop_time+=1;
									DISP_FLAG=TRUE;
								}
								SetDate_Comp();
                                break;
                            case 10:
                                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==DC)
                                {
									if(presscount>LKEY1)//����������
									{
										Key_Beep();
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last+=1; 
										DISP_FLAG=TRUE;
									}
                                }else
                                {
//                                    U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check=1;
                                }
								SetDate_Comp();
                                break;
                            case 11:
                                if(presscount>LKEY1)//����������
								{
									Key_Beep();
                                    U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check+=1;
									DISP_FLAG=TRUE;
								}
									SetDate_Comp();
                                break;
                            default:
                                break;
                            
                        }
                        break;	
                    case Key_F3:
						Key_Beep();
						presscount=0;
                        switch(item)//�ڼ���
                        {
                            case 0:
                                SetSystemStatus(SYS_STATUS_SYSSET);
                                break;
                            case 1:

                            if(U9001_Save_sys.U9001_save.current_step<U9001_Save_sys.U9001_save.all_step)
                            {
                                disp_all=1;
                                U9001_Save_sys.U9001_save.current_step++;
                                memcpy(&U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step],&U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step+1],sizeof(U9001_Setup_Typedef));
                                DISP_FLAG=TRUE;
                            }
                                
                                break;
                            case 2:
//                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter=2;
//								SetDate_Comp();
//                                disp_all=1;
//								DISP_FLAG=TRUE;
                                break;
                            case 3:
                                 if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==OS_SETUP)
                                {
                                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower>10)
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower--;
                                }
                                else
								U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out--;
								SetDate_Comp();
								DISP_FLAG=TRUE;
    //                            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out>Hign_Vout[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter])//
    //                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out=Hign_Vout[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter];
                                break;
                            case 4:
								
								if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==IR_SETUP)
								{
									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper < 10000)
									{
										 if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>1)
										 {
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper-=1;
											 DISP_FLAG=TRUE;
										 }
										 else{
											 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=1;
										 }
									 }else{
//										 if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>100)
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper-=100;
										DISP_FLAG=TRUE; 
//										 else
//											 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=1;
									 }
								}else{
									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper < 2000)
									{
										if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>1)
										{
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper-=1;
											DISP_FLAG=TRUE;
										}
										 else{
											 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=1;
										 }
									}else{
										if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>10)
										{
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper-=10;
											DISP_FLAG=TRUE;
										}
										 else{
											 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=10;
										 }
									}
                                    
								}
                                 
                            
                            
                            
    //                            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>High_Upper[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter])//
    //                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=High_Upper[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter];
                                break;
                             case 5:
                                 if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==OS_SETUP)
                                {
									DISP_FLAG=TRUE;
                                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>=100)
                                        U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper-=10;
                                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper<100)
                                        U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=0;
                                }
                                else
                                {
									DISP_FLAG=TRUE;
                                 if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time>1)
                                  U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time--;
								 SetDate_Comp();
                             }
								
    //                             else
    //                                 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time=0;
    //                            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper.num>High_Upper[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter])//
    //                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper.num=High_Upper[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter];
                                break;
                            case 6:
								if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==IR_SETUP)
								{
									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower < 10000)
									{
										 if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower>1)
										 {
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower-=1;
											 DISP_FLAG=TRUE;
										 }
										 else{
											 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower=1;
											 
										 }
									 }else{
//										 if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>100)
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower-=100;
											DISP_FLAG=TRUE;
//										 else
//											 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=1;
									 }
								}else{
									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper < 2000)
									{
										DISP_FLAG=TRUE;
										if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower>1)
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower-=1;
										else
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower=0;
									}else{
										DISP_FLAG=TRUE;
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower-=10;
									}
								}
                                    
                                break;
                            case 7:
								DISP_FLAG=TRUE;
                                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].rise_time)
                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].rise_time--;
                                break;
                            case 8:
                                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==IR)
                                {
									DISP_FLAG=TRUE;
									 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc=0;
//                                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc)
//                                        U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc--;
                                }
    //                            else
    //                             U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc--;
                                break;
                            case 9:
								DISP_FLAG=TRUE;
                                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].drop_time)
                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].drop_time--;
                                break;
                            case 10:
                                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==DC)
                                {
                                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last)
									{
                                        U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last--; 
										DISP_FLAG=TRUE;
									}
                                }else
                                {
                                    U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check=1;
									DISP_FLAG=TRUE;
                                }
                                break;
                            case 11:
                                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check)
									{
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check--;
										DISP_FLAG=TRUE;
									}
										
                                
                                break;
                            default:
                                break;
                            
                        }
                        break;
					case L_Key_F3:
						presscount++;
                        switch(item)//�ڼ���
                        {
                            case 0:
                                SetSystemStatus(SYS_STATUS_SYSSET);
                                break;
//                            case 1:

//                            if(U9001_Save_sys.U9001_save.current_step<U9001_Save_sys.U9001_save.all_step)
//                            {
//                                disp_all=1;
//								if(presscount>LKEY1)//����������
//									U9001_Save_sys.U9001_save.current_step++;
//                                memcpy(&U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step],&U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step+1],sizeof(U9001_Setup_Typedef));
//                                
//                            }
//                                
//                                break;
//                            case 2:
//                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter=2;
//								SetDate_Comp();
//                                disp_all=1;
//                                break;
                            case 3:
                                 if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==OS_SETUP)
                                {
                                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower>10)
									{
										if(presscount>LKEY1)//����������
										{
											DISP_FLAG=TRUE;
											Key_Beep();
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower--;
										}
									}
                                }
                                else{
									if(presscount>LKEY2)//����������
									{
										DISP_FLAG=TRUE;
										Key_Beep();
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out-=10;
									}else if(presscount>LKEY1)//����������
									{
										DISP_FLAG=TRUE;
										Key_Beep();
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out--;
									}
								}
								SetDate_Comp();
    //                            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out>Hign_Vout[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter])//
    //                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out=Hign_Vout[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter];
                                break;
                            case 4:
								
								if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==IR_SETUP)
								{
									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper < 10000)
									{
										if(presscount>LKEY2)//����������
										{
											DISP_FLAG=TRUE;
											Key_Beep();
											 if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>10)
												U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper-=10;
											 else
												 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=1;
										}else if(presscount>LKEY1)//����������
										{
											DISP_FLAG=TRUE;
											Key_Beep();
											 if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>1)
												U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper-=1;
											 else
												 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=1;
										}
										
									 }else{
//										 if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>100)
											if(presscount>LKEY2)//����������
											{
												DISP_FLAG=TRUE;
												Key_Beep();
												U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper-=1000;
											}else if(presscount>LKEY1)//����������
											{
												DISP_FLAG=TRUE;
												Key_Beep();
												 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper-=100;
											}
//										 else
//											 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=1;
									 }
								}else{
									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper < 2000)
									{
										if(presscount>LKEY2)//����������
										{
											DISP_FLAG=TRUE;
											Key_Beep();
											 if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>10)
												U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper-=10;
											 else
												 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=1;
										}else if(presscount>LKEY1)//����������
										{
											DISP_FLAG=TRUE;
											Key_Beep();
											 if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>1)
												U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper-=1;
											 else
												 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=1;
										}
//										if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>1)
//											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper-=1;
//										 else
//											 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=1;
									}else{
										if(presscount>LKEY2)//����������
										{
											DISP_FLAG=TRUE;
											Key_Beep();
											 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper-=100;
										}else if(presscount>LKEY1)//����������
										{
											DISP_FLAG=TRUE;
											Key_Beep();
											 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper-=10;
										}
//										if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>10)
//											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper-=10;
//										 else
//											 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=10;
									}
                                    
								}
                                 
                            
                            
                            
    //                            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>High_Upper[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter])//
    //                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=High_Upper[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter];
                                break;
                             case 5:
                                 if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==OS_SETUP)
                                {
									if(presscount>LKEY1)//����������
									{
										DISP_FLAG=TRUE;
										Key_Beep();
										 if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>=100)
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper-=10;
										if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper<100)
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=0;
									}
//                                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>=100)
//                                        U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper-=10;
//                                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper<100)
//                                        U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=0;
                                }
                                else
                                {
									if(presscount>LKEY1)//����������
									{
										DISP_FLAG=TRUE;
										Key_Beep();
										if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time>1)
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time--;
									}
									 SetDate_Comp();
								 }
								
    //                             else
    //                                 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time=0;
    //                            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper.num>High_Upper[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter])//
    //                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper.num=High_Upper[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter];
                                break;
                            case 6:
								if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==IR_SETUP)
								{
									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower < 10000)
									{
										if(presscount>LKEY2)//����������
										{
											DISP_FLAG=TRUE;
											Key_Beep();
											 if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower>10)
												U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower-=10;
											 else
												 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower=1;
										}else if(presscount>LKEY1)//����������
										{
											DISP_FLAG=TRUE;
											Key_Beep();
											if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower>1)
												U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower-=1;
											 else
												 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower=1;
										}
//										 if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower>1)
//											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower-=1;
//										 else
//											 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower=1;
									 }else{
										 if(presscount>LKEY2)//����������
										{
											DISP_FLAG=TRUE;
											Key_Beep();
											 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower-=1000;
										}else if(presscount>LKEY1)//����������
										{
											DISP_FLAG=TRUE;
											Key_Beep();
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower-=100;
										}
//										 if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>100)
											
//											 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=1;
									 }
								}else{
									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper < 2000)
									{
										if(presscount>LKEY2)//����������
										{
											DISP_FLAG=TRUE;
											Key_Beep();
											 if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower>10)
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower-=10;
										else
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower=0;
										}else if(presscount>LKEY1)//����������
										{
											DISP_FLAG=TRUE;
											Key_Beep();
											if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower>1)
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower-=1;
										else
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower=0;
										}
									}else{
										if(presscount>LKEY2)//����������
										{
											DISP_FLAG=TRUE;
											Key_Beep();
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower-=100;
										}else if(presscount>LKEY1)//����������
										{
											DISP_FLAG=TRUE;
											Key_Beep();
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower-=10;
										}
										
									}
								}
                                    
                                break;
                            case 7:
								if(presscount>LKEY1)//����������
								{
									DISP_FLAG=TRUE;
									Key_Beep();
									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].rise_time)
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].rise_time--;
								}
                                break;
                            case 8:
                                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==IR)
                                {
									DISP_FLAG=TRUE;
									 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc=0;
//                                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc)
//                                        U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc--;
                                }
    //                            else
    //                             U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc--;
                                break;
                            case 9:
								if(presscount>LKEY1)//����������
								{
									DISP_FLAG=TRUE;
									Key_Beep();
									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].drop_time)
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].drop_time--;
								}
                                break;
                            case 10:
                                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==DC)
                                {
									DISP_FLAG=TRUE;
									if(presscount>LKEY1)//����������
									{
										Key_Beep();
										if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last)
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last--; 
									}
                                }else{
//                                    U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check=1;
                                }
                                break;
                            case 11:
								if(presscount>LKEY1)//����������
								{
									DISP_FLAG=TRUE;
									Key_Beep();
                                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check)
                                   U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check--;
								}
                                
                                break;
                            default:
                                break;
                            
                        }
                        break;
                    case Key_F4:
						Key_Beep();
						presscount=0;
                        switch(item)//�ڼ���
                        {
                            case 0:
                                SetSystemStatus(SYS_STATUS_SYS);
                                break;
                            case 1:
                                
                                if(U9001_Save_sys.U9001_save.current_step>1)
                                    U9001_Save_sys.U9001_save.current_step--;
                                disp_all=1;
								DISP_FLAG=TRUE;
                                
                                break;
                            case 2:
                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter=3;
                                SetDate_Comp();
								disp_all=1;
								DISP_FLAG=TRUE;
                                break;
                            case 3:
                                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==OS_SETUP)
                                {
                                    DISP_FLAG=TRUE;
                                    U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower-=10;
                                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower<=10)
                                        U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower=10;
                                }
                                else
                                {
									DISP_FLAG=TRUE;
                                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out>=150)
                                    U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out-=100;
                                }
    //                            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out>Hign_Vout[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter])//
    //                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out=Hign_Vout[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter];
                                break;
                            case 4:
								if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==IR_SETUP)
								{
									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper < 10000)
									{
										DISP_FLAG=TRUE;
										 if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>100)
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper-=100;
										 else
											 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=1;
									 }else{
//										 if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>1000)
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper-=1000;
//										 else
//											 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=1;
									 }
								}else{
									DISP_FLAG=TRUE;
                                   if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>100)
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper-=100;
									else
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=1;
								}
                                
                                
    //                            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>High_Upper[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter])//
    //                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=High_Upper[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter];
                                break;
                            case 5:
                                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==OS_SETUP)
                                {
									DISP_FLAG=TRUE;
                                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>=100)
                                    U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper-=100;
                                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper<100)
                                        U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=0;
                                }
                                else
                                {
									DISP_FLAG=TRUE;
                                     if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time>10)
                                        U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time-=10;
                                     else
                                         U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time=0;
									 SetDate_Comp();
                                 }
								
    //                            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper.num>High_Upper[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter])//
    //                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper.num=High_Upper[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter];
                                break;
                            case 6:
								if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==IR_SETUP)
								{
									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower < 10000)
									{
										DISP_FLAG=TRUE;
										 if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower>100)
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower-=100;
										 else
											 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower=1;
									 }else{
										 DISP_FLAG=TRUE;
//										 if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>1000)
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower-=1000;
//										 else
//											 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=1;
									 }
								}else{
									DISP_FLAG=TRUE;
									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower>100)
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower-=100;
									else
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower=0;
								}
                                break;
                            case 7:
								DISP_FLAG=TRUE;
                                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].rise_time>10)
                                    U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].rise_time-=10;
                                else
                                    U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].rise_time=0;
                                break;
                            case 8:
								DISP_FLAG=TRUE;
                                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==IR)
                                {
                                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc)
                                        U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc--;
                                }
    //                            else
    //                             U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc--;
                                break;
                            case 9:
								DISP_FLAG=TRUE;
                                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].drop_time>10)
                                    U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].drop_time-=10;
                                else
                                    U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].drop_time=0;
                                break;
                            case 10:
								DISP_FLAG=TRUE;
                                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==DC)
                                {
                                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last>10)
                                        U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last-=10; 
                                    else
                                        U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last=0;
                                }else
                                {
                                    U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check=1;
                                }
                                break;
                            case 11:
								DISP_FLAG=TRUE;
                                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check>10)
                                        U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check-=10;
                                    else
                                        U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check=0;
                                
                                break;
                            default:
                                break;
                            
                        }
                        break;
					 case L_Key_F4:
						 
						presscount++;
                        switch(item)//�ڼ���
                        {
                            case 0:
                                SetSystemStatus(SYS_STATUS_SYS);
                                break;
//                            case 1:
//                                if(presscount>LKEY1)
//								{
//									if(U9001_Save_sys.U9001_save.current_step>1)
//										U9001_Save_sys.U9001_save.current_step--;
//								}
//                                disp_all=1;
//                            
//                                
//                                break;
//                            case 2:
//                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter=3;
//                                SetDate_Comp();
//								disp_all=1;
//                                break;
                            case 3:
                                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==OS_SETUP)
                                {
                                     if(presscount>LKEY1)
									 {
										 DISP_FLAG=TRUE;
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower-=10;
									 }
                                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower<=10)
                                        U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower=10;
                                }
                                else
                                {
									 if(presscount>LKEY3)
									 {
										 DISP_FLAG=TRUE;
										if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out>=150)
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out-=300;
									 }else if(presscount>LKEY2){
										 DISP_FLAG=TRUE;
										if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out>=150)
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out-=200;
									 }else if(presscount>LKEY1){
										 DISP_FLAG=TRUE;
										if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out>=150)
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out-=100;
									 }
                                }
    //                            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out>Hign_Vout[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter])//
    //                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out=Hign_Vout[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter];
                                break;
                            case 4:
								if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==IR_SETUP)
								{
									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper < 10000)
									{
										 if(presscount>LKEY3)
										 {
											 DISP_FLAG=TRUE;
											 Key_Beep();
											if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>300)
												U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper-=300;
											else
												U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=1;
										 }else if(presscount>LKEY2){
											 DISP_FLAG=TRUE;
											 Key_Beep();
											if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>200)
												U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper-=200;
											else
												U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=1;
										 }else if(presscount>LKEY1){
											 DISP_FLAG=TRUE;
											 Key_Beep();
											if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>100)
												U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper-=100;
											else
												U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=1;
										 }
										 
									 }else{
//										 if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>1000)
										 if(presscount>LKEY3)
										 {
											 DISP_FLAG=TRUE;
											 Key_Beep();
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper-=3000;
										 }else if(presscount>LKEY2){
											 DISP_FLAG=TRUE;
											 Key_Beep();
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper-=2000;
										 }else if(presscount>LKEY1){
											 DISP_FLAG=TRUE;
											 Key_Beep();
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper-=1000;
										 }
											
//										 else
//											 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=1;
									 }
								}else{
									if(presscount>LKEY3)
									 {
										 DISP_FLAG=TRUE;
										 Key_Beep();
										if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>300)
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper-=300;
										else
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=1;
									 }else if(presscount>LKEY2){
										 DISP_FLAG=TRUE;
										 Key_Beep();
										if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>200)
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper-=200;
										else
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=1;
									 }else if(presscount>LKEY1){
										 DISP_FLAG=TRUE;
										 Key_Beep();
										  if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>100)
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper-=100;
										else
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=1;
									 }
                                 
								}
                                
                                
    //                            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>High_Upper[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter])//
    //                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=High_Upper[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter];
                                break;
                            case 5:
                                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==OS_SETUP)
                                {
									if(presscount>LKEY1){
										DISP_FLAG=TRUE;
										Key_Beep();
										if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>=100)
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper-=100;
										if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper<100)
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=0;
									}
                                }
                                else
                                {
									if(presscount>LKEY1){
										DISP_FLAG=TRUE;
										Key_Beep();
										 if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time>10)
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time-=10;
										 else
											 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time=0;
									 }
									 SetDate_Comp();
                                 }
								
    //                            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper.num>High_Upper[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter])//
    //                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper.num=High_Upper[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter];
                                break;
                            case 6:
								if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==IR_SETUP)
								{
									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower < 10000)
									{
										if(presscount>LKEY3)
										 {
											 DISP_FLAG=TRUE;
											 Key_Beep();
											if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower>300)
												U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower-=300;
											 else
												 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower=1;
										 }else if(presscount>LKEY2){
											 DISP_FLAG=TRUE;
											 Key_Beep();
											if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower>200)
												U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower-=200;
											 else
												 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower=1;
										 }else if(presscount>LKEY1){
											 DISP_FLAG=TRUE;
											 Key_Beep();
											if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower>100)
												U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower-=100;
											 else
												 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower=1;
										 }
										 
									 }else{
										 if(presscount>LKEY3)
										 {
											 DISP_FLAG=TRUE;
											 Key_Beep();
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower-=3000;
										 }else if(presscount>LKEY2){
											 DISP_FLAG=TRUE;
											 Key_Beep();
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower-=2000;
										 }else if(presscount>LKEY1){
											 DISP_FLAG=TRUE;
											 Key_Beep();
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower-=1000;
										 }
//										 if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>1000)
//											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower-=1000;
//										 else
//											 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=1;
									 }
								}else{
									if(presscount>LKEY3)
										 {
											 DISP_FLAG=TRUE;
											 Key_Beep();
											if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower>300)
												U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower-=300;
											 else
												 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower=0;
										 }else if(presscount>LKEY2){
											 DISP_FLAG=TRUE;
											 Key_Beep();
											if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower>200)
												U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower-=200;
											 else
												 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower=0;
										 }else if(presscount>LKEY1){
											 DISP_FLAG=TRUE;
											 Key_Beep();
											if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower>100)
												U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower-=100;
											 else
												 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower=0;
										 }
//									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower>100)
//										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower-=100;
//									else
//										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower=0;
								}
                                break;
                            case 7:
								if(presscount>LKEY1){
									DISP_FLAG=TRUE;
									Key_Beep();
									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].rise_time>10)
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].rise_time-=10;
									else
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].rise_time=0;
								}
                                break;
                            case 8:
                                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==IR)
                                {
									if(presscount>LKEY1){
										DISP_FLAG=TRUE;
										Key_Beep();
										if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc)
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc--;
									}
                                }
    //                            else
    //                             U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc--;
                                break;
                            case 9:
								if(presscount>LKEY1){
									DISP_FLAG=TRUE;
									Key_Beep();
									if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].drop_time>10)
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].drop_time-=10;
									else
										U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].drop_time=0;
								}
                                break;
                            case 10:
                                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==DC)
                                {
									if(presscount>LKEY1){
										DISP_FLAG=TRUE;
										Key_Beep();
										if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last>10)
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last-=10; 
										else
											U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last=0;
									}
                                }else
                                {
//                                    U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check=1;
                                }
                                break;
                            case 11:
								if(presscount>LKEY1){
									DISP_FLAG=TRUE;
									Key_Beep();
                                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check>10)
                                        U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check-=10;
                                    else
                                        U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check=0;
								}
                                
                                break;
                            default:
                                break;
                            
                        }
                        break;
                    case Key_F5:
                        switch(item)//�ڼ���
                        {
                            case 0:
                                
                                break;
                            case 1:
								Key_Beep();
								DISP_FLAG=TRUE;
                                 if(U9001_Save_sys.U9001_save.current_step<U9001_Save_sys.U9001_save.all_step)
                                    U9001_Save_sys.U9001_save.current_step++;
                                 SetDate_Comp();
                                 disp_all=1;
                                break;
                            case 2:
								Key_Beep();
                                DISP_FLAG=TRUE;
                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter=4;
                                SetDate_Comp();
                            disp_all=1;
                                break;
                            case 3:
    //                          U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out+=100;
    //                            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out>Hign_Vout[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter])//
    //                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out=Hign_Vout[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter];
                                break;
                            case 4:
								Key_Beep();
								DISP_FLAG=TRUE;
								if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter == IR_SETUP)
								{
									U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=0;
								}
                                break;
                            case 5:
								Key_Beep();
								DISP_FLAG=TRUE;
                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time=0;
                                break;
                            case 6:
								Key_Beep();
								DISP_FLAG=TRUE;
                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower=0;
                                break;
                            case 7:
								Key_Beep();
								DISP_FLAG=TRUE;
                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].rise_time=0;
                                break;
                            case 8:
								Key_Beep();
								DISP_FLAG=TRUE;
                                 if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==IR)
                                {
                                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc)
                                        U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc--;
                                }
                                else
                                 U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc=0;
                                break;
                            case 9:
								Key_Beep();
								DISP_FLAG=TRUE;
                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].drop_time=0;
                                break;
                            case 10:
								Key_Beep();
								DISP_FLAG=TRUE;
                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last=0;
                            break;
                            case 11:
								Key_Beep();
								DISP_FLAG=TRUE;
                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check=0;
                                break;
                            
                        }
                        break;
                    case Key_STOP:
                        SetSystemStatus(SYS_STATUS_IDEM);
                    break;
                    
                    case Key_Disp:
                        Key_Beep();
                            //SetSystemStatus(SYS_STATUS_TEST);
                        SetSystemStatus(SYS_STATUS_IDEM);
                    break;
    //				case Key_SETUP:
    //                        SetSystemStatus(SYS_STATUS_SETUPTEST);
    //				break;
                    case Key_FAST:

                        
                        
                    
                    break;
                    case Key_RIGHT:
						Key_Beep();
                        if(item%2 == 1)
						{
							if(item>=MAX_SETP[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter])
							{
//								if(item>MAX_SETP[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter])
//								{
//									
//								}else{
//									DISP_FLAG=TRUE;
//								}
								item=MAX_SETP[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter];
							}else{
								item++;
								DISP_FLAG=TRUE;
							}
							
						}							
                    break;
                    case Key_LEFT:
						Key_Beep();
                        if(item%2 == 0)
						{
							DISP_FLAG=TRUE;
							item--;
						}                  
                    break;
                    case Key_DOWN:
						Key_Beep();
//						DISP_FLAG=TRUE;
						if(item>=MAX_SETP[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter])
						{
//								if(item>MAX_SETP[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter])
//								{
//									
//								}else{
//									DISP_FLAG=TRUE;
//								}
							item=MAX_SETP[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter];
						}else{
							if(item != 0)
							{
								item += 2; 
							}else{
								item += 1;
							}
							DISP_FLAG=TRUE;
						}					
                    break;
                    case Key_UP:
						Key_Beep();
						
                        if(item == 1)
						{
							item -= 1; 
							DISP_FLAG=TRUE;
						}else if(item == 0){
							
						}else{
							item -= 2;
							DISP_FLAG=TRUE;
						}
                        
                    break;


                    case Key_PAGE://��ҳ
						Key_Beep();
                        SetSystemStatus(SYS_STATUS_SYSSET);
                    break;
                    
                    case Key_COMP:
                        
                                
                    break;
                    case Key_SYST:
                        
                        break;
                    case Key_FILE:
//                        SetSystemStatus(SYS_STATUS_FILE);
                        break;
    //				case Key_EXIT:
    //                    SetSystemStatus(SYS_STATUS_IDEM);
    //				break;
                    
                    default:
                    break;
                        
                }
            }
            else
            {
                switch(keyvalue)
                {
//                    case Key_RIGHT:
//                        input_flag=0;
//                        list=0;						
//                    break;
//                    case Key_LEFT:
//                        input_flag=0;
//                        list=1;                   
//                    break;
//                    case Key_DOWN:
//                        if(line<(MAX_SETP[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter]+1)/2)
//                            line++;  
//                        input_flag=0;                        
//                        
//                    break;
//                    case Key_UP:
//                        if(line)
//                        {
//                            line--;
//                                              
//                        }
//                        if(line==0)
//                           list=0;
//                       input_flag=0;
//                        
//                    break;
//                    case Key_SETUP:
//                        input_flag=0;
//                        break;
//                    case Key_Disp:
//                        SetSystemStatus(SYS_STATUS_ITEM);
//                        break;
                    case Key_F1:
						DISP_FLAG=TRUE;
                        input_flag=0;
						Key_Beep();
                        setvalue =atof((char*)Disp_buff);
                        setvalue+=0.000001f;
                        switch(item)
                        {
                            case 3://��ѹ����
                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out=setvalue;
								SetDate_Comp();
                                break;
                            case 4:
                                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==OS_SETUP)
                                {
//                                   if(setvalue >= 1)
//								   {
//									   U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last=setvalue*1e2;
//								   }else{
									   U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last=setvalue*1e3;
//								   }
                                }
                                else
                                {
                                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==IR_SETUP)
                                        setvalue*=10;
                                    U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=setvalue;
                                }
								SetDate_Comp();
                            break;
                            case 5:
                                setvalue*=10;
                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time=setvalue;
								SetDate_Comp();
                                break;
                            case 6:
                                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==IR_SETUP)
                                setvalue*=10;
                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower=setvalue;
								SetDate_Comp();
                                break;
                            case 7:
                                setvalue*=10;
                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].rise_time=setvalue;
								SetDate_Comp();
                                break;
                            case 8:
                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc=setvalue;
								SetDate_Comp();
                                break;
                            case 9:
                                setvalue*=10;
                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].drop_time=setvalue;
								SetDate_Comp();
                                break;
                            case 10:
								setvalue*=10;
                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last=setvalue;
								SetDate_Comp();
                                break;
                            case 11:
                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check=setvalue*10;
								SetDate_Comp();
                                break;
                                
                        }
                        break;
                    case Key_F2:
						DISP_FLAG=TRUE;
						Key_Beep();
                        input_flag=0;
                        setvalue =atof((char*)Disp_buff);
                        setvalue+=0.000001f;
                        switch(item)
                        {
                            case 3://��ѹ����
                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out=setvalue*1000;
								SetDate_Comp();
                                break;
                            case 4:
                                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==IR_SETUP)
                                {
                                    setvalue*=10;
                                }
                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper=setvalue*1000;
								SetDate_Comp();
                            break;
                            
                            case 6:
                                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter==IR_SETUP)
                                {
                                    setvalue*=10;
                                }
                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower=setvalue*1000;
								SetDate_Comp();
                                break;
                            
                            case 8:
                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc=setvalue*10;
								SetDate_Comp();
                                break;
                            case 9:
                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].drop_time=setvalue;
								SetDate_Comp();
                                break;
                           
                            case 11:
                                U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check=setvalue*10000;
								SetDate_Comp();
                                break;
                            default:
                                break;
                                
                        }
                        break;
                    case Key_F3:
                        break;
                    case Key_F5:
						DISP_FLAG=TRUE;
						Key_Beep();
                        input_flag=0;
                        
                        break;
                    case Key_NUM0:
                    case Key_NUM1:
                    case Key_NUM2:
                    case Key_NUM3:
                    case Key_NUM4:
                    case Key_NUM5:
                    case Key_NUM6:
                    case Key_NUM7:
                    case Key_NUM8:
                    case Key_NUM9:
                    case Key_DOT:  
						Key_Beep();
                        if(key_count<5)
                        {
                            Disp_buff[key_count]=keyvalue;
                            key_count++;

                        }
                        input_flag=1;
//                        Disp_Pre_Uint(keynum);
                        DISP_FLAG=TRUE;
                    break;
                    case Key_BACK:
						DISP_FLAG=TRUE;
						Key_Beep();
                         if(key_count)
                        {	
                            key_count--;
                            Disp_buff[key_count]=' ';

                        
                        }
                }
            
            }
            
            keyvalue=KEY_NONE;
		}
	 	
	
	
	
	}
	if(Tft_5520.Set_Value[Tft_5520.Group].Range)
	Range=Tft_5520.Set_Value[Tft_5520.Group].Range-1;
	else
		Range=MAX_R_RANGE-1;
	U9001_Save_sys.U9001_save.start_step = U9001_Save_sys.U9001_save.current_step;
	Savetoeeprom();
//	Save_fileDate(0);
//	lcd_Clear(LCD_COLOR_TEST_BACK);
}

void Setup_config_Process(void)
{
    u16 *pt;
	u8 list=0;//��
    u8 line=0;//��
    u8 item=0;
    u8 disp_all=1;
    u8 num;
    u8 input_flag=0;
    u8 Disp_buff[8];
    u8 key_count=0;
    u8 select_num=0;
    u8 lenth=5;
    float setvalue;
//	vu8 key;
//    U9001_Testconfg_Typedef ptt;
//	DISP_FLAG=TRUE;
//    GUI_SetColor(LCD_COLOR_TEST_BACK);
	GUI_SetBkColor(LCD_COLOR_TEST_BACK);
	GUI_Clear();
    pt=(u16*)&U9001_Save_sys.U9001_Testconfg;

    DISP_FLAG=TRUE;
    num=sizeof(U9001_Testconfg_Typedef)/2;
	pageflag = 3;//��������
 	while(GetSystemStatus()==SYS_STATUS_CLEAR)
	{
	    
		if(DISP_FLAG==TRUE)
		{
			
            if(disp_all)
            {
                
                GUI_SetColor(LCD_COLOR_TEST_BACK);
                GUI_FillRect(0,24,380,270);
                GUI_SetColor(GUI_WHITE);
                Disp_Test_SetConfig_Item();
                disp_all=0;
            }
            item=list*((num+1)/2)+line;
            if(item>num)
            {
                item=num;
                list=item/((num+1)/2);
                line=item-list*((num+1)/2);
            }
             if(input_flag)
            {
                disp_Inputnum((u8*)Disp_buff);
                
            }else
            {
                DispSetConfig_value(item);
                memset(Disp_buff,0,10);

                GUI_SetColor(LCD_COLOR_TEST_BACK);
                GUI_FillRect(20,214,350,242);

            }
            
            
			DISP_FLAG=FALSE;


		
		}

//		delayMs(1,100);
		
		if(keyvalue!=KEY_NONE)
		{	
            
			
			
            if(input_flag==0)
            {
			switch(keyvalue)
			{
                 case Key_NUM0:
                    case Key_NUM1:
                    case Key_NUM2:
                    case Key_NUM3:
                    case Key_NUM4:
                    case Key_NUM5:
                    case Key_NUM6:
                    case Key_NUM7:
                    case Key_NUM8:
                    case Key_NUM9:
                    case Key_DOT: 
						Key_Beep();
                         memset(Disp_buff,0,6);
                        if(key_count<lenth)
                        {
                            Disp_buff[key_count]=keyvalue;
                            key_count++;

                        }
                        input_flag=1;
						if(item == 7)
						{
							Disp_Pre_Uint(5);
						}else{
							Disp_Pre_Uint(1);
						}
                        DISP_FLAG=TRUE;
                case Key_F1:
                    switch(item)//�ڼ���
                    {
                        case 0:
							Key_Beep();
                            SetSystemStatus(SYS_STATUS_SETUPTEST);
                            break;
                        case 1:
//                            if(U9001_save_sys.U9001_Testconfg.pass_time)
                           //ParameterLimit
                            
                        case 2:
                            
                        case 3:
                        case 7:
							DISP_FLAG=TRUE;
							Key_Beep();
                            if(*(pt+(item-1))<ParameterLimit[item-1][1])
                                *(pt+(item-1))+=10;
                            if(*(pt+(item-1))>=ParameterLimit[item-1][1])
                                *(pt+(item-1))=ParameterLimit[item-1][1];
                            break;
                        case 4:
                            break;
                        case 5:
                        case 6:
                            
                            

                        case 8:
                        case 9:
                        case 10:
                        case 11:
                        case 12:
                        case 13:
                        case 14:
							DISP_FLAG=TRUE;
							Key_Beep();
                            *(pt+(item-1))=0;
                            break;
                        case 15:
							DISP_FLAG=TRUE;
							Key_Beep();
                            if(*(pt+(item-1))<ParameterLimit[item-1][1])
                                *(pt+(item-1))+=1;
                            break;
                        
                    }
                    break;
                case Key_F2:
                    switch(item)//�ڼ���
                    {
                        case 0:
                            
                            break;
                       case 1:

                            
                        case 2:
                            
                        case 3:
                        case 7:
							DISP_FLAG=TRUE;
							Key_Beep();
                            if(*(pt+(item-1))<ParameterLimit[item-1][1])
                                *(pt+(item-1))+=1;
                            break;
                        case 4:
                            break;
                        case 5:
                        case 6:
                            
                            

                        case 8:
                        case 9:
                        case 10:
                        case 11:
                        case 12:
                        case 13:
                        case 14:
							DISP_FLAG=TRUE;
							Key_Beep();
                            *(pt+(item-1))=1;
                            break;
                        case 15:
							DISP_FLAG=TRUE;
							Key_Beep();
                            if(*(pt+(item-1)))
                                *(pt+(item-1))-=1;
                            break;
                        
                    }
                    break;
                case Key_F3:
                    switch(item)//�ڼ���
                    {
                        case 0:
                            SetSystemStatus(SYS_STATUS_SYSSET);
                            break;
                         case 1:

                            
                        case 2:
                            
                        case 3:
                        case 7:
							DISP_FLAG=TRUE;
							Key_Beep();
                            if(*(pt+(item-1)))
                                *(pt+(item-1))-=1;
							if(*(pt+(item-1))<ParameterLimit[item-1][0])
                                *(pt+(item-1))=ParameterLimit[item-1][0];
                            break;
                        case 4:
                            break;
                        case 5:
                        case 6:
                            
                            

                        case 8:
                        case 9:
                        case 10:
                            break;
                        case 11:
							DISP_FLAG=TRUE;
							Key_Beep();
                            *(pt+(item-1))=2;
                        case 12:
                        case 13:
							break;
                        case 14:
							DISP_FLAG=TRUE;
							Key_Beep();
							*(pt+(item-1))=0;
							U9001_Save_sys.U9001_save.disp=1;
							clearflag=1;
							clearstart=1;
							SetSystemStatus(SYS_STATUS_IDEM);
//							SetSystemStatus(SYS_STATUS_START);
//							SetSystemMessage(MSG_RAMP);
//							Uart0_Send(0xa1);
                            //����
                            break;
                        case 15:
                            
                            break;
                        
                    }
                    break;
                case Key_F4:
                    switch(item)//�ڼ���
                    {
                        case 0:
                            
                            break;
                         case 1:

                            
                        case 2:
                            
                        case 3:
                        case 7:
							DISP_FLAG=TRUE;
							Key_Beep();
                            if(*(pt+(item-1))>10)
                                *(pt+(item-1))-=10;
                            else
                                *(pt+(item-1))=0;
							if(*(pt+(item-1))<ParameterLimit[item-1][0])
                                *(pt+(item-1))=ParameterLimit[item-1][0];
                            break;
                        case 4:
                            break;
                        case 5:
                        case 6:
                            
                            

                        case 8:
                        case 9:
                        case 10:
                            break;
                        case 11:
							DISP_FLAG=TRUE;
							Key_Beep();
                            *(pt+(item-1))=2;
                        case 12:
                        case 13:
                        case 14:
                            
                            break;
                        case 15:
                            
                            break;
                        
                    }
                    break;
                case Key_F5:
                   switch(item)//�ڼ���
                    {
                        case 0:
                            
                            break;
                         case 1:   
                            
                        case 3:
                            break;
                        case 2:
							DISP_FLAG=TRUE;
							Key_Beep();
                            *(pt+(item-1))=1000;
                        break;
                        case 7:
							DISP_FLAG=TRUE;
                            Key_Beep();
                             *(pt+(item-1))=0;
                            break; 
                        case 4:
                            break;
                        case 5:
                        case 6:
                            
                        case 8:
                        case 9:
                        case 10:
                            break;
                        case 11:
                        case 12:
                        case 13:
                        case 14:
                            
                            break;
                        case 15:
                            
                            break;
                        
                    }
                    break;
				case Key_STOP:
					SetSystemStatus(SYS_STATUS_IDEM);
                break;
				
				case Key_Disp:
					Key_Beep();
                        //SetSystemStatus(SYS_STATUS_TEST);
					SetSystemStatus(SYS_STATUS_IDEM);
				break;

				case Key_FAST:
					
				break;
				case Key_RIGHT:
					DISP_FLAG=TRUE;
					Key_Beep();
                    list=1;
                    						
				break;
				case Key_LEFT:
					DISP_FLAG=TRUE;
					Key_Beep();
                    list=0;

				break;
				case Key_DOWN:
					DISP_FLAG=TRUE;
					Key_Beep();
					if(line<(num+1)/2)
                        line++;
					
				break;
				case Key_UP:
					DISP_FLAG=TRUE;
					Key_Beep();
                    if(line)
                        line--;
                    if(line==0)
                        list=0;
				break;
				

				case Key_PAGE://��ҳ
					Key_Beep();
                    SetSystemStatus(SYS_STATUS_SYSSET);
				break;
				
				case Key_COMP:
					
							
				break;
//				case Key_EXIT:
//                    SetSystemStatus(SYS_STATUS_IDEM);
//				break;
				
				default:
				break;
					
			}
        }
        else
        {
                switch(keyvalue)
                {
//                    case Key_RIGHT:
//						Key_Beep();
//                        input_flag=0;
//                        list=0;						
//                    break;
//                    case Key_LEFT:
//						Key_Beep();
//                        input_flag=0;
//                        list=1;                   
//                    break;
//                    case Key_DOWN:
////                        if(line<(MAX_SETP[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter]+1)/2)
////                            line++;  
////                        input_flag=0;                        
//                        
//                    break;
//                    case Key_UP:
////                        if(line)
////                        {
////                            line--;
////                                              
////                        }
////                        if(line==0)
////                           list=0;
////                       input_flag=0;
//                        
//                    break;
//                    case Key_SETUP:
//						Key_Beep();
//                        input_flag=0;
//						
//                        break;
//                    case Key_Disp:
//						Key_Beep();
//                        SetSystemStatus(SYS_STATUS_ITEM);
//                        break;
                    case Key_F1:
						DISP_FLAG=TRUE;
						Key_Beep();
						if(item != 7)
						{
							input_flag=0;
							key_count = 0;
							setvalue =atof((char*)Disp_buff);
							setvalue+=0.000001f;
							setvalue*=10;
						}else{
							input_flag=0;
							key_count = 0;
							setvalue =atof((char*)Disp_buff);
							setvalue+=0.000001f;
						}
                        switch(item)
                        {
                            case 1://
                                
                                U9001_Save_sys.U9001_Testconfg.pass_time=setvalue;
                                break;
                            case 2:
                                U9001_Save_sys.U9001_Testconfg.step_time=setvalue;
                            break;
                            case 3:
                               
                                U9001_Save_sys.U9001_Testconfg.delay_time=setvalue;
                                break;
							case 7:
                               
                                U9001_Save_sys.U9001_Testconfg.comp_setp=setvalue;
                                break;
                            case 15:
                                U9001_Save_sys.U9001_Testconfg.dis_time=setvalue;
                                break;
                                
                        }
						if(*(pt+(item-1))>=ParameterLimit[item-1][1])
                               *(pt+(item-1))=ParameterLimit[item-1][1];
						if(*(pt+(item-1))<=ParameterLimit[item-1][0])
                               *(pt+(item-1))=ParameterLimit[item-1][0];
                        break;
                    case Key_F2:
                        
                        break;
                    case Key_F3:
                        break;
                    case Key_F5:
						DISP_FLAG=TRUE;
						Key_Beep();
                        input_flag=0;
                        
                        break;
                    case Key_NUM0:
                    case Key_NUM1:
                    case Key_NUM2:
                    case Key_NUM3:
                    case Key_NUM4:
                    case Key_NUM5:
                    case Key_NUM6:
                    case Key_NUM7:
                    case Key_NUM8:
                    case Key_NUM9:
                    case Key_DOT:  
						Key_Beep();						
                        if(key_count<lenth)
                        {
                            Disp_buff[key_count]=keyvalue;
                            key_count++;

                        }
                        input_flag=1;
//                        Disp_Pre_Uint(keynum);
                        DISP_FLAG=TRUE;
                    break;
                    case Key_BACK:
						DISP_FLAG=TRUE;
						Key_Beep();
                         if(key_count)
                        {	
                            key_count--;
                            Disp_buff[key_count]=' ';
                        }
                }
            
            }
		
		keyvalue=KEY_NONE;
		}
	 	
	
	
	
	}
	if(Tft_5520.Set_Value[Tft_5520.Group].Range)
	Range=Tft_5520.Set_Value[Tft_5520.Group].Range-1;
	else
		Range=MAX_R_RANGE-1;
	Savetoeeprom();
    
//	lcd_Clear(LCD_COLOR_TEST_BACK);
}
//���ݱ������
void Data_StoreProcess(void)
{
	vu32 keynum=0;
	vu8 key;
	Delay_Key();
 	while(GetSystemStatus()==SYS_STATUS_DATASTORE)
	{
	 key=HW_KeyScsn();
		if(key==0xff)
		{
			keynum=0;
		}
		else
			keynum++;
		if(keynum==2)
		{
			switch(key)
			{
				case Key_F1:
				break;
				case Key_F2:
				break;
				case Key_F3:
				break;
				case Key_F4:
				break;
				case Key_F5:
				break;
				case Key_Disp:
				break;
				case Key_SETUP:
				break;
				case Key_FAST:
				break;
				case Key_LEFT:
				break;
				case Key_RIGHT:
				break;
				case Key_UP:
				break;
				case Key_DOWN:
				break;
				case Key_NUM1:
				break;
				case Key_NUM2:
				break;
				case Key_NUM3:
				break;
				case Key_NUM4:
				break;
				case Key_NUM5:
				break;
				case Key_NUM6:
				break;
				case Key_NUM7:
				break;
				case Key_NUM8:
				break;
				case Key_NUM9:
				break;
				case Key_NUM0:
				break;
				case Key_DOT:
				break;
				case Key_BACK:
				break;
				
				default:
				break;
					
			}
		
		
		}
	 	
	
	}
}


void Sys_Process(void)
{
	GUI_SetBkColor(LCD_COLOR_TEST_BACK);
	GUI_Clear();

	while(GetSystemStatus()==SYS_STATUS_SYS)
	{
		if(DISP_FLAG==1)
		{
			Disp_Sys();
			DispSYSConfig_value();
			DISP_FLAG=0;	
			
		}

//		keyvalue=Key_Read_WithTimeOut(TICKS_PER_SEC_SOFTTIMER/10);
		if(keyvalue!=KEY_NONE)
		{
//			DISP_FLAG=1;
            
			switch(keyvalue)
			{
                case Key_F1:
					Key_Beep();
                    SetSystemStatus(SYS_STATUS_SYSSET);
                break;
				case  Key_LEFT:
                    //SetSystemStatus(SYS_STATUS_SYSSET);
				//break;
				case Key_PAGE://��ҳ
                    SetSystemStatus(SYS_STATUS_ITEM);
				
				
				case Key_Disp:
					Key_Beep();
                    SetSystemStatus(SYS_STATUS_IDEM);
				break;
				case Key_SETUP:
					Key_Beep();
                    SetSystemStatus(SYS_STATUS_SETUPTEST);
				break;
                case Key_STOP:
				case Key_EXIT:
                    SetSystemStatus(SYS_STATUS_IDEM);
                break;
				default:
				break;
					
			}
		
		keyvalue=KEY_NONE;
		}

	
	}

}

//ϵͳ����
void Use_SysSetProcess(void)
{
	
//    vu16 *pt;
	u8 list=0;//��
    u8 line=0;//��
    u8 item=0;
    u8 disp_all=1;
//	vu8 key;
	RTC_TIME_Type pFullTime;
	DISP_FLAG=TRUE;
//    GUI_SetColor(LCD_COLOR_TEST_BACK);
	GUI_SetBkColor(LCD_COLOR_TEST_BACK);
	GUI_Clear();
    

	
	

//	Disp_Sys_Item(2);
//	Disp_Fastbutton(2);
//	Disp_Fastset_Disp(2);
//	Disp_mainitem(2);
//	DISP_FLAG=1;
//	//Delay_Key();
 	while(GetSystemStatus()==SYS_STATUS_SYSSET)
	{
	  	if(DISP_FLAG==TRUE)
		{
//            if(line==0)
//                list=0;
//            if(line<7)
//                item=list*6+line;
           
            if(disp_all)
            {
                
                GUI_SetColor(LCD_COLOR_TEST_BACK);
                GUI_FillRect(0,24,380,270);
                GUI_SetColor(GUI_WHITE);
                Disp_SYS_Set_Item();
                disp_all=0;
            }
            Disp_SysSet_value(line);
					DISP_FLAG=FALSE;
		
		}
//		if(DISP_FLAG==1)
//		{
//			Disp_Sys_value(list);
//			DISP_FLAG=0;
//						
//		}
//		
//		Disp_Set_Time(list);

//		key=Key_Read_WithTimeOut(TICKS_PER_SEC_SOFTTIMER/10);
		
		if(keyvalue!=KEY_NONE)
		{
			
			
			switch(keyvalue)
			{
				case Key_F1:
					DISP_FLAG=1;
					Key_Beep();
					switch(line)
					{
						case 0:
						{
//							SetSystemStatus(SYS_STATUS_SYS);
						}break;
						case 1:
						{
							U9001_Save_sys.U9001_SYS.language = 0;
						}break;
						case 2:
						{
							U9001_Save_sys.U9001_SYS.pass_beep = 0;
						}break;
						case 3:
						{
							U9001_Save_sys.U9001_SYS.fail_beep = 0;
						}break;
						case 4:
						{
							U9001_Save_sys.U9001_SYS.key_beep = 0;
						}break;
						case 5:
						{
							U9001_Save_sys.U9001_SYS.bussmode = 0;
						}break;
						case 6:
						{
							U9001_Save_sys.U9001_SYS.buss_addr++;
							SetDate_Comp();
						}break;
						case 7:
						{
							U9001_Save_sys.U9001_SYS.buard = 0;
						}break;
						default:break;
					}
				break;
				case Key_F2:
					DISP_FLAG=1;
					Key_Beep();
					switch(line)
					{
						case 0:
						{
							SetSystemStatus(SYS_STATUS_SYS);
						}break;
						case 1:
						{
							U9001_Save_sys.U9001_SYS.language = 1;
						}break;
						case 2:
						{
							U9001_Save_sys.U9001_SYS.pass_beep = 1;
						}break;
						case 3:
						{
							U9001_Save_sys.U9001_SYS.fail_beep = 1;
						}break;
						case 4:
						{
							U9001_Save_sys.U9001_SYS.key_beep = 1;
						}break;
						case 5:
						{
							U9001_Save_sys.U9001_SYS.bussmode = 1;
						}break;
						case 6:
						{
							U9001_Save_sys.U9001_SYS.buss_addr--;
							SetDate_Comp();
						}break;
						case 7:
						{
							U9001_Save_sys.U9001_SYS.buard = 1;
						}break;
						default:break;
					}
				break;
				case Key_F3:
					DISP_FLAG=1;
					Key_Beep();
					switch(line)
					{
						case 0:
						{
//							SetSystemStatus(SYS_STATUS_SYS);
						}break;
						case 1:
						{
//							U9001_save_sys.U9001_SYS.language = 1;
						}break;
						case 2:
						{
							U9001_Save_sys.U9001_SYS.pass_beep = 2;
						}break;
						case 3:
						{
							U9001_Save_sys.U9001_SYS.fail_beep = 2;
						}break;
						case 4:
						{
//							U9001_save_sys.U9001_SYS.key_beep = 1;
						}break;
//						case 5:
//						{
//							U9001_save_sys.U9001_SYS.key_beep = 1;
//						}break;
						case 7:
						{
							U9001_Save_sys.U9001_SYS.buard = 2;
						}break;
						default:break;
					}
				break;
				case Key_F4:
					DISP_FLAG=1;
					Key_Beep();
					switch(line)
					{
						case 7:
						{
							U9001_Save_sys.U9001_SYS.buard = 3;
						}break;
						default:break;
					}
				break;
				case Key_F5:
					DISP_FLAG=1;
					Key_Beep();
					switch(line)
					{
						case 7:
						{
							U9001_Save_sys.U9001_SYS.buard = 4;
						}break;
						default:break;
					}
				break;
				case Key_Disp:
					Key_Beep();
                    SetSystemStatus(SYS_STATUS_IDEM);
				break;
				case Key_SETUP:
					Key_Beep();
                    SetSystemStatus(SYS_STATUS_SETUPTEST);
				break;
				case Key_FAST:
				break;
				case Key_LEFT:
					DISP_FLAG=1;
					Key_Beep();
                   if(line > 4)
					{
						line-=4;
					}
				break;
				case Key_RIGHT:
					DISP_FLAG=1;
					Key_Beep();
                    if(line < 4)
					{
						line+=4;
					}else if(line == 4){
						line = 7;
					}
				break;
//                case Key_LEFT:
                    
				case Key_DOWN:
					DISP_FLAG=1;
					Key_Beep();
					if(line<7)
						line++;
				break;
				case Key_UP:
					DISP_FLAG=1;
					Key_Beep();
					if(line)
						line--;
				break;
				
				case Key_PAGE://��ҳ
					Key_Beep();
                    SetSystemStatus(SYS_STATUS_SYS);
				break;

				case Key_EXIT:
                case Key_STOP:
                    SetSystemStatus(SYS_STATUS_IDEM);
				break;
				
				default:
				break;
					
			}
			//shi=RTC_ReadGPREG (LPC_RTC, 0);
			if(CONFIG_TIME!=RTC_ReadGPREG (LPC_RTC, 0))//���Ĵ������ݽ����ж�
			{
				RTC_Init(LPC_RTC);
				LPC_RTC->SEC=pFullTime.SEC;
				LPC_RTC->MIN=pFullTime.MIN;
				LPC_RTC->HOUR=pFullTime.HOUR;
				
				LPC_RTC->DOM=pFullTime.DOM;
				
				LPC_RTC->MONTH=pFullTime.MONTH;
				LPC_RTC->YEAR=pFullTime.YEAR;
				RTC_WriteGPREG (LPC_RTC,0 ,CONFIG_TIME);
				RTC_ResetClockTickCounter(LPC_RTC);
				RTC_Cmd(LPC_RTC, ENABLE);
				//RTC_SetFullTime (LPC_RTC, &RtcSet);
			
			}
				
		keyvalue=KEY_NONE;
		}
	
	
	}
	Savetoeeprom();
//	lcd_Clear(LCD_COLOR_TEST_BACK);
}
//У׼ֵ�ϡ����� ����ֵ
//const vu16 CalibrateLimit[][2]=
//{
//	 1100, 900,
//	 1100, 900, 
//	 1100, 900, 
//	 1100, 900, 
//	 1100, 900, 
//	 1200, 800, 
//	 1200, 800, 
//	 1200, 800, 
//	1200, 800, 
//};
const vu16 CalibrateLimitvalue[]=
{
	 1000, 
	 1000,  
	 1000,  
	 1000,  
	 1000,  
	 1000,  
	 1000,  
	 1000,  
	1000,
};
//У��ֵ  ����ad�Ƚ�ֵ  У��ֵ�ͱ�׼ֵ�ı���ϵ��
const float CalibrateAdLimit[2]=
{
	 1.2, 0.8,

};
const uint16_t CAL_VALUE_DA[]={4000,400,400,400,400,400,400,400,400};
const vu16 Hole_Ratio[]={1,10,100,1000};


//��˾У��
void Fac_DebugProcess(void)
{
	vu32 keynum=0;
	vu8 key;
 	while(GetSystemStatus()==SYS_STATUS_FACRDEBUG)
	{
		key=HW_KeyScsn();
		if(key==0xff)
		{
			keynum=0;
		}
		else
			keynum++;
		if(keynum==2)
		{
			switch(key)
			{
				case Key_F1:
				break;
				case Key_F2:
				break;
				case Key_F3:
				break;
				case Key_F4:
				break;
				case Key_F5:
				break;
				case Key_Disp:
				break;
				case Key_SETUP:
				break;
				case Key_FAST:
				break;
				case Key_LEFT:
				break;
				case Key_RIGHT:
				break;
				case Key_UP:
				break;
				case Key_DOWN:
				break;
				case Key_NUM1:
				break;
				case Key_NUM2:
				break;
				case Key_NUM3:
				break;
				case Key_NUM4:
				break;
				case Key_NUM5:
				break;
				case Key_NUM6:
				break;
				case Key_NUM7:
				break;
				case Key_NUM8:
				break;
				case Key_NUM9:
				break;
				case Key_NUM0:
				break;
				case Key_DOT:
				break;
				case Key_BACK:
				break;
				
				default:
				break;
					
			}
		
		
		}
	
	
	}
}	
//==========================================================
//�������ƣ�Uart_Process
//�������ܣ����ڴ���
//��ڲ�������
//���ڲ�������
//�������ڣ�2015.10.26 
//�޸����ڣ�2015.10.26 10:02
//��ע˵������
//==========================================================
//u8 Uart_Process(void)
//{
//	vu8 i;
//#if HW_UART_SUPPORT
//	u8 kind=0xff;
//	u8 data=0;
//	u8 str[(FRAME_LEN_MAX-FRAME_LEN_MIN)+1];//�շ����ݻ���
////	if(SaveData.Sys_Setup.Bus_Mode==0)//������Ч
//	{
//		if (ComBuf.rec.end)//�������ݽ���
//		{data=1;
//			//memset(str,'\0',(FRAME_LEN_MAX-FRAME_LEN_MIN+1));//��ջ���
//			{
//				//memcpy(str,&ComBuf.rec.buf[PDATASTART-1],ComBuf.send.len-FRAME_LEN_MIN);//���ݰ�
//				kind=ComBuf.rec.buf[PFRAMEKIND];//������
//                switch(kind)
//                {
//                    case 0xfa:
////                        if(GetSystemStatus()!=SYS_STATUS_TEST)
////                            SetSystemStatus(SYS_STATUS_TEST);
//                        break;
//                    case 0xfb:
//                       // Tft_5520_Dispvalue.comp=0x93;
//                        break;
//                    case 0:
//                        Tft_5520_Dispvalue.Range=ComBuf.rec.buf[2];
//                        Tft_5520_Dispvalue.Test_R_Dot=ComBuf.rec.buf[3];
//                        Tft_5520_Dispvalue.comp=ComBuf.rec.buf[4];
//                        Tft_5520_Dispvalue.Test_R=ComBuf.rec.buf[6];
//                        Tft_5520_Dispvalue.Test_R<<=8;
//                        Tft_5520_Dispvalue.Test_R+=ComBuf.rec.buf[5];
//                        
//                        Tft_5520_Dispvalue.Test_V=ComBuf.rec.buf[8];
//                        Tft_5520_Dispvalue.Test_V<<=8;
//                        Tft_5520_Dispvalue.Test_V+=ComBuf.rec.buf[7];
//                        
//                        Tft_5520_Dispvalue.Test_T=ComBuf.rec.buf[10];
//                        Tft_5520_Dispvalue.Test_T<<=8;
//                        Tft_5520_Dispvalue.Test_T+=ComBuf.rec.buf[9];    
//                    break;
//                    case 1://
//                        break;
//                    case 2://
//                        break;
//                    case 3://
//                        break;
//                        
//            
//                }

//			}
//            
//			//׼��������һ֡����sprintf
//			ComBuf.rec.end=0;//���ջ���ɶ���־��λ
//			ComBuf.rec.ptr=0;//����ָ������
//		}
//	}
////	WriteString_Big(0, 150, (uint8_t *)&ComBuf.rec.buf[1]);
//	ComBuf.rec.end=0;

////	switch(kind)
////	{
////		case FRAME_READ_RESULT://��ȡ���
////			//���ڷ��Ͳ�������:��ѹ(5)+����(6)+ʱ��(4)+��ѡ(1)=16�ֽ�
////			switch (GetSystemMessage())//ϵͳ��Ϣ
////			{
////				case MSG_ABORT:
////					kind=0x9B;//������ֹ
////					break;
////				case MSG_PASS:
////					kind=0x91;//����ͨ��
////					break;
////				case MSG_HIGH:
////					kind=0x92;//���ޱ���
////					break;
////				case MSG_LOW:
////					kind=0x92;//���ޱ���
////					break;
////				default:
////					kind=0x90;//��������
////					break;
////			}		
////			ComBuf.send.buf[1+5+6+4]=kind;
////			ComBuf.send.begin=0;
////			ComBuf.send.len=PackStandFrame(ComBuf.send.buf , &ComBuf.send.buf[1] , 16  );
//////			if(SendDataToCom()==0)//���ͳɹ��б�
//////			{
//////			//	Delay_1ms(100);//��ʱ
//////			//	SendDataToCom();//����
//////			}
////			break;
////		
////		case FRAME_START://����
////			SetSystemStatus(SYS_STATUS_TEST);//ϵͳ״̬-��������
////			break;

////		case FRAME_RESET://��λ
////			//SetSystemStatus(SYS_STATUS_IDLE);//ϵͳ״̬-����
////			break;

////		case FRAME_WRITE_SN://д���к�
////			break;
////		
////		case FRAME_CLR_BOOT_NUM://�忪������
////			break;
////		
////		case FRAME_DATA://����֡
////			break;

////		default:
////			break;
////	}
//	return data;
//#endif
//}

////ȫ�ֱ���
//u8 WaitRecTimeOver;

//==========================================================
//�������ƣ�PackStandFrame
//�������ܣ������ݴ����֡
//��ڲ�����*framebuf:֡���ݻ���
//			*datbuf:���ݻ���
//			len:���ݳ���
//���ڲ�����һ֡���ݳ���
//�������ڣ�2014.04.11
//�޸����ڣ�2014.04.11 10:28
//��ע˵�����ɵķ���Э��
//��ʼ(0xAB)����ѹ(5) ����(6) ʱ��(4)����ѡ(1)������(0xAF)=18�ֽ�
//==========================================================
int8_t PackStandFrame(int8_t * framebuf, int8_t * datbuf, int8_t len)
{
	if(len>(SEND_LEN_MAX-2))//���ݵ���󳤶�
		len=(SEND_LEN_MAX-2);
	framebuf[0]=UART_SEND_BEGIN;//֡ͷ
	memcpy(&framebuf[1], datbuf, len);//����
	framebuf[len+1]=UART_SEND_END;//֡β
	return (len+1);//����һ֡���ݳ���
}

const u32 UNIT_VALUE[15]=
{
	1,1E3,1E6

};
Sort_TypeDef Input_compvalue(Disp_Coordinates_Typedef *Coordinates)
{
	u8 key;
	u8 page=0;
	u8 disp_flag=1;
//	u8 index=0;
	u8 disp_cound=0;
	u8 disp_pow=0;
//	u8 input_flag=0;
//	u32 unit_c;
	u8 dot=5;//С����
	u8 dot_num=0;
	vu8 While_flag=1;
	vu32 keynum=0;
	vu8 Disp_buff[10];
	vu8 key_count;
	vu8 dot_num1;
	vu8 del_flag=0;
	Sort_TypeDef   Sort_set;
	
//	u8 unit=0;//��λ
	u32 Word;
	
	float conp_value=0;
//	float old_value;
	key_count=0;
//	old_value=SaveData.SetParameter.Nominal_value.comp_value;
//	if(SaveData.SysParameter.language==0)
//		Disp_Inputback("������ֵ");//����ɫ
//	else
//		Disp_Inputback("INPUTNUM");//����ɫ
	LCD_DrawRect( Coordinates->xpos, Coordinates->ypos,Coordinates->xpos+Coordinates->lenth , 
	Coordinates->ypos+16 , Red );
	while(While_flag)
	{
		key=HW_KeyScsn();
		if(key==0xff)
		{
			keynum=0;
		}
		else
			keynum++;
		if(keynum==KEY_NUM)
		{
			disp_flag=1;
			switch(key)
			{
				case Key_F1:
					//unit_c=UNIT_VALUE[DispBuf[disp_cound]-'0'];
						conp_value*=pow(10,5-(disp_cound-dot_num));
						//Sort_set.Num=	conp_value *unit_c/pow(10,dot);
						Sort_set.Num=conp_value;
						Sort_set.Dot=dot;
						if(page==0)
							Sort_set.Unit=0;
						else
							Sort_set.Unit=4;
							
						While_flag=0;
					break;
				case Key_F2:
						conp_value*=pow(10,5-(disp_cound-dot_num));
						//Sort_set.Num=	conp_value *unit_c/pow(10,dot);
						Sort_set.Num=conp_value;
						Sort_set.Dot=dot;
						if(page==0)
							Sort_set.Unit=1;
						else
							Sort_set.Unit=5;
						While_flag=0;
					break;
				case Key_F3:
					conp_value*=pow(10,5-(disp_cound-dot_num));
						//Sort_set.Num=	conp_value *unit_c/pow(10,dot);
						Sort_set.Num=conp_value;
						Sort_set.Dot=dot;
						if(page==0)
							Sort_set.Unit=2;
						else
							Sort_set.Unit=6;
						While_flag=0;
					break;
				case Key_F4:
						if(page==0)
						{
						conp_value*=pow(10,5-(disp_cound-dot_num));
						//Sort_set.Num=	conp_value *unit_c/pow(10,dot);
						Sort_set.Num=conp_value;
						Sort_set.Dot=dot;
						Sort_set.Unit=3;
						While_flag=0;
						}
					break;
				case Key_F5:
					if(page)
						page=0;
					else
						page=1;
				
					break;
				case Key_Disp:
					SetSystemStatus(SYS_STATUS_TEST);
					While_flag=0;
					Sort_set.Updata_flag=0;
				break;
				case Key_SETUP:
					While_flag=0;
					Sort_set.Updata_flag=0;
				SetSystemStatus(SYS_STATUS_SETUPTEST);
				break;
				case Key_FAST:
				break;
				case Key_LEFT:
				break;
				case Key_RIGHT:
				break;
				case Key_UP:
				break;
				case Key_DOWN:
				break;
				case Key_NUM1:
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='1';
						key_count++;
						Word=Disp_buff[disp_cound]-'0';
						conp_value=conp_value*10+Word;
						disp_pow++;
						disp_cound++;
						if(dot_num==0)
							dot--;
							
					}
				break;
				case Key_NUM2:
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='2';
						key_count++;
						Word=Disp_buff[disp_cound]-'0';
						conp_value=conp_value*10+Word;
						disp_pow++;
						disp_cound++;
						if(dot_num==0)
							dot--;
					}
				break;
				case Key_NUM3:
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='3';
						key_count++;
						Word=Disp_buff[disp_cound]-'0';
						conp_value=conp_value*10+Word;
						disp_pow++;
						disp_cound++;
						if(dot_num==0)
							dot--;
					}
				break;
				case Key_NUM4:
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='4';
						key_count++;
						Word=Disp_buff[disp_cound]-'0';
						conp_value=conp_value*10+Word;
						disp_pow++;
						disp_cound++;
						if(dot_num==0)
							dot--;
					}
					
				break;
				case Key_NUM5:
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='5';
						key_count++;
						Word=Disp_buff[disp_cound]-'0';
						conp_value=conp_value*10+Word;
						disp_pow++;
						disp_cound++;
						if(dot_num==0)
							dot--;
					}
				break;
				case Key_NUM6:
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='6';
						key_count++;
						Word=Disp_buff[disp_cound]-'0';
						conp_value=conp_value*10+Word;
						disp_pow++;
						disp_cound++;
						if(dot_num==0)
							dot--;
					}
				break;
				case Key_NUM7:
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='7';
						
						Word=Disp_buff[disp_cound]-'0';
						key_count++;
						conp_value=conp_value*10+Word;
						disp_pow++;
						disp_cound++;
						if(dot_num==0)
							dot--;
					}
				break;
				case Key_NUM8:
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='8';
						key_count++;
						Word=Disp_buff[disp_cound]-'0';
						conp_value=conp_value*10+Word;
						disp_pow++;
						disp_cound++;
						if(dot_num==0)
							dot--;
					}
				break;
				case Key_NUM9:
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='9';
						key_count++;
						Word=Disp_buff[disp_cound]-'0';
						conp_value=conp_value*10+Word;
						disp_pow++;
						disp_cound++;
						if(dot_num==0)
							dot--;
					}
				break;
				case Key_NUM0:
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='0';
						key_count++;
						Word=Disp_buff[disp_cound]-'0';
						conp_value=conp_value*10+Word;
						disp_pow++;
						disp_cound++;
						if(dot_num==0)
							dot--;
					}
				break;
				case Key_DOT:
					if(dot_num==0)
					{
						dot_num1=key_count;
						Disp_buff[key_count]='.';
						dot_num=1;
						key_count++;
						disp_cound++;
					}
					break;
				case Key_BACK:
					if(key_count>0)
					{	key_count--;
						Disp_buff[key_count]=' ';
						if(dot_num1==key_count)
						{
							dot_num=0;
							dot_num1=0;
							
						}
						del_flag=1;
					
					}
//					else
//					{
//						if(Disp_buff[key_count]==0)
//								Disp_buff[key_count]='-';
//							else if(Disp_buff[key_count]=='-')
//								Disp_buff[key_count]='+';
//							else
//								Disp_buff[key_count]='-';
//							key_count++;
//					}
				break;
				
				
				
				default:
				break;
					
			}
			if(disp_flag)
			{
				disp_flag=0;
				Disp_button_Num_Input(page);
				Colour.Fword=White;
				Colour.black=Red;
				if(del_flag)
				{
					PutChar( Coordinates->xpos+(key_count+1)*10, Coordinates->ypos, 
					Disp_buff[key_count], Colour.Fword, Colour.black ); 
					del_flag=0;
					
				}
				else if(key_count>0)				
					PutChar( Coordinates->xpos+key_count*10, Coordinates->ypos, 
					Disp_buff[key_count-1], Colour.Fword, Colour.black ); 
			
			}
				
				
			
			
		}
			
	}
		
		
	return Sort_set;
}
	
	


//��������
Sort_TypeDef Disp_Set_Num(Disp_Coordinates_Typedef *Coordinates)
{
	Sort_TypeDef Sort_num,Sort_num1;
	Disp_button_Num_time();
	Sort_num=Disp_NumKeyboard_Set(Coordinates);
	Sort_num1=Time_Set_Cov(&Sort_num);
	if(Sort_num1.Updata_flag==0)
	{
		Sort_num1.Dot=0;
		Sort_num1.Num=0;
		Sort_num1.Unit=0;
	
	}
		
	return Sort_num1;

}
//��ѹ����
Sort_TypeDef Disp_Set_CompNum(Disp_Coordinates_Typedef *Coordinates)
{
	Sort_TypeDef Sort_num,Sort_num1;
	Disp_button_Num_Freq();
	Sort_num=Disp_NumKeyboard_Set(Coordinates);
	Sort_num1=Input_Set_Cov(&Sort_num);
	if(Sort_num1.Updata_flag==0)
	{
		Sort_num1.Dot=0;
		Sort_num1.Num=0;
		Sort_num1.Unit=0;
	
	}
		
	return Sort_num1;

}
Sort_TypeDef Disp_Set_InputNum(Disp_Coordinates_Typedef *Coordinates)
{
	Sort_TypeDef Sort_num1;
	Disp_button_Num_Input(0);
	Sort_num1=Input_compvalue(Coordinates);
	//if(SaveData.Limit_Tab.Mode==0)
	Sort_num1=Input_Set_Cov(&Sort_num1);
	//else
	//Sort_num1=Input_Set_CovPre(&Sort_num1);
	if(Sort_num1.Updata_flag==0)
	{
		Sort_num1.Dot=0;
		Sort_num1.Num=0;
		Sort_num1.Unit=0;
	
	}
		
	return Sort_num1;
	
	

}


vu16 Freq_Set_Num(Disp_Coordinates_Typedef *Coordinates)//Ƶ������
{
	Sort_TypeDef Sort_num;
//	vu8 i;
	vu16 num;
	Disp_button_Num_Freq();
	Sort_num=Disp_NumKeyboard_Set(Coordinates);
	num= Debug_Value(&Sort_num);
	//Sort_num1=Freq_Set_Cov(&Sort_num);
	
	
		
	return num;
	
}
vu8 Avg_Set_Num(Disp_Coordinates_Typedef *Coordinates)//ƽ��������
{
	Sort_TypeDef Sort_num,Sort_num1;
	vu8 i;
	vu8 num;
	Disp_button_Num_Avg();
	Sort_num=Disp_NumKeyboard_Set(Coordinates);
	Sort_num1=Freq_Set_Cov(&Sort_num);
	if(Sort_num1.Updata_flag==0)
	{
		Sort_num1.Dot=0;
		Sort_num1.Num=0;
		Sort_num1.Unit=0;
	
	}
	if(Sort_num1.Dot==0)
	{
		if(Sort_num1.Num>32)
			Sort_num1.Num=32;
	
	} else
	if(Sort_num1.Dot==1)
	{
		for(i=0;i<5;i++)
		{
			if(Sort_num1.Num>0&&Sort_num1.Num<=9)
				break;
			else
				Sort_num1.Num/=10;
		
		
		}
	
	}else if(Sort_num1.Dot==2)
	{
		for(i=0;i<5;i++)
		{
			if(Sort_num1.Num>10&&Sort_num1.Num<=99)
				break;
			else
				Sort_num1.Num/=10;
		
		
		}
		if(Sort_num1.Num>32)
			Sort_num1.Num=32;
	
	
	
	}else
	{
		Sort_num1.Num=32;
	
	
	}
	num=Sort_num1.Num;
	if(num==0)
		num=1;
		
	return num;
	
}
void Set_daot(vu8 *buff,vu8 dot)
{
	vu8 i;
	for(i=0;i<dot;i++)
	{
		buff[5-i]=buff[5-i-1];
	
	
	}
	if(dot==0)
		buff[5]=' ';
	else
	buff[5-dot]='.';
	buff[6]=0;


}
int8_t V_Test_Comp(int32_t value)
{
	vu8 data;
	
	if(value>Save_Res.Set_Data.V_high.mid_data)
		data=VH_FAIL;
	else if(value<Save_Res.Set_Data.V_low.mid_data)
		data=VL_FAIL;
	else
		data=ALL_PASS;
	return data;
		


}
int8_t R_Test_Comp(int32_t value)
{
	vu8 data;
	
	if(value>Save_Res.Set_Data.High_Res.mid_data)
		data=RH_FAIL;
	else if(value<Save_Res.Set_Data.Res_low.mid_data)
		data=RL_FAIL;
	else
		data=ALL_PASS;
	return data;

}
void Comp_prompt(int8_t value)
{
	if(value==ALL_PASS)
	{
		Pass_Led();
	
	}
	else
	{
		Fail_led();
	
	}
	switch(Save_Res.Set_Data.beep)
	{
		case 0://�������ر�
			Beep_Off();
			break;
		case 1://�ϸ�Ѷ��
			if(value==ALL_PASS)
				Beep_on();
			else
				Beep_Off();
			break;
		case 2://���ϸ�Ѷ��
			if(value==ALL_PASS)
				Beep_Off();
			else
				Beep_on();
				
			
			break;
		default:
			Beep_Off();
			break;
	
	}



}

float Inttofloat(Sort_TypeDef *pt)//intת��Ϊfloat  INT����С����͵�λ
{
	float value;
//	vu8 i;
	value=pt->Num;
	value=value*pow(1000,pt->Unit);
	value/=pow(10,pt->Dot);
	return value;
}

int32_t Input_int(int8_t *pt)
{
	u32 value=0;
	u8 i,j=0;
//	u8 dot=0;
	for(i=0;i<5;i++)
	{
		if(*(pt+i+1)>='0')
		{
			value*=10;
			value+=*(pt+i+1)-'0';
			j++;
			
		}
//		else
//			dot=4-i;
	
	}
	return value;


}
//����BCDת��ΪINT
int32_t BCDtoInt(int8_t *pt)
{
	u32 value=0;
	u8 i,j=0;
	u8 dot=0;
	if(*(pt+1)=='-')
	{
		value=0xfffffff;
		return value;
	}
	for(i=0;i<5;i++)
	{
		if(*(pt+1+i)>='0')
		{
			value*=10;
			value+=*(pt+1+i)-'0';
			j++;
			
		}
		else
			dot=4-i;
	
	}

	value/=pow(10,dot);
	
	return value;
}
//��ѹBCDת��ΪINT
int32_t VBCDtoInt(int8_t *pt)
{
	u32 value=0;
	
	u8 i,j=0;
	u8 dot=0;
	for(i=0;i<5;i++)
	{
		if(*(pt+1+i)>='0')
		{
			value*=10;
			value+=*(pt+1+i)-'0';
			j++;
			
		}
		else
			dot=4-i;
		
	
	}
	Test_Unit.V_dot=dot;
	value*=1000;
	value/=pow(10,dot);
	if(*pt=='-')
		Test_Unit.V_Neg=0;
	else
		Test_Unit.V_Neg=1;
	
	return value;
}
void V_BCD_Int(int32_t data)
{
	u8 i;
	vu16 word;
	if(data>60e3)
	{
		for(i=0;i<5;i++)
		DispBuf[i]='-';
	}
	else 
		if(data>6e3)
		{
			word=data/10;
			Hex_Format((word),2,4,FALSE);
		
		
		}
		else
			Hex_Format((data),3,4,FALSE);
			


}
void BCD_Int(int32_t data)
{
	u8 i;
	vu16 word;
	if(data>30e6)
	{
		Test_Unit.Res_dot=1;
		for(i=0;i<5;i++)
		DispBuf[i]='-';
	
	}else
		if(data>=3e6)//xx.xx
		{
			Test_Unit.Res_dot=1;
			word=data/10e3;
			Hex_Format((word),2,4,FALSE);
				
		}else
			if(data>=3e5)//x.xxx
			{
				Test_Unit.Res_dot=1;
				word=data/10e2;
				Hex_Format((word),3,4,FALSE);
			}
			else
				if(data>=3e4)//xxx.xm
				{
					Test_Unit.Res_dot=0;
					word=data/10e1;
					Hex_Format((word),1,4,FALSE);
				}else
				if(data>=3e3)
				{
					Test_Unit.Res_dot=0;
					word=data/10;
					Hex_Format((word),2,4,FALSE);
				}
				else
				{
					Test_Unit.Res_dot=0;
					word=data/10;
					Hex_Format((word),2,4,FALSE);
				
				
				}


}
float Debug_Res(float a,float b,float c)
{
	//float d;
	return a*b/c;
}
void  Write_Usbdata (volatile uint8_t  *buffer,uint32_t num)
{
    int32_t  fdw;
//    uint32_t  bytes_written;
        fdw = FILE_Open(FILENAME_W, RDWR);
        if (fdw > 0) 
		{
			usb_oenflag=1;

			//bytes_written = FILE_Write(fdw, buffer, num);//MAX_BUFFER_SIZE);
            FILE_Write(fdw, buffer, num);

			FILE_Close(fdw);
            
        } 
		else
			usb_oenflag=2;

    } 

void  Main_Read (void)
{
    int32_t  fdr,i;
//    uint32_t  bytes_read;
    

    fdr = FILE_Open("data.txt", RDONLY);
    if (fdr > 0) {
        //PRINT_Log("Reading from %s...\n", FILENAME_R);
//        do {
           // bytes_read = FILE_Read(fdr, UserBuffer, MAX_BUFFER_SIZE);
        FILE_Read(fdr, UserBuffer, MAX_BUFFER_SIZE);
 //       } while (bytes_read);

        FILE_Close(fdr);
//        PRINT_Log("Read Complete\n");
		for(i=0;i<7;i++)
		{
			Save_Res.Production_Data.buff[i]=*(UserBuffer+i);
			Save_Res.Number.buff[i]=*(UserBuffer+8+i);
			
		}
		Save_Res.Production_Data.flag=0x55;
		Save_Res.Number.flag=0x55;
		Savetoeeprom();
    } 

}
void Debug_jisuan(void)
{
	uint32_t testvalue;
	Tft_5520.Calibrate.IRVol.NumAd=1;
	testvalue=rebuff_v[0]>>21;
    if(debug)
        testvalue=testvalue*500/800;
    else
        testvalue=testvalue*500/800/Tft_5520.Calibrate.IRVol.NumAd;
        Tft_5520_Dispvalue.Test_V=testvalue;

}
//float[-26830,]

float Fft_Dci(Fft_Cov_Typedef fft)
{
    u8 i,j;
    float value=0;
    float temp=0;
    if(fft.num==0)
        ;
    else if(fft.num==1)
        value=fft.fftbuff[0];
    else
        if(fft.num<FFTLENTH-2)
        {
            for(i=1;i<fft.num;i++)
            {
                temp+=fft.fftbuff[i];
            
            
            }
            value=temp/(fft.num-1);
        
        }

    else
    {
        for(j=0;j<(fft.num-1);j++)
        {
        
            for(i=0;i<(fft.num-j);i++)
            {
                if(fft.fftbuff[i]>fft.fftbuff[i+1])
                {
                    temp=fft.fftbuff[i];
                    fft.fftbuff[i]=fft.fftbuff[i+1];
                    fft.fftbuff[i]=temp;
                
                }
            
            }
        
        }
        for(i=2;i<fft.num-2;i++)
        {
            value+=fft.fftbuff[i];
                
        }
        value/=(fft.num-4);
    
    }
    return value;

}

void Getberore_Res(void)
{
//    vu8 i;
    float jisuan;
	
//	jisuan=fabs((Res_count.i-Tft_5520.Clear_buff[Range+VRange_flag]));
//	jisuan=Res_count.i;
        
    if(jisuan==0)
        jisuan=1;
//	Res_count.before_r=(Res_count.v)*Res_count.rate/jisuan/7.322;

}
void Change_Disp_beforer(float f)   //���ٻ���ʱ��
{
    const vu16 Hole_Ratio[]={1,10,100,1000}; 
    Tft_5520_Dispvalue.Test_R=  f/Hole_Ratio[Range];    
}
void Change_Disp_r(float f)   //�̶���λ
{
    const vu16 Hole_Ratio[]={1,10,100,1000}; 
	static u16 R_compvalue;
    vu8 auto_=1;

    if(auto_)//�̶�����
    {
		
        Tft_5520_Dispvalue.Test_R=  f/Hole_Ratio[Range]+0.5;
        Tft_5520_Dispvalue.Test_R_Dot=RANGE_UNIT[Range];
    }
    else//�Զ�����
    {
        if(f<=4000)
        {
            Tft_5520_Dispvalue.Test_R_Dot=3;
            Tft_5520_Dispvalue.Test_R=f+0.5;
        
        }
        else
            if(f<=40000)
            {
				Tft_5520_Dispvalue.Test_R_Dot=2;
				Tft_5520_Dispvalue.Test_R=f/10+0.5;
            
            
            }
            else
                if(f<=400000)
                {
                    Tft_5520_Dispvalue.Test_R_Dot=1;
					Tft_5520_Dispvalue.Test_R=f/100+0.5;
                
                }
                else
                    if(f<1000000)
                    {
                        Tft_5520_Dispvalue.Test_R_Dot=0;
						Tft_5520_Dispvalue.Test_R=f/1000+0.5;
                    
                    }
                    else//����10G��ʾOVER
                    {
                    
                    }
    
    
		}
		if(Tft_5520_Dispvalue.Test_R>R_compvalue)
		{
			if(Tft_5520_Dispvalue.Test_R-R_compvalue>3)
			{
				R_compvalue=Tft_5520_Dispvalue.Test_R;
			}
			else
			{
				Tft_5520_Dispvalue.Test_R=R_compvalue;
			
			}
		
		}else
		{
			if(R_compvalue-Tft_5520_Dispvalue.Test_R>3)
			{
				R_compvalue=Tft_5520_Dispvalue.Test_R;
			}
			else
			{
				Tft_5520_Dispvalue.Test_R=R_compvalue;
			
			}
		
		
		
		}
		

}

//==========================================================
//�������ƣ�Range_Changecomp
//�������ܣ�������Ի���
//��ڲ�������
//���ڲ�������
//�������ڣ�2015.10.26 
//�޸����ڣ�2015.10.29 13:35
//��ע˵������
//==========================================================

void Test_cleardata(void)
{
    
    u8 key;
    u8 i=0;
    u8 range;
    Clear_flag=0;
    Tft_5520_Dispvalue.Clear_Time=0;
    AD_7192_Rest(0);
    AD_7192_Rest(1);
    Delay(10);
//    AD_7192Config(0);
    step=V_SAMP;
//    step=I_SAMP;
    U_step=V_HSHORT;
    Delay(1);
    SPI_I_CS_HIGH();
    SPI_U_CS_LOW();
    Delay(1);
     SPI_ADDA_SendByte(0x28);  //�˿ڿ��ƼĴ���     
    SPI_ADDA_SendByte(0x33);  
    SPI_ADDA_SendByte(0x28);  //�˿ڿ��ƼĴ���     
    SPI_ADDA_SendByte(0x34);
    
    SPI_ADDA_SendByte(0x08);//ģʽ�Ĵ���
    SPI_ADDA_SendByte(0x38);  //ģʽ�ڲ�����
    SPI_ADDA_SendByte(0x8c); //04
    SPI_ADDA_SendByte(16); 
    
    SPI_ADDA_SendByte(0x10);//���üĴ���
    SPI_ADDA_SendByte(0);  //
    SPI_ADDA_SendByte(0x80); 
    SPI_ADDA_SendByte(0x08);
    Delay(1);
    
    range=Tft_5520.Set_Value[Tft_5520.Group].Range;
    Range=0;
    VRange_flag=0;
    Clear_num=0;
//    SetSystemMessage(MSG_CLEAR);
//    LcdAddr.x=MSG_ADDR_X; LcdAddr.y=0;
//	Lcd_SetAddr();//��������
//	Disp_SysMessage();//��ʾ��Ϣ
	debug=1;
	//Disp_Teststatue(9);
	Disp_big_HZ(GetSystemMessage());
    while(GetSystemStatus()==SYS_STATUS_CLEAR)
    {
		Disp_Range(0);
//		Hex_Format(fabs(Res_count.i),0,5,0);
//		WriteString_Big(DISP_R_X+BIG_HZ_W,DISP_R_Y ,DispBuf);//����

        if(Clear_num>40)
        {
            Clear_num=0;
//            Tft_5520.Clear_buff[i]=Res_count.i;
            i++;
            Range++;
            Clear_flag=1;
            if(i>3)
            {
                SetSystemStatus(SYS_STATUS_IDEM);
				SetSystemMessage(MSG_IDLE);
                Tft_5520.Set_Value[Tft_5520.Group].Range=range;
				Savetoeeprom();
                //Store_set_flash();
                
            }
            
        }
//        key=Key_Read();//��ȡ����
        switch(key)
		{
//			case KEY_SET:	//���ü�
//			case L_KEY_SET:	//�������ü�
//				//SetSystemStatus(SYS_STATUS_SETUP);//ϵͳ״̬-��ͣ����
//				break;
//	
//			case KEY_UP:	//�ϼ�
//			case L_KEY_UP:	//�����ϼ�
////				if(SaveData.Parameter.Range>RANGE_MAX)
////				SaveData.Parameter.Range=0;
////				else
////				SaveData.Parameter.Range++;
////				if(SaveData.Parameter.Range==0)
////					Range=1;
////				else
////				Range=SaveData.Parameter.Range-1;
////				//Disp_Char(SaveData.Parameter.Range+'0');
////				Range_Control(Range);
//				
//				break;
//	
//			case KEY_DOWN:		//�¼�
//			case L_KEY_DOWN:	//�����¼�
////				if(SaveData.Parameter.Range==0)
////				SaveData.Parameter.Range=RANGE_MAX;
////				else
////				SaveData.Parameter.Range--;

////				if(SaveData.Parameter.Range==0)
////					Range=1;
////				else
////				Range=SaveData.Parameter.Range-1;
////				//Disp_Char(SaveData.Parameter.Range+'0');
////				Range_Control(Range);
//				break;
//	
//			case KEY_ENTER:		//ȷ�ϼ�
//				break;
//			case L_KEY_ENTER:	//����ȷ�ϼ�
//				//clear_flag=TRUE;
////					zreo_num[Range]=Res.Real;
//				//SaveData.Parameter.Range=0;
//				//Res.Real=zreo_num[Range];
//				//����
//				break;

//			case KEY_START:		//������
//			case L_KEY_START:	//����������
//				break;

//			case KEY_RESET:		//��λ��
//			case L_KEY_RESET:	//������λ��
////				SetSystemStatus(SYS_STATUS_TEST_ABORT);//ϵͳ״̬-��ͣ����
////				SetSystemMessage(MSG_ABORT);//ϵͳ��Ϣ-������ֹ
//				break;
			
			default:
				break;
		}
    }
	debug=0;
//    SaveData.System.clear=1;


}
//==========================================================
//�������ƣ�Start_Process
//�������ܣ��������Դ���
//��ڲ�������
//���ڲ�������
//�������ڣ�2015.01.26
//�޸����ڣ�2015.08.22 15:42
//��ע˵������
//ע�������
//==========================================================
void Start_Process(void)
{
	while(GetSystemStatus()==SYS_STATUS_START)
	{
		//ȫ�ֱ�������
		StepIndex=0;//��һ����ʼ
		TotalStep=0;//�����ܲ���
		F_Fail=FALSE;//�����ʧ�ܱ�־
        V_DA_out(0);//���ź�D/A����ر�
        Sing_out_C(0);//��ֹ���Ҳ�Ƶ�����
        Short_out(0);
        FRB_out(0);
		//״̬����

//		PLC_OutProg();//��PLC����

		//ϵͳ״̬����
        if(U9001_Save_sys.U9001_save.U9001_Setup[1].parameter!=OS)
            SetSoftTimer(TEST_SOFTTIMER,U9001_Save_sys.U9001_Testconfg.delay_time*10);//���ð�����ʱ����(����ʱ������)
				
        while (!GetSoftTimerOut(TEST_SOFTTIMER)) //��ʱδ��
        {
            //��ʾ����ʱ����ӳ�����
            Disp_DelayTime(TEST_SOFTTIMER);
            
            if(READ_STOP()==0)
            {               
                SetSystemMessage(MSG_PAUSE);//ϵͳ��Ϣ-��ͣ����
                SetSystemStatus(SYS_STATUS_ABORT);//ϵͳ״̬-��ͣ����
                Uart0_Send(0xa0);
                return;
            }
        }
		SetSystemMessage(MSG_RAMP);//ϵͳ��Ϣ-����
		SetSystemStatus(SYS_STATUS_TEST);//ϵͳ״̬-����
	}
}
//==========================================================
//�������ƣ�TestPause_Process
//�������ܣ�������ͣ����
//��ڲ�������
//���ڲ�������
//�������ڣ�2015.01.25 
//�޸����ڣ�2015.09.06 13:28
//��ע˵������
//==========================================================
void TestPause_Process(void)
{
	u8 i;
	u8 key;
    u8 pass_flag=0;
	u8 item;
	u16 time;//������ʱʱ��
	Bool f_disp;//��ʾ��־

	f_disp=FALSE;//��ʾ��־
    ARC_out(0);
    if(U9001_Save_sys.U9001_save.disp)//��ʾ��ѡ���
        Disp_Comp();
//    else
//		Disp_CompTest();
                //Disp_Test_List(U9001_Save_sys.U9001_save.current_step);
    if(GetSystemMessage()==MSG_PASS)
        SetSoftTimer(PASS_SOFTTIMER,U9001_Save_sys.U9001_Testconfg.pass_time*10);//���ð�����ʱ����(����ʱ������)
	if(osgetflag == 1)
	{
		SetSystemStatus(SYS_STATUS_SETUPTEST);//�ص����ý���
		osgetflag = 0;
		V_DA_out(0);
		Sing_out_C(0);
		Short_out(0);
		FRB_out(0);
		Uart0_Send(0xa0);
		return;
	}else{
		if(U9001_Save_sys.U9001_Testconfg.comp_setp == 0)
		{
			if(U9001_Save_sys.U9001_save.current_step<U9001_Save_sys.U9001_save.all_step)
			{
				switch(U9001_Save_sys.U9001_Testconfg.fail_mode)
				{
					case 0:
						if(F_Fail == TRUE)
						{
							Uart0_Send(0xa5);//FAIL��
							if(U9001_Save_sys.U9001_SYS.fail_beep == 0)
							{
								BeepCon();
							}else if(U9001_Save_sys.U9001_SYS.fail_beep == 1){
								BeepGap();
							}
							SetSystemStatus(SYS_STATUS_FINISH);//���Խ��� 
							
							return;
						}else{
							U9001_Save_sys.U9001_save.current_step++;
							SetSystemMessage(MSG_WAIT);
						}
						break;
					case 1:
						U9001_Save_sys.U9001_save.current_step++;
						SetSystemMessage(MSG_WAIT);
						break;
					case 2:
						SetSystemMessage(MSG_WAIT);
						break;
					case 3:
						U9001_Save_sys.U9001_save.current_step++;
						SetSystemMessage(MSG_WAIT);
						break;
					default:
						break;
				
				}
				
				
			}
			else
			{
				if(clearflag == 1)
				{
					clearflag = 0;
					U9001_Save_sys.U9001_Testconfg.clear = 1;
				}
				if(U9001_Save_sys.U9001_Testconfg.dis_time == 0)
				{
					if(F_Fail == TRUE)
					{
						Uart0_Send(0xa5);//FAIL��
						if(U9001_Save_sys.U9001_SYS.fail_beep == 0)
						{
							BeepCon();
						}else if(U9001_Save_sys.U9001_SYS.fail_beep == 1){
							BeepGap();
						}
					}else{
						Uart0_Send(0xa4);//PASS��
						if(U9001_Save_sys.U9001_SYS.pass_beep == 0)
						{
							BeepCon();
						}else if(U9001_Save_sys.U9001_SYS.pass_beep == 1){
							BeepGap();
						}
					}
					SetSystemStatus(SYS_STATUS_FINISH);//���Խ��� 
					return;
				}else{
					SetSoftTimer(DIS_SOFTTIMER,U9001_Save_sys.U9001_Testconfg.dis_time*10);
					while(!GetSoftTimerOut(DIS_SOFTTIMER))
					{
						Disp_DelayTime(DIS_SOFTTIMER);
					}
					if(F_Fail == TRUE)
					{
						Uart0_Send(0xa5);//FAIL��
						if(U9001_Save_sys.U9001_SYS.fail_beep == 0)
						{
							BeepCon();
						}else if(U9001_Save_sys.U9001_SYS.fail_beep == 1){
							BeepGap();
						}
					}else{
						Uart0_Send(0xa4);//PASS��
						if(U9001_Save_sys.U9001_SYS.pass_beep == 0)
						{
							BeepCon();
						}else if(U9001_Save_sys.U9001_SYS.pass_beep == 1){
							BeepGap();
						}
					}
					SetSystemStatus(SYS_STATUS_FINISH);//���Խ��� 
					return;
				}
				
			}
		}else{
			if(U9001_Save_sys.U9001_save.current_step<U9001_Save_sys.U9001_Testconfg.comp_setp && 
			   U9001_Save_sys.U9001_save.current_step<U9001_Save_sys.U9001_save.all_step)
			{
				switch(U9001_Save_sys.U9001_Testconfg.fail_mode)
				{
					case 0:
						if(F_Fail == TRUE)
						{
							Uart0_Send(0xa5);//FAIL��
							if(U9001_Save_sys.U9001_SYS.fail_beep == 0)
							{
								BeepCon();
							}else if(U9001_Save_sys.U9001_SYS.fail_beep == 1){
								BeepGap();
							}
							SetSystemStatus(SYS_STATUS_FINISH);//���Խ��� 
							return;
						}else{
							U9001_Save_sys.U9001_save.current_step++;
							SetSystemMessage(MSG_WAIT);
						}
						break;
					case 1:
						U9001_Save_sys.U9001_save.current_step++;
						SetSystemMessage(MSG_WAIT);
						break;
					case 2:
						SetSystemMessage(MSG_WAIT);
						break;
					case 3:
						U9001_Save_sys.U9001_save.current_step++;
						SetSystemMessage(MSG_WAIT);
						break;
					default:
						break;
				
				}
				
				
			}
			else
			{
				if(clearflag == 1)
				{
					clearflag = 0;
					U9001_Save_sys.U9001_Testconfg.clear = 1;
				}
				if(U9001_Save_sys.U9001_Testconfg.dis_time == 0)
				{
					if(F_Fail == TRUE)
					{
						Uart0_Send(0xa5);//FAIL��
						if(U9001_Save_sys.U9001_SYS.fail_beep == 0)
						{
							BeepCon();
						}else if(U9001_Save_sys.U9001_SYS.fail_beep == 1){
							BeepGap();
						}
					}else{
						Uart0_Send(0xa4);//PASS��
						if(U9001_Save_sys.U9001_SYS.pass_beep == 0)
						{
							BeepCon();
						}else if(U9001_Save_sys.U9001_SYS.pass_beep == 1){
							BeepGap();
						}
					}
					SetSystemStatus(SYS_STATUS_FINISH);//���Խ��� 
					return;
				}else{
					SetSoftTimer(DIS_SOFTTIMER,U9001_Save_sys.U9001_Testconfg.dis_time*10);
					while(!GetSoftTimerOut(DIS_SOFTTIMER))
					{
						Disp_DelayTime(DIS_SOFTTIMER);
					}
					if(F_Fail == TRUE)
					{
						Uart0_Send(0xa5);//FAIL��
						if(U9001_Save_sys.U9001_SYS.fail_beep == 0)
						{
							BeepCon();
						}else if(U9001_Save_sys.U9001_SYS.fail_beep == 1){
							BeepGap();
						}
					}else{
						Uart0_Send(0xa4);//PASS��
						if(U9001_Save_sys.U9001_SYS.pass_beep == 0)
						{
							BeepCon();
						}else if(U9001_Save_sys.U9001_SYS.pass_beep == 1){
							BeepGap();
						}
					}
					SetSystemStatus(SYS_STATUS_FINISH);//���Խ��� 
					return;
				}
				
			}
		}
	}
    time=U9001_Save_sys.U9001_Testconfg.step_time;

	//������ʱ��������һ���������
	if(GetSystemMessage()==MSG_PAUSE)//��ͣ����
	{
		//��ʾ����
//		LcdAddr.x=TITLE_ADDR_X;	LcdAddr.y=TITLE_ADDR_Y;
//		Disp_Str("������ͣ",0);//��ʾ
//		time=TEST_VALUE_OVER;//����ʱ���ʼ��
        time=U9001_Save_sys.U9001_Testconfg.step_time;
	}

    GUI_DispStringAt(DispBuf,DISP_V_XPOS+44,DISP_V_YPOS+100+23);
    disp_TestMSG(GetSystemMessage());
    //�ж��Ƿ�ϸ� �ϸ��ʱ���ִ���������
    
	while(GetSystemStatus()==SYS_STATUS_TEST_PAUSE)
	{
        if(GetSoftTimerOut(PASS_SOFTTIMER)&&(pass_flag==0))
        {
            pass_flag=1;
            SetSoftTimer(DELAY_SOFTTIMER,U9001_Save_sys.U9001_Testconfg.step_time*10);
        }
        
        if(pass_flag==0)
            Disp_DelayTime(PASS_SOFTTIMER);
//            Hex_Format(Get_SorftTime(PASS_SOFTTIMER),1,4,FALSE);
        else
            Disp_DelayTime(DELAY_SOFTTIMER);
//             Hex_Format(Get_SorftTime(DELAY_SOFTTIMER),1,4,FALSE);
        
        GUI_DispStringAt(DispBuf,DISP_V_XPOS+44,DISP_V_YPOS+100+23);
//		//��ʾ����ʱ�䣬����ʱ

				
       

		if(GetSoftTimerOut(DELAY_SOFTTIMER)&&(pass_flag==1))
		{
			SetSystemStatus(SYS_STATUS_TEST);//ϵͳ״̬-���Կ�ʼ��������һ������
            if(U9001_Save_sys.U9001_save.disp==0)
            {
                GUI_Clear();
                Disp_IdelButton();
                Disp_Idel_Item();
            }
            Uart0_Send(0xa1);
						PLC_OutProg();//��PLC����
		}
        if(READ_STOP()==0)
        {
			Beep_Off();
            V_DA_out(0);
            Sing_out_C(0);
            Short_out(0);
            FRB_out(0);
            SetSystemMessage(MSG_PAUSE);//ϵͳ��Ϣ-��ͣ����
            SetSystemStatus(SYS_STATUS_ABORT);//ϵͳ״̬-��ͣ����
            Uart0_Send(0xa0);
        }

//		Uart_Process();//���ڴ���

	}


}
