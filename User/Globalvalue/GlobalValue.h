//==========================================================
//文件名称：GlobalValue.h
//文件描述：全局变量头文件
//文件版本：Ver1.0
//创建日期：2015.10.26 
//修改日期：2015.10.28 15:38
//文件作者：
//备注说明：无
//注意事项：无
//==========================================================
#ifndef __GlobalValue_h__
#define __GlobalValue_h__
#include "stdint.h"
#include "stdbool.h"

typedef unsigned char vu8;
//typedef unsigned char u8;
typedef unsigned short     int vu16;
typedef unsigned           int vu32;
#define ZC5520 0
#define SetSystemStatus(status) (SystemStatus=status)
#define GetSystemStatus() (SystemStatus)
#define GetSystemMessage() (SystemMessage)
#define SetSystemMessage(msg) (SystemMessage=msg)
#define FIT_INUM    12

#define FFTLENTH 16
//==========================================================
//软件版本：01-9310 02-9320 03-9320A
#define SOFTWARE_VERSION		U9001_Save_sys.version
//#define SOFTWARE_VERSION		(0x02)
//==========================================================
//默认出厂编号(0)
#define DEFAULT_MACHINE_ID 		(0)

//默认出厂密码(0)
#define DEFAULT_PASSWORD 		(0)

//默认开机次数(0)
#define SET_BOOT_DEFAULT		(0)

//==========================================================
//调试配置
#define _DEBUG_CONFIG_FALSE		(0)
#define _DEBUG_CONFIG_TRUE		(1)

//软定时器定义
#define KEYBOARD_SOFTTIMER	(0) //按键延时软定时器
#define DISP_SOFTTIMER 		(1) //显示延时软定时器
#define TEST_SOFTTIMER 		(2) //测试延时软定时器
#define PASS_SOFTTIMER      (3) //合格延时软定时器
#define DELAY_SOFTTIMER      (4) //间隔延时软定时器
#define DIS_SOFTTIMER        (5) //放电时间软定时器
//==========================================================
//仿真调试
//#define DEBUG_SUPPORT			(_DEBUG_CONFIG_TRUE)
#define DEBUG_SUPPORT			(_DEBUG_CONFIG_FALSE)

//==========================================================
//ISP开关配置，仿真调试用
#if DEBUG_SUPPORT
	#define HW_ISP_SUPPORT		(_DEBUG_CONFIG_FALSE)
#else
	#define HW_ISP_SUPPORT		(_DEBUG_CONFIG_TRUE)
#endif

//==========================================================
//Uart开关配置
#define HW_UART_SUPPORT			(_DEBUG_CONFIG_TRUE)

//==========================================================
//蜂鸣器开关配置
#define HW_BEEP_SUPPORT			(_DEBUG_CONFIG_TRUE)

//==========================================================
//AD值滤波配置，增加数据稳定度
#define AD_FILTER_SUPPORT		(_DEBUG_CONFIG_TRUE)

//==========================================================
#define SETUP_MENU_MAX  	(2)//最大设置菜单

#define DEBUG_MENU_MAX  	(5)//最大校准菜单

#define PARAMETER_MENU_MAX	(3)//最大参数菜单

#define SYSTEM_MENU_MAX		(4)//最大系统菜单

#define CONFIG_MENU_MAX		(3)//最大配置项(0-2)

#define PW_LENGTH 			(4)//最大密码位数

#define RANGE_MAX 			(4)//最大量程

#define AD_BUF_LENGTH		(8)//A/D值缓冲区长度

#define MAX_R_RANGE			(6)//电阻最大量程

#define BUTTOM_X_VALUE		(84)//显示下面提示显示的第一个X位置

#define BUTTOM_MID_VALUE	(80)//显示下面提示显示的中间间隔位置

#define BUTTOM_Y_VALUE		(271-30)//显示下面提示显示的第一个Y位置

#define KEY_NUM				(3)	//按键灵敏度

#define NUM_LENTH 			(5)//显示长度
#define NUM_FREQ 			(33+4)//显示长度
#define DEBUG_RANGE			(4+1)
#define SET_MAX             (11)
#define SYS_MAX				(11)
#define GROUP_MAX           (5)
#define MAX_TEXT_STEP       (10)
#define MAXTSOFTTIMER (6)		//软定时器数目

#define USBSENDLENTH        (27)

#define	CONFIG_TIME			(0x5a5a)
//==========================================================
#define OHM 				(0xF4)	//CGROM中有Ω字符，编码为0xF4

//==========================================================
//显示分区的X坐标
#define MSG_ADDR_X 			(1)
#define AUTO_ADDR_X 		(12)
#define RANGE_ADDR_X 		(17)

#define RES_ADDR_X 			(1)
#define VOL_ADDR_X 			(12)

#define DISP_UNIT_XPOS	380
#define DISP_UNIT_YPOS	92

#define INPUT_RECT_X			(80)
#define INPUT_RECT_Y			(50)

#define DISP_V_XPOS             40
#define DISP_V_YPOS             120
#define I_SCAN_NUM      (97)    //电流采样次数

#define _4094NUM    2

#define LKEY1    5
#define LKEY2    15
#define LKEY3    26
//测试值最大值
//#define ACW_CURRENT_MAX		(2000)
//#define DCW_CURRENT_MAX		(2000)
#define ACW_CURRENT_MAX		(3000)
#define DCW_CURRENT_MAX		(3000)
#define IR_RESISTANCE_MAX	(250000)
#define GND_RESISTANCE_MAX	(310)
//测试值溢出
#define TEST_VALUE_OVER		(65535)
//最小分选时间--0.5s
#define SORT_TIME_MIN		(5)
///////////////////////////////////////////////////////////
//16bit数据的位操作定义
#define	SetD0	(0x0001)
#define	SetD1	(0x0002)
#define	SetD2	(0x0004)
#define	SetD3	(0x0008)
#define	SetD4	(0x0010)
#define	SetD5	(0x0020)
#define	SetD6	(0x0040)
#define	SetD7	(0x0080)
#define	SetD8	(0x0100)
#define	SetD9	(0x0200)
#define	SetD10	(0x0400)
#define	SetD11	(0x0800)
#define	SetD12	(0x1000)
#define	SetD13	(0x2000)
#define	SetD14	(0x4000)
#define	SetD15	(0x8000)

#define	ClrD0	(0xfffe)
#define	ClrD1	(0xfffd)
#define	ClrD2	(0xfffb)
#define	ClrD3	(0xfff7)
#define	ClrD4	(0xffef)
#define	ClrD5	(0xffdf)
#define	ClrD6	(0xffbf)
#define	ClrD7	(0xff7f)
#define	ClrD8	(0xfeff)
#define	ClrD9	(0xfdff)
#define	ClrD10	(0xfbff)
#define	ClrD11	(0xf7ff)
#define	ClrD12	(0xefff)
#define	ClrD13	(0xdfff)
#define	ClrD14	(0xbfff)
#define	ClrD15	(0x7fff)

#define HW__5053_A_H			((u16)(SetD0))
#define HW__5053_A_L			((u16)(ClrD0))
#define HW_AC_ON			((u16)(ClrD4))
#define HW_AC_OFF			((u16)(SetD4))

#define  HW_ACDC_START			((u16)(ClrD3))
#define  HW_ACDC_STOP			((u16)(SetD3))

#define HW_DC_ON			((u16)(ClrD5))
#define HW_DC_OFF			((u16)(SetD5))

#define HW_GFIVREF_0H			((u16)(SetD6))
#define HW_GFIVREF_0L			((u16)(ClrD6))

#define HW_GFIVREF_1H			((u16)(SetD7))
#define HW_GFIVREF_1L			((u16)(ClrD7))

#define HW_50HZ					((u16)(ClrD8&ClrD9))
#define HW_60HZ					((u16)(SetD9))

#define HW_400HZ				((u16)(SetD8))

#define WAVE_OUT_ON             ((u16)(ClrD10))
#define WAVE_OUT_OFF            ((u16)(SetD10))

#define AD_CS_H					((u8)(SetD0))
#define AD_CS_L					((u8)(ClrD0))

#define DA_CS_H					((u8)(SetD1))
#define DA_CS_L					((u8)(ClrD1))

#define _4094_STR_H				((u8)(SetD3))
#define _4094_STR_L				((u8)(ClrD3))

#define CPLD_CS_H					((u8)(SetD4))
#define CPLD_CS_L					((u8)(ClrD4))

#define PLC_PASS_H					((u8)(SetD1))
#define PLC_PASS_L					((u8)(ClrD1))

#define PLC_FAIL_H					((u8)(SetD0))
#define PLC_FAIL_L					((u8)(ClrD0))

#define PLC_PROG_H					((u8)(SetD2))
#define PLC_PROG_L					((u8)(ClrD2))



#define HW_HW_PGA_MASK		((u8)((ClrD1&ClrD2&ClrD3&ClrD4&ClrD0)))
#define HW_PGA_1K 			((u8)(SetD0))
#define HW_PGA_10K 			((u8)(SetD1))
#define HW_PGA_100K 		((u8)(SetD2))
#define HW_PGA_103K			((u8)(SetD3))
#define HW_PGA_104K			((u8)(SetD4))
#define HW_PGA_105K			HW_HW_PGA_MASK
#define HW_RY_FLOATH			((u8)(SetD5))
#define HW_RY_FLOATL			((u8)(ClrD5))

#define HW_RY_DC_ON			((u8)(SetD6))
#define HW_RY_DC_OFF			((u8)(ClrD6))

#define HW_RY_AC_ON			((u8)(SetD7))
#define HW_RY_AC_OFF			((u8)(ClrD7))
//#define HW_PRO_TEST			((u8)(SetD1))//测试中信号
//#define HW_PRO_UNTEST		((u8)(ClrD1))

//#define HW_FD_TEST			((u8)(SetD5))//测试
//#define HW_FD_NOTEST		((u8)(~SetD5))//放电


//#define HW_GYB_100V 		((u8)(ClrD3))
//#define HW_GYB_1000V 		((u8)(SetD3))

////#define HW_GYB_1000VFEEDMASK ((u8)(ClrD2&ClrD1))
//#define HW_GYB_1000VFEED	((u8)(SetD0))
//#define HW_GYB_100VFEED		((u8)(ClrD0))


#define HW_GYBFD_ON			((u8)(SetD1))
#define HW_GYBFD_OFF		((u8)(ClrD1))
//==========================================================
//标题长度
#define TITLE_LEN_MAX		(12)
extern  vu8 MenuIndex;//待机菜单项
extern uint8_t Send_buff2[30];
extern uint32_t  rebuff_i[200];
extern uint32_t Clear_num;
extern uint8_t  u_adover_flag;
extern uint8_t  ry_flag,debug,Clear_flag;
extern uint32_t SoftTimer[MAXTSOFTTIMER];//软定时器
extern uint8_t V_Range;
extern uint8_t DISP_FLAG;
extern char U_Storebuff[50];
extern u8 _4094_databuff[_4094NUM];
extern char Resbuf[500];
extern const char TestPara[][8];
extern const char DISP_COMP[][2][15];
extern const char SEND_COMP[][2][15];
extern const u8 Test_IRUINT[][6];
extern bool f_disp;//显示更新标志
//========================================================
extern uint8_t keyvalue,key_x;//按键值
extern uint32_t background;//背景色
extern vu8 RampStepTime;//缓升步进时间
extern vu16 RampStepYs;//缓升步进余数
extern vu16 RampStepFd;//缓升步进幅度

extern  vu8 DropStepTime;//缓降步进时间
extern  vu16 DropStepYs;//缓降步进余数
extern  vu16 DropStepFd;//缓降步进幅度

extern  vu16 TestOut;//测试输出值
extern  vu16 FullOut;//满载输出值
extern vu8 AdCount;//A/D值采样计数
extern vu16 Current;//电流
extern u16 rangelomit_low,rangr_limit_high;
extern vu16 Vbuf[AD_BUF_LENGTH];//电压A/D值缓冲
extern vu16 Ibuf[AD_BUF_LENGTH];//电流A/D值缓冲
extern vu32 Resistance;//电阻
extern u8 save_flag;
extern bool F_Fail;//测试失败标志
extern int Voltage;//电压
extern uint8_t testflag;
enum SysMessageEnum
{
	MSG_IDLE=0 ,
	MSG_TEST , //1
	MSG_ABORT ,//2
	MSG_PASS ,//3
    MSG_RAMP,//4
    MSG_DROP,//5
    MSG_LOW ,//6
    
	MSG_WAIT ,//7
    
	MSG_HIGH ,//8
    
	MSG_SHORT,//9
		
		
	MSG_FAIL ,//10
    MSG_OVER , //11
    MSG_ARC,//12
    MSG_PAUSE,//13
    MSG_GIF,//14
    MSG_OFL ,//15
	MSG_OPEN ,//16
};
enum Setup_Group_Item
{
	
	ACW_SETUP=0,
    DCW_SETUP,
    IR_SETUP,
	OS_SETUP,
    PA_SETUP
	

};
//上位机返回状态消息
enum UartMessageEnum
{
	W_PASS=0 , 
	W_STOP, 
	W_F_HI , 
	W_F_LO , 
	W_ARC,
	W_OVER,
	I_PASS,
	I_STOP,
	I_HIGH,
	I_LO,

};
typedef struct
{
	float xface;
	float yface;
	vu16 xoff;
	vu16 yoff;
	uint8_t passeord;

}Touch_SaveTypedef;
typedef struct
{
    vu16 x;
    vu16 y;

}
LcdAddr_TypeDef;

extern LcdAddr_TypeDef LcdAddr;

typedef struct 
{
    vu8 start;//头
    vu8 king;// a1 启动  a0 复位  a2 lock A3 save a4 pass  a5 fail a6lock=0 a7  save=0;
     
//    vu8 value;
//    vu8 dianya[5];//电压ascii
//    vu8 dianzu[6];
//    vu8 time[6];
//    vu8 comp;
    vu8 end;

}Send_Testvalue_Typedef;
extern Send_Testvalue_Typedef Send_Testvalue[2];
typedef struct 
{
    uint8_t    Range;
    uint32_t    Test_R;
    uint8_t    Test_R_Dot;  
    uint16_t    Test_V;
    uint16_t    Test_T;
    uint8_t    comp;
	uint32_t Clear_Time;


}Tft_5520_Disp_Typedef;
extern Tft_5520_Disp_Typedef Tft_5520_Dispvalue;
//数值框属性定义
typedef struct
{
	vu8 *Title;//标题
	vu16 Num;//数值
	vu8  Dot;//小数点(0-3: xxxx xxx.x xx.xx x.xxx)
	//vu8  Unit;//单位(0-2: MΩ  GΩ  TΩ)
	vu16 Max;//最大值
	vu16 Min;//最小值
}NumBox_TypeDef;
extern NumBox_TypeDef NumBox;
//绝缘电阻定义
typedef struct 
{
    uint16_t Speed;
    uint16_t voltage;
    uint16_t Delay_Time;
    uint16_t Hi_Open;
    uint16_t Lo_Open;
    uint16_t Range;
    uint16_t beep_Mode;
    uint16_t Hi_Limit;
    uint16_t Lo_Limit;
    uint16_t Short;
    
    

}Set_Value_Typedef;//主菜单设置
typedef struct 
{
    uint8_t Uart;
    uint8_t Buad;
    uint8_t U_Switch;
    uint8_t I_O;
    uint8_t Lang;

}Sys_Set_Typedef;

typedef struct
{
	vu16 standard;//标准显示值
	vu16 ad_value;

}Debug_Value_Typedef;
//校正值
typedef struct 
{
	vu16 Num;  //输出值
	vu16 NumAd;//输出A/D值
}Cal_TypeDef;
//校正项
typedef struct 
{
	Cal_TypeDef IRVol;//绝缘电压档  0  50V  1  500V
	Cal_TypeDef IRCur[4];// 0 1K采样电阻档 1  10K  2  100K  3  1000K   4 10M   5  80M 6 64M   7  128M
	
}Calibrate_TypeDef;
typedef struct 
{
    uint8_t Group;
    Set_Value_Typedef Set_Value[GROUP_MAX];
    Sys_Set_Typedef Sys_Set;
	Calibrate_TypeDef	Calibrate;
   // Debug_Value_Typedef Debugvalue[6];
	float Clear_buff[5];
	Touch_SaveTypedef	Touch_Save;//触摸屏矫正数据

}Tft_5520_Typedef;
extern Tft_5520_Typedef Tft_5520;
extern  vu8 StepIndex;//步骤项
extern vu8 TotalStep;//测试总步骤
extern const vu16 DefaultCalDaOut[][6];
extern const u8 CalibrateLimitOffset[];
extern  const vu16 DefaultCalibrate[];
extern  const u16 CalibrateLimit[][2];
//交流电阻数据定义
//带小数点和单位的数

typedef struct
{
	vu8 Res_dot;
	vu8 V_dot;
	vu8 V_Neg;
}Test_UnitTypedef;
extern Test_UnitTypedef Test_Unit;

typedef struct 
{
	vu8 sign;
	vu32 Num;
	vu32 Dot;
	vu32 Unit;
	vu32 Updata_flag;
	vu32 Order;
	vu32 mid_data;
}Sort_TypeDef;
typedef struct
{
	vu16 set_item;//测试项目
	vu16 set_time;//设置的测试时间
	vu16 set_ramp;//设置的缓升或延时时间
	float set_high;//设置的上限
	float set_low;//设置的下限
    vu16 set_drop;//设置的下限

}Test_mid_Typedef;
extern Test_mid_Typedef Test_mid;
typedef struct
{
	vu8 trip;//触发
	vu8 speed;//测试速度
	vu8 Res_comp;
	Sort_TypeDef Res_low;//电阻下限
	vu8 V_comp;
	Sort_TypeDef V_low;
	vu8 Range;
	vu8 beep;
	Sort_TypeDef Nominal_Res;
	Sort_TypeDef High_Res;
	Sort_TypeDef Nominal_V;
	Sort_TypeDef	V_high;
}Set_Data_Typedef;
typedef struct
{
	vu8 Year;
	vu8 Mon;
	vu8 data;

}Data_Value_Typedef;
typedef struct
{
	vu8 Hour;
	vu8 Min;
	vu8 Sec;

}Timer_Value_Typedef;
typedef struct
{
	vu8 uart;
	vu8 buard;
	vu8 U_store;
	vu8 plc;
	vu8 lanage;
	
	Timer_Value_Typedef Time;
	Data_Value_Typedef Data;
	//vu8 
	

}Sys_Setvalue_Typedef;
typedef struct 
{
	vu8 buff[8];
	vu8 flag;

} Out_Typedef;
typedef struct
{
	Set_Data_Typedef Set_Data;
	Debug_Value_Typedef Debug_Value[DEBUG_RANGE];
	Sys_Setvalue_Typedef Sys_Setvalue;
	float clear;
	float V_Clear;
	vu8 softswitch;
	Out_Typedef Production_Data;
	Out_Typedef Number;

}Save_Res_Typedef;

extern Save_Res_Typedef Save_Res;
enum Test_SampEnum
{
    V_SAMP=0,//电压采样
    I_SAMP ,//电流采样
    CALCULATE,//计算
    

};
enum Test_VEnum
{
    V_HSHORT=0,//电压高端短路检测
    V_LSHORT ,//电压低端短路检测
    V_TESTSAMP,//测试电压采样
    I_SHORT,//短路检测
    

};
enum Ptam
{
    AC=0,
    DC,
    IR,
    OS,
    PA

};


typedef struct
{
	vu8 buff[5];
	vu32 Num;
	vu32 Dot;
	vu32 Unit;


}Range_Set_Typedef;

//==========================================================




typedef struct
{
    Sort_TypeDef low;
    Sort_TypeDef high;

}Comp_Value_Typedef;
//极限列表设置
typedef struct
{
    vu32 Param;//参数
    Sort_TypeDef Nom;//标称
    vu32 Mode;//方式
    vu32 Aux;//附属
    vu32 Comp;//比较开关
    Comp_Value_Typedef Comp_Value[9];
    Comp_Value_Typedef Comp_Value_2nd;
	vu32 count;
}Limit_Tab_Typedef;

typedef struct
{
    vu32 Value;
    vu32 Unit; 
} Sweep_Value_Typedef;
typedef struct
{
    u32 Text_vot;
    u32 Text_value;
	u32 Text_time;
    u8 text_flag;
	u8 text_unit;

}Save_TestValue_Typedef;
extern Save_TestValue_Typedef Save_TestValue[];
typedef struct
{
	vu16 xpos;
	vu16 ypos;
	vu16 lenth;


}Disp_Coordinates_Typedef;

typedef struct
{
    float fftbuff[FFTLENTH];
    uint32_t num;
    uint32_t i;

}Fft_Cov_Typedef;
extern Fft_Cov_Typedef Fft_Cov;
extern Fft_Cov_Typedef Fft_Cov_slow;
typedef struct
{
    u16 num;
    u8 dot;
    u8 uint;
    

}UnitNum_typedef;
typedef struct
{
    u32 parameter;
    u32 V_out;
    u32 Upper;
//    u16 Upper;
    u32 time;
    u32 lower;
    u32 rise_time;
    u32 range_arc;
    u32 drop_time;
    u32 equa_last;
    u32 check;
    

}U9001_Setup_Typedef;

typedef struct
{
    u16 pass_num;
    u16 fail_num;
    u16 all_num;
    u8 precent;

}
U9001_Count_Typedef;
typedef struct
{
	u8 language;
    u8 pass_beep;
    u8 fail_beep;
    u8 key_beep;
    u8 beep;//蜂鸣器声音大小
	u8 password[8];
	u8 bussmode;
    u8 buss_addr;
    u8 buard;
	u8 date_bit;
	u8 stop_bit;
	u8 ord_bit;
    u8 pselect;//协议选择

}U9001_SYS_Typedef;
typedef struct
{
    u8 disp;
    u8 all_step;
    u8 current_step;
    
    U9001_Setup_Typedef  U9001_Setup[11];
    U9001_Count_Typedef  U9001_Count;
	u8 start_step;
	u16 clearvalue[10];

}U9001_save_Typedef;
//extern  U9001_save_Typedef U9001_Save_sys.U9001_save;
typedef struct 
{
	Cal_TypeDef AcwVol[1];//交流耐压电压值-1档
	Cal_TypeDef AcwCur[2];//交流耐压电流值-2档
	Cal_TypeDef DcwVol[1];//只流耐压电压值-1档
	Cal_TypeDef DcwCur[3];//直流耐压电流值-2档
	Cal_TypeDef IrVol[1];//绝缘电阻电压值-1档
	Cal_TypeDef IrCur[5];//绝缘电阻电流值-4档
	Cal_TypeDef GndVol[1];//接地电阻电压值-1档
	Cal_TypeDef GndCur[1];//接地电阻电流值-1档
	
}U9001_Calibrate_TypeDef;
extern U9001_Calibrate_TypeDef U9001_Calibrate;
typedef struct
{
    u16 pass_time;
    u16 step_time;
    u16 delay_time;
    u16 breakover;
    u16 hz;
    u16 ARC_mode;
    u16 comp_setp;
    u16 start_setp;
    u16 Autorange;
    u16 shock;
    u16 fail_mode;
    u16 rise_comp;
    u16 DC50;
    u16 clear;
    u16 dis_time;
    
}U9001_Testconfg_Typedef;
typedef struct 
{
   
    U9001_SYS_Typedef U9001_SYS;
    U9001_Testconfg_Typedef U9001_Testconfg;
	U9001_save_Typedef U9001_save;
	u8 jkflag;
	u8 version;
//    U9001_Calibrate_TypeDef U9001_Calibrate;
}U9001_Save_sysTypedef;
extern U9001_Save_sysTypedef U9001_Save_sys;
typedef struct
{
    u32 Vol;//显示电压
    u32 I_R;//显示电流或者电阻
    u16 Time;//显示时间
    u8 status;//测量状态

}Test_Value_Typedef;
extern Test_Value_Typedef Test_Value;
typedef struct 
{
	vu32 Test_v;	//测试电压(10-1000 V)
	vu8 dot;	//小数点位

}Test_VTypeDef;
typedef struct 
{
	vu16 Test_Res;	//测试电压(10-1000 V)
	vu8 dot;	//小数点位
	vu8 unit;

}Test_RESTypeDef;
typedef struct 
{
	Test_VTypeDef Test_v;	//测试电压(10-1000 V)
	vu16 Test_Time;	//测试时间(0-999.9s)
	Test_RESTypeDef Test_Res;	//测试电阻(xxxx MΩ)
	vu16 Test_I;

}Test_TypeDef;
extern Test_TypeDef Test_value;
//extern uint32_t  numBlks;
//extern uint8_t  inquiryResult[36];
extern uint8_t usb_oenflag;
extern uint8_t Group;
extern  uint8_t debug_saveflag;
extern uint8_t step;
extern uint8_t U_step;
extern uint8_t Range,VRange_flag;
extern u16 Test_Time;
//==========================================================
//系统项(2项)
//typedef struct 
//{
//	Sort_TypeDef Res_Hi;//上限(xxxx mΩ)
//	Sort_TypeDef Res_Lo;//下限(xxxx mΩ)
//	Sort_TypeDef Vol_Hi;//上限(xxxx V)
//	Sort_TypeDef Vol_Lo;//下限(xxxx V)
//}System_TypeDef;

//==========================================================
//配置项(3项)
//typedef struct 
//{
//	vu16 Password;//设置密码(0-9999，默认0)
//	vu16 BootNum;//开机次数(0-9999，默认0)
//	vu16 ProductId;//出厂编号(0-9999，默认0)
//}Config_TypeDef;

//==========================================================
//校正值
//typedef struct 
//{
//	vu16 Num;  //显示值
//	vu16 NumAd;//A/D值
//}Cal_TypeDef;

////校正项

//typedef struct 
//{
//	vu16 Real;  //实数
//	vu16 Imaginary;//虚数
//	vu16 DC_voltage;
//	vu8	 Dot;
//	vu8	 Negative;
//}Res_TypeDef;
//==========================================================
//保存信息
//typedef struct 
//{
//	Parameter_TypeDef Parameter;//参数项
//	System_TypeDef System;//系统项
//	Config_TypeDef Config;//配置项
//	Calibrate_TypeDef Calibrate;//校准项
//	vu16 Crc16;//校验字
//}Save_TypeDef;
//
////==========================================================
////默认校准值上下限
//extern const u16 CalibrateLimit[][2];
//extern const s16 BASE_NUM[6];
//extern const u8 DOT_POS[6];

//==========================================================
//全局变量
//extern  Save_TypeDef SaveData;//保存值
//extern  Cal_TypeDef Cal[6];//校准值
//extern  Res_TypeDef	Res;
//extern  vu8 Range;//当前量程
//extern  bool F_Password;//密码有效标志
//extern  bool F_100ms;//100ms定时标志
//extern  bool F_Fail;//测试失败标志
//extern  vu8 MenuIndex;//待机菜单项
//extern  vu8 MenuSelect;//菜单选择项
#define  FILENAME_R  "MSREAD.TXT"
#define  FILENAME_W  "ZC5520.TXT"
extern uint8_t textFileBuffer[50];
extern NumBox_TypeDef NumBox;
extern  unsigned long SystemStatus;//系统状态
extern unsigned long SystemMessage;//系统信息
extern char DispBuf[12];
extern unsigned long Count_buff[12];
extern vu8 Uart_Send_Flag;
extern u8 MenuSelect;//菜单选择项
extern uint32_t Usb_Open_flag;
extern uint32_t softswitch;
extern uint8_t Uart3_returnflag;
extern uint8_t Pc_Sendflag;
extern  uint32_t  rebuff_v[4];
extern float receive;
extern uint8_t  F_100ms;//100ms定时标志
extern uint8_t pageflag;
extern UnitNum_typedef test_value;
//extern  vu16 Voltage;//测试电压
//extern  vu16 Current;//测试电流
//extern  vu16 Resistance,xxxx;//测试电阻
//extern  NumBox_TypeDef NumBox;//数值框
//extern vu16 i_buff[FIT_INUM];		   //滤波值
//extern vu8 ffit_data1;
//extern	vu8	count_ffit;
//extern 	u16 	zreo_num[6];
//extern u8 fuhao;
//extern bool clear_flag;
//extern bool vol_flag;
//extern u32 MenuIndex;//待机菜单项
//extern u32 MenuSelect;//菜单选择项
//
//extern u32 SystemStatus;//系统状态
//extern u32 SystemMessage;//系统信息

//==========================================================
//系统状态


enum SysStatusEnum
{	
	SYS_STATUS_ERROR ,			//异常
	SYS_STATUS_RESET ,			//复位
	SYS_STATUS_POWER ,			//开机
//	SYS_STATUS_POWER ,
	SYS_STATUS_DATASTORE ,		//数据保存
	SYS_STATUS_FILE ,			//文件管理
	SYS_STATUS_SETUPTEST ,			//测量设置
	SYS_STATUS_TEST ,			//测试
	SYS_STATUS_RANGE ,		//档号显示
	SYS_STATUS_RANGECOUNT,	//档计数显示
	SYS_STATUS_ITEM ,			//列表扫描显示
	SYS_STATUS_USERDEBUG,//用户校正 
	SYS_STATUS_FACRDEBUG, //工厂校正
	SYS_STATUS_SYSSET,     //系统设置
    SYS_STATUS_LIMITSET,     //极限设置
    SYS_STATUS_ITEMSET,//列表扫描设置
	SYS_STATUS_TOOL,//工具
	SYS_STATUS_SYS,
    SYS_STATUS_IDEM,//等待测试
    SYS_STATUS_ABORT,//暂停测试
    SYS_STATUS_FINISH,//测试完成
	SYS_STATUS_CLEAR,//清零
	SYS_STATUS_SHORT,
	SYS_STATUS_HIOPEN,
    SYS_STATUS_TEST_PAUSE,
	SYS_STATUS_LOOPEN,
    SYS_STATUS_START
    
};
//==========================================================
//分选状态
enum SlectStatusEnum
{
	RH_FAIL=0,
	RL_FAIL,
	VH_FAIL,
	VL_FAIL,
	ALL_FAIL,
	ALL_PASS,
    
//    MSG_PAUSE
//	NO_COMP
	
};


//==========================================================
//系统消息
//enum SysMessageEnum
//{
//	MSG_CLEAR , 
//	MSG_IDLE , 
//	MSG_TEST , 
//	MSG_ABORT , 
//	MSG_PASS ,
////	MSG_FAIL ,
//	MSG_HIGH , 
//	MSG_LOW , 
//};
extern const uint8_t  RANGE_UNIT[12];
//==========================================================
//最大AD值
#define AD_MAX (8000) //12bit=4096-1
////最大电压AD值
//#define VOL_AD_MAX (AD_MAX-100)
////最大电流AD值
//#define CUR_AD_MAX (AD_MAX-100)

//量程上下限
#define RANGR_LIMIT_HIGH (2300)
#define RANGR_LIMIT_LOW  (190)
extern uint32_t clear_flag;
//==========================================================
//函数列表
void Check_Calibrate_Limit(void);
void InitGlobalValue(void);//全局变量初始化
void LoadDefaultSet(void);//加载默认设置值
void LoadDefaultParameter(void);//加载默认参数值
void LoadDefaultSystem(void);//加载默认系统值
void LoadDefaultConfig(void);//加载默认配置值
void LoadDefaultCalibrate(void);//加载默认校准值
void ReadSaveData(void);//读取保存参数
//void WriteSaveData(void);//写入保存参数
void SetDate_Comp(void);
void Savetoeeprom(void);
#define SetSystemStatus(status) (SystemStatus=status)
#define GetSystemStatus() (SystemStatus)
#define GetSystemMessage() (SystemMessage)
#define SetSystemMessage(msg) (SystemMessage=msg)

void Check_Calibrate_Limit(void);//校准值检查
void Hex_Format(vu32 dat , vu8 Dot , vu8 len , vu8 dispzero);
void SetSoftTimer(uint8_t  id, uint32_t ticks);
uint8_t GetSoftTimerOut(uint8_t id);
uint32_t Get_SorftTime(uint8_t id);
uint8_t Key_Read_WithTimeOut(uint8_t  ticks);
extern u16 cpld_value;
extern u8 EMC_High,Emc_low;
extern char sendbuff[20];
extern char sendbuff1[20];
extern u8 sendflag;
#endif
