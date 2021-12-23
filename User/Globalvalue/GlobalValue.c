//==========================================================
//文件名称：GlobalValue.c
//文件描述：全局变量定义
//文件版本：Ver1.0
//创建日期：2015.10.28
//修改日期：2015.10.28 15:38
//文件作者：
//备注说明：无
//注意事项：无
//==========================================================
#include "TypeDefine.h"	//数值类型
//#include "Beep.h"
#include "stdbool.h"
#include "Globalvalue/GlobalValue.h"
#include  "lpc_types.h"
#include "use_disp.h"
//#include


//==========================================================
//全局变量
Tft_5520_Typedef Tft_5520;
//Res_countTypeDef Res_count;
Save_Res_Typedef Save_Res;
Fft_Cov_Typedef Fft_Cov;
Fft_Cov_Typedef Fft_Cov_slow;


Tft_5520_Disp_Typedef Tft_5520_Dispvalue;
Send_Testvalue_Typedef Send_Testvalue[2];
Save_TestValue_Typedef Save_TestValue[10];
Test_TypeDef Test_value;


Test_UnitTypedef Test_Unit;
uint32_t clear_flag;
uint32_t Usb_Open_flag;
uint8_t Send_buff2[30];
vu8 MenuIndex;//待机菜单项
uint32_t softswitch;
uint8_t usb_oenflag;
uint8_t debug_saveflag;
uint8_t Uart3_returnflag;
uint8_t Pc_Sendflag;
uint32_t SoftTimer[MAXTSOFTTIMER];//软定时器
uint8_t textFileBuffer[50];
uint8_t DISP_FLAG;
vu8 RampStepTime;//缓升步进时间
vu16 RampStepYs;//缓升步进余数
vu16 RampStepFd;//缓升步进幅度

vu8 DropStepTime;//缓降步进时间
vu16 DropStepYs;//缓降步进余数
vu16 DropStepFd;//缓降步进幅度
vu16 TestOut;//测试输出值
vu16 FullOut;//满载输出值
vu8 AdCount;//A/D值采样计数
 vu16 Vbuf[AD_BUF_LENGTH];//电压A/D值缓冲
 vu16 Ibuf[AD_BUF_LENGTH];//电流A/D值缓冲
vu16 Current;
 vu32 Resistance;//电阻
  u8 save_flag;
 int Voltage;//电压
 bool F_Fail;//测试失败标志
u16 rangelomit_low,rangr_limit_high;
//  u8 run_stemp;//运行步数   连接的时候用
uint8_t  F_100ms;

uint8_t Range,VRange_flag,debug,Clear_flag;

uint8_t step;
uint8_t U_step;
u8 _4094_databuff[_4094NUM];
uint32_t rebuff_v[4];
uint32_t  rebuff_i[200];
uint32_t Clear_num;
uint8_t  u_adover_flag;
uint8_t  ry_flag;
float receive;
//===========================================================
uint8_t keyvalue,key_x;
uint32_t background;
u16 cpld_value;
u8 EMC_High,Emc_low;
Test_mid_Typedef Test_mid;

 vu8 StepIndex;//步骤项
 vu8 TotalStep;//测试总步骤

//uint32_t  numBlks;
//uint8_t  inquiryResult[36];
//uint32_t MenuIndex;//待机菜单项
//uint32_t MenuSelect;//菜单选择项
//
//uint32_t SystemStatus;//系统状态
//uint32_t SystemMessage;//系统信息
// Save_TypeDef SaveData;//保存值
// Cal_TypeDef Cal[6];//校准值
//
// NumBox_TypeDef NumBox;//数值框
// Res_TypeDef	Res;
// bool F_Fail;//测试失败标志
// bool F_100ms;//100ms定时标志
// bool F_Password;//密码有效标志
//
// vu8 Range;//当前量程
//
vu8 Uart_Send_Flag;
unsigned long Count_buff[12];
//unsigned long MenuIndex;//待机菜单项
u8 MenuSelect;//菜单选择项
//
 unsigned long SystemStatus;//系统状态
unsigned long SystemMessage;//系统信息
Disp_Coordinates_Typedef Disp_Coordinates;
char DispBuf[12];
NumBox_TypeDef NumBox;
LcdAddr_TypeDef LcdAddr;
uint8_t V_Range;
char U_Storebuff[50];
u16 Test_Time;
const uint8_t  RANGE_UNIT[12]={	3,	2,	1,	0};
//==========================================================
//默认校准输出值
const vu16 DefaultCalDaOut[][6]=
{
	 2200,550,550,0,0,0, //Acw-4kV
	 2400,362,604,0,0,0, //Dcw-4kV
	  604,604,604,604,604,604, //Ir -1kV
	 3000,0,0,0,0,0 //Gnd-25A
};
//==========================================================
//默认校准数据
const vu16 DefaultCalibrate[]=
{
	 3900, 2000, 1000,1211, 2000, 2210 ,//ACW
	 3900, 2200, 600,730, 2000, 2400 ,//DCW
	 1000, 552, 500,2400, 5000, 2400, 5000, 2400, 5000, 2400 ,5000, 2400 ,//IR
	 2500, 3000, 6000,3000 ,//GND
};
//==========================================================
//校准值上、下限
const u16 CalibrateLimit[][2]=
{
	 4100,3800, 2100,1900, 1200, 800, 1300,1100, 2200, 1800, 2400,2000, //ACW
	 4100,3800, 2400,2000, 800, 500, 900,600, 2200, 1800, 2600,2200, //DCW
	 1200, 800, 650, 450, 520,490, 2600,2200, 5200,4900, 2600,2200, //IR
	 5200,4900, 2600,2200, 5200,4900, 2600,2200, 5200,4900, 2600,2200, //IR
	 2600,2400, 3200,2800, 6500,5500, 3500,3000, //GND
};
//const u16 Calibrate[][2]=
//{
//    4000,2000,1000,1600,2000,3200,
//    4000,2000, 600,1000,2000,3200,
//    1000,500 , 5000, 2000,5000, 2000, 5000, 2000, 5000, 2000, 5000, 2000 //IR

//};
//校准值上、下限地址偏移量
const u8 CalibrateLimitOffset[]=
{
	0,
	3,
	3+3,
	3+3+5,
};
//==========================================================
//加载默认校准值
//==========================================================
void LoadDefaultCalibrate(void)
{
	u8 i;
	u8 *pt,*pt2;

	pt =(u8*)&(U9001_Save_sys.U9001_Calibrate);
	pt2=(u8*)&(DefaultCalibrate);
	for(i=0;i<sizeof(DefaultCalibrate);i++)
	{
		*pt++=*pt2++;
	}
	Savetoeeprom();//保存校准值
}
//
// vu16 Voltage;//电压
// vu16 Current;//电流
// vu16 Resistance,xxxx;//电阻
//vu16 i_buff[FIT_INUM];		   //滤波值
//vu8	count_ffit;
//u16 	zreo_num[6];
//vu8 ffit_data1;
//u8 fuhao;
//bool clear_flag;
//bool vol_flag;
//==========================================================
//函数名称：InitGlobalValue
//函数功能：初始化全局变量
//入口参数：无
//出口参数：无
//创建日期：2015.10.28
//修改日期：2015.10.28 15:33
//备注说明：无
//==========================================================
void InitGlobalValue(void)
{
	vu16 len;
	vu8 * buf;
						 
	buf=(vu8 *)&Tft_5520;//数据首地址
	len=sizeof(Tft_5520_Typedef);
	while(len--)
	{
		*buf=0;//清空
		buf++;
	}
//	buf=(vu8 *)&SaveData.Limit_ScanValue;//列表扫描初始化
//	len=sizeof(SaveData.Limit_ScanValue);
//	while(len--)
//	{
//		*buf=0xff;//清空
//		buf++;
//	}
//	SaveData.Limit_ScanValue.fun=0;
//	//F_Password=FALSE;//密码有效标志	
    if(Tft_5520.Group>=GROUP_MAX)
        Tft_5520.Group=0;
    Uart3_returnflag=0;
}
//==========================================================
//函数名称：Check_Calibrate_Limit
//函数功能：校准值检查
//入口参数：无
//出口参数：无
//创建日期：2015.01.14 
//修改日期：2015.08.27 14:33
//备注说明：上、下限检查,	L<=x<=H
//==========================================================
void Check_Calibrate_Limit(void)
{
	u8 i;
	u8 j;
	vu16 *pt;

	pt=(vu16 *)&U9001_Save_sys.U9001_Calibrate;
	j=sizeof(CalibrateLimit)/4;
	for(i=0;i<j;i++)//共j项
	{
		NumBox.Max=CalibrateLimit[i][0];//Max
		NumBox.Min=CalibrateLimit[i][1];//Min
		if(*pt > NumBox.Max)
			*pt=DefaultCalibrate[i];
		if(*pt < NumBox.Min)
			*pt=DefaultCalibrate[i];
		pt++;
	}
}
////==========================================================
////函数名称：LoadDefaultSet
////函数功能：加载默认设置值
////入口参数：无
////出口参数：无
////创建日期：2015.10.28
////修改日期：2015.10.28 15:33
////备注说明：无
////==========================================================
//void LoadDefaultSet(void)
//{
//	LoadDefaultParameter();//加载默认参数
//	SaveParameter();//保存设置数据
//	
//	LoadDefaultSystem();//加载默认系统值
//	SaveSystem();//保存系统参数
//	
//	LoadDefaultConfig();//加载默认配置值
//	SaveConfig();//保存配置值
//}

////==========================================================
////默认参数设置数据
//const vu8 DefaultParameter[]=
//{
//	0,	//量程-自动
//	0,	//讯响-关闭
//	0,	//清零-关闭
//	0,	//外触发-关闭
//	0,	//串口-关闭
//};

////测试底数
//const s16 BASE_NUM[6]=
//{	
//	100,//1300,
//	0,//170,
//	0,//20,
//	0,//10,
//	0
//};
////测试电阻小数点位置
//const u8 DOT_POS[6]=
//{	
//	2,
//	1,
//	3,
//	2,
//	0
//};
////==========================================================
////加载默认参数
////==========================================================
//void LoadDefaultParameter(void)
//{
//	u8 i;
//	u8 *pt,*pt2;

//	pt =(u8*)&(SaveData.Parameter);
//	pt2=(u8*)&(DefaultParameter);
//	for(i=0;i<sizeof(DefaultParameter);i++)
//	{
//		*pt++=*pt2++;
//	}
//}
////==========================================================
////默认系统设置数据
//const u8 DefaultSystem[]=
//{
//	0,200,1,0,	//电阻上限-020.0m
//	0,100,2,0,	//电阻下限-01.00m
//};
////==========================================================
////加载默认系统值
////==========================================================
//void LoadDefaultSystem(void)
//{
//	u8 i;
//	vu8 *pt;

//	pt =(vu8*)&(SaveData.System);
//	for(i=0;i<sizeof(DefaultSystem);i++)
//	{
//		*pt++=DefaultSystem[i];
//	}
//}

////==========================================================
////加载默认配置 值
////==========================================================
//void LoadDefaultConfig(void)
//{
//	SaveData.Config.Password=DEFAULT_PASSWORD;//设置密码
//	SaveData.Config.BootNum=SET_BOOT_DEFAULT;//开机次数
//	SaveData.Config.ProductId=DEFAULT_MACHINE_ID;//出厂编号
//}

////==========================================================
////默认校准数据
//const vu16 DefaultCalibrate[][2]=
//{
//	 1000, 1000, 
//	 1000, 1000, 
//	 5000, 3200, 
//	 5000, 3200, 
//	 5000, 3200, 
//	 5000, 3200,
//};
////==========================================================
////校准值上、下限
//const u16 CalibrateLimit[][2]=
//{
//	 1300, 800,
//	 1300, 800, 
//	 1300, 800, 
//	 1300, 800, 
//	 3200,2800, 
//	 3200,2800, 
//};

////==========================================================
////加载默认校准值
////==========================================================
//void LoadDefaultCalibrate(void)
//{
//	u8 i;
//	u8 *pt,*pt2;

//	pt =(u8*)&(SaveData.Calibrate);
//	pt2=(u8*)&(DefaultCalibrate);
//	for(i=0;i<sizeof(DefaultCalibrate);i++)
//	{
//		*pt++=*pt2++;
//	}
//	SaveCalibrate();//保存校准值
//}

////==========================================================
////函数名称：Check_Calibrate_Limit
////函数功能：校准值检查
////入口参数：无
////出口参数：无
////创建日期：2015.10.28
////修改日期：2015.10.28 15:38
////备注说明：上、下限检查,	L<=x<=H
////==========================================================
//void Check_Calibrate_Limit(void)
//{
////	u8 i;
//	u8 j;
//	vu16 *pt;

//	pt=(vu16 *)&SaveData.Calibrate;
//	j=sizeof(CalibrateLimit)/sizeof(CalibrateLimit[0]);
////	for(i=0;i<j;i++)//共j项
////	{
////		NumBox.Max=CalibrateLimit[i][0];//Max
////		NumBox.Min=CalibrateLimit[i][1];//Min
////		if(*pt > NumBox.Max)
////			*pt=NumBox.Max;
////		if(*pt < NumBox.Min)
////			*pt=NumBox.Min;
////		pt++;
////	}
//}

////==========================================================
////函数名称：ReadSaveData
////函数功能：读取保存参数
////入口参数：无
////出口参数：无
////创建日期：2015.10.28
////修改日期：2015.10.28 15:38
////备注说明：无
////==========================================================
//void ReadSaveData(void)
//{
//#if DEBUG_SUPPORT
//	LoadDefaultSet();//调用默认设置值
//	LoadDefaultCalibrate();//加载默认校准值
//#else
//	ReadParameter();//读取设置参数
//	ReadSystem();//读取系统参数
//	ReadConfig();//读取配置信息
//	ReadCalibrate();//读取校准值
//#endif
//	
//	//更新当前量程
//	if(SaveData.Parameter.Range==0)//量程自动
//		Range=0;
//	else
//		Range=SaveData.Parameter.Range-1;//偏移量1

//}

//==========================================================
//函数名称：SetSoftTimer
//函数功能：软定时器设置
//入口参数：软定时器id,定时节拍
//出口参数：无
//创建日期：2014.09.29
//修改日期：2014.10.21 15:08
//备注说明：无
//==========================================================
void SetSoftTimer(uint8_t  id, uint32_t ticks)
{
	SoftTimer[id]=ticks;
}
//==========================================================
//函数名称：GetSoftTimerOut
//函数功能：软定时器溢出
//入口参数：软定时器id
//出口参数：True(定时到)/False
//创建日期：2014.09.29
//修改日期：2014.09.29 11:18
//备注说明：无
//==========================================================
uint8_t GetSoftTimerOut(uint8_t id)
{
	return (SoftTimer[id]?FALSE:TRUE);
}
uint32_t Get_SorftTime(uint8_t id)
{
    return SoftTimer[id];
}
U9001_save_Typedef U9001_save;
U9001_Save_sysTypedef U9001_Save_sys;
//==========================================================
//END
//==========================================================

