//==========================================================
//�ļ����ƣ�GlobalValue.h
//�ļ�������ȫ�ֱ���ͷ�ļ�
//�ļ��汾��Ver1.0
//�������ڣ�2015.10.26 
//�޸����ڣ�2015.10.28 15:38
//�ļ����ߣ�
//��ע˵������
//ע�������
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
//����汾��01-9310 02-9320 03-9320A
#define SOFTWARE_VERSION		U9001_Save_sys.version
//#define SOFTWARE_VERSION		(0x02)
//==========================================================
//Ĭ�ϳ������(0)
#define DEFAULT_MACHINE_ID 		(0)

//Ĭ�ϳ�������(0)
#define DEFAULT_PASSWORD 		(0)

//Ĭ�Ͽ�������(0)
#define SET_BOOT_DEFAULT		(0)

//==========================================================
//��������
#define _DEBUG_CONFIG_FALSE		(0)
#define _DEBUG_CONFIG_TRUE		(1)

//��ʱ������
#define KEYBOARD_SOFTTIMER	(0) //������ʱ��ʱ��
#define DISP_SOFTTIMER 		(1) //��ʾ��ʱ��ʱ��
#define TEST_SOFTTIMER 		(2) //������ʱ��ʱ��
#define PASS_SOFTTIMER      (3) //�ϸ���ʱ��ʱ��
#define DELAY_SOFTTIMER      (4) //�����ʱ��ʱ��
#define DIS_SOFTTIMER        (5) //�ŵ�ʱ����ʱ��
//==========================================================
//�������
//#define DEBUG_SUPPORT			(_DEBUG_CONFIG_TRUE)
#define DEBUG_SUPPORT			(_DEBUG_CONFIG_FALSE)

//==========================================================
//ISP�������ã����������
#if DEBUG_SUPPORT
	#define HW_ISP_SUPPORT		(_DEBUG_CONFIG_FALSE)
#else
	#define HW_ISP_SUPPORT		(_DEBUG_CONFIG_TRUE)
#endif

//==========================================================
//Uart��������
#define HW_UART_SUPPORT			(_DEBUG_CONFIG_TRUE)

//==========================================================
//��������������
#define HW_BEEP_SUPPORT			(_DEBUG_CONFIG_TRUE)

//==========================================================
//ADֵ�˲����ã����������ȶ���
#define AD_FILTER_SUPPORT		(_DEBUG_CONFIG_TRUE)

//==========================================================
#define SETUP_MENU_MAX  	(2)//������ò˵�

#define DEBUG_MENU_MAX  	(5)//���У׼�˵�

#define PARAMETER_MENU_MAX	(3)//�������˵�

#define SYSTEM_MENU_MAX		(4)//���ϵͳ�˵�

#define CONFIG_MENU_MAX		(3)//���������(0-2)

#define PW_LENGTH 			(4)//�������λ��

#define RANGE_MAX 			(4)//�������

#define AD_BUF_LENGTH		(8)//A/Dֵ����������

#define MAX_R_RANGE			(6)//�����������

#define BUTTOM_X_VALUE		(84)//��ʾ������ʾ��ʾ�ĵ�һ��Xλ��

#define BUTTOM_MID_VALUE	(80)//��ʾ������ʾ��ʾ���м���λ��

#define BUTTOM_Y_VALUE		(271-30)//��ʾ������ʾ��ʾ�ĵ�һ��Yλ��

#define KEY_NUM				(3)	//����������

#define NUM_LENTH 			(5)//��ʾ����
#define NUM_FREQ 			(33+4)//��ʾ����
#define DEBUG_RANGE			(4+1)
#define SET_MAX             (11)
#define SYS_MAX				(11)
#define GROUP_MAX           (5)
#define MAX_TEXT_STEP       (10)
#define MAXTSOFTTIMER (6)		//��ʱ����Ŀ

#define USBSENDLENTH        (27)

#define	CONFIG_TIME			(0x5a5a)
//==========================================================
#define OHM 				(0xF4)	//CGROM���Ц��ַ�������Ϊ0xF4

//==========================================================
//��ʾ������X����
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
#define I_SCAN_NUM      (97)    //������������

#define _4094NUM    2

#define LKEY1    5
#define LKEY2    15
#define LKEY3    26
//����ֵ���ֵ
//#define ACW_CURRENT_MAX		(2000)
//#define DCW_CURRENT_MAX		(2000)
#define ACW_CURRENT_MAX		(3000)
#define DCW_CURRENT_MAX		(3000)
#define IR_RESISTANCE_MAX	(250000)
#define GND_RESISTANCE_MAX	(310)
//����ֵ���
#define TEST_VALUE_OVER		(65535)
//��С��ѡʱ��--0.5s
#define SORT_TIME_MIN		(5)
///////////////////////////////////////////////////////////
//16bit���ݵ�λ��������
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
//#define HW_PRO_TEST			((u8)(SetD1))//�������ź�
//#define HW_PRO_UNTEST		((u8)(ClrD1))

//#define HW_FD_TEST			((u8)(SetD5))//����
//#define HW_FD_NOTEST		((u8)(~SetD5))//�ŵ�


//#define HW_GYB_100V 		((u8)(ClrD3))
//#define HW_GYB_1000V 		((u8)(SetD3))

////#define HW_GYB_1000VFEEDMASK ((u8)(ClrD2&ClrD1))
//#define HW_GYB_1000VFEED	((u8)(SetD0))
//#define HW_GYB_100VFEED		((u8)(ClrD0))


#define HW_GYBFD_ON			((u8)(SetD1))
#define HW_GYBFD_OFF		((u8)(ClrD1))
//==========================================================
//���ⳤ��
#define TITLE_LEN_MAX		(12)
extern  vu8 MenuIndex;//�����˵���
extern uint8_t Send_buff2[30];
extern uint32_t  rebuff_i[200];
extern uint32_t Clear_num;
extern uint8_t  u_adover_flag;
extern uint8_t  ry_flag,debug,Clear_flag;
extern uint32_t SoftTimer[MAXTSOFTTIMER];//��ʱ��
extern uint8_t V_Range;
extern uint8_t DISP_FLAG;
extern char U_Storebuff[50];
extern u8 _4094_databuff[_4094NUM];
extern char Resbuf[500];
extern const char TestPara[][8];
extern const char DISP_COMP[][2][15];
extern const char SEND_COMP[][2][15];
extern const u8 Test_IRUINT[][6];
extern bool f_disp;//��ʾ���±�־
//========================================================
extern uint8_t keyvalue,key_x;//����ֵ
extern uint32_t background;//����ɫ
extern vu8 RampStepTime;//��������ʱ��
extern vu16 RampStepYs;//������������
extern vu16 RampStepFd;//������������

extern  vu8 DropStepTime;//��������ʱ��
extern  vu16 DropStepYs;//������������
extern  vu16 DropStepFd;//������������

extern  vu16 TestOut;//�������ֵ
extern  vu16 FullOut;//�������ֵ
extern vu8 AdCount;//A/Dֵ��������
extern vu16 Current;//����
extern u16 rangelomit_low,rangr_limit_high;
extern vu16 Vbuf[AD_BUF_LENGTH];//��ѹA/Dֵ����
extern vu16 Ibuf[AD_BUF_LENGTH];//����A/Dֵ����
extern vu32 Resistance;//����
extern u8 save_flag;
extern bool F_Fail;//����ʧ�ܱ�־
extern int Voltage;//��ѹ
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
//��λ������״̬��Ϣ
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
    vu8 start;//ͷ
    vu8 king;// a1 ����  a0 ��λ  a2 lock A3 save a4 pass  a5 fail a6lock=0 a7  save=0;
     
//    vu8 value;
//    vu8 dianya[5];//��ѹascii
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
//��ֵ�����Զ���
typedef struct
{
	vu8 *Title;//����
	vu16 Num;//��ֵ
	vu8  Dot;//С����(0-3: xxxx xxx.x xx.xx x.xxx)
	//vu8  Unit;//��λ(0-2: M��  G��  T��)
	vu16 Max;//���ֵ
	vu16 Min;//��Сֵ
}NumBox_TypeDef;
extern NumBox_TypeDef NumBox;
//��Ե���趨��
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
    
    

}Set_Value_Typedef;//���˵�����
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
	vu16 standard;//��׼��ʾֵ
	vu16 ad_value;

}Debug_Value_Typedef;
//У��ֵ
typedef struct 
{
	vu16 Num;  //���ֵ
	vu16 NumAd;//���A/Dֵ
}Cal_TypeDef;
//У����
typedef struct 
{
	Cal_TypeDef IRVol;//��Ե��ѹ��  0  50V  1  500V
	Cal_TypeDef IRCur[4];// 0 1K�������赵 1  10K  2  100K  3  1000K   4 10M   5  80M 6 64M   7  128M
	
}Calibrate_TypeDef;
typedef struct 
{
    uint8_t Group;
    Set_Value_Typedef Set_Value[GROUP_MAX];
    Sys_Set_Typedef Sys_Set;
	Calibrate_TypeDef	Calibrate;
   // Debug_Value_Typedef Debugvalue[6];
	float Clear_buff[5];
	Touch_SaveTypedef	Touch_Save;//��������������

}Tft_5520_Typedef;
extern Tft_5520_Typedef Tft_5520;
extern  vu8 StepIndex;//������
extern vu8 TotalStep;//�����ܲ���
extern const vu16 DefaultCalDaOut[][6];
extern const u8 CalibrateLimitOffset[];
extern  const vu16 DefaultCalibrate[];
extern  const u16 CalibrateLimit[][2];
//�����������ݶ���
//��С����͵�λ����

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
	vu16 set_item;//������Ŀ
	vu16 set_time;//���õĲ���ʱ��
	vu16 set_ramp;//���õĻ�������ʱʱ��
	float set_high;//���õ�����
	float set_low;//���õ�����
    vu16 set_drop;//���õ�����

}Test_mid_Typedef;
extern Test_mid_Typedef Test_mid;
typedef struct
{
	vu8 trip;//����
	vu8 speed;//�����ٶ�
	vu8 Res_comp;
	Sort_TypeDef Res_low;//��������
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
    V_SAMP=0,//��ѹ����
    I_SAMP ,//��������
    CALCULATE,//����
    

};
enum Test_VEnum
{
    V_HSHORT=0,//��ѹ�߶˶�·���
    V_LSHORT ,//��ѹ�Ͷ˶�·���
    V_TESTSAMP,//���Ե�ѹ����
    I_SHORT,//��·���
    

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
//�����б�����
typedef struct
{
    vu32 Param;//����
    Sort_TypeDef Nom;//���
    vu32 Mode;//��ʽ
    vu32 Aux;//����
    vu32 Comp;//�ȽϿ���
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
    u8 beep;//������������С
	u8 password[8];
	u8 bussmode;
    u8 buss_addr;
    u8 buard;
	u8 date_bit;
	u8 stop_bit;
	u8 ord_bit;
    u8 pselect;//Э��ѡ��

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
	Cal_TypeDef AcwVol[1];//������ѹ��ѹֵ-1��
	Cal_TypeDef AcwCur[2];//������ѹ����ֵ-2��
	Cal_TypeDef DcwVol[1];//ֻ����ѹ��ѹֵ-1��
	Cal_TypeDef DcwCur[3];//ֱ����ѹ����ֵ-2��
	Cal_TypeDef IrVol[1];//��Ե�����ѹֵ-1��
	Cal_TypeDef IrCur[5];//��Ե�������ֵ-4��
	Cal_TypeDef GndVol[1];//�ӵص����ѹֵ-1��
	Cal_TypeDef GndCur[1];//�ӵص������ֵ-1��
	
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
    u32 Vol;//��ʾ��ѹ
    u32 I_R;//��ʾ�������ߵ���
    u16 Time;//��ʾʱ��
    u8 status;//����״̬

}Test_Value_Typedef;
extern Test_Value_Typedef Test_Value;
typedef struct 
{
	vu32 Test_v;	//���Ե�ѹ(10-1000 V)
	vu8 dot;	//С����λ

}Test_VTypeDef;
typedef struct 
{
	vu16 Test_Res;	//���Ե�ѹ(10-1000 V)
	vu8 dot;	//С����λ
	vu8 unit;

}Test_RESTypeDef;
typedef struct 
{
	Test_VTypeDef Test_v;	//���Ե�ѹ(10-1000 V)
	vu16 Test_Time;	//����ʱ��(0-999.9s)
	Test_RESTypeDef Test_Res;	//���Ե���(xxxx M��)
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
//ϵͳ��(2��)
//typedef struct 
//{
//	Sort_TypeDef Res_Hi;//����(xxxx m��)
//	Sort_TypeDef Res_Lo;//����(xxxx m��)
//	Sort_TypeDef Vol_Hi;//����(xxxx V)
//	Sort_TypeDef Vol_Lo;//����(xxxx V)
//}System_TypeDef;

//==========================================================
//������(3��)
//typedef struct 
//{
//	vu16 Password;//��������(0-9999��Ĭ��0)
//	vu16 BootNum;//��������(0-9999��Ĭ��0)
//	vu16 ProductId;//�������(0-9999��Ĭ��0)
//}Config_TypeDef;

//==========================================================
//У��ֵ
//typedef struct 
//{
//	vu16 Num;  //��ʾֵ
//	vu16 NumAd;//A/Dֵ
//}Cal_TypeDef;

////У����

//typedef struct 
//{
//	vu16 Real;  //ʵ��
//	vu16 Imaginary;//����
//	vu16 DC_voltage;
//	vu8	 Dot;
//	vu8	 Negative;
//}Res_TypeDef;
//==========================================================
//������Ϣ
//typedef struct 
//{
//	Parameter_TypeDef Parameter;//������
//	System_TypeDef System;//ϵͳ��
//	Config_TypeDef Config;//������
//	Calibrate_TypeDef Calibrate;//У׼��
//	vu16 Crc16;//У����
//}Save_TypeDef;
//
////==========================================================
////Ĭ��У׼ֵ������
//extern const u16 CalibrateLimit[][2];
//extern const s16 BASE_NUM[6];
//extern const u8 DOT_POS[6];

//==========================================================
//ȫ�ֱ���
//extern  Save_TypeDef SaveData;//����ֵ
//extern  Cal_TypeDef Cal[6];//У׼ֵ
//extern  Res_TypeDef	Res;
//extern  vu8 Range;//��ǰ����
//extern  bool F_Password;//������Ч��־
//extern  bool F_100ms;//100ms��ʱ��־
//extern  bool F_Fail;//����ʧ�ܱ�־
//extern  vu8 MenuIndex;//�����˵���
//extern  vu8 MenuSelect;//�˵�ѡ����
#define  FILENAME_R  "MSREAD.TXT"
#define  FILENAME_W  "ZC5520.TXT"
extern uint8_t textFileBuffer[50];
extern NumBox_TypeDef NumBox;
extern  unsigned long SystemStatus;//ϵͳ״̬
extern unsigned long SystemMessage;//ϵͳ��Ϣ
extern char DispBuf[12];
extern unsigned long Count_buff[12];
extern vu8 Uart_Send_Flag;
extern u8 MenuSelect;//�˵�ѡ����
extern uint32_t Usb_Open_flag;
extern uint32_t softswitch;
extern uint8_t Uart3_returnflag;
extern uint8_t Pc_Sendflag;
extern  uint32_t  rebuff_v[4];
extern float receive;
extern uint8_t  F_100ms;//100ms��ʱ��־
extern uint8_t pageflag;
extern UnitNum_typedef test_value;
//extern  vu16 Voltage;//���Ե�ѹ
//extern  vu16 Current;//���Ե���
//extern  vu16 Resistance,xxxx;//���Ե���
//extern  NumBox_TypeDef NumBox;//��ֵ��
//extern vu16 i_buff[FIT_INUM];		   //�˲�ֵ
//extern vu8 ffit_data1;
//extern	vu8	count_ffit;
//extern 	u16 	zreo_num[6];
//extern u8 fuhao;
//extern bool clear_flag;
//extern bool vol_flag;
//extern u32 MenuIndex;//�����˵���
//extern u32 MenuSelect;//�˵�ѡ����
//
//extern u32 SystemStatus;//ϵͳ״̬
//extern u32 SystemMessage;//ϵͳ��Ϣ

//==========================================================
//ϵͳ״̬


enum SysStatusEnum
{	
	SYS_STATUS_ERROR ,			//�쳣
	SYS_STATUS_RESET ,			//��λ
	SYS_STATUS_POWER ,			//����
//	SYS_STATUS_POWER ,
	SYS_STATUS_DATASTORE ,		//���ݱ���
	SYS_STATUS_FILE ,			//�ļ�����
	SYS_STATUS_SETUPTEST ,			//��������
	SYS_STATUS_TEST ,			//����
	SYS_STATUS_RANGE ,		//������ʾ
	SYS_STATUS_RANGECOUNT,	//��������ʾ
	SYS_STATUS_ITEM ,			//�б�ɨ����ʾ
	SYS_STATUS_USERDEBUG,//�û�У�� 
	SYS_STATUS_FACRDEBUG, //����У��
	SYS_STATUS_SYSSET,     //ϵͳ����
    SYS_STATUS_LIMITSET,     //��������
    SYS_STATUS_ITEMSET,//�б�ɨ������
	SYS_STATUS_TOOL,//����
	SYS_STATUS_SYS,
    SYS_STATUS_IDEM,//�ȴ�����
    SYS_STATUS_ABORT,//��ͣ����
    SYS_STATUS_FINISH,//�������
	SYS_STATUS_CLEAR,//����
	SYS_STATUS_SHORT,
	SYS_STATUS_HIOPEN,
    SYS_STATUS_TEST_PAUSE,
	SYS_STATUS_LOOPEN,
    SYS_STATUS_START
    
};
//==========================================================
//��ѡ״̬
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
//ϵͳ��Ϣ
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
//���ADֵ
#define AD_MAX (8000) //12bit=4096-1
////����ѹADֵ
//#define VOL_AD_MAX (AD_MAX-100)
////������ADֵ
//#define CUR_AD_MAX (AD_MAX-100)

//����������
#define RANGR_LIMIT_HIGH (2300)
#define RANGR_LIMIT_LOW  (190)
extern uint32_t clear_flag;
//==========================================================
//�����б�
void Check_Calibrate_Limit(void);
void InitGlobalValue(void);//ȫ�ֱ�����ʼ��
void LoadDefaultSet(void);//����Ĭ������ֵ
void LoadDefaultParameter(void);//����Ĭ�ϲ���ֵ
void LoadDefaultSystem(void);//����Ĭ��ϵͳֵ
void LoadDefaultConfig(void);//����Ĭ������ֵ
void LoadDefaultCalibrate(void);//����Ĭ��У׼ֵ
void ReadSaveData(void);//��ȡ�������
//void WriteSaveData(void);//д�뱣�����
void SetDate_Comp(void);
void Savetoeeprom(void);
#define SetSystemStatus(status) (SystemStatus=status)
#define GetSystemStatus() (SystemStatus)
#define GetSystemMessage() (SystemMessage)
#define SetSystemMessage(msg) (SystemMessage=msg)

void Check_Calibrate_Limit(void);//У׼ֵ���
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
