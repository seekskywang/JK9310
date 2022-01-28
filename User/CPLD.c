#include "lpc177x_8x_gpio.h"
#include "GlobalValue.h"
#include    "cpld.h"
#include    "timer.h"
#include "lpc177x_8x_emc.h"

void CPLD_GPIO(void)
{
	GPIO_SetDir(2, (1<<22), GPIO_DIRECTION_OUTPUT);//CPLD_CLK
	GPIO_SetDir(2, (1<<27), GPIO_DIRECTION_OUTPUT);//CPLD_DIN
    GPIO_SetDir(2, (1<<26), GPIO_DIRECTION_INPUT);//CPLD_DOUT
	GPIO_SetDir(0, (1<<8), GPIO_DIRECTION_OUTPUT);//IAD_CS
	GPIO_SetDir(1, (1<<12), GPIO_DIRECTION_OUTPUT);//DA_CS
//	GPIO_SetDir(3, (1<<19), GPIO_DIRECTION_OUTPUT);//SPI_OUT
	
	GPIO_SetDir(0, (1<<5), GPIO_DIRECTION_INPUT);//ARC
    GPIO_SetDir(0, (1<<6), GPIO_DIRECTION_INPUT);//SHORT
    GPIO_SetDir(0, (1<<7), GPIO_DIRECTION_INPUT);//GFI
//    
//	GPIO_IntCmd(0, 1<<5, 0);//p0_29 ???μ???D??
//    GPIO_IntCmd(0, 1<<6, 0);//p0_29 ???μ???D??
//    GPIO_IntCmd(0, 1<<7, 0);//p0_29 ???μ???D??
	NVIC_SetPriority(GPIO_IRQn, 0);
//	NVIC_EnableIRQ(GPIO_IRQn);
//	NVIC_DisableIRQ(GPIO_IRQn);
	
//	GPIO_IntCmd(0, 1<<5, 1);//下降沿中断
//    GPIO_IntCmd(0, 1<<6, 1);//下降沿中断
//    GPIO_IntCmd(0, 1<<6, 1);//下降沿中断
    GPIO_SetDir(3, (1<<28), GPIO_DIRECTION_OUTPUT);//FMQ
    Key_Start_Stop_GPIO();

}
void Key_Start_Stop_GPIO(void)
{
    GPIO_SetDir(3, (1<<19), GPIO_DIRECTION_INPUT);
    
    GPIO_SetDir(3, (1<<20), GPIO_DIRECTION_INPUT);

}
void FMC_Int(void)
{
	uint32_t i;
//	TIM_TIMERCFG_Type TIM_ConfigStruct;
	LPC_EMC_TypeDef *pEMC = LPC_EMC;
	/**************************************************************************
	* Initialize EMC for NAND FLASH
	**************************************************************************/
//	EMC_Init();

	pEMC->Control = EMC_Control_E;
	/*
	pEMC->Config = 0;
	pEMC->StaticExtendedWait = 150-1;

	pEMC->StaticConfig1 = 1 << 8;	// 使能扩展等待
	pEMC->StaticWaitWen1 = 0x1f;
	pEMC->StaticWaitOen1 = 0x1f;
	pEMC->StaticWaitRd1 = 0x1f;
	pEMC->StaticWaitPage1 = 0x1f;
	pEMC->StaticWaitWr1 = 0x1f;
	pEMC->StaticWaitTurn1 = 0x1f;
	*/

	EMC_StaMemConfigMW (3,EMC_StaticConfig_MW_8BITS);

	EMC_StaMemConfigPB(3,EMC_StaticConfig_PB);

	EMC_SetStaMemoryParameter(3, EMC_STA_MEM_WAITWEN, EMC_StaticWaitWen_WAITWEN(0x1f/*2*/));

	EMC_SetStaMemoryParameter(3, EMC_STA_MEM_WAITOEN, EMC_StaticWaitOen_WAITOEN(0/*2*/));

	EMC_SetStaMemoryParameter(3, EMC_STA_MEM_WAITRD, EMC_StaticWaitwr_WAITWR(6));

	EMC_SetStaMemoryParameter(3, EMC_STA_MEM_WAITPAGE, EMC_StaticwaitPage_WAITPAGE(0x1f));

	EMC_SetStaMemoryParameter(3, EMC_STA_MEM_WAITWR, EMC_StaticWaitwr_WAITWR(0x1f));

	EMC_SetStaMemoryParameter(3, EMC_STA_MEM_WAITTURN, EMC_StaticWaitTurn_WAITTURN(0x1f));

    CPLD_CS(1);
    DA_CS(1);
    AD_CS(1);

}
void Write_U211_574(u8 date)
{
    *U211_574=date;
}
void Write_U212_574(u8 date)
{
    *U212_574=date;
}

void Set_Cpld_clk(u8 date)
{
    if(date)
        GPIO_SetValue(2, (1<<22));
    else
        GPIO_ClearValue(2, (1<<22));

}
void Set_Spi_out(u8 date)
{
    if(date)
        GPIO_SetValue(2, (1<<27));
    else
        GPIO_ClearValue(2, (1<<27));
}
void CD4094_CS(u8 date)//0 选中  1 不选中
{
    if(date)
    {
        Emc_low|=_4094_STR_H;
    }
    else
    {
        Emc_low&=_4094_STR_L;
    
    }
    Write_U211_574(Emc_low);

}
void CPLD_CS(u8 date)//0 选中  1 不选中
{
    if(date)
    {
        Emc_low|=CPLD_CS_H;
    }
    else
    {
        Emc_low&=CPLD_CS_L;
    
    }
    Write_U211_574(Emc_low);

}
void Write_Cpld(u16 date)//往CPLD里面写数据
{
    u8 i;
    //选择CPLD
//    Set_Cpld_clk(0);
   CPLD_CS(0);
    delayMs(1,1);
    for(i=0;i<16;i++)
    {
        
        if(date&0x0001)
            Set_Spi_out(1);
        else
            Set_Spi_out(0);
        date>>=1;
        delayMs(1,1);
        Set_Cpld_clk(0);
        delayMs(1,1);
        Set_Cpld_clk(1);
        
    }
    delayMs(1,1);
     CPLD_CS(1);
    
    

}
void Sing_out_C(u8 date)//1 频率输出  0 频率不输出
{
    if(date==1)
    {
        cpld_value&=WAVE_OUT_ON;
    }else 
    {
        cpld_value|=WAVE_OUT_OFF;
    }
    Write_Cpld(cpld_value);
}
void Sin_Clk_Enable(void)//输出正弦波频率
{
    Sing_out_C(1);
}

void Sin_Clk_Disable(void)//输出正弦波频率
{
    Sing_out_C(0);
}
void Sin_out(u8 date)//0  50  1 60 2 400 写频率字
{
    cpld_value&=HW_50HZ;
     if(date==1)//60Hz
    {
        cpld_value|=HW_400HZ;
    }else if(date==2)//400Hz
    {
        cpld_value|=HW_60HZ;
    }else if(date==3)//50Hz
    {
        cpld_value&=HW_400HZ;
    }else if(date==4)//600Hz
    {
//        cpld_value&=HW_60HZ;
		cpld_value = 0x300;
    }
	
//    cpld_value=0X000F;
    Write_Cpld(cpld_value);
}

void GFI_out(void)//
{
    cpld_value&=HW_GFIVREF_0L;
    cpld_value&=HW_GFIVREF_1L;

//    cpld_value=0X000F;
    Write_Cpld(cpld_value);
}
void FRB_out(u8 date)//0  停止  1  AC  2 DC
{
    
    if(date==0)
    {
        cpld_value|=HW_ACDC_STOP;
        cpld_value|=HW_DC_OFF;
        cpld_value&=HW_AC_ON;
        cpld_value|=HW__5053_A_H;
    }else if(date==1)
    {
        cpld_value&=HW_ACDC_START;
//		cpld_value|=HW_DC_OFF;
//        cpld_value&=HW_AC_ON;
        cpld_value&=HW_DC_ON;
        cpld_value|=HW_AC_OFF;
        cpld_value&=HW__5053_A_L;
        
        
    }else if(date==2)
    {
        cpld_value&=HW_ACDC_START;
//		cpld_value&=HW_DC_ON;
//        cpld_value|=HW_AC_OFF;
        cpld_value|=HW_DC_OFF;
        cpld_value&=HW_AC_ON;
        cpld_value&=HW__5053_A_L;
    }else if(date==3)
    {
        cpld_value&=HW_ACDC_START;
//		cpld_value&=HW_DC_ON;
//        cpld_value|=HW_AC_OFF;
        cpld_value&=HW_DC_ON;
        cpld_value|=HW_AC_OFF;
        cpld_value|=HW__5053_A_H;
    }
 
    
    
    Write_Cpld(cpld_value);
}
void DA_CS(u8 date)//0 选中  1 不选中
{
    if(date)
    {
        Emc_low|=DA_CS_H;
    }
    else
    {
        Emc_low&=DA_CS_L;
    
    }
    Write_U211_574(Emc_low);

}

void AD_CS(u8 date)//0 选中  1 不选中
{
    if(date)
    {
        Emc_low|=AD_CS_H;
    }
    else
    {
        Emc_low&=AD_CS_L;
    
    }
    Write_U211_574(Emc_low);

}
void PLC_OutPass(void)
{
    EMC_High|=(PLC_FAIL_H|PLC_PROG_H);
    EMC_High&=(PLC_PASS_L);
    Write_U212_574(EMC_High);
}
void PLC_OutFail(void)
{
    EMC_High|=(PLC_PASS_H|PLC_PROG_H);
    EMC_High&=PLC_FAIL_L;
    Write_U212_574(EMC_High);
}
void PLC_OutProg(void)
{
    EMC_High|=(PLC_PASS_H|PLC_FAIL_H);
    EMC_High&=PLC_PROG_L;
    Write_U212_574(EMC_High);
}