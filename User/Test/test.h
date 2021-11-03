#ifndef __test_h
#define __test_h
#include "stdint.h"
#include  "Globalvalue/globalvalue.h"
#define  MAX_BUFFER_SIZE             (4000)
#define  WRITE_SIZE          (10 * 1000000)

#define BLINK_TICKS_SOFTTIMER (TICKS_PER_SEC_SOFTTIMER/3)
#define  FILENAME_b  "jk2520b.TXT"

extern const vu8 Disp_Main_Ord[][3];
extern Cal_TypeDef Cal[9];
void Power_Process(void);
void  Main_Read (void);


//���Գ���
void Test_Process(void);

//�ļ��������
void File_Process(void);

//�������ó���
void Setup_Process(void);
//���ݱ������
void Data_StoreProcess(void) ;
//������ʾ
void Range_Process(void);
//��������ʾ
void Range_CountProcess(void);
 // �б���ʾ
void ItemProcess(void);
//�û�У��
void Use_DebugProcess(void) ;
vu8 Uart_Process(void);
void Fac_DebugProcess(void);
void Sys_Process(void);//ϵͳ��Ϣ
void Use_SysSetProcess(void);//ϵͳ����
void Use_LimitSetProcess(void);//�����б�����
void Use_ITEMSetProcess(void);//�б�ɨ������
int8_t PackStandFrame(int8_t * framebuf, int8_t * datbuf, int8_t len);
Sort_TypeDef Disp_NumKeyboard_Set(Disp_Coordinates_Typedef *Disp_Coordinates);
Sort_TypeDef Disp_Set_Num(Disp_Coordinates_Typedef *Coordinates);
vu16 Freq_Set_Num(Disp_Coordinates_Typedef *Coordinates);
vu8 Avg_Set_Num(Disp_Coordinates_Typedef *Coordinates);//ƽ��������
Sort_TypeDef Disp_Set_InputNum(Disp_Coordinates_Typedef *Coordinates);
void Set_daot(vu8 *buff,vu8 dot);
Sort_TypeDef Disp_Set_InputpreNum(Disp_Coordinates_Typedef *Coordinates);
void Set_Compbcd_float(void);

Sort_TypeDef Disp_Set_CompNum(Disp_Coordinates_Typedef *Coordinates);
int32_t BCDtoInt(int8_t *pt);
int32_t VBCDtoInt(int8_t *pt);
int8_t V_Test_Comp(int32_t value);//��ѹ�Ƚ�
int8_t R_Test_Comp(int32_t value);//����Ƚ�
void Comp_prompt(int8_t value);//�Ƚ���ʾ
void BCD_Int(int32_t data);
float Debug_Res(float a,float b,float c);
void V_BCD_Int(int32_t data);
int32_t Input_int(int8_t *pt);
void  Write_Usbdata (volatile uint8_t  *buffer,uint32_t num );
void Soft_Turnon(void);//������ػ�
void Idem_Process(void);
void Abroat_Process(void);
void Finsh_Process(void);

float Fft_Dci(Fft_Cov_Typedef fft);
void Getberore_Res(void);
void Test_Debug(uint8_t V_Set);
void Change_Disp_beforer(float f);   //���ٻ���ʱ��
void Change_Disp_r(float f);  //�̶���λ
vu8 Range_Changecomp(void);
void Range_Control(uint8_t  dat);
void Test_cleardata(void);
extern uint32_t keynum;
void Setup_config_Process(void);
void Start_Process(void);
void TestPause_Process(void);
#endif
