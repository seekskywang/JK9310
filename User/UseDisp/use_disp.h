#ifndef __use_disp_H_
#define __use_disp_H_
#include  "stdint.h"
#include  "Globalvalue/globalvalue.h"
#include  "LPC177x_8x.h"
#define TOP_BAR_HIGH	4
#define FIRSTLINE	26
#define FIRSTLINE_RETC	24
#define	SPACE1		24
#define LIST1		0+4
#define LIST2		184
#define SELECT_1END 192
#define SELECT_2END (400)

#define FASTBUTTON_START	380
#define FASTBUTTON_END		(480-4)
#define FASTBUTTON_HIGH		48

#define DATA_SPACE1			80

#define SCREENSTART 		0
#define	SCREEN_WIDTH		480
#define	SCREEN_HIGH			272

#define	DISP_R_X			100
#define	DISP_R_Y			110
#define	DISP_SPACE			40

#define	DISP_BIG_X1			10+5
#define	DISP_BIG_Y1			150+5


#define	DISP_SPACE1			40

#define BIG_HZ_HIGH			55
#define	BIG_HZ_W			40
#define MAXBUTTONNUM        5
#define LISTVALU_RETC       90
#define LISTVALUE1          100+20
#define LISTVALUE2_RETC      276
#define LISTVALUE2          280+20
extern const char DISP_COMP[][2][15];
typedef struct
{
    uint32_t res;
    uint8_t dot;
    uint8_t uint;
    uint8_t polar;
    

} Test_ValueTypedef;
extern const uint8_t COMP_DISP[][6];
//extern Test_ValueTypedef Test_Value;
extern const uint32_t FreqNum[];
extern const uint8_t Y_Button_Tip[][7+1];
extern const uint8_t R_Button_Tip[][7+1];
extern  const u8 ItemTab[][15];
void Disp_Fastbutton(uint32_t page);
void Disp_MidRect(void);
void Disp_TestScreen(void);

void Disp_Test_Item(void);
void Disp_Range_Item(void);
void Disp_Range_Count_Item(void);
void Disp_List_Count_Item(void);
void Disp_Test_Set_Item(void);
void Disp_UserCheck_Item(void);
void Disp_LimitList_Item(void);
void Disp_ListScan_Item(void);
void Disp_Sys_Item(uint8_t page);
//void Disp_Fastbutton(void);
void Disp_Button_value1(uint32_t value);
void Disp_Button_TestSet(uint32_t value);
void DispSet_value(uint8_t list);
void Disp_Test_value(uint8_t list);

void Disp_Debug_value(uint8_t list);
void Disp_SysSet_value(uint8_t list);
void Disp_button_Num_time(void);
void Disp_button_Num_Freq(void);
Sort_TypeDef Time_Set_Cov(Sort_TypeDef *Time);
Sort_TypeDef Freq_Set_Cov(Sort_TypeDef *Freq);
void Disp_button_Num_Avg(void);
void Disp_button_Num_Input(vu8 page);
Sort_TypeDef Input_Set_Cov(Sort_TypeDef *Input_Ref);
void Disp_Cp(void);
void Disp_Comp(void);
void Disp_CompTest(void);

void Disp_Cp_Rp(void);
void Disp_Cs_Rs(void);
void Disp_Cs_D(void);
void Disp_Cs_Q(void);
void Disp_Lp_Q(void);
void Disp_Lp_Rp(void);
void Disp_Lp_Rd(void);
void Disp_Lp_D(void);
void Disp_Lp_G(void);
void Disp_Ls_D(void);
void Disp_Ls_Q(void);
void Disp_Ls_Rs(void);
void Disp_Ls_Rd(void);
void Disp_Z_d(void);
void Disp_Z_r(void);
void Disp_Y_d(void);
void Disp_Y_r(void);
void Disp_R_X(void);
void Disp_Rs_Q(void);
void Disp_Rp_Q(void);
void Disp_big_HZ(uint8_t list);
//void Disp_Testvalue(int8_t chosen ,int32_t eee);//显示测试数据
void Disp_Big_MainUnit(vu8 unit,vu8 unit1);
void Disp_Big_SecondUnit(vu8 unit,vu8 unit1);

void UARTPutChar (LPC_UART_TypeDef *UARTx, uint8_t ch);
void Disp_open(void);
//测试时档号显示界面的显示

void Disp_RangeDispvalue(vu8 data);
void Disp_Range_DispMainUnit(vu8 num,vu8 unit);
void Disp_Range_DispSecondUnit(vu8 num,vu8 unit);
void Disp_RangeCount(void);

void Disp_Scan_Compvalue(vu8 set);

void Disp_Button_ItemScanSet(uint32_t value);
void Disp_Button_ItemScan_no(void);

void Send_Uart3(uint8_t *buff);
void Disp_Set_Unit_12(uint32_t unit,uint32_t x,uint32_t y);
void Disp_Set_Unit_16(uint32_t unit,uint32_t x,uint32_t y);
Sort_TypeDef Input_Set_CovPre(Sort_TypeDef *Input_Ref);
void Disp_Test_Unit_v(uint32_t x,uint32_t y);
void Disp_Test_Unit_mA(uint32_t x,uint32_t y);
void Send_UartClear(void);//清零命令
void Savetoeeprom(void);
void ReadSavedata(void);
void SaveCal(void);
void ReadCal(void);
void Disp_G_B(void);
void SetData_High_Low_Comp(void);
void SetDate_Comp(void);
void Disp_Sys(void);
void Disp_Sys_Item1(void);
void Disp_Debug_Reference(uint8_t list,uint16_t value);
int16_t Debug_Value(Sort_TypeDef *Debug_value);
void Disp_Range(uint8_t flag);
void Disp_Usbflag(uint8_t flag);
void Disp__(void);
int8_t Number_Setup(NumBox_TypeDef * pt);
void Send_Set_Value(void);
void Send_UartStop(void);
void Disp_Teststatue(uint8_t num);
void Disp_Time(void);
void Disp_Set_Time(uint8_t list);

void Disp_Testvalue(u8 test);

void Disp_Fastset_Disp(uint8_t num);
void Disp_mainitem(uint32_t item);
uint8_t  ListBox_Setup(NumBox_TypeDef * pt);
void Comp_Disp(float compvalue,uint32_t  hi,uint32_t lo);
uint8_t Touch_Test(uint8_t num);
uint8_t Touch_Setup(uint8_t num);
u32 Input_Num(u8 num,u8 mode);
uint8_t Touch_SYS_Setup(uint8_t num);

void Touch_SYS_D(void);
extern const uint8_t Test_Speedvalue[][15+1];
extern const uint8_t Test_Compvalue[][15+1];
extern const uint8_t User_Range[][15+1];
extern const uint8_t Setup_Beep[][15+1];
extern const uint8_t User_LimitList_Item2[][2+1];
extern	const uint8_t User_setRange[][15+1];
void dispSetupButtonvalue(u8 item,u8 list);
void Disp_Test_SetConfig_Item(void);
void DispSetConfig_value(uint8_t list);
void Disp_Idel_Item(void);
void Disp_Idel_list(void);
void Disp_Test_List(u8 i);
void Disp_IdelButton(void);
void Disp_SYS_Set_Item(void);
void dispSYSButtonvalue(void);
void DispSYSConfig_value(void);
void dispSYSSetButtonvalue(u8 list);
UnitNum_typedef IntToStr(u32 value);
UnitNum_typedef IntToStr_mA(u32 value);
UnitNum_typedef FToStr(u32 value);
UnitNum_typedef IntToStr_nF(u32 value);
void Disp_SetupConfig_Button(u8 list);
void DispSetup_ConfigButtonvalue(u8 list);
//u16 Read_Ad(u16 channel);
vu8 Range_Changecomp(void);
void ACW_Select(void);
void Short_out(u8 date);
void ARC_out(u8 start);
extern const u8 MAX_SETP[5];
void Test_Init(void);
void Get_Result(void);
void ACW_Select(void);//交流选择
void DCW_Select(void);
void OS_Select(void);//直流选择
void IR_Select(void);
void Get_Result(void);
Sort_TypeDef Disp_NumKeyboard_Set(Disp_Coordinates_Typedef *Coordinates );
void Disp_Pre_Uint(u8 value);//%
void disp_Inputnum(u8 *buff);
 u32 jisuandianzu(u16 bzdzn,u16 bzdzadn,u16 I_ad,u16 V_ad,u16 bzdy);
 void disp_TestMSG(u8 date);
 void Disp_DelayTime(u8 item);
#endif
