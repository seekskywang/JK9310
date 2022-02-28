

#include "lpc177x_8x_gpio.h"
#include "lpc177x_8x_pwm.h"
#include "TouchPanel/TouchPanel.h"
#include "spi_ad_da/spi_da_ad.h"
#include "pwm.h"
#include "cpld.h"
#include    "timer.h"
#include "Key/key.h"
#include    "math.h"

//void SPI_ADDA_GPIO(void)
//{
//	GPIO_SetDir(1, (1<<3), GPIO_DIRECTION_OUTPUT);//SPICLK
//	GPIO_SetDir(0, (1<<9), GPIO_DIRECTION_OUTPUT);//UAD_CS
//	GPIO_SetDir(0, (1<<8), GPIO_DIRECTION_OUTPUT);//IAD_CS
//	GPIO_SetDir(1, (1<<12), GPIO_DIRECTION_OUTPUT);//DA_CS
//	GPIO_SetDir(3, (1<<19), GPIO_DIRECTION_OUTPUT);//SPI_OUT
//	
//	GPIO_SetDir(0, (1<<29), GPIO_DIRECTION_INPUT);//SPI_IN
//	GPIO_IntCmd(0, 1<<29, 1);//�½����ж�

//}
void Set_inport(void)
{
	GPIO_SetValue(0, (1<<29));
}
//DA_CS
uint8_t SPI_ADDA_SendByte(uint8_t byte)
{
    uint8_t i;
    SPI3_CLK_HIGH() ;
//    delay_us(1);
    for(i=0;i<8;i++)
    {
//        delay_us(1);
        SPI3_CLK_LOW(); 
        if(byte&0x80)
            SPI3_MOSI_HIGH();
        else
            SPI3_MOSI_LOW();
        byte<<=1;
//        delay_us(1);
        SPI3_CLK_HIGH() ;
        
        
    
    
    }
    SPI3_MOSI_LOW();
    return 1;
}

uint8_t SPI3_Receivedata(void)
{
    uint8_t i;
    uint8_t data=0;
//    u8 return_data;
    
    SPI3_CLK_HIGH() ;
    Set_inport();
//    delay_us(1);
    //delay(10);
     for(i=0;i<8;i++)
    {   
        SPI3_CLK_LOW(); 
        data<<=1;
//        delay_us(1);
        //delay(10);
        if(SPI3_ADTA_IN()&0x20000000)
            data|=1;
        
        //data<<=1;
//        delay_us(1);
        //delay(10);
        SPI3_CLK_HIGH() ;
        //delay_us(2);
        
    
    
    }
    return data;
    

}

void Send_value(uint8_t *buff)//��DA������
{
    uint8_t i;
	SPI_U_CS_HIGH();
	SPI_I_CS_HIGH();
    SPI_DA_CS_LOW();
//    delay_ms(1);
//    SPI_ADDA_SendByte(0x55);
    for(i=0;i<3;i++)
    {
        SPI_ADDA_SendByte((*(buff+i)));
    
    }
//    delay_ms(1);
    SPI_DA_CS_HIGH();

}
void Send_Vout(uint32_t value)
{
    uint8_t buff[3];
    value=value<<4;
    
    buff[0]=0x30;
    buff[2]=value;
    value=value>>8;
    buff[1]=value;
    Send_value(buff);


}
void HV_Out_Gpioint(void)
{
	GPIO_SetDir(1, (1<<0), GPIO_DIRECTION_OUTPUT);//hv_clr
	GPIO_SetDir(0, (1<<2), GPIO_DIRECTION_OUTPUT);
	
	GPIO_SetDir(3, (1<<27), GPIO_DIRECTION_OUTPUT);//�ŵ�


}
void Discharge(void)
{
   GPIO_SetValue(2, (1<<22));
}
void Close_dischange(void)
{
    GPIO_ClearValue(0, (1<<2));


}
void Start_HV_Out(void)
{
	pwm_intt(120);
	GPIO_ClearValue(1, (1<<0));
	GPIO_ClearValue(3, (1<<27));
	GPIO_ClearValue(0, (1<<2));


}
void Stop_Vout_vot(void)//��ѹֹͣ���
{
	pwm_intt(306);//120 500k
//    Vout_Freq(196);//���500Hz
	GPIO_SetValue(1, (1<<0));
//    GPIO_SetBits(GPIOB,GPIO_Pin_2);//���ʹ��
    //GPIO_SetBits(GPIOA,GPIO_Pin_6);
	GPIO_SetValue(3, (1<<27));
	GPIO_SetValue(0, (1<<2));
//    GPIO_SetBits(GPIOB,GPIO_Pin_0);//�ŵ�

//    GPIO_ResetBits(GPIOG,GPIO_Pin_1);//����֪�����ĸ��ſ��Ƶ�
    
    
//    Send_Vout(0);//��ѹ���


}
void Start_Vout_vot(uint16_t da)//��ѹ��ѹ���
{
    uint32_t value;
   
//    pwm_intt(120);//���500Hz
//    GPIO_ResetBits(GPIOB,GPIO_Pin_2);//���ʹ��
//    GPIO_ResetBits(GPIOA,GPIO_Pin_6);
    value=da;
    Send_Vout(value);//��ѹ���
    


}

uint32_t Read_AD_7192(uint8_t value)
{
     uint32_t returnvalue=0;

    if(value==1)
    {
        returnvalue=SPI3_Receivedata();
        returnvalue<<=8;
        returnvalue|=SPI3_Receivedata();
        returnvalue<<=8;
       returnvalue|=SPI3_Receivedata();
        returnvalue<<=8;
        returnvalue|=SPI3_Receivedata();
    }
    else
    {
        returnvalue=0;
        returnvalue=SPI3_Receivedata();
        returnvalue<<=8;
        returnvalue|=SPI3_Receivedata();
        returnvalue<<=8;
        returnvalue|=SPI3_Receivedata();
    
    }
    return returnvalue;
}

void AD_7192_Rest(uint8_t chip)
{
    uint8_t i;
    
    if(chip==1)//��λU212//��ѹ
    {
        SPI_U_CS_LOW();
        SPI_I_CS_HIGH() ;
//        delay_ms(1);
        for(i=0;i<6;i++)
            SPI_ADDA_SendByte(0xff);    
//         delay_ms(1);   
        SPI_U_CS_HIGH() ;
    }
        
    else//��λU211
    {
        SPI_I_CS_LOW() ;
        SPI_U_CS_HIGH() ;
//        delay_ms(1);
        for(i=0;i<6;i++)
            SPI_ADDA_SendByte(0xff);   
//        delay_ms(1);
        SPI_I_CS_HIGH() ;
    
    }
}
void AD_7129_IO(uint8_t port)
{
    SPI_U_CS_LOW();
//    delay_us(20);
    SPI_ADDA_SendByte(0x28);  //�˿ڿ��ƼĴ���     
    SPI_ADDA_SendByte(port);  //����P1P2P3P0�����P0 P1 P2���1
//    SPI_DAA_CS_HIGH();


}
void AD_7192Config(uint32_t value)
{
   
    if(value==1)
    {
        SPI_U_CS_LOW();
//        delay_us(100);
        SPI_ADDA_SendByte(0x28);  //�˿ڿ��ƼĴ���     
//        delay_us(1);
        SPI_ADDA_SendByte(0x37);  //����P1P2P3P0�����P0 P1 P2���1
        SPI_ADDA_SendByte(0x28);  //�˿ڿ��ƼĴ��� 
        SPI_ADDA_SendByte(0x36);  //����P1P2P3P0����� P1 P2���1  P2��·���
        

        
        SPI_ADDA_SendByte(0x08);//ģʽ�Ĵ���
//        delay_us(1);        
        SPI_ADDA_SendByte(0x38);  //ģʽ�ڲ�����
        SPI_ADDA_SendByte(0x8c); //04
        SPI_ADDA_SendByte(16); 
        
        
        SPI_ADDA_SendByte(0x10);//���üĴ���
        SPI_ADDA_SendByte(0);  //
        SPI_ADDA_SendByte(0x10); 
        SPI_ADDA_SendByte(8);
//        delay_us(10);
//        SPI_ADDA_SendByte(0x28);  //�˿ڿ��ƼĴ���     
//        delay_us(1);
//        SPI_ADDA_SendByte(0x37);  //����P1P2P3P0�����P0 P1 P2���1
//        SPI_ADDA_SendByte(0x28);  //�˿ڿ��ƼĴ��� 
//        SPI_ADDA_SendByte(0x36);  //����P1P2P3P0����� P1 P2���1  P2��·���
//        SPI_ADDA_SendByte(0x5C);
          SPI_U_CS_HIGH();
        
        
    }
    else
    {
        SPI_I_CS_LOW();
//        delay_us(100);
        
        SPI_ADDA_SendByte(0x08);//ģʽ�Ĵ���
        
        SPI_ADDA_SendByte(0x00);  //����ģʽ �ⲿ����
        SPI_ADDA_SendByte(0x04); 
        SPI_ADDA_SendByte(0x01); //�ٶ���4.8k
        
        
        SPI_ADDA_SendByte(0x10);//���üĴ���
        SPI_ADDA_SendByte(0);  //
        SPI_ADDA_SendByte(0x10); 
        SPI_ADDA_SendByte(0x00); 
        SPI_ADDA_SendByte(0x5C);
//        delay_us(100);
        SPI_I_CS_HIGH();
    
    
    }
}

void U_adconfig(void)
{
	SPI_I_CS_HIGH();    
    SPI_U_CS_LOW();    
    SPI_ADDA_SendByte(0x08);//ģʽ�Ĵ���
    SPI_ADDA_SendByte(0x98);  //ģʽ�ڲ�����
    SPI_ADDA_SendByte(0x8c); //04
    SPI_ADDA_SendByte(16); 



}
void V_DA_out(u16 date)
{
	u8 i;
//	u16 value;
	DA_CS(0);
	delayMs(1,1);
	date<<=2;
	for(i=0;i<16;i++)
    {
        
        if(date&0x8000)
            Set_Spi_out(1);
        else
            Set_Spi_out(0);
        date<<=1;
//        delayMs(1,1);
        Set_Cpld_clk(1);
//        delayMs(1,1);
        Set_Cpld_clk(0);
        
    }
//    delayMs(1,1);
	
	DA_CS(1);
}
//u16 Read_Ad(u16 channel)
//{
//    u8 i;
//    u16 re=0;
//    AD_CS(0);
//    delayMs(1,1);
//    Set_Cpld_clk(1);
//    for(i=0;i<16;i++)
//    {
//        
//        if(channel&0x8000)
//            Set_Spi_out(1);
//        else
//            Set_Spi_out(0);
//        channel<<=1;
//        
//        if(GPIO_ReadValue(2)&(1<<26))
//            re+=1;
//        re<<=1;
////        delayMs(1,1);
//        Set_Cpld_clk(1);
////        delayMs(1,1);
//        Set_Cpld_clk(0);
//        
//    }
//    AD_CS(1);
//    re&=0xfff;
//    return re;

//}

void Out_4094(u8 *buff) //дCD4094�Ӻ���;
{
	u8 i;       //�����Ӻ���;
	u8 j;
    u8 bux[2];
    bux[0]=*buff;
    bux[1]=*(buff+1);
   CD4094_CS(0);
    
	Set_Cpld_clk(0);
    //delay_us(4);
	for(j=0;j<_4094NUM;j++)
    
	{
		for(i=0;i<8;i++)     //д��λ���ݽ�CD4094;
		{

			if(*(bux+j)&0x80)     //�жϲ�����������;
                Set_Spi_out(1);
            else
                Set_Spi_out(0);
			
			Set_Cpld_clk(0);       //�γ�����������;
			//delay_us(2);
			Set_Cpld_clk(1);
			*(bux+j)<<=1;      //������λ,����һλ����;
            //delay_us(2);
		}
	}
     //delay_us(2);
    CD4094_CS(1); 
     //delay_us(2);
    delayMs(1,1);
    CD4094_CS(0); 

}
//==========================================================
//�������ƣ�Read_MCP3202
//�������ܣ�ͨ�� A/Dת��
//��ڲ�����ͨ��������
//���ڲ�����12λAD
//�������ڣ�2014.04.01
//�޸����ڣ�2014.04.01
//��ע˵�������ݷ�������1=MSB~LSB��Ҳ�����ó�0:=LSB~MSB
//==========================================================
u16 Read_MCP3202(u16 ch)
{
	u8 i;
	u16 dat;
	u16 adz=0;
		

	AD_CS(0);
	Delay(8);
    Set_Cpld_clk(1);	//clk=0
    Delay(2);
	//����5λͨ��������
	dat=ch;//ͨ��������
	i=16;
	while(i--)
	{
        
        GPIO_SetValue(2,1<<26);
		if(dat&0x8000)
			Set_Spi_out(1);
		else
			Set_Spi_out(0);
        
        if(GPIO_ReadValue(2)&(1<<26))
			adz+=1;
        Delay(2);
		Set_Cpld_clk(0);	//clk=0
        Delay(2);

		Set_Cpld_clk(1);	//clk=1
		dat<<=1;
        adz<<=1;
	}
    Delay(2);
	//��ȡ��8λAD
//	adz=0;
//	i=12;
//	while(i--)
//	{
//		adz<<=1;
//        GPIO_SetValue(2,1<<26);
//        Delay(2);
////		_nop_();
//// 		MCP3202_DOUT=1;
//        
//		if(GPIO_ReadValue(2)&(1<<26))
//			adz+=1;
//		Set_Cpld_clk(0);	//clk=0
//        Delay(2);
////		_nop_();
////		_nop_();
//		Set_Cpld_clk(1);//clk=1
//	}
	AD_CS(1);	//cs=1
    adz&=0x0fff;
	
	return (adz);
}

//==========================================================
//��ȡͨ��0��A/Dֵ
//==========================================================
u16 Read_Ad_Ch0(void)
{
	return(Read_MCP3202(AD_CH0));
}

//==========================================================
//��ȡͨ��1��A/Dֵ
//==========================================================
u16 Read_Ad_Ch1(void)
{
	return(Read_MCP3202(AD_CH1));
}

//==========================================================
//�������ƣ�Read_Ad
//�������ܣ���ȡA/Dֵ
//��ڲ�������
//���ڲ�������
//�������ڣ�2014.11.08 
//�޸����ڣ�2015.08.26 10:32
//��ע˵������
//==========================================================
void Read_Ad(void)
{
	vu8 i;
	vu32 vol;
	vu32 cur;

	vol=0;
	cur=0;
	for(i=0;i<16;i++)
	{
		
		vol+=Read_Ad_Ch1();//��ȡͨ��1��A/Dֵ
        cur+=Read_Ad_Ch0();//��ȡͨ��0��A/Dֵ
	}
	Voltage=(u16)(vol>>3);//�ۼ�ƽ��
	Current=(u16)(cur>>3);//�ۼ�ƽ��
}
//==========================================================
//�������ƣ�Ad_Filter
//�������ܣ�A/Dֵ�˲�
//��ڲ�������
//���ڲ�������
//�������ڣ�2014.11.08 
//�޸����ڣ�2015.08.26 10:35
//��ע˵����Voltage��Current��ADֵ�˲�����
//==========================================================
void Ad_Filter(void)
{
	vu8 i;
	vu32 sum;

	static int LastVoltage;//�ϴ�ֵ
	static int LastCurrent;//�ϴ�ֵ

	if(AdCount<AD_BUF_LENGTH)//��������б�
		AdCount++;//ADֵ����

	//���Ƹ��»��壬��һ����һ��
	for(i=(AD_BUF_LENGTH-1);i>0;i--)
	{
		Vbuf[i]=Vbuf[i-1];
	}
	Vbuf[0]=Voltage;//��������ֵ

	//ȡƽ��ֵ
	sum=0;
	i=AdCount;
	while(i--)
	{
		sum+=Vbuf[i];//�ۼ�
	}
	sum/=AdCount;//ƽ��
	Voltage=(u16)sum;
	
	//������Χ��ȡ�ϴ�ֵ��Ϊ����ֵ
	if(abs(Voltage-LastVoltage)<2)//������Χ�б�
		Voltage=LastVoltage;
	else
		LastVoltage=Voltage;

	//���Ƹ��»��壬��һ����һ��
	for(i=(AD_BUF_LENGTH-1);i>0;i--)
	{
		Ibuf[i]=Ibuf[i-1];
	}
	Ibuf[0]=Current;//��������ֵ

	//ȡƽ��ֵ
	sum=0;
	i=AdCount;
	while(i--)
	{
		sum+=Ibuf[i];//�ۼ�
	}
	sum/=AdCount;//ƽ��
	Current=(u16)sum;
	
	//������Χ��ȡ�ϴ�ֵ��Ϊ����ֵ
	if(abs(Current-LastCurrent)<2)//������Χ�б�
		Current=LastCurrent;
	else
		LastCurrent=Current;
}

