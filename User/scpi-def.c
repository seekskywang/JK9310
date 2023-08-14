/*-
 * BSD 2-Clause License
 *
 * Copyright (c) 2012-2018, Jan Breuer
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file   scpi-def.c
 * @date   Thu Nov 15 10:58:45 UTC 2012
 *
 * @brief  SCPI parser test
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scpi/scpi.h"
#include "scpi-def.h"
#include  "Globalvalue/globalvalue.h"
#include "use_disp.h"


uint8_t cmpres;
static const char CurPage[][10]={
	{"MEAS"},//测量显示
    {"LIST"},//列表显示
    {"MSET"},//测量设置
    {"MCON"},//测量配置
};

static const char IRUNIT[][3]={
	{"k"},//
    {"M"},//
    {"G"},//
};

static const char CURR_UNIT[][3]=
{
    {"uA"},
    {"mA"},
    {" A"},

};

static const char Respond[][5]={
	{"OK"}
};
char Resbuf[500];



void UartRes(void)
{
	u8 i;
	char stepbuf[50];
	for(i=0;i<U9001_Save_sys.U9001_save.all_step;i++)
	{
		if(U9001_Save_sys.U9001_save.U9001_Setup[i+1].parameter == AC ||
		   U9001_Save_sys.U9001_save.U9001_Setup[i+1].parameter == DC)//耐压模式
		{
			if(GetSystemStatus() == SYS_STATUS_FINISH && 
				U9001_Save_sys.U9001_save.current_step < U9001_Save_sys.U9001_save.all_step)
			{
				if(Test_Value.I_R==0xffff || GetSystemMessage() == MSG_SHORT)
				{
					if(i > U9001_Save_sys.U9001_save.current_step - 1)
					{
						sprintf(stepbuf,"STEP%02d,%s,%s,%.3fkV,UPPER,%.1fs;",
						i+1,
						TestPara[U9001_Save_sys.U9001_save.U9001_Setup[i+1].parameter],//模式
						SEND_COMP[Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].text_flag][1],//分选结果
						(double)Save_TestValue[i].Text_vot/1000,//电压
						(double)Save_TestValue[i].Text_time/10);//测试时间
					}else{
						sprintf(stepbuf,"STEP%02d,%s,%s,%.3fkV,UPPER,%.1fs;",
						i+1,
						TestPara[U9001_Save_sys.U9001_save.U9001_Setup[i+1].parameter],//模式
						SEND_COMP[Save_TestValue[i].text_flag][1],//分选结果
						(double)Save_TestValue[i].Text_vot/1000,//电压
						(double)Save_TestValue[i].Text_time/10);//测试时间
					}
				}else{
					if(i > U9001_Save_sys.U9001_save.current_step - 1)
					{
						sprintf(stepbuf,"STEP%02d,%s,%s,%.3fkV,%.3f%s,%.1fs;",
						i+1,
						TestPara[U9001_Save_sys.U9001_save.U9001_Setup[i+1].parameter],//模式
						SEND_COMP[Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].text_flag][1],//分选结果
						(double)Save_TestValue[i].Text_vot/1000,//电压
						(double)Save_TestValue[i].Text_value/1000,//电流
						CURR_UNIT[Save_TestValue[i].text_unit],//电流单位	
						(double)Save_TestValue[i].Text_time/10);//测试时间
					}else{
						sprintf(stepbuf,"STEP%02d,%s,%s,%.3fkV,%.3f%s,%.1fs;",
						i+1,
						TestPara[U9001_Save_sys.U9001_save.U9001_Setup[i+1].parameter],//模式
						SEND_COMP[Save_TestValue[i].text_flag][1],//分选结果
						(double)Save_TestValue[i].Text_vot/1000,//电压
						(double)Save_TestValue[i].Text_value/1000,//电流
						CURR_UNIT[Save_TestValue[i].text_unit],//电流单位	
						(double)Save_TestValue[i].Text_time/10);//测试时间
					}
				}
			}else{
				if(Test_Value.I_R==0xffff || GetSystemMessage() == MSG_SHORT)
				{
					sprintf(stepbuf,"STEP%02d,%s,%s,%.3fkV,UPPER,%.1fs;",
					i+1,
					TestPara[U9001_Save_sys.U9001_save.U9001_Setup[i+1].parameter],//模式
					SEND_COMP[Save_TestValue[i].text_flag][1],//分选结果
					(double)Save_TestValue[i].Text_vot/1000,//电压
					(double)Save_TestValue[i].Text_time/10);//测试时间
				}else{
					sprintf(stepbuf,"STEP%02d,%s,%s,%.3fkV,%.3f%s,%.1fs;",
					i+1,
					TestPara[U9001_Save_sys.U9001_save.U9001_Setup[i+1].parameter],//模式
					SEND_COMP[Save_TestValue[i].text_flag][1],//分选结果
					(double)Save_TestValue[i].Text_vot/1000,//电压
					(double)Save_TestValue[i].Text_value/1000,//电流
					CURR_UNIT[Save_TestValue[i].text_unit],//电流单位	
					(double)Save_TestValue[i].Text_time/10);//测试时间
				}
			}
		}else if(U9001_Save_sys.U9001_save.U9001_Setup[i+1].parameter == OS){//OS模式
			sprintf(stepbuf,"STEP%02d,%s,%s,%.3fkV,%.1fnF,%.1fs;",
			i+1,
			TestPara[U9001_Save_sys.U9001_save.U9001_Setup[i+1].parameter],//模式
			SEND_COMP[Save_TestValue[i].text_flag][1],//分选结果
			(double)Save_TestValue[i].Text_vot/1000,//电压
			(double)Save_TestValue[i].Text_value/10,//电容
			(double)Save_TestValue[i].Text_time/10);//测试时间
		}else if(U9001_Save_sys.U9001_save.U9001_Setup[i+1].parameter == IR){//绝缘模式
			if(GetSystemStatus() == SYS_STATUS_FINISH && 
				U9001_Save_sys.U9001_save.current_step < U9001_Save_sys.U9001_save.all_step)
			{
				if(Test_Value.I_R==0xffff*10)
				{
					if(i > U9001_Save_sys.U9001_save.current_step - 1)
					{
						sprintf(stepbuf,"STEP%02d,%s,%s,%.3fkV,UPPER,%.1fs;",
						i+1,
						TestPara[U9001_Save_sys.U9001_save.U9001_Setup[i+1].parameter],//模式
						SEND_COMP[Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].text_flag][1],//分选结果
						(double)Save_TestValue[i].Text_vot/1000,//电压
						(double)Save_TestValue[i].Text_time/10);//测试时间
					}else{
						sprintf(stepbuf,"STEP%02d,%s,%s,%.3fkV,UPPER,%.1fs;",
						i+1,
						TestPara[U9001_Save_sys.U9001_save.U9001_Setup[i+1].parameter],//模式
						SEND_COMP[Save_TestValue[i].text_flag][1],//分选结果
						(double)Save_TestValue[i].Text_vot/1000,//电压
						(double)Save_TestValue[i].Text_time/10);//测试时间
					}
				}else{
					if(i > U9001_Save_sys.U9001_save.current_step - 1)
					{
						sprintf(stepbuf,"STEP%02d,%s,%s,%.3fkV,%.3f%sΩ,%.1fs;",
						i+1,
						TestPara[U9001_Save_sys.U9001_save.U9001_Setup[i+1].parameter],//模式
						SEND_COMP[Save_TestValue[U9001_Save_sys.U9001_save.current_step-1].text_flag][1],//分选结果
						(double)Save_TestValue[i].Text_vot/1000,//电压
						(double)Save_TestValue[i].Text_value/1000,//内阻
						IRUNIT[Save_TestValue[i].text_unit],//内阻单位
						(double)Save_TestValue[i].Text_time/10);//测试时间
					}else{
						sprintf(stepbuf,"STEP%02d,%s,%s,%.3fkV,%.3f%sΩ,%.1fs;",
						i+1,
						TestPara[U9001_Save_sys.U9001_save.U9001_Setup[i+1].parameter],//模式
						SEND_COMP[Save_TestValue[i].text_flag][1],//分选结果
						(double)Save_TestValue[i].Text_vot/1000,//电压
						(double)Save_TestValue[i].Text_value/1000,//内阻
						IRUNIT[Save_TestValue[i].text_unit],//内阻单位
						(double)Save_TestValue[i].Text_time/10);//测试时间
					}
				}
			}else{
				if(Test_Value.I_R==0xffff*10)
				{
					sprintf(stepbuf,"STEP%02d,%s,%s,%.3fkV,UPPER,%.1fs;",
					i+1,
					TestPara[U9001_Save_sys.U9001_save.U9001_Setup[i+1].parameter],//模式
					SEND_COMP[Save_TestValue[i].text_flag][1],//分选结果
					(double)Save_TestValue[i].Text_vot/1000,//电压
					(double)Save_TestValue[i].Text_time/10);//测试时间
				}else{
					sprintf(stepbuf,"STEP%02d,%s,%s,%.3fkV,%.3f%sΩ,%.1fs;",
					i+1,
					TestPara[U9001_Save_sys.U9001_save.U9001_Setup[i+1].parameter],//模式
					SEND_COMP[Save_TestValue[i].text_flag][1],//分选结果
					(double)Save_TestValue[i].Text_vot/1000,//电压
					(double)Save_TestValue[i].Text_value/1000,//内阻
					IRUNIT[Save_TestValue[i].text_unit],//内阻单位
					(double)Save_TestValue[i].Text_time/10);//测试时间
				}
			}
		}
		
		strcat(Resbuf,stepbuf);
	}
}

static scpi_result_t DMM_MeasureVoltageDcQ(scpi_t * context) {
    scpi_number_t param1, param2;
    char bf[15];
//    fprintf(stderr, "meas:volt:dc\r\n"); /* debug command name */

    /* read first parameter if present */
    if (!SCPI_ParamNumber(context, scpi_special_numbers_def, &param1, FALSE)) {
        /* do something, if parameter not present */
    }

    /* read second paraeter if present */
    if (!SCPI_ParamNumber(context, scpi_special_numbers_def, &param2, FALSE)) {
        /* do something, if parameter not present */
    }


    SCPI_NumberToStr(context, scpi_special_numbers_def, &param1, bf, 15);
//    fprintf(stderr, "\tP1=%s\r\n", bf);


    SCPI_NumberToStr(context, scpi_special_numbers_def, &param2, bf, 15);
//    fprintf(stderr, "\tP2=%s\r\n", bf);

	
    SCPI_ResultDouble(context, 15);

    return SCPI_RES_OK;
}

//查询当前页面
static scpi_result_t PageQuery(scpi_t * context) {
	
    SCPI_ResultCharacters(context, CurPage[pageflag],4);
    return SCPI_RES_OK;
}

//设置页面
static scpi_result_t PageSet(scpi_t * context) {
     const char * param;
    size_t param_len;
	uint8_t i;

    if (!SCPI_ParamCharacters(context, &param, &param_len, true)) {
		return SCPI_RES_ERR;
    }
	
	
	for(i=0;i<4;i++)
	{
		if (strncasecmp(param, CurPage[i], param_len) == 0)
		{
			switch(i)
			{
				case 0:
					DISP_FLAG = TRUE;
					U9001_Save_sys.U9001_save.disp = 0; 
					SetSystemStatus(SYS_STATUS_IDEM);
				break;
				case  1:
					DISP_FLAG = TRUE;
					U9001_Save_sys.U9001_save.disp = 1;
					SetSystemStatus(SYS_STATUS_IDEM);		
				break;
				case 2:
					SetSystemStatus(SYS_STATUS_SETUPTEST);
				break;
				case 3:
					SetSystemStatus(SYS_STATUS_CLEAR);
				break;
				default:break;
			}
		}

	}
//    fprintf(stderr, "TEXT: ***%s***\r\n", buffer);

    return SCPI_RES_OK;
}
//查询AC电压1
static scpi_result_t VoltageAcQ1(scpi_t * context) {
//    fprintf(stderr, "\tP2=%s\r\n", bf);

    SCPI_ResultDouble(context, U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out);
	
    return SCPI_RES_OK;
}

//设置AC电压1
static scpi_result_t VoltageAcS1(scpi_t * context) {
    uint32_t param;
	
    if (!SCPI_ParamUInt32(context,&param, FALSE)) {
		return SCPI_RES_ERR;
    }
	U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].V_out = param;
	DISP_FLAG = TRUE;
	f_disp = TRUE;
	SetDate_Comp();
	Savetoeeprom();
	SCPI_ResultCharacters(context, Respond[0],2);
    return SCPI_RES_OK;
}

//查询AC电流上限1
static scpi_result_t CUpperAcQ1(scpi_t * context) {
//    fprintf(stderr, "\tP2=%s\r\n", bf);
	if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter == IR)
	{
		SCPI_ResultFloat(context, (float)U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper/10);
	}else{
		SCPI_ResultFloat(context, (float)U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper/1000);
	}

    return SCPI_RES_OK;
}

//设置AC电流上限1
static scpi_result_t CUpperAcS1(scpi_t * context) {
    float param;
	
    if (!SCPI_ParamFloat(context,&param, FALSE)) {
		return SCPI_RES_ERR;
    }
	if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter == IR)
	{
		U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper = param*10;
	}else {
		U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper = param*1000;
	}
	DISP_FLAG = TRUE;
	f_disp = TRUE;
	SetDate_Comp();
	Savetoeeprom();
	SCPI_ResultCharacters(context, Respond[0],2);
    return SCPI_RES_OK;
}

//查询AC电流下限1
static scpi_result_t CLOWAcQ1(scpi_t * context) {
//    fprintf(stderr, "\tP2=%s\r\n", bf);
	if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter == IR)
	{
		SCPI_ResultFloat(context, (float)U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower/10);
	}else{
		SCPI_ResultFloat(context, (float)U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower/1000);
	}

    return SCPI_RES_OK;
}

//设置AC电流下限1
static scpi_result_t CLOWAcS1(scpi_t * context) {
    float param;
	
    if (!SCPI_ParamFloat(context,&param, FALSE)) {
		return SCPI_RES_ERR;
    }
	if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter == IR)
	{
		U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower		= param*10;
	}else {
		U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower = param*1000;
	}
	DISP_FLAG = TRUE;
	f_disp = TRUE;
	SetDate_Comp();
	Savetoeeprom();
	SCPI_ResultCharacters(context, Respond[0],2);
    return SCPI_RES_OK;
}

//查询测试时间1
static scpi_result_t TTIMQ1(scpi_t * context) {
//    fprintf(stderr, "\tP2=%s\r\n", bf);

    SCPI_ResultFloat(context, (float)U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time/10);

    return SCPI_RES_OK;
}

//设置测试时间1
static scpi_result_t TTIMS1(scpi_t * context) {
    float param;
	
    if (!SCPI_ParamFloat(context,&param, FALSE)) {
		return SCPI_RES_ERR;
    }
		DISP_FLAG = TRUE;
	f_disp = TRUE;
	U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].time = param;
	SetDate_Comp();
	Savetoeeprom();
	SCPI_ResultCharacters(context, Respond[0],2);
    return SCPI_RES_OK;
}

//查询上升时间1
static scpi_result_t RTIMQ1(scpi_t * context) {
//    fprintf(stderr, "\tP2=%s\r\n", bf);

    SCPI_ResultFloat(context, (float)U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].rise_time/10);

    return SCPI_RES_OK;
}

//设置上升时间1
static scpi_result_t RTIMS1(scpi_t * context) {
    float param;
	
    if (!SCPI_ParamFloat(context,&param, FALSE)) {
		return SCPI_RES_ERR;
    }
		DISP_FLAG = TRUE;
	f_disp = TRUE;
	U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].rise_time = param;
	SetDate_Comp();
	Savetoeeprom();
	SCPI_ResultCharacters(context, Respond[0],2);
    return SCPI_RES_OK;
}

//查询下降时间1
static scpi_result_t FTIMQ1(scpi_t * context) {
//    fprintf(stderr, "\tP2=%s\r\n", bf);

    SCPI_ResultFloat(context, (float)U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].drop_time/10);

    return SCPI_RES_OK;
}

//设置下降时间1
static scpi_result_t FTIMS1(scpi_t * context) {
    float param;
	
    if (!SCPI_ParamFloat(context,&param, FALSE)) {
		return SCPI_RES_ERR;
    }
		DISP_FLAG = TRUE;
	f_disp = TRUE;
	U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].drop_time = param;
	SetDate_Comp();
	Savetoeeprom();
	SCPI_ResultCharacters(context, Respond[0],2);
    return SCPI_RES_OK;
}

//查询ARC
static scpi_result_t ARCQ1(scpi_t * context) {
//    fprintf(stderr, "\tP2=%s\r\n", bf);

    SCPI_ResultFloat(context, (float)U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc/10);

    return SCPI_RES_OK;
}

//设置ARC
static scpi_result_t ARCS1(scpi_t * context) {
    float param;
	
    if (!SCPI_ParamFloat(context,&param, FALSE)) {
		return SCPI_RES_ERR;
    }
		DISP_FLAG = TRUE;
	f_disp = TRUE;
	U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].range_arc = param;
	SetDate_Comp();
	Savetoeeprom();
	SCPI_ResultCharacters(context, Respond[0],2);
    return SCPI_RES_OK;
}

//查询频率
static scpi_result_t FREQQ1(scpi_t * context) {
//    fprintf(stderr, "\tP2=%s\r\n", bf);
	if(U9001_Save_sys.U9001_Testconfg.hz == 0)
	{
		 SCPI_ResultFloat(context, 50);
	}else if(U9001_Save_sys.U9001_Testconfg.hz == 1){
		 SCPI_ResultFloat(context, 60);
	}

    return SCPI_RES_OK;
}

//设置频率
static scpi_result_t FREQS1(scpi_t * context) {
    float param;
	
    if (!SCPI_ParamFloat(context,&param, FALSE)) {
		return SCPI_RES_ERR;
    }
	if(param == 50)
	{
		U9001_Save_sys.U9001_Testconfg.hz = 0;
	}else if(param == 60){
		U9001_Save_sys.U9001_Testconfg.hz = 1;
	}
	DISP_FLAG = TRUE;
	f_disp = TRUE;
	Savetoeeprom();
	SCPI_ResultCharacters(context, Respond[0],2);
    return SCPI_RES_OK;
}

//查询开路比率
static scpi_result_t OPENQ1(scpi_t * context) {
//    fprintf(stderr, "\tP2=%s\r\n", bf);
	SCPI_ResultFloat(context, (float)U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower);

    return SCPI_RES_OK;
}

//设置开路比率
static scpi_result_t OPENS1(scpi_t * context) {
    float param;
	
    if (!SCPI_ParamFloat(context,&param, FALSE)) {
		return SCPI_RES_ERR;
    }
		DISP_FLAG = TRUE;
	f_disp = TRUE;
	U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].lower = param;
	SetDate_Comp();
	Savetoeeprom();
	SCPI_ResultCharacters(context, Respond[0],2);
    return SCPI_RES_OK;
}

//查询短路比率
static scpi_result_t SHORTQ1(scpi_t * context) {
//    fprintf(stderr, "\tP2=%s\r\n", bf);
	SCPI_ResultFloat(context, (float)U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper);

    return SCPI_RES_OK;
}

//设置短路比率
static scpi_result_t SHORTS1(scpi_t * context) {
    float param;
	
    if (!SCPI_ParamFloat(context,&param, FALSE)) {
		return SCPI_RES_ERR;
    }
		DISP_FLAG = TRUE;
	f_disp = TRUE;
	U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper = param;
//	SetDate_Comp();
	if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper > 500)
		U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].Upper = 500;
	Savetoeeprom();
	SCPI_ResultCharacters(context, Respond[0],2);
    return SCPI_RES_OK;
}
//开始测试
static scpi_result_t StartT(scpi_t * context) {
    SetSystemStatus(SYS_STATUS_START);
    SetSystemMessage(MSG_RAMP);
    Uart0_Send(0xa1);
		PLC_OutProg();//开PLC启动
		SCPI_ResultCharacters(context, Respond[0],2);
    return SCPI_RES_OK;
}

//停止测试
static scpi_result_t StopT(scpi_t * context) {
	switch(GetSystemStatus())
	{
		case SYS_STATUS_FINISH:
			Beep_Off();
			SetSystemStatus(SYS_STATUS_IDEM);
			SetSystemMessage(MSG_IDLE);
            V_DA_out(0);
            Sing_out_C(0);
            Short_out(0);
            FRB_out(0);
            Uart0_Send(0xa0);
		break;
		case SYS_STATUS_IDEM:
			Beep_Off();
			V_DA_out(0);
            Sing_out_C(0);
            Short_out(0);
            FRB_out(0);
            Uart0_Send(0xa0);
		break;
		case SYS_STATUS_TEST_PAUSE:
			Beep_Off();
			V_DA_out(0);
            Sing_out_C(0);
            Short_out(0);
            FRB_out(0);
            SetSystemMessage(MSG_PAUSE);//系统信息-暂停测试
            SetSystemStatus(SYS_STATUS_ABORT);//系统状态-暂停测试
            Uart0_Send(0xa0);
		break;
		case SYS_STATUS_ABORT:
			Beep_Off();
			SetSystemMessage(MSG_IDLE);//系统信息-暂停测试
			SetSystemStatus(SYS_STATUS_IDEM);//系统状态-暂停测试
		break;
		case SYS_STATUS_TEST:
			Beep_Off();
			V_DA_out(0);
			Sing_out_C(0);
			Short_out(0);
			FRB_out(0);
			SetSystemMessage(MSG_PAUSE);//系统信息-暂停测试
			SetSystemStatus(SYS_STATUS_ABORT);//系统状态-暂停测试
			Uart0_Send(0xa0);
			SetSystemMessage(MSG_IDLE);//系统信息-暂停测试
				SetSystemStatus(SYS_STATUS_IDEM);//系统状态-暂停测试
		
		break;
		case SYS_STATUS_START:   
			Beep_Off();
            SetSystemMessage(MSG_PAUSE);//系统信息-暂停测试
            SetSystemStatus(SYS_STATUS_ABORT);//系统状态-暂停测试
            Uart0_Send(0xa0);
		break;
	}
	
//    V_DA_out(0);
//    Sing_out_C(0);
//	Short_out(0);
//	FRB_out(0);
//	Uart0_Send(0xa0);
	SCPI_ResultCharacters(context, Respond[0],2);
    return SCPI_RES_OK;
}

//设置步骤
static scpi_result_t SetStep(scpi_t * context) {
    
	uint32_t param;
	
    if (!SCPI_ParamUInt32(context,&param, FALSE)) {
		return SCPI_RES_ERR;
    }
		DISP_FLAG = TRUE;
	f_disp = TRUE;
	U9001_Save_sys.U9001_save.current_step = param;
	Savetoeeprom();
	 SCPI_ResultCharacters(context, Respond[0],2);
    return SCPI_RES_OK;
}

//设置总步骤数
static scpi_result_t SetTSteps(scpi_t * context) {
    
	uint32_t param;
	
    if (!SCPI_ParamUInt32(context,&param, FALSE)) {
		return SCPI_RES_ERR;
    }
		DISP_FLAG = TRUE;
	f_disp = TRUE;
	U9001_Save_sys.U9001_save.all_step = param;
	Savetoeeprom();
	 SCPI_ResultCharacters(context, Respond[0],2);
    return SCPI_RES_OK;
}

//AC
static scpi_result_t ACMode(scpi_t * context) {
    
	U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter = AC;
	Savetoeeprom();
	DISP_FLAG = TRUE;
	f_disp = TRUE;
	 SCPI_ResultCharacters(context, Respond[0],2);
    return SCPI_RES_OK;
}

//DC
static scpi_result_t DCMode(scpi_t * context) {
    
	U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter = DC;
	Savetoeeprom();
	DISP_FLAG = TRUE;
	f_disp = TRUE;
	 SCPI_ResultCharacters(context, Respond[0],2);
    return SCPI_RES_OK;
}

//IR
static scpi_result_t IRMode(scpi_t * context) {
    
	U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter = IR;
	Savetoeeprom();
	DISP_FLAG = TRUE;
	f_disp = TRUE;
	 SCPI_ResultCharacters(context, Respond[0],2);
    return SCPI_RES_OK;
}

//OS
static scpi_result_t OSMode(scpi_t * context) {
    
	U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter = OS;
	Savetoeeprom();
	DISP_FLAG = TRUE;
	f_disp = TRUE;
	 SCPI_ResultCharacters(context, Respond[0],2);
    return SCPI_RES_OK;
}

//读取测试结果
static scpi_result_t ReadRes(scpi_t * context) {
    
	
//	Save_TestValue[U9001_Save_sys.U9001_save.current_step-1]
	UartRes();
	SCPI_ResultCharacters(context, Resbuf,strlen(Resbuf));
	memset(Resbuf,0,sizeof(Resbuf));
    return SCPI_RES_OK;
}


static scpi_result_t VoltageDcQ1(scpi_t * context) {
//    fprintf(stderr, "\tP2=%s\r\n", bf);

    SCPI_ResultDouble(context, U9001_Save_sys.U9001_save.U9001_Setup[1].V_out);

    return SCPI_RES_OK;
}

static scpi_result_t DMM_MeasureVoltageAcQ(scpi_t * context) {
    scpi_number_t param1, param2;
    char bf[15];
//    fprintf(stderr, "meas:volt:ac\r\n"); /* debug command name */

    /* read first parameter if present */
    if (!SCPI_ParamNumber(context, scpi_special_numbers_def, &param1, FALSE)) {
        /* do something, if parameter not present */
    }

    /* read second paraeter if present */
    if (!SCPI_ParamNumber(context, scpi_special_numbers_def, &param2, FALSE)) {
        /* do something, if parameter not present */
    }


    SCPI_NumberToStr(context, scpi_special_numbers_def, &param1, bf, 15);
//    fprintf(stderr, "\tP1=%s\r\n", bf);


    SCPI_NumberToStr(context, scpi_special_numbers_def, &param2, bf, 15);
//    fprintf(stderr, "\tP2=%s\r\n", bf);

    SCPI_ResultDouble(context, 0);

    return SCPI_RES_OK;
}

static scpi_result_t DMM_ConfigureVoltageDc(scpi_t * context) {
    double param1, param2;
//    fprintf(stderr, "conf:volt:dc\r\n"); /* debug command name */

    /* read first parameter if present */
    if (!SCPI_ParamDouble(context, &param1, TRUE)) {
        return SCPI_RES_ERR;
    }

    /* read second paraeter if present */
    if (!SCPI_ParamDouble(context, &param2, FALSE)) {
        /* do something, if parameter not present */
    }

//    fprintf(stderr, "\tP1=%lf\r\n", param1);
//    fprintf(stderr, "\tP2=%lf\r\n", param2);

    return SCPI_RES_OK;
}

static scpi_result_t TEST_Bool(scpi_t * context) {
    scpi_bool_t param1;
//    fprintf(stderr, "TEST:BOOL\r\n"); /* debug command name */

    /* read first parameter if present */
    if (!SCPI_ParamBool(context, &param1, TRUE)) {
        return SCPI_RES_ERR;
    }

//    fprintf(stderr, "\tP1=%d\r\n", param1);

    return SCPI_RES_OK;
}

scpi_choice_def_t trigger_source[] = {
    {"BUS", 5},
    {"IMMediate", 6},
    {"EXTernal", 7},
    SCPI_CHOICE_LIST_END /* termination of option list */
};

static scpi_result_t TEST_ChoiceQ(scpi_t * context) {

    int32_t param;
    const char * name;

    if (!SCPI_ParamChoice(context, trigger_source, &param, TRUE)) {
        return SCPI_RES_ERR;
    }

    SCPI_ChoiceToName(trigger_source, param, &name);
//    fprintf(stderr, "\tP1=%s (%ld)\r\n", name, (long int) param);

    SCPI_ResultInt32(context, param);

    return SCPI_RES_OK;
}

static scpi_result_t TEST_Numbers(scpi_t * context) {
    int32_t numbers[2];

    SCPI_CommandNumbers(context, numbers, 2, 1);

//    fprintf(stderr, "TEST numbers %d %d\r\n", numbers[0], numbers[1]);

    return SCPI_RES_OK;
}

static scpi_result_t TEST_Text(scpi_t * context) {
    char buffer[100];
    size_t copy_len;

    if (!SCPI_ParamCopyText(context, buffer, sizeof (buffer), &copy_len, FALSE)) {
        buffer[0] = '\0';
    }

//    fprintf(stderr, "TEXT: ***%s***\r\n", buffer);

    return SCPI_RES_OK;
}

static scpi_result_t TEST_ArbQ(scpi_t * context) {
    const char * data;
    size_t len;

    if (SCPI_ParamArbitraryBlock(context, &data, &len, FALSE)) {
        SCPI_ResultArbitraryBlock(context, data, len);
    }

    return SCPI_RES_OK;
}

struct _scpi_channel_value_t {
    int32_t row;
    int32_t col;
};
typedef struct _scpi_channel_value_t scpi_channel_value_t;

/**
 * @brief
 * parses lists
 * channel numbers > 0.
 * no checks yet.
 * valid: (@1), (@3!1:1!3), ...
 * (@1!1:3!2) would be 1!1, 1!2, 2!1, 2!2, 3!1, 3!2.
 * (@3!1:1!3) would be 3!1, 3!2, 3!3, 2!1, 2!2, 2!3, ... 1!3.
 *
 * @param channel_list channel list, compare to SCPI99 Vol 1 Ch. 8.3.2
 */
static scpi_result_t TEST_Chanlst(scpi_t *context) {
    scpi_parameter_t channel_list_param;
#define MAXROW 2    /* maximum number of rows */
#define MAXCOL 6    /* maximum number of columns */
#define MAXDIM 2    /* maximum number of dimensions */
    scpi_channel_value_t array[MAXROW * MAXCOL]; /* array which holds values in order (2D) */
    size_t chanlst_idx; /* index for channel list */
    size_t arr_idx = 0; /* index for array */
    size_t n, m = 1; /* counters for row (n) and columns (m) */

    /* get channel list */
    if (SCPI_Parameter(context, &channel_list_param, TRUE)) {
        scpi_expr_result_t res;
        scpi_bool_t is_range;
        int32_t values_from[MAXDIM];
        int32_t values_to[MAXDIM];
        size_t dimensions;

        bool for_stop_row = FALSE; /* true if iteration for rows has to stop */
        bool for_stop_col = FALSE; /* true if iteration for columns has to stop */
        int32_t dir_row = 1; /* direction of counter for rows, +/-1 */
        int32_t dir_col = 1; /* direction of counter for columns, +/-1 */

        /* the next statement is valid usage and it gets only real number of dimensions for the first item (index 0) */
        if (!SCPI_ExprChannelListEntry(context, &channel_list_param, 0, &is_range, NULL, NULL, 0, &dimensions)) {
            chanlst_idx = 0; /* call first index */
            arr_idx = 0; /* set arr_idx to 0 */
            do { /* if valid, iterate over channel_list_param index while res == valid (do-while cause we have to do it once) */
                res = SCPI_ExprChannelListEntry(context, &channel_list_param, chanlst_idx, &is_range, values_from, values_to, 4, &dimensions);
                if (is_range == FALSE) { /* still can have multiple dimensions */
                    if (dimensions == 1) {
                        /* here we have our values
                         * row == values_from[0]
                         * col == 0 (fixed number)
                         * call a function or something */
                        array[arr_idx].row = values_from[0];
                        array[arr_idx].col = 0;
                    } else if (dimensions == 2) {
                        /* here we have our values
                         * row == values_fom[0]
                         * col == values_from[1]
                         * call a function or something */
                        array[arr_idx].row = values_from[0];
                        array[arr_idx].col = values_from[1];
                    } else {
                        return SCPI_RES_ERR;
                    }
                    arr_idx++; /* inkrement array where we want to save our values to, not neccessary otherwise */
                    if (arr_idx >= MAXROW * MAXCOL) {
                        return SCPI_RES_ERR;
                    }
                } else if (is_range == TRUE) {
                    if (values_from[0] > values_to[0]) {
                        dir_row = -1; /* we have to decrement from values_from */
                    } else { /* if (values_from[0] < values_to[0]) */
                        dir_row = +1; /* default, we increment from values_from */
                    }

                    /* iterating over rows, do it once -> set for_stop_row = false
                     * needed if there is channel list index isn't at end yet */
                    for_stop_row = FALSE;
                    for (n = values_from[0]; for_stop_row == FALSE; n += dir_row) {
                        /* usual case for ranges, 2 dimensions */
                        if (dimensions == 2) {
                            if (values_from[1] > values_to[1]) {
                                dir_col = -1;
                            } else if (values_from[1] < values_to[1]) {
                                dir_col = +1;
                            }
                            /* iterating over columns, do it at least once -> set for_stop_col = false
                             * needed if there is channel list index isn't at end yet */
                            for_stop_col = FALSE;
                            for (m = values_from[1]; for_stop_col == FALSE; m += dir_col) {
                                /* here we have our values
                                 * row == n
                                 * col == m
                                 * call a function or something */
                                array[arr_idx].row = n;
                                array[arr_idx].col = m;
                                arr_idx++;
                                if (arr_idx >= MAXROW * MAXCOL) {
                                    return SCPI_RES_ERR;
                                }
                                if (m == (size_t)values_to[1]) {
                                    /* endpoint reached, stop column for-loop */
                                    for_stop_col = TRUE;
                                }
                            }
                            /* special case for range, example: (@2!1) */
                        } else if (dimensions == 1) {
                            /* here we have values
                             * row == n
                             * col == 0 (fixed number)
                             * call function or sth. */
                            array[arr_idx].row = n;
                            array[arr_idx].col = 0;
                            arr_idx++;
                            if (arr_idx >= MAXROW * MAXCOL) {
                                return SCPI_RES_ERR;
                            }
                        }
                        if (n == (size_t)values_to[0]) {
                            /* endpoint reached, stop row for-loop */
                            for_stop_row = TRUE;
                        }
                    }


                } else {
                    return SCPI_RES_ERR;
                }
                /* increase index */
                chanlst_idx++;
            } while (SCPI_EXPR_OK == SCPI_ExprChannelListEntry(context, &channel_list_param, chanlst_idx, &is_range, values_from, values_to, 4, &dimensions));
            /* while checks, whether incremented index is valid */
        }
        /* do something at the end if needed */
        /* array[arr_idx].row = 0; */
        /* array[arr_idx].col = 0; */
    }

    {
        size_t i;
//        fprintf(stderr, "TEST_Chanlst: ");
        for (i = 0; i< arr_idx; i++) {
//            fprintf(stderr, "%d!%d, ", array[i].row, array[i].col);
        }
//        fprintf(stderr, "\r\n");
    }
    return SCPI_RES_OK;
}

/**
 * Reimplement IEEE488.2 *TST?
 *
 * Result should be 0 if everything is ok
 * Result should be 1 if something goes wrong
 *
 * Return SCPI_RES_OK
 */
static scpi_result_t My_CoreTstQ(scpi_t * context) {

    SCPI_ResultInt32(context, 8);

    return SCPI_RES_OK;
}

const scpi_command_t scpi_commands[] = {
    /* IEEE Mandated Commands (SCPI std V1999.0 4.1.1) */
    { .pattern = "*CLS", .callback = SCPI_CoreCls,},
    { .pattern = "*ESE", .callback = SCPI_CoreEse,},
    { .pattern = "*ESE?", .callback = SCPI_CoreEseQ,},
    { .pattern = "*ESR?", .callback = SCPI_CoreEsrQ,},
    { .pattern = "*IDN?", .callback = SCPI_CoreIdnQ,},
    { .pattern = "*OPC", .callback = SCPI_CoreOpc,},
    { .pattern = "*OPC?", .callback = SCPI_CoreOpcQ,},
    { .pattern = "*RST", .callback = SCPI_CoreRst,},
    { .pattern = "*SRE", .callback = SCPI_CoreSre,},
    { .pattern = "*SRE?", .callback = SCPI_CoreSreQ,},
    { .pattern = "*STB?", .callback = SCPI_CoreStbQ,},
    { .pattern = "*TST?", .callback = My_CoreTstQ,},
    { .pattern = "*WAI", .callback = SCPI_CoreWai,},

    /* Required SCPI commands (SCPI std V1999.0 4.2.1) */
    {.pattern = "SYSTem:ERRor[:NEXT]?", .callback = SCPI_SystemErrorNextQ,},
    {.pattern = "SYSTem:ERRor:COUNt?", .callback = SCPI_SystemErrorCountQ,},
    {.pattern = "SYSTem:VERSion?", .callback = SCPI_SystemVersionQ,},

    /* {.pattern = "STATus:OPERation?", .callback = scpi_stub_callback,}, */
    /* {.pattern = "STATus:OPERation:EVENt?", .callback = scpi_stub_callback,}, */
    /* {.pattern = "STATus:OPERation:CONDition?", .callback = scpi_stub_callback,}, */
    /* {.pattern = "STATus:OPERation:ENABle", .callback = scpi_stub_callback,}, */
    /* {.pattern = "STATus:OPERation:ENABle?", .callback = scpi_stub_callback,}, */

    {.pattern = "STATus:QUEStionable[:EVENt]?", .callback = SCPI_StatusQuestionableEventQ,},
    /* {.pattern = "STATus:QUEStionable:CONDition?", .callback = scpi_stub_callback,}, */
    {.pattern = "STATus:QUEStionable:ENABle", .callback = SCPI_StatusQuestionableEnable,},
    {.pattern = "STATus:QUEStionable:ENABle?", .callback = SCPI_StatusQuestionableEnableQ,},

    {.pattern = "STATus:PRESet", .callback = SCPI_StatusPreset,},

    /* DMM */
    {.pattern = "MEASure:VOLTage:DC?", .callback = DMM_MeasureVoltageDcQ,},
    {.pattern = "CONFigure:VOLTage:DC", .callback = DMM_ConfigureVoltageDc,},
    {.pattern = "MEASure:VOLTage:DC:RATio?", .callback = SCPI_StubQ,},
    {.pattern = "MEASure:VOLTage:AC?", .callback = DMM_MeasureVoltageAcQ,},
    {.pattern = "MEASure:CURRent:DC?", .callback = SCPI_StubQ,},
    {.pattern = "MEASure:CURRent:AC?", .callback = SCPI_StubQ,},
    {.pattern = "MEASure:RESistance?", .callback = SCPI_StubQ,},
    {.pattern = "MEASure:FRESistance?", .callback = SCPI_StubQ,},
    {.pattern = "MEASure:FREQuency?", .callback = SCPI_StubQ,},
    {.pattern = "MEASure:PERiod?", .callback = SCPI_StubQ,},

    {.pattern = "SYSTem:COMMunication:TCPIP:CONTROL?", .callback = SCPI_SystemCommTcpipControlQ,},

    {.pattern = "TEST:BOOL", .callback = TEST_Bool,},
    {.pattern = "TEST:CHOice?", .callback = TEST_ChoiceQ,},
    {.pattern = "TEST#:NUMbers#", .callback = TEST_Numbers,},
    {.pattern = "TEST:TEXT", .callback = TEST_Text,},
    {.pattern = "TEST:ARBitrary?", .callback = TEST_ArbQ,},
    {.pattern = "TEST:CHANnellist", .callback = TEST_Chanlst,},
	/*TEST*/
	{.pattern = "DISPlay:PAGE?", .callback = PageQuery,},//查询当前页面
	{.pattern = "DISPlay:PAGE", .callback = PageSet,},//设置页面
	
	{.pattern = "VOLT?", .callback = VoltageAcQ1,},//查询步骤1交流电压设置值
	{.pattern = "VOLT", .callback = VoltageAcS1,},//设置步骤1交流电压设置值
	{.pattern = "UPPC?", .callback = CUpperAcQ1,},//查询步骤1电流上限设置值
	{.pattern = "UPPC", .callback = CUpperAcS1,},//设置步骤1电流上限设置值
	{.pattern = "LOWC?", .callback = CLOWAcQ1,},//查询步骤1电流下限设置值
	{.pattern = "LOWC", .callback = CLOWAcS1,},//设置步骤1电流下限设置值
	{.pattern = "TTIM?", .callback = TTIMQ1,},//查询步骤1测试时间
	{.pattern = "TTIM", .callback = TTIMS1,},//设置步骤1测试时间
	{.pattern = "RTIM?", .callback = RTIMQ1,},//查询步骤1上升时间
	{.pattern = "RTIM", .callback = RTIMS1,},//设置步骤1上升时间
	{.pattern = "DTIM?", .callback = FTIMQ1,},//查询步骤1下降时间
	{.pattern = "DTIM", .callback = FTIMS1,},//设置步骤1下降时间
	{.pattern = "ARC?", .callback = ARCQ1,},//查询ARC
	{.pattern = "ARC", .callback = ARCS1,},//设置ARC
	{.pattern = "FREQ?", .callback = FREQQ1,},//查询频率
	{.pattern = "FREQ", .callback = FREQS1,},//设置频率
	{.pattern = "OPEN?", .callback = OPENQ1,},//查询OPEN比率
	{.pattern = "OPEN", .callback = OPENS1,},//设置OPEN比率
	{.pattern = "SHORT?", .callback = SHORTQ1,},//查询SHORT比率
	{.pattern = "SHORT", .callback = SHORTS1,},//设置SHORT比率
	{.pattern = "TEST", .callback = StartT,},//START
	{.pattern = "RESET", .callback = StopT,},//STOP
	{.pattern = "STEP", .callback = SetStep,},//STEP
	{.pattern = "TSTEPS", .callback = SetTSteps,},//TSTEPS
	{.pattern = "ACW", .callback = ACMode,},//AC
	{.pattern = "DCW", .callback = DCMode,},//DC
	{.pattern = "IRW", .callback = IRMode,},//IR
	{.pattern = "OSW", .callback = OSMode,},//OS
	{.pattern = "TD?", .callback = ReadRes,},//读取测试结果
	
	{.pattern = "FUNC:SOURce:STEP1:DC:VOLT?", .callback = VoltageDcQ1,},//查询步骤1直流电压设置值
    SCPI_CMD_LIST_END
};

scpi_interface_t scpi_interface = {
    .error = NULL,
    .write = SCPI_Write, 
    .control = NULL,
    .flush = NULL,
    .reset = NULL, 
};

char scpi_input_buffer[SCPI_INPUT_BUFFER_LENGTH];
scpi_error_t scpi_error_queue_data[SCPI_ERROR_QUEUE_SIZE];

scpi_t scpi_context;
