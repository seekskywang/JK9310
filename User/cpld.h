#ifndef __cpld_H_
#define __cpld_H_

#define U211_574   ((volatile uint8_t *)	0x9C800000)
#define U212_574	((volatile uint8_t *)	0x9CA00000)
//#define HY271G_DATA  ((volatile uint8_t *)	0x90000000)
#define READ_START()			(GPIO_ReadValue(3)&(1<<20))
#define READ_STOP()			(GPIO_ReadValue(3)&(1<<19))
void CPLD_GPIO(void);
void FMC_Int(void);
void Write_U211_574(u8 date);
void Write_U212_574(u8 date);
void Sin_out(u8 date);
void DA_CS(u8 date);//0 选中  1 不选中
void AD_CS(u8 date);//0 选中  1 不选中
void Set_Spi_out(u8 date);
void Set_Cpld_clk(u8 date);
void CPLD_CS(u8 date);//0 选中  1 不选中;
void CD4094_CS(u8 date);//0 选中  1 不选中

void PLC_OutPass(void);
void PLC_OutFail(void);
void PLC_OutProg(void);
void PLC_OutOff(void);
void PLC_CompOff(void);
void Sing_out_C(u8 date);//1 频率输出  0 频率不输出
void Key_Start_Stop_GPIO(void);

void GFI_out(void);
void FRB_out(u8 date);//0  停止  1  AC  2 DC
void Sin_Clk_Enable(void);
void Sin_Clk_Disable(void);

#endif
