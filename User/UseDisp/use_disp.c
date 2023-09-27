#include "LCD/lpc177x_8x_lcd.h"
#include"EX_SDRAM/EX_SDRAM.H"
#include "LCD/AsciiLib.h"
#include "use_disp.h"
#include  "stdint.h"
#include  "stdio.h"
#include "Key/key.h"
#include "test/Test.h"
#include  "Globalvalue/globalvalue.h"
#include "lpc177x_8x_eeprom.h"
#include "timer.h"
#include <math.h>
#include "lpc177x_8x_rtc.h"

#include  "usbhost_lpc17xx.h"
#include  "gui.h"
#include    "cpld.h"
#include    "math.h"
#include "spi_da_ad.h"
//#include "UseDisp/Use_Disp.h"
extern GUI_CONST_STORAGE GUI_FONT GUI_FontGUI_FONTXBFFON16;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontGUI_FONTXBFFON50;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHZMCRBLK16;
extern u16 ParameterLimit[][2];
Test_Value_Typedef Test_Value;
UnitNum_typedef test_value;
const u8 MAX_SETP[5]={9,11,10,5,5};
u32 savesize;
float RZ;
float Cvalue;
//=======================================================================
//项目
const u8 ItemTab[][15]=
{
	"交流耐压",
	"直流耐压",
	"绝缘电阻",
	"接地电阻",
	"关闭测试",
//	"泄漏电流",
};
const char Test_List_Tab[][2][16]=
{
    {"电压","Voltage"},//U9001_save_sys
    {"测量","Test "},
    {"结果","Res "},
};
const char SYS_Set_Tab[][2][16]=
{
    {"语言    ：","LANGUAGE"},//U9001_save_sys
	{"合格讯响：","PASSBEEP"},
	{"失败讯响：","FAILBEEP"},
	{"按键音  ：","BUTTONBEEP"},
    {"总线模式:","BUSMODE"},
    {"总线地址：","BUSADDR"},	
    {"波特率  ：","BAUDRATE"},
    
//    {"数据位数：","ON "},
//    {"报警音量：","ON "},
//    {"停止位数：","ON "},
//	 {"密码   ：","OFF"},   
//    {"奇偶校验：","ON "},
};
const char SYS_Disp_Tab[][2][16]=
{
		{"型号    ：","MODEL  :"},
//    {"名称    ：","NAME   :"},
    {"序列号  ：","SN     :"},
    {"固件版本：","VER    :"},
		{"公司网址：","WEBSITE:"},
    {"公司电话：","TEL    :"},
};
const char SYS_DispButton_Tab[][2][16]=
{
    {"系统设置","SYSSET"},
    {"系统信息","SYSINFO"},
    {""," "},
     {""," "},
     {""," "},

};
const char TEST_TIMEDISP[][2][16]=
{
    {"等待测试  ","IDLE     "},//0
    {"测试中... ","TEST...  "},//1
    {"暂停终止  ","ABORT    "},//2
    {"测试合格  ","PASS     "},//3
    {"电压上升  ","RAMP     "},//4
    
    {"电压下降  ","DROP     "},//5
    
    {"下限失败  ","LOFAIL  "},//6
    
    {"等待      ","WAIT    "},//7
    {"上限失败  ","HIFAIL  "},//8
    {"短路失败  ","SHORT   "},//9
    {"失败      ","FAIL    "},//10
    {"OVER      ","OVER    "},//11
    {"电弧失败  ","ARCFAIL "},//12
    {"暂停      ","PAUSE   "},//13
    {"GIF失败   ","GIFFAIL "},//14
		{"OFL失败	  ","OFLFAIL "},//15
		{"开路失败  ","OPENFAIL"},//16
};

const char DELAY_TIMEDISP[][2][16]=
{
    {"    "," "},
    {"    "," "},
    {"延时启动","OFF"},
    {"合格保持","ON "},
    {"步骤间隔","ON "},
    {"放电    ","ON "},
  
    {"    "," "},

};
const char VERSION_Tab[][2][32]=
{
    {"JK9310","JK9310"},
    {"JK9320","JK9320"},
		{"JK9320A","JK9320A"},
};

const char VERSION_Tab1[][2][32]=
{
    {"9310","9310"},
    {"9320","9320"},
		{"9320A","9320A"},
};

const char SYS_NAME_Tab[][2][32]=
{
//    {"耐压测试仪","HIPOT TESTER"},
    {"Ver.1.023","Ver.1.023"},
    {"www.JK17.com","www.JK17.com"},
    {"0519-85563477","0519-85563477"},

};
//1.006修正PLC
//1.007修改绝缘电阻测量上限(GlobalValue.h：IR_RESISTANCE_MAX：50000 -> 250000)
//1.008增加DC微安档位
//1.009修改耐压最小分选时间从0.2s到0.5s；修正100ms定时器不准的问题
//1.010直流耐压最大电流从5mA增加到10mA(9320&9320A)
//1.011修改IR模式上限随下限增加的bug
//1.012增加JK-1协议
//1.013修改绝缘连续测试分选判断bug
//1.014导通测量默认打开
//1.015上位机通讯debug
//1.016测试时间显示debug
//1.017列表界面上位机数据bug
//1.018SCPI协议设置数据仪器刷新
//1.019SCPI协议数据超范围处理
//1.020SCPI协议在步骤未全部完成的情况下失败停止后，停止步骤之后的未测步骤状态全部统一为失败步骤的状态
//1.021绝缘内阻SPCI返回数据的绝缘内阻值中，4档单位为G时放大倍数按5档单独处理
//1.022主参数绝缘电阻显示单位加上Ω显示
//1.023关闭GIF失败
const char SYS_SetTime_Tab[][2][15]=
{
	{"日期：","OFF"},
    {"时间：","ON "},
};
const char Setup_Tab[][11][2][14]=
{
    
	{{"步骤  :","STEP  :"},{"参数  :","FUNC  :"},{"电压  :","Voltage  :"},{"上限  :","Upper  :"},{"时间  :","TIME  :"},{"下限  :","Lower  :"},{"上升  :","RAMP  :"},{"电弧  :","ARC  :"},{"下降  :","DROP  :"}},//AC
	{{"步骤  :","STEP  :"},{"参数  :","FUNC  :"},{"电压  :","Voltage  :"},{"上限  :","Upper  :"},{"时间  :","TIME  :"},{"下限  :","Lower  :"},{"上升  :","RAMP  :"},{"电弧  :","ARC  :"},{"下降  :","DROP  :"},{"持续  :","HOLD  :"},{"检测  :","DECT  :"}},//DC
  {{"步骤  :","STEP  :"},{"参数  :","FUNC  :"},{"电压  :","Voltage  :"},{"上限  :","Upper  :"},{"时间  :","TIME  :"},{"下限  :","Lower  :"},{"上升  :","RAMP  :"},{"量程  :","ARC  :"},{"下降  :","DROP  :"},{"补偿  :","OFFSET  :"}},//IR
	{{"步骤  :","STEP  :"},{"参数  :","FUNC  :"},{"开路  :","OPEN  :"},{"标准  :","STD  :"},{"短路  :","SHORT  :"}},//OS
  {{"STEP  :","STEP  :"},{"FUNC  :","FUNC  :"},{"MESSAGE  :","MESSAGE  :"},{"UNSIGNAL  :","UNSIGNAL  :"},{"TIME  :","TIME  :"}}//PA

};

const char Idel_Tab[][11][2][14]=
{
    
	{{"步骤  :","STEP  :"},{"参数  :","FUNC  :"},{"电压  :","Voltage  :"},{"上限  :","Upper  :"},{"时间  :","TIME  :"},{"下限  :","Lower  :"},{"电弧  :","ARC  :"}},//AC
  {{"步骤  :","STEP  :"},{"参数  :","FUNC  :"},{"电压  :","Voltage  :"},{"上限  :","Upper  :"},{"时间  :","TIME  :"},{"下限  :","Lower  :"},{"电弧  :","ARC  :"}},//DC
  {{"步骤  :","STEP  :"},{"参数  :","FUNC  :"},{"电压  :","Voltage  :"},{"上限  :","Upper  :"},{"时间  :","TIME  :"},{"下限  :","Lower  :"}},//IR
  {{"步骤  :","STEP  :"},{"参数  :","FUNC  :"},{"开路  :","OPEN  :"},{"标准  :","STD  :"},{"短路  :","SHORT  :"}},//OS
	{{"STEP  :","STEP  :"},{"FUNC  :","FUNC  :"},{"MESSAGE  :","MESSAGE  :"},{"UNSIGNAL  :","UNSIGNAL  :"},{"TIME  :","TIME  :"}}//PA

};


const char SetupACButton_Tab[][2][5][14]=
{
    {{"测量设置","测量配置","系统设置","     ","        ",},{"DISP","CONFIG","SYSSET","     ","   "}},
    {{"  插入  ","  删除  ","  复制  ","  上步  ","  下步  "},{"  ADD  ","  DEL  ","  COPY  ","  PREV  ","  NEXT  "}},
    {{"   AC   ","   DC   ","   IR   ","   OS   ","   PA   "},{"   AC   ","   DC   ","   IR   ","   OS   ","   PA   "}},
    {{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","      "},{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","        "}},
    {{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","      "},{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","        "}},
    
    {{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","连续"},{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","CONST"}},
    
    
    {{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","关闭 "},{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","OFF "}},
    
     {{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","关闭 "},{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","OFF "}},
    {{"   ↑+ ","   ↓ - ","       ","          ","关闭 "},{"   ↑+ ","   ↓ - ","       ","     ","OFF "}},
    
    {{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","关闭 "},{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","OFF  "}},

};

const char SetupOSButton_Tab[][2][5][14]=
{
    {{"测量设置","测量配置","系统设置","   ","    "},{"DISP","CONFIG","SYSSET","     ","   "}},
    {{"  插入  ","  删除  ","  复制  ","  上步  ","  下步  "},{"  ADD  ","  DEL  ","  COPY  ","  PREV  ","  NEXT  "}},
    {{"   AC   ","   DC   ","   IR   ","   OS   ","   PA   "},{"   AC   ","   DC   ","   IR   ","   OS   ","   PA   "}},
    {{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","        "},{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","        "}},
    {{"  清除  ","  获取  ","       ","          ","        "},{"   CLEAR   "," GET ","    ","    ","        "}},
    {{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","        "},{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","        "}},
   
};

const char SetupPAButton_Tab[][2][5][14]=
{
	{{"测量设置","测量配置","系统设置","   ","    "},{"DISP","CONFIG","SYSSET","     ","   "}},
    {{"  插入  ","  删除  ","  复制  ","  上步  ","  下步  "},{"  ADD  ","  DEL  ","  COPY  ","  PREV  ","  NEXT  "}},
    {{"   AC   ","   DC   ","   IR   ","   OS   ","   PA   "},{"   AC   ","   DC   ","   IR   ","   OS   ","   PA   "}},
    {{"      ","      ","       ","        ","        "},{"    ","     ","      ","      ","        "}},
    {{"  关闭  ","  打开  ","       ","          ","        "},{"   OFF   "," ON ","    ","    ","        "}},
    {{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","  连续  "},{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","CONST"}},
   
};


const char SetupACButton_Tab1[][2][5][14]=
{
    {{"测量设置","测量配置","系统设置","     ","        ",},{"DISP","CONFIG","SYSSET","     ","   "}},
    {{"  插入  ","  删除  ","  复制  ","  上步  ","  下步  "},{"  ADD  ","  DEL  ","  COPY  ","  PREV  ","  NEXT  "}},
    {{"   AC   ","        ","        ","   OS   ","   PA   "},{"   AC   ","        ","        ","   OS   ","   PA   "}},
    {{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","      "},{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","        "}},
    {{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","      "},{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","        "}},
    
    {{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","连续"},{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","CONST"}},
    
    
    {{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","关闭 "},{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","OFF "}},
    
     {{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","关闭 "},{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","OFF "}},
    {{"   ↑+ ","   ↓ - ","       ","          ","关闭 "},{"   ↑+ ","   ↓ - ","       ","     ","OFF "}},
    
    {{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","关闭 "},{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","OFF  "}},

};

const char SetupOSButton_Tab1[][2][5][14]=
{
    {{"测量设置","测量配置","系统设置","   ","    "},{"DISP","CONFIG","SYSSET","     ","   "}},
    {{"  插入  ","  删除  ","  复制  ","  上步  ","  下步  "},{"  ADD  ","  DEL  ","  COPY  ","  PREV  ","  NEXT  "}},
    {{"   AC   ","        ","        ","   OS   ","   PA   "},{"   AC   ","        ","        ","   OS   ","   PA   "}},
    {{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","        "},{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","        "}},
    {{"  清除  ","  获取  ","       ","          ","        "},{"   CLEAR   "," GET ","    ","    ","        "}},
    {{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","        "},{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","        "}},
   
};

const char SetupPAButton_Tab1[][2][5][14]=
{
	{{"测量设置","测量配置","系统设置","   ","    "},{"DISP","CONFIG","SYSSET","     ","   "}},
    {{"  插入  ","  删除  ","  复制  ","  上步  ","  下步  "},{"  ADD  ","  DEL  ","  COPY  ","  PREV  ","  NEXT  "}},
    {{"   AC   ","        ","        ","   OS   ","   PA   "},{"   AC   ","        ","        ","   OS   ","   PA   "}},
    {{"      ","      ","       ","        ","        "},{"    ","     ","      ","      ","        "}},
    {{"  关闭  ","  打开  ","       ","          ","        "},{"   OFF   "," ON ","    ","    ","        "}},
    {{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","  连续  "},{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","CONST"}},
   
};


const char SysSetButton_Tab[][2][5][14]=
{
    {{"系统设置","系统信息","        ","        ","        ",},{"SYSSET","SYSINFO","      ","      ","      "}},
    {{"  中文  ","ENGLISH","        "," ","  "},{"  中文  ","ENGLISH","        "," ","  "}},
    {{"  连续  ","  间断  ","  关闭  ","  "," "},{"CONST","INT","OFF","  ","  "}},
    {{"  连续  ","  间断  ","  关闭  ","  "," "},{"CONST","INT","OFF","  ","  "}},
    {{"  关闭  ","  打开  ","   ","   ","        "},{" OFF ","  ON  ","    ","    ","        "}},
//    {{"  低音  ","  中音  ","  高音  ","   ","        "},{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","        "}},
//    {{"  关闭  ","  系统  ","  文件  ","  修改  ","        "},{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","        "}},
    {{"MODBUS","SCPI ","JK-1 ","    ","    "},{"MODBUS","SCPI ","JK-1 ","    ","    "}},
    {{"   ↑+ ","   ↓ - ","       ","          ","    "},{"   ↑+ ","   ↓ - ","       ","          ","  关闭  "}},
    {{"4800","9600","19200","38400","115200"},{"4800","9600","19200","38400","115200"}},
//    {{"8","7","       ","          ","    "},{"   ↑+ ","   ↓ - ","       ","          ","  关闭  "}},
//    {{"1 ","2","       ","          ","    "},{"   ↑+ ","   ↓ - ","       ","          ","  关闭  "}},
//    {{"无 ","奇","偶","          ","    "},{"   ↑+ ","   ↓ - ","       ","          ","  关闭  "}},
    


};
const char SetupDCButton_Tab[][2][5][14]=
{
    {{"测量设置","测量配置","系统设置","        ","        "},{"DISP","CONFIG","SYSSET","     ","   "}},
    {{"  插入  ","  删除  ","  复制  ","  上步  ","  下步  "},{"  ADD  ","  DEL  ","  COPY  ","  PREV  ","  NEXT  "}},
    {{"   AC   ","   DC   ","   IR   ","   OS   ","   PA   "},{"   AC   ","   DC   ","   IR   ","   OS   ","   PA   "}},
    {{"  ↑↑++  ","  ↑+  ","  ↓ -  ","  ↓↓--  ","        "},{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","        "}},
    {{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","        "},{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","        "}},
    {{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","  连续  "},{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","CONST"}},
    {{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","  关闭  "},{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","OFF"}},
    {{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","  关闭  "},{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","OFF"}},
    {{"   ↑+ ","   ↓ - ","       ","          ","  关闭  "},{"   ↑+ ","   ↓ - ","       ","          ","OFF  "}},
    {{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","  关闭  "},{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","OFF"}},
    {{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","  关闭  "},{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","OFF"}},
    {{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","  关闭  "},{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","OFF"}},
    
};

const char SetupIRButton_Tab[][2][5][14]=
{
    {{"测量设置","测量配置","系统设置","   ","    "},{"DISP","CONFIG","SYSSET","     ","   "}},
    {{"  插入  ","  删除  ","  复制  ","  上步  ","  下步  "},{"  ADD  ","  DEL  ","  COPY  ","  PREV  ","  NEXT  "}},
    {{"   AC   ","   DC   ","   IR   ","   OS   ","   PA   "},{"   AC   ","   DC   ","   IR   ","   OS   ","   PA   "}},
    {{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","        "},{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","        "}},
    {{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","  关闭  "},{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","OFF"}},
    {{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","  连续  "},{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","CONST"}},
    {{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","        "},{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","        "}},
    {{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","  关闭  "},{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","OFF"}},
    
    {{"   ↑+ ","   ↓ - "," 自动   ","          ","        "},{"   ↑+ ","   ↓ - ","AUTO","          ","     "}},
    {{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","  关闭  "},{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","OFF"}},
    {{"  关闭  ","  打开  ","       ","          ","        "},{"   OFF   "," ON ","    ","    ","        "}},

};

const char SetupConfig_Button_Tab[][2][5][14]=
{

    {{"测量设置","测量配置","系统设置","   ","        "},{"DISP","CONFIG","SYSSET","     ","   "}},
    {{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","  关闭  "},{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","OFF"}},
    {{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","  KEY  "},{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","  KEY  "}},
    {{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","        "},{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","        "}},
    {{"       ","       ","       ","          ","        "},{"   OFF   "," ON ","    ","    ","        "}},
    {{"  50Hz  ","  60Hz  ","       ","          ","        "},{"   50Hz   "," 60Hz ","    ","    ","        "}},
    {{"  电流  ","  等级  ","       ","          ","        "},{"   OFF   "," ON ","    ","    ","        "}},
    {{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","  关闭   "},{" ↑↑++ ","  ↑+  ","  ↓ -  ","  ↓↓--  ","  OFF   "}},
    {{"  关闭  ","  打开  ","       ","          ","        "},{"   OFF   "," ON ","    ","    ","        "}},
    {{"  关闭  ","  打开  ","       ","          ","        "},{"   OFF   "," ON ","    ","    ","        "}},
    {{"  关闭  ","  打开  ","       ","          ","        "},{"   OFF   "," ON ","    ","    ","        "}},
    {{"  停止  ","  继续  ","  重测  ","          ","        "},{"   OFF   "," ON ","    ","    ","        "}},
    {{"  关闭  ","  打开  ","       ","          ","        "},{"   OFF   "," ON ","    ","    ","        "}},
    {{"  关闭  ","  打开  ","       ","          ","        "},{"   OFF   "," ON ","    ","    ","        "}},
    {{"  关闭  ","  打开  "," 清零  ","          ","        "},{"   OFF   "," ON ","    ","    ","        "}},
    
    {{"   ↑+ ","   ↓ - ","    ","          ","        "},{"   ↑+ ","   ↓ - ","       ","          ","     "}},
 
};


const char Sys_SetValue[][2][5][14]=
{
    {{"中文","ENG"},{"中文","ENG"}},
    {{"连续","间断","关闭"},{"CONST","INTER","OFF"}},
    {{"连续","间断","关闭"},{"CONST","INTER","OFF"}},
    {{"关闭","打开"},{"OFF","ON"}},
    {{"低音","中音","高音"},{"LOW","MID","HIGH"}},
    {{"关闭","中音","高音"},{"OFF","MID","HIGH"}},
    {{"MODBUS","SCPI  ","JK-1  "},{"MODBUS","SCPI  ","JK-1  "}},
    {{"","",""},{"","",""}},//总线地址
    {{"4800","9600","19200","38400","115200"},{"4800","9600","19200","38400","115200"}},
    {{"8","7","19200","38400","115200"},{"8","7","19200","38400","115200"}},
    {{"1","2","19200","38400","115200"},{"1","2","19200","38400","115200"}},
    {{"无","奇","偶","38400","115200"},{"NONE","ODD","EVEN","38400","115200"}},
    
};
const char Switch_Tab[][2][8]=
{
    {"关闭","OFF"},
    {"打开","ON "}
};
const char FailMode_Tab[][2][8]=
{
    {"停止","OFF"},
    {"继续","ON "},
    {"重测","ON "},
    {"下步","ON "},
};
const char ArvMode_Tab[][2][8]=
{
    {"电流","OFF"},
    {"等级","ON "}
};
const char Setup_Range_Tab[][7][8]=
{
    {"自动","300nA","3uA","30uA","300uA","3mA","10mA"},{"AUTO","300nA","3uA","30uA","300uA","3mA","10mA"}
};
const char Continue_Tab[][2][8]=
{
    {"连续","CONT"}
   
};
const char TestPara[][8]=
{
    "AC","DC","IR","OS","PA"


};
const char FREQ[][8]=
{
    "50Hz",
    "60Hz"

};
const char IR_Uint[][8]=
{
    "MΩ","kΩ"
};
const char TEST_UNIT[][2][8]=
{
    {"uA","kΩ"},
    {"mA","MΩ"},
    {" A","GΩ"},

};
const char DISP_COMP[][2][15]=
{
    {"uA","kΩ"},//0
    {"mA","MΩ"},//1
    {" A","GΩ"},//2
    {"合格","PASS"},//3
    {"合格","PASS"},//4
    {"合格","PASS"},//5
    {"下限失败","LOFAIL"},//6
    
    {"下限失败","LOFAIL"},//7
    {"上限失败","HIFAIL"},//8
    {"短路失败","SHORT"},//9
    {"上限失败","HIFAIL"},//10
    {"上限失败","HIFAIL"},//11
    {"电弧失败","ARCFAIL"},//12
    {"上限失败","HIFAIL"},//13
    {"GIF失败","GIFFAIL"},//14
	{"待测","IDLE"}//15
};

const char SEND_COMP[][2][15]=
{
   {"等待测试","IDLE"},//0
    {"测试中... ","TEST"},//1
    {"暂停终止","ABORT"},//2
    {"测试合格","PASS"},//3
    {"电压上升","RAMP"},//4
    
    {"电压下降","DROP"},//5
    
    {"下限失败","LOFAIL"},//6
    
    {"等待","WAIT"},//7
    {"上限失败","HIFAIL"},//8
    {"短路失败","SHORT"},//9
    {"失败","FAIL"},//10
    {"OVER","OVER"},//11
    {"电弧失败","ARCFAIL"},//12
    {"暂停","PAUSE"},//13
    {"GIF失败","GIFFAIL"},//14
	{"OFL失败","OFLFAIL"},//15
	{"开路失败","OPENFAIL"},//16
};

const char SetupConfig_Tab[][2][15]=
{
    {"合格保持","PASSHOLD"},
    {"自动量程","AUTORANGE"},
    {"步骤间隔","STEPINTER"},
    {"触电保护","SHOCKPROT"},
    {"延时启动","STARTDELAY"},
    {"失败模式","FAILMODE"},
    {"导通测量","CONMEASURE"},
    {"上升判断","RAMPJUDGE"},
    {"交流频率","ACFREQ   "},
    {"DC50增益","DC50"},
    {"电弧模式","ARCMODE"},
    {"清零设置","CLEARSET"},
    {"判断步骤","STEPJUDGE"},   
    {"放电时间","DISTIME"},
    {"开始步骤","STARTSTEP"},
   

};

const char IdelButton_Tab[][2][15]=
{
    {"测量显示","DISP"},
    {"列表显示","LIST"},
    {"",""},
    {" ",""},
    {"统计显示","STAT"},

};
const char Disp_ButtonUint[][2][5][10]=//0 电压  1 时间  2 电流  3 电阻
{
   {{"V","KV","","","取消"}, {"V","KV","","","EXIT"}},
   {{"s","","","","取消"}, {"s","","","","EXIT"}},
   {{"uA","mA","","","取消"}, {"uA","mA","","","EXIT"}},
   {{"MΩ","GΩ","","","取消"}, {"MΩ","GΩ","","","EXIT"}},
   {{"nF","","","","取消"}, {"nF","pF","","","EXIT"}},
   {{"x1","","","","取消"}, {"x1","","","","EXIT"}},
};
//
const uint8_t Num_1[][9]=
    
{"1","2","3","4","5","6","7","8","9"};
const uint8_t TFT5520_state[][15+1]=
{
    {"等待测试"},
    {"测试中  "},
    {"暂停测试"},
    {"测试合格"},
    {"小于下限"},
    {"大于上限"},
    {"输出短路"},
    {"高端开路"},
    {"低端开路"},
    {"开路清零"},
    
};

const uint8_t Test_Setitem[][15+1]=
{
	{"组    别:"},
	{"电阻上限:"},
	{"电阻下限:"},
	{"量    程:"},
	{"输出电压:"},
	{"判定时间:"},
};
const uint8_t Test_Tirpvalue[][8+1]=
{
	{"自动"},
	{"手动"}

};
const uint8_t Test_Speedvalue[][15+1]=
{
	
	{"慢速"},
    {"快速"},
};
const uint8_t Test_Compvalue[][15+1]=
{
	{"关闭"},
	{"打开"}

};
//const uint8_t Sys_Disp_Main[][12+1]=
//{
//	{"型号        "},
//	{"仪器序列号  "},
//	{"仪器程序版本"},
//	{"操作系统    "},
//	{"OS版本      "},
//	{"信号处理器  "},
//	{"USB接口     "},
//	
//};
const uint8_t Sys_config_Disp_Main[][15+1]=
{
	{"语言     "},
//	{"讯响     "},
	{"串口开关 "},
	{"波特率   "},
	{"U盘开关  "},

};
	




const uint8_t Lp_Button_Tip2[][7+1]=  //测试参数选择时候的下面的提示符号
{
    {"Lp-D"},
    {"Lp-G"},
    {"    "},
    {"    "},
    {"RETURN "},

};





const uint8_t RangeDisp_Main[][3+1]=
{
	{"Cp:"},{"Cp:"},{"Cp:"},{"Cp:"},
	{"Cs:"},{"Cs:"},{"Cs:"},
	{"Lp:"},{"Lp:"},{"Lp:"},{"Lp:"},
	{"Ls:"},{"Ls:"},{"Ls:"},{"Ls:"},
	{"Z :"},{"Z :"},
	{"Y :"},{"Y :"},
	{"R :"},{"Rp:"},{"Rs:"},
	{"G :"}
	
};
const uint8_t RangeDisp_Second[][3+1]=
{
	{"D :"},{"Q :"},{"G :"},{"Rp:"},
	{"D :"},{"Q :"},{"rS:"},
	{"Q :"},{"Rp:"},{"Rd:"},{"D :"},
	{"D :"},{"Q :"},{"Rs:"},{"Rd:"},
	{"r :"},{"r :"},
	{"r :"},{"r :"},
	{"X :"},{"Q :"},{"Q :"},
	{"B :"}
};
const uint8_t Fun_SelectValue[4][7]=
{
	{0,4,	7,	11,	15,	17,	19},
	{1,5,	8,	12,	16,	18,	20},
	{2,6,	9,	13,	0xff,0xff,21},
	{3,0xff,10,	14,	0xff,0xff,0xff}


};
const uint32_t FreqNum[]=
	{20,25,30,40,50,60,75,100,120,150,200,250,300,400,500,600,750,
	1e3,1200,1500,2e3,2500,3e3,4e3,5e3,6e3,7500,
	1e4,12e3,15e3,2e4,25e3,3e4,4e4,5e4,6e4,75e3,
	1e5};
const uint8_t User_Freq[][10+1]=
{
	{"20.000 Hz"},
	{"25.000 Hz"},
	{"30.000 Hz"},
	{"40.000 Hz"},
    {"50.000 Hz"},
    {"60.000 Hz"},
    {"75.000 Hz"},
    {"100.00 Hz"},
    {"120.00 Hz"},
    {"150.00 Hz"},
    {"200.00 Hz"},
    {"250.00 Hz"},
    {"300.00 Hz"},
    {"400.00 Hz"},
    {"500.00 Hz"},
    {"600.00 Hz"},
    {"750.00 Hz"},
    {"1.0000KHz"},
    {"1.2000KHz"},
    {"1.5000KHz"},
    {"2.0000KHz"},
    {"2.5000KHz"},
    {"3.0000KHz"},
    {"4.0000KHz"},
    {"5.0000KHz"},
    {"6.0000KHz"},
    {"7.5000KHz"},
    {"10.000KHz"},
    {"12.000KHz"},
    {"15.000KHz"},
    {"20.000KHz"},
    {"25.000KHz"},
    {"30.000KHz"},
    {"40.000KHz"},
    {"50.000KHz"},
    {"60.000KHz"},
    {"75.000KHz"},
    {"100.00KHz"},
	{"120.00KHz"},
	{"150.00KHz"},
	{"200.00KHz"},


};
const uint8_t COMP_DISP[][6]=
{
	" ",
	"",
	" ",
	"PASS ",
	"LOW  ",
	"HIGH ",
	"SHORT",
	"HOPEN",
	"LOPEN",
	"",
	" ",
	"",



};
const uint8_t Setup_Beep[][15+1]=
{
	"关闭  ",
	"合格  ",
	"不合格",

};

const uint8_t FreqButton_Tip[][7+1]=  //频率选择时候的下面的提示符号
{
    {"DECR --"},
    {"DECR -"},
    {"INCR +"},
    {"INCR ++"},
	{" "}

};
const uint8_t User_Level[][10+1]=
{
    {"0.100 V"},
    {"0.300 V"},
	{"1.000 V"},

};
const uint8_t LevleButton_Tip[][7+1]=  //频率选择时候的下面的提示符号
{
    {"x1000"},
    {"x100 "},
	{"x10  "},
	{"x1   "},
	{"单位 "},

};

const uint8_t User_Range[][15+1]=
{
    "自动    ",
    "2MΩ    ",
	"20MΩ   ",
	"200MΩ  ",
	"2000MΩ ",

};
const uint8_t User_setRange[][15+1]=
{
    {"自动   "},
    {"2MΩ   "},
	{"20MΩ  "},
	{"200MΩ "},
	{"2000MΩ"},

};
const  uint8_t Auto_RangeDisp[][12+1]=
{
	{"AUTO_2M   "},
	{"AUTO_20M  "},
	{"AUTO_200M "},
	{"AUTO_2000M"},

};
const  uint8_t Hand_RangeDisp[][12+1]=
{
	{"HAND_2M   "},
	{"HAND_20M  "},
	{"HAND_200M "},
	{"HAND_2000M"},

};
const uint8_t RangeButton_Tip[][7+1]=  //频率选择时候的下面的提示符号
{
    {"AUTO"},
    {"HOLD"},
    {"DECR -"},
    {"INCR +"},

};
const uint8_t User_Biad[][10+1]=
{
    {"---- "},
   

};
const uint8_t BiasButton_Tip[][7+1]=  //频率选择时候的下面的提示符号
{
    {"DECR -"},
    {"INCR +"},

};
const uint8_t User_Speed[][10+1]=
{
    {"FAST "},
    {"MED "},
    {"SLOW "},
   
};
const uint8_t SpeedButton_Tip[][7+1]=  //频率选择时候的下面的提示符号
{
    {"FAST "},
    {"MED "},
    {"SLOW "},

};
const uint8_t User_Comp[][10+1]=
{
    
    {"OFF "},
	{"ON  "},
   
};
const uint8_t Limit_Scan_Fun[][4+1]=
{
	{"SEQ"},
	{"STEP"},

};
const uint8_t Limit_Scan_Page[][15+1]=
{
	{"上一页"},
	{"下一页"},

};
	
const uint8_t Sys_Bais_Value[][6+1]=
{
	{"INT"},
	{"ZC1788"}


};
const uint8_t Sys_Addr_value[][6+1]=
{
	{"RS232C"},
	{"GPIB"},
	{"USBTMC"},
	{"USBCDC"},
	{"RS485"},


};
const uint8_t Switch_Value[][4+1]=
{
	{"OFF"},
	{"ON"}

};
const uint8_t CompButton_Tip[][7+1]=  //频率选择时候的下面的提示符号
{
    {"ON  "},
    {"OFF "}
};
const uint8_t User_ListMode[][10+1]=
{
    {"SEQ"},
    {"STEP "},
   
};
const uint8_t ListModeButton_Tip[][7+1]=  //频率选择时候的下面的提示符号
{
    {"SEQ "},
    {"SETP "}
};
const uint8_t Setup_Trig[][10+1]=
{
    {"INT "},
    {"MAN "},
    {"EXT "},
    {"BUS "},
   
};
const uint8_t SetupTrig_Button_Tip[][7+1]=  //频率选择时候的下面的提示符号
{
    {"INT "},
    {"MAN "},
    {"EXT "},
    {"BUS "},
};
const uint8_t Setup_Alc[][10+1]=
{
    {"ON  "},
    {"OFF "},
   
};
const uint8_t SetupAlc_Button_Tip[][7+1]=  //频率选择时候的下面的提示符号
{
    {"ON  "},
    {"OFF "},
};
const uint8_t Setup_Rsou[][10+1]=
{
    {"100Ω  "},
    {"30Ω   "},
   
};

const uint8_t SetupTrigDly_Button_Tip[][7+1]=  //频率选择时候的下面的提示符号
{
    {"DECR -"},
    {"INCR +"},
};
const uint8_t Setup_Rev_A[][10+1]=
{
    {"ABS  "},
    {"%   "},
    {"OFF   "},
   
};
const uint8_t SetupRev_A_Button_Tip[][7+1]=  //频率选择时候的下面的提示符号
{
    {"ABS"},
    {"%  "},
    {"OFF"},
};
const uint8_t Setup_Rev_B[][10+1]=
{
    {"ABS  "},
    {"%   "},
    {"OFF   "},
   
};
const uint8_t SetupRev_B_Button_Tip[][7+1]=  //频率选择时候的下面的提示符号
{
    {"ABS"},
    {"%  "},
    {"OFF"},
};
const uint8_t SetupAvg_Button_Tip[][7+1]=  //频率选择时候的下面的提示符号
{
    {"DECR -"},
    {"INCR +"},
};
const uint8_t Setup_Vm[][10+1]=
{
    {"ON  "},
    {"OFF "},
   
};
const uint8_t SetupVm_Button_Tip[][7+1]=  //频率选择时候的下面的提示符号
{
    {"ON  "},
    {"OFF "},
};
const uint8_t Setup_Dcr[][10+1]=
{
    {"FIX  "},
    {"ALT "},
   
};
const uint8_t SetupDcr_Button_Tip[][7+1]=  //频率选择时候的下面的提示符号
{
    {"FIX  "},
    {"ALT "},
};
const uint8_t SetupRefa_Button_Tip[][7+1]=  //频率选择时候的下面的提示符号
{
    {"MEASURE"},
};
const uint8_t SetupRefB_Button_Tip[][7+1]=  //频率选择时候的下面的提示符号
{
    {"MEASURE"},
};

const uint8_t Correct_Open[][10+1]=
{
    {"ON  "},
    {"OFF "},
   
};
const uint8_t CorrectOpen_Button_Tip[][7+1]=  //比较界面选择时候的下面的提示符号
{
    {"ON  "},
    {"OFF "},
};
const uint8_t Correct_Short[][10+1]=
{
    {"ON  "},
    {"OFF "},
   
};
const uint8_t CorrectShort_Button_Tip[][7+1]=  //比较界面选择时候的下面的提示符号
{
    {"ON  "},
    {"OFF "},
};
const uint8_t Correct_Load[][10+1]=
{
    {"ON  "},
    {"OFF "},
   
};
const uint8_t CorrectLoad_Button_Tip[][7+1]=  //比较界面选择时候的下面的提示符号
{
    {"ON  "},
    {"OFF "},
};
const uint8_t CorrectSpot_Button_Tip[][7+1]=  //比较界面选择时候的下面的提示符号
{
    {"DECR ++"},
    {"DECR + "},
    {"INCR --"},
    {"INCR - "}
};
const uint8_t Correct_Freq[][10+1]=
{
    {"ON  "},
    {"OFF "},
   
};
const uint8_t CorrectFreq_Button_Tip[][7+1]=  //比较界面选择时候的下面的提示符号
{
    {"ON  "},
    {"OFF "},
};
const uint8_t Correct_Cable[][10+1]=
{
    {"0m  "},
    {"1m "},
    {"2m  "},
    {"4m "},
   
};
const uint8_t CorrectCable_Button_Tip[][7+1]=  //比较界面选择时候的下面的提示符号
{
    {"0m "},
    {"1m "},
    {"2m "},
    {"4m "},
};
//const uint8_t User_ListNo[][10+1]=
//{
//    {"SEQ"},
//    {"STEP "},
//   
//};
//const uint8_t ListNoButton_Tip[][7+1]=  //频率选择时候的下面的提示符号
//{
//    {"SEQ "},
//    {"SETP "}
//};
const uint8_t Test_Setitem_E[][9+1]=
{
	{"FUNC    :"},
	{"FREQ    :"},
	{"LEVEL   :"},
	{"RANGE   :"},
    {"BIAS    :"},
	{"SPEED   :"},
	
	{"Vm:"},
	{"Im:"},
	{"CORR:"}
	
};
const uint8_t  Range_Item[][15+1]=
{
	{"功能  :"},
	{"频率  :"},
	{"电平  :"},
	{"量程  :"},
	{"偏置  :"},
    {"速度  :"},
    {"比较  :"},
};
const uint8_t  Range_Item_E[][7+1]=
{
	{"FUNC  :"},
	{"FREQ  :"},
	{"LEVEL :"},
	{"RANGE :"},
	{"BIAS  :"},
	{"SPEED :"},
	{"COMP  :"}
};
const uint8_t	Set_testitem[][15+1]=
{
	{"组别    :"},
	{"速度    :"},
	{"         "},
	{"设置电压:"},
	{"测试时间:"},
	{"         "},
	{"高端开路:"},
	{"低端开路:"},
	{"        "},
	
	{"量程    :"},
	{"讯响    :"},
	{"         "},
	{"电阻上限:"},
	{"电阻下限:"},
	
	{"         "},
	{"短路检测:"},
	{"         "} ,
    {"         "},
};
const uint8_t	Set_testitem_E[][9+1]=
{
	{"FUNC    :"},
	{"FREQ    :"},
	{"LEVEL   :"},
	{"TRIG    :"},
	{"ALC     :"},
	{"Rsou    :"},
	{"TRIG DLY:"},
	{"STEP DLY:"},
	{"DEV A   :"},
	{"RANGE   :"},
    {"BIAS    :"},
	{"SPEED   :"},
	
	{"AVG     :"},
	{"Vm/Im   :"},
	{"DCR POL :"},
	{"DCR RNG :"},
	{"DC LEV  :"},
	{"REF A   :"}
};
const uint8_t Sys_Sys[][20+1]=
{
	#ifdef ZC5520 
{"仪器型号  ZC5520 "},
 #else
 {"仪器型号  ZC2683A"},
 #endif
	
	{"软件版本  Ver:1.0"},
	{"硬件版本  Ver:1.0"},
//	{"生产日期  "},
//	{"仪器编号  "},
//	{"出厂日期"},
//	{"账号    "},



};
//const uint8_t PRU_DATA[]=
//{""};
const uint8_t Sys_Setitem[][15+1]=
{
	{"串口开关"},
	{"波特率  "},
	{"U盘开关 "},
	{"I/O口   "},
	{"显示语言"},
	
	{"日    期"},
	{"时    间"},
//	{"账号    "},
	
};
const uint8_t Sys_Setitem_E[][10+1]=
{
	{"MAIN FUNC:"},
	{"PASS BEEP:"},
	{"FAIL BEEP:"},
	{"LANGUAGE :"},
	{"PASS WORD:"},
	{"MENU DISP:"},
	{"DATA     :"},
	{"BAUD RATE:"},
	{"BIAS SRC :"},
	{"BUS MODE :"},
	{"GPIBADDR :"},
	{"TALK ONLY:"},
	{"TIME     :"}
};
const char   TFT5520_NAME[][2][13+1]=
{
    {"测量设置",""},
    {"测量配置",""},
    {" ",""},
    {" ",""},
    {" ",""},

};
const char   TFT5520_TOP_NAME[][2][15+1]=
{
    {"<测量显示>","<TEST DISP>"},
    {"<测量设置>","<TEST SET >"},
    {"<测量配置>","<TEST CONF >"},
//    {"<测量配置>","<TEST CONF>"},
		{"<系统设置>","<SYS SET  >"},
		{"<系统信息>","<SYS INFO >"},    
    {"<列表显示>","<LIST DISP>"},
};

const uint8_t All_TopName[][21+1]=
{
	{"< 测量显示 >"},
	{"< 档号显示 >"},
	{"< 档计数显示 >  "},
	{"< 列表扫描显示 >"},
	{"< 测量设置 >    "},
	{"< 用户校正 >"},
	{"< 极限列表设置 >"},
	{"< 列表扫描设置 >"},
	{"< 系统设置 >    "},
	{"[ LCR文件列表 ] "},
	{"< 系统显示 >    "},

};
const uint8_t All_TopName_E[][25+1]=
{
	{"< MEAS DISPLAY >     "},
	{"< BIN No. DISP >     "},
	{"< BIN COUNT DISP >   "},
	{"< LIST SWEEP DISP >  "},
	{"< MEASURE SETUP >    "},
	{"< CORRECTION >       "},
	{"< LIMIT TABLE SETUP >"},
	{"< LIST SWEEP SETUP > "},
	{"< SYSTEM SETUP >     "},
	{"[ LCR文件列表 ]      "},

};
const uint8_t Range_Count_Item[][15+1]=
{
	{"参数 :"},
	{"标称 :"},
	{"计数 :"}

};
const uint8_t Range_Count_Item_E[][6+1]=
{
	{"PARAM :"},
	{"NOM.  :"},
	{"COUNT :"}

};
const uint8_t User_Check_main[][12+1]=
{
	{"档号    "},
	{"标准值  "},
	{"测试AD值"},

};
const uint8_t User_Check_Item[][12+1]=
{
	{"1000V   "},
	{"1.000MΩ"},
	{"10.00MΩ"},
	{"100.0MΩ"},
	{"1000 MΩ"},
	
//	{" 开路   A:"},
//	{" 短路   A:"},
//	{" 负载   A:"},
//	{"电缆  :"},
//	{"方式  :"},
//	{"功能  :"},
//	{"       "},
//	{"  B:"},
//	{"  B:"},
//	{"       "},
//	{"  B:"},
//	{"  B:"}



};
const uint8_t User_Check_Item_E[][12+1]=
{
	{"OPEN  :"},
	{"SHORT :"},
	{"LOAD  :"},
	{"SPOT No.  :"},
	{"FREQ      :"},
	{" REF     A:"},
	{" OPEN    A:"},
	{" SHORT   A:"},
	{" LOAD:   A"},
	{"CABLE :"},
	{"MODE  :"},
	{"FUNC  :"},
	{"       "},
	{"  B:"},
	{"  B:"},
	{"       "},
	{"  B:"},
	{"  B:"}



};
const uint8_t User_LimitList_Item[][15+1]=
{
 	{"参数  :"},
	{"标称  :"},
	{"方式  :"},
	{"附属  :"},
	{"比较  :"},
	{""},


};
const uint8_t User_LimitList_Value[][3+1]=
{
	
	"ABS",
	" % ",
};
const uint8_t User_LimitList_Item_E[][7+1]=
{
 	{"PARAM :"},
	{"NOM   :"},
	{"MODE  :"},
	{"AUX   :"},
	{"COMP  :"},
	{""},


};
const uint8_t User_LimitList_Item2[][2+1]=
{
	//{"BIN"},
	{" 1"},
	{" 2"},
	{" 3"},
	{" 4"},
	{" 5"},
	{" 6"},
	{" 7"},
	{" 8"},
	{" 9"},
	//{"2nd"}


};
const uint8_t User_LimitScan_Item2[][3+1]=
{
	{"No."},
	{"001"},
	{"002"},
	{"003"},
	{"004"},
	{"005"},
	{"006"},
	{"007"},
	{"008"},
	{"009"},
	{"010"}


};
const u8 Test_IRUINT[][10]=
{
    "kΩ  ",
    "MΩ  ",
//    "MΩ  ",
//    "MΩ  ",
//    "MΩ  ",
    "GΩ  ",
	  "        ",
};
const uint8_t User_ListScan_Item[][15+1]=
{
	{"模式 :"},
	{"频率[Hz]"},
    {"电平[V]"},
	{"LMT"},
	{"下限"},
	{"上限"},
    {"延时[s]"},

};

const uint8_t User_ListScan_Item_E[][8+1]=
{
	{"MODE :"},
	{"FREQ[Hz]"},
    {"LEVEL[V]"},
	{"LMT"},
	{"LOW "},
	{"HIGH"},
    {"DELY[s]"},

};
const uint8_t Sys_Language_Value[][7+1]=
{
	{"中文"},
	{"ENGLISH"}

};

const uint8_t Sys_Buard_Value[][6+1]=
{
	
	{"9600"},
	{"14400"},
	{"19200"},
	
	{"56000"},
	{"115200"},

};
const uint8_t Set_Unit[][5+1]=
{
	{"p"},
	{"n"},
	{"u"},
	{"m"},
	{"next"},
	{"*1"},
	{"k"},
	{"M"},
	
	{""},
	{"back"},



};
const uint8_t Disp_Unit[][2+1]=
{"pF","nF","u","m"," ","k","M"};
const uint8_t Disp_Range_Main_NUm[]={0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,2,2,3,3,3,2,2,1,1};
const uint8_t Disp_Range_Main_Disp[][2+1]=
{	"F",
	"H",
	"Ω",
	"S",
	"r",
	"°"
};//F,H,Ω，S r °空格
vu32 const Main_Func_Comp[][2]=
{
	{0,21},
	{0,33},
	{0,1},
	{0,3},
	{0,0},
	{0,0},
	{0,60000},
	{0,60000},
	{0,2},
	{0,2},
	{0,7},//量程
	{0,0},
	{0,2},
	{0,16},
	{0,1},
	{0,0},
	{0,0},
	{0,0},
	{0,999999},
	{0,5},
	{0,7},
	{0,999999},
	{0,5},
	{0,7}


};
const vu16 Debug_Comp_ad[][2]=
{
	{800,1500},
	{800,1500},
	{800,1500},
	{800,1500},
	{800,1200},
	


};
const vu16 Debug_Comp_ad1[]=
{
	1000,1000,1000,1000, 1000,3000
//	{800,1500},
//	{800,1500},
//	{800,1500},
//	{800,1500},
//	{1500,5500},
//	{1500,5500},


};
const vu16 Debug_Comp[][2]=
{
	{800,1200},
	{800,1200},
	{800,1200},
	{800,1200},
	{800,1200},
	


};
const vu16 Debug_Comp1[]=
{ 1000,1000,1000,1000, 1000,1000
	

};
//显示 下面的快捷键按键
void Disp_Fastbutton(uint32_t  page)
{
	uint32_t i;
//	uint32_t color;
    GUI_SetColor(LCD_COLOR_TEST_BUTON);
	for(i=0;i<5;i++)
	{

//		GUI_FillRect( FASTBUTTON_START, TOP_BAR_HIGH+i*FASTBUTTON_HIGH+6*i,FASTBUTTON_END, TOP_BAR_HIGH+(i+1)*FASTBUTTON_HIGH+6*i);	
        GUI_DrawGradientRoundedV(FASTBUTTON_START, TOP_BAR_HIGH+i*FASTBUTTON_HIGH+6*i,FASTBUTTON_END, TOP_BAR_HIGH+(i+1)*FASTBUTTON_HIGH+6*i,3,GUI_GRAY,GUI_DARKGRAY);
	}

}//118
void Disp_button_Num_time(void)
	
{
//	Disp_Fastbutton();
	
	Colour.black=LCD_COLOR_TEST_BUTON;
	Colour.Fword=White;
	WriteString_16(84, 271-30, " mΩ  ",  0);
	WriteString_16(84+80, 271-30, " Ω ",  0);


}
void Disp_button_Num_Input(vu8 page)
	
{
	vu8 i;
//	Disp_Fastbutton();
	
	Colour.black=LCD_COLOR_TEST_BUTON;
	Colour.Fword=White;//
	if(page==0)
	{
		for(i=0;i<5;i++)
		WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, Set_Unit[i],  0);
	}
	else
	{
		for(i=0;i<5;i++)
		WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, Set_Unit[i+5],  0);
	}
	
//	WriteString_16(84+80, 271-30, "sec",  0);


}
void Disp_button_Num_Freq(void)
	
{
//	Disp_Fastbutton();
	
	Colour.black=LCD_COLOR_TEST_BUTON;
	Colour.Fword=White;
	WriteString_16(84, 271-30, " 确认 ",  0);
	//WriteString_16(84+80, 271-30, " Ω ",  0);


}
void Disp_button_Num_Avg(void)
	
{
//	Disp_Fastbutton();
	
	Colour.black=LCD_COLOR_TEST_BUTON;
	Colour.Fword=White;
	WriteString_16(84, 271-30, "*1",  0);
	//WriteString_16(84+80, 271-30, "KHz",  0);


}
void Disp_Button_value1(uint32_t value)
{
//	Disp_Fastbutton();
	if(value==0)
	{
		Colour.Fword=White;
		Colour.black=LCD_COLOR_TEST_BUTON;
		WriteString_16(84, 271-40, "测量",  0);
		WriteString_16(84, 271-20, "显示",  0);
		WriteString_16(84+80, 271-40, "测量",  0);
		WriteString_16(84+80, 271-20, "设置",  0);
		WriteString_16(84+80+80, 271-40, "系统",  0);
		WriteString_16(84+80+80, 271-20, "设置",  0);
		WriteString_16(84+80+80+80, 271-40, "系统",  0);
		WriteString_16(84+80+80+80, 271-20, "信息",  0);
//		WriteString_16(84+80+80+80+80, 271-40, "更多",  0);
//		WriteString_16(84+80+80+80+80, 271-20, " 1/2",  0);
	}
    else if(value==1)
    {
        Colour.Fword=White;
		Colour.black=LCD_COLOR_TEST_BUTON;
		WriteString_16(84, 271-40, "文件",  0);
		WriteString_16(84, 271-20, "管理",  0);
		WriteString_16(84+80, 271-40, "保存",  0);
		WriteString_16(84+80, 271-20, "数据",  0);
        WriteString_16(84+80+80+80+80, 271-40, "更多",  0);
		WriteString_16(84+80+80+80+80, 271-20, " 2/2",  0);
    
    }

}
void Disp_Button_ItemScan_no(void)
{
//	Disp_Fastbutton();
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BUTON;
	WriteString_16(84, 271-40, "清除",  0);
	WriteString_16(84, 271-20, "表格",  0);
//				WriteString_16(84+80, 271-40, "自动",  0);
//				WriteString_16(84+80, 271-20, "复制",  0);
//        WriteString_16(84+80+80, 271-30, "工具",  0);
//		WriteString_16(84+80+80, 271-20, "设置",  0);
	WriteString_16(84+80+80+80, 271-30, "上一页",  0);
	WriteString_16(84+80+80+80+80, 271-30, "下一页",  0);
}
void Disp_Button_ItemScanSet(uint32_t value)
{
//	Disp_Fastbutton();
 	if(value==0||value==1)
	{
		Colour.Fword=White;
		Colour.black=LCD_COLOR_TEST_BUTON;
		WriteString_16(84, 271-30, "删除行",  0);
//		WriteString_16(84, 271-20, "设置",  0);
		WriteString_16(84+80, 271-40, "线性",  0);
		WriteString_16(84+80, 271-20, "自动",  0);
		WriteString_16(84+80+80, 271-40, "对数",  0);
		WriteString_16(84+80+80, 271-20, "自动",  0);
		WriteString_16(84+80+80+80, 271-30, "上一页",  0);
//		WriteString_16(84+80+80+80, 271-20, "设置",  0);
		WriteString_16(84+80+80+80+80, 271-30, "下一页",  0);
//		WriteString_16(84+80+80+80+80, 271-20, " 1/2",  0);
	}
    else 
//		if(value==2)
//		{
//			Colour.Fword=White;
//				Colour.black=LCD_COLOR_TEST_BUTON;
//				WriteString_16(84, 271-40, "清除",  0);
//				WriteString_16(84, 271-20, "表格",  0);
////				WriteString_16(84+80, 271-40, "自动",  0);
////				WriteString_16(84+80, 271-20, "复制",  0);
//		//        WriteString_16(84+80+80, 271-30, "工具",  0);
//		//		WriteString_16(84+80+80, 271-20, "设置",  0);
//				WriteString_16(84+80+80+80, 271-30, "上一页",  0);
//				WriteString_16(84+80+80+80+80, 271-30, "下一页",  0);
//			
//		
//		}else
				
			{
				Colour.Fword=White;
				Colour.black=LCD_COLOR_TEST_BUTON;
				WriteString_16(84, 271-30, "删除行",  0);
		//		WriteString_16(84, 271-20, "管理",  0);
				WriteString_16(84+80, 271-40, "自动",  0);
				WriteString_16(84+80, 271-20, "复制",  0);
		//        WriteString_16(84+80+80, 271-30, "工具",  0);
		//		WriteString_16(84+80+80, 271-20, "设置",  0);
				WriteString_16(84+80+80+80, 271-30, "上一页",  0);
				WriteString_16(84+80+80+80+80, 271-30, "下一页",  0);
			
			
			}


}
void Disp_Button_TestSet(uint32_t value)
{
//	Disp_Fastbutton();
 	if(value==0)
	{
		Colour.Fword=White;
		Colour.black=LCD_COLOR_TEST_BUTON;
		WriteString_16(84, 271-40, "测量",  0);
		WriteString_16(84, 271-20, "显示",  0);
		WriteString_16(84+80, 271-40, "测量",  0);
		WriteString_16(84+80, 271-20, "设置",  0);
		WriteString_16(84+80+80, 271-40, "系统",  0);
		WriteString_16(84+80+80, 271-20, "设置",  0);
		WriteString_16(84+80+80+80, 271-40, "系统",  0);
		WriteString_16(84+80+80+80, 271-20, "信息",  0);
//		WriteString_16(84+80+80+80+80, 271-40, "更多",  0);
//		WriteString_16(84+80+80+80+80, 271-20, " 1/2",  0);
	}
    else if(value==1)
    {
        Colour.Fword=White;
		Colour.black=LCD_COLOR_TEST_BUTON;
		WriteString_16(84, 271-40, "文件",  0);
		WriteString_16(84, 271-20, "管理",  0);
		WriteString_16(84+80, 271-40, "系统",  0);
		WriteString_16(84+80, 271-20, "设置",  0);
        WriteString_16(84+80+80, 271-30, "工具",  0);
//		WriteString_16(84+80+80, 271-20, "设置",  0);
        WriteString_16(84+80+80+80+80, 271-40, "更多",  0);
		WriteString_16(84+80+80+80+80, 271-20, " 2/2",  0);
    
    
    }


}

void Disp_Button_SysSet(void)
{

		Colour.Fword=White;
		Colour.black=LCD_COLOR_TEST_BUTON;
		WriteString_16(84, 271-40, "测量",  0);
		WriteString_16(84, 271-20, "显示",  0);
		WriteString_16(84+80, 271-40, "测量",  0);
		WriteString_16(84+80, 271-20, "设置",  0);
		WriteString_16(84+80+80, 271-40, "系统",  0);
		WriteString_16(84+80+80, 271-20, "设置",  0);
		WriteString_16(84+80+80+80, 271-40, "系统",  0);
		WriteString_16(84+80+80+80, 271-20, "信息",  0);
//		WriteString_16(84+80+80+80+80, 271-40, "系统",  0);
//		WriteString_16(84+80+80+80+80, 271-20, "复位",  0);


}
void Disp_TopBar_Color(vu16 page)
{
	LCD_DrawRect( SCREENSTART, SCREENSTART, SCREEN_WIDTH+1, SPACE1 , LCD_COLOR_BLACK);
    
	Colour.black=LCD_COLOR_BLACK;
	Colour.Fword=LCD_COLOR_WHITE;
	WriteString_16(4, 3, "ZCtek",  0);
//    LCD_DrawRect( 0, 271-18, 479, 271 , LCD_COLOR_TEST_BAR);


}
void Disp_MidRect(void)
{

//	LCD_DrawRect( 4, 92,FASTBUTTON_START-8 , 271-18 , LCD_COLOR_TEST_MID ) ;
    GUI_SetColor(LCD_COLOR_TEST_MID);
    GUI_FillRoundedRect(4, 92,FASTBUTTON_START-8 , 271-18 ,5);
   // LCD_DrawRect( 320, 92,480 , 271-18 , LCD_COLOR_BLACK ) ;
}
void Disp_mainitem(uint32_t item)
{

    GUI_SetColor(GUI_ORANGE);
    GUI_SetFont(&GUI_FontGUI_FONTXBFFON16);
    GUI_FillRect(SCREENSTART, 0,SCREENSTART+ SELECT_1END, SPACE1 );
    GUI_SetColor(GUI_FONTCOLOUR2);
    GUI_SetTextMode( GUI_TEXTMODE_TRANS);
    GUI_DispStringAt(TFT5520_TOP_NAME[item][U9001_Save_sys.U9001_SYS.language],SCREENSTART, 2);
	

}
void Disp_TestScreen(void)
{
//	Disp_TopBar_Color(0);
	Disp_mainitem(0);
	
//	Disp_Fastbutton();//显示快速按键
	Disp_MidRect();//显示中间色块
}
//
//测量显示主菜单的项目显示
//

void Disp_Fastset_Disp(uint8_t num)
{
	uint32_t  i;
	GUI_RECT Rect;
	GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
    GUI_SetBkColor(LCD_COLOR_TEST_BUTON);
    GUI_SetColor(GUI_WHITE);
    for(i=0;i<MAXBUTTONNUM;i++)
    {
       
        Rect.x0=FASTBUTTON_START;
        Rect.y0=TOP_BAR_HIGH+i*FASTBUTTON_HIGH+6*i;
        Rect.x1=FASTBUTTON_END;
        Rect.y1=TOP_BAR_HIGH+(i+1)*FASTBUTTON_HIGH+6*i;
//        GUI_DispStringAt(TFT5520_NAME[i][U9001_save_sys.U9001_SYS.language],FASTBUTTON_START, TOP_BAR_HIGH+i*FASTBUTTON_HIGH+i*2+15);
        GUI_DispStringInRect(TFT5520_NAME[i][U9001_Save_sys.U9001_SYS.language],&Rect,GUI_TA_HCENTER|GUI_TA_VCENTER);
    }
}
void Disp_Test_Item(void)
{
	uint32_t i;
	Disp_TestScreen();
	Disp_Fastbutton(0);
	Disp_Fastset_Disp(0);

	for(i=0;i<6;i++)
	{
		if(i<3)
			WriteString_16(LIST1, FIRSTLINE+(i+1)*SPACE1, Test_Setitem[i],  0);
		else
			WriteString_16(LIST2, FIRSTLINE+(i-3+1)*SPACE1, Test_Setitem[i],  0);
//		WriteString_16(0, 210, Test_Setitem[6],  0);
//		WriteString_16(120, 210, Test_Setitem[7],  0);
//		WriteString_16(250, 210, Test_Setitem[8],  0);
	
	}
//	Disp_Button_value1(0);
	

}
//档号显示子函数
void Disp_Range_MidRect(void)
{

	LCD_DrawRect( 0, 112,479 , 190 , LCD_COLOR_TEST_MID ) ;
}
void Disp_RangeScreen(void)
{
	Disp_TopBar_Color(0);
//	Disp_Fastbutton();//显示快速按键
	Disp_Range_MidRect();//显示中间色块
}
//档号显示主菜单项目显示

//档计数显示子函数

void Disp_Rang_Count_MidRect(void)
{

	LCD_DrawRect( 0, 86,479 , 160 , LCD_COLOR_TEST_MID ) ;
	LCD_DrawRect( 0, 193,479 , 227 , LCD_COLOR_TEST_MID ) ;
}

void Disp_Range_CountScreen(void)
{
	Disp_TopBar_Color(0);
//	Disp_Fastbutton();//显示快速按键
	Disp_Rang_Count_MidRect();//显示中间色块
}

//列表显示子函数
void Disp_List_Count_MidRect(void)
{

	LCD_DrawRect( 0, 96,479 , 160 , LCD_COLOR_TEST_MID ) ;
//	LCD_DrawRect( 0, 195,479 , 227 , LCD_COLOR_TEST_MID ) ;
}

void Disp_List_CountScreen(void)
{
	Disp_TopBar_Color(0);
//	Disp_Fastbutton();//显示快速按键
	Disp_List_Count_MidRect();//显示中间色块
}
//列表显示界面
void Disp_List_Count_Item(void)
{
	Disp_List_CountScreen();
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BAR;
	WriteString_16(0, 4, All_TopName[3],  0);
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BACK;

	WriteString_16(LIST1, FIRSTLINE, "方式 :",  0);
	WriteString_16(LIST1+40, FIRSTLINE+SPACE1, "频率[Hz]",  0);	
	WriteString_16(LIST1+140, FIRSTLINE+SPACE1, "电平[V]",  0);	
	WriteString_16(LIST1+140+100, FIRSTLINE+SPACE1, "Cp[F]",  0);
	WriteString_16(LIST1+140+100+100, FIRSTLINE+SPACE1, "D[ ]",  0);
	WriteString_16(LIST1+440, FIRSTLINE+SPACE1, "CMP",  0);
	Disp_Button_value1(0);

}

//列表显示子函数
void Disp_TestSet_MidRect(void)
{

//	LCD_DrawRect( 0, 90,479 , 154 , LCD_COLOR_TEST_MID ) ;
//	LCD_DrawRect( 0, 195,479 , 227 , LCD_COLOR_TEST_MID ) ;
}

void Disp_TestSetScreen(vu8 page)
{
//	Disp_TopBar_Color(page);
//	Disp_Fastbutton();//显示快速按键
	Disp_TestSet_MidRect();//显示中间色块
}

void Disp_SYS_Set_Item(void)
{
	uint32_t i;
//	Disp_TestSetScreen(1);
//    Disp_mainitem(2);

	Disp_Fastbutton(1);
    
//	Disp_Fastset_Disp(1);
	Disp_mainitem(3);
    GUI_SetColor(White);
	GUI_SetBkColor(LCD_COLOR_TEST_BACK);
//    U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter=1;
    for(i=0;i<7;i++)
    {
        if(i<4)
        {
            GUI_DispStringAt(SYS_Set_Tab[i][U9001_Save_sys.U9001_SYS.language],LIST1, FIRSTLINE+SPACE1*i);
        }
        else
        {
             GUI_DispStringAt(SYS_Set_Tab[i][U9001_Save_sys.U9001_SYS.language],LIST2, FIRSTLINE+SPACE1*(i-4));
        }	
    }
//	for(i=0;i<2;i++)
//		GUI_DispStringAt(SYS_SetTime_Tab[i][U9001_save_sys.U9001_SYS.language],LIST1, FIRSTLINE+SPACE1*((i+6)));
  
	

}

void Disp_Test_Set_Item(void)
{
	uint32_t i;
    u8 num;
//	Disp_TestSetScreen(1);
//    Disp_mainitem(2);

	Disp_Fastbutton(1);
    
//	Disp_Fastset_Disp(1);
	Disp_mainitem(1);
    GUI_SetColor(White);
	GUI_SetBkColor(LCD_COLOR_TEST_BACK);
//    U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter=1;
//    num=(sizeof(Setup_Tab[0])/(sizeof(Setup_Tab[0][0])));
    num=MAX_SETP[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter];
    for(i=0;i<num;i++)
    {
        if(i%2==0)
        {
            GUI_DispStringAt(Setup_Tab[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter][i][U9001_Save_sys.U9001_SYS.language],LIST1, FIRSTLINE+SPACE1*((i/2)));

        }
        else
        {

            
             GUI_DispStringAt(Setup_Tab[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter][i][U9001_Save_sys.U9001_SYS.language],LIST2, FIRSTLINE+SPACE1*(i-1)/2);
            
            
        }	
    }
  
	

}
const u8 Test_dot[][7]=
{
    2,3,0,0,0,0,0,//AC
    2,3,0,0,0,0,0,//DC
    3,3,2,1,3,2,1,//IR
    3,2,1,3,2,1,3,//OS

};
void Disp_Test_List(u8 i)
{
    
    GUI_SetColor(GUI_LIGHTYELLOW);
    GUI_SetBkColor(LCD_COLOR_TEST_BACK);
    GUI_SetFont(&GUI_FontGUI_FONTXBFFON16);
    GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
    Hex_Format(Test_Value.Vol,3,4,0);
    strcat(DispBuf,"kV");
    
    GUI_DispStringAt(DispBuf,80,40+20*(i));//TEST_UNIT
    
		if(U9001_Save_sys.U9001_save.all_step == 2)
		{
			if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter == IR)
			{
				memset(sendbuff1,0,20);
				memcpy(sendbuff1,DispBuf,4);
				strcat(sendbuff1,"kV;");
			}else{
				memset(sendbuff,0,20);
				memcpy(sendbuff,DispBuf,4);
				strcat(sendbuff,"kV;");
			}
//			if(U9001_Save_sys.U9001_save.current_step == 2)
//			{
//				memset(sendbuff1,0,20);
//				memcpy(sendbuff1,DispBuf,4);
//				strcat(sendbuff1,"kV;");
//			}
		}else{
			memset(sendbuff,0,20);
			memcpy(sendbuff,DispBuf,4);
			strcat(sendbuff,"kV;");
			
			memset(sendbuff1,0,20);
			memcpy(sendbuff1,DispBuf,4);
			strcat(sendbuff1,"kV;");
		}
//    if(Resistance==0xffff)
//        memcpy(DispBuf,"UPPER",6); 
//    else
//    {  
//        test_value=IntToStr(Resistance);
//        //                Hex_Format(test_value.num,test_value.dot,4,0);
//        Hex_Format(test_value.num,Test_dot[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter][Range],4,0);
//    }
//    if(Test_mid.set_item==IR_SETUP)//Test_IRUINT
//    {
//        strcat(DispBuf,Test_IRUINT[test_value.uint]);
//    }else if((Test_mid.set_item==ACW_SETUP)||(Test_mid.set_item==DCW_SETUP))
//        strcat(DispBuf,"mA ");
//    GUI_DispStringAt(DispBuf,160,40+20*(i));
    switch(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter)
    {
        case AC:
        case DC:
					if(Test_Value.I_R==0xffff)
						memcpy(DispBuf,"UPPER",6); 
					else
					{  
		//                    test_value=IntToStr(Test_Value.I_R);
		//                    if(Range==0)
		//                        Hex_Format(test_value.num,1,4,0);
		//                    else
		//                        Hex_Format(test_value.num,1,3,0);
						 if(U9001_Save_sys.U9001_Testconfg.clear == 1)
						 {
							 if(Test_Value.I_R < U9001_Save_sys.U9001_save.clearvalue[U9001_Save_sys.U9001_save.current_step-1])
							 {
								 Test_Value.I_R = 0;
							 }else{
								 Test_Value.I_R -= U9001_Save_sys.U9001_save.clearvalue[U9001_Save_sys.U9001_save.current_step-1];
							 }
						 }
						 if(Range==1)
						 {
								Hex_Format(Test_Value.I_R,3,4,0);
								memcpy(&sendbuff[7],&DispBuf[0],4);
						 }else if(Range==0){
								Hex_Format(Test_Value.I_R,2,4,0);
								memcpy(&sendbuff[7],&DispBuf[1],4);
						 }else{
								Hex_Format(Test_Value.I_R,1,4,0);
								memcpy(&sendbuff[7],"0.00",4);
						 }
//						 if(Range==1)
//							Hex_Format(Test_Value.I_R,3,4,0);
//						 else
//							Hex_Format(Test_Value.I_R,2,4,0);
					}
//					if(Range<2)
						strcat(sendbuff,"mA;");
//							 else
//								 strcat(sendbuff,"uA;");
					strcat(DispBuf,"mA ");
					GUI_DispStringAt(DispBuf,160,40+20*(i));
				break;
        case IR:
			if(Test_Value.I_R==0xffff*10)
			{
			   memcpy(DispBuf,"UPPER   ",8); 
				 strcat(sendbuff1,">9999");
			}else
			{  
				test_value=IntToStr(Test_Value.I_R);
			 
				Hex_Format(test_value.num,test_value.dot,4,0);
				strcat(DispBuf,Test_IRUINT[test_value.uint]);
				strcat(sendbuff1,(char*)DispBuf);
			}
			GUI_DispStringAt(DispBuf,160,40+20*(i));
			if(test_value.uint == 0)
			{
				strcat(sendbuff1,(char*)"k;");
			}else if(test_value.uint == 1){
				strcat(sendbuff1,(char*)"M;");
			}else if(test_value.uint == 2){
				strcat(sendbuff1,(char*)"G;");
			}
//            GUI_DispStringAt(DispBuf,DISP_V_XPOS+50,DISP_V_YPOS+50-20);
//            GUI_DispStringAt(Test_IRUINT[test_value.uint],DISP_V_XPOS+50+6*25,DISP_V_YPOS+50-20);·	
            break;
        case OS:
		
			if(Test_Value.I_R==0xffff)
			   memcpy(DispBuf,"UPPER   ",8); 
			else
			{  
				test_value=FToStr(Test_Value.I_R);
			 
				Hex_Format(test_value.num,test_value.dot,4,0);
			}
			strcat(DispBuf,Disp_Unit[test_value.uint]);
            GUI_DispStringAt(DispBuf,160,40+20*(i));
            
           
            
          
        
            break;
        
            
        case PA:
            break;
        default:
            break;
    
    }
    Hex_Format(Test_Value.Time,1,4,1);
    strcat(DispBuf," s");
    GUI_DispStringAt(DispBuf,DISP_V_XPOS+44,DISP_V_YPOS+100+23);
    disp_TestMSG(GetSystemMessage());

}
void Disp_Idel_list(void)
{
    uint32_t i;
    UnitNum_typedef Disp_Value;
	Disp_Fastbutton(1);
    Disp_mainitem(5);//Test_List_Tab
    GUI_SetColor(GUI_WHITE);
    GUI_SetFont(&GUI_FontGUI_FONTXBFFON16);
    for(i=0;i<3;i++)
    {
        GUI_DispStringAt(Test_List_Tab[i][U9001_Save_sys.U9001_SYS.language],100+i*80,40);
    
    }
    GUI_SetFont(&GUI_Font20_ASCII);
    for(i=0;i<U9001_Save_sys.U9001_save.all_step;i++)
    {
        GUI_DispDecAt(i+1,20,40+20*(i+1),2);
        
        GUI_DispStringAt(TestPara[U9001_Save_sys.U9001_save.U9001_Setup[i+1].parameter],50,40+20*(i+1));
    }
    GUI_SetColor(GUI_LIGHTYELLOW);
    GUI_SetFont(&GUI_FontGUI_FONTXBFFON16);
    for(i=0;i<U9001_Save_sys.U9001_save.all_step;i++)
    {
        Hex_Format(U9001_Save_sys.U9001_save.U9001_Setup[i+1].V_out,3,4,FALSE);
        strcat(DispBuf,"kV");
        GUI_DispStringAt(DispBuf,80,40+20*(i+1));//TEST_UNIT
        
        Disp_Value=IntToStr_mA(U9001_Save_sys.U9001_save.U9001_Setup[i+1].Upper);
        Hex_Format(Disp_Value.num,Disp_Value.dot,4,FALSE);
        if((U9001_Save_sys.U9001_save.U9001_Setup[i+1].parameter==AC)||(U9001_Save_sys.U9001_save.U9001_Setup[i+1].parameter==DC))
            strcat(DispBuf,TEST_UNIT[1][0]);
        else if(U9001_Save_sys.U9001_save.U9001_Setup[i+1].parameter==IR)
        {
            strcat(DispBuf,TEST_UNIT[Disp_Value.uint][1]);
        }
        GUI_DispStringAt(DispBuf,160,40+20*(i+1));//上限
        GUI_DispCharAt(234,100,100);
        
        
    }
    GUI_SetFont(&GUI_FontGUI_FONTXBFFON16);

}
void Disp_Idel_Item(void)
{
	uint32_t i;
    UnitNum_typedef Disp_Value;
    
	Disp_Fastbutton(1);
	Disp_mainitem(0);
    GUI_SetColor(GUI_WHITE);
	GUI_SetBkColor(LCD_COLOR_TEST_BACK);
    for(i=0;i<(sizeof(Idel_Tab[0])/(sizeof(Idel_Tab[0][0])));i++)
    {
        if((i%2)==0)
        {
          
            GUI_DispStringAt(Idel_Tab[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter][i][U9001_Save_sys.U9001_SYS.language],LIST1, FIRSTLINE+SPACE1*((i+1)/2));
        }
        else
        {
        
             GUI_DispStringAt(Idel_Tab[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter][i][U9001_Save_sys.U9001_SYS.language],LIST2, FIRSTLINE+SPACE1*((i-1)/2));
            
        }	
    }
    GUI_SetColor(GUI_WHITE);
    sprintf(DispBuf,"%d/%d",U9001_Save_sys.U9001_save.current_step,U9001_Save_sys.U9001_save.all_step);
    GUI_DispStringAt(DispBuf,LISTVALUE1,FIRSTLINE);//显示步骤
    GUI_DispStringAt(TestPara[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter],LISTVALUE2,FIRSTLINE+0*SPACE1);//参数
    switch(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter)
    {
        case AC:
    
    
            Hex_Format(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out,3,4,FALSE);
            strcat(DispBuf,"kV");
            GUI_DispStringAt(DispBuf,LISTVALUE1,FIRSTLINE+SPACE1);//电压
            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time)
            {
                Hex_Format(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time,1,4,FALSE);
                strcat(DispBuf,"s");
                GUI_DispStringAt(DispBuf,LISTVALUE1,FIRSTLINE+2*SPACE1);//测试时间
            }
            else//
            {
                GUI_DispStringAt(Continue_Tab[0][U9001_Save_sys.U9001_SYS.language],LISTVALUE1,FIRSTLINE+2*SPACE1);//连续测试
            }
             if(U9001_Save_sys.U9001_Testconfg.ARC_mode)//等级
            {
                if((U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc>0)&&(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc<=9))
                    GUI_DispDecAt(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc,LISTVALUE1,FIRSTLINE+3*SPACE1,1);
                else
                    GUI_DispStringAt(Switch_Tab[0][U9001_Save_sys.U9001_SYS.language],LISTVALUE1,FIRSTLINE+3*SPACE1);
            }
            else
            {
                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc)
                {
                    Hex_Format(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc,1,3,FALSE);
                    strcat(DispBuf,"mA");
                    
                    GUI_DispStringAt(DispBuf,LISTVALUE1,FIRSTLINE+3*SPACE1);
                
                }
                else
                    GUI_DispStringAt(Switch_Tab[0][U9001_Save_sys.U9001_SYS.language],LISTVALUE1,FIRSTLINE+3*SPACE1);
            }
            
//             Hex_Format(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper.num,U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper.dot,4,FALSE);
            Disp_Value=IntToStr_mA(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper);
            Hex_Format(Disp_Value.num,Disp_Value.dot,4,FALSE);
            strcat(DispBuf,TEST_UNIT[1][0]);
            GUI_DispStringAt(DispBuf,LISTVALUE2,FIRSTLINE+1*SPACE1);//上限
            
             if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower)
            {
                Disp_Value=IntToStr_mA(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower);
                Hex_Format(Disp_Value.num,Disp_Value.dot,4,FALSE);
                strcat(DispBuf,TEST_UNIT[1][0]);
               
                GUI_DispStringAt(DispBuf,LISTVALUE2,FIRSTLINE+2*SPACE1);//下限
            }
            else
                GUI_DispStringAt(Switch_Tab[0][U9001_Save_sys.U9001_SYS.language],LISTVALUE2,FIRSTLINE+2*SPACE1);
             GUI_SetColor(GUI_YELLOW);
            GUI_SetBkColor(LCD_COLOR_TEST_BACK);
            GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
            GUI_SetFont(&GUI_FontGUI_FONTXBFFON50);
            GUI_DispStringAt("V:",DISP_V_XPOS,DISP_V_YPOS);
            GUI_DispStringAt("kV",DISP_V_XPOS+200,DISP_V_YPOS);
            GUI_DispStringAt("I:",DISP_V_XPOS,DISP_V_YPOS+50);
            GUI_DispStringAt("mA",DISP_V_XPOS+200,DISP_V_YPOS+50);
            GUI_SetFont(&GUI_FontGUI_FONTXBFFON16);
            GUI_DispStringAt("T :",DISP_V_XPOS+12,DISP_V_YPOS+100+23);
            GUI_DispStringAt("s",DISP_V_XPOS+140-40,DISP_V_YPOS+100+23);
            break;
        case DC:
            
    
            Hex_Format(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out,3,4,FALSE);
            strcat(DispBuf,"kV");
            GUI_DispStringAt(DispBuf,LISTVALUE1,FIRSTLINE+SPACE1);//电压
            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time)
            {
                Hex_Format(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time,1,4,FALSE);
                strcat(DispBuf,"s");
                GUI_DispStringAt(DispBuf,LISTVALUE1,FIRSTLINE+2*SPACE1);//测试时间
            }
            else//
            {
                GUI_DispStringAt(Continue_Tab[0][U9001_Save_sys.U9001_SYS.language],LISTVALUE1,FIRSTLINE+2*SPACE1);//测试时间
            }
//            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].rise_time!=0)
//            {
//                Hex_Format(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].rise_time,1,4,FALSE);
//                strcat(DispBuf,"s");
//                GUI_DispStringAt(DispBuf,LISTVALUE1,FIRSTLINE+3*SPACE1);//缓升时间时间 
//            }
//            else
//            {
//            
//                GUI_DispStringAt(Switch_Tab[0][U9001_save_sys.U9001_SYS.language],LISTVALUE1,FIRSTLINE+3*SPACE1);
//            }
    
//            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].drop_time!=0)
//            {
//                Hex_Format(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].drop_time,1,4,FALSE);
//                strcat(DispBuf,"s");
//                GUI_DispStringAt(DispBuf,LISTVALUE1,FIRSTLINE+4*SPACE1);//下降时间时间 
//            }
//            else
//            {
//            
//                GUI_DispStringAt(Switch_Tab[0][U9001_save_sys.U9001_SYS.language],LISTVALUE1,FIRSTLINE+4*SPACE1);
//            }
//            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check)
//            {
//                Hex_Format(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check,1,4,FALSE);
//                strcat(DispBuf,"uA");
//                GUI_DispStringAt(DispBuf,LISTVALUE1,FIRSTLINE+5*SPACE1);//检测
//            
//            }
//            else
//                GUI_DispStringAt(Switch_Tab[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check][U9001_save_sys.U9001_SYS.language],LISTVALUE1,FIRSTLINE+5*SPACE1);
            Disp_Value=IntToStr_mA(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper);
            Hex_Format(Disp_Value.num,Disp_Value.dot,4,FALSE);
            strcat(DispBuf,TEST_UNIT[1][0]);

            GUI_DispStringAt(DispBuf,LISTVALUE2,FIRSTLINE+1*SPACE1);//上限
    
            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower)
            {
                Disp_Value=IntToStr_mA(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower);
                
                Hex_Format(Disp_Value.num,Disp_Value.dot,4,FALSE);
                strcat(DispBuf,TEST_UNIT[1][0]);
                GUI_DispStringAt(DispBuf,LISTVALUE2,FIRSTLINE+2*SPACE1);//下限
            }
            else
                GUI_DispStringAt(Switch_Tab[0][U9001_Save_sys.U9001_SYS.language],LISTVALUE2,FIRSTLINE+2*SPACE1);
            
             if(U9001_Save_sys.U9001_Testconfg.ARC_mode)//电弧等级
            {
                if((U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc>0)&&(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc<=9))
                    GUI_DispDecAt(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc,LISTVALUE1,FIRSTLINE+3*SPACE1,1);
                else
                    GUI_DispStringAt(Switch_Tab[0][U9001_Save_sys.U9001_SYS.language],LISTVALUE1,FIRSTLINE+3*SPACE1);
            }
            else
            {
                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc)
                {
                    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc>200)
                        U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc=200;
                    Hex_Format(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc,1,3,FALSE);
                    strcat(DispBuf,"mA");
                    GUI_DispStringAt(DispBuf,LISTVALUE1,FIRSTLINE+3*SPACE1);
                
                }
                else
                    GUI_DispStringAt(Switch_Tab[0][U9001_Save_sys.U9001_SYS.language],LISTVALUE1,FIRSTLINE+3*SPACE1);
            }
//            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last)
//            {
//                Hex_Format(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last,2,4,FALSE);
//                strcat(DispBuf,"s");
//                GUI_DispStringAt(DispBuf,LISTVALUE2,4*SPACE1);
//            }else
//            {
//                GUI_DispStringAt(Switch_Tab[0][U9001_save_sys.U9001_SYS.language],LISTVALUE2,FIRSTLINE+4*SPACE1);
//            }
            
            GUI_SetColor(GUI_YELLOW);
            GUI_SetBkColor(LCD_COLOR_TEST_BACK);
            GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
            GUI_SetFont(&GUI_FontGUI_FONTXBFFON50);
            GUI_DispStringAt("V:",DISP_V_XPOS,DISP_V_YPOS);
            GUI_DispStringAt("kV",DISP_V_XPOS+200,DISP_V_YPOS);
            GUI_DispStringAt("I:",DISP_V_XPOS,DISP_V_YPOS+50);
            GUI_DispStringAt("mA",DISP_V_XPOS+200,DISP_V_YPOS+50);
            GUI_SetFont(&GUI_FontGUI_FONTXBFFON16);
            GUI_DispStringAt("T :",DISP_V_XPOS+12,DISP_V_YPOS+100+23);
            GUI_DispStringAt("s",DISP_V_XPOS+140-40,DISP_V_YPOS+100+23);
            break;
        case IR:
            Hex_Format(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out,3,4,FALSE);
            strcat(DispBuf,"kV");
            GUI_DispStringAt(DispBuf,LISTVALUE1,FIRSTLINE+SPACE1);//电压
            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time)
            {
                Hex_Format(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time,1,4,FALSE);
                strcat(DispBuf,"s");
                GUI_DispStringAt(DispBuf,LISTVALUE1,FIRSTLINE+2*SPACE1);//测试时间
            }
            else//
            {
                GUI_DispStringAt(Continue_Tab[0][U9001_Save_sys.U9001_SYS.language],LISTVALUE1,FIRSTLINE+2*SPACE1);//测试时间
            }

           if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper)
           {
                Disp_Value=IntToStr(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper);
                Hex_Format(Disp_Value.num,Disp_Value.dot,4,FALSE);
                strcat(DispBuf,TEST_UNIT[Disp_Value.uint][1]);

                GUI_DispStringAt(DispBuf,LISTVALUE2,FIRSTLINE+1*SPACE1);//上限
           }
           else
               GUI_DispStringAt(Switch_Tab[0][U9001_Save_sys.U9001_SYS.language],LISTVALUE2,FIRSTLINE+1*SPACE1);
          
            Disp_Value=IntToStr(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower);
            Hex_Format(Disp_Value.num,Disp_Value.dot,4,FALSE);
            strcat(DispBuf,TEST_UNIT[Disp_Value.uint][1]);
            GUI_DispStringAt(DispBuf,LISTVALUE2,FIRSTLINE+2*SPACE1);//下限
           
//            GUI_DispStringAt(Setup_Range_Tab[U9001_save_sys.U9001_SYS.language][U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc],LISTVALUE2,3*SPACE1);
//       
//           GUI_DispStringAt(Switch_Tab[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last][U9001_save_sys.U9001_SYS.language],LISTVALUE2,4*SPACE1);
             GUI_SetColor(GUI_YELLOW);
            GUI_SetBkColor(LCD_COLOR_TEST_BACK);
            GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
            GUI_SetFont(&GUI_FontGUI_FONTXBFFON50);
            GUI_DispStringAt("V:",DISP_V_XPOS,DISP_V_YPOS);
            GUI_DispStringAt("kV",DISP_V_XPOS+200,DISP_V_YPOS);
            GUI_DispStringAt("R:",DISP_V_XPOS,DISP_V_YPOS+50);
            GUI_DispStringAt("Ω",DISP_V_XPOS+200,DISP_V_YPOS+50);
            GUI_SetFont(&GUI_FontGUI_FONTXBFFON16);
            GUI_DispStringAt("T :",DISP_V_XPOS+12,DISP_V_YPOS+100+23);
            GUI_DispStringAt("s",DISP_V_XPOS+140-40,DISP_V_YPOS+100+23);
            break;
        case OS:
            sprintf(DispBuf,"%3d%%",U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower);
            GUI_DispStringAt(DispBuf,LISTVALUE1,FIRSTLINE+SPACE1);
            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>=100)
            {
                sprintf(DispBuf,"%3d%%",U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper);
                GUI_DispStringAt(DispBuf,LISTVALUE1,FIRSTLINE+2*SPACE1);
            }
            else
               GUI_DispStringAt(Switch_Tab[0][U9001_Save_sys.U9001_SYS.language],LISTVALUE1,FIRSTLINE+2*SPACE1); 
            
            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last)
            {
//                Disp_Value=FToStr(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last);
				if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last >= 1000)
				{
					Hex_Format(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last/10,2,4,0);
				}else{
					Hex_Format(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last,3,4,0);
				}
                
                strcat(DispBuf,Disp_Unit[1]);
                GUI_DispStringAt(DispBuf,LISTVALUE2,FIRSTLINE+1*SPACE1);
            }
            else
               GUI_DispStringAt("NONE",LISTVALUE2,FIRSTLINE+1*SPACE1);
             GUI_SetColor(GUI_YELLOW);
            GUI_SetBkColor(LCD_COLOR_TEST_BACK);
            GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
            GUI_SetFont(&GUI_FontGUI_FONTXBFFON50);
            GUI_DispStringAt("V:",DISP_V_XPOS,DISP_V_YPOS);
            GUI_DispStringAt("kV",DISP_V_XPOS+200,DISP_V_YPOS);
            GUI_DispStringAt("C:",DISP_V_XPOS,DISP_V_YPOS+50);
            GUI_DispStringAt("nF",DISP_V_XPOS+200,DISP_V_YPOS+50);
            GUI_SetFont(&GUI_FontGUI_FONTXBFFON16);
            GUI_DispStringAt("T :",DISP_V_XPOS+12,DISP_V_YPOS+100+23);
            GUI_DispStringAt("s",DISP_V_XPOS+140-40,DISP_V_YPOS+100+23);
            break;
        case PA:
             GUI_DispStringAt("PAUSE",LISTVALUE1,FIRSTLINE+SPACE1);
		
			GUI_DispStringAt(Switch_Tab[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check]
							[U9001_Save_sys.U9001_SYS.language],LISTVALUE2,FIRSTLINE+1*SPACE1);
			if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time)
            {
                Hex_Format(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time,1,4,FALSE);
                strcat(DispBuf,"s");
                GUI_DispStringAt(DispBuf,LISTVALUE1,FIRSTLINE+2*SPACE1);//测试时间
            }
            else//
            {
                GUI_DispStringAt(Continue_Tab[0][U9001_Save_sys.U9001_SYS.language],LISTVALUE1,FIRSTLINE+2*SPACE1);//连续测试
            }
            break;
        default:
            break;
        
    
    }
  
	

}
void Disp_Test_SetConfig_Item(void)
{
	uint32_t i;
//	Disp_TestSetScreen(1);
//    Disp_mainitem(2);

	Disp_Fastbutton(1);
    
//	Disp_Fastset_Disp(1);
	Disp_mainitem(2);
    GUI_SetColor(White);
	GUI_SetBkColor(LCD_COLOR_TEST_BACK);
//    U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter=1;
    for(i=0;i<(sizeof(SetupConfig_Tab)/(sizeof(SetupConfig_Tab[0])));i++)
    {
        if((i%2)==0)
        {
            
            
            GUI_DispStringAt(SetupConfig_Tab[i][U9001_Save_sys.U9001_SYS.language],LIST1, FIRSTLINE+SPACE1*((i+1)/2));

        }
        else
        {

            
             GUI_DispStringAt(SetupConfig_Tab[i][U9001_Save_sys.U9001_SYS.language],LIST2, FIRSTLINE+SPACE1*((i-1)/2));
            
            
        }	
    }
  
	

}



//const List_Scan[][6][2]=
//{
//	{{},{},{},{},{}},
//	{{},{},{},{},{}},
//	{{},{},{},{},{}},
//	{{},{},{},{},{}},
//	{{},{},{},{},{}},
//	{{},{},{},{},{}},
//	{},
//	{},
//	{},
//	{}


//};

//void Disp_List_Select(Button_Page_Typedef* Button_Page,vu8 i)
//{
//	vu8 Black_Select;
////	vu8 i;
////	for(i=0;i<10;i++)
//	{
//		Black_Select=(Button_Page->index==i)?1:0;
//		if(Black_Select)
//		{
//			Colour.black=LCD_COLOR_SELECT;
//		
//		}
//		else
//		{
//			Colour.black=LCD_COLOR_TEST_BACK;
//		}
//		//if()	
//		LCD_DrawRect( 32+Button_Page->page*60, FIRSTLINE+SPACE1+3+16*(Button_Page->index-1),88+Button_Page->page*60 ,
//			FIRSTLINE+SPACE1+3+16*Button_Page->index+16 , Colour.black ) ;
//		//LCD_DrawRect( LIST1+88, FIRSTLINE-2,SELECT_1END , FIRSTLINE+SPACE1-4 , Colour.black ) ;//SPACE1
////		WriteString_16(LIST1+88, FIRSTLINE, User_FUNC[SaveData.Main_Func.Param.test],  1);//增加算法  把顺序改过来
//	}
//}
//void Disp_List_Select(Button_Page_Typedef* Button_Page)
//{
//	static vu32 pos[2];
//	if(Button_Page->index==0&&Button_Page->page==0)
//	{
//		Colour.black=LCD_COLOR_TEST_BACK;
//	
//	}
//	else
//		if(Button_Page->index==1&&Button_Page->page==0)//
//		{
//			Colour.black=LCD_COLOR_SELECT;
//			LCD_DrawRect( 48, FIRSTLINE,88 , FIRSTLINE+16 , Colour.black ) ;
//			Colour.black=LCD_COLOR_TEST_BACK;
////			pos[0]=Button_Page->index;
////			pos[1]=Button_Page->page;
//		
//		}
//		else
//		{
//			Colour.black=LCD_COLOR_SELECT;
//			LCD_DrawRect( 32+Button_Page->page*60, FIRSTLINE+SPACE1+3+16*(Button_Page->index-1),88+Button_Page->page*60 ,
//			FIRSTLINE+SPACE1+3+16*Button_Page->index+16 , Colour.black ) ;
//			
//			Colour.black=LCD_COLOR_TEST_BACK;
////			if(pos[0]==1&&pos[1]==0)
////			{
////				LCD_DrawRect( 48, FIRSTLINE,88 , FIRSTLINE+16 , Colour.black ) ;
////			}
////			else
////			{
////			
////				LCD_DrawRect( 32+pos[1]*60, FIRSTLINE+SPACE1+3+16*pos[0],88+pos[1]*60 ,
////			FIRSTLINE+SPACE1+3+16*pos[0]+16 , Colour.black ) ;
////			}
////			pos[0]=Button_Page->index;
////			pos[1]=Button_Page->page;
//		
//		}



//}

//测试的时候显示的设置值
void Disp_Test_value(uint8_t list)
{
//	vu32 flag;
//	vu32 i;
//	vu32 xpose;
	vu32 Black_Select;
//	vu32 Select_color;
		
	Black_Select=(list==1)?1:0;//组别
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	LCD_DrawRect( LIST1+DATA_SPACE1, SPACE1*2,SELECT_1END , SPACE1*3 , Colour.black ) ;//SPACE1
    PutChar( LIST1+DATA_SPACE1+24, SPACE1*2, Tft_5520.Group+'1', Colour.Fword, Colour.black );
	//WriteString_16(LIST1+88, FIRSTLINE, Test_Tirpvalue[Save_Res.Set_Data.trip],  0);//增加算法  把顺序改过来
	
//电阻上限
	Black_Select=(list==2)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
		

	Hex_Format(Tft_5520.Set_Value[Tft_5520.Group].Hi_Limit , 0 , 4 , 0);
    LCD_DrawRect( LIST1+DATA_SPACE1, FIRSTLINE+SPACE1*2 ,SELECT_1END , FIRSTLINE+SPACE1*3 -4, Colour.black ) ;
	WriteString_16(LIST1+DATA_SPACE1+8, FIRSTLINE+SPACE1*2, DispBuf,  0);

		WriteString_16(LIST1+90+8*6, FIRSTLINE+SPACE1*2, "MΩ",  0);

//电阻下限
	Black_Select=(list==3)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}

	
	Hex_Format(Tft_5520.Set_Value[Tft_5520.Group].Lo_Limit , 0 , 4 , 0);
     LCD_DrawRect( LIST1+DATA_SPACE1, FIRSTLINE+SPACE1*3 ,SELECT_1END , FIRSTLINE+SPACE1*4-4 , Colour.black ) ;
	WriteString_16(LIST1+DATA_SPACE1+8, FIRSTLINE+SPACE1*3, DispBuf,  0);
	
		WriteString_16(LIST1+90+8*6, FIRSTLINE+SPACE1*3, "MΩ",  0);
//量程
	Black_Select=(list==4)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
    if(GetSystemStatus()==SYS_STATUS_IDEM)
    {
		
	LCD_DrawRect( LIST2+DATA_SPACE1+4, FIRSTLINE-2+SPACE1,SELECT_2END , FIRSTLINE+SPACE1*2-4 , Colour.black ) ;//SPACE1

	WriteString_16(LIST2+DATA_SPACE1+4, FIRSTLINE+SPACE1, User_Range[Tft_5520.Set_Value[Tft_5520.Group].Range],  0);
	}
	
//输出电压
	Black_Select=(list==5)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}

	Hex_Format(Tft_5520.Set_Value[Tft_5520.Group].voltage , 0 , 4 , 0);
    LCD_DrawRect( LIST2+DATA_SPACE1, FIRSTLINE+SPACE1*2,SELECT_2END , FIRSTLINE+SPACE1*3-4 , Colour.black ) ;
	WriteString_16(LIST2+DATA_SPACE1+10+8, FIRSTLINE+SPACE1*2, DispBuf,  0);
	WriteString_16(LIST2+DATA_SPACE1+8*7, FIRSTLINE+SPACE1*2, " V",  0);
	
//判定时间

	Black_Select=(list==6)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}

	Hex_Format(Tft_5520.Set_Value[Tft_5520.Group].Delay_Time , 1 , 4 , 0);
	WriteString_16(LIST2+DATA_SPACE1+8, FIRSTLINE+SPACE1*3, DispBuf,  0);
	WriteString_16(LIST2+DATA_SPACE1+8*7, FIRSTLINE+SPACE1*3, " s",  0);


}
//显示设置参数的值Setup_Valueall
void DispSet_value(uint8_t list)
{
	vu32 i;
//	vu32 Black_Select;
    u8 num=(MAX_SETP[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter]+1)/2;
    UnitNum_typedef Disp_Value;
    for(i=1;i<MAX_SETP[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter]+1;i++)
    {
        if(i==list)
        {
            GUI_SetColor(LCD_COLOR_SELECT);
        }
        else
        {
             GUI_SetColor(LCD_COLOR_TEST_BACK);
        }//
        if(i%2)
            GUI_FillRect(LISTVALU_RETC,FIRSTLINE_RETC+SPACE1*(i-1)/2,LISTVALU_RETC+90,FIRSTLINE_RETC+SPACE1*((i-1)/2+1));
        else
            GUI_FillRect(LISTVALUE2_RETC,FIRSTLINE_RETC+SPACE1*((i-1)/2),LISTVALUE2_RETC+90,FIRSTLINE_RETC+SPACE1*((i-1)/2+1));
            
        
        
    }
    GUI_SetTextMode(GUI_TEXTMODE_TRANS);
    GUI_SetColor(GUI_WHITE);
    sprintf(DispBuf,"%d/%d",U9001_Save_sys.U9001_save.current_step,U9001_Save_sys.U9001_save.all_step);
    GUI_DispStringAt(DispBuf,LISTVALUE1,FIRSTLINE);//显示步骤
    GUI_DispStringAt(TestPara[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter],LISTVALUE2,FIRSTLINE+0*SPACE1);//参数
    switch(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter)
    {
        case AC:
    
    
            Hex_Format(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out,3,4,FALSE);
            strcat(DispBuf,"kV");
            GUI_DispStringAt(DispBuf,LISTVALUE1,FIRSTLINE+SPACE1);//电压
            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time)
            {
                Hex_Format(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time,1,4,FALSE);
                strcat(DispBuf,"s");
                GUI_DispStringAt(DispBuf,LISTVALUE1,FIRSTLINE+2*SPACE1);//测试时间
            }
            else//
            {
                GUI_DispStringAt(Continue_Tab[0][U9001_Save_sys.U9001_SYS.language],LISTVALUE1,FIRSTLINE+2*SPACE1);//连续测试
            }
            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].rise_time!=0)
            {
                Hex_Format(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].rise_time,1,4,FALSE);
                strcat(DispBuf,"s");
                GUI_DispStringAt(DispBuf,LISTVALUE1,FIRSTLINE+3*SPACE1);//缓升时间时间 
            }
            else
            {
            
                GUI_DispStringAt(Switch_Tab[0][U9001_Save_sys.U9001_SYS.language],LISTVALUE1,FIRSTLINE+3*SPACE1);
            }
    
            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].drop_time!=0)
            {
                Hex_Format(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].drop_time,1,4,FALSE);
                strcat(DispBuf,"s");
                GUI_DispStringAt(DispBuf,LISTVALUE1,FIRSTLINE+4*SPACE1);//下降时间时间 
            }
            else
            {
            
                GUI_DispStringAt(Switch_Tab[0][U9001_Save_sys.U9001_SYS.language],LISTVALUE1,FIRSTLINE+4*SPACE1);
            }

    
	
            Disp_Value=IntToStr_mA(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper);
           Hex_Format(Disp_Value.num,Disp_Value.dot,4,FALSE);
            strcat(DispBuf,TEST_UNIT[1][0]);
            GUI_DispStringAt(DispBuf,LISTVALUE2,FIRSTLINE+1*SPACE1);//上限
    
            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower)
            {
                Disp_Value=IntToStr_mA(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower);
                Hex_Format(Disp_Value.num,Disp_Value.dot,4,FALSE);
                strcat(DispBuf,TEST_UNIT[1][0]);
               
                GUI_DispStringAt(DispBuf,LISTVALUE2,FIRSTLINE+2*SPACE1);//下限
            }
            else
                GUI_DispStringAt(Switch_Tab[0][U9001_Save_sys.U9001_SYS.language],LISTVALUE2,FIRSTLINE+2*SPACE1);
            if(U9001_Save_sys.U9001_Testconfg.ARC_mode)//等级
            {
                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc>9)
                    U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc=9;
                if((U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc>0)&&(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc<=9))
                    GUI_DispDecAt(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc,LISTVALUE2,FIRSTLINE+3*SPACE1,1);
                else
                    GUI_DispStringAt(Switch_Tab[0][U9001_Save_sys.U9001_SYS.language],LISTVALUE2,FIRSTLINE+3*SPACE1);
            }
            else
            {
                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc)
                {
                    Hex_Format(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc,1,3,FALSE);
                    strcat(DispBuf,"mA");
                    
                    GUI_DispStringAt(DispBuf,LISTVALUE2,FIRSTLINE+3*SPACE1);
                
                }
                else
                    GUI_DispStringAt(Switch_Tab[0][U9001_Save_sys.U9001_SYS.language],LISTVALUE2,FIRSTLINE+3*SPACE1);
            }
		
            break;
        case DC:
            
    
            Hex_Format(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out,3,4,FALSE);
            strcat(DispBuf,"kV");
            GUI_DispStringAt(DispBuf,LISTVALUE1,FIRSTLINE+SPACE1);//电压
            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time)
            {
                Hex_Format(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time,1,4,FALSE);
                strcat(DispBuf,"s");
                GUI_DispStringAt(DispBuf,LISTVALUE1,FIRSTLINE+2*SPACE1);//测试时间
            }
            else//
            {
                GUI_DispStringAt(Continue_Tab[0][U9001_Save_sys.U9001_SYS.language],LISTVALUE1,FIRSTLINE+2*SPACE1);//测试时间
            }
            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].rise_time!=0)
            {
                Hex_Format(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].rise_time,1,4,FALSE);
                strcat(DispBuf,"s");
                GUI_DispStringAt(DispBuf,LISTVALUE1,FIRSTLINE+3*SPACE1);//缓升时间时间 
            }
            else
            {
            
                GUI_DispStringAt(Switch_Tab[0][U9001_Save_sys.U9001_SYS.language],LISTVALUE1,FIRSTLINE+3*SPACE1);
            }
    
            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].drop_time!=0)
            {
                Hex_Format(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].drop_time,1,4,FALSE);
                strcat(DispBuf,"s");
                GUI_DispStringAt(DispBuf,LISTVALUE1,FIRSTLINE+4*SPACE1);//下降时间时间 
            }
            else
            {
            
                GUI_DispStringAt(Switch_Tab[0][U9001_Save_sys.U9001_SYS.language],LISTVALUE1,FIRSTLINE+4*SPACE1);
            }
            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check)
            {
                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check>=3500)
                    U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check=3500;
                Hex_Format(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check,1,4,FALSE);
                strcat(DispBuf,"uA");
                GUI_DispStringAt(DispBuf,LISTVALUE1,FIRSTLINE+5*SPACE1);//检测
            
            }
            else
                GUI_DispStringAt(Switch_Tab[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check][U9001_Save_sys.U9001_SYS.language],LISTVALUE1,FIRSTLINE+5*SPACE1);
            Disp_Value=IntToStr_mA(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper);
            
            Hex_Format(Disp_Value.num,Disp_Value.dot,4,FALSE);
            strcat(DispBuf,TEST_UNIT[1][0]);//strcat(DispBuf,TEST_UNIT[1][0]);
            GUI_DispStringAt(DispBuf,LISTVALUE2,FIRSTLINE+1*SPACE1);//上限
    
            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower)
            {
                Disp_Value=IntToStr_mA(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower);
                Hex_Format(Disp_Value.num,Disp_Value.dot,4,FALSE);
                strcat(DispBuf,TEST_UNIT[1][0]);
                GUI_DispStringAt(DispBuf,LISTVALUE2,FIRSTLINE+2*SPACE1);//下限
            }
            else
                GUI_DispStringAt(Switch_Tab[0][U9001_Save_sys.U9001_SYS.language],LISTVALUE2,FIRSTLINE+2*SPACE1);
            
             if(U9001_Save_sys.U9001_Testconfg.ARC_mode)//电弧等级
            {
                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc>9)
                    U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc=9;
                if((U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc>0)&&(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc<=9))
                    GUI_DispDecAt(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc,LISTVALUE2,FIRSTLINE+3*SPACE1,1);
                else
                    GUI_DispStringAt(Switch_Tab[0][U9001_Save_sys.U9001_SYS.language],LISTVALUE2,FIRSTLINE+3*SPACE1);
            }
            else
            {
                if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc)
                {
                    Hex_Format(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc,1,4,FALSE);
                    strcat(DispBuf,"mA");
                    GUI_DispStringAt(DispBuf,LISTVALUE2,FIRSTLINE+3*SPACE1);
                
                }
                else
                    GUI_DispStringAt(Switch_Tab[0][U9001_Save_sys.U9001_SYS.language],LISTVALUE2,FIRSTLINE+3*SPACE1);
            }
            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last)
            {
                Hex_Format(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last,1,4,FALSE);
                strcat(DispBuf,"s");
                GUI_DispStringAt(DispBuf,LISTVALUE2,FIRSTLINE+4*SPACE1);
            }else
            {
                GUI_DispStringAt(Switch_Tab[0][U9001_Save_sys.U9001_SYS.language],LISTVALUE2,FIRSTLINE+4*SPACE1);
            }
            break;
        case IR:
            Hex_Format(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out,3,4,FALSE);
            strcat(DispBuf,"kV");
            GUI_DispStringAt(DispBuf,LISTVALUE1,FIRSTLINE+SPACE1);//电压
            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time)
            {
                Hex_Format(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time,1,4,FALSE);
                strcat(DispBuf,"s");
                GUI_DispStringAt(DispBuf,LISTVALUE1,FIRSTLINE+2*SPACE1);//测试时间
            }
            else//
            {
                GUI_DispStringAt(Continue_Tab[0][U9001_Save_sys.U9001_SYS.language],LISTVALUE1,FIRSTLINE+2*SPACE1);//测试时间
            }
            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].rise_time!=0)
            {
                Hex_Format(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].rise_time,1,4,FALSE);
                strcat(DispBuf,"s");
                GUI_DispStringAt(DispBuf,LISTVALUE1,FIRSTLINE+3*SPACE1);//缓升时间时间 
            }
            else
            {
            
                GUI_DispStringAt(Switch_Tab[0][U9001_Save_sys.U9001_SYS.language],LISTVALUE1,FIRSTLINE+3*SPACE1);
            }
    
            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].drop_time!=0)
            {
                Hex_Format(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].drop_time,1,4,FALSE);
                strcat(DispBuf,"s");
                GUI_DispStringAt(DispBuf,LISTVALUE1,FIRSTLINE+4*SPACE1);//下降时间时间 
            }
            else
            {
            
                GUI_DispStringAt(Switch_Tab[0][U9001_Save_sys.U9001_SYS.language],LISTVALUE1,FIRSTLINE+4*SPACE1);
            }
           if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper)
           {
                Disp_Value=IntToStr(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper);
                Hex_Format(Disp_Value.num,Disp_Value.dot,4,FALSE);
                strcat(DispBuf,TEST_UNIT[Disp_Value.uint][1]);

                GUI_DispStringAt(DispBuf,LISTVALUE2,FIRSTLINE+1*SPACE1);//上限
           }
           else
               GUI_DispStringAt(Switch_Tab[0][U9001_Save_sys.U9001_SYS.language],LISTVALUE2,FIRSTLINE+1*SPACE1);
          
            Disp_Value=IntToStr(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower);
            Hex_Format(Disp_Value.num,Disp_Value.dot,4,FALSE);
            strcat(DispBuf,TEST_UNIT[Disp_Value.uint][1]);
            GUI_DispStringAt(DispBuf,LISTVALUE2,FIRSTLINE+2*SPACE1);//下限
           
            GUI_DispStringAt(Setup_Range_Tab[U9001_Save_sys.U9001_SYS.language][U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc],LISTVALUE2,FIRSTLINE+3*SPACE1);
       
           GUI_DispStringAt(Switch_Tab[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check][U9001_Save_sys.U9001_SYS.language],LISTVALUE2,FIRSTLINE+4*SPACE1);
            
            break;
        case OS:
            sprintf(DispBuf,"%3d%%",U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower);
            GUI_DispStringAt(DispBuf,LISTVALUE1,FIRSTLINE+SPACE1);
            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper>=100)
            {
                sprintf(DispBuf,"%3d%%",U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper);
                GUI_DispStringAt(DispBuf,LISTVALUE1,FIRSTLINE+2*SPACE1);
            }
            else
               GUI_DispStringAt(Switch_Tab[0][U9001_Save_sys.U9001_SYS.language],LISTVALUE1,FIRSTLINE+2*SPACE1); 
            
            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last)
            {
//                Disp_Value=FToStr(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last);
				if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last >= 1000)
				{
					Hex_Format(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last/10,2,4,0);
				}else{
					Hex_Format(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].equa_last,3,4,0);
				}
                
                strcat(DispBuf,Disp_Unit[1]);
                GUI_DispStringAt(DispBuf,LISTVALUE2,FIRSTLINE+1*SPACE1);
            }
            else
               GUI_DispStringAt("NONE",LISTVALUE2,FIRSTLINE+1*SPACE1); 
            break;
        case PA:
            GUI_DispStringAt("PAUSE",LISTVALUE1,FIRSTLINE+SPACE1);
		
			GUI_DispStringAt(Switch_Tab[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].check]
							[U9001_Save_sys.U9001_SYS.language],LISTVALUE2,FIRSTLINE+1*SPACE1);
			if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time)
            {
                Hex_Format(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time,1,4,FALSE);
                strcat(DispBuf,"s");
                GUI_DispStringAt(DispBuf,LISTVALUE1,FIRSTLINE+2*SPACE1);//测试时间
            }
            else//
            {
                GUI_DispStringAt(Continue_Tab[0][U9001_Save_sys.U9001_SYS.language],LISTVALUE1,FIRSTLINE+2*SPACE1);//连续测试
            }
            break;
        default:
            break;
        
    
    }
    
    dispSetupButtonvalue(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter, list);
    //增加最下面的提示

}

void DispSetConfig_value(uint8_t list)
{
	vu32 i;
	vu32 Black_Select;
    u8 num;
    u16* pt;
    pt=(u16*)&U9001_Save_sys.U9001_Testconfg;
    num=sizeof(SetupConfig_Tab)/sizeof(SetupConfig_Tab[0]);
    for(i=1;i<sizeof(SetupConfig_Tab)+1;i++)
    {
        if(i==list)
        {
            GUI_SetColor(LCD_COLOR_SELECT);
        }
        else
        {
             GUI_SetColor(LCD_COLOR_TEST_BACK);
        }//
        if(i<=(num+1)/2)
            GUI_FillRect(LISTVALU_RETC+20,FIRSTLINE_RETC+SPACE1*(i-1),LISTVALU_RETC+90,FIRSTLINE_RETC+SPACE1*i);
        else
            GUI_FillRect(LISTVALUE2_RETC+20,FIRSTLINE_RETC+SPACE1*(i-(num+1)/2-1),LISTVALUE2_RETC+90,FIRSTLINE_RETC+SPACE1*(i-(num+1)/2));
            
        
        
    }
    GUI_SetTextMode(GUI_TEXTMODE_TRANS);
    GUI_SetColor(GUI_WHITE);
    for(i=0;i<3;i++)
    {
        if((i==1)&&(*(pt+i)==1000))
        {
            GUI_DispStringAt("KEY",LISTVALUE1,FIRSTLINE+i*SPACE1);
        }
        else
        {
            Hex_Format(*(pt+i),1,3,FALSE);
            strcat(DispBuf,"s");
            GUI_DispStringAt(DispBuf,LISTVALUE1,FIRSTLINE+i*SPACE1);//合格保持时间
        }
    }
    
     
 
   
    
    GUI_DispStringAt(Switch_Tab[1][U9001_Save_sys.U9001_SYS.language],LISTVALUE1,FIRSTLINE+3*SPACE1);//下降时间时间 

 
    
     GUI_DispStringAt(FREQ[*(pt+4)],LISTVALUE1,FIRSTLINE+4*SPACE1);
    GUI_DispStringAt(ArvMode_Tab[*(pt+5)][U9001_Save_sys.U9001_SYS.language],LISTVALUE1,FIRSTLINE+5*SPACE1);
	if(*(pt+6)==0)
	{
		 GUI_DispStringAt(Switch_Tab[0][U9001_Save_sys.U9001_SYS.language],LISTVALUE1,FIRSTLINE+6*SPACE1);
		
	}else
    {
        Hex_Format(*(pt+6),0,2,FALSE);
//        strcat(DispBuf,"s");
        GUI_DispStringAt(DispBuf,LISTVALUE1,FIRSTLINE+6*SPACE1);//合格保持时间
    }
    GUI_DispStringAt(Switch_Tab[*(pt+7)][U9001_Save_sys.U9001_SYS.language],LISTVALUE1,FIRSTLINE+7*SPACE1);
       
    

    GUI_DispStringAt(Switch_Tab[*(pt+8)][U9001_Save_sys.U9001_SYS.language],LISTVALUE2,FIRSTLINE+0*SPACE1);
    GUI_DispStringAt(Switch_Tab[*(pt+9)][U9001_Save_sys.U9001_SYS.language],LISTVALUE2,FIRSTLINE+1*SPACE1);
    GUI_DispStringAt(FailMode_Tab[*(pt+10)][U9001_Save_sys.U9001_SYS.language],LISTVALUE2,FIRSTLINE+2*SPACE1);
    GUI_DispStringAt(Switch_Tab[*(pt+11)][U9001_Save_sys.U9001_SYS.language],LISTVALUE2,FIRSTLINE+3*SPACE1);
    GUI_DispStringAt(Switch_Tab[*(pt+12)][U9001_Save_sys.U9001_SYS.language],LISTVALUE2,FIRSTLINE+4*SPACE1);
    GUI_DispStringAt(Switch_Tab[*(pt+13)][U9001_Save_sys.U9001_SYS.language],LISTVALUE2,FIRSTLINE+5*SPACE1);
    if(*(pt+14/**2*/))
    {
        Hex_Format(*(pt+14),1,2,FALSE);
        strcat(DispBuf,"s");
        GUI_DispStringAt(DispBuf,LISTVALUE2,FIRSTLINE+6*SPACE1);//合格保持时间  delay_time
    }
    else
    {
        GUI_DispStringAt(Switch_Tab[0][U9001_Save_sys.U9001_SYS.language],LISTVALUE2,FIRSTLINE+6*SPACE1);
    }
    
  
    
    DispSetup_ConfigButtonvalue(list);
    //增加最下面的提示

}
void DispSYSConfig_value(void)
{
	vu32 i;
//	vu32 Black_Select;

    GUI_SetTextMode(GUI_TEXTMODE_TRANS);
    GUI_SetColor(GUI_WHITE);
    if(U9001_Save_sys.jkflag == 0)
		{
			GUI_DispStringAt(VERSION_Tab1[SOFTWARE_VERSION-1][U9001_Save_sys.U9001_SYS.language],LISTVALUE1,FIRSTLINE+1*SPACE1);
		}else{
			GUI_DispStringAt(VERSION_Tab[SOFTWARE_VERSION-1][U9001_Save_sys.U9001_SYS.language],LISTVALUE1,FIRSTLINE+1*SPACE1);
			GUI_DispStringAt(SYS_NAME_Tab[1][U9001_Save_sys.U9001_SYS.language],LISTVALUE1,FIRSTLINE+4*SPACE1);
			GUI_DispStringAt(SYS_NAME_Tab[2][U9001_Save_sys.U9001_SYS.language],LISTVALUE1,FIRSTLINE+5*SPACE1);
		}
 
    
//    Hex_Format(U9001_save_sys.U9001_Testconfg.delay_time,1,3,FALSE);
//    strcat(DispBuf,"s");
//    GUI_DispStringAt(DispBuf,LISTVALUE1,FIRSTLINE+3*SPACE1);//合格保持时间  delay_time
    
    GUI_DispStringAt(SYS_NAME_Tab[0][U9001_Save_sys.U9001_SYS.language],LISTVALUE1,FIRSTLINE+3*SPACE1);//下降时间时间 

 
    
     
    

    dispSYSButtonvalue();
    //增加最下面的提示

}

const vu8 Debug_Dot[DEBUG_RANGE]={0,3,2,1,0};
void Disp_Debug_Reference(uint8_t list,uint16_t value)
{
	
   // vu16 value=0;;        
	Hex_Format(value, Debug_Dot[list-1] , 4 , 0);
	WriteString_16(LIST1+160+160, FIRSTLINE+SPACE1*(list+1), DispBuf,  1);



}

//显示设置参数的值Setup_Valueall
void Disp_Debug_value(uint8_t list)
{
	vu32 i;
	for(i=0;i<DEBUG_RANGE;i++)
	{
		if(list==(i+1))
		{
			Colour.black=LCD_COLOR_SELECT;
		
		}
		else
		{
			Colour.black=LCD_COLOR_TEST_BACK;
		}

		LCD_DrawRect( LIST1+160, FIRSTLINE+SPACE1*(i+2)-2,SELECT_1END+40 , FIRSTLINE+SPACE1*(i+3)-4 , Colour.black ) ;//SPACE1
		Hex_Format(Cal[i].Num, Debug_Dot[i] , 4 , 0);
        WriteString_16(LIST1+160, FIRSTLINE+SPACE1*(i+2), DispBuf,  1);
	}
	Colour.black=LCD_COLOR_TEST_BACK;
}

//void Disp_Sys(void)
//{
//	vu8 i;
//	Disp_Button_SysSet();
//	for(i=0;i<sizeof(Sys_Disp_Main);i++)
//	{
//		WriteString_16(LIST1+90, FIRSTLINE+SPACE1*i+2, Sys_Disp_Main[i],  0);
//	}


//}
void Disp_Set_Time(uint8_t list)
{
	vu32 Black_Select;
	uint8_t buff[10];
//	char buff2[10];
	RTC_TIME_Type pFullTime;
	Colour.black=LCD_COLOR_TEST_BACK;
	//RTC_DATA_Type 
	RTC_GetFullTime (LPC_RTC, &pFullTime);
	sprintf((char *)buff ,"    :  :  ");
	WriteString_16(LIST1+90, FIRSTLINE+6*SPACE1, buff,  0);
	
	sprintf((char *)buff ,"  :  :  ");
	WriteString_16(LIST1+90+20, FIRSTLINE+7*SPACE1, buff,  0);
	Black_Select=(list==6)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	sprintf((char *)buff ,"%02d",pFullTime.YEAR);	
	LCD_DrawRect( LIST1+90, FIRSTLINE+6*SPACE1,LIST1+90+40 , FIRSTLINE+SPACE1*7-4 , Colour.black ) ;//SPACE1

	WriteString_16(LIST1+90, FIRSTLINE+SPACE1*6, buff,  0);
	
	Black_Select=(list==7)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	sprintf((char *)buff ,"%02d",pFullTime.MONTH);		
	LCD_DrawRect( LIST1+90+50, FIRSTLINE+SPACE1*6,LIST1+90+50+20 , FIRSTLINE+SPACE1*7-4 , Colour.black ) ;//SPACE1
	WriteString_16(LIST1+90+50, FIRSTLINE+SPACE1*6, buff,  0);
	//WriteString_16(LIST1+90, FIRSTLINE+SPACE1*6, Test_Compvalue[Tft_5520.Sys_Set.Uart],  0);//
	
	Black_Select=(list==8)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	sprintf((char *)buff ,"%02d",pFullTime.DOM);		
	LCD_DrawRect( LIST1+90+50+30, FIRSTLINE+SPACE1*6,LIST1+90+50+30+20 , FIRSTLINE+SPACE1*7-4 , Colour.black ) ;//SPACE1
	WriteString_16(LIST1+90+50+30, FIRSTLINE+SPACE1*6, buff,  0);//
	
	Black_Select=(list==9)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	sprintf((char *)buff ,"%02d",pFullTime.HOUR);	
	LCD_DrawRect( LIST1+90+20, FIRSTLINE+SPACE1*7,LIST1+90+40  , FIRSTLINE+SPACE1*8-4 , Colour.black ) ;//SPACE1
	WriteString_16(LIST1+90+20, FIRSTLINE+SPACE1*7, buff,  0);//
	
	Black_Select=(list==10)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	sprintf((char *)buff ,"%02d",pFullTime.MIN);	
	LCD_DrawRect( LIST1+90+50, FIRSTLINE+SPACE1*7,LIST1+90+50+20, FIRSTLINE+SPACE1*8-4 , Colour.black ) ;//SPACE1
	WriteString_16(LIST1+90+50, FIRSTLINE+SPACE1*7, buff,  0);//
	
	Black_Select=(list==11)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	sprintf((char *)buff ,"%02d",pFullTime.SEC);	
	LCD_DrawRect( LIST1+90+50+30, FIRSTLINE+SPACE1*7,LIST1+90+50+30+20 , FIRSTLINE+SPACE1*8-4 , Colour.black ) ;//SPACE1
	WriteString_16(LIST1+90+50+30, FIRSTLINE+SPACE1*7, buff,  0);//


}
//显示设置参数的值Setup_Valueall
void Disp_SysSet_value(uint8_t list)
{
	vu32 i;
//	vu32 Black_Select;
    u8 num;
    num=7;
    for(i=1;i<num+1;i++)
    {
        if(i==list)
        {
            GUI_SetColor(LCD_COLOR_SELECT);
        }
        else
        {
             GUI_SetColor(LCD_COLOR_TEST_BACK);
        }//
        if(i<=4)
            GUI_FillRect(LISTVALU_RETC+20,FIRSTLINE_RETC+SPACE1*((i-1)),LISTVALU_RETC+70,FIRSTLINE_RETC+SPACE1*((i)));
        else
            GUI_FillRect(LISTVALUE2_RETC+20,FIRSTLINE_RETC+SPACE1*(i-5),LISTVALUE2_RETC+75,FIRSTLINE_RETC+SPACE1*(i-4));
            
        
        
    }
    GUI_SetTextMode(GUI_TEXTMODE_TRANS);
    GUI_SetColor(GUI_WHITE);
   
//    sprintf(DispBuf,"%d/%d",U9001_Save_sys.U9001_save.current_step,U9001_Save_sys.U9001_save.all_step);
    GUI_DispStringAt(Sys_SetValue[0][U9001_Save_sys.U9001_SYS.language][U9001_Save_sys.U9001_SYS.language],LISTVALUE1,FIRSTLINE);//语言
    GUI_DispStringAt(Sys_SetValue[1][U9001_Save_sys.U9001_SYS.language][U9001_Save_sys.U9001_SYS.pass_beep],LISTVALUE1,FIRSTLINE+1*SPACE1);//合格讯响
    GUI_DispStringAt(Sys_SetValue[2][U9001_Save_sys.U9001_SYS.language][U9001_Save_sys.U9001_SYS.fail_beep],LISTVALUE1,FIRSTLINE+2*SPACE1);//合格讯响
    GUI_DispStringAt(Sys_SetValue[3][U9001_Save_sys.U9001_SYS.language][U9001_Save_sys.U9001_SYS.key_beep],LISTVALUE1,FIRSTLINE+3*SPACE1);
//    GUI_DispStringAt(Sys_SetValue[4][U9001_save_sys.U9001_SYS.language][U9001_save_sys.U9001_SYS.beep],LISTVALUE1,FIRSTLINE+4*SPACE1);
//    GUI_DispStringAt(U9001_save_sys.U9001_SYS.password,LISTVALUE1,FIRSTLINE+5*SPACE1);
    
    GUI_DispStringAt(Sys_SetValue[6][U9001_Save_sys.U9001_SYS.language][U9001_Save_sys.U9001_SYS.bussmode],LISTVALUE2,FIRSTLINE);//语言
    GUI_DispDecAt(U9001_Save_sys.U9001_SYS.buss_addr,LISTVALUE2,FIRSTLINE+1*SPACE1,3);
    GUI_DispStringAt(Sys_SetValue[8][U9001_Save_sys.U9001_SYS.language][U9001_Save_sys.U9001_SYS.buard],LISTVALUE2,FIRSTLINE+2*SPACE1);
//    GUI_DispStringAt(Sys_SetValue[10][U9001_save_sys.U9001_SYS.language][U9001_save_sys.U9001_SYS.date_bit],LISTVALUE2,FIRSTLINE+3*SPACE1);
//    GUI_DispStringAt(Sys_SetValue[9][U9001_save_sys.U9001_SYS.language][U9001_save_sys.U9001_SYS.stop_bit],LISTVALUE2,FIRSTLINE+4*SPACE1);
//    GUI_DispStringAt(Sys_SetValue[10][U9001_save_sys.U9001_SYS.language][U9001_save_sys.U9001_SYS.ord_bit],LISTVALUE2,FIRSTLINE+5*SPACE1);

    
    dispSYSSetButtonvalue(list);

}
//用户校正界面子函数
void Disp_Usercheck_Screen(void)
{
	Disp_TopBar_Color(0);
	//Disp_Fastbutton();//显示快速按键
//	Disp_TestSet_MidRect();//显示中间色块
}
void Disp_UserCheck_Item(void)
{
	uint32_t i;
	Disp_Usercheck_Screen();
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BAR;
	//WriteString_16(0, 4, All_TopName[5],  0);
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BACK;//User_Check_main
	
	for(i=0;i<(sizeof(User_Check_main)/(sizeof(User_Check_main[0])));i++)
	{
		WriteString_16(LIST1+160*i, FIRSTLINE+SPACE1, User_Check_main[i],  0);

	}
	
	for(i=0;i<(sizeof(User_Check_Item)/(sizeof(User_Check_Item[0])));i++)
	{
		WriteString_16(LIST1, FIRSTLINE+SPACE1*(i+2), User_Check_Item[i],  0);

	}
//	else
//	{
//		WriteString_16(LIST2, FIRSTLINE+SPACE1*(i-sizeof(User_Check_Item)/(sizeof(User_Check_Item[0]))/2), User_Check_Item[i],  0);
//	}	
//	Disp_Button_TestSet(0);
}
//极限列表设置子函数
void Disp_LimitList_MidRect(void)
{

	LCD_DrawRect( 0, 90+32,479 , 150+32 , LCD_COLOR_TEST_MID ) ;
//	LCD_DrawRect( 0, 195,479 , 227 , LCD_COLOR_TEST_MID ) ;
}
void Disp_LimitList_Screen(void)
{
	Disp_TopBar_Color(0);
//	Disp_Fastbutton();//显示快速按键
	Disp_LimitList_MidRect();//显示中间色块	 User_LimitList_Item
}

//列表模式选择的子函数
void Disp_ListScan_MidRect(void)
{

	LCD_DrawRect( 0, 90+32,479 , 150+32 , LCD_COLOR_TEST_MID ) ;
//	LCD_DrawRect( 0, 195,479 , 227 , LCD_COLOR_TEST_MID ) ;
}
void Disp_ListScan_Screen(void)
{
	Disp_TopBar_Color(0);
//	Disp_Fastbutton();//显示快速按键	  
//	Disp_ListScan_MidRect();//显示中间色块	 User_LimitList_Item
}
void Disp_ListScan_Item(void)
{
	uint32_t i;
	Disp_ListScan_Screen();
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BAR;
	WriteString_16(0, 4, All_TopName[7],  0);
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BACK;
	WriteString_16(LIST1, FIRSTLINE, User_ListScan_Item[0],  0);
	for(i=1;i<(sizeof(User_ListScan_Item)/(sizeof(User_ListScan_Item[0])));i++)
		WriteString_16(LIST1+30+(i-1)*77, FIRSTLINE+SPACE1, User_ListScan_Item[i],  0);
//	for(i=0;i<11;i++)
//	WriteString_16(LIST1, FIRSTLINE+SPACE1+i*16, User_LimitScan_Item2[i],0);
	Disp_Button_TestSet(0);
}
//系统设置子函数
void Disp_SysLine(void)
{
 	uint32_t i;
	for(i=0;i<5;i++)
	{
		//if(i<=13/2)
			LCD_DrawLine( 90, FIRSTLINE+(i+1)*SPACE1-2, 90+100,FIRSTLINE+(i+1)*SPACE1-2, White );
//		else
//			LCD_DrawLine( 90+250, FIRSTLINE+(i-13/2+1)*SPACE1-2, 90+100+250,FIRSTLINE+(i-13/2+1)*SPACE1-2, White );
	}

}
//void Disp_Sys_MidRect(void)
//{
//
//	LCD_DrawRect( 0, 90+32,479 , 150+32 , LCD_COLOR_TEST_MID ) ;
////	LCD_DrawRect( 0, 195,479 , 227 , LCD_COLOR_TEST_MID ) ;
//}
void Disp_Sys_Screen(void)
{
	Disp_TopBar_Color(2);
//	Disp_Fastbutton();//显示快速按键	  
//	Disp_ListScan_MidRect();//显示中间色块	 User_LimitList_Item  
}
void Disp_Sys(void)
{
	uint32_t i;
//	Disp_TestSetScreen(1);
//    Disp_mainitem(2);

	Disp_Fastbutton(1);

	Disp_mainitem(4);
    GUI_SetColor(White);
	GUI_SetBkColor(LCD_COLOR_TEST_BACK);

    for(i=0;i<(sizeof(SYS_Disp_Tab)/(sizeof(SYS_Disp_Tab[0])));i++)
    {
          
            
        GUI_DispStringAt(SYS_Disp_Tab[i][U9001_Save_sys.U9001_SYS.language],LIST1, FIRSTLINE+SPACE1*((i+1)));

       	
    }
  
}
void Disp_Sys_Item(uint8_t page)
{
	uint32_t i;
//	Disp_Sys_Screen();
    Disp_TopBar_Color(page);
	Colour.Fword=White;
//	Disp_TestSetScreen(2);
//	Colour.Fword=White;
//	Disp_TestScreen();
	//Colour.Fword=White;
	
//    for(i=0;i<4;i++)
//    {
//        if(i!=page)
//            Colour.black=LCD_COLOR_TEST_BACK;
//        else
//            Colour.black=LCD_COLOR_TEST_BAR;
//        WriteString_16(0+i*(120), 4, TFT5520_NAME[i],0);
//    }
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BACK;
//	WriteString_16(LIST1, FIRSTLINE, User_ListScan_Item[0],  0);
    if(page==2)
	for(i=0;i<(sizeof(Sys_Setitem)/(sizeof(Sys_Setitem[0])));i++)
	//if(i<=sizeof(Sys_Setitem)/(sizeof(Sys_Setitem[0]))/2)
	{
		WriteString_16(LIST1, FIRSTLINE+SPACE1*(i+1), Sys_Setitem[i],  0);

	}
//	else
//	{
//		WriteString_16(LIST2,FIRSTLINE+SPACE1*(i-sizeof(Sys_Setitem)/(sizeof(Sys_Setitem[0]))/2), Sys_Setitem[i],  0);
//	}
//	Disp_SysLine();
//	Disp_Button_SysSet();
}
Sort_TypeDef Time_Set_Cov(Sort_TypeDef *Time)
{
	u32 value;
	if(Time->Num>9999)
	{
		Time->Num/=10;
		Time->Dot=4;
		
	}
	Time->Dot=4-Time->Dot;
	value=Time->Num;
	if(Time->Unit==0)
		value*=1000;
	else if(Time->Unit==1)
		value*=1e6;
	value/=pow(10,Time->Dot);
	if(value>30e6)
	{
		value=30e6;
		Time->Num=3000;
		Time->Dot=2;
		Time->Unit=1;
		
	}else if(value>=10e6)
	{
		Time->Num=value/10000;
		Time->Dot=2;
		Time->Unit=1;
		
	
	
	
	}else if(value>=1e6)
	{
		Time->Num=value/1000;
		Time->Dot=3;
		Time->Unit=1;
	
	
	}else if(value>=1e5)
	{
		Time->Num=value/100;
		Time->Dot=1;
		Time->Unit=0;
	
	
	}
	else if(value>=1e4)
	{
		Time->Num=value/10;
		Time->Dot=2;
		Time->Unit=0;
	
	}else if(value>=1e3)
	{
		Time->Num=value;
		Time->Dot=3;
		Time->Unit=0;
	
	
	}else 
	{
		Time->Num=value;
		Time->Dot=3;
		Time->Unit=0;
	
	}
	Time->mid_data=	value;
	
	

	return *Time;
}
int16_t Debug_Value(Sort_TypeDef *Debug_value)
{
	return Debug_value->Num;


}
Sort_TypeDef Freq_Set_Cov(Sort_TypeDef *Freq)
{
//	vu8 i;
	if(Freq->Unit==0)//单位是Hz
	{
		switch(Freq->Dot)
		{
			case 0:
				if(Freq->Num>1e5)
				{
					Freq->Num=1e5;
					Freq->Updata_flag=0;
				
				}
				else
					Freq->Updata_flag=1;
					
				break;
			case 1://1个小数点 
				break;
			case 2://两个小数点
//				for(i=0;i<5;i++)
//				{
//					if(Freq->Num>=10&&Freq->Num<100)
//						break;
//					else
//						Freq->Num/=10;
//				
//				
//				}
				Freq->Num/=1000;
				
				break;
			case 3://三个小数点
//				for(i=0;i<5;i++)
//				{
//					if(Freq->Num>=100&&Freq->Num<1000)
//						break;
//					else
//						Freq->Num/=10;
//				
//				
//				}
				Freq->Num/=100;
				break;
			case 4://4个小数点
//				for(i=0;i<5;i++)
//				{
//					if(Freq->Num>=1e3&&Freq->Num<1e4)
//						break;
//					else
//						Freq->Num/=10;
//				
//				
//				}
				Freq->Num/=10;
				break;
			default:
//				for(i=0;i<5;i++)
//				{
//					if(Freq->Num>=1e4&&Freq->Num<1e5)
//						break;
//					else
//						Freq->Num/=10;
//				
//				
//				}
				break;
		
		
		
		}
		
		
		
		
	
	}
	else//单位是KHZ
	{
		switch(Freq->Dot)
		{
			case 0:
				if(Freq->Num>100000)
				Freq->Num=100000;
				else
					Freq->Num*=1000;
					
				break;
			case 1:
				if(Freq->Num>0)
					Freq->Num/=10;
//				if(Freq->Num>=10000)
				
//				for(i=0;i<5;i++)
//				{
//					if((Freq->Num>=10&&Freq->Num<100)||Freq->Num<10)
//						break;
//					else
//						Freq->Num/=10;
//				
//				
//				}
				//Freq->Num*=100;
				break;
			case 2:
				
//				for(i=0;i<5;i++)
//				{
//					if(Freq->Num>=10&&Freq->Num<100)
//						break;
//					else
//						Freq->Num/=10;
//				
//				
//				}
//				Freq->Num*=1000;
				break;
			case 3:
				if(Freq->Num>10000)
					Freq->Num=10000;
				Freq->Num*=10;
//				for(i=0;i<5;i++)
//				{
//					if(Freq->Num>=10&&Freq->Num<100)
//						break;
//					else
//						Freq->Num/=10;
//				
//				
//				}
//				Freq->Num*=1e4;
				break;
			case 4:
				if(Freq->Num>1000)
					Freq->Num=1000;
				Freq->Num*=100;
				
				break;
			case 5:
			if(Freq->Num>100)
				Freq->Num=100;
			Freq->Num*=1000;
			
			break;
			default:
				Freq->Num=1e6;
				break;
		
		}
	
	
	}
	
	
	return *Freq;
}
Sort_TypeDef Input_Set_Cov(Sort_TypeDef *Input_Ref)//
{
//	vu8 i;
//	vu32 buff[]={10000,1000,100,10,1};
	vu32 value;
	if(Input_Ref->Num>9999)
	{
		Input_Ref->Num/=10;
		Input_Ref->Dot=4;
		
	}
	value=Input_Ref->Num*1000;
	value/=(pow(10,4-Input_Ref->Dot));
	if(value>60000)
	{
		value=60000;
		
		
	}
	if(value>10000)
	{
		Input_Ref->Dot=2;
		Input_Ref->Num=value/10;
	
	
	}else //if(value>1e3)
	{
		Input_Ref->Dot=3;
		Input_Ref->Num=value;
	
	
	}
//	else
//	{
//		Input_Ref->Dot=3;
//		Input_Ref->Num=value;
//	
//	
//	}
	Input_Ref->mid_data=value;
//	value=pow(10.0,4-(float)Input_Ref->Dot);
//	value=Input_Ref->Num/value;
//	if(value>=1000)
//	{
//		Input_Ref->Unit++;
//		Input_Ref->Dot+=3;
//	
//	
//	}else if(value==0)
//	{
//		if(Input_Ref->Unit>0)
//		{
//			Input_Ref->Unit--;
//			if(Input_Ref->Dot>=3)
//				Input_Ref->Dot-=3;
//		
//		
//		}
//	
//	
//	}
	
	return *Input_Ref;
}
Sort_TypeDef Input_Set_CovPre(Sort_TypeDef *Input_Ref)//
{
//	vu8 i;
//	vu32 buff[]={10000,1000,100,10,1};//10.000
	vu32 value;
	Input_Ref->Unit=0;
	//if(Input_Ref->Dot)
	value=pow(10.0,(float)Input_Ref->Dot);
	value=Input_Ref->Num/value;
	if(value>100)
	{
		
		Input_Ref->Dot=2;
	
	
	}
	
	return *Input_Ref;
}



void Disp_Cp_Rp(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)_C);//_p
	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)_p);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+5*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55,(uint8_t*)_p);
	//WriteString_Big(60,92+55 ,(uint8_t*)Out_Assic+6);

}

void Disp__(void)
{
    uint8_t i;
//    Colour.black=LCD_COLOR_TEST_MID;
    for(i=0;i<3;i++)
    {
//        for(j=0;j<5;j++)
//        LCD_ShowFontCN_40_55(60+40*(j+1),92+55*i,32,55,(uint8_t*)_num[11*32*55/8]);
        WriteString_Big(DISP_R_X+DISP_SPACE, DISP_R_Y+BIG_HZ_HIGH*i, "-----");
    
    }


}
void Disp_R_X(void)
{
	Colour.black=LCD_COLOR_TEST_BACK;
	LCD_ShowFontCN_40_55(DISP_R_X,DISP_R_Y,BIG_HZ_W,BIG_HZ_HIGH,(uint8_t*)Out_Assic+5*BIG_HZ_W*BIG_HZ_HIGH/8);//R
	LCD_ShowFontCN_40_55(DISP_R_X+DISP_SPACE*5,DISP_R_Y,BIG_HZ_W,BIG_HZ_HIGH, (uint8_t*)Out_Assic+13*40*BIG_HZ_HIGH/8);//M
	LCD_ShowFontCN_40_55(DISP_R_X+DISP_SPACE*6,DISP_R_Y,BIG_HZ_W,BIG_HZ_HIGH,(uint8_t*)Out_Assic+20*40*BIG_HZ_HIGH/8);//Ω
    
	LCD_ShowFontCN_40_55(DISP_R_X,DISP_R_Y+BIG_HZ_HIGH,BIG_HZ_W,BIG_HZ_HIGH,(uint8_t*)Out_Assic+27*40*BIG_HZ_HIGH/8);//V
	LCD_ShowFontCN_40_55(DISP_R_X+BIG_HZ_W*6,DISP_R_Y+BIG_HZ_HIGH,BIG_HZ_W,BIG_HZ_HIGH,(uint8_t*)Out_Assic+27*40*BIG_HZ_HIGH/8);//V
    
    LCD_ShowFontCN_40_55(DISP_R_X,DISP_R_Y+BIG_HZ_HIGH*2,BIG_HZ_W,BIG_HZ_HIGH,(uint8_t*)Out_Assic+28*BIG_HZ_W*BIG_HZ_HIGH/8);//T
    LCD_ShowFontCN_40_55(DISP_R_X+BIG_HZ_W*6,DISP_R_Y+BIG_HZ_HIGH*2,BIG_HZ_W,BIG_HZ_HIGH,(uint8_t*)Out_Assic+29*BIG_HZ_W*BIG_HZ_HIGH/8);
}
//void Disp__(void)
//{

//}
UnitNum_typedef IntToStr(u32 value)
{
    UnitNum_typedef return_value;

    if(value>5e7)
    {
        value=5e7;
        return_value.num=5020;
        return_value.dot=2;
        return_value.uint=2;
    }else if(value>=1e7)
    {
        return_value.num=value/1e4;
        return_value.dot=2;
        return_value.uint=3;
    }else if(value>=1e6)
    {
        return_value.num=value/1e3;
        return_value.dot=1;
        return_value.uint=2;
    }
    else if(value>=1e5)
    {
        return_value.num=value/1e3;
        return_value.dot=1;
        return_value.uint=2;
    } else if(value>=1e4)
    {
        return_value.num=value/1e2;
        return_value.dot=2;
        return_value.uint=2;
    }
    else
        {
        return_value.num=value;
        return_value.dot=1;
        return_value.uint=1;
    }
    return return_value;
}

UnitNum_typedef FToStr(u32 value)
{
    UnitNum_typedef return_value;

    if(value>5e7)
    {
        value=5e7;
        return_value.num=5020;
        return_value.dot=2;
        return_value.uint=1;
    }else if(value>=1e7)
    {
        return_value.num=value/1e4;
        return_value.dot=2;
        return_value.uint=1;
    }else if(value>=1e6)
    {
        return_value.num=value/1e3;
        return_value.dot=3;
        return_value.uint=1;
    }
    else if(value>=1e5)
    {
        return_value.num=value/1e2;
        return_value.dot=1;
        return_value.uint=0;
    } else if(value>=1e4)
    {
        return_value.num=value/1e1;
        return_value.dot=2;
        return_value.uint=0;
    }
    else
        {
        return_value.num=value;
        return_value.dot=3;
        return_value.uint=0;
    }
    return return_value;
}
UnitNum_typedef IntToStr_mA(u32 value)
{
    UnitNum_typedef return_value;

    
    if(value>=1e5)
    {
        return_value.num=value/1e2;
        return_value.dot=2;
        return_value.uint=2;
    } else if(value>=2e3)
    {
        return_value.num=value/1e1;
        return_value.dot=2;
        return_value.uint=2;
    }
    else
        {
        return_value.num=value;
        return_value.dot=3;
        return_value.uint=1;
    }
    return return_value;
}
UnitNum_typedef IntToStr_nF(u32 value)
{
    UnitNum_typedef return_value;

    
//    if(value>=1e3)
//    {
//        return_value.num=value;
//        return_value.dot=2;
////        return_value.uint=2;
//    } else 
	if(Range <= 2)
    {
//        return_value.num=value/1e1;
		return_value.num=value;
        return_value.dot=2;
//        return_value.uint=2;
    }
    else
        {
        return_value.num=value;
        return_value.dot=3;
        return_value.uint=1;
    }
    return return_value;
}
void Disp_Testvalue(u8 test)
{
    UnitNum_typedef test_value;
    GUI_SetColor(GUI_WHITE);
    sprintf(DispBuf,"%d/%d",U9001_Save_sys.U9001_save.current_step,U9001_Save_sys.U9001_save.all_step);
    GUI_DispStringAt(DispBuf,LISTVALUE1,FIRSTLINE);//显示步骤
    GUI_DispStringAt(TestPara[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter],LISTVALUE2,FIRSTLINE+0*SPACE1);//参数
    GUI_SetBkColor(LCD_COLOR_TEST_BACK);
    GUI_SetColor(GUI_LIGHTYELLOW);
    GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
    GUI_SetFont(&GUI_FontGUI_FONTXBFFON50);
    if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter!=PA)
    {
         if(test==1)
        {
    //                Test_Value.Vol=1000;
            Hex_Format(Test_Value.Vol,3,4,0);
        }else
        {
            Hex_Format(0,3,4,0);
        }
    }
    
		if(U9001_Save_sys.U9001_save.all_step == 2)
		{
			if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter == IR)
			{
				memset(sendbuff1,0,20);
				memcpy(sendbuff1,DispBuf,4);
				strcat(sendbuff1,"kV;");
			}else{
				memset(sendbuff,0,20);
				memcpy(sendbuff,DispBuf,4);
				strcat(sendbuff,"kV;");
			}
//			if(U9001_Save_sys.U9001_save.current_step == 2)
//			{
//				memset(sendbuff1,0,20);
//				memcpy(sendbuff1,DispBuf,4);
//				strcat(sendbuff1,"kV;");
//			}
		}else{
			memset(sendbuff,0,20);
			memcpy(sendbuff,DispBuf,4);
			strcat(sendbuff,"kV;");
			
			memset(sendbuff1,0,20);
			memcpy(sendbuff1,DispBuf,4);
			strcat(sendbuff1,"kV;");
		}
		
    GUI_DispStringAt(DispBuf,DISP_V_XPOS+50,DISP_V_YPOS);
    switch(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter)
    {
        case AC:
        case DC:
            if(test==1)
            {
                if(Test_Value.I_R==0xffff || GetSystemMessage() == MSG_SHORT)
								{
									memcpy(DispBuf,"UPPER",6); 
									memcpy(&sendbuff[7],"OFL",3);
                }else
                {  
//                    test_value=IntToStr(Test_Value.I_R);
//                    if(Range==0)
//                        Hex_Format(test_value.num,1,4,0);
//                    else
//                        Hex_Format(test_value.num,1,3,0);
								 if(U9001_Save_sys.U9001_Testconfg.clear == 1)
								 {
									 if(Test_Value.I_R < U9001_Save_sys.U9001_save.clearvalue[U9001_Save_sys.U9001_save.current_step-1])
									 {
										 Test_Value.I_R = 0;
									 }else{
										 Test_Value.I_R -= U9001_Save_sys.U9001_save.clearvalue[U9001_Save_sys.U9001_save.current_step-1];
									 }
								 }
									if(Range==1)
									{
											Hex_Format(Test_Value.I_R,3,4,0);
											memcpy(&sendbuff[7],&DispBuf[0],4);
									}else if(Range==0){
											Hex_Format(Test_Value.I_R,2,4,0);
											memcpy(&sendbuff[7],&DispBuf[1],4);
									}else{
											Hex_Format(Test_Value.I_R,1,4,0);
											memcpy(&sendbuff[7],"0.00",4);
									}
									
               }
								
//							 if(Range<2)
									strcat(sendbuff,"mA;");
//							 else
//								 strcat(sendbuff,"uA;");
							 
            }
            else
            {
//                test_value=IntToStr();
				
                Hex_Format(0,3,4,0);
            }
						GUI_SetColor(LCD_COLOR_TEST_BACK);
            GUI_FillRect(DISP_V_XPOS+50,DISP_V_YPOS+50,DISP_V_XPOS+50+150,DISP_V_YPOS+50+62);
						GUI_SetColor(GUI_LIGHTYELLOW);
            GUI_DispStringAt(DispBuf,DISP_V_XPOS+50,DISP_V_YPOS+50);
						if(Range<2)
							GUI_DispStringAt("mA  ",DISP_V_XPOS+50+6*25,DISP_V_YPOS+50);
						else
							GUI_DispStringAt("uA  ",DISP_V_XPOS+50+6*25,DISP_V_YPOS+50);
            break;
        case IR:
					if(test==1)
					{
							if(Test_Value.I_R==0xffff*10){
								memcpy(DispBuf,"UPPER       ",12);
//								if(U9001_Save_sys.U9001_save.all_step == 1)
//									strcat(sendbuff,">9999");
//								else
									strcat(sendbuff1,">9999");
							}else
							{  
								test_value=IntToStr(Test_Value.I_R);
							
								Hex_Format(test_value.num,test_value.dot,4,0);
								GUI_SetColor(LCD_COLOR_TEST_BACK);
								GUI_FillRect(DISP_V_XPOS+50,DISP_V_YPOS+50,DISP_V_XPOS+50+150,DISP_V_YPOS+50+62);
								GUI_SetColor(GUI_LIGHTYELLOW);
								GUI_DispStringAt(Test_IRUINT[test_value.uint],DISP_V_XPOS+50+6*25,DISP_V_YPOS+50);
//								if(U9001_Save_sys.U9001_save.all_step == 1)
//									strcat(sendbuff,(char*)DispBuf);
//								else
									strcat(sendbuff1,(char*)DispBuf);
							}
//							if(U9001_Save_sys.U9001_save.all_step == 1)
//								strcat(sendbuff,(char*)"M;");
//							else
							Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].text_unit=test_value.uint;
							if(test_value.uint == 0)
							{
								strcat(sendbuff1,(char*)"k;");
							}else if(test_value.uint == 1){
								strcat(sendbuff1,(char*)"M;");
							}else if(test_value.uint == 2){
								strcat(sendbuff1,(char*)"G;");
							}
					}
					else
					{
//                test_value=IntToStr();
							
							Hex_Format(0,1,3,0);
							GUI_DispStringAt(Test_IRUINT[3],DISP_V_XPOS+50+6*25,DISP_V_YPOS+50);
					}
					GUI_DispStringAt(DispBuf,DISP_V_XPOS+50,DISP_V_YPOS+50);
//            GUI_DispStringAt(Test_IRUINT[test_value.uint],DISP_V_XPOS+50+6*25,DISP_V_YPOS+50-20);·	
					break;
        case OS:
            
            if(test==1)
            {
                if(Test_Value.I_R==0xffff)
                   memcpy(DispBuf,"UPPER   ",8); 
                else
                {  
                    test_value=IntToStr_nF(Test_Value.I_R);
                 
                    Hex_Format(test_value.num,test_value.dot,4,0);
                }
            }
            else
            {
//                test_value=IntToStr();
                
                Hex_Format(0,3,4,0);
            }
			GUI_SetColor(LCD_COLOR_TEST_BACK);
             GUI_FillRect(DISP_V_XPOS+50,DISP_V_YPOS+50,DISP_V_XPOS+50+150,DISP_V_YPOS+50+62);
			 GUI_SetColor(GUI_LIGHTYELLOW);
            GUI_DispStringAt(DispBuf,DISP_V_XPOS+50,DISP_V_YPOS+50);
            GUI_DispStringAt(Disp_Unit[1],DISP_V_XPOS+50+6*25,DISP_V_YPOS+50);
            
           
            
          
        
            break;
        
            
        case PA:
            break;
        default:
            break;
    
    }
      GUI_SetFont(&GUI_FontGUI_FONTXBFFON16);
    if(test==1)
    {
        Hex_Format(Test_Value.Time,1,4,1);
    }
    else
    {
        Hex_Format(0,1,4,1);
    }
    GUI_DispStringAt(DispBuf,DISP_V_XPOS+44,DISP_V_YPOS+100+23);
    disp_TestMSG(GetSystemMessage());
}
//void Disp_Unit(void)
//{

//}

void Disp_Big_MainUnit(vu8 unit,vu8 unit1)
{
	const vu8 nuitnum[]={12,15,16,17,22,14,13};
	const vu8 nuit_nuit[]={18,19,20,2,11,23,22};//F,H,Ω，S r °空格
	LCD_ShowFontCN_40_55(DISP_UNIT_XPOS,DISP_UNIT_YPOS,40,55,(uint8_t*)Out_Assic+nuitnum[unit]*(55*40/8));
	LCD_ShowFontCN_40_55(DISP_UNIT_XPOS+40,DISP_UNIT_YPOS,40,55,(uint8_t*)Out_Assic+nuit_nuit[unit1]*(55*40/8));
}

void Disp_Big_SecondUnit(vu8 unit,vu8 unit1)
{
	const vu8 nuitnum[]={12,15,16,17,22,14,13};
	const vu8 nuit_nuit[]={18,19,20,2,11,23,22};//F,H,Ω，S r °空格
	LCD_ShowFontCN_40_55(DISP_UNIT_XPOS,DISP_UNIT_YPOS+55,40,55,(uint8_t*)Out_Assic+nuitnum[unit]*(55*40/8));
	LCD_ShowFontCN_40_55(DISP_UNIT_XPOS+40,DISP_UNIT_YPOS+55,40,55,(uint8_t*)Out_Assic+nuit_nuit[unit1]*(55*40/8));
}

void Send_UartClear(void)//清零命令
{
	vu8 Send_buff[24]={0xAA,0XFE,0XBF,0};
    vu8 i;
	for(i=0;i<3;i++)
    {
        UARTPutChar(LPC_UART0, Send_buff[i]);
        Delay(1);
    
    }
	

}

//==========================================================
//函数名称：Parameter_valuecomp
//函数功能：初始化值得比较
//入口参数：无
//出口参数：无
//创建日期：2015.10.28
//修改日期：2015.10.28 15:33
//备注说明：无
//==========================================================

const float xxbuff[4]={-30000,-30000,-30000,-30000,};
const float Comp_Clear[2]=
{
    50000,-50000
    

};
//void Parameter_valuecomp(void)
//{
//	u8 i;
//	u16 *pt;
////	for(i=0;i<PARAMETER_MENU_MAX;i++)//主参数比较
////	{	pt=(u16* )&SaveData.Parameter + i;
////		if(*pt>ParameterLimit[i][0])
////		*pt=ParameterLimit[i][1];
////	}//SYSTEM_MENU_MAX
////    for(i=0;i<SYSTEM_MENU_MAX;i++)//系统比较
////	{	pt=(u16* )&SaveData.System+ i;
////		if(*pt>SystemLimit[i][0])
////		*pt=SystemLimit[i][1];
////	}
//    if((Tft_5520.Calibrate.IRVol.Num>CalibrateLimit[0][0])||(Tft_5520.Calibrate.IRVol.Num<CalibrateLimit[0][1]))
//            Tft_5520.Calibrate.IRVol.Num=1000;
//    for(i=0;i<DEBUG_NUM_MAX-1;i++)//校正输入值比较
//    {
//        if((Tft_5520.Calibrate.IRCur[i].Num>CalibrateLimit[i][0])||(Tft_5520.Calibrate.IRCur[i].Num<CalibrateLimit[i][1]))
//            Tft_5520.Calibrate.IRCur[i].Num=1000;
//    
//    }//CalibrateAdLimit
//   
//    
//    if((Tft_5520.Calibrate.IRVol.NumAd>CalibrateAdLimit[0])||(Tft_5520.Calibrate.IRVol.NumAd<CalibrateAdLimit[1]))
//            SaveData.Calibrate.IRVol.NumAd=1;
//    for(i=0;i<DEBUG_NUM_MAX-1;i++)//校正AD值比较
//    {
//        if((Tft_5520.Calibrate.IRCur[i].NumAd>CalibrateAdLimit[0])||(Tft_5520.Calibrate.IRCur[i].NumAd<CalibrateAdLimit[1]))
//            Tft_5520.Calibrate.IRCur[i].NumAd=1;
//    
//    }//CLEAR_NUM_MAX
//     for(i=0;i<CLEAR_NUM_MAX;i++)//清零值
//    {
//        if((Tft_5520.Clear_buff[i]>Comp_Clear[0])||(Tft_5520.Clear_buff[i]<Comp_Clear[1]))
//            Tft_5520.Clear_buff[i]=xxbuff[i];
//    
//    
//    }
//    


//}

//num 0  电压校正 1  1.000M校正点  2  10.00M校正点 
//3  100.0M校正点   4  1000M校正点


//

//void Send_Uart3(uint8_t *buff)
//{
//	vu8 Send_buff[10]={0xAB,0X00,0xbf,0};
//	
////	Send_buff[3]=ord->Ordel;
////	Send_buff[4]=ord->name;
////	Send_buff[5]=0X80;//校验和 
////	Send_buff[6]=0XBF;
////	Send_buff[7]=0;
////	UARTPuts( LPC_UART3, Send_buff);
//	UARTPutChar (LPC_UART3, 0xaa);
//	UARTPuts( LPC_UART3, buff);
//	//_printf("s%",(const)Send_buff);

//}

void Disp_RangeDispvalue(vu8 data)
{
	Colour.black=LCD_COLOR_TEST_BACK;
	WriteString_16(20, 200, RangeDisp_Main[data],  1);//
	WriteString_16(280, 200, RangeDisp_Second[data],  1);
	Colour.black=LCD_COLOR_TEST_MID;
	LCD_ShowFontCN_40_55(80,92+30,40,55, (uint8_t*)Out_Assic+7*40*55/8);
	LCD_ShowFontCN_40_55(80+40,92+30,40,55, (uint8_t*)Out_Assic+24*40*55/8);
	LCD_ShowFontCN_40_55(80+40+20,92+30,40,55, (uint8_t*)Out_Assic+25*40*55/8);
//


}
void Disp_Range_DispMainUnit(vu8 num,vu8 unit)
{
	WriteString_16(128, 200, (uint8_t*)Disp_Unit[num],  1);//单位的量
	WriteString_16(136, 200, (uint8_t*)Disp_Range_Main_Disp[unit],  1);//单位

}
void Disp_Range_DispSecondUnit(vu8 num,vu8 unit)
{
	WriteString_16(388, 200, (uint8_t*)Disp_Unit[num],  0);//单位的量
	WriteString_16(396, 200, (uint8_t*)Disp_Range_Main_Disp[unit],  0);//单位

}

void Disp_RangeCount(void)
{
	
	u8 i;
	
	for(i=0;i<11;i++)
	
	{
		if((i>1&&i<7)||((i>8)))
			Colour.black=LCD_COLOR_TEST_MID;
		else
			Colour.black=LCD_COLOR_TEST_BACK;
		Hex_Format(Count_buff[i] , 0, 6 , 0);//加单位
				
		WriteString_12(LIST2+140, 58+i*15, DispBuf,  1);//显示标称值  后面要接着显示单位
	}

}
void Disp_Set_Unit_16(uint32_t unit,uint32_t x,uint32_t y)//Disp_Range_Main_NUm
{
	WriteString_16(x, y, Disp_Range_Main_Disp[Disp_Range_Main_NUm[unit]],  0);


}
void Disp_Set_Unit_12(uint32_t unit,uint32_t x,uint32_t y)
{
	WriteString_12(x, y, Disp_Range_Main_Disp[unit],  1);


}
void Disp_Test_Unit_v(uint32_t x,uint32_t y)
{
	WriteString_16(x, y, "V",  1);

}
void Disp_Test_Unit_mA(uint32_t x,uint32_t y)
{
	WriteString_16(x, y, "mA",  1);

}
void Savetoeeprom(void)
{
	//Saveeeprom
	u8 *pt;
	
	pt=(u8*)&U9001_Save_sys;
	
	EEPROM_Write(0, 3, pt, MODE_8_BIT, sizeof(U9001_Save_sys));
//	EEPROM_Write(5, 0, pt1, MODE_8_BIT, sizeof(U9001_Save_sys.U9001_save));
}

void SaveCal(void)
{
	//Saveeeprom
	u8 *pt;
	
	pt=(u8*)&U9001_Calibrate;
	EEPROM_Write(0, 0, pt, MODE_8_BIT, sizeof(U9001_Calibrate));
//	EEPROM_Write(5, 0, pt1, MODE_8_BIT, sizeof(U9001_Save_sys.U9001_save));
}

void ReadSavedata(void)
{	
//	u8 i;
	U9001_Save_sysTypedef *pt;
//	U9001_save_Typedef *pt1;
	//Saveeeprom_Typedef 
	pt=&U9001_Save_sys;
//	pt1=&U9001_Save_sys.U9001_save;
	EEPROM_Read(0, 3, (u8 *)pt, MODE_8_BIT, sizeof(U9001_Save_sys));
//	EEPROM_Read(5, 0, (u8 *)pt1, MODE_8_BIT, sizeof(U9001_Save_sys.U9001_save));
}

void ReadCal(void)
{
	//Saveeeprom
	u8 *pt;
	savesize = sizeof(U9001_Calibrate);
	pt=(u8*)&U9001_Calibrate;
	EEPROM_Read(0, 0, (u8 *)pt, MODE_8_BIT, sizeof(U9001_Calibrate));
//	EEPROM_Write(5, 0, pt1, MODE_8_BIT, sizeof(U9001_Save_sys.U9001_save));
}

const vu16 Set_Data_Comp[][2]=
{
//    {0,1},
//    {0,GROUP_MAX-1},
    {0,1},
    {10,1000},
    {0,9999},
    {0,1},
    {0,1},
    {0,RANGE_MAX},
    {0,2},
    {0,9999},
    {0,9999},
    {0,1},

};
const vu16 Set_Data_Int[]={0,100,100,0,0,0,0,1000,1,0};
const  vu16 Sys_Data_Comp[][2]=
{
	{0,1},
	{0,2},
	{0,2},
	{0,1},
	{0,2},
	{2019,2030},
	{0,2},
	{0,127},
	{0,4},
	{0,1},
	{0,1},
    {0,2}


};
const u32 Set_DateFileCompvalue[][2]=
{
    {0,4},
    {10,6000},
    {0,12000},
    {0,9999},
    {0,12000},
    {0,9999},
    {0,100},
    {0,9999},
    {0,3500},

};
//#if (SOFTWARE_VERSION==0x01)
const u32 Set_ACW_Compvalue[][2]=
{
    {0,4},
    {50,5000},
    {1,12000},
    {3,9999},
    {0,12000},
    {0,9999},
    {0,150},
    {0,9999},
    {0,3500},
};
//#elif (SOFTWARE_VERSION==0x02 || SOFTWARE_VERSION==0x03)
const u32 Set_ACW_Compvalue1[][2]=
{
    {0,4},
    {50,5000},
    {1,20000},
    {3,9999},
    {0,20000},
    {0,9999},
    {0,150},
    {0,9999},
    {0,3500},
};
//#endif


const u32 Set_DCW_Compvalue[][2]=
{
    {0,4},
    {50,6000},
    {1,5000},
    {2,9999},
    {0,5000},
    {0,9999},
    {0,100},
    {0,9999},
    {0,9999},
    {0,3500},
};

const u32 Set_DCW_Compvalue1[][2]=
{
    {0,4},
    {50,6000},
    {1,10000},
    {2,9999},
    {0,5000},
    {0,9999},
    {0,100},
    {0,9999},
    {0,9999},
    {0,3500},
};

const u32 Set_IR_Compvalue[][2]=
{
    {0,4},
    {50,1000},
    {0,500000},
    {3,9999},
    {1,500000},
    {0,9999},
    {0,7},
    {0,9999},
    {0,1},
//    {0,9999},
//    {0,3500},
};
const u32 Set_OS_Compvalue[][2]=
{
    {0,4},
    {100,100},
    {10,100},
//    {5,5},
	{5,5},
    {0,500},
    {0,9999},
    {10,100},
    {0,0},
    {0,35000},
};

const u32 Set_PA_Compvalue[][2]=
{
    {0,4},
    {100,100},
    {0,100},
//    {5,5},
	{3,9999 },
    {0,500},
    {0,9999},
    {0,100},
    {0,0},
    {0,35000000},
};

void SetDate_Comp(void)
{
   u8 i,j;
   u32 *pt;
	 u16 *pt1;
		if(U9001_Save_sys.jkflag > 1)
		{
			U9001_Save_sys.jkflag=0;
		}
		if(U9001_Save_sys.version != 1 
			&& U9001_Save_sys.version != 2
			&& U9001_Save_sys.version != 3){
				U9001_Save_sys.version = 1; 
		}
    if(U9001_Save_sys.U9001_save.all_step>MAX_TEXT_STEP)
        U9001_Save_sys.U9001_save.all_step=1;
    if(U9001_Save_sys.U9001_save.current_step>U9001_Save_sys.U9001_save.all_step || U9001_Save_sys.U9001_save.current_step==0)
        U9001_Save_sys.U9001_save.current_step=1;
    if(U9001_Save_sys.U9001_save.start_step>U9001_Save_sys.U9001_save.current_step || U9001_Save_sys.U9001_save.start_step==0)
        U9001_Save_sys.U9001_save.start_step=1;
    for(i=0;i<MAX_TEXT_STEP;i++)
    {
        if(U9001_Save_sys.U9001_save.U9001_Setup[i+1].parameter>4)
            U9001_Save_sys.U9001_save.U9001_Setup[i+1].parameter=0;
      pt=(u32*)&U9001_Save_sys.U9001_save.U9001_Setup[i+1];
       switch(U9001_Save_sys.U9001_save.U9001_Setup[i+1].parameter)//项目
        {
            case IR_SETUP://绝缘电阻测试
                for(j=0;j<9;j++)
                {
                     if((*(pt+j)>Set_IR_Compvalue[j][1])/*||(*(pt+j)<Set_IR_Compvalue[j][0])*/)
                    {
                        *(pt+j)=Set_IR_Compvalue[j][1];
                    }else if((*(pt+j)<Set_IR_Compvalue[j][0])){
						if(j==3)
						{
							*(pt+j)=0;
						}else{
							*(pt+j)=Set_IR_Compvalue[j][0];
						}
					}
                }
                
                break;

            case ACW_SETUP://耐压测试
							if(SOFTWARE_VERSION==1)
							{
                for(j=0;j<9;j++)
                {
									 if((*(pt+j)>Set_ACW_Compvalue[j][1])/*||(*(pt+j)<Set_ACW_Compvalue[j][0])*/)
									 {
									   *(pt+j)=Set_ACW_Compvalue[j][1];
									 }else if((*(pt+j)<Set_ACW_Compvalue[j][0])){
										 if(j==3)
										 {
											 *(pt+j)=0;
										 }else{
											 *(pt+j)=Set_ACW_Compvalue[j][0];
										 }
									}
                }
							}else if(SOFTWARE_VERSION==2||SOFTWARE_VERSION==3){
								for(j=0;j<9;j++)
                {
									 if((*(pt+j)>Set_ACW_Compvalue1[j][1])/*||(*(pt+j)<Set_ACW_Compvalue[j][0])*/)
									 {
									   *(pt+j)=Set_ACW_Compvalue1[j][1];
									 }else if((*(pt+j)<Set_ACW_Compvalue1[j][0])){
										 if(j==3)
										 {
											 *(pt+j)=0;
										 }else{
											 *(pt+j)=Set_ACW_Compvalue1[j][0];
										 }
									}
                }
							}
                
            break;

            case DCW_SETUP://
							if(SOFTWARE_VERSION==1)
							{
                for(j=0;j<10;j++)
                {
									if((*(pt+j)>Set_DCW_Compvalue[j][1])/*||(*(pt+j)<Set_DCW_Compvalue[j][0])*/)
									{
											*(pt+j)=Set_DCW_Compvalue[j][1];
									}else if((*(pt+j)<Set_DCW_Compvalue[j][0])){
										if(j==3)
										{
											*(pt+j)=0;
										}else{
											*(pt+j)=Set_DCW_Compvalue[j][0];
										}
									}
                }
							}else if(SOFTWARE_VERSION==2||SOFTWARE_VERSION==3){
								for(j=0;j<10;j++)
                {
									if((*(pt+j)>Set_DCW_Compvalue1[j][1])/*||(*(pt+j)<Set_DCW_Compvalue[j][0])*/)
									{
											*(pt+j)=Set_DCW_Compvalue1[j][1];
									}else if((*(pt+j)<Set_DCW_Compvalue1[j][0])){
										if(j==3)
										{
											*(pt+j)=0;
										}else{
											*(pt+j)=Set_DCW_Compvalue1[j][0];
										}
									}
                }
							}
            break;

            case OS_SETUP://
                 for(j=0;j<9;j++)
                {
                     if((*(pt+j)>Set_OS_Compvalue[j][1])/*||(*(pt+j)<Set_OS_Compvalue[j][0])*/)
                    {
                        *(pt+j)=Set_OS_Compvalue[j][1];
                    }else if((*(pt+j)<Set_OS_Compvalue[j][0])){
//						if(j==3)
//						{
//							*(pt+j)=0;
//						}else{
							*(pt+j)=Set_OS_Compvalue[j][0];
//						}
					}
                }
                break;

            case PA_SETUP://无测试项目
				 for(j=0;j<9;j++)
                {
                     if((*(pt+j)>Set_PA_Compvalue[j][1])/*||(*(pt+j)<Set_DCW_Compvalue[j][0])*/)
                    {
                        *(pt+j)=Set_PA_Compvalue[j][1];
                    }else if((*(pt+j)<Set_PA_Compvalue[j][0])){
						if(j==3)
						{
							*(pt+j)=0;
						}else{
							*(pt+j)=Set_PA_Compvalue[j][0];
						}
					}
                }
                break;
                break;

            default:
                
                return;
                
        }	
    
    } 
		
//		pt1=(u16*)&U9001_Save_sys.U9001_Testconfg;
//		for(i=0;i<15;i++)
//    {
//			if(*(pt1+(i))>=ParameterLimit[i][1])
//			{
//				*(pt1+(i))=ParameterLimit[i][0];
//			}
//		}
}
//void SetDate_Comp(void)
//{
//    u8 i,j;
//    u32 *pt;
//    if(U9001_Save_sys.U9001_save.all_step>MAX_TEXT_STEP)
//        U9001_Save_sys.U9001_save.all_step=1;
//    if(U9001_Save_sys.U9001_save.current_step>U9001_Save_sys.U9001_save.all_step)
//        U9001_Save_sys.U9001_save.current_step=1;
//    
//    for(i=0;i<MAX_TEXT_STEP;i++)
//    {
//        pt=(u32*)&U9001_Save_sys.U9001_save.U9001_Setup[i];
//        for(j=0;j<9;j++)
//        {
//            if(*(pt+j)>Set_DateFileCompvalue[j][1])
//            {
//                *(pt+j)=Set_DateFileCompvalue[j][0];
//            }
//        }
//    
//    }
//}
//设置数据的上下限判定
void SetData_High_Low_Comp(void)
{
	vu16 j,i;
	vu8 *pt;
    uint16_t *ppt;
//    ppt=(uint16_t *)&Tft_5520.Set_Value;
//    for(i=0;i<GROUP_MAX;i++)
//    {
//        for(j=0;j<SET_MAX-1;j++)
//        {
//            if(*(ppt+i*sizeof(Tft_5520.Set_Value[0])/2+j)>Set_Data_Comp[j][1]||*(ppt+i*sizeof(Tft_5520.Set_Value[0])/2+j)<Set_Data_Comp[j][0])
//                *(ppt+i*sizeof(Tft_5520.Set_Value[0])/2+j)=Set_Data_Int[j];
//        
//        }
//    
//    
//    }

//	pt=(vu8 *)&Tft_5520.Sys_Set;
    if(U9001_Save_sys.U9001_SYS.language>Sys_Data_Comp[0][1])
         U9001_Save_sys.U9001_SYS.language=Sys_Data_Comp[0][0];
    if(U9001_Save_sys.U9001_SYS.pass_beep>Sys_Data_Comp[1][1])
        U9001_Save_sys.U9001_SYS.pass_beep=Sys_Data_Comp[1][0];
    if(U9001_Save_sys.U9001_SYS.fail_beep>Sys_Data_Comp[2][1])
        U9001_Save_sys.U9001_SYS.fail_beep=Sys_Data_Comp[2][0];
    if(U9001_Save_sys.U9001_SYS.key_beep>Sys_Data_Comp[3][1])
        
        U9001_Save_sys.U9001_SYS.key_beep=Sys_Data_Comp[3][0];
    if(U9001_Save_sys.U9001_SYS.beep>Sys_Data_Comp[4][1])
        U9001_Save_sys.U9001_SYS.beep=Sys_Data_Comp[4][0];
    if(U9001_Save_sys.U9001_SYS.bussmode>Sys_Data_Comp[6][1])
        U9001_Save_sys.U9001_SYS.bussmode=Sys_Data_Comp[6][0];
    if(U9001_Save_sys.U9001_SYS.buss_addr>Sys_Data_Comp[7][1])
        U9001_Save_sys.U9001_SYS.buss_addr=Sys_Data_Comp[7][0];
    if(U9001_Save_sys.U9001_SYS.buard>Sys_Data_Comp[8][1])
        U9001_Save_sys.U9001_SYS.buard=Sys_Data_Comp[8][0];
    if(U9001_Save_sys.U9001_SYS.date_bit>Sys_Data_Comp[9][1])
        U9001_Save_sys.U9001_SYS.date_bit=Sys_Data_Comp[9][0];
    if(U9001_Save_sys.U9001_SYS.stop_bit>Sys_Data_Comp[10][1])
        U9001_Save_sys.U9001_SYS.stop_bit=Sys_Data_Comp[10][0];
    if(U9001_Save_sys.U9001_SYS.language>Sys_Data_Comp[11][1])
        U9001_Save_sys.U9001_SYS.ord_bit=Sys_Data_Comp[11][0];
    
//    Check_Calibrate_Limit();
    
    
//	for(i=0;i<5;i++)
//	{
//	if(*(pt+i)>Sys_Data_Comp[i][1])
//		*(pt+i)=Sys_Data_Comp[i][0];
//	
//	}
	
	

}
void Disp_Range(uint8_t flag)
{
	Colour.black=LCD_COLOR_TEST_BACK;
	if(flag==0)//Hand_RangeDisp
		WriteString_16(LIST2+88, FIRSTLINE+SPACE1, Auto_RangeDisp[Range],  0);
	else
		WriteString_16(LIST2+88, FIRSTLINE+SPACE1, Hand_RangeDisp[Range],  0);
		//riteString_12(LIST2+140, 58+i*15, Auto_RangeDisp[Test_Dispvalue.Rangedisp],  1);

}
void Disp_Usbflag(uint8_t flag)
{
    u32 Fword;
    Fword=Colour.Fword;
	Colour.black=LCD_COLOR_TEST_BACK;
	if(flag==1)
		Colour.Fword=LCD_COLOR_GREEN;
		
	else if(flag==2)
		Colour.Fword=LCD_COLOR_RED;
	else
		Colour.Fword=LCD_COLOR_TEST_BACK;
//	LCD_ShowFontCN_40_55(uint16_t x,uint16_t y,uint16_t xsize,uint16_t ysize,uint8_t * image);
	LCD_ShowFontCN_40_55(2,DISP_R_Y+5,48,21,(uint8_t*)(nBitmapDot)+3*48/8);
	//WriteString_16(BUTTOM_X_VALUE+4*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, " 显示",  0);
	Colour.Fword=Fword;

}
const u8 Bit_Dot[][4]=
{
    {0,1,2,3},
    {0,1,2,4},
    {0,1,3,4},
    {0,2,3,4},

};
//==========================================================
//函数名称：ListBox_Setup
//函数功能：列表框设置
//入口参数：无
//出口参数：键值
//创建日期：2015.10.28
//修改日期：2015.10.28 09:50
//备注说明：标题栏字符长度,TITLE_LEN_MAX=8
//==========================================================
u8 ListBox_Setup(NumBox_TypeDef * pt)
{
	u8 key,count_f;
	u8 len;
	u8 blink;//闪烁标志
	LcdAddr_TypeDef addr;

	blink=TRUE;//闪烁标志

	addr.x=LcdAddr.x;//显示地址备份
	addr.y=LcdAddr.y;
    if(pt->Num>pt->Max)
        pt->Num=pt->Max;
	len=strlen((const char *)(pt->Title));//标题栏字符长度
	len++;//结束符
	if(len>TITLE_LEN_MAX)len=TITLE_LEN_MAX;//最大长度限定
	//Colour.black=LCD_COLOR_SELECT;
	while(1)
	{
		if(blink)
		{
//			LcdAddr.x=addr.x;//显示地址
//			LcdAddr.y=addr.y;
//			Disp_StrAt((u8*)(pt->Title+len*pt->Num));//显示菜单值
			LcdAddr.x=addr.x;//显示地址
			LcdAddr.y=addr.y;
             
            WriteString_16(LcdAddr.x, LcdAddr.y, (pt->Title+len*pt->Num),  0);
		}
		else
		{
			//WriteString_16(LcdAddr.x, LcdAddr.y, "    ",  0);
			LCD_DrawRect( LcdAddr.x, LcdAddr.y,LcdAddr.x+DATA_SPACE1 , LcdAddr.y+ SPACE1-4, Colour.black ) ;
		
		
		}
		if(count_f++>2)
        {
            if(blink)
                blink=FALSE;
            else
                blink=TRUE;
            
            count_f=0;
        }

		key=Key_Read_WithTimeOut(TICKS_PER_SEC_SOFTTIMER/10);//等待按键(100*10ms/10=100ms)
		if(key!=KEY_NONE)
		{
			Key_Beep();
			switch(key)
			{
				case Key_EXIT://退出
					
					//break;
				case Key_STOP://停止按键
					
					return(0);
					
				case Key_DOWN:		//下键
				   // blink=TRUE;//显示闪烁标志
					if(pt->Num<=pt->Min)
						pt->Num=pt->Max;
					else
						pt->Num--;
					break;

				case Key_UP:	//上键
					  //blink=TRUE;//显示闪烁标志

					if(pt->Num>=pt->Max)
						pt->Num=pt->Min;
					else
						pt->Num++;
					break;
				case Key_FAST :
	//                if(dispdata>pt->Max)
	//                    dispdata=pt->Max;
	//                if(dispdata<pt->Min)
	//                    dispdata=pt->Min;
	//                pt->Num=dispdata;

					
					return(1);
				   // break;
				case Key_LEFT:
					blink=TRUE;//显示闪烁标志
	//                if(current_f+len*10<current_f+3*10)
	//                {
	//                    //Disp_bit_c++;
	//                    len++;
	//                    
	//                }
	//                else
	//                {
	//                    len=0;
	//                    
	//                }
	//                Disp_bit=current_f+Bit_Dot[NumBox.Dot][current_f+len-current_f]*10;
					break;
				case Key_RIGHT:
					blink=TRUE;//显示闪烁标志
	//                if(current_f+len*10>current_f)
	//                {
	//                    len--;
	//                    
	//                }
	//                else
	//                {
	//                    len=3;
	//                    
	//                }
	//                Disp_bit=current_f+Bit_Dot[NumBox.Dot][current_f+len-current_f]*10;
					break;


            }
		}
	}
}
//==========================================================
//函数名称：Number_Setup
//函数功能：数值框设置处理
//入口参数：NumBox_TypeDef
//出口参数：键值
//创建日期：2015.10.28
//修改日期：2015.10.28 09:44
//备注说明：无
//==========================================================
int8_t Number_Setup(NumBox_TypeDef * pt)
{
	u8 key;
	u8 len=0;
    u8 i;
//	u8 count;//步进计数器
//	u8 current;//当前设置位
    vu16 Disp_bit;
    vu16 Dispset_buff[4];
//    const u16 div[]={1000,100,10,1};
    vu16 dispdata;
    vu16 count_f=0;
    vu16 current_f;
	vu8 blink;//显示闪烁标志
	LcdAddr_TypeDef addr;//显示地址备份

	blink=TRUE;//显示闪烁标志

	addr.x=LcdAddr.x;//显示地址备份
	addr.y=LcdAddr.y;
    Disp_bit=LcdAddr.x;
    current_f=Disp_bit;

    dispdata=pt->Num;
    for(i=4;i>0;i--)
    {
        Dispset_buff[i-1]=dispdata%10;
        dispdata/=10;
    
    }

	while(1)
	{
        //count_f++;
		if(blink)//显示
		{
//			LcdAddr.x=addr.x;//显示地址
//			LcdAddr.y=addr.y;
//             dispdata=Dispset_buff[3]+Dispset_buff[2]*10+Dispset_buff[1]*100+Dispset_buff[0]*1000;
//			Hex_Format(dispdata,pt->Dot,4,TRUE);//数值格式化，4位数值
//            WriteString_16(LcdAddr.x, LcdAddr.y, DispBuf,  1);
//			//显示菜单值
		}
        else
        {
        
//            LcdAddr.x=Disp_bit;
//            PutChar( LcdAddr.x, LcdAddr.y, ' ', LCD_COLOR_SELECT, LCD_COLOR_SELECT );

        }
        if(count_f++>2)
        {
            if(blink)
                blink=FALSE;
            else
                blink=TRUE;
            
            count_f=0;
        }
        key=Key_Read_WithTimeOut(TICKS_PER_SEC_SOFTTIMER/10);
        if(key!=KEY_NONE)
		{	
			//Disp_Flag=1;
			Key_Beep();
			switch(key)
			{
				case Key_EXIT://退出
					
					//break;
				case Key_STOP://停止按键
					
					return(0);
					
				case Key_DOWN:		//下键
					blink=TRUE;//显示闪烁标志

						if(Dispset_buff[len]>0)
							Dispset_buff[len]--;
						else
							Dispset_buff[len]=9;
	//                   

					break;

				case Key_UP:	//上键
					  blink=TRUE;//显示闪烁标志

						if(Dispset_buff[len]<9)
							Dispset_buff[len]++;
						else
							Dispset_buff[len]=0;
	 
					break;
				case Key_FAST :
					if(dispdata>pt->Max)
						dispdata=pt->Max;
					if(dispdata<pt->Min)
						dispdata=pt->Min;
					pt->Num=dispdata;

					
					return(1);
				   // break;
				case Key_RIGHT:
					blink=TRUE;//显示闪烁标志
					if(current_f+len*10<current_f+3*10)
					{
						//Disp_bit_c++;
						len++;
						
					}
					else
					{
						len=0;
						
					}
					Disp_bit=current_f+Bit_Dot[NumBox.Dot][current_f+len-current_f]*10;
					break;
				case Key_LEFT:
					blink=TRUE;//显示闪烁标志
					if(current_f+len*10>current_f)
					{
						len--;
						
					}
					else
					{
						len=3;
						
					}
					Disp_bit=current_f+Bit_Dot[NumBox.Dot][current_f+len-current_f]*10;
				break;


			}
        }
	}
    
}
void Disp_Teststatue(uint8_t num)
{
    uint32_t colour=Colour.black;
    Colour.black=LCD_COLOR_TEST_BAR;
    WriteString_16(10, 272-17, TFT5520_state[num],  0);
    Colour.black=colour;

}

void Comp_Disp(float compvalue,uint32_t  hi,uint32_t lo)
{
	SetSystemMessage(MSG_PASS);
	if(Tft_5520.Set_Value[Tft_5520.Group].Hi_Limit)
	{
		if(compvalue>hi)//大于上限
		{
			
			SetSystemMessage(MSG_HIGH);
			Fail_led();
		   
			if(Tft_5520.Set_Value[Tft_5520.Group].beep_Mode==2)//不合格响
			{
				Beep_on();
			
			
			}
			else
				 Beep_Off();

				
		}
		else
		SetSystemMessage(MSG_PASS);
	}
	if(GetSystemMessage()==MSG_PASS)
	if(Tft_5520.Set_Value[Tft_5520.Group].Lo_Limit)
	{
		
		if(compvalue<lo)//下限失败
		{
			SetSystemMessage(MSG_LOW);
			Fail_led();
			if(Tft_5520.Set_Value[Tft_5520.Group].beep_Mode==2)//不合格响
			{
				Beep_on();
			
			}
			 else
				 Beep_Off();
			
		}
		else //合格
			{
				Pass_Led();
				SetSystemMessage(MSG_PASS);
				if(Tft_5520.Set_Value[Tft_5520.Group].beep_Mode==1)//合格响
				{
					Beep_on();
				
				}
				 else
					 Beep_Off();
			}
	}
	
			
 Disp_big_HZ(GetSystemMessage());
}
        




Test_ValueTypedef Time_Set_Cov_num(Test_ValueTypedef *Time)
{
//	float value;
	if(Time->res>99999)
	{
		Time->res/=10;
		Time->dot=3;
		
	}
	if(Time->dot==1)
		Time->res/=10;
	//value=Time->res;

	return *Time;
}
void disp_Numtouch(u16 xpos,u16 ypos,u8 flag,u8 *num)
{
	if(xpos<50)
		xpos=50;
	if(ypos<50)
		ypos=50;
	if(flag)
	{
		Colour.black=LCD_COLOR_RED;
		//PutPixel
//		LCD_DrawLine(xpos+4, ypos, 65, 0);
//		LCD_DrawLine(xpos+4, ypos+40, 65, 0);
	
	}else
	{
		Colour.black=LCD_COLOR_BLUE;
	}
		LCD_DrawRect(xpos+5, ypos, xpos+5+55, ypos+1,Colour.black);
		LCD_DrawRect(xpos+5, ypos+40,xpos+5+ 55, ypos+40+1,Colour.black);
		
		LCD_DrawRect(xpos, ypos+5, 1+xpos, ypos+5+30,Colour.black);
		LCD_DrawRect(xpos+65, ypos+5, xpos+65+1, ypos+5+30,Colour.black);
		//Colour.Fword=LCD_COLOR_BLUE;
		LCD429_PutPixel(xpos+1,ypos+4,Colour.black);
		
		LCD429_PutPixel(xpos+2,ypos+3,Colour.black);
		LCD429_PutPixel(xpos+3,ypos+2,Colour.black);
		LCD429_PutPixel(xpos+4,ypos+1,Colour.black);
//		//LCD429_PutPixel(xpos+5,ypos+1,LCD_COLOR_BLUE);
//		
		LCD429_PutPixel(xpos+64,ypos+4,Colour.black);
		LCD429_PutPixel(xpos+63,ypos+3,Colour.black);
		LCD429_PutPixel(xpos+62,ypos+2,Colour.black);
		LCD429_PutPixel(xpos+61,ypos+1,Colour.black);
		
		LCD429_PutPixel(xpos+61,ypos+39,Colour.black);
		LCD429_PutPixel(xpos+62,ypos+38,Colour.black);
		LCD429_PutPixel(xpos+63,ypos+37,Colour.black);
		LCD429_PutPixel(xpos+64,ypos+36,Colour.black);
		
		LCD429_PutPixel(xpos+1,ypos+36,Colour.black);
		LCD429_PutPixel(xpos+2,ypos+37,Colour.black);
		LCD429_PutPixel(xpos+3,ypos+38,Colour.black);
		LCD429_PutPixel(xpos+4,ypos+39,Colour.black);
		Colour.black =LCD_COLOR_CYAN;;
		Colour.Fword=LCD_COLOR_BLACK;
		WriteString_16( xpos+21, ypos+17, num, 0 ); 
}

const u8 TOUCH_NUM[][4]=
{ 
	"1","2","3"," C",
	"4","5","6","  ",
	"7","8","9","  ",
	".","0","DEL","MΩ",};
const u8 TOUCH_NUM1[][4]=
{ 
	"1","2","3"," C ",
	"4","5","6","",
	"7","8","9","",
	".","0","DEL"," V",};
const u8 TOUCH_NUM2[][4]=
{ 
	"1","2","3"," C ",
	"4","5","6","",
	"7","8","9","",
	".","0","DEL"," s",};
u32 Input_Num(u8 num,u8 mode)
{
	u8 i,j;
	u8 key;
	u8 select=0;
//	u8 disp_flag=1;
//	u8 pos=0;//光标位置
	u8 keyvale=0;
	u8 lenth,key_count=0,dot_num=0,dot_num1=0,While_flag=1;
	vu8 Disp_buff[10];
	Test_ValueTypedef return_num;
	lenth=NUM_LENTH;	
	for(i=0;i<7;i++)
	Disp_buff[i]=' ';
	Disp_buff[7]=0;
	Colour.black=LCD_COLOR_TEST_MID;
	LCD_DrawRect(INPUT_RECT_X, INPUT_RECT_Y, INPUT_RECT_X+300, INPUT_RECT_Y+205,LCD_COLOR_CYAN);
	Colour.black =LCD_COLOR_CYAN;
	WriteString_16( INPUT_RECT_X+10, INPUT_RECT_Y+10, "INPUTNUM:", 0 ); 
	LCD_DrawRect_Rect(INPUT_RECT_X+100, INPUT_RECT_Y+5, INPUT_RECT_X+100+150, INPUT_RECT_Y+5+25,LCD_COLOR_MAGENTA);//LCD_DrawFullRect

	DISP_FLAG=TRUE;
	//Touch_beep();
	while(While_flag)
	{
		if(DISP_FLAG)
		{
			DISP_FLAG=FALSE;
			for(i=0;i<4;i++)
			{
				for(j=0;j<4;j++)
				{ 
					if(i*4+j==keyvale)
						select=1;
					else
						select=0;
					if(mode==0)
						disp_Numtouch(INPUT_RECT_X+5+j*75,INPUT_RECT_Y+35+i*42,select,(u8 *)TOUCH_NUM[i*4+j]);
					else if(mode==1)
						disp_Numtouch(INPUT_RECT_X+5+j*75,INPUT_RECT_Y+35+i*42,select,(u8 *)TOUCH_NUM1[i*4+j]);
					else
						disp_Numtouch(INPUT_RECT_X+5+j*75,INPUT_RECT_Y+35+i*42,select,(u8 *)TOUCH_NUM2[i*4+j]);
						
						
				}
			
			}
            Colour.Fword=LCD_COLOR_RED;
			WriteString_16(INPUT_RECT_X+108, INPUT_RECT_Y+10, (const uint8_t *)Disp_buff,  0);
			
		}
		key=Key_Read_WithTimeOut(TICKS_PER_SEC_SOFTTIMER/10);//等待按键(100*10ms/10=100ms)

		if(key!=KEY_NONE)
		{
			DISP_FLAG=TRUE;
			switch(key)
			{

		
				case Key_UP:	//上键
	//			case L_KEY_UP:	//长按上键
					if(keyvale>4)
						keyvale-=4;

					break;

				case Key_DOWN:		//下键
	//			case L_KEY_DOWN:	//长按下键
					if(keyvale<12)
						keyvale+=4;

					break;

				case Key_START:		//启动键
				//case L_KEY_START:	//长按启动键
					SetSystemStatus(SYS_STATUS_TEST);//测试状态
					break;
		
				case Key_FAST:		//确认键
					switch(keyvale)
					{
						case 0:
							if(key_count<lenth)
							{
								Disp_buff[key_count]='1';
								key_count++;
							}
							break;
						case 1:
							if(key_count<lenth)
							{
								Disp_buff[key_count]='2';
								key_count++;
							}
							break;
						case 2:
							if(key_count<lenth)
							{
								Disp_buff[key_count]='3';
								key_count++;
							}
							break;
						case 3:
//							return_num.uint=0;
//					
//							While_flag=0;
//							if(key_count<lenth)
//							{
//								if(dot_num==0)
//								{
//									if(key_count>0)
//									{
//										Disp_buff[key_count]='.';
//										dot_num1=key_count;
//										key_count++;
//									
//									
//									}
//									dot_num++;
//								}
//							
//							}
							key_count=0;
							for(i=0;i<7;i++)
								Disp_buff[i]=' ';
							break;
						case 4:
							if(key_count<lenth)
							{
								Disp_buff[key_count]='4';
								key_count++;
							}
							break;
						case 5:
							if(key_count<lenth)
							{
								Disp_buff[key_count]='5';
								key_count++;
							}
							break;
						case 6:
							if(key_count<lenth)
							{
								Disp_buff[key_count]='6';
								key_count++;
							}
							break;
						case 7:
//							return_num.uint=1;
//					
//							While_flag=0;
//							if(key_count<lenth)
//							{
//								if(dot_num==0)
//								{
//									if(key_count>0)
//									{
//										Disp_buff[key_count]='.';
//										dot_num1=key_count;
//										key_count++;
//									
//									
//									}
//									dot_num++;
//								}
//							
//							}
							break;
						case 8:
							if(key_count<lenth)
							{
								Disp_buff[key_count]='7';
								key_count++;
							}
							break;
						case 9:
							if(key_count<lenth)
							{
								Disp_buff[key_count]='8';
								key_count++;
							}
							break;
						case 10:
							if(key_count<lenth)
							{
								Disp_buff[key_count]='9';
								key_count++;
							}
							break;
						case 11:
//							return_num.uint=2;
//					
//							While_flag=0;
//							if(key_count<lenth)
//							{
//								if(dot_num==0)
//								{
//									if(key_count>0)
//									{
//										Disp_buff[key_count]='.';
//										dot_num1=key_count;
//										key_count++;
//									
//									
//									}
//									dot_num++;
//								}
//							
//							}
							break;
						case 12:
							if(key_count<lenth&&key_count>0)
							{
								if(dot_num==0)
								{
									if(key_count>0)
									{
										Disp_buff[key_count]='.';
										dot_num1=key_count;
										key_count++;
									
									
									}
									dot_num++;
								}
							}
							break;
						case 13:
							if(key_count<lenth)
							{
								Disp_buff[key_count]='0';
								key_count++;
							}
							break;
						case 14:
							if(key_count>0)
							{	
								key_count--;
								Disp_buff[key_count]=' ';
								if(dot_num1==key_count)
								{
									dot_num=0;
									dot_num1=0;
									
								}
							
							}
//							else
//							{
//								if(Disp_buff[key_count]==0)
//										Disp_buff[key_count]='-';
//									else if(Disp_buff[key_count]=='-')
//										Disp_buff[key_count]='+';
//									else
//										Disp_buff[key_count]='-';
//									key_count++;
//							}
							break;
						case 15:
							return_num.uint=3;
					
							While_flag=0;
							if(key_count<lenth)
							{
								if(dot_num==0)
								{
									if(key_count>0)
									{
										Disp_buff[key_count]='.';
										dot_num1=key_count;
										key_count++;
									
									
									}
									dot_num++;
								}
							
							}
							break;
						default:
							break;
					
					}
//					delay_ms(500);
					break;

				case Key_LEFT:
					if(keyvale>0)
						keyvale--;
					else
						keyvale=15;
					break;
				case Key_RIGHT:
					if(keyvale>15)
						keyvale=0;
					else
						keyvale++;
					break;
				
				default:
					break;
			}
		}
	
	
	}
	for(i=key_count;i<lenth;i++)
		Disp_buff[i]='0';
	return_num.res=0;
	if(mode!=2)
	{
		
		for(i=0;i<lenth;i++)
		{
			if((Disp_buff[i]>='0')&&(Disp_buff[i]<='9'))
			{

				return_num.res*=10;
				return_num.res+=Disp_buff[i]-'0';				
			
			}
			else
			break;	
		}
	}
	else
	{
		for(i=0;i<lenth;i++)
		{
			if(Disp_buff[0]>='0'&&(Disp_buff[0]<='9'))
			{
				if(Disp_buff[i]>='0'&&(Disp_buff[i]<='9'))
				{
				
					if(dot_num1>i)
					{
						return_num.res*=10;
						return_num.res+=Disp_buff[i]-'0';
					
					}
					else
					{
						return_num.res*=10;
						return_num.res+=Disp_buff[i]-'0';
						break;
					
					}
				}
			
			}	
		}
	
	}
//	return_num.dot=dot_num1;
//	if(mode)
//	{
//		return_num=Time_Set_Cov_num( &return_num);
//	}
//	else
//	 return_num=Time_Set_Cov( &return_num);
	lcd_Clear(LCD_COLOR_TEST_BACK);
	Disp_Test_Set_Item();
	return return_num.res;



}



void Disp_big_HZ(u8 list)
{
//	Colour.black=LCD_COLOR_TEST_BAR;
//	LCD_DrawRect( DISP_BIG_X1-5, DISP_BIG_Y1-3, DISP_BIG_X1+80-5, DISP_BIG_Y1+80-3 , LCD_COLOR_TEST_BAR );
//	LCD_DrawRect_Rect( DISP_BIG_X1-2-5, DISP_BIG_Y1-2-3, DISP_BIG_X1+81-5, DISP_BIG_Y1+81 -3, LCD_COLOR_BLACK );
//	switch(list)
//	{
//		case MSG_IDLE://等待测试
////			LCD_ShowFontCN_40_55(DISP_BIG_X1,DISP_BIG_Y1,32,32,(uint8_t*)big_HZ+0*32*32/8);//等
////			LCD_ShowFontCN_40_55(DISP_BIG_X1+DISP_SPACE1,DISP_BIG_Y1,32,32,(uint8_t*)big_HZ+1*32*32/8);//待
////		
////			LCD_ShowFontCN_40_55(DISP_BIG_X1,DISP_BIG_Y1+DISP_SPACE1,32,32,(uint8_t*)big_HZ+2*32*32/8);//测
////			LCD_ShowFontCN_40_55(DISP_BIG_X1+DISP_SPACE1,DISP_BIG_Y1+DISP_SPACE1,32,32,(uint8_t*)big_HZ+3*32*32/8);//试
//			break;
//		case MSG_TEST:
////			LCD_ShowFontCN_40_55(DISP_BIG_X1,DISP_BIG_Y1,32,32,(uint8_t*)big_HZ+0*32*32/8);//等
////			LCD_ShowFontCN_40_55(DISP_BIG_X1+DISP_SPACE1,DISP_BIG_Y1,32,32,(uint8_t*)big_HZ+1*32*32/8);//待
//		
////			LCD_ShowFontCN_40_55(DISP_BIG_X1,DISP_BIG_Y1,32,32,(uint8_t*)big_HZ+2*32*32/8);//测
////			LCD_ShowFontCN_40_55(DISP_BIG_X1+DISP_SPACE1,DISP_BIG_Y1,32,32,(uint8_t*)big_HZ+3*32*32/8);//试
////			
////			LCD_ShowFontCN_40_55(DISP_BIG_X1,DISP_BIG_Y1+DISP_SPACE1,32,32,(uint8_t*)big_HZ+4*32*32/8);//中
////			LCD_ShowFontCN_40_55(DISP_BIG_X1+DISP_SPACE1,DISP_BIG_Y1+DISP_SPACE1,32,32,(uint8_t*)big_HZ+13*32*32/8);//。。。
//			break;
//		case MSG_ABORT:
////			LCD_ShowFontCN_40_55(DISP_BIG_X1,DISP_BIG_Y1,32,32,(uint8_t*)big_HZ+14*32*32/8);//暂
////			LCD_ShowFontCN_40_55(DISP_BIG_X1+DISP_SPACE1,DISP_BIG_Y1,32,32,(uint8_t*)big_HZ+15*32*32/8);//停
////		
////			LCD_ShowFontCN_40_55(DISP_BIG_X1,DISP_BIG_Y1+DISP_SPACE1,32,32,(uint8_t*)big_HZ+2*32*32/8);//测
////			LCD_ShowFontCN_40_55(DISP_BIG_X1+DISP_SPACE1,DISP_BIG_Y1+DISP_SPACE1,32,32,(uint8_t*)big_HZ+3*32*32/8);//试
//			break;
//		case MSG_PASS:
////			LCD_ShowFontCN_40_55(DISP_BIG_X1,DISP_BIG_Y1,32,32,(uint8_t*)big_HZ+2*32*32/8);//测
////			LCD_ShowFontCN_40_55(DISP_BIG_X1+DISP_SPACE1,DISP_BIG_Y1,32,32,(uint8_t*)big_HZ+3*32*32/8);//试
////			
////			LCD_ShowFontCN_40_55(DISP_BIG_X1,DISP_BIG_Y1+DISP_SPACE1,32,32,(uint8_t*)big_HZ+5*32*32/8);//合
////			LCD_ShowFontCN_40_55(DISP_BIG_X1+DISP_SPACE1,DISP_BIG_Y1+DISP_SPACE1,32,32,(uint8_t*)big_HZ+6*32*32/8);//格
//			break;
//		
//		case MSG_LOW:
////			LCD_ShowFontCN_40_55(DISP_BIG_X1,DISP_BIG_Y1,32,32,(uint8_t*)big_HZ+17*32*32/8);//小
////			LCD_ShowFontCN_40_55(DISP_BIG_X1+DISP_SPACE1,DISP_BIG_Y1,32,32,(uint8_t*)big_HZ+18*32*32/8);//于
////			
////			LCD_ShowFontCN_40_55(DISP_BIG_X1,DISP_BIG_Y1+DISP_SPACE1,32,32,(uint8_t*)big_HZ+20*32*32/8);//下
////			LCD_ShowFontCN_40_55(DISP_BIG_X1+DISP_SPACE1,DISP_BIG_Y1+DISP_SPACE1,32,32,(uint8_t*)big_HZ+21*32*32/8);//限
//			break;
//		case MSG_HIGH:
////			LCD_ShowFontCN_40_55(DISP_BIG_X1,DISP_BIG_Y1,32,32,(uint8_t*)big_HZ+16*32*32/8);//大
////			LCD_ShowFontCN_40_55(DISP_BIG_X1+DISP_SPACE1,DISP_BIG_Y1,32,32,(uint8_t*)big_HZ+18*32*32/8);//于
////			
////			LCD_ShowFontCN_40_55(DISP_BIG_X1,DISP_BIG_Y1+DISP_SPACE1,32,32,(uint8_t*)big_HZ+19*32*32/8);//上
////			LCD_ShowFontCN_40_55(DISP_BIG_X1+DISP_SPACE1,DISP_BIG_Y1+DISP_SPACE1,32,32,(uint8_t*)big_HZ+21*32*32/8);//限
//			break;
//		case MSG_SHORT:
////			LCD_ShowFontCN_40_55(DISP_BIG_X1,DISP_BIG_Y1,32,32,(uint8_t*)big_HZ+2*32*32/8);//测
////			LCD_ShowFontCN_40_55(DISP_BIG_X1+DISP_SPACE1,DISP_BIG_Y1,32,32,(uint8_t*)big_HZ+3*32*32/8);//试
////			
////			LCD_ShowFontCN_40_55(DISP_BIG_X1,DISP_BIG_Y1+DISP_SPACE1,32,32,(uint8_t*)big_HZ+12*32*32/8);//短
////			LCD_ShowFontCN_40_55(DISP_BIG_X1+DISP_SPACE1,DISP_BIG_Y1+DISP_SPACE1,32,32,(uint8_t*)big_HZ+8*32*32/8);//路
//			break;
////		case MSG_HOPEN:
////			LCD_ShowFontCN_40_55(DISP_BIG_X1,DISP_BIG_Y1,32,32,(uint8_t*)big_HZ+11*32*32/8);//高
////			LCD_ShowFontCN_40_55(DISP_BIG_X1+DISP_SPACE1,DISP_BIG_Y1,32,32,(uint8_t*)big_HZ+10*32*32/8);//端
////			
////			LCD_ShowFontCN_40_55(DISP_BIG_X1,DISP_BIG_Y1+DISP_SPACE1,32,32,(uint8_t*)big_HZ+7*32*32/8);//开
////			LCD_ShowFontCN_40_55(DISP_BIG_X1+DISP_SPACE1,DISP_BIG_Y1+DISP_SPACE1,32,32,(uint8_t*)big_HZ+8*32*32/8);//路
//			break;
//		case MSG_LOPEN:
////			LCD_ShowFontCN_40_55(DISP_BIG_X1,DISP_BIG_Y1,32,32,(uint8_t*)big_HZ+9*32*32/8);//低
////			LCD_ShowFontCN_40_55(DISP_BIG_X1+DISP_SPACE1,DISP_BIG_Y1,32,32,(uint8_t*)big_HZ+10*32*32/8);//端
////			
////			LCD_ShowFontCN_40_55(DISP_BIG_X1,DISP_BIG_Y1+DISP_SPACE1,32,32,(uint8_t*)big_HZ+7*32*32/8);//开
////			LCD_ShowFontCN_40_55(DISP_BIG_X1+DISP_SPACE1,DISP_BIG_Y1+DISP_SPACE1,32,32,(uint8_t*)big_HZ+8*32*32/8);//路
//			break;
//		case MSG_CLEAR:
////			LCD_ShowFontCN_40_55(DISP_BIG_X1,DISP_BIG_Y1,32,32,(uint8_t*)big_HZ+22*32*32/8);//清
////			LCD_ShowFontCN_40_55(DISP_BIG_X1+DISP_SPACE1,DISP_BIG_Y1,32,32,(uint8_t*)big_HZ+23*32*32/8);//零
////			
////			LCD_ShowFontCN_40_55(DISP_BIG_X1,DISP_BIG_Y1+DISP_SPACE1,32,32,(uint8_t*)big_HZ+4*32*32/8);//中
////			LCD_ShowFontCN_40_55(DISP_BIG_X1+DISP_SPACE1,DISP_BIG_Y1+DISP_SPACE1,32,32,(uint8_t*)big_HZ+13*32*32/8);//...
//			break;
//		default:
//			break;
//	
//	
//	}
//	Colour.black=LCD_COLOR_TEST_BACK;

}
void Disp_open(void)
{

	LCD_DrawRect( 0, 0, 479, 271 , LCD_COLOR_TEST_BAR );
	LCD_DrawRect( 0, 67, 479, 163+66 , 0xfaaa );
	


}
//==========================================================
//函数名称：Password_Check
//函数功能：密码校验
//入口参数：无
//出口参数：TRUE:有效
//创建日期：2015.01.29 
//修改日期：2015.08.24 11:40
//备注说明：无
//==========================================================
//Bool Password_Check(void)
//{
//	u8 key;

//	if(MenuIndex!=2)//出厂信息界面
//	{
//		if(F_Password==TRUE)//密码有效标志	
//			return (TRUE);
//	
//		if(SaveData.Factory.Password==0)//密码为零时无需校验
//			return (TRUE);
//	}
//	else
//	{
//		Disp_Clr( );//清屏
//	}
//	
//	LcdAddr.x=10;LcdAddr.y=2;
//	Disp_Str("请输入密码",0);//显示
//	Lcd_Draw_Line(0,20,LCD_ADDR_X_MAX);//X=0:画横线，否则画竖线，len:长度或高度
//	Lcd_Draw_Line(0,107,LCD_ADDR_X_MAX);//X=0:画横线，否则画竖线，len:长度或高度
//	LcdAddr.x=1;LcdAddr.y=110;
//	Disp_Str("按<上,下>键修改,<确认>键输入",0);//显示

//	while(1)
//	{
//		LcdAddr.x=13;//设置显示地址
//		LcdAddr.y=50;
//		Disp_Str("****",0);//显示

//		LcdAddr.x=13;//设置显示地址
//		NumBox.Num=0;//Value
//		NumBox.Max=9999;//Max
//		NumBox.Min=0;//Min
//		NumBox.Dot=0;//Dot

//		key=Number_Setup(&NumBox);//数值框设置
//		switch(key)//按键处理
//		{
//			case KEY_UP:	//上键
////			case L_KEY_UP:	//长按上键
//				break;

//			case KEY_DOWN:		//下键
////			case L_KEY_DOWN:	//长按下键
//				break;

//			case KEY_LEFT:	//左键
////			case L_KEY_LEFT:	//长按左键
//				break;
//	
//			case KEY_SET:	//设置键
////			case L_KEY_SET:	//长按设置键
////				break;

//			case KEY_RIGHT:		//右键
////			case L_KEY_RIGHT:	//长按右键
//				break;
//	
//			case KEY_ENTER:		//确认键
////			case L_KEY_ENTER:	//长按确认键
//				Disp_Clr();//清屏
//				if(MenuIndex==2)//出厂信息界面
//				{
//					if(NumBox.Num==918)//出厂配置密码
//						return (TRUE);
//					else
//						return (FALSE);
//				}
//				else
//				{
//					if(NumBox.Num==SaveData.Factory.Password)//当前密码判别
//					{
//						F_Password=TRUE;//密码验证有效
//						return (TRUE);
//					}
//					else
//						return (FALSE);
//				}
//				break;
//	
//			case KEY_START:		//启动键
////			case L_KEY_START:	//长按启动键
//				break;
//	
//			case KEY_RESET:		//复位键
////			case L_KEY_RESET:	//长按复位键
//				return (FALSE);
//				break;
//			
//			default:
//				break;
//		}
//	}
//}
void Disp_Time(void)
{

}//SetupConfig_Button_Tab
void DispSetup_ConfigButtonvalue(u8 list)
{
    u8 i;
    GUI_RECT Rect;
    Disp_Fastbutton(1);
     GUI_SetBkColor(LCD_COLOR_TEST_BUTON);
    GUI_SetTextMode(GUI_TEXTMODE_TRANS);
    GUI_SetColor(GUI_WHITE);
    
            for(i=0;i<MAXBUTTONNUM;i++)
            {
                Rect.x0=FASTBUTTON_START;
                Rect.y0=TOP_BAR_HIGH+i*FASTBUTTON_HIGH+6*i;
                Rect.x1=FASTBUTTON_END;
                Rect.y1=TOP_BAR_HIGH+(i+1)*FASTBUTTON_HIGH+6*i;
                 
                GUI_DispStringInRect(SetupConfig_Button_Tab[list][U9001_Save_sys.U9001_SYS.language][i],&Rect,GUI_TA_HCENTER|GUI_TA_VCENTER);            
              
            }

}
void Disp_IdelButton(void)
{
	u8 i;
	GUI_RECT Rect;
	GUI_SetBkColor(LCD_COLOR_TEST_BUTON);
	GUI_SetTextMode(GUI_TEXTMODE_TRANS);
	GUI_SetColor(GUI_WHITE);
	for(i=0;i<MAXBUTTONNUM;i++)
	{
		Rect.x0=FASTBUTTON_START;
		Rect.y0=TOP_BAR_HIGH+i*FASTBUTTON_HIGH+6*i;
		Rect.x1=FASTBUTTON_END;
		Rect.y1=TOP_BAR_HIGH+(i+1)*FASTBUTTON_HIGH+6*i;
		 
		GUI_DispStringInRect(IdelButton_Tab[i][U9001_Save_sys.U9001_SYS.language],&Rect,GUI_TA_HCENTER|GUI_TA_VCENTER);            
	
	}

}


void dispSetupButtonvalue(u8 item,u8 list)
{
    u8 i;
    GUI_RECT Rect;
    Disp_Fastbutton(1);
//     GUI_SetBkColor(LCD_COLOR_TEST_BUTON);
    GUI_SetTextMode(GUI_TEXTMODE_TRANS);
    GUI_SetColor(GUI_WHITE);
    
        for(i=0;i<MAXBUTTONNUM;i++)
        {
            Rect.x0=FASTBUTTON_START;
            Rect.y0=TOP_BAR_HIGH+i*FASTBUTTON_HIGH+6*i;
            Rect.x1=FASTBUTTON_END;
            Rect.y1=TOP_BAR_HIGH+(i+1)*FASTBUTTON_HIGH+6*i;
            switch(item)
            {
                case AC:   
									if(SOFTWARE_VERSION!=3)
									{
                    GUI_DispStringInRect(SetupACButton_Tab[list][U9001_Save_sys.U9001_SYS.language][i],&Rect,GUI_TA_HCENTER|GUI_TA_VCENTER);   
									}else{
										 GUI_DispStringInRect(SetupACButton_Tab1[list][U9001_Save_sys.U9001_SYS.language][i],&Rect,GUI_TA_HCENTER|GUI_TA_VCENTER);
									}										
                    break;
                case DC:
                    GUI_DispStringInRect(SetupDCButton_Tab[list][U9001_Save_sys.U9001_SYS.language][i],&Rect,GUI_TA_HCENTER|GUI_TA_VCENTER); 
                    break;
                case IR:
                    GUI_DispStringInRect(SetupIRButton_Tab[list][U9001_Save_sys.U9001_SYS.language][i],&Rect,GUI_TA_HCENTER|GUI_TA_VCENTER); 
                    break;
                case OS:
									if(SOFTWARE_VERSION!=3)
									{
                    GUI_DispStringInRect(SetupOSButton_Tab[list][U9001_Save_sys.U9001_SYS.language][i],&Rect,GUI_TA_HCENTER|GUI_TA_VCENTER);   
									}else{
										 GUI_DispStringInRect(SetupOSButton_Tab1[list][U9001_Save_sys.U9001_SYS.language][i],&Rect,GUI_TA_HCENTER|GUI_TA_VCENTER);
									}
//                    GUI_DispStringInRect(SetupOSButton_Tab[list][U9001_Save_sys.U9001_SYS.language][i],&Rect,GUI_TA_HCENTER|GUI_TA_VCENTER); 
                    break;
                case PA:
									if(SOFTWARE_VERSION!=3)
									{
                    GUI_DispStringInRect(SetupPAButton_Tab[list][U9001_Save_sys.U9001_SYS.language][i],&Rect,GUI_TA_HCENTER|GUI_TA_VCENTER);   
									}else{
										 GUI_DispStringInRect(SetupPAButton_Tab1[list][U9001_Save_sys.U9001_SYS.language][i],&Rect,GUI_TA_HCENTER|GUI_TA_VCENTER);
									}
//                    GUI_DispStringInRect(SetupPAButton_Tab[list][U9001_Save_sys.U9001_SYS.language][i],&Rect,GUI_TA_HCENTER|GUI_TA_VCENTER); 
                    break;
                    
            }
        }
 
}
void dispSYSSetButtonvalue(u8 list)
{
    u8 i;
    GUI_RECT Rect;
	Disp_Fastbutton(1);
//     GUI_SetBkColor(LCD_COLOR_TEST_BUTON);
    GUI_SetTextMode(GUI_TEXTMODE_TRANS);
    GUI_SetColor(GUI_WHITE);
    
            for(i=0;i<MAXBUTTONNUM;i++)
            {
                Rect.x0=FASTBUTTON_START;
                Rect.y0=TOP_BAR_HIGH+i*FASTBUTTON_HIGH+6*i;
                Rect.x1=FASTBUTTON_END;
                Rect.y1=TOP_BAR_HIGH+(i+1)*FASTBUTTON_HIGH+6*i;
                GUI_DispStringInRect(SysSetButton_Tab[list][U9001_Save_sys.U9001_SYS.language][i],&Rect,GUI_TA_HCENTER|GUI_TA_VCENTER);            

//                switch(item)
//                {
//                    case AC:   
//                        GUI_DispStringInRect(SetupACButton_Tab[list][U9001_save_sys.U9001_SYS.language][i],&Rect,GUI_TA_HCENTER|GUI_TA_VCENTER);            
//                        break;
//                    case DC:
//                        GUI_DispStringInRect(SetupDCButton_Tab[list][U9001_save_sys.U9001_SYS.language][i],&Rect,GUI_TA_HCENTER|GUI_TA_VCENTER); 
//                        break;
//                    case IR:
//                        GUI_DispStringInRect(SetupIRButton_Tab[list][U9001_save_sys.U9001_SYS.language][i],&Rect,GUI_TA_HCENTER|GUI_TA_VCENTER); 
//                        break;
//                    case OS:
//                        GUI_DispStringInRect(SetupOSButton_Tab[list][U9001_save_sys.U9001_SYS.language][i],&Rect,GUI_TA_HCENTER|GUI_TA_VCENTER); 
//                        break;
//                    case PA:
//                        GUI_DispStringInRect(SetupPAButton_Tab[list][U9001_save_sys.U9001_SYS.language][i],&Rect,GUI_TA_HCENTER|GUI_TA_VCENTER); 
//                        break;
//                        
//                }
            }

}
void dispSYSButtonvalue(void)
{
    u8 i;
    GUI_RECT Rect;
	Disp_Fastbutton(1);
	 GUI_SetTextMode(GUI_TEXTMODE_TRANS);
//     GUI_SetBkColor(LCD_COLOR_TEST_BUTON);
//    GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
    GUI_SetColor(GUI_WHITE);
    for(i=0;i<MAXBUTTONNUM;i++)
    {
        Rect.x0=FASTBUTTON_START;
        Rect.y0=TOP_BAR_HIGH+i*FASTBUTTON_HIGH+6*i;
        Rect.x1=FASTBUTTON_END;
        Rect.y1=TOP_BAR_HIGH+(i+1)*FASTBUTTON_HIGH+6*i;
        GUI_DispStringInRect(SYS_DispButton_Tab[i][U9001_Save_sys.U9001_SYS.language],&Rect,GUI_TA_HCENTER|GUI_TA_VCENTER);            
        
    }

}
//==========================================================
//函数名称：Range_Control
//函数功能：量程控制
//入口参数：u8 dat
//出口参数：无
//创建日期：2014.09.25
//修改日期：2015.09.04 15:35
//备注说明：
//==========================================================
u8 Range_Set[]={0x10,0x08,0x01,0x04,0x02,0x00};//50R 500R 5K 50K 500K 5M
void Range_Control(u8 dat)
{
    if(dat>5)
        dat=5;
    _4094_databuff[0]&=0XE0;
    _4094_databuff[0]|=Range_Set[dat];
    
    Out_4094(_4094_databuff);
    delayMs(1,50);
}
//vu8 Range_Changecomp(void)
//{
//	
//	uint8_t  f_upper=FALSE;//量程过压标志
//	uint8_t  f_below=FALSE;//量程欠压标志
//	uint8_t  f_switch=FALSE;//量程切换标志
//	//量程自动换挡处?Test_value.Test_I
//	if(Tft_5520_Dispvalue.Test_R>RANGR_LIMIT_HIGH)//高于量程上限 大于190M  RANGE要增加
//	{
//		f_upper=TRUE;//量程过压标志
//		f_below=FALSE;//量程欠压标志

//	}
//	else if(Tft_5520_Dispvalue.Test_R<RANGR_LIMIT_LOW)//低于量程下限
//	{
//		f_below=TRUE;//量程低压标志
//		f_upper=FALSE;//量程欠压标志
//	}
//	else
//	{
//		f_upper=FALSE;//量程过压标志
//		f_below=FALSE;//量程欠压标志
//	}
//	f_switch=FALSE;//量程切换标志
//	if(Tft_5520.Set_Value[Tft_5520.Group].Range==0)
//	{
//		if((Range<RANGE_MAX-1-VRange_flag)&&(f_upper==TRUE))//量程非最高且低压
//		{
//			Range++;
//			f_switch=TRUE;//量程切换标志

//		}
//		if((Range>0)&&(f_below==TRUE))//量程非最低且过压
//		{
//			Range--;
//			f_switch=TRUE;//量程切换标志
//		}
//	}

//	if(f_switch==TRUE||Clear_flag==1)//量程切换标志
//	{
//        Clear_flag=0;
//		Range_Control(Range+VRange_flag);//量程控制
//	}
//    return f_switch;
//}
void ACW_Select(void)//交流选择
{

    
   _4094_databuff[0]|=HW_RY_AC_ON;
    _4094_databuff[0]&=HW_RY_DC_OFF;
    
    Out_4094(_4094_databuff);
    
    
    FRB_out(1);
}
void DCW_Select(void)//直流选择
{
    
    _4094_databuff[0]|=HW_RY_DC_ON;
    _4094_databuff[0]&=HW_RY_AC_OFF;
    
    
    Out_4094(_4094_databuff);
    
     FRB_out(2);
    
}
void OS_Select(void)//直流选择
{
    
    _4094_databuff[0]|=HW_RY_DC_ON;
    _4094_databuff[0]|=HW_RY_AC_ON;
    
    
    Out_4094(_4094_databuff);
    
     FRB_out(3);
    
}
void Short_out(u8 date)//0  停止  1  启动
{
    _4094_databuff[1]|=0x40;
    if(date)
    {
        
        _4094_databuff[1]&=0x7f;
    }else
    {
        _4094_databuff[1]|=0x80;
    }
    
    
    Out_4094(_4094_databuff);
}
const u8 ARC_Range[]=
{
    0x3f,38,34,30,26,23,19,14,10,5
};    
void ARC_out(u8 start)//0  停止  1  启动
{
    if(start==0)
    {
        _4094_databuff[1]|=0x3f;
    }else
    {
        _4094_databuff[1]&=0xC0;
        if(U9001_Save_sys.U9001_Testconfg.ARC_mode)//等级模式
        {
            _4094_databuff[1]|=ARC_Range[U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc];
        }else
        {
            if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc)
                _4094_databuff[1]|=(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc-1)/5;
            else
                _4094_databuff[1]|=ARC_Range[0];//arc最大值
        }
    }
    
    Out_4094(_4094_databuff);
}
//==========================================================
//函数名称：Test_Init
//函数功能：测试初始化
//入口参数：无
//出口参数：无
//创建日期：2015.01.25 
//修改日期：2015.08.26 13:48
//备注说明：耗时约100ms/12M
//==========================================================
void Test_Init(void)
{
	u8 i;
	u8 len;
	u8 *pt;
	u8 *pt2;
	u32 dat;
	u16 set_item;//设置的输出项目
	u16 set_out;//设置的输出值
	u16 set_ramp;//设置的缓升或延时时间
    u16 set_drop;//设置的缓升或延时时间
	vu8 set_arc;//设置的电弧等级

	//系统信息更新
	SetSystemMessage(MSG_RAMP);//系统信息-缓升

	//读取设置值
	set_item=U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter;//当前参数
	set_out=U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out;//设置的输出
	set_ramp=U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].rise_time;//缓升或延时时间
    set_drop=U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].drop_time;
	set_arc=U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc;//电弧等级
    Short_out(1);//short中断比较设置
    GFI_out();//GFI中断比较设置
    ARC_out(1);//ARC中断比较设置
	//读取校准值
	if(set_item==ACW_SETUP)
	{
		i=3;//ACW
		pt=(u8*)(&U9001_Calibrate.AcwVol);//校准值首地址
	}
	else if(set_item==DCW_SETUP)
	{
		i=4;//DCW
		pt=(u8*)(&U9001_Calibrate.DcwVol);//校准值首地址
	}
	else if(set_item==IR_SETUP)
	{
		i=6;//IR
		pt=(u8*)(&U9001_Calibrate.IrVol);//校准值首地址
	}
	else
	{
		i=3;//ACW
		pt=(u8*)(&U9001_Calibrate.AcwVol);//校准值首地址
//        pt=(u8*)(&U9001_Calibrate.IrVol);//校准值首地址
//		i=2;//GND
//		pt=(u8*)(&SaveData.Calibrate.GndVol);//校准值首地址
	}
	
	pt2=(u8*)(&Cal);//更新通用校准值
	len=i*sizeof(Cal_TypeDef);
	for(i=0;i<len;i++)
	{
		*pt2++=*pt++;
	}

	//全局变量处理
	Voltage=0;//电压
	Current=0;//电流
	Resistance=0;//电阻
//	TestTime=0;//时间
 	AdCount=0;//A/D值采样计数

	RampStepYs=0;//缓升步进余数
	RampStepFd=0;//缓升步进幅度
	RampStepTime=0;//缓升步进时间
	TestOut=0;//测试输出
	FullOut=0;//满载输出
	F_100ms=FALSE;//100ms定时
	
	//AD缓冲区清空
	for(i=0;i<(AD_BUF_LENGTH);i++)
	{
		Vbuf[i]=0;
		Ibuf[i]=0;
	}

	//满载D/A输出值计算
//    if(set_item==OS_SETUP)
//        set_out=115;
	dat=set_out;
	dat*=DefaultCalDaOut[set_item][0];
    
	dat/=Cal[0].Num;
    if(set_item==OS_SETUP)
        dat=260;
	FullOut=(u16)dat;

	//缓升值计算
	if(set_ramp)//缓升时间
	{
		RampStepTime=1;//缓升步进时间,100ms
		dat=FullOut;dat/=set_ramp;
		RampStepFd=(u16)dat;//缓升步进幅度
		dat=FullOut;dat%=set_ramp;
		RampStepYs=(u16)dat;//缓升步进余数
		if(RampStepFd<1)
		{
			RampStepTime=10;//缓升步进时间,1s
			dat=FullOut;dat*=10;dat/=set_ramp;
			RampStepFd=(u16)dat;//缓升步进幅度
			dat=FullOut;dat*=10;dat%=set_ramp;
			RampStepYs=(u16)dat;//缓升步进余数
			if(RampStepFd<1)
			{
				RampStepTime=100;//缓升步进时间,10s
				dat=FullOut;dat*=10;dat/=set_ramp;
				RampStepFd=(u16)dat;//缓升步进幅度
				dat=FullOut;dat*=10;dat%=set_ramp;
				RampStepYs=(u16)dat;//缓升步进余数
			}
		}
	}
    
    //缓降值计算
	if(set_drop)//缓升时间
	{
		DropStepTime=1;//缓升步进时间,100ms
		dat=FullOut;dat/=set_drop;
		DropStepFd=(u16)dat;//缓升步进幅度
		dat=FullOut;dat%=set_drop;
		DropStepYs=(u16)dat;//缓升步进余数
		if(DropStepFd<1)
		{
			DropStepTime=10;//缓升步进时间,1s
			dat=FullOut;dat*=10;dat/=set_drop;
			DropStepFd=(u16)dat;//缓升步进幅度
			dat=FullOut;dat*=10;dat%=set_drop;
			DropStepYs=(u16)dat;//缓升步进余数
			if(DropStepFd<1)
			{
				DropStepTime=100;//缓升步进时间,10s
				dat=FullOut;dat*=10;dat/=set_drop;
				DropStepFd=(u16)dat;//缓升步进幅度
				dat=FullOut;dat*=10;dat%=set_drop;
				DropStepYs=(u16)dat;//缓升步进余数
			}
		}
	}

	//输出控制
	switch(set_item)//参数
	{
		case ACW_SETUP:
            
            ACW_Select();
//			Dc_Output_Off();//DC输出和采样关闭
			if(U9001_Save_sys.U9001_Testconfg.hz)
				Sin_out(1);//输出频率控制
			else
				Sin_out(0);//输出频率控制
            if(U9001_Save_sys.U9001_Testconfg.Autorange == 0)
			{
				if(Test_mid.set_high < 3000)
				{
					Range=1;
				}else{
					Range=0;
				}
			}else if(U9001_Save_sys.U9001_Testconfg.Autorange == 1){
				Range=0;//量程初始
			}
//			Arc_Out(set_arc);//电弧等级输出
			break;

		case DCW_SETUP:
            DCW_Select();
            Sin_out(2);//400HZ
			if(U9001_Save_sys.U9001_Testconfg.Autorange == 0)
			{
				if(Test_mid.set_high < 3000)
				{
					Range=1;
				}else{
					Range=0;
				}
			}else if(U9001_Save_sys.U9001_Testconfg.Autorange == 1){
				Range=0;//量程初始
			}
//			Dc_Output_On();//DC输出和采样
//			Frequency_Control(FREQ_400);//输出频率控制
//			Arc_Out(set_arc);//电弧等级输出
			break;

		case IR_SETUP:
            DCW_Select();
            Range=1;//量程初始
            Sin_out(2);//400HZ
//			Dc_Output_On();//DC输出和采样
//			Frequency_Control(FREQ_400);//输出频率控制
//			SetSystemMessage(MSG_TEST);//系统信息-满载测试
			break;

		case OS_SETUP:
            OS_Select();
            Sin_out(2);//400HZ
            Range=1;//量程初始
            SetSystemMessage(MSG_TEST);//系统信息-满载测试
//			Dc_Output_Off();//DC输出和采样关闭
//			if(SaveData.System.Freq60)
//				Frequency_Control(FREQ_60);//输出频率控制
//			else
//				Frequency_Control(FREQ_50);//输出频率控制
			break;
        case PA_SETUP:
            break;
		default:
			SetSystemStatus(SYS_STATUS_TEST_PAUSE);//测试暂停状态
			return;
			break;
	}		

 	
	Range_Control(Range);//量程控制
    Sing_out_C(1);//输出正弦波频率
	
	Uart0_Send(0x02);//关灯
	PLC_CompOff();//关PLC分选
	Beep_Off();//关蜂鸣器
//	//声光状态控制
//	Led_Pass_Off();//关合格灯
//	Led_Fail_Off();//关报警灯
//	Led_HV_On();//开高压灯
//	if(SaveData.System.Beep)
//		Beep_One();//响一声

	//显示测试界面
//	LcdAddr.x=TITLE_ADDR_X;	LcdAddr.y=TITLE_ADDR_Y;
//	Disp_Str("测试中..",0);//显示测试状态
//	//显示测试参数
//	LcdAddr.x=TITLE_ADDR_X+14; LcdAddr.y=TITLE_ADDR_Y;
//	Disp_Str(&ItemTab[set_item],0);//显示参数

//	//显示隔离线
//	Lcd_Draw_Line(0,TITLE_ADDR_Y+18,LCD_ADDR_X_MAX);//X=0:画横线，否则画竖线，len:长度或高度
//	Lcd_Draw_Line(0,TITLE_ADDR_Y+20,LCD_ADDR_X_MAX);//X=0:画横线，否则画竖线，len:长度或高度

//	//显示步骤序号
//	LcdAddr.x=STEP_ADDR_X; LcdAddr.y=STEP_ADDR_Y+TotalStep*HEIGHT_OF_STEP;
//	Disp_Str(&NumTab[TotalStep][0],0);//显示测试总步骤
//	Disp_Str(&ItemCharTab[set_item],0);//显示参数缩写字符

//	//显示初始化
//	LcdAddr.x=DATA_ADDR_X; LcdAddr.y=DATA_ADDR_Y+HEIGHT_OF_STEP*TotalStep;
	switch(set_item)//按项目显示
	{
		case ACW_SETUP:
		case DCW_SETUP:
//			Disp_Txt_Str("-----V -----mA -----s ");//显示测试值
			break;

		case IR_SETUP:
//			Disp_Txt_Str("----V  ----M  -----s ");//显示测试值
			break;

		case PA_SETUP:
//			Disp_Txt_Str("-----A ----m  -----s ");//显示测试值
			break;
        case OS_SETUP:
//			Disp_Txt_Str("-----A ----m  -----s ");//显示测试值
			break;
		
		default:
			break;
	}

	//显示测试信息
//	LcdAddr.x=MSG_ADDR_X;
//	Disp_SysMessage();//显示信息

}
//==========================================================
//函数名称：Get_Result
//函数功能：计算测试值结果
//入口参数：无
//出口参数：无
//创建日期：2014.11.08 
//修改日期：2015.08.27 14:43
//备注说明：更新Voltage
//==========================================================
void Get_Result(void)
{
	static u32 dat;
    static u16 v;
	//最大AD值判别
	if(Voltage>AD_MAX)//最大AD值判别
		Voltage=AD_MAX;
	v=Voltage/2;
	//校准处理
	dat=Voltage*10;//放大10倍
	dat*=Cal[0].Num;
	dat/=Cal[0].NumAd;
    
	//四舍五入
//	if(dat%10>4)dat+=10;
	dat/=20;
	Voltage=(u16)dat;
    Test_Value.Vol=Voltage;
	//最大AD值判别
	if(Current>AD_MAX)//最大AD值判别
		Current=AD_MAX;

	//校准处理
    if(Test_mid.set_item==IR_SETUP)
    {
        Resistance=jisuandianzu(Cal[Range].Num,Cal[Range].NumAd,Current,v,Cal[0].NumAd);
                        Test_Value.I_R=Resistance;
    
    } else if(Test_mid.set_item==OS_SETUP)
    {
		RZ = (float)Voltage/(float)Current;
		Cvalue = 2*3.14*400/RZ;
		if(Range == 5)
		{
			Cvalue/=73.1;
		}else if(Range == 4){
			Cvalue/=43.08;
		}
		else if(Range == 3){
			Cvalue/=43.08;
		}else if(Range == 2){
			Cvalue/=204.257;
		}else if(Range == 1){
			Cvalue/=77.81;
		}
//        Resistance=jisuandianzu(0x138a,0x99b,Current,v,100);//5002,2459
//		Resistance=jisuandianzu(0x138a,0x99b,Current,50,100);//5002,2459
//        Test_Value.I_R=Resistance;
		Test_Value.I_R=(u16)Cvalue;
    }
    else
    {
        dat=Current*10;//放大10倍	
        dat*=Cal[Range+1].Num;
        dat/=Cal[Range+1].NumAd;
//		 dat*=Cal[Range].Num;
//        dat/=Cal[Range].NumAd;
        //四舍五入
        if(dat%10>4)dat+=10;
        dat/=10;
        Current=(u16)dat;
    }
}
void InputNum(void)
{
    
    GUI_RECT rect;
    GUI_SetColor(GUI_WHITE);
    GUI_FillRect(20,214,350,242);
    rect.x0=30;rect.y0=216;rect.x1=115;rect.y1=240;
    GUI_DrawGradientRoundedV(rect.x0, rect.y0, rect.x1, rect.y1, 4, GUI_LIGHTBLUE, GUI_DARKBLUE);
    GUI_SetColor(GUI_WHITE);
    GUI_SetFont(GUI_FONT_16B_ASCII);
    
    GUI_DispStringInRect("INPUTNUM:",&rect,GUI_TA_VCENTER|GUI_TA_HCENTER);
//    GUI_DispStringAt("INPUTNUM:",30,220);
    GUI_SetFont(&GUI_FontGUI_FONTXBFFON16);
    GUI_SetColor(GUI_WHITE);
    GUI_FillRect(132,217,250,240);
    GUI_SetColor(GUI_GRAY);
    GUI_DrawRect(132,217,250,240);
    GUI_SetColor(GUI_DARKGRAY);
    GUI_SetPenSize(2);
    GUI_DrawLine(130,215,250,215);
    GUI_DrawLine(130,217,130,240);
    
    
    GUI_SetColor(CL_GREY4);
    GUI_DrawLine(132,237,250,239);
    GUI_DrawLine(250,217,250,239);
     GUI_SetColor(GUI_WHITE);

}

//数字键输入显示
Sort_TypeDef Disp_NumKeyboard_Set(Disp_Coordinates_Typedef *Coordinates )
{
	vu8 While_flag=1;
	vu8 Disp_buff[10];
	vu8 key,i;
	vu8 dispflag=1;
	vu8 dot_num=0,dot_num1=0;
//	vu8 page=0;
	vu32 keynum=0;
	vu8 key_count=0;
//	vu32 Num[6]={1,10,100,1e3,1e4,1e5};
	Sort_TypeDef   Sort_set;
	Sort_set.Dot=0;
	Sort_set.Num=0;
	Sort_set.Unit=0;
	Sort_set.Num=0;
	for(i=0;i<6;i++)
	Disp_buff[i]=' ';
	Disp_buff[7]=0;
	
	while(While_flag)
	{
		
		if(keyvalue!=KEY_NONE)
		{
			dispflag=1;
			Key_Beep();
			switch(key)
			{
				case Key_F1:
					
					Sort_set.Unit=0;
					
					While_flag=0;
					if(key_count<NUM_LENTH)
					{
						if(dot_num==0)
						{
							if(key_count>0)
							{
								Disp_buff[key_count]='.';
								dot_num1=key_count;
								key_count++;
							
							
							}
							dot_num++;
						}
					
					}
						
					
					Sort_set.Updata_flag=1;
					
				break;
				case Key_F2:
					Sort_set.Unit=1;
					
					While_flag=0;
					if(key_count<NUM_LENTH)
					{
						if(dot_num==0)
						{
							if(key_count>0)
							{
								Disp_buff[key_count]='.';
								dot_num1=key_count;
								key_count++;
							
							
							}
							dot_num++;
						}
					
					}
						
					
					Sort_set.Updata_flag=1;
					
				break;
				case Key_F3:
					
				break;
				case Key_F4:
					
				break;
				case Key_F5:
					
				break;
				case Key_Disp:
					SetSystemStatus(SYS_STATUS_TEST);
					While_flag=0;
					Sort_set.Updata_flag=0;
				break;
				case Key_SETUP:
					While_flag=0;
					Sort_set.Updata_flag=0;
				SetSystemStatus(SYS_STATUS_SETUPTEST);
				break;
				case Key_FAST:
				break;
				case Key_LEFT:
				break;
				case Key_RIGHT:
				break;
				case Key_UP:
				break;
				case Key_DOWN:
				break;
				case Key_NUM1:
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='1';
						
						
							
						
						key_count++;
							
					}
				break;
				case Key_NUM2:
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='2';
						key_count++;
					}
				break;
				case Key_NUM3:
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='3';
						key_count++;
					}
				break;
				case Key_NUM4:
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='4';
						key_count++;
					}
					
				break;
				case Key_NUM5:
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='5';
						key_count++;
					}
				break;
				case Key_NUM6:
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='6';
						key_count++;
					}
				break;
				case Key_NUM7:
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='7';
						key_count++;
					}
				break;
				case Key_NUM8:
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='8';
						key_count++;
					}
				break;
				case Key_NUM9:
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='9';
						key_count++;
					}
				break;
				case Key_NUM0:
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='0';
						key_count++;
					}
				break;
				case Key_DOT:
					
					if(key_count<NUM_LENTH&&key_count>0)
					{
						if(dot_num==0)
						{
							if(key_count>0)
							{
								Disp_buff[key_count]='.';
								dot_num1=key_count;
								key_count++;
							
							
							}
							dot_num++;
						}
					}
//					else 
//					{
//						if(Disp_buff[key_count]==0)
//							Disp_buff[key_count]='-';
//						else if(Disp_buff[key_count]=='-')
//							Disp_buff[key_count]='+';
//						else
//							Disp_buff[key_count]='-';
//						key_count++;
//							
//					
//					
//					
//					
//					}
				break;
				case Key_BACK:
					if(key_count>0)
					{	key_count--;
						Disp_buff[key_count]=' ';
						if(dot_num1==key_count)
						{
							dot_num=0;
							dot_num1=0;
							
						}
					
					}
					else
					{
						if(Disp_buff[key_count]==0)
								Disp_buff[key_count]='-';
							else if(Disp_buff[key_count]=='-')
								Disp_buff[key_count]='+';
							else
								Disp_buff[key_count]='-';
							key_count++;
					}
				break;
				
				
				default:
				break;
					
			}
		
		keyvalue=KEY_NONE;
		}
		if(dispflag)
		{
			dispflag=0;
			LCD_DrawRect( Coordinates->xpos, Coordinates->ypos,Coordinates->xpos+Coordinates->lenth , Coordinates->ypos+16 , Red );
			Colour.Fword=White;
			Colour.black=Red;
			WriteString_16(Coordinates->xpos, Coordinates->ypos, Disp_buff,  0);
			//dispflag=0;
		}
	
	}
	for(i=key_count;i<NUM_LENTH;i++)
		Disp_buff[i]='0';
	for(i=0;i<NUM_LENTH;i++)
	{
		if(Disp_buff[0]>='0'&&(Disp_buff[0]<='9'))
		{
			if(Disp_buff[i]>='0'&&(Disp_buff[i]<='9'))
			{
			
				if(dot_num1>i)
				{
					Sort_set.Num*=10;
					Sort_set.Num+=Disp_buff[i]-'0';
				
				}
				else
				{
					Sort_set.Num*=10;
					Sort_set.Num+=Disp_buff[i]-'0';
				
				
				}
			}
			
			
			//Sort_set.Num+=(Disp_buff[key_count-1]-'0');
		
		
		}
	
	
	
	}
	Sort_set.Dot=dot_num1;
//	if(Disp_buff[0]>='0'&&(Disp_buff[0]<'9'))
//		{
//			if(Disp_buff[key_count-1]!='.')		
//			{
//				Sort_set.Num*=Num[key_count-dot_num-1];
//				Sort_set.Num+=(Disp_buff[key_count-1]-'0');//*Num[key_count-dot_num-1];
//				
//			}				
//			//*(Disp_buff[key_count-1]-'0'))+=Num[key_count-dot_num-1];
//			else
//			{
//			
//			}
//		
//		
//		}
//			
//		else 
//			;//(Disp_buff[key_count-1]-'0')*Sort_set.Num+=Num[key_count-dot_num-2];
	return Sort_set;

}
///////////////////////////////键盘输入、、Disp_ButtonUint
void Disp_Pre_Uint(u8 value)//%
{
    GUI_RECT Rect;
    u8 i;
    
    Disp_Fastbutton(1);
    GUI_SetColor(GUI_WHITE);
    GUI_SetFont(&GUI_FontGUI_FONTXBFFON16);
    for(i=0;i<5;i++)
    {
        Rect.x0=FASTBUTTON_START;
        Rect.y0=TOP_BAR_HIGH+i*FASTBUTTON_HIGH+6*i;
        Rect.x1=FASTBUTTON_END;
        Rect.y1=TOP_BAR_HIGH+(i+1)*FASTBUTTON_HIGH+6*i;
        GUI_DispStringInRect(Disp_ButtonUint[value][U9001_Save_sys.U9001_SYS.language][i],&Rect,GUI_TA_HCENTER|GUI_TA_VCENTER);
       
    }
        

    InputNum();

}
void disp_Inputnum(u8 *buff)
{
    GUI_SetTextMode(GUI_TM_NORMAL);
    GUI_SetBkColor(GUI_WHITE);
    GUI_SetColor(GUI_BLACK);
    GUI_SetFont(&GUI_FontGUI_FONTXBFFON16);
    GUI_DispStringAt((char*)buff,146,218);
    GUI_SetTextMode(GUI_TM_TRANS);
}
 u32 jisuandianzu(u16 bzdzn,u16 bzdzadn,u16 I_ad,u16 V_ad,u16 bzdy)
 {
      unsigned long a,b,jieguo;
      
     // unsigned int jieguo;
      a=bzdzn;
      b=bzdzadn;
      if(I_ad==0)
      I_ad=1;
      jieguo=((a*b/bzdy)*V_ad)/I_ad;//5002,2459
     return jieguo;//测试电阻
 }
void disp_TestMSG(u8 date)
{
    GUI_DispStringAt(TEST_TIMEDISP[date][U9001_Save_sys.U9001_SYS.language],40+140,272-30);

}
void disp_DelayMSG(u8 date)
{
    GUI_DispStringAt(DELAY_TIMEDISP[date][U9001_Save_sys.U9001_SYS.language],40+140,272-30);

}
void Disp_DelayTime(u8 item)
{
    GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
    GUI_SetBkColor(LCD_COLOR_TEST_BACK);
    GUI_SetColor(GUI_LIGHTYELLOW);
    disp_DelayMSG(item);
    switch(item)
    {
        case TEST_SOFTTIMER://延迟启动
            Hex_Format(Get_SorftTime(TEST_SOFTTIMER)/10,1,4,false);
            
            
            break;
        case PASS_SOFTTIMER://合格保持
            Hex_Format(Get_SorftTime(PASS_SOFTTIMER)/10,1,4,false);
            break;
        case DELAY_SOFTTIMER://步骤间隔
            Hex_Format(Get_SorftTime(DELAY_SOFTTIMER)/10,1,4,false);
            break;
		case DIS_SOFTTIMER://放电时间
            Hex_Format(Get_SorftTime(DIS_SOFTTIMER)/10,1,4,false);
            break;
//        case 3://放电时间
//            break;
        default:
            break;
    
    }
	GUI_DispStringAt(DispBuf,DISP_V_XPOS+44,DISP_V_YPOS+100+23);
//    GUI_DispStringAt(DispBuf,DISP_V_XPOS+44,DISP_V_YPOS+100);

}
void Disp_Comp(void)
{
    GUI_DispStringAt(DISP_COMP[Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].text_flag][U9001_Save_sys.U9001_SYS.language],260,60+20*(U9001_Save_sys.U9001_save.current_step-1));
}

void Disp_CompTest(void)
{
    GUI_DispStringAt(DISP_COMP[Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].text_flag][U9001_Save_sys.U9001_SYS.language],260,250);
}

