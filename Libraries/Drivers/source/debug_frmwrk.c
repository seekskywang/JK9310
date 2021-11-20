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
#include "scpi/scpi.h"
#include "scpi-def.h"
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

//#if (USED_UART_DEBUG_PORT == 0)
//	/*
//	 * Initialize UART0 pin connect
//	 * P0.2: TXD
//	 * P0.3: RXD
//	 */
	
	PINSEL_ConfigPin(0, 0, 4);
	PINSEL_ConfigPin(0, 1, 4);
	
//	PINSEL_ConfigPin (0, 2, 1);
//	PINSEL_ConfigPin (0, 3, 1);
//#elif (USED_UART_DEBUG_PORT == 1)
//	/*
//	 * Initialize UART1 pin connect
//	 * P2.0: TXD
//	 * P2.1: RXD
//	 */
//	PINSEL_ConfigPin(2, 0, 2);
//	PINSEL_ConfigPin(2, 1, 2);
//#elif (USED_UART_DEBUG_PORT == 2)
//	/*
//	 * Initialize UART2 pin connect
//	 * P0.10: TXD
//	 * P0.11: RXD
//	 */
//	PINSEL_ConfigPin(0, 10, 1);
//	PINSEL_ConfigPin(0, 11, 1);
//#elif (USED_UART_DEBUG_PORT == 3)
//	/*
//	 * Initialize UART3 pin connect
//	 * P0.2: TXD
//	 * P0.3: RXD
//	 */
//	PINSEL_ConfigPin(0, 2, 2);
//	PINSEL_ConfigPin(0, 3, 2);
//#elif (USED_UART_DEBUG_PORT == 4)
//	/*
//	 * Initialize UART4 pin connect
//	 * P0.22: TXD
//	 * P2.9: RXD
//	 */
//	PINSEL_ConfigPin(0, 22, 3);
//	PINSEL_ConfigPin(2, 9, 3);

//#endif

	/* Initialize UART Configuration parameter structure to default state:
	 * Baudrate = 9600bps
	 * 8 data bit
	 * 1 Stop bit
	 * None parity
	 */
	UART_ConfigStructInit(&UARTConfigStruct);
	// Re-configure baudrate to 115200bps
	UARTConfigStruct.Baud_rate = 9600;
//	UARTConfigStruct.Baud_rate = 9600;
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


	/*
	 * Initialize UART3 pin connect
	 * P0.2: TXD
	 * P0.3: RXD
	 */
	PINSEL_ConfigPin (0, 2, 2);
	PINSEL_ConfigPin (0, 3, 2);

	/* Initialize UART Configuration parameter structure to default state:
	 * Baudrate = 9600bps
	 * 8 data bit
	 * 1 Stop bit
	 * None parity
	 */
	UART_ConfigStructInit(&UARTConfigStruct);
	// Re-configure baudrate to 115200bps
	UARTConfigStruct.Baud_rate = 9600;
//	UARTConfigStruct.Baud_rate = 9600;
	// Initialize DEBUG_UART_PORT peripheral with given to corresponding parameter
	UART_Init(LPC_UART3, &UARTConfigStruct);//|UART_INTCFG_THRE
	UART_IntConfig(LPC_UART3,UART_INTCFG_RBR,ENABLE);
	//NVIC_SetPriority(UART0_IRQn, ((0x01<<3)|0x01));//??UART2?????
	 NVIC_EnableIRQ(UART3_IRQn);
	// Enable UART Transmit
	UART_TxCmd(LPC_UART3, ENABLE);

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
//==========================================================
//函数名称：Uart_Process
//函数功能：串口处理 读按键
//入口参数：无
//出口参数：无
//创建日期：2015.10.26 
//修改日期：2015.10.26 10:02
//备注说明：无
//==========================================================
vu8 Uart_Process(void)
{
	vu8 kind=0xff;
    Send_Testvalue_Typedef *pt;
		if (ComBuf.rec.end)//接收数据结束
		{
				kind=ComBuf.rec.buf[PFRAMEKIND];//命令字
			//准备接收下一帧数据sprintf
			ComBuf.rec.end=0;//接收缓冲可读标志复位
			ComBuf.rec.ptr=0;//接收指针清零
            memset(ComBuf.rec.buf,0,5);
            
		}
	ComBuf.rec.end=0;
	return kind;
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
}

/*********************************************************************//**
 * @brief		UART3 interrupt handler sub-routine
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void UART3_IRQHandler(void)//UART3_IRQn
{
	 uint8_t Status,Res;
	 Status=UART_GetLineStatus(LPC_UART3);//
	 Res=UART_ReceiveByte(LPC_UART3);
	 SCPI_Input(&scpi_context, &Res, 1);
}

//void UART0_IRQHandler(void)//UART0_IRQn
//{
//	uint8_t Status,dat;
//	Status=UART_GetLineStatus(LPC_UART0);//
//	Status=UART_ReceiveByte(LPC_UART0);
//    keyvalue=Status;
//	
//	SCPI_Input(&scpi_context, &Status, 1);
//}

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

