#include "touch.h"
#include "Key/key.h"
#include <stdlib.h>
#include <math.h>
#include "LCD/lpc177x_8x_lcd.h"
#include "EX_SDRAM/EX_SDRAM.H"
#include "lpc177x_8x_ssp.h"
#include "lpc177x_8x_clkpwr.h"
#include "lpc177x_8x_pinsel.h"
#include "lpc177x_8x_gpio.h"
#include "GlobalValue.h"
#include "use_disp.h"
//Ĭ��Ϊtouchtype=0������.
u8 CMD_RDY=0XD0;
u8 CMD_RDX=0X90;
char* const TP_REMIND_MSG_TBL="Please use the stylus click the cross on the screen.The cross will always move until the screen adjustment is completed.";

//static void Touch_NVIC_Configuration(void)
//{
//    NVIC_InitTypeDef NVIC_InitStructure; 
//    // �����ж���Ϊ0
//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
//		// �����ж���Դ
//    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn; 	
//		// ������ռ���ȼ�
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	 
//	  // ���������ȼ�
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
//	
//}
void SPI_Touch_Init(void)
{
	PINSEL_ConfigPin(0, 20, 0);//CS
	GPIO_SetDir (0,(1<<20),1);
	
	PINSEL_ConfigPin(2, 25, 0);//IRQ
    GPIO_SetDir (2,(1<<25),0);
    TouchPanel_nCS_H;
	PINSEL_ConfigPin(2, 27, 0);//CLK
	GPIO_SetDir (2,(1<<27),1);
	
	PINSEL_ConfigPin(0, 19, 0);//DIN
	GPIO_SetDir (0,(1<<19),1);
	
	PINSEL_ConfigPin(2, 21, 0);//DOUT
	GPIO_SetDir (2,(1<<21),0);

}
void touch_init(void)
{

	SPI_Touch_Init();
 
}

void SPI_Touch_WriteByte(uint8_t dat)//��������SPIд����
{
	u8 count=0;   
	for(count=0;count<8;count++)  
	{ 	  
		if(dat&0x80)_SPI_TOUCH_DOUTH;  
		else _SPI_TOUCH_DOUTL;   
		dat<<=1;    
		_SPI_TOUCH_CLKL; 
		delay_us(1);
		_SPI_TOUCH_CLKH;		//��������Ч	        
	}	

}
//SPI������ 
//�Ӵ�����IC��ȡadcֵ
//CMD:ָ��
//����ֵ:����������	   
u16 TP_Read_AD(u8 CMD)	  
{ 	 
	u8 count=0; 	  
	u16 Num=0; 
	_SPI_TOUCH_CLKL;		//������ʱ�� 	 
	_SPI_TOUCH_DOUTL; 	//����������
	_SPI_TOUCH_CSL; 		//ѡ�д�����IC
	SPI_Touch_WriteByte(CMD);//����������
	delay_us(6);//ADS7846��ת��ʱ���Ϊ6us
	_SPI_TOUCH_CLKL; 	     	    
	delay_us(1);    	   
	_SPI_TOUCH_CLKH;		//��1��ʱ�ӣ����BUSY
	delay_us(1);    
	_SPI_TOUCH_CLKL; 	     	    
	for(count=0;count<16;count++)//����16λ����,ֻ�и�12λ��Ч 
	{ 				  
		Num<<=1; 	 
		_SPI_TOUCH_CLKL;	//�½�����Ч  	    	   
		delay_us(1);    
 		_SPI_TOUCH_CLKH;
 		if(_SPI_TOUCH_DIN())Num++; 		 
	}  	
	Num>>=4;   	//ֻ�и�12λ��Ч.
	_SPI_TOUCH_CSH;		//�ͷ�Ƭѡ	 
	return(Num);   
}
//��ȡһ������ֵ(x����y)
//������ȡREAD_TIMES������,����Щ������������,
//Ȼ��ȥ����ͺ����LOST_VAL����,ȡƽ��ֵ 
//xy:ָ�CMD_RDX/CMD_RDY��
//����ֵ:����������
#define READ_TIMES 5 	//��ȡ����
#define LOST_VAL 1	  	//����ֵ
u16 TP_Read_XOY(u8 xy)
{
	u16 i, j;
	u16 buf[READ_TIMES];
	u16 sum=0;
	u16 temp;
	for(i=0;i<READ_TIMES;i++)buf[i]=TP_Read_AD(xy);		 		    
	for(i=0;i<READ_TIMES-1; i++)//����
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])//��������
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}	  
	sum=0;
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];
	temp=sum/(READ_TIMES-2*LOST_VAL);
	return temp;   
} 

//��ȡx,y����
//��Сֵ��������100.
//x,y:��ȡ��������ֵ
//����ֵ:0,ʧ��;1,�ɹ���
u8 TP_Read_XY(u16 *x,u16 *y)
{
	u16 xtemp,ytemp;			 	 		  
	xtemp=TP_Read_XOY(CMD_RDX);
	ytemp=TP_Read_XOY(CMD_RDY);	  												   
	//if(xtemp<100||ytemp<100)return 0;//����ʧ��
	*x=xtemp;
	*y=ytemp;
	return 1;//�����ɹ�
}

//����2�ζ�ȡ������IC,�������ε�ƫ��ܳ���
//ERR_RANGE,��������,����Ϊ������ȷ,�����������.	   
//�ú����ܴ�����׼ȷ��
//x,y:��ȡ��������ֵ
//����ֵ:0,ʧ��;1,�ɹ���
#define ERR_RANGE 50 //��Χ 
u8 TP_Read_XY2(u16 *x,u16 *y) 
{
	u16 x1,y1;
 	u16 x2,y2;
 	u8 flag;    
    flag=TP_Read_XY(&x1,&y1);   
    if(flag==0)return(0);
    flag=TP_Read_XY(&x2,&y2);	   
    if(flag==0)return(0);   
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//ǰ�����β�����+-50��
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x=(x1+x2)/2;
        *y=(y1+y2)/2;
        return 1;
    }else return 0;	  
} 

//��LCD�����йصĺ���  
//��һ��������
//����У׼�õ�
//x,y:����
//color:��ɫ
void TP_Drow_Touch_Point(u16 x,u16 y,u32 color)
{
	//LCD_SetColors();
//	CurrentTextColor=color;
//	LCD_DrawLine(x-12,y,25,0);//����
	LCD_DrawLine( x-12, y, x+13, y ,color );
	LCD_DrawLine(x,y-12,x,y+13,color);//����
	LCD_SetPoint(x+1,y+1,color);
	LCD_SetPoint(x-1,y+1,color);
	LCD_SetPoint(x+1,y-1,color);
	LCD_SetPoint(x-1,y-1,color);
	LCD_DrawCircle(x,y,6,color);//������Ȧ
}

//��һ�����(2*2�ĵ�)		   
//x,y:����
//color:��ɫ
void TP_Draw_Big_Point(u16 x,u16 y,u32 color)
{	    
	LCD_SetPoint(x,y,color);//���ĵ� 
	LCD_SetPoint(x+1,y,color);
	LCD_SetPoint(x,y+1,color);
	LCD_SetPoint(x+1,y+1,color);	 	  	
}
_m_tp_dev tp_dev=
{
	TP_Init,
	TP_Scan,
	TP_Adjust,
	0,
	0, 
	0,
	0,
	0,
	0,	  	 		
	0,
	0,	  	 		
};
//��������ʼ��  		    
//����ֵ:0,û�н���У׼
//       1,���й�У׼
u8 TP_Init(void)
{
	touch_init();
 
		TP_Read_XY(&tp_dev.x[0],&tp_dev.y[0]);//��һ�ζ�ȡ��ʼ��	 
		if(TP_Get_Adjdata())return 0;//�Ѿ�У׼
		else			   //δУ׼?
		{ 										    
			lcd_Clear(LCD_COLOR_BLUE2);//����
			TP_Adjust();  	//��ĻУ׼ 
			//TP_Save_Adjdata();			
		}			
		TP_Get_Adjdata();	
	
	return 1; 									 
}

//////////////////////////////////////////////////////////////////////////	 
//������EEPROM����ĵ�ַ�����ַ,ռ��13���ֽ�(RANGE:SAVE_ADDR_BASE~SAVE_ADDR_BASE+12)
//#define SAVE_ADDR_BASE 40
//����У׼����										    
void TP_Save_Adjdata(void)
{
	//����У�����!		   							  
	Tft_5520.Touch_Save.xface=tp_dev.xfac*100000000;//����xУ������      
	Tft_5520.Touch_Save.yface=tp_dev.yfac*100000000;//����yУ������    
	//����xƫ����
	Tft_5520.Touch_Save.xoff=tp_dev.xoff;
	//����yƫ����
	Tft_5520.Touch_Save.yoff=tp_dev.yoff;
	//���津������
	Tft_5520.Touch_Save.passeord=0X0A;//���У׼����
	Savetoeeprom();
}

//�õ�������EEPROM�����У׼ֵ
//����ֵ��1���ɹ���ȡ����
//        0����ȡʧ�ܣ�Ҫ����У׼
u8 TP_Get_Adjdata(void)
{					  
	if(Tft_5520.Touch_Save.passeord==0X0A)//�������Ѿ�У׼����			   
	{    												 
		tp_dev.xfac=Tft_5520.Touch_Save.xface/100000000;//�õ�xУ׼����
		tp_dev.yfac=Tft_5520.Touch_Save.yface/100000000;//�õ�yУ׼����
	    //�õ�xƫ����
		tp_dev.xoff=Tft_5520.Touch_Save.xoff;			   	  
 	    //�õ�yƫ����
		tp_dev.yoff=Tft_5520.Touch_Save.yoff;				 	  
 		//tp_dev.touchtype=AT24CXX_ReadOneByte(SAVE_ADDR_BASE+12);//��ȡ�������ͱ��
		tp_dev.touchtype=1;
		if(tp_dev.touchtype)//X,Y��������Ļ�෴
		{
			CMD_RDX=0X90;
			CMD_RDY=0XD0;	 
		}else				   //X,Y��������Ļ��ͬ
		{
			CMD_RDX=0XD0;
			CMD_RDY=0X90;	 
		}		 
		return 1;	 
	}
	return 0;
}

//��������ɨ��
//tp:0,��Ļ����;1,��������(У׼�����ⳡ����)
//����ֵ:��ǰ����״̬.
//0,�����޴���;1,�����д���
u8 TP_Scan(u8 tp)
{//	u16 xx;		   
	if(PEN==0)//�а�������
	{
		//tp_dev.x[4]=GPIO_ReadInputData(GPIOH);
		if(tp)
			TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]);//��ȡ��������
		else if(TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0]))//��ȡ��Ļ����
		{
	 		tp_dev.x[0]=tp_dev.xfac*tp_dev.x[0]+tp_dev.xoff;//�����ת��Ϊ��Ļ����
			tp_dev.y[0]=tp_dev.yfac*tp_dev.y[0]+tp_dev.yoff;  
	 	} 
		if((tp_dev.sta&TP_PRES_DOWN)==0)//֮ǰû�б�����
		{		 
			tp_dev.sta=TP_PRES_DOWN|TP_CATH_PRES;//��������  
			tp_dev.x[4]=tp_dev.x[0];//��¼��һ�ΰ���ʱ������
			tp_dev.y[4]=tp_dev.y[0];  	   			 
		}			   
	}
	else
	{
		if(tp_dev.sta&TP_PRES_DOWN)//֮ǰ�Ǳ����µ�
		{
			tp_dev.sta&=~(1<<7);//��ǰ����ɿ�	
		}else//֮ǰ��û�б�����
		{
			tp_dev.x[4]=0;
			tp_dev.y[4]=0;
			tp_dev.x[0]=0xffff;
			tp_dev.y[0]=0xffff;
		}	    
	}
	return tp_dev.sta&TP_PRES_DOWN;//���ص�ǰ�Ĵ���״̬
}

//������У׼����
//�õ��ĸ�У׼����
void TP_Adjust(void)
{								 
	u16 pos_temp[4][2];//���껺��ֵ
	u8  cnt=0;	
	u16 d1,d2;
	u32 tem1,tem2;
	double fac; 	
	u32 outtime=0;
 	cnt=0;	
//	LCD_SetColors(LCD_COLOR_BLUE, LCD_COLOR_WHITE) ;	
//	LCD_Clear(LCD_COLOR_BLUE2);//����   
//	LCD_SetColors(LCD_COLOR_RED, LCD_COLOR_WHITE) ;
	Colour.Fword=White;
	Colour.black=LCD_COLOR_BLUE2;
	lcd_Clear(LCD_COLOR_BLUE2);//���� 
	//LCD_SetColors(LCD_COLOR_RED, LCD_COLOR_BLUE2) ;
	WriteString_16(40, 40, (u8*)TP_REMIND_MSG_TBL, 0);	
	TP_Drow_Touch_Point(20,20,LCD_COLOR_BLUE);//����1 
	tp_dev.sta=0;//���������ź� 
	tp_dev.xfac=0;//xfac��������Ƿ�У׼��,����У׼֮ǰ�������!�������	 
	while(1)//�������10����û�а���,���Զ��˳�
	{
		tp_dev.scan(1);//ɨ����������
		if((tp_dev.sta&0xc0)==TP_CATH_PRES)//����������һ��(��ʱ�����ɿ���.)
		{	
			outtime=0;		
			tp_dev.sta&=~(1<<6);//��ǰ����Ѿ����������.
						   			   
			pos_temp[cnt][0]=tp_dev.x[0];
			pos_temp[cnt][1]=tp_dev.y[0];
			cnt++;	  
			switch(cnt)
			{			   
				case 1:						 
					TP_Drow_Touch_Point(20,20,LCD_COLOR_BLUE2);				//�����1 
					TP_Drow_Touch_Point(LCD_PIXEL_WIDTH-20,20,LCD_COLOR_BLUE);	//����2
					break;
				case 2:
 					TP_Drow_Touch_Point(LCD_PIXEL_WIDTH-20,20,LCD_COLOR_BLUE2);	//�����2
					TP_Drow_Touch_Point(20,LCD_PIXEL_HEIGHT-20,LCD_COLOR_BLUE);	//����3
					break;
				case 3:
 					TP_Drow_Touch_Point(20,LCD_PIXEL_HEIGHT-20,LCD_COLOR_BLUE2);			//�����3
 					TP_Drow_Touch_Point(LCD_PIXEL_WIDTH-20,LCD_PIXEL_HEIGHT-20,LCD_COLOR_BLUE);	//����4
					break;
				case 4:	 //ȫ���ĸ����Ѿ��õ�
	    		    //�Ա����
					tem1=abs(pos_temp[0][0]-pos_temp[1][0]);//x1-x2
					tem2=abs(pos_temp[0][1]-pos_temp[1][1]);//y1-y2
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�1,2�ľ���
					
					tem1=abs(pos_temp[2][0]-pos_temp[3][0]);//x3-x4
					tem2=abs(pos_temp[2][1]-pos_temp[3][1]);//y3-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//�õ�3,4�ľ���
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05||d1==0||d2==0)//���ϸ�
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(LCD_PIXEL_WIDTH-20,LCD_PIXEL_HEIGHT-20,LCD_COLOR_WHITE);	//�����4
   	 					TP_Drow_Touch_Point(20,20,LCD_COLOR_RED);								//����1
 						//TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//��ʾ����   
 						continue;
					}
					tem1=abs(pos_temp[0][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[0][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�1,3�ľ���
					
					tem1=abs(pos_temp[1][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[1][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//�õ�2,4�ľ���
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//���ϸ�
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(LCD_PIXEL_WIDTH-20,LCD_PIXEL_HEIGHT-20,LCD_COLOR_WHITE);	//�����4
   	 					TP_Drow_Touch_Point(20,20,LCD_COLOR_RED);								//����1
 						//TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//��ʾ����   
						continue;
					}//��ȷ��
								   
					//�Խ������
					tem1=abs(pos_temp[1][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[1][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�1,4�ľ���
	
					tem1=abs(pos_temp[0][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[0][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//�õ�2,3�ľ���
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//���ϸ�
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(LCD_PIXEL_WIDTH-20,LCD_PIXEL_HEIGHT-20,LCD_COLOR_WHITE);	//�����4
   	 					TP_Drow_Touch_Point(20,20,LCD_COLOR_RED);								//����1
// 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//��ʾ����   
						continue;
					}//��ȷ��
					//������
					tp_dev.xfac=(float)(LCD_PIXEL_WIDTH-40)/(pos_temp[1][0]-pos_temp[0][0]);//�õ�xfac		 
					tp_dev.xoff=(LCD_PIXEL_WIDTH-tp_dev.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2;//�õ�xoff
						  
					tp_dev.yfac=(float)(LCD_PIXEL_HEIGHT-40)/(pos_temp[2][1]-pos_temp[0][1]);//�õ�yfac
					tp_dev.yoff=(LCD_PIXEL_HEIGHT-tp_dev.yfac*(pos_temp[2][1]+pos_temp[0][1]))/2;//�õ�yoff  
					if(fabs(tp_dev.xfac)>2||fabs(tp_dev.yfac)>2)//������Ԥ����෴��.
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(LCD_PIXEL_WIDTH-20,LCD_PIXEL_HEIGHT-20,LCD_COLOR_WHITE);	//�����4
   	 					TP_Drow_Touch_Point(20,20,LCD_COLOR_RED);							//����1
//						LCD_ShowString(40,26,lcddev.width,lcddev.height,16,"TP Need readjust!");
						tp_dev.touchtype=!tp_dev.touchtype;//�޸Ĵ�������.
						if(tp_dev.touchtype)//X,Y��������Ļ�෴
						{
							CMD_RDX=0X90;
							CMD_RDY=0XD0;	 
						}else				   //X,Y��������Ļ��ͬ
						{
							CMD_RDX=0XD0;
							CMD_RDY=0X90;	 
						}			    
						continue;
					}		
//					POINT_COLOR=BLUE;
//					CurrentTextColor=LCD_COLOR_BLUE;
//					LCD_Clear(LCD_COLOR_WHITE);//����
					WriteString_16(35,110,(u8*)"Touch Screen Adjust OK!",0);//У�����
					delay_ms(1000);
					TP_Save_Adjdata();  
 					lcd_Clear(LCD_COLOR_WHITE);//����   
					return;//У�����				 
			}
		}
//		delay_ms(10);
		outtime++;
		if(outtime>1000000)
		{
			TP_Get_Adjdata();
			break;
	 	} 
 	}
}
