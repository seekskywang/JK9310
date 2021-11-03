/**********************************************************************
* $Id$		debug_frmwrk.c			2011-06-02
*//**
* @file		debug_frmwrk.c
* @brief	Contains some utilities that used for debugging through UART
* @version	1.0
* @date		02. June. 2011
* @author	NXP MCU SW Application Team
* 
* Copyright(C) 2011, NXP Semiconductor
* All rights reserved.
*
***********************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
**********************************************************************/

#include "debug_frmwrk.h"
#include "lpc177x_8x_pinsel.h"
#include "Key/key.h"
#include "stdarg.h"
#include "string.h"
#include "GlobalValue.h"
#include <assert.h>
Com_TypeDef ComBuf;
Com_TypeDef ComBuf3;
Com_TypeDef FacBuf;
u8 strbuff[8];
//extern Send_Testvalue_Typedef Send_Testvalue[2];
/* Debug framework */
//const vu8 READDATA[7]={0xAB,0x01,0x06,0x03,0x08,0xbf,'\0'};
void (*_db_msg)(LPC_UART_TypeDef *UARTx, const void *s);
void (*_db_msg_)(LPC_UART_TypeDef *UARTx, const void *s);
void (*_db_char)(LPC_UART_TypeDef *UARTx, uint8_t ch);
void (*_db_dec)(LPC_UART_TypeDef *UARTx, uint8_t decn);
void (*_db_dec_16)(LPC_UART_TypeDef *UARTx, uint16_t decn);
void (*_db_dec_32)(LPC_UART_TypeDef *UARTx, uint32_t decn);
void (*_db_hex)(LPC_UART_TypeDef *UARTx, uint8_t hexn);
void (*_db_hex_16)(LPC_UART_TypeDef *UARTx, uint16_t hexn);
void (*_db_hex_32)(LPC_UART_TypeDef *UARTx, uint32_t hexn);
void (*_db_hex_)(LPC_UART_TypeDef *UARTx, uint8_t hexn);
void (*_db_hex_16_)(LPC_UART_TypeDef *UARTx, uint16_t hexn);
void (*_db_hex_32_)(LPC_UART_TypeDef *UARTx, uint32_t hexn);
vu8 Uart0_Process(void);
uint8_t (*_db_get_char)(LPC_UART_TypeDef *UARTx);
uint8_t (*_db_get_val)(LPC_UART_TypeDef *UARTx, uint8_t option, uint8_t numCh, uint32_t * val);
//Com_TypeDef ComBuf;//串口收发缓冲

/*********************************************************************//**
 * @brief		Puts a character to UART port
 * @param[in]	UARTx	Pointer to UART peripheral
 * @param[in]	ch		Character to put
 * @return		None
 **********************************************************************/
void UARTPutChar (LPC_UART_TypeDef *UARTx, uint8_t ch)
{
	UART_Send(UARTx, &ch, 1, BLOCKING);
}


/*********************************************************************//**
 * @brief		Get a character to UART port
 * @param[in]	UARTx	Pointer to UART peripheral
 * @return		character value that returned
 **********************************************************************/
uint8_t UARTGetChar (LPC_UART_TypeDef *UARTx)
{
	uint8_t tmp = 0;

	UART_Receive(UARTx, &tmp, 1, BLOCKING);

	return(tmp);
}

/*********************************************************************//**
 * @brief		Get a character to UART port
 * @param[in]	UARTx	Pointer to UART peripheral
 * @return		character value that returned
 **********************************************************************/
uint8_t UARTGetValue (LPC_UART_TypeDef *UARTx, uint8_t option,
												uint8_t numCh, uint32_t* val)
{
	uint8_t tmpCh = 0, cnt, factor, isValidCh = FALSE;
	uint32_t tmpVal, rVal, cntFailed, multiplier;

	//it does not get any value
	if(numCh <= 0)
	{
		*val = 0;
		return 0;
	}

	cntFailed = 0;

	// To store the multiplier of Decimal (10) or Heximal (16)
	factor = (option == DBG_GETVAL_IN_HEX) ? 16 : ((option == DBG_GETVAL_IN_DEC) ? 10 : 0);

	if (factor == 0)
	{
		*val = 0;

		return 0;
	}

	rVal = 0;

	while (numCh > 0)
	{
		isValidCh = TRUE;

		UART_Receive(UARTx, &tmpCh, 1, BLOCKING);

		if((tmpCh >= '0') && (tmpCh<= '9'))
		{
			tmpVal = (uint32_t) (tmpCh - '0');
		}
		else if (option == DBG_GETVAL_IN_HEX)
		{
			factor = 16;

			switch (tmpCh)
			{
				case 'a':

				case 'A':
					tmpVal = 10;

					break;

				case 'b':

				case 'B':
					tmpVal = 11;

					break;

				case 'c':

				case 'C':
					tmpVal = 12;

					break;

				case 'd':

				case 'D':
					tmpVal = 13;

					break;

				case 'e':

				case 'E':
					tmpVal = 14;

					break;

				case 'f':

				case 'F':
					tmpVal = 15;

					break;

				default:
					isValidCh = FALSE;
					break;
			}
		}
		else
		{
			isValidCh = FALSE;
		}

		multiplier = 1;

		if(isValidCh == FALSE)
		{
			if(option == DBG_GETVAL_IN_DEC)
			{
				UARTPuts(UARTx, "Please enter a char from '0' to '9'!!!\r\n");
			}
			else if (option == DBG_GETVAL_IN_HEX)
			{
				UARTPuts(UARTx, "Please enter a char from '0' to '9', and 'a/A', 'b/B', c/C', 'd/D', 'e/E' and 'f/F'!!!\r\n");
			}

			cntFailed ++;

			if(cntFailed >= NUM_SKIPPED_ALLOWED)
			{
				UARTPuts(UARTx, "Reach limitation of re-tries. Return FAILED\r\n");

				//it's failed, should return
				return 0;
			}
		}
		else
		{
			//Echo the character to the terminal
			UARTPutChar(UARTx, tmpCh);

			if(numCh == 1)
			{
				factor = 1;
				multiplier = 1;
			}
			else
			{
				for(cnt = 1; cnt < numCh; cnt++)
				{
					multiplier *= factor;
				}
			}

			tmpVal *= multiplier;

			//Update the value return
			rVal += tmpVal;

			numCh --;
		}
	}

	*val = rVal;

	return(1);
}



/*********************************************************************//**
 * @brief		Puts a string to UART port
 * @param[in]	UARTx 	Pointer to UART peripheral
 * @param[in]	str 	string to put
 * @return		None
 **********************************************************************/
void UARTPuts(LPC_UART_TypeDef *UARTx, const void *str)
{
	uint8_t *s = (uint8_t *) str;

	while (*s!=0xBF)
	{
		UARTPutChar(UARTx, *s++);
	}
	UARTPutChar(UARTx, 0xbf);
	
}


/*********************************************************************//**
 * @brief		Puts a string to UART port and print new line
 * @param[in]	UARTx	Pointer to UART peripheral
 * @param[in]	str		String to put
 * @return		None
 **********************************************************************/
void UARTPuts_(LPC_UART_TypeDef *UARTx, const void *str)
{
	UARTPuts (UARTx, str);
	UARTPuts (UARTx, "\n\r");
}


/*********************************************************************//**
 * @brief		Puts a decimal number to UART port
 * @param[in]	UARTx	Pointer to UART peripheral
 * @param[in]	decnum	Decimal number (8-bit long)
 * @return		None
 **********************************************************************/
void UARTPutDec(LPC_UART_TypeDef *UARTx, uint8_t decnum)
{
	uint8_t c1=decnum%10;
	uint8_t c2=(decnum/10)%10;
	uint8_t c3=(decnum/100)%10;
	UARTPutChar(UARTx, '0'+c3);
	UARTPutChar(UARTx, '0'+c2);
	UARTPutChar(UARTx, '0'+c1);
}

/*********************************************************************//**
 * @brief		Puts a decimal number to UART port
 * @param[in]	UARTx	Pointer to UART peripheral
 * @param[in]	decnum	Decimal number (8-bit long)
 * @return		None
 **********************************************************************/
void UARTPutDec16(LPC_UART_TypeDef *UARTx, uint16_t decnum)
{
	uint8_t c1=decnum%10;
	uint8_t c2=(decnum/10)%10;
	uint8_t c3=(decnum/100)%10;
	uint8_t c4=(decnum/1000)%10;
	uint8_t c5=(decnum/10000)%10;
	UARTPutChar(UARTx, '0'+c5);
	UARTPutChar(UARTx, '0'+c4);
	UARTPutChar(UARTx, '0'+c3);
	UARTPutChar(UARTx, '0'+c2);
	UARTPutChar(UARTx, '0'+c1);
}

/*********************************************************************//**
 * @brief		Puts a decimal number to UART port
 * @param[in]	UARTx	Pointer to UART peripheral
 * @param[in]	decnum	Decimal number (8-bit long)
 * @return		None
 **********************************************************************/
void UARTPutDec32(LPC_UART_TypeDef *UARTx, uint32_t decnum)
{
	uint8_t c1=decnum%10;
	uint8_t c2=(decnum/10)%10;
	uint8_t c3=(decnum/100)%10;
	uint8_t c4=(decnum/1000)%10;
	uint8_t c5=(decnum/10000)%10;
	uint8_t c6=(decnum/100000)%10;
	uint8_t c7=(decnum/1000000)%10;
	uint8_t c8=(decnum/10000000)%10;
	uint8_t c9=(decnum/100000000)%10;
	uint8_t c10=(decnum/100000000)%10;
	UARTPutChar(UARTx, '0'+c10);
	UARTPutChar(UARTx, '0'+c9);
	UARTPutChar(UARTx, '0'+c8);
	UARTPutChar(UARTx, '0'+c7);
	UARTPutChar(UARTx, '0'+c6);
	UARTPutChar(UARTx, '0'+c5);
	UARTPutChar(UARTx, '0'+c4);
	UARTPutChar(UARTx, '0'+c3);
	UARTPutChar(UARTx, '0'+c2);
	UARTPutChar(UARTx, '0'+c1);
}

/*********************************************************************//**
 * @brief		Puts a hex number to UART port
 * @param[in]	UARTx	Pointer to UART peripheral
 * @param[in]	hexnum	Hex number (8-bit long)
 * @return		None
 **********************************************************************/
void UARTPutHex_ (LPC_UART_TypeDef *UARTx, uint8_t hexnum)
{
	uint8_t nibble, i;

	i = 1;
	do
	{
		nibble = (hexnum >> (4*i)) & 0x0F;

		UARTPutChar(UARTx, (nibble > 9) ? ('A' + nibble - 10) : ('0' + nibble));
	}
	while (i--);
}


/*********************************************************************//**
 * @brief		Puts a hex number to UART port
 * @param[in]	UARTx	Pointer to UART peripheral
 * @param[in]	hexnum	Hex number (8-bit long)
 * @return		None
 **********************************************************************/
void UARTPutHex (LPC_UART_TypeDef *UARTx, uint8_t hexnum)
{
	uint8_t nibble, i;

	UARTPuts(UARTx, "0x");

	i = 1;
	do {
		nibble = (hexnum >> (4*i)) & 0x0F;
		UARTPutChar(UARTx, (nibble > 9) ? ('A' + nibble - 10) : ('0' + nibble));
	} while (i--);
}


/*********************************************************************//**
 * @brief		Puts a hex number to UART port
 * @param[in]	UARTx	Pointer to UART peripheral
 * @param[in]	hexnum	Hex number (16-bit long)
 * @return		None
 **********************************************************************/
void UARTPutHex16_ (LPC_UART_TypeDef *UARTx, uint16_t hexnum)
{
	uint8_t nibble, i;

	i = 3;
	do
	{
		nibble = (hexnum >> (4*i)) & 0x0F;

		UARTPutChar(UARTx, (nibble > 9) ? ('A' + nibble - 10) : ('0' + nibble));
	}
	while (i--);
}


/*********************************************************************//**
 * @brief		Puts a hex number to UART port
 * @param[in]	UARTx	Pointer to UART peripheral
 * @param[in]	hexnum	Hex number (16-bit long)
 * @return		None
 **********************************************************************/
void UARTPutHex16 (LPC_UART_TypeDef *UARTx, uint16_t hexnum)
{
	uint8_t nibble, i;

	UARTPuts(UARTx, "0x");

	i = 3;
	do
	{
		nibble = (hexnum >> (4*i)) & 0x0F;

		UARTPutChar(UARTx, (nibble > 9) ? ('A' + nibble - 10) : ('0' + nibble));
	}
	while (i--);
}

/*********************************************************************//**
 * @brief		Puts a hex number to UART port
 * @param[in]	UARTx	Pointer to UART peripheral
 * @param[in]	hexnum	Hex number (32-bit long)
 * @return		None
 **********************************************************************/
void UARTPutHex32_ (LPC_UART_TypeDef *UARTx, uint32_t hexnum)
{
	uint8_t nibble, i;

	i = 7;
	do
	{
		nibble = (hexnum >> (4*i)) & 0x0F;

		UARTPutChar(UARTx, (nibble > 9) ? ('A' + nibble - 10) : ('0' + nibble));
	}
	while (i--);
}


/*********************************************************************//**
 * @brief		Puts a hex number to UART port
 * @param[in]	UARTx	Pointer to UART peripheral
 * @param[in]	hexnum	Hex number (32-bit long)
 * @return		None
 **********************************************************************/
void UARTPutHex32 (LPC_UART_TypeDef *UARTx, uint32_t hexnum)
{
	uint8_t nibble, i;

	UARTPuts(UARTx, "0x");

	i = 7;
	do
	{
		nibble = (hexnum >> (4*i)) & 0x0F;

		UARTPutChar(UARTx, (nibble > 9) ? ('A' + nibble - 10) : ('0' + nibble));
	}
	while (i--);
}

/*********************************************************************//**
 * @brief		print function that supports format as same as printf()
 * 				function of <stdio.h> library
 * @param[in]	None
 * @return		None
 **********************************************************************/
void  _printf (const  char *format, ...)
{
    static  char  buffer[512 + 1];
            va_list     vArgs;
//            char	*tmp;
    va_start(vArgs, format);
    vsprintf((char *)buffer, (char const *)format, vArgs);
    va_end(vArgs);

    _DBG(buffer);
}

/*********************************************************************//**
 * @brief		Initialize Debug frame work through initializing UART port
 * @param[in]	None
 * @return		None
 **********************************************************************/
void debug_frmwrk_init(void)
{
	UART_CFG_Type UARTConfigStruct;
	u32 data;
#if (USED_UART_DEBUG_PORT == 0)
	/*
	 * Initialize UART0 pin connect
	 * P0.2: TXD
	 * P0.3: RXD
	 */
	PINSEL_ConfigPin (0, 2, 1);
	PINSEL_ConfigPin (0, 3, 1);
#elif (USED_UART_DEBUG_PORT == 1)
	/*
	 * Initialize UART1 pin connect
	 * P2.0: TXD
	 * P2.1: RXD
	 */
	PINSEL_ConfigPin(2, 0, 2);
	PINSEL_ConfigPin(2, 1, 2);
#elif (USED_UART_DEBUG_PORT == 2)
	/*
	 * Initialize UART2 pin connect
	 * P0.10: TXD
	 * P0.11: RXD
	 */
	PINSEL_ConfigPin(0, 10, 1);
	PINSEL_ConfigPin(0, 11, 1);
#elif (USED_UART_DEBUG_PORT == 3)
	/*
	 * Initialize UART3 pin connect
	 * P0.0: TXD
	 * P0.1: RXD
	 */
	PINSEL_ConfigPin(0, 0, 4);
	PINSEL_ConfigPin(0, 1, 4);
#elif (USED_UART_DEBUG_PORT == 4)
	/*
	 * Initialize UART4 pin connect
	 * P0.22: TXD
	 * P2.9: RXD
	 */
	PINSEL_ConfigPin(0, 22, 3);
	PINSEL_ConfigPin(2, 9, 3);

#endif

	/* Initialize UART Configuration parameter structure to default state:
	 * Baudrate = 9600bps
	 * 8 data bit
	 * 1 Stop bit
	 * None parity
	 */
//	 switch(Tft_5520.Sys_Set.Buad)
//	{
//		case 0:
//			data=9600;
//		break;
//		case 1:
//			data=14400;
//		break;
//		case 2:
//			data=19200;
//		break;
//		case 3:
//			data=56000;
//		break;
//		case 4:
//			data=115200;
//		break ;
//		default:
//			data=9600;
//		break ;
//	
//	
//	}
	UART_ConfigStructInit(&UARTConfigStruct);
	// Re-configure baudrate to 115200bps
	UARTConfigStruct.Baud_rate = 9600;//data;//115200;

	// Initialize DEBUG_UART_PORT peripheral with given to corresponding parameter
	UART_Init(LPC_UART0, &UARTConfigStruct);//|UART_INTCFG_THRE
	UART_IntConfig(LPC_UART0,UART_INTCFG_RBR,ENABLE);
	//NVIC_SetPriority(UART0_IRQn, ((0x01<<3)|0x01));//??UART2?????
	 NVIC_EnableIRQ(UART0_IRQn);
	// Enable UART Transmit
	UART_TxCmd(LPC_UART0, ENABLE);

	_db_msg	= UARTPuts;
	_db_msg_ = UARTPuts_;
	_db_char = UARTPutChar;
	_db_hex = UARTPutHex;
	_db_hex_16 = UARTPutHex16;
	_db_hex_32 = UARTPutHex32;
	_db_hex_ = UARTPutHex_;
	_db_hex_16_ = UARTPutHex16_;
	_db_hex_32_ = UARTPutHex32_;
	_db_dec = UARTPutDec;
	_db_dec_16 = UARTPutDec16;
	_db_dec_32 = UARTPutDec32;
	_db_get_char = UARTGetChar;
	_db_get_val = UARTGetValue;
}

void Uart3_init(uint32_t freq)//wlan
{
	UART_CFG_Type UARTConfigStruct;
	vu32 data;

	/*
	 * Initialize UART3 pin connect
	 * P0.0: TXD
	 * P0.1: RXD
	 */
	PINSEL_ConfigPin(0, 0, 2);
	PINSEL_ConfigPin(0, 1, 2);
	GPIO_WAN_Config();
	
	



	/* Initialize UART Configuration parameter structure to default state:
	 * Baudrate = 9600bps
	 * 8 data bit
	 * 1 Stop bit
	 * None parity
	 */
	UART_ConfigStructInit(&UARTConfigStruct);
	// Re-configure baudrate to 115200bps
	switch(freq)
	{
		case 0:
			data=9600;
		break;
		case 1:
			data=14400;
		break;
		case 2:
			data=19200;
		break;
		case 3:
			data=56000;
		break;
		case 4:
			data=115200;
		break ;
		default:
			data=9600;
		break ;
	
	
	}
	UARTConfigStruct.Baud_rate = data;

	// Initialize DEBUG_UART_PORT peripheral with given to corresponding parameter
	UART_Init(LPC_UART3, &UARTConfigStruct);//|UART_INTCFG_THRE
	UART_IntConfig(LPC_UART3,UART_INTCFG_RBR,ENABLE);
//	NVIC_SetPriority(UART3_IRQn, ((0x01<<3)|0x02));//??UART2?????
	 NVIC_EnableIRQ(UART3_IRQn);
	// Enable UART Transmit
	UART_TxCmd(LPC_UART3, ENABLE);

	
}
//==========================================================
//函数名称：Uart0RecTimeOut
//函数功能：串口0接收超时判别
//入口参数：无
//出口参数：无
//创建日期：2014.04.07
//修改日期：2014.04.07
//备注说明：需要在系统时基10mS定时中调用
//==========================================================
void Uart0RecTimeOut(void)
{
	if (ComBuf.rec.TimeOut)//串口接收超时计时
	{
		ComBuf.rec.TimeOut--;
	}
	else//接收超时，结束当前帧
	{
		if (ComBuf.rec.ptr)//当前接收指针不为空
		{
			ComBuf.rec.end=TRUE;//数据接收结束
			ComBuf.rec.len=ComBuf.rec.ptr;//接收数据缓冲长度
			ComBuf.rec.ptr=0;
		}
	}
}

//==========================================================
//函数名称：Uart_Process
//函数功能：串口处理 读按键
//入口参数：无
//出口参数：无
//创建日期：2015.10.26 
//修改日期：2015.10.26 10:02
//备注说明：无
//==========================================================
//vu8 Uart_Process(void)
//{
//	vu8 kind=0xff;
//    Send_Testvalue_Typedef *pt;
//		if (ComBuf.rec.end)//接收数据结束
//		{
//				kind=ComBuf.rec.buf[PFRAMEKIND];//命令字
//			//准备接收下一帧数据sprintf
//			ComBuf.rec.end=0;//接收缓冲可读标志复位
//			ComBuf.rec.ptr=0;//接收指针清零
//            memset(ComBuf.rec.buf,0,5);
//            
//		}
//	ComBuf.rec.end=0;
//	return kind;
//}

vu8 Uart_Process(void)
{
//	u8 strbuff[8];
	u8 sec_king,i;
#if HW_UART_SUPPORT
	u8 kind=0xff;
	u8 lenth = 0;
	u8 recbuf[30];
#if DEBUG_SUPPORT
	 u8 str[(FRAME_LEN_MAX-FRAME_LEN_MIN)+1];//收发数据缓冲
#else
	 u8 str[(FRAME_LEN_MAX-FRAME_LEN_MIN)+1];//收发数据缓冲
#endif
if(FacBuf.rec.end)//通讯协议选择
{
	U9001_Save_sys.U9001_SYS.pselect	= FacBuf.rec.buf[2];
//	SaveData.devaddr = FacBuf.rec.buf[3];
	memset(FacBuf.rec.buf,'\0',30);//清空缓冲
	FacBuf.rec.end=FALSE;//接收缓冲可读标志复位
	FacBuf.rec.ptr=0;//接收指针清零
	
	SavePselect();
//	Saveaddr();
}
if(U9001_Save_sys.U9001_SYS.pselect == 0)//通讯协议1
{
	//	if(SaveData.System.Uart)//串口有效
	//	{
			if (ComBuf.rec.end)//接收数据结束
			{
				memset(str,'\0',(FRAME_LEN_MAX-FRAME_LEN_MIN+1));//清空缓冲
				{
					memcpy(str,&ComBuf.rec.buf[PDATASTART],(FRAME_LEN_MAX-FRAME_LEN_MIN)+1);//ComBuf.send.len-FRAME_LEN_MIN);//数据包
					kind=ComBuf.rec.buf[PFRAMEKIND];//命令字
					sec_king=ComBuf.rec.buf[PDATASTART];
				}
				//准备接收下一帧数据
				ComBuf.rec.end=FALSE;//接收缓冲可读标志复位
				ComBuf.rec.ptr=0;//接收指针清零
	//		}
	//	}

		switch(kind)
		{
			case FRAME_READ_RESULT://读取结果
				//串口发送测试数据:电压(5)+电阻(6)+时间(4)+分选(1)=16字节
				switch (GetSystemMessage())//系统信息
				{
					case MSG_IDLE:
						memcpy(strbuff,"----",5);
						break;
					case MSG_PAUSE:
						memcpy(strbuff,"----",5);
						//kind=0x9B;//测试中止
						break;
					case MSG_PASS:
						//kind=0x91;//测试通过
						memcpy(strbuff,"PASS",5);
						break;
					case MSG_HIGH:
						//kind=0x92;//上限报警
						memcpy(strbuff,"HIGH",5);
						break;
					case MSG_LOW:
						//kind=0x92;//下限报警
						memcpy(strbuff,"LOW ",5);
						break;
					case MSG_OVER:
						memcpy(strbuff,"BRK ",5);
						break;
					case MSG_ARC:
						memcpy(strbuff,"ARC ",5);
						break;
					case MSG_OFL://过流报警
						memcpy(strbuff,"HIGH",5);
						break;
					default:
						//kind=0x90;//正常测试
						memcpy(strbuff,"TEST",5);
						break;
				}	
				
				

				if(SaveData.Setup.Group_Item == W_ISETUP || SaveData.Setup.Group_Item == I_WSETUP)
				{
					if(sendflag == 0)
					{
						sendflag = 1;
						memset(ComBuf.send.buf,0,40);
		//				ComBuf.send.buf[0]=0xAA;
		//				ComBuf.send.begin=FALSE;
						if(GetSystemMessage() == MSG_IDLE)
						{
							strcat((char*)ComBuf.send.buf,(char*)sendbuff);
	//						memcpy(&ComBuf.send.buf[0],sendbuff,14);
							strcat((char*)ComBuf.send.buf,(char*)strbuff);
						}else{
							if(multstep == 1)
							{
								strcat((char*)ComBuf.send.buf,(char*)sendbuff);
	//							memcpy(&ComBuf.send.buf[0],sendbuff,14);
								if(SaveData.Setup.Group_Item == W_ISETUP)
								{
									strcat((char*)ComBuf.send.buf,(char*)strbuff);
								}else if(SaveData.Setup.Group_Item == I_WSETUP){
									strcat((char*)ComBuf.send.buf,"----");
								}
							}else{
								if(SaveData.Setup.Group_Item == W_ISETUP)
								{
									if(sendbuff[13] == 0x3b)
									{
										memcpy(&sendbuff[14],"PASS",4);
									}else if(sendbuff[14] == 0x3b){
										memcpy(&sendbuff[15],"PASS",4);
									}
//									strcat((char*)sendbuff,"PASS");
									memcpy(&ComBuf.send.buf[0],sendbuff,18);
								}else if(SaveData.Setup.Group_Item == I_WSETUP){
//									strcat((char*)sendbuff,"mA;");
									if(sendbuff[13] == 0x3b)
									{
										memcpy(&sendbuff[14],strbuff,4);
									}else if(sendbuff[14] == 0x3b){
										memcpy(&sendbuff[15],strbuff,4);
									}
//									strcat((char*)sendbuff,(char*)strbuff);
									strcat((char*)ComBuf.send.buf,(char*)sendbuff);
//									memcpy(&ComBuf.send.buf[0],sendbuff,18);
								}							
							}
						}
	//					memset(ComBuf1.send.buf,0,21);
	//					ComBuf1.send.buf[0]=0xAA;
	//					ComBuf1.send.begin=FALSE;
	//					memcpy(&ComBuf1.send.buf[1],sendbuff1,16);
						
						strcat((char*)ComBuf.send.buf,";");
						strcat((char*)ComBuf.send.buf,(char*)sendbuff1);
						if(multstep == 1)
						{
	//						strcat((char*)ComBuf.send.buf,"----");
							if(SaveData.Setup.Group_Item == W_ISETUP)
							{
								strcat((char*)ComBuf.send.buf,"----");
							}else if(SaveData.Setup.Group_Item == I_WSETUP){
								strcat((char*)ComBuf.send.buf,(char*)strbuff);
							}
						}else{
							if(SaveData.Setup.Group_Item == W_ISETUP)
							{
								strcat((char*)ComBuf.send.buf,(char*)strbuff);
							}else if(SaveData.Setup.Group_Item == I_WSETUP){
								strcat((char*)ComBuf.send.buf,"PASS");
							}
							
						}
						strcat((char*)ComBuf.send.buf,(char*)sendend);//尾部增加回车和换行符
						for(i=0;i<39;i++)
						{
							USART_SendData(USART1, ComBuf.send.buf[i]);
							while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
						}
						ComBuf.rec.end=FALSE;//接收缓冲可读标志复位
	//					USART_SendData(USART1, 0xBB);
	//					while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);

	//					break;
					}else{
						for(i=0;i<39;i++)
						{
							USART_SendData(USART1, ComBuf.send.buf[i]);
							while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
						}
						ComBuf.rec.end=FALSE;//接收缓冲可读标志复位
					}
				}else{
					if(sendflag == 0)
					{
						sendflag = 1;
						memset(ComBuf.send.buf,0,40);
		//				ComBuf.send.buf[0]=0xAA;
		//				ComBuf.send.begin=FALSE;
						memcpy(&ComBuf.send.buf[0],sendbuff,14);
						strcat((char*)ComBuf.send.buf,(char*)strbuff);
						
						strcat((char*)ComBuf.send.buf,(char*)sendend);//尾部增加回车和换行符
						
						for(i=0;i<20;i++)
						{
							USART_SendData(USART1, ComBuf.send.buf[i]);
							while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
						}
					}else{
						for(i=0;i<20;i++)
						{
							USART_SendData(USART1, ComBuf.send.buf[i]);
							while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
						}
					}
				}
				

				
				
				
				break;
			
			case FRAME_START://启动
				run_stemp=0;
				SetSystemStatus(SYS_STATUS_START);//系统状态-启动测试
				break;

			case FRAME_RESET://复位
				if(GetSystemStatus() == SYS_STATUS_TEST){
					SetSystemMessage(MSG_PAUSE);//系统信息-暂停测试
					SetSystemStatus(SYS_STATUS_TEST_PAUSE);//系统状态-待机
					Plc_Start_Off();
				}else{
					SetSystemStatus(SYS_STATUS_IDLE);//系统状态-待机
				}
				break;

			case FRAME_WRITE_SN://写序列号
				break;
			
			case FRAME_CLR_BOOT_NUM://清开机次数
				break;
			case FRAME_ITEM://设置测试项目
				switch(sec_king)
				{
					case 0x00:
						SaveData.Setup.Group_Item=0;
						break;
					case 0x01:
						SaveData.Setup.Group_Item=1;
						break;
					case 0x10:
						SaveData.Setup.Group_Item=2;
						break;
					case 0x11:
						SaveData.Setup.Group_Item=3;
						break;
					default:
						break;
				}
				SaveGroup();//保存组别
				Store_set_flash();
				Disp_Idle_Menu();//显示待测界面
				break;
			case FRAME_DATA://数据帧
				SaveData.Group=str[1]-1;
				switch(sec_king)
				{
					case 0xAC:
						SaveData.Setup.Item=0;
						SaveData.Setup.Output=(u16)BCDtoHex1(str[2],1)*100+BCDtoHex1(str[3],1);
						SaveData.Setup.High=(u16)BCDtoHex1(str[4],1)*100+BCDtoHex1(str[5],1);
						SaveData.Setup.Low=(u16)BCDtoHex1(str[6],1)*100+BCDtoHex1(str[7],1);
						SaveData.Setup.RampDelay=(u16)BCDtoHex1(str[8],1)*100+BCDtoHex1(str[9],1);
						SaveData.Setup.TestTime=(u16)BCDtoHex1(str[10],1)*100+BCDtoHex1(str[11],1);
						SaveData.Setup.Arc=str[12];
						if(str[13]==0x50)
							SaveData.Setup.Freq=0;
						else
							SaveData.Setup.Freq=1;
						break;
					case 0xDC:
						SaveData.Setup.Item=1;
						SaveData.Setup.Output=(u16)BCDtoHex1(str[2],1)*100+BCDtoHex1(str[3],1);
						SaveData.Setup.High=(u16)BCDtoHex1(str[4],1)*100+BCDtoHex1(str[5],1);
						SaveData.Setup.Low=(u16)BCDtoHex1(str[6],1)*100+BCDtoHex1(str[7],1);
						SaveData.Setup.RampDelay=(u16)BCDtoHex1(str[8],1)*100+BCDtoHex1(str[9],1);
						SaveData.Setup.TestTime=(u16)BCDtoHex1(str[10],1)*100+BCDtoHex1(str[11],1);
						SaveData.Setup.Arc=str[12];
						break;
					case 0xAD:
						SaveData.Setup.I_Volt=(u16)BCDtoHex1(str[2],1)*100+BCDtoHex1(str[3],1);
						SaveData.Setup.I_High=(u16)BCDtoHex1(str[4],1)*100+BCDtoHex1(str[5],1);
						SaveData.Setup.I_Low=(u16)BCDtoHex1(str[6],1)*100+BCDtoHex1(str[7],1);
						SaveData.Setup.I_Delay=(u16)BCDtoHex1(str[8],1)*100+BCDtoHex1(str[9],1);
						
						break;
					
				
				}
				SaveGroup();//保存组别
				Store_set_flash();
				Disp_Idle_Menu();//显示待测界面
				break;
			case FRAME_SELECT_GROUP:
				SaveData.Group=sec_king-1;
				SaveGroup();
				SetSystemStatus(SYS_STATUS_IDLE);//待机状态
				ReadSetByGroup();
				Parameter_valuecomp();//比较设置参数
				Disp_Idle_Menu();//显示待测界面
				break;		
			default:
				break;
			
		}
//	Disp_Idle_Menu();//显示待测界面
	}
}else if(SaveData.pselect == 1){//通讯协议2
	if (ComBuf.rec.end)//接收数据结束
	{
		memcpy(recbuf,&ComBuf.rec.buf[0],30);
		lenth=recbuf[2];//数据长度
		kind=recbuf[3];//命令字
		ComBuf.rec.end=FALSE;//接收缓冲可读标志复位
		ComBuf.rec.ptr=0;//接收指针清零
	}
	
	switch(kind)
	{
		case TEST_RST://测试/复位命令
		{
			if(lenth == 2)
			{
				if(recbuf[4] == 'T')//测试命令
				{
					run_stemp=0;
					SetSystemStatus(SYS_STATUS_START);//系统状态-启动测试
					respond = 1;
				}else if(recbuf[4] == 'R'){//复位命令
					if(GetSystemStatus() == SYS_STATUS_TEST){
						SetSystemMessage(MSG_PAUSE);//系统信息-暂停测试
						SetSystemStatus(SYS_STATUS_TEST_PAUSE);//系统状态-待机
						Plc_Start_Off();
						respond = 1;
					}else{
						SetSystemStatus(SYS_STATUS_IDLE);//系统状态-待机
					}
				}else if(recbuf[4] == 'S'){//读数据命令
					SendRes();
				}
			}else if(lenth == 3){
				if(recbuf[4] == 't')//选择组别并测试命令
				{
					run_stemp=0;
					SaveData.Group=recbuf[5];
					SaveGroup();
					SetSystemStatus(SYS_STATUS_IDLE);//待机状态
					ReadSetByGroup();
					Parameter_valuecomp();//比较设置参数					
					SetSystemStatus(SYS_STATUS_START);//系统状态-启动测试
				}
			}
		}break;
		case GROUPSET://组别设置
		{
			SaveData.Group=recbuf[4];
			SaveGroup();
			SetSystemStatus(SYS_STATUS_IDLE);//待机状态
			ReadSetByGroup();
			Parameter_valuecomp();//比较设置参数
			Disp_Idle_Menu();//显示待测界面
		}break;
		case BASICSET:	//设置组别参数
		{
			U2.w_ma = recbuf[4];
			SaveData.Setup.Group_Item = U2.BIT_FLAG.item;//测试项目
			SaveData.Setup.Item = U2.BIT_FLAG.ACDC;//交直流
			SaveData.Setup.Freq = U2.BIT_FLAG.ACWF;//频率
			SaveData.Setup.Arc = U2.BIT_FLAG.ARC;//电弧
			
			SaveData.Setup.I_Volt = recbuf[5];//绝缘测试输出电压
			SaveData.Setup.Output = (u16)(recbuf[6]<<8) + recbuf[7];//耐压测试输出电压
			SaveData.Setup.High=(u16)(recbuf[8]<<8) + recbuf[9];//耐压测试漏电上限
			SaveData.Setup.Low=(u16)(recbuf[10]<<8) + recbuf[11];//耐压测试漏电下限
			SaveData.Setup.RampDelay=(u16)(recbuf[12]<<8) + recbuf[13];//耐压缓升时间
			SaveData.Setup.TestTime=(u16)(recbuf[14]<<8) + recbuf[15];//耐压测试时间
			SaveData.Setup.I_High=(u16)(recbuf[16]<<8) + recbuf[17];//绝缘上限
			SaveData.Setup.I_Low=(u16)(recbuf[18]<<8) + recbuf[19];//绝缘下限
			SaveData.Setup.I_Delay=(u16)(recbuf[20]<<8) + recbuf[21];//延时判定
			
			SaveGroup();//保存组别
			Store_set_flash();
			Disp_Idle_Menu();//显示待测界面
		}break;
	}
}
#endif
}
/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
/*********************************************************************//**
 * @brief		UART0 interrupt handler sub-routine
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void UART0_IRQHandler(void)//UART0_IRQn
{
	 uint8_t Status,dat;
	Status=UART_GetLineStatus(LPC_UART0);//
	Status=UART_ReceiveByte(LPC_UART0);
    keyvalue=Status;
	if(U9001_Save_sys.U9001_SYS.pselect == 0)//原始协议
	{
		if (!ComBuf.rec.end)//接收没结束
		{
			SetRecTimeOut(REC_TIME_OUT);//设置接收超时周期
			dat=Status;
			if (/*dat==(u8)(UART_REC_BEGIN)*/ComBuf.rec.ptr==0)//帧头
			{
				if(dat!=(u8)(UART_REC_BEGIN)) //首字节
				{
					ComBuf.rec.ptr=0;//重新接收 
				}
				else
				{
					ComBuf.rec.buf[ComBuf.rec.ptr++]=dat;
				}
			}
			else if (dat==(u8)(UART_REC_END))//帧尾
			{
				ComBuf.rec.buf[ComBuf.rec.ptr++]=dat;
				ComBuf.rec.end=TRUE;//接收结束
				ComBuf.rec.len=ComBuf.rec.ptr;//存接收数据长度
				ComBuf.rec.ptr=0;//指针清零重新开始新的一帧接收
				ComBuf.rec.TimeOut=0;//接收超时清零
			}
			else
			{
				if (ComBuf.rec.ptr>=REC_LEN_MAX)//最大接收帧长度
				{
					ComBuf.rec.ptr=0;//重新接收
				}
				else
				{
					ComBuf.rec.buf[ComBuf.rec.ptr++]=dat;
				}
			}
		}
	}else if(U9001_Save_sys.U9001_SYS.pselect == 1){//新协议
		if (!ComBuf.rec.end)//接收没结束
			{
				SetRecTimeOut(REC_TIME_OUT);//设置接收超时周期
				dat=Status;
				if (/*dat==(u8)(UART_REC_BEGIN20) && */ComBuf.rec.ptr == 0)//帧头1
				{
					if(dat!=(u8)(UART_REC_BEGIN20)) //首字节
					{
						ComBuf.rec.ptr=0;//重新接收 
					}
					else
					{
						ComBuf.rec.buf[ComBuf.rec.ptr++]=dat;
					}
				}
				else if (/*dat==SaveData.devaddr(u8)(UART_REC_BEGIN21) && */ComBuf.rec.ptr == 1)//帧头2
				{
					if(dat!=UART_REC_BEGIN21) //第二字节
					{
						ComBuf.rec.ptr=0;//重新接收 
					}
					else
					{
						ComBuf.rec.buf[ComBuf.rec.ptr++]=dat;
					}
				}else{
					if (ComBuf.rec.ptr>=REC_LEN_MAX)//最大接收帧长度
					{
						ComBuf.rec.ptr=0;//重新接收
					}
					else
					{
						ComBuf.rec.buf[ComBuf.rec.ptr++]=dat;
						if(ComBuf.rec.ptr > 2 && (ComBuf.rec.ptr == ComBuf.rec.buf[2]+3))
						{
//							ComBuf.rec.buf[ComBuf.rec.ptr++]=dat;
							ComBuf.rec.end=TRUE;//接收结束
							ComBuf.rec.len=ComBuf.rec.ptr;//存接收数据长度
							ComBuf.rec.ptr=0;//指针清零重新开始新的一帧接收
							ComBuf.rec.TimeOut=0;//接收超时清零
						}
					}
				}
			}
	}
	
	
	//出厂协议设置命令
		if(!FacBuf.rec.end)
		{
			SetRecTimeOut(REC_TIME_OUT);//设置接收超时周期
			dat=USART_ReceiveData(USART1);
			if (/*dat==0XFF*/FacBuf.rec.ptr==0)//帧头1
			{
				if(dat!=0XFF) //首字节
				{
					FacBuf.rec.ptr=0;//重新接收 
				}
				else
				{
					FacBuf.rec.buf[FacBuf.rec.ptr++]=dat;
				}
			}
			else if (/*dat==0XEE*/FacBuf.rec.ptr==1)//帧头2
			{
				if(dat!=0XEE) //第二字节
				{
					FacBuf.rec.ptr=0;//重新接收 
				}
				else
				{
					FacBuf.rec.buf[FacBuf.rec.ptr++]=dat;
				}
			}else if (/*dat==0XDD*/FacBuf.rec.ptr==4)//帧尾
			{
				if(dat!=0XDD) //第五字节
				{
					FacBuf.rec.ptr=0;//重新接收 
				}else{
					FacBuf.rec.buf[FacBuf.rec.ptr++]=dat;
					FacBuf.rec.end=TRUE;//接收结束
					FacBuf.rec.len=FacBuf.rec.ptr;//存接收数据长度
					FacBuf.rec.ptr=0;//指针清零重新开始新的一帧接收
					FacBuf.rec.TimeOut=0;//接收超时清零	
				}					
				
			}
			else
			{
				if (FacBuf.rec.ptr>=REC_LEN_MAX)//最大接收帧长度
				{
					FacBuf.rec.ptr=0;//重新接收
				}
				else
				{
					FacBuf.rec.buf[FacBuf.rec.ptr++]=dat;
				}
			}
		}
			
	   }
	if (!ComBuf.rec.end)//接收没结束
		{
			SetRecTimeOut(REC_TIME_OUT);//设置接收超时周期
			dat=Status;
			if (dat==(vu8)(UART_REC_BEGIN))//帧头
			{
				if(ComBuf.rec.ptr!=0) //首字节
				{
					ComBuf.rec.ptr=0;//重新接收 
				}
				else
				{
					ComBuf.rec.buf[ComBuf.rec.ptr++]=dat;
				}
			}
			else if (dat==(vu8)(UART_REC_END))//帧尾
			{
				ComBuf.rec.buf[ComBuf.rec.ptr++]=dat;
				ComBuf.rec.end=TRUE;//接收结束
				ComBuf.rec.len=ComBuf.rec.ptr;//存接收数据长度
				ComBuf.rec.ptr=0;//指针清零重新开始新的一帧接收
				ComBuf.rec.TimeOut=0;//接收超时清零
                keyvalue=Uart_Process();
			}
			else
			{
				if (ComBuf.rec.ptr>=REC_LEN_MAX)//最大接收帧长度
				{
					ComBuf.rec.ptr=0;//重新接收
				}
				else
				{
					ComBuf.rec.buf[ComBuf.rec.ptr++]=dat;
				}
			}
		}
}
//void UART3_IRQHandler(void)
//{
//	uint8_t Status;
//    uint8_t dat;
////	USART_ClearFlag(UART3_IRQn,USART_FLAG_TC);
//	
//	//NVIC_ClearPendingIRQ(UART3_IRQn);
//	Status=UART_GetLineStatus(LPC_UART3);
//	Status=UART_ReceiveByte(LPC_UART3);
//    if (!ComBuf.rec.end)//接收没结束
//    {
//        SetRecTimeOut(REC_TIME_OUT);//设置接收超时周期
//        dat=Status;
//        if (dat==(vu8)(UART_REC_BEGIN))//帧头
//        {
//            if(ComBuf3.rec.ptr!=0) //首字节
//            {
//                ComBuf3.rec.ptr=0;//重新接收 
//            }
//            else
//            {
//                ComBuf3.rec.buf[ComBuf3.rec.ptr++]=dat;
//            }
//        }
//        else if (dat==(vu8)(UART_REC_END))//帧尾
//        {
//            ComBuf3.rec.buf[ComBuf3.rec.ptr++]=dat;
//            ComBuf3.rec.end=TRUE;//接收结束
//            ComBuf3.rec.len=ComBuf3.rec.ptr;//存接收数据长度
//            ComBuf3.rec.ptr=0;//指针清零重新开始新的一帧接收
//            ComBuf3.rec.TimeOut=0;//接收超时清零
////           Uart3_Process();
//        }
//        else
//        {
//            if (ComBuf3.rec.ptr>=REC_LEN_MAX)//最大接收帧长度
//            {
//                ComBuf3.rec.ptr=0;//重新接收
//            }
//            else
//            {
//                ComBuf3.rec.buf[ComBuf3.rec.ptr++]=dat;
//            }
//        }
//    }

//}
void Uart0_Send(u8 king)
{
    vu8 *pt;
    vu8 i;
	Send_Testvalue[0].start=UART_SEND_BEGIN;
	Send_Testvalue[0].end=UART_SEND_END;
    Send_Testvalue[0].king=king;
    pt=(vu8 *)&Send_Testvalue[0];
    for(i=0;i<(sizeof(Send_Testvalue[0]));i++)
    {
		UARTPutChar(LPC_UART0,*(pt+i) );
        
    }

}
//UARTPuts( LPC_UART0, Send_buff);


//==========================================================
//函数名称：SendDataToCom
//函数功能：发送串口数据
//入口参数：无
//出口参数：True(发送)/False
//创建日期：2014.04.08 
//修改日期：2014.04.08 12:50
//备注说明：按串口发送结构表处理
//==========================================================
//bool SendDataToCom(void)
//{
//	if ((!ComBuf.send.begin)&&ComBuf.send.len)//有数据且发送没开始
//		{
//			ComBuf.send.begin=TRUE;//置发送忙标志
//			ComBuf.send.ptr=0;//当前指针清零
//			SBUF=ComBuf.send.buf[ComBuf.send.ptr++];//启动发送
//			return (1);
//		}
//	return (0);
//}

