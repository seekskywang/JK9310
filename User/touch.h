#ifndef _touch_H
#define _touch_H
#include <stdint.h>

#define TouchPanel_Enable 	 TouchPanel_nCS_L
#define TouchPanel_Disable 	 TouchPanel_nCS_H

#define TouchPanel_nCS_L      GPIO_ClearValue(0,(1<<20))
                              

#define TouchPanel_nCS_H      GPIO_SetValue(0,(1<<20))
                              
							  
#define _SPI_TOUCH_CSL    TouchPanel_nCS_L
#define _SPI_TOUCH_CSH    TouchPanel_nCS_H

//#define _SPI_TOUCH_DINL    GPIO_ResetBits(GPIOG,GPIO_Pin_14)
//#define _SPI_TOUCH_DINH    GPIO_SetBits(GPIOG,GPIO_Pin_14)

#define _SPI_TOUCH_CLKL  GPIO_ClearValue(2,(1<<27))
#define _SPI_TOUCH_CLKH  GPIO_SetValue(2,(1<<27))

#define _SPI_TOUCH_DOUTL  GPIO_ClearValue(0,(1<<19))
#define _SPI_TOUCH_DOUTH  GPIO_SetValue(0,(1<<19))

#define _SPI_TOUCH_DIN()	(GPIO_ReadValue(2)&(1<<21))
#define PEN					(GPIO_ReadValue(2)&(1<<25))

#define TP_PRES_DOWN 0x80  //����������	  
#define TP_CATH_PRES 0x40  //�а��������� 
#define CT_MAX_TOUCH  5    //������֧�ֵĵ���,�̶�Ϊ5��

//������������
typedef struct
{
	uint8_t (*init)(void);			//��ʼ��������������
	uint8_t (*scan)(uint8_t);				//ɨ�败����.0,��Ļɨ��;1,��������;	 
	void (*adjust)(void);		//������У׼ 
	uint16_t x[CT_MAX_TOUCH]; 		//��ǰ����
	uint16_t y[CT_MAX_TOUCH];		//�����������5������,����������x[0],y[0]����:�˴�ɨ��ʱ,����������,��
								//x[4],y[4]�洢��һ�ΰ���ʱ������. 
	uint8_t  sta;					//�ʵ�״̬ 
								//b7:����1/�ɿ�0; 
	                            //b6:0,û�а�������;1,�а�������. 
								//b5:����
								//b4~b0:���ݴ��������µĵ���(0,��ʾδ����,1��ʾ����)
/////////////////////������У׼����(����������ҪУ׼)//////////////////////								
	float xfac;					
	float yfac;
	short xoff;
	short yoff;	   
//�����Ĳ���,��������������������ȫ�ߵ�ʱ��Ҫ�õ�.
//b0:0,����(�ʺ�����ΪX����,����ΪY�����TP)
//   1,����(�ʺ�����ΪY����,����ΪX�����TP) 
//b1~6:����.
//b7:0,������
//   1,������ 
	uint8_t touchtype;
}_m_tp_dev;

	 	//������������touch.c���涨��
uint8_t TP_Init(void);
uint8_t TP_Scan(uint8_t tp);
void TP_Adjust(void);
uint8_t TP_Get_Adjdata(void);
void TP_Save_Adjdata(void);
void touch_init(void);
#endif 
