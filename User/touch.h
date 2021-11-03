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

#define TP_PRES_DOWN 0x80  //触屏被按下	  
#define TP_CATH_PRES 0x40  //有按键按下了 
#define CT_MAX_TOUCH  5    //电容屏支持的点数,固定为5点

//触摸屏控制器
typedef struct
{
	uint8_t (*init)(void);			//初始化触摸屏控制器
	uint8_t (*scan)(uint8_t);				//扫描触摸屏.0,屏幕扫描;1,物理坐标;	 
	void (*adjust)(void);		//触摸屏校准 
	uint16_t x[CT_MAX_TOUCH]; 		//当前坐标
	uint16_t y[CT_MAX_TOUCH];		//电容屏有最多5组坐标,电阻屏则用x[0],y[0]代表:此次扫描时,触屏的坐标,用
								//x[4],y[4]存储第一次按下时的坐标. 
	uint8_t  sta;					//笔的状态 
								//b7:按下1/松开0; 
	                            //b6:0,没有按键按下;1,有按键按下. 
								//b5:保留
								//b4~b0:电容触摸屏按下的点数(0,表示未按下,1表示按下)
/////////////////////触摸屏校准参数(电容屏不需要校准)//////////////////////								
	float xfac;					
	float yfac;
	short xoff;
	short yoff;	   
//新增的参数,当触摸屏的左右上下完全颠倒时需要用到.
//b0:0,竖屏(适合左右为X坐标,上下为Y坐标的TP)
//   1,横屏(适合左右为Y坐标,上下为X坐标的TP) 
//b1~6:保留.
//b7:0,电阻屏
//   1,电容屏 
	uint8_t touchtype;
}_m_tp_dev;

	 	//触屏控制器在touch.c里面定义
uint8_t TP_Init(void);
uint8_t TP_Scan(uint8_t tp);
void TP_Adjust(void);
uint8_t TP_Get_Adjdata(void);
void TP_Save_Adjdata(void);
void touch_init(void);
#endif 
