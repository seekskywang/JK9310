//==========================================================
//�ļ����ƣ�GlobalValue.c
//�ļ�������ȫ�ֱ�������
//�ļ��汾��Ver1.0
//�������ڣ�2015.10.28
//�޸����ڣ�2015.10.28 15:38
//�ļ����ߣ�
//��ע˵������
//ע�������
//==========================================================
#include "TypeDefine.h"	//��ֵ����
//#include "Beep.h"
#include "stdbool.h"
#include "Globalvalue/GlobalValue.h"
#include  "lpc_types.h"
#include "use_disp.h"
//#include


//==========================================================
//ȫ�ֱ���
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
vu8 MenuIndex;//�����˵���
uint32_t softswitch;
uint8_t usb_oenflag;
uint8_t debug_saveflag;
uint8_t Uart3_returnflag;
uint8_t Pc_Sendflag;
uint32_t SoftTimer[MAXTSOFTTIMER];//��ʱ��
uint8_t textFileBuffer[50];
uint8_t DISP_FLAG;
vu8 RampStepTime;//��������ʱ��
vu16 RampStepYs;//������������
vu16 RampStepFd;//������������

vu8 DropStepTime;//��������ʱ��
vu16 DropStepYs;//������������
vu16 DropStepFd;//������������
vu16 TestOut;//�������ֵ
vu16 FullOut;//�������ֵ
vu8 AdCount;//A/Dֵ��������
 vu16 Vbuf[AD_BUF_LENGTH];//��ѹA/Dֵ����
 vu16 Ibuf[AD_BUF_LENGTH];//����A/Dֵ����
vu16 Current;
 vu32 Resistance;//����
  u8 save_flag;
 int Voltage;//��ѹ
 bool F_Fail;//����ʧ�ܱ�־
u16 rangelomit_low,rangr_limit_high;
//  u8 run_stemp;//���в���   ���ӵ�ʱ����
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

 vu8 StepIndex;//������
 vu8 TotalStep;//�����ܲ���

//uint32_t  numBlks;
//uint8_t  inquiryResult[36];
//uint32_t MenuIndex;//�����˵���
//uint32_t MenuSelect;//�˵�ѡ����
//
//uint32_t SystemStatus;//ϵͳ״̬
//uint32_t SystemMessage;//ϵͳ��Ϣ
// Save_TypeDef SaveData;//����ֵ
// Cal_TypeDef Cal[6];//У׼ֵ
//
// NumBox_TypeDef NumBox;//��ֵ��
// Res_TypeDef	Res;
// bool F_Fail;//����ʧ�ܱ�־
// bool F_100ms;//100ms��ʱ��־
// bool F_Password;//������Ч��־
//
// vu8 Range;//��ǰ����
//
vu8 Uart_Send_Flag;
unsigned long Count_buff[12];
//unsigned long MenuIndex;//�����˵���
u8 MenuSelect;//�˵�ѡ����
//
 unsigned long SystemStatus;//ϵͳ״̬
unsigned long SystemMessage;//ϵͳ��Ϣ
Disp_Coordinates_Typedef Disp_Coordinates;
char DispBuf[12];
NumBox_TypeDef NumBox;
LcdAddr_TypeDef LcdAddr;
uint8_t V_Range;
char U_Storebuff[50];
u16 Test_Time;
const uint8_t  RANGE_UNIT[12]={	3,	2,	1,	0};
//==========================================================
//Ĭ��У׼���ֵ
const vu16 DefaultCalDaOut[][6]=
{
	 2200,550,550,0,0,0, //Acw-4kV
	 2400,362,604,0,0,0, //Dcw-4kV
	  604,604,604,604,604,604, //Ir -1kV
	 3000,0,0,0,0,0 //Gnd-25A
};
//==========================================================
//Ĭ��У׼����
const vu16 DefaultCalibrate[]=
{
	 3900, 2000, 1000,1211, 2000, 2210 ,//ACW
	 3900, 2200, 600,730, 2000, 2400 ,//DCW
	 1000, 552, 500,2400, 5000, 2400, 5000, 2400, 5000, 2400 ,5000, 2400 ,//IR
	 2500, 3000, 6000,3000 ,//GND
};
//==========================================================
//У׼ֵ�ϡ�����
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
//У׼ֵ�ϡ����޵�ַƫ����
const u8 CalibrateLimitOffset[]=
{
	0,
	3,
	3+3,
	3+3+5,
};
//==========================================================
//����Ĭ��У׼ֵ
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
	Savetoeeprom();//����У׼ֵ
}
//
// vu16 Voltage;//��ѹ
// vu16 Current;//����
// vu16 Resistance,xxxx;//����
//vu16 i_buff[FIT_INUM];		   //�˲�ֵ
//vu8	count_ffit;
//u16 	zreo_num[6];
//vu8 ffit_data1;
//u8 fuhao;
//bool clear_flag;
//bool vol_flag;
//==========================================================
//�������ƣ�InitGlobalValue
//�������ܣ���ʼ��ȫ�ֱ���
//��ڲ�������
//���ڲ�������
//�������ڣ�2015.10.28
//�޸����ڣ�2015.10.28 15:33
//��ע˵������
//==========================================================
void InitGlobalValue(void)
{
	vu16 len;
	vu8 * buf;
						 
	buf=(vu8 *)&Tft_5520;//�����׵�ַ
	len=sizeof(Tft_5520_Typedef);
	while(len--)
	{
		*buf=0;//���
		buf++;
	}
//	buf=(vu8 *)&SaveData.Limit_ScanValue;//�б�ɨ���ʼ��
//	len=sizeof(SaveData.Limit_ScanValue);
//	while(len--)
//	{
//		*buf=0xff;//���
//		buf++;
//	}
//	SaveData.Limit_ScanValue.fun=0;
//	//F_Password=FALSE;//������Ч��־	
    if(Tft_5520.Group>=GROUP_MAX)
        Tft_5520.Group=0;
    Uart3_returnflag=0;
}
//==========================================================
//�������ƣ�Check_Calibrate_Limit
//�������ܣ�У׼ֵ���
//��ڲ�������
//���ڲ�������
//�������ڣ�2015.01.14 
//�޸����ڣ�2015.08.27 14:33
//��ע˵�����ϡ����޼��,	L<=x<=H
//==========================================================
void Check_Calibrate_Limit(void)
{
	u8 i;
	u8 j;
	vu16 *pt;

	pt=(vu16 *)&U9001_Save_sys.U9001_Calibrate;
	j=sizeof(CalibrateLimit)/4;
	for(i=0;i<j;i++)//��j��
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
////�������ƣ�LoadDefaultSet
////�������ܣ�����Ĭ������ֵ
////��ڲ�������
////���ڲ�������
////�������ڣ�2015.10.28
////�޸����ڣ�2015.10.28 15:33
////��ע˵������
////==========================================================
//void LoadDefaultSet(void)
//{
//	LoadDefaultParameter();//����Ĭ�ϲ���
//	SaveParameter();//������������
//	
//	LoadDefaultSystem();//����Ĭ��ϵͳֵ
//	SaveSystem();//����ϵͳ����
//	
//	LoadDefaultConfig();//����Ĭ������ֵ
//	SaveConfig();//��������ֵ
//}

////==========================================================
////Ĭ�ϲ�����������
//const vu8 DefaultParameter[]=
//{
//	0,	//����-�Զ�
//	0,	//Ѷ��-�ر�
//	0,	//����-�ر�
//	0,	//�ⴥ��-�ر�
//	0,	//����-�ر�
//};

////���Ե���
//const s16 BASE_NUM[6]=
//{	
//	100,//1300,
//	0,//170,
//	0,//20,
//	0,//10,
//	0
//};
////���Ե���С����λ��
//const u8 DOT_POS[6]=
//{	
//	2,
//	1,
//	3,
//	2,
//	0
//};
////==========================================================
////����Ĭ�ϲ���
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
////Ĭ��ϵͳ��������
//const u8 DefaultSystem[]=
//{
//	0,200,1,0,	//��������-020.0m
//	0,100,2,0,	//��������-01.00m
//};
////==========================================================
////����Ĭ��ϵͳֵ
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
////����Ĭ������ ֵ
////==========================================================
//void LoadDefaultConfig(void)
//{
//	SaveData.Config.Password=DEFAULT_PASSWORD;//��������
//	SaveData.Config.BootNum=SET_BOOT_DEFAULT;//��������
//	SaveData.Config.ProductId=DEFAULT_MACHINE_ID;//�������
//}

////==========================================================
////Ĭ��У׼����
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
////У׼ֵ�ϡ�����
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
////����Ĭ��У׼ֵ
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
//	SaveCalibrate();//����У׼ֵ
//}

////==========================================================
////�������ƣ�Check_Calibrate_Limit
////�������ܣ�У׼ֵ���
////��ڲ�������
////���ڲ�������
////�������ڣ�2015.10.28
////�޸����ڣ�2015.10.28 15:38
////��ע˵�����ϡ����޼��,	L<=x<=H
////==========================================================
//void Check_Calibrate_Limit(void)
//{
////	u8 i;
//	u8 j;
//	vu16 *pt;

//	pt=(vu16 *)&SaveData.Calibrate;
//	j=sizeof(CalibrateLimit)/sizeof(CalibrateLimit[0]);
////	for(i=0;i<j;i++)//��j��
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
////�������ƣ�ReadSaveData
////�������ܣ���ȡ�������
////��ڲ�������
////���ڲ�������
////�������ڣ�2015.10.28
////�޸����ڣ�2015.10.28 15:38
////��ע˵������
////==========================================================
//void ReadSaveData(void)
//{
//#if DEBUG_SUPPORT
//	LoadDefaultSet();//����Ĭ������ֵ
//	LoadDefaultCalibrate();//����Ĭ��У׼ֵ
//#else
//	ReadParameter();//��ȡ���ò���
//	ReadSystem();//��ȡϵͳ����
//	ReadConfig();//��ȡ������Ϣ
//	ReadCalibrate();//��ȡУ׼ֵ
//#endif
//	
//	//���µ�ǰ����
//	if(SaveData.Parameter.Range==0)//�����Զ�
//		Range=0;
//	else
//		Range=SaveData.Parameter.Range-1;//ƫ����1

//}

//==========================================================
//�������ƣ�SetSoftTimer
//�������ܣ���ʱ������
//��ڲ�������ʱ��id,��ʱ����
//���ڲ�������
//�������ڣ�2014.09.29
//�޸����ڣ�2014.10.21 15:08
//��ע˵������
//==========================================================
void SetSoftTimer(uint8_t  id, uint32_t ticks)
{
	SoftTimer[id]=ticks;
}
//==========================================================
//�������ƣ�GetSoftTimerOut
//�������ܣ���ʱ�����
//��ڲ�������ʱ��id
//���ڲ�����True(��ʱ��)/False
//�������ڣ�2014.09.29
//�޸����ڣ�2014.09.29 11:18
//��ע˵������
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

