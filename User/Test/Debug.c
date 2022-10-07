//==========================================================
//文件名称：Debug.c
//文件描述：调试设置子程序
//文件版本：Ver1.0
//创建日期：2015.02.13
//修改日期：2015.09.01 14:21
//文件作者：黄宏灵
//备注说明：无
//注意事项：无
//==========================================================
#include <stdio.h>
#include <lpc_types.h>
#include <string.h>
#include <stdlib.h>
#include "key/key.h"
#include <cpld.h>
#include "use_disp.h"
#include <cpld.h>
#include "spi_ad_da/spi_da_ad.h"
#include "TypeDefine.h"
#include "GlobalValue.h"
#include "lpc177x_8x_gpio.h"
#include "debug_frmwrk.h"
//#include "Hardware.h"
//#include "Delay.h"
//#include "Interrupt.h"
//#include "Lcd240128.h"
//#include "Disp.h"
//#include "Keyboard.h"
//#include "Beep.h"
//#include "Isp.h"
//#include "Uart0.h"
#include "EX_SDRAM/EX_SDRAM.H"
#include "gui.h"
#include "Test.h"
#include "Debug.h"
extern GUI_CONST_STORAGE GUI_FONT GUI_FontGUI_FONTXBFFON32;
//==========================================================
//调试菜单
const char DebugMenu[][20]=
{
	"1.F1交流耐压",
	"2.F2直流耐压",
	"3.F3绝缘电阻",
	
	"4.F5恢复默认",
};

const char DispModel[2][3][20]=
{
	{
	"9310",
	"9320",
	"9320A",
	},	
	{
	"JK9310",
	"JK9320",
	"JK9320A",
	}
};
//==========================================================
//调试菜单项
enum DebugMenuEnum
{	
	DEBUG_MENU_ACW ,
	DEBUG_MENU_DCW ,
	DEBUG_MENU_IR ,
	DEBUG_MENU_GND ,
	DEBUG_MENU_DEFAULT ,
};

//==========================================================
//函数名称：Debug_Process
//函数功能：调试设置处理
//入口参数：无
//出口参数：无
//创建日期：2015.02.13
//修改日期：2015.08.31 10:29
//备注说明：无
//==========================================================
void Debug_Process(void)
{
	u8 i;
	u8 key;
	bool F_Disp;//显示更新标志

	F_Disp=TRUE;//显示更新
    Short_out(1);//short中断比较设置
    GFI_out();//GFI中断比较设置
    ARC_out(1);//ARC中断比较设置
	MenuIndex=DEBUG_MENU_ACW;//取菜单项
    GUI_SetBkColor(LCD_COLOR_TEST_BACK);//GUI_DARKBLUE
	GUI_SetColor(GUI_WHITE);
	GUI_Clear();
    GUI_SetFont(&GUI_FontGUI_FONTXBFFON32);//
	while(SystemStatus==SYS_STATUS_USERDEBUG)
	{
		if(F_Disp)
		{
			F_Disp=FALSE;//显示更新

            GUI_SetColor(GUI_WHITE);
            GUI_DispStringAt("校正界面",176,4);
			for(i=0;i<sizeof(DebugMenu)/sizeof(DebugMenu[0]);i++)//显示列表
			{
                GUI_DispStringAt(DebugMenu[i],120,40+40*i);
			}
			GUI_DispStringAt(DispModel[U9001_Save_sys.jkflag][SOFTWARE_VERSION-1],120,40+40*4);
		}

		if(keyvalue!=KEY_NONE)
		{	
            DISP_FLAG=TRUE;
            switch(keyvalue)
            {
                case Key_F1:
                    GUI_Clear();
                    MenuIndex=DEBUG_MENU_ACW;
                    Acw_Calibrate();//交流耐压校准处理
                    
                    F_Disp=TRUE;	
                    GUI_Clear();
                
                break;
                case Key_F2:
                    GUI_Clear();
                    MenuIndex=DEBUG_MENU_DCW;
                    Dcw_Calibrate();//直流耐压校准处理
                    F_Disp=TRUE;
                    GUI_Clear();
                break;
                case Key_F3:
                    GUI_Clear();
                    MenuIndex=DEBUG_MENU_IR;
                    Ir_Calibrate();//绝缘电阻校准处理
                    F_Disp=TRUE;
                    GUI_Clear();
                break;
                case Key_F4:
//                    GUI_Clear();
//                    Gnd_Calibrate();//接地电阻校准处理
                break;
                case Key_F5://退出保存
                    GUI_Clear();
                    Default_Calibrate();//加载默认校准值
                break;
                case Key_Disp://加载默认值
                    
                break;
                case Key_SETUP:
                break;
                case Key_FAST:
                break;
                case Key_LEFT:
                break;
                case Key_RIGHT:
									GUI_Clear();
									U9001_Save_sys.version++;
									if(U9001_Save_sys.version>3)
										U9001_Save_sys.version=1;
									Savetoeeprom();
									F_Disp=TRUE;
									GUI_Clear();
                break;
                case Key_UP:
									
                break;
								case Key_DOWN:
								{
									GUI_Clear();
									if(U9001_Save_sys.jkflag==0)
									{
										U9001_Save_sys.jkflag=1;
									}else if(U9001_Save_sys.jkflag==1){
										U9001_Save_sys.jkflag=0;
									}
									Savetoeeprom();
									F_Disp=TRUE;
									GUI_Clear();
								}break;
                
                default:
                    break;
            }
            keyvalue=KEY_NONE;
        }
	}
}

//==========================================================
//函数名称：Default_Calibrate
//函数功能：加载默认校准值
//入口参数：无
//出口参数：无
//创建日期：2015.02.13
//修改日期：2015.08.31 10:29
//备注说明：无
//==========================================================
void Default_Calibrate(void)
{
	u8 key;

//	Disp_Clr();//清屏
//	LcdAddr.x=10;LcdAddr.y=2;
//	Disp_Str("恢复默认值?",0);//显示
//	Lcd_Draw_Line(0,20,LCD_ADDR_X_MAX);//X=0:画横线，否则画竖线，len:长度或高度
//	LcdAddr.x=1;LcdAddr.y=110;
//	Disp_Str("按<确认>键恢复,<左,右>键返回",0);//显示
    GUI_DispStringAt("按<SETUP>键恢复,<DISP>键返回",10,271-32);

	while(SystemStatus==SYS_STATUS_USERDEBUG)
	{
		if(keyvalue!=KEY_NONE)
		{	
		switch(keyvalue)
		{
//			case KEY_SET:	//设置键
//			case L_KEY_SET:	//长按设置键
//				break;
//	
//			case KEY_UP:	//上键
//			case L_KEY_UP:	//长按上键
//				break;
//	
//			case KEY_DOWN:		//下键
//			case L_KEY_DOWN:	//长按下键
//				break;
//	
//			case KEY_LEFT:		//左键
////			case L_KEY_LEFT:	//长按左键
////				break;
////
			case Key_Disp:		//右键
//			case L_KEY_RIGHT:	//长按右键
				return;
				break;

			case Key_SETUP:		//确认键
//			case L_KEY_ENTER:	//长按确认键
				LoadDefaultCalibrate();//加载默认校准值
				LcdAddr.x=8;LcdAddr.y=20;
				GUI_DispStringAt("恢复默认值成功",40,40);//显示
//				Beep_One();//蜂鸣器响一声
				Delay(1200);//延时1.2s
				return;
				break;
//	
//			case KEY_START:		//启动键
//			case L_KEY_START:	//长按启动键
//				break;
	
//			case KEY_RESET:		//复位键
//			case L_KEY_RESET:	//长按复位键
//				return;
//				break;
			
			default:
				break;
		}
        keyvalue=KEY_NONE;
    }
	}
}

//==========================================================
//交流耐压校准菜单
const char CalAcwTab[][25]=
{
	"1.ACW电压4000V  ",
	"2.ACW电流10.00mA",
	"3.ACW电流2.000mA",
};
//==========================================================
//交流耐压校准项
enum CalAcwEnum
{	
	CAL_ACW_4kV ,
	CAL_ACW_10mA ,
	CAL_ACW_1mA ,
};

//==========================================================
//函数名称：Acw_Calibrate
//函数功能：交流耐压校准处理
//入口参数：无
//出口参数：无
//创建日期：2015.02.13
//修改日期：2015.08.31 10:41
//备注说明：无
//==========================================================
void Acw_Calibrate(void)
{
	u8 i;
	u8 key;
	Bool F_Disp;//显示更新标志

	F_Disp=TRUE;//显示更新
	MenuSelect=0;//菜单选择项

	while(SystemStatus==SYS_STATUS_USERDEBUG)
	{
		if(F_Disp==TRUE)
		{
			F_Disp=FALSE;//显示更新

            GUI_SetColor(GUI_WHITE);
            GUI_DispStringAt("按<启动>键开始，<上,下>键移动",10,271-32);
            GUI_DispStringAt("ACW校正",180,2);
//			LcdAddr.y=27;
			for(i=0;i<sizeof(CalAcwTab)/sizeof(CalAcwTab[0]);i++)//显示列表
			{
				LcdAddr.x=8;
				if(MenuSelect==i)
					GUI_SetColor(GUI_RED);
				else
					GUI_SetColor(GUI_WHITE);
                GUI_DispStringAt(&CalAcwTab[i][0],80,40+40*i);//当前项高亮显示
				GUI_SetColor(GUI_WHITE);
			}
		}
		if(READ_START()==0)
        {
            CalTest_Process();//校准测试处理
            F_Disp=TRUE;
            GUI_Clear();
        }
        if(READ_STOP()==0)
        {
        
        }
		if(keyvalue!=KEY_NONE)
        {
            switch(keyvalue)
            {
                case Key_SETUP:	//设置键
                
                    break;
                case Key_Disp:	//设置键
                    return;
                    break;

                case Key_UP:	//上键
                    if(MenuSelect>0)
                        MenuSelect--;
                    else
                        MenuSelect=sizeof(CalAcwTab)/sizeof(CalAcwTab[0])-1;
                    F_Disp=TRUE;//显示更新
                    break;
        

            

                case Key_DOWN:		//右键
    //			case L_KEY_RIGHT:	//长按右键
                    MenuSelect++;
                    if(MenuSelect>=sizeof(CalAcwTab)/sizeof(CalAcwTab[0]))
                        MenuSelect=0;
                    F_Disp=TRUE;//显示更新
                    break;
        
                
                default:
                    break;
            }
            keyvalue=KEY_NONE;
        }
	}
}

////==========================================================
//直流耐压校准菜单
const u8 CalDcwTab[][25]=
{
	"1.DCW电压4000V  ",
	"2.DCW电流6.00mA",
	"3.DCW电流2.000mA",
	"3.DCW电流200.0uA",
};
//==========================================================
//直流耐压校准项
enum CalDcwEnum
{	
	CAL_DCW_4kV ,
	CAL_DCW_10mA ,
	CAL_DCW_1mA ,
};

//==========================================================
//函数名称：Dcw_Calibrate
//函数功能：直流耐压校准处理
//入口参数：无
//出口参数：无
//创建日期：2015.02.13
//修改日期：2015.08.31 10:29
//备注说明：无
//==========================================================
void Dcw_Calibrate(void)
{
	u8 i;
	u8 key;
	Bool F_Disp;//显示更新标志

	F_Disp=TRUE;//显示更新
	MenuSelect=0;//菜单选择项

	while(SystemStatus==SYS_STATUS_USERDEBUG)
	{
		if(F_Disp==TRUE)
		{
			F_Disp=FALSE;//显示更新
//			LcdAddr.x=9;LcdAddr.y=2;
//			Disp_Str("直流耐压校准",0);//显示
//			Lcd_Draw_Line(0,20,LCD_ADDR_X_MAX);//X=0:画横线，否则画竖线，len:长度或高度
//			Lcd_Draw_Line(0,107,LCD_ADDR_X_MAX);//X=0:画横线，否则画竖线，len:长度或高度
//			LcdAddr.x=1;LcdAddr.y=110;
//			Disp_Str("按<启动>键开始,<左,右>键移动",0);//显示
            GUI_SetColor(GUI_WHITE);
            GUI_DispStringAt("按<启动>键开始，<上,下>键移动",20,271-32);
            GUI_DispStringAt("DCW校正",180,2);
//		
//			LcdAddr.y=27;
			for(i=0;i<sizeof(CalDcwTab)/sizeof(CalDcwTab[0]);i++)//显示列表
			{
				LcdAddr.x=8;
				if(MenuSelect==i)
					GUI_SetColor(GUI_RED);
				else
                    GUI_SetColor(GUI_WHITE);
					GUI_DispStringAt((char*)CalDcwTab[i],80,40+40*i);//当前项高亮显示行
			}
		}
        GUI_SetColor(GUI_WHITE);
		if(READ_START()==0)
        {
            CalTest_Process();//校准测试处理
            F_Disp=TRUE;
            GUI_Clear();
        }
        if(READ_STOP()==0)
        {
        
        }
		if(keyvalue!=KEY_NONE)
        {
            switch(keyvalue)
            {
                case Key_SETUP:	//设置键
                
                    break;
                case Key_Disp:	//设置键
                    return;
                    break;

                case Key_UP:	//上键
                    if(MenuSelect>0)
                        MenuSelect--;
                    else
                        MenuSelect=sizeof(CalAcwTab)/sizeof(CalAcwTab[0])-1;
                    F_Disp=TRUE;//显示更新
                    break;
        

            

                case Key_DOWN:		//右键
    //			case L_KEY_RIGHT:	//长按右键
                    MenuSelect++;
                    if(MenuSelect>=sizeof(CalDcwTab)/sizeof(CalDcwTab[0]))
                        MenuSelect=0;
                    F_Disp=TRUE;//显示更新
                    break;
        
                
                default:
                    break;
            }
            keyvalue=KEY_NONE;
        }
	}
}

//==========================================================
//绝缘校准菜单
const char CalIrTab[][20]=
{
	"1.电压1000V",
	"2.电阻500k ",
	"3.电阻5M   ",
	"4.电阻50M  ",
	"5.电阻500M ",
    "6.电阻5000M ",
};
//==========================================================
//绝缘校准项
enum CalIrEnum
{	
	CAL_IR_1kV ,
	CAL_IR_500k ,
	CAL_IR_5M ,
	CAL_IR_50M ,
	CAL_IR_500M ,
    CAL_IR_5000M ,
};

//==========================================================
//函数名称：Ir_Calibrate
//函数功能：绝缘校准处理
//入口参数：无
//出口参数：无
//创建日期：2015.02.13
//修改日期：2015.08.31 10:29
//备注说明：无
//==========================================================
void Ir_Calibrate(void)
{
	u8 i;
	u8 key;
	Bool F_Disp;//显示更新标志

	F_Disp=TRUE;//显示更新
	MenuSelect=0;//取当前项

	while(SystemStatus==SYS_STATUS_USERDEBUG)
	{
		if(F_Disp==TRUE)
		{
//			F_Disp=FALSE;//显示更新
//			LcdAddr.x=9;LcdAddr.y=2;
//			Disp_Str("绝缘电阻校准",0);//显示
//			Lcd_Draw_Line(0,20,LCD_ADDR_X_MAX);//X=0:画横线，否则画竖线，len:长度或高度
//			Lcd_Draw_Line(0,107,LCD_ADDR_X_MAX);//X=0:画横线，否则画竖线，len:长度或高度
//			LcdAddr.x=1;LcdAddr.y=110;
//			Disp_Str("按<启动>键开始,<左,右>键移动",0);//显示
            GUI_SetColor(GUI_WHITE);
            GUI_DispStringAt("按<启动>键开始，<上,下>键移动",20,271-32);
            GUI_DispStringAt("IR校正",180,2);
//		
//			LcdAddr.y=22;
			for(i=0;i<sizeof(CalIrTab)/sizeof(CalIrTab[0]);i++)//显示列表
			{
				LcdAddr.x=8;
				if(MenuSelect==i)
					GUI_SetColor(GUI_RED);
				else
					GUI_SetColor(GUI_WHITE);
                GUI_DispStringAt(&CalIrTab[i][0],80,40+32*i);
				LcdAddr.y+=17;//下一行
			}
		}
		
		if(READ_START()==0)
        {
            CalTest_Process();//校准测试处理
            F_Disp=TRUE;
            GUI_Clear();
        }
        if(READ_STOP()==0)
        {
        
        }
		if(keyvalue!=KEY_NONE)
        {
            switch(keyvalue)
            {
                case Key_SETUP:	//设置键
                
                    break;
                case Key_Disp:	//设置键
                    return;
                    break;

                case Key_UP:	//上键
                    if(MenuSelect>0)
                        MenuSelect--;
                    else
                        MenuSelect=sizeof(CalIrTab)/sizeof(CalIrTab[0])-1;
                    F_Disp=TRUE;//显示更新
                    break;
        

            

                case Key_DOWN:		//右键
    //			case L_KEY_RIGHT:	//长按右键
                    MenuSelect++;
                    if(MenuSelect>=sizeof(CalIrTab)/sizeof(CalIrTab[0]))
                        MenuSelect=0;
                    F_Disp=TRUE;//显示更新
                    break;
        
                
                default:
                    break;
            }
            keyvalue=KEY_NONE;
        }
	}
}

//==========================================================
//接地校准菜单
const char CalGndTab[][20]=
{
	"1.电压6.000V",
	"2.电流25.00A",
	"3.电流10.00A",
};
//==========================================================
//接地校准项
enum CalGndEnum
{	
	CAL_GND_6V ,
	CAL_GND_25A ,
	CAL_GND_10A ,
};

//==========================================================
//函数名称：Gnd_Calibrate
//函数功能：接地校准处理
//入口参数：无
//出口参数：无
//创建日期：2015.02.13
//修改日期：2015.08.31 10:29
//备注说明：无
//==========================================================
void Gnd_Calibrate(void)
{
	u8 i;
	u8 key;
	Bool F_Disp;//显示更新标志

	F_Disp=TRUE;//显示更新
	MenuSelect=0;//菜单选择项

	while(SystemStatus==SYS_STATUS_USERDEBUG)
	{
		if(F_Disp==TRUE)
		{
//			F_Disp=FALSE;//显示更新
//			LcdAddr.x=9;LcdAddr.y=2;
//			Disp_Str("接地电阻校准",0);//显示
//			Lcd_Draw_Line(0,20,LCD_ADDR_X_MAX);//X=0:画横线，否则画竖线，len:长度或高度
//			Lcd_Draw_Line(0,107,LCD_ADDR_X_MAX);//X=0:画横线，否则画竖线，len:长度或高度
//			LcdAddr.x=1;LcdAddr.y=110;
//			Disp_Str("按<启动>键开始,<左,右>键移动",0);//显示
//		
//			LcdAddr.y=27;
			for(i=0;i<sizeof(CalGndTab)/sizeof(CalGndTab[0]);i++)//显示列表
			{
				LcdAddr.x=8;
//				if(MenuSelect==i)
//					Disp_Str(&CalGndTab[i][0],1);//当前项高亮显示
//				else
//					Disp_Str(&CalGndTab[i][0],0);
				LcdAddr.y+=20;//下一行
			}
		}
		
		key=Key_Read_WithTimeOut(TICKS_PER_SEC_SOFTTIMER/10);//等待按键(100*10ms/10=100ms)
		switch(key)
		{
//			case KEY_SET:	//设置键
//			case L_KEY_SET:	//长按设置键
//				break;

//			case KEY_UP:	//上键
//			case L_KEY_UP:	//长按上键
//				break;
	
//			case KEY_DOWN:		//下键
//			case L_KEY_DOWN:	//长按下键
//				break;
	
//			case KEY_LEFT:	//左键
////			case L_KEY_LEFT:	//长按左键
//				if(MenuSelect>0)
//					MenuSelect--;
//				else
//					MenuSelect=sizeof(CalGndTab)/sizeof(CalGndTab[0])-1;
//				F_Disp=TRUE;//显示更新
//				break;
//	
//			case KEY_RIGHT:		//右键
////			case L_KEY_RIGHT:	//长按右键
//				MenuSelect++;
//				if(MenuSelect>=sizeof(CalGndTab)/sizeof(CalGndTab[0]))
//					MenuSelect=0;
//				F_Disp=TRUE;//显示更新
//				break;

//			case KEY_ENTER:		//确认键
//			case L_KEY_ENTER:	//长按确认键
//				return;
//				break;
//	
//			case KEY_START:		//启动键
//			case L_KEY_START:	//长按启动键
//				CalTest_Process();//校准测试处理
//				Disp_Clr( );//清屏
//				F_Disp=TRUE;//显示更新
//				break;
//	
//			case KEY_RESET:		//复位键
//			case L_KEY_RESET:	//长按复位键
//				return;
//				break;
			
			default:
				break;
		}
	}
}

//==========================================================
//函数名称：CalTest_Process
//函数功能：校准测试处理
//入口参数：无
//出口参数：无
//创建日期：2015.08.31
//修改日期：2015.08.31 10:29
//备注说明：无
//==========================================================
void CalTest_Process(void)
{
	u8 *pt;
	u8 *pt2;
	
	u8 i;
	vu8 key;
	vu8 sortT;//分选延时
	u8 count;//步进计数器
	
	vu16 dat;
	vu32 temp;
	Bool f_disp=FALSE;//显示更新标志

	bool f_sort=FALSE;//分选标志
	bool f_upper=FALSE;//量程过压标志
	bool f_below=FALSE;//量程欠压标志
	bool f_switch=FALSE;//量程切换标志

	vu16 set_item;//测试项目
	vu16 set_ramp;//设置的缓升
	vu16 set_high;//测试值上限
	vu16 set_low;//测试值下限
	vu16 ad_high;//AD值上限
	vu16 ad_low;//AD值下限
    u16 TestTime;

//	Disp_Clr( );//清屏
    GUI_Clear();
	//更新设置值
	set_item=MenuIndex;//项目

	//读取校准值上下限
	i=CalibrateLimitOffset[set_item];//各参数首项偏移量
    
	i+=MenuSelect;//当前选择项
	i*=4;//每项4字节
	set_high=CalibrateLimit[i][0];//上限
	set_low=CalibrateLimit[i][1];//下限
	i++;
	ad_high=CalibrateLimit[i][0];//上限
	ad_low=CalibrateLimit[i][1];//下限

	//读取当前项目的校准值
	if(set_item==ACW_SETUP)
	{
		i=3;//ACW的校准项数
		pt=(u8*)(&U9001_Calibrate.AcwVol);//校准值首地址
        
	}
	else if(set_item==DCW_SETUP)
	{
		i=4;//DCW
		pt=(u8*)(&U9001_Calibrate.DcwVol);//校准值首地址
	}
	else if(set_item==IR_SETUP)
	{
		i=6;//IR
		pt=(u8*)(&U9001_Calibrate.IrVol);//校准值首地址
	}
	else
	{
		i=2;//GND
		pt=(u8*)(&U9001_Calibrate.GndVol);//校准值首地址
	}
	pt2=(u8*)(&Cal);//更新通用校准值
	key=i*sizeof(Cal_TypeDef);
	for(i=0;i<key;i++)
	{
		*pt2++=*pt++;
	}

	//全局变量处理
	Voltage=0;//电压
	Current=0;//电流
	Resistance=0;//电阻
	TestTime=0;//时间
 	Range=0;//量程初始化
 	AdCount=0;//A/D值采样计数

	RampStepYs=0;//缓升步进余数
	RampStepFd=0;//缓升步进幅度
	RampStepTime=0;//缓升步进时间
	TestOut=0;//测试输出
	FullOut=0;//满载输出
	F_100ms=FALSE;//100ms定时
	
	//AD缓冲区清空
	for(i=0;i<(AD_BUF_LENGTH);i++)
	{
		Vbuf[i]=0;
		Ibuf[i]=0;
	}

	//默认满载D/A输出值
	FullOut=DefaultCalDaOut[set_item][MenuSelect];

	//缓升值计算
	set_ramp=10;//1.0s
	RampStepFd=FullOut/set_ramp;//缓升步进幅度
	RampStepYs=FullOut%set_ramp;//缓升步进余数

	//输出控制
	switch(set_item)//参数
	{
		case ACW_SETUP:
////			Dc_Output_Off();//DC输出和采样关闭
////			Frequency_Control(FREQ_50);//输出频率控制
////			Arc_Out(0);//电弧等级输出
            ACW_Select();
            Sin_out(0);//输出频率控制
            if(MenuSelect>0)//非输出项判别
                Range=MenuSelect-1;
            else
                Range=0;
			break;

		case DCW_SETUP:
////			Dc_Output_On();//DC输出和采样
////			Frequency_Control(FREQ_400);//输出频率控制
////			Arc_Out(0);//电弧等级输出
//            DCW_Select();
//            Sin_out(2);//输出频率控制
            DCW_Select();
            Sin_out(2);//输出频率控制
            if(MenuSelect>0)//非输出项判别
                Range=MenuSelect-1;
            else
                Range=0;
			break;

		case IR_SETUP:
//            DCW_Select();
//            Sin_out(2);//输出频率控制
////			Dc_Output_On();//DC输出和采样
////			Frequency_Control(FREQ_400);//输出频率控制
//			SetSystemMessage(MSG_TEST);//系统信息-满载测试
            DCW_Select();
            Sin_out(2);//输出频率控制
            if(MenuSelect)//非输出项判别
                Range=MenuSelect;
            else
                Range=0;
			break;

//		case ITEM_GND:
//			Dc_Output_Off();//DC输出和采样关闭
//			Frequency_Control(FREQ_50);//输出频率控制
//			break;
	
		default:
			SetSystemStatus(SYS_STATUS_RESET);//复位
			return;
			break;
	}		

	//量程控制
	
		
//	Amp_Output_On(set_item);//功放输出开
	Range_Control(Range);//量程控制
	Delay(50);//硬件延时

	Sin_Clk_Enable();//输出正弦波频率
	
	//声光状态控制
//	Led_Pass_Off();//关合格灯
//	Led_Fail_Off();//关报警灯
//	Led_HV_On();//开高压灯
//	Beep_One();//响一声

	//显示测试界面
//	LcdAddr.x=TITLE_ADDR_X;	LcdAddr.y=TITLE_ADDR_Y;
	GUI_DispStringAt("校准中..",40,271-32);//显示测试状态
    Uart0_Send(0xa1);
		PLC_OutProg();//开PLC启动
//	//显示测试参数
//	LcdAddr.x=TITLE_ADDR_X+14; LcdAddr.y=TITLE_ADDR_Y;
	GUI_DispStringAt((char*)ItemTab[set_item],170,2);//显示参数

//	//显示隔离线
//	Lcd_Draw_Line(0,TITLE_ADDR_Y+18,LCD_ADDR_X_MAX);//X=0:画横线，否则画竖线，len:长度或高度
//	Lcd_Draw_Line(0,TITLE_ADDR_Y+20,LCD_ADDR_X_MAX);//X=0:画横线，否则画竖线，len:长度或高度

//	Lcd_Draw_Line(0,107,LCD_ADDR_X_MAX);//X=0:画横线，否则画竖线，len:长度或高度
//	LcdAddr.x=1;LcdAddr.y=110;
	GUI_DispStringAt("按快捷键修改，OK键保存",20,271-72);//显示
    dispSetupButtonvalue(3,3);
    GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
	sortT=0;//分选延时
	count=0;//步进计数器
	SetSystemMessage(MSG_RAMP);//系统信息-缓升
	SetSoftTimer(DISP_SOFTTIMER,TICKS_PER_SEC_SOFTTIMER/10);//设置延时时间，1000/10=100mS

	while(GetSystemStatus()==SYS_STATUS_USERDEBUG)
	{
		if( GetSoftTimerOut(DISP_SOFTTIMER) )//等待延时结束
		{
			F_100ms=TRUE;
			SetSoftTimer(DISP_SOFTTIMER,TICKS_PER_SEC_SOFTTIMER/10);//设置延时时间，1000/10=100mS
		}

		if(F_100ms==TRUE)//100ms定时处理
		{
			F_100ms=FALSE;
			f_disp=TRUE;//显示更新

			if(sortT<9999)sortT++;

			if(GetSystemMessage()==MSG_RAMP)//系统信息-缓升
			{
				TestOut+=RampStepFd;//测试输出值计算
				if(RampStepYs)//缓升步进余数
				{
					TestOut+=RampStepYs;//测试输出值计算
				  	RampStepYs--;//缓升步进余数
				}

				if(sortT>=set_ramp)//缓升时间判别
				{
					sortT=0;//测试时间清零
					TestOut=FullOut;//测试输出值计算
					SetSystemMessage(MSG_TEST);//系统信息-满载测试
				}
				TestTime=sortT;//测试时间
			}
			else
			{
				TestOut=FullOut;//测试输出值计算
			}
			
			//信号输出控制
			V_DA_out(TestOut);//信号D/A输出
            
		}

		//读取A/D值
		Read_Ad();//读取AD值
		Ad_Filter();//AD值滤波
//		Get_Result();//计算测试值
        Test_Value.Vol=Voltage/2;
		if(set_item==IR_SETUP)//修正绝缘电阻的电流值，校准值为电阻
		{
//			Current*=2;
//			Current/=5;
		}
		
		//计算电阻值
		switch(set_item)//参数项
		{
			case ACW_SETUP:
			case DCW_SETUP:
				//最大值判别
				dat=ACW_CURRENT_MAX;
				if(set_item==DCW_SETUP)
					dat=DCW_CURRENT_MAX;
//				if(Current>dat)//最大值判别
//					Current=TEST_VALUE_OVER;//电阻溢出
				break;
	
			case IR_SETUP:
				//档位换算
//				if(Current)//电流值非零
//				{
//					temp=Test_Value.Vol;//电压值
//					switch(Range)
//					{
//						case 0: //0.5M
//							temp*=1;
//							break;
//				
//						case 1:	//5M
//							temp*=10;
//							break;
//				
//						case 2:	//50M
//							temp*=100;
//							break;
//				
//						case 3:	//500M
//							temp*=1000;
//							break;
//                        case 4:	//5000M
//							temp*=10000;
//							break;
//				
//						default:
//							break;
//					}
//					temp/=Current;//电流值
//					Resistance=(u16)temp;//电阻值
//					//最大值判别
//					if(Resistance>IR_RESISTANCE_MAX)//最大值判别
//						Resistance=TEST_VALUE_OVER;//电阻溢出
//				}
//				else
//				{
//					Resistance=TEST_VALUE_OVER;//电阻溢出
//				}
				break;
		
//			case ITEM_GND:
//				if(Current)//电流值非零
//				{
//					temp=Voltage;//电压值
//					temp*=1000;
//					temp/=Current;//电流值
//					Resistance=(u16)temp;//电阻值
//					//最大值判别
//					if(Resistance>GND_RESISTANCE_MAX)//最大值判别
//						Resistance=TEST_VALUE_OVER;//电阻溢出
//				}
//				else
//				{
//					Resistance=TEST_VALUE_OVER;//电阻溢出
//				}
//				break;
		}
		
		//过流处理
		if((GetSystemMessage()==MSG_OVER))
		{
//			Output_Off();//输出关闭
//			Led_Fail_On();//开报警灯
//			Beep_Long();//蜂鸣器长响一声
			return;
		}

		//分选判别
		if(sortT>=SORT_TIME_MIN)//超过最小时间后才开始分选
			f_sort=TRUE;//分选标志
	
 		//上下限判别
		if((f_sort==TRUE) && (AdCount>=AD_BUF_LENGTH))
		{
			switch(set_item)//参数项
			{
				case ACW_SETUP:
				case DCW_SETUP:
					if(MenuSelect==0)
						dat=Test_Value.Vol;
					else
						dat=Current;
					break;
		
				case IR_SETUP:
					if(MenuSelect==0)
						dat=Test_Value.Vol;
					else
//						dat=Resistance;
                        dat=Current;
					break;

//				case ITEM_GND:
//					if(MenuSelect==0)
//						dat=Current;
//					else
//						dat=Voltage;
//					break;

			}
			//上下限报警
			if((Cal[MenuSelect].Num>set_high)||(Cal[MenuSelect].Num<set_low))//超上下限
			{
//				Beep_One();//蜂鸣器响一声
			}
		}

		if(f_disp==TRUE)//显示更新
		{
			f_disp=FALSE;//显示更新
			//LcdAddr.x=DATA_ADDR_X; LcdAddr.y=DATA_ADDR_Y+HEIGHT_OF_STEP;
			switch(set_item)//按测试项目显示
			{
				case ACW_SETUP:
				case DCW_SETUP:
					Hex_Format(Test_Value.Vol,3,4,1);//数据格式化到DispBuf
                    
                    GUI_DispStringAt(DispBuf,40,40);
                    if(MenuSelect==0)
                    {
                        Hex_Format(Cal[0].Num,0,4,1);//数据格式化到DispBuf
                        strcat(DispBuf,"kV");
                        GUI_DispStringAt(DispBuf,160,40);
                    }

					

//					LcdAddr.x=DATA_ADDR_X+WIDTH_OF_VALUE;
//                    if(MenuSelect==1)
//                        Hex_Format(Current,0,4,1);//数据格式化到DispBuf
//                    else
                        Hex_Format(Current,0,4,1);//数据格式化到DispBuf
                    
                        

					if(Current!=TEST_VALUE_OVER)
                    {
//                        strcat(DispBuf," mA");
						 GUI_DispStringAt(DispBuf,40,80);
                        if(MenuSelect)
                        {
                            Hex_Format(Cal[MenuSelect].Num,0,4,1);//数据格式化到DispBuf
                            strcat(DispBuf,"mA");
                            GUI_DispStringAt(DispBuf,160,80);
                            
                        }
                        
                    }
					else
						GUI_DispStringAt("-----",40,80);//显示测试值
//					Disp_Txt_Str("mA");
					break;

				case IR_SETUP:
                    Hex_Format(Test_Value.Vol,3,4,1);//数据格式化到DispBuf
                    
                    GUI_DispStringAt(DispBuf,40,40);
                
                    Hex_Format(Cal[0].Num,0,4,1);//数据格式化到DispBuf
                    strcat(DispBuf,"kV");
                    GUI_DispStringAt(DispBuf,160,40);
                
                    
                    if(Current!=TEST_VALUE_OVER)
                    {
                         if(MenuSelect)
                        {
                            Hex_Format(Current,0,4,1);//数据格式化到DispBuf
                            GUI_DispStringAt(DispBuf,40,80);
                            Hex_Format(Cal[MenuSelect].Num,0,4,1);//数据格式化到DispBuf
                            strcat(DispBuf,"MΩ");
                            GUI_DispStringAt(DispBuf,160,80);
                            
                        }
                        
                    }
					else
						GUI_DispStringAt("-----",40,80);//显示测试值 
					break;

//				case ITEM_GND:
//					Hex_Format(Current,2,4,1);//数据格式化到DispBuf
//					Disp_Txt_Str(DispBuf);//显示测试值
//					Disp_Txt_Str("A");

//					LcdAddr.x=DATA_ADDR_X+WIDTH_OF_VALUE;
//					Hex_Format(Resistance,0,4,1);//数据格式化到DispBuf
//					if(Resistance!=TEST_VALUE_OVER)
//						Disp_Txt_Str(DispBuf);//显示测试值
//					else
//						Disp_Txt_Str("----");//显示测试值
//					Disp_Txt_Str("m");
//					Disp_Str("m",0);//显示
//					Disp_Char(OHM,0);//显示Ω
//					Disp_Str(" ",0);//空格
//					break;
			}
		}
		if(READ_STOP()==0)
        {
            V_DA_out(0);
            FRB_out(0);
            Uart0_Send(0xa0);
            return;
        }
		if(keyvalue!=KEY_NONE)
        {
		switch(keyvalue)
		{
            case Key_F2:
			case Key_UP:	//上键
				count=0;//步进计数器
				Cal[MenuSelect].Num++;
				if(Cal[MenuSelect].Num>9999)
					Cal[MenuSelect].Num=0;
				break;

			case Key_F1:	//长按上键
				count=0;//步进计数器
//				if(count>10)//步进计数器
					Cal[MenuSelect].Num+=10;
//				else
//					Cal[MenuSelect].Num++;
				if(Cal[MenuSelect].Num>9999)
					Cal[MenuSelect].Num=0;
				break;
			case L_Key_F1:
			{
				count++;
				if(count>10)//步进计数器
					Cal[MenuSelect].Num+=10;
				if(count>30)//步进计数器
					Cal[MenuSelect].Num+=30;
				if(Cal[MenuSelect].Num>9999)
					Cal[MenuSelect].Num=0;
			}break;
	        case Key_F3:
			case Key_DOWN:		//下键
				count=0;//步进计数器
				Cal[MenuSelect].Num--;
				if(Cal[MenuSelect].Num>9999)
					Cal[MenuSelect].Num=9999;
				break;

            

			case Key_F4:		//
               count=0;//步进计数器
//				if(count>10)//步进计数器
					Cal[MenuSelect].Num-=10;
//				else
//					Cal[MenuSelect].Num--;
				if(Cal[MenuSelect].Num>9999)
					Cal[MenuSelect].Num=0;
            break;
			case L_Key_F4:
			{
				count++;
				if(count>10)//步进计数器
					Cal[MenuSelect].Num-=10;
				if(count>30)//步进计数器
					Cal[MenuSelect].Num-=30;
				if(Cal[MenuSelect].Num>9999)
					Cal[MenuSelect].Num=0;
			}break;
            case Key_F5:
            break;
            case Key_FAST:
                FRB_out(0);
                Uart0_Send(0xa0);
                V_DA_out(0);
//				Output_Off();//输出关闭
				//更新当前项目的校准值
                if(MenuSelect==0)
                   Cal[MenuSelect].NumAd=Voltage/2; //电压的AD值
                else
                    Cal[MenuSelect].NumAd=Current;
				if(set_item==ACW_SETUP)
				{
					i=3;//ACW的校准项数
					pt=(u8*)(&U9001_Calibrate.AcwVol);//校准值首地址
				}
				else if(set_item==DCW_SETUP)
				{
					i=4;//DCW
					pt=(u8*)(&U9001_Calibrate.DcwVol);//校准值首地址
				}
				else if(set_item==IR_SETUP)
				{
					i=6;//IR
					pt=(u8*)(&U9001_Calibrate.IrVol);//校准值首地址
				}
				else
				{
//					i=2;//GND
//					pt=(u8*)(&SaveData.Calibrate.GndVol);//校准值首地址
				}
				pt2=(u8*)(&Cal);//更新通用校准值
				key=i*sizeof(Cal_TypeDef);
				for(i=0;i<key;i++)
				{
					*pt++=*pt2++;
				}
				//校验和保存校准数据
				SaveCal();//保存校准值
				return;
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
//                case Key_NUM0:
                    break;

////			case KEY_START:		//启动键
////			case L_KEY_START:	//长按启动键
////				break;

//			case KEY_RESET:		//复位键
//			case L_KEY_RESET:	//长按复位键
//				Output_Off();//输出关闭
//				return;
//				break;
			
			default:
				break;
		}
        keyvalue=KEY_NONE;
    }
	}
}

//==========================================================
//END
//==========================================================
