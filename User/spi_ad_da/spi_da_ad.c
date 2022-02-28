

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
//	GPIO_IntCmd(0, 1<<29, 1);//下降沿中断

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

void Send_value(uint8_t *buff)//往DA送数据
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
	
	GPIO_SetDir(3, (1<<27), GPIO_DIRECTION_OUTPUT);//放电


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
void Stop_Vout_vot(void)//高压停止输出
{
	pwm_intt(306);//120 500k
//    Vout_Freq(196);//输出500Hz
	GPIO_SetValue(1, (1<<0));
//    GPIO_SetBits(GPIOB,GPIO_Pin_2);//输出使能
    //GPIO_SetBits(GPIOA,GPIO_Pin_6);
	GPIO_SetValue(3, (1<<27));
	GPIO_SetValue(0, (1<<2));
//    GPIO_SetBits(GPIOB,GPIO_Pin_0);//放电

//    GPIO_ResetBits(GPIOG,GPIO_Pin_1);//还不知道是哪个脚控制的
    
    
//    Send_Vout(0);//电压输出


}
void Start_Vout_vot(uint16_t da)//高压电压输出
{
    uint32_t value;
   
//    pwm_intt(120);//输出500Hz
//    GPIO_ResetBits(GPIOB,GPIO_Pin_2);//输出使能
//    GPIO_ResetBits(GPIOA,GPIO_Pin_6);
    value=da;
    Send_Vout(value);//电压输出
    


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
    
    if(chip==1)//复位U212//电压
    {
        SPI_U_CS_LOW();
        SPI_I_CS_HIGH() ;
//        delay_ms(1);
        for(i=0;i<6;i++)
            SPI_ADDA_SendByte(0xff);    
//         delay_ms(1);   
        SPI_U_CS_HIGH() ;
    }
        
    else//复位U211
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
    SPI_ADDA_SendByte(0x28);  //端口控制寄存器     
    SPI_ADDA_SendByte(port);  //配置P1P2P3P0输出，P0 P1 P2输出1
//    SPI_DAA_CS_HIGH();


}
void AD_7192Config(uint32_t value)
{
   
    if(value==1)
    {
        SPI_U_CS_LOW();
//        delay_us(100);
        SPI_ADDA_SendByte(0x28);  //端口控制寄存器     
//        delay_us(1);
        SPI_ADDA_SendByte(0x37);  //配置P1P2P3P0输出，P0 P1 P2输出1
        SPI_ADDA_SendByte(0x28);  //端口控制寄存器 
        SPI_ADDA_SendByte(0x36);  //配置P1P2P3P0输出， P1 P2输出1  P2短路检测
        

        
        SPI_ADDA_SendByte(0x08);//模式寄存器
//        delay_us(1);        
        SPI_ADDA_SendByte(0x38);  //模式内部晶振
        SPI_ADDA_SendByte(0x8c); //04
        SPI_ADDA_SendByte(16); 
        
        
        SPI_ADDA_SendByte(0x10);//配置寄存器
        SPI_ADDA_SendByte(0);  //
        SPI_ADDA_SendByte(0x10); 
        SPI_ADDA_SendByte(8);
//        delay_us(10);
//        SPI_ADDA_SendByte(0x28);  //端口控制寄存器     
//        delay_us(1);
//        SPI_ADDA_SendByte(0x37);  //配置P1P2P3P0输出，P0 P1 P2输出1
//        SPI_ADDA_SendByte(0x28);  //端口控制寄存器 
//        SPI_ADDA_SendByte(0x36);  //配置P1P2P3P0输出， P1 P2输出1  P2短路检测
//        SPI_ADDA_SendByte(0x5C);
          SPI_U_CS_HIGH();
        
        
    }
    else
    {
        SPI_I_CS_LOW();
//        delay_us(100);
        
        SPI_ADDA_SendByte(0x08);//模式寄存器
        
        SPI_ADDA_SendByte(0x00);  //连续模式 外部晶振
        SPI_ADDA_SendByte(0x04); 
        SPI_ADDA_SendByte(0x01); //速度是4.8k
        
        
        SPI_ADDA_SendByte(0x10);//配置寄存器
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
    SPI_ADDA_SendByte(0x08);//模式寄存器
    SPI_ADDA_SendByte(0x98);  //模式内部晶振
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

void Out_4094(u8 *buff) //写CD4094子函数;
{
	u8 i;       //定义子函数;
	u8 j;
    u8 bux[2];
    bux[0]=*buff;
    bux[1]=*(buff+1);
   CD4094_CS(0);
    
	Set_Cpld_clk(0);
    //delay_us(4);
	for(j=0;j<_4094NUM;j++)
    
	{
		for(i=0;i<8;i++)     //写八位数据进CD4094;
		{

			if(*(bux+j)&0x80)     //判断并发串行数据;
                Set_Spi_out(1);
            else
                Set_Spi_out(0);
			
			Set_Cpld_clk(0);       //形成上升延脉冲;
			//delay_us(2);
			Set_Cpld_clk(1);
			*(bux+j)<<=1;      //数据移位,送下一位数据;
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
//函数名称：Read_MCP3202
//函数功能：通道 A/D转换
//入口参数：通道控制字
//出口参数：12位AD
//创建日期：2014.04.01
//修改日期：2014.04.01
//备注说明：数据方向配置1=MSB~LSB，也可配置成0:=LSB~MSB
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
	//发送5位通道控制字
	dat=ch;//通道控制字
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
	//读取低8位AD
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
//读取通道0的A/D值
//==========================================================
u16 Read_Ad_Ch0(void)
{
	return(Read_MCP3202(AD_CH0));
}

//==========================================================
//读取通道1的A/D值
//==========================================================
u16 Read_Ad_Ch1(void)
{
	return(Read_MCP3202(AD_CH1));
}

//==========================================================
//函数名称：Read_Ad
//函数功能：读取A/D值
//入口参数：无
//出口参数：无
//创建日期：2014.11.08 
//修改日期：2015.08.26 10:32
//备注说明：无
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
		
		vol+=Read_Ad_Ch1();//读取通道1的A/D值
        cur+=Read_Ad_Ch0();//读取通道0的A/D值
	}
	Voltage=(u16)(vol>>3);//累加平均
	Current=(u16)(cur>>3);//累加平均
}
//==========================================================
//函数名称：Ad_Filter
//函数功能：A/D值滤波
//入口参数：无
//出口参数：无
//创建日期：2014.11.08 
//修改日期：2015.08.26 10:35
//备注说明：Voltage，Current的AD值滤波处理
//==========================================================
void Ad_Filter(void)
{
	vu8 i;
	vu32 sum;

	static int LastVoltage;//上次值
	static int LastCurrent;//上次值

	if(AdCount<AD_BUF_LENGTH)//缓冲完成判别
		AdCount++;//AD值计数

	//左移更新缓冲，出一个进一个
	for(i=(AD_BUF_LENGTH-1);i>0;i--)
	{
		Vbuf[i]=Vbuf[i-1];
	}
	Vbuf[0]=Voltage;//更新最新值

	//取平均值
	sum=0;
	i=AdCount;
	while(i--)
	{
		sum+=Vbuf[i];//累加
	}
	sum/=AdCount;//平均
	Voltage=(u16)sum;
	
	//跳动范围内取上次值作为本次值
	if(abs(Voltage-LastVoltage)<2)//跳动范围判别
		Voltage=LastVoltage;
	else
		LastVoltage=Voltage;

	//左移更新缓冲，出一个进一个
	for(i=(AD_BUF_LENGTH-1);i>0;i--)
	{
		Ibuf[i]=Ibuf[i-1];
	}
	Ibuf[0]=Current;//更新最新值

	//取平均值
	sum=0;
	i=AdCount;
	while(i--)
	{
		sum+=Ibuf[i];//累加
	}
	sum/=AdCount;//平均
	Current=(u16)sum;
	
	//跳动范围内取上次值作为本次值
	if(abs(Current-LastCurrent)<2)//跳动范围判别
		Current=LastCurrent;
	else
		LastCurrent=Current;
}

