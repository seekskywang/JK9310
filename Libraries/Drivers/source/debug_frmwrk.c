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
#include <stdlib.h>
#include <stdio.h>
Com_TypeDef ComBuf;
Com_TypeDef ComBuf3;
Com_TypeDef FacBuf;
u8 strbuff[8];
u8 g_mods_timeout = 0;

static const char IRUNITJK1[][3]={
	{"k"},//
    {"M"},//
    {"G"},//
};
char sendend[2] = {0x0d,0x0a};

struct MODS_T g_tModS;
static void MODS_03H(void);
static void MODS_06H(void);

static void MODS_SendAckErr(uint8_t _ucErrCode);
static uint8_t MODS_WriteRegValue(uint16_t reg_addr, uint16_t reg_value);
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

/*************************************************************************************************************************
*函数 ? ? ? ? : void BCDtoHex(u8 *pBuff,u8 len)
*功能 ? ? ? ? : BCD码转为十六进制
*参数 ? ? ? ? : pBuff:输入的十六进制数组,len:数组长度
*返回 ? ? ? ? : 无
*依赖 : 底层宏定义
* 作者 : li_qcxy@126.com
* 时间 : 2017-1-5
* 最后修改时间:?
*说明 ? ? ? ? : 
*************************************************************************************************************************/
void BCDtoHex(u8 *pBuff,u8 len) //BCD码转为十六进制
{
	u8 i,temp;
	for(i = 0;i < len;i ++)
	{
		temp =pBuff[i] / 16;
		pBuff[i] = temp * 10 + pBuff[i] % 16;
	}
}
u8 BCDtoHex1(u8 pBuff,u8 len) //BCD码转为十六进制
{
	u8 i,temp;
	
	temp =pBuff / 16;
	pBuff = temp * 10 + pBuff % 16;
	return pBuff;
}

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

///*********************************************************************//**
// * @brief		print function that supports format as same as printf()
// * 				function of <stdio.h> library
// * @param[in]	None
// * @return		None
// **********************************************************************/
//void  _printf (const  char *format, ...)
//{
//    static  char  buffer[512 + 1];
//            va_list     vArgs;
////            char	*tmp;
//    va_start(vArgs, format);
//    vsprintf((char *)buffer, (char const *)format, vArgs);
//    va_end(vArgs);

//    _DBG(buffer);
//}

/*
*********************************************************************************************************
*	函 数 名: MODS_SendAckErr
*	功能说明: 发送错误应答
*	形    参: _ucErrCode : 错误代码
*	返 回 值: 无
*********************************************************************************************************
*/
static void MODS_SendAckErr(uint8_t _ucErrCode)
{
	uint8_t txbuf[3];

	txbuf[0] = g_tModS.RxBuf[0];					/* 485地址 */
	txbuf[1] = g_tModS.RxBuf[1] | 0x80;				/* 异常的功能码 */
	txbuf[2] = _ucErrCode;							/* 错误代码(01,02,03,04) */

	MODS_SendWithCRC(txbuf, 3);
}


/*
*********************************************************************************************************
*	函 数 名: MODS_SendAckOk
*	功能说明: 发送正确的应答.
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void MODS_SendAckOk(void)
{
	uint8_t txbuf[6];
	uint8_t i;

	for (i = 0; i < 6; i++)
	{
		txbuf[i] = g_tModS.RxBuf[i];
	}
	MODS_SendWithCRC(txbuf, 6);
}

/*
*********************************************************************************************************
*	函 数 名: BEBufToUint16
*	功能说明: 将2字节数组(大端Big Endian次序，高字节在前)转换为16位整数
*	形    参: _pBuf : 数组
*	返 回 值: 16位整数值
*
*   大端(Big Endian)与小端(Little Endian)
*********************************************************************************************************
*/
uint16_t BEBufToUint16(uint8_t *_pBuf)
{
    return (((uint16_t)_pBuf[0] << 8) | _pBuf[1]);
}

/*****************  发送一个字符 **********************/
//void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
//{
//	/* 发送一个字节数据到USART */
//	USART_SendData(pUSARTx,ch);
//		
//	/* 等待发送数据寄存器为空 */
//	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
//}

//void uart1SendChars(u8 *str, u16 strlen)
//{ 
//	  u16 k= 0 ; 
//   do { 
//       USART_SendData(USART1,*(str + k));
//       while (USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
//       k++; 
//       }   //循环发送,直到发送完毕   
//    while (k < strlen); 
//} 

void MODS_SendWithCRC(uint8_t *_pBuf, uint8_t _ucLen)
{
	uint16_t crc;
	uint8_t i;
	uint8_t buf[S_TX_BUF_SIZE];
	memcpy(buf, _pBuf, _ucLen);
	crc = CRC16(_pBuf, _ucLen);
	buf[_ucLen++] = crc >> 8;
	buf[_ucLen++] = crc;
//通过485发数据
//	RS485_SendBuf(buf, _ucLen);
	for(i=0;i<_ucLen;i++)
	{
		UARTPutChar(LPC_UART3,buf[i]);
	}
//	uart1SendChars(buf, _ucLen);
	
// #if 1									/* 此部分为了串口打印结果,实际运用中可不要 */
// 	g_tPrint.Txlen = _ucLen;
// 	memcpy(g_tPrint.TxBuf, buf, _ucLen);
// #endif
}


 // CRC 高位字节值表
static const uint8_t s_CRCHi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
} ;
// CRC 低位字节值表
const uint8_t s_CRCLo[] = {
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
	0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
	0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
	0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
	0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
	0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
	0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
	0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
	0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
	0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
	0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
	0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
	0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
	0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
	0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
	0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
	0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
	0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
	0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

uint16_t CRC16(uint8_t *_pBuf, uint16_t _usLen)
{    
    uint8_t ucCRCHi = 0xFF; /* 高CRC字节初始化 */
	uint8_t ucCRCLo = 0xFF; /* 低CRC 字节初始化 */
	uint16_t usIndex;  /* CRC循环中的索引 */

    while (_usLen--)
    {
		usIndex = ucCRCHi ^ *_pBuf++; /* 计算CRC */
		ucCRCHi = ucCRCLo ^ s_CRCHi[usIndex];
		ucCRCLo = s_CRCLo[usIndex];
    }
    return ((uint16_t)ucCRCHi << 8 | ucCRCLo);
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

void UartResJK1(void)
{
	u8 i;
	char stepbuf[50];
	if(U9001_Save_sys.U9001_save.all_step == 1)//单项
	{
		if(U9001_Save_sys.U9001_save.U9001_Setup[1].parameter != IR)
		{
			sprintf(Resbuf,"%.2fkV;%.2fmA;%s",
			(double)Save_TestValue[0].Text_vot/1000,//电压
			(double)Save_TestValue[0].Text_value/1000,//电流
			SEND_COMP[Save_TestValue[0].text_flag][1]//分选结果		
			);
		}else{
			sprintf(Resbuf,"%.2fkV;%.1f%s;%s;",			
			(double)Save_TestValue[i].Text_vot/1000,//电压
			(double)Save_TestValue[i].Text_value/1000,//内阻
			IRUNITJK1[Save_TestValue[i].text_unit],//内阻单位
			SEND_COMP[Save_TestValue[i].text_flag][1]);//分选结果
		}
	}else{
		
	}
//	else if(U9001_Save_sys.U9001_save.U9001_Setup[i+1].parameter == OS){//OS模式
//		sprintf(stepbuf,"STEP%02d,%s,%s,%.3fkV,%.1fnF,%.1fs;",
//		i+1,
//		TestPara[U9001_Save_sys.U9001_save.U9001_Setup[i+1].parameter],//模式
//		SEND_COMP[Save_TestValue[i].text_flag][1],//分选结果
//		(double)Save_TestValue[i].Text_vot/1000,//电压
//		(double)Save_TestValue[i].Text_value/10,//电容
//		(double)Save_TestValue[i].Text_time/10);//测试时间
//	}else if(U9001_Save_sys.U9001_save.U9001_Setup[i+1].parameter == IR){//绝缘模式
//		sprintf(stepbuf,"STEP%02d,%s,%s,%.3fkV,%.3f%sΩ,%.1fs;",
//		i+1,
//		TestPara[U9001_Save_sys.U9001_save.U9001_Setup[i+1].parameter],//模式
//		SEND_COMP[Save_TestValue[i].text_flag][1],//分选结果
//		(double)Save_TestValue[i].Text_vot/1000,//电压
//		(double)Save_TestValue[i].Text_value/1000,//内阻
//		IRUNIT[Save_TestValue[i].text_unit],//内阻单位
//		(double)Save_TestValue[i].Text_time/10);//测试时间
//	}
	
	strcat(Resbuf,stepbuf);
}

vu8 Uart3_Process(void)
{
	u8 sec_king,i;
	u8 kind=0xff;
	u8 lenth = 0;
	u8 recbuf[30];
	u8 str[(FRAME_LEN_MAX-FRAME_LEN_MIN)+1];//收发数据缓冲
	if (ComBuf3.rec.end)//接收数据结束
	{
		memset(str,'\0',(FRAME_LEN_MAX-FRAME_LEN_MIN+1));//清空缓冲
		{
			memcpy(str,&ComBuf3.rec.buf[PDATASTART],(FRAME_LEN_MAX-FRAME_LEN_MIN)+1);//ComBuf3.send.len-FRAME_LEN_MIN);//数据包
			kind=ComBuf3.rec.buf[PFRAMEKIND];//命令字
			sec_king=ComBuf3.rec.buf[PDATASTART];
		}
		//准备接收下一帧数据
		ComBuf3.rec.end=FALSE;//接收缓冲可读标志复位
		ComBuf3.rec.ptr=0;//接收指针清零
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
					case MSG_SHORT://过流报警
						if(U9001_Save_sys.U9001_save.U9001_Setup[U9001_Save_sys.U9001_save.current_step].parameter == IR)
							memcpy(strbuff,"LOW ",5);
						else
							memcpy(strbuff,"HIGH",5);
						break;
					default:
						//kind=0x90;//正常测试
						memcpy(strbuff,"TEST",5);
						break;
				}	
				
				

				if(U9001_Save_sys.U9001_save.all_step == 2)//W-I I-W
				{
//					if(U9001_Save_sys.U9001_save.current_step == 1)
//					{
						memset(ComBuf3.send.buf,0,40);
		//				ComBuf3.send.buf[0]=0xAA;
		//				ComBuf3.send.begin=FALSE;
						if(GetSystemMessage() == MSG_IDLE)
						{
							strcat((char*)ComBuf3.send.buf,(char*)sendbuff);
	//						memcpy(&ComBuf3.send.buf[0],sendbuff,14);
							strcat((char*)ComBuf3.send.buf,(char*)strbuff);
						}else{
							if(U9001_Save_sys.U9001_save.current_step == 1)
							{
								strcat((char*)ComBuf3.send.buf,(char*)sendbuff);
	//							memcpy(&ComBuf3.send.buf[0],sendbuff,14);
								if(U9001_Save_sys.U9001_save.U9001_Setup[2].parameter == IR)
								{
									strcat((char*)ComBuf3.send.buf,(char*)strbuff);
								}else if(U9001_Save_sys.U9001_save.U9001_Setup[1].parameter == IR){
									strcat((char*)ComBuf3.send.buf,"----");
								}
							}else{
								if(U9001_Save_sys.U9001_save.U9001_Setup[2].parameter == IR)
								{
									if(sendbuff[13] == 0x3b)
									{
										memcpy(&sendbuff[14],"PASS",4);
									}else if(sendbuff[14] == 0x3b){
										memcpy(&sendbuff[15],"PASS",4);
									}
//									strcat((char*)sendbuff,"PASS");
									memcpy(&ComBuf3.send.buf[0],sendbuff,18);
								}else if(U9001_Save_sys.U9001_save.U9001_Setup[1].parameter == IR){
//									strcat((char*)sendbuff,"mA;");
									if(sendbuff[13] == 0x3b)
									{
										memcpy(&sendbuff[14],strbuff,4);
									}else if(sendbuff[14] == 0x3b){
										memcpy(&sendbuff[15],strbuff,4);
									}
//									strcat((char*)sendbuff,(char*)strbuff);
									strcat((char*)ComBuf3.send.buf,(char*)sendbuff);
//									memcpy(&ComBuf3.send.buf[0],sendbuff,18);
								}							
							}
						}
	//					memset(ComBuf1.send.buf,0,21);
	//					ComBuf1.send.buf[0]=0xAA;
	//					ComBuf1.send.begin=FALSE;
	//					memcpy(&ComBuf1.send.buf[1],sendbuff1,16);
						
						strcat((char*)ComBuf3.send.buf,";");
						strcat((char*)ComBuf3.send.buf,(char*)sendbuff1);
						if(U9001_Save_sys.U9001_save.current_step == 1)
						{
	//						strcat((char*)ComBuf3.send.buf,"----");
							if(U9001_Save_sys.U9001_save.U9001_Setup[2].parameter == IR)
							{
								strcat((char*)ComBuf3.send.buf,"----");
							}else if(U9001_Save_sys.U9001_save.U9001_Setup[1].parameter == IR){
								strcat((char*)ComBuf3.send.buf,(char*)strbuff);
							}
						}else{
							if(U9001_Save_sys.U9001_save.U9001_Setup[2].parameter == IR)
							{
								strcat((char*)ComBuf3.send.buf,(char*)strbuff);
							}else if(U9001_Save_sys.U9001_save.U9001_Setup[1].parameter == IR){
								strcat((char*)ComBuf3.send.buf,"PASS");
							}
							
						}
						strcat((char*)ComBuf3.send.buf,(char*)sendend);//尾部增加回车和换行符
						for(i=0;i<39;i++)
						{
							UARTPutChar(LPC_UART3, ComBuf3.send.buf[i]);
						}
						ComBuf3.rec.end=FALSE;//接收缓冲可读标志复位
	//					USART_SendData(USART1, 0xBB);
	//					while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);

	//					break;
//					}else{
//						for(i=0;i<39;i++)
//						{
//							UARTPutChar(LPC_UART3, ComBuf3.send.buf[i]);
//						}
//						ComBuf3.rec.end=FALSE;//接收缓冲可读标志复位
//					}
				}else{
//					if(sendflag == 0)
//					{
//						sendflag = 1;
						memset(ComBuf3.send.buf,0,40);
		//				ComBuf3.send.buf[0]=0xAA;
		//				ComBuf3.send.begin=FALSE;
						if(U9001_Save_sys.U9001_save.U9001_Setup[1].parameter == IR)
							memcpy(&ComBuf3.send.buf[0],sendbuff1,14);
						else
							memcpy(&ComBuf3.send.buf[0],sendbuff,14);
						strcat((char*)ComBuf3.send.buf,(char*)strbuff);
						
						strcat((char*)ComBuf3.send.buf,(char*)sendend);//尾部增加回车和换行符
						
						for(i=0;i<20;i++)
						{
							UARTPutChar(LPC_UART3, ComBuf3.send.buf[i]);
						}
//					}else{
//						for(i=0;i<20;i++)
//						{
//							UARTPutChar(LPC_UART3, ComBuf3.send.buf[i]);
//						}
//					}
				}
				

				
				
				
				break;
			
			case FRAME_START://启动
				SetSystemStatus(SYS_STATUS_START);
				SetSystemMessage(MSG_RAMP);
				Uart0_Send(0xa1);
				PLC_OutProg();//开PLC启动
				break;

			case FRAME_RESET://复位
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
					break;
					case SYS_STATUS_START:   
						Beep_Off();
						SetSystemMessage(MSG_PAUSE);//系统信息-暂停测试
						SetSystemStatus(SYS_STATUS_ABORT);//系统状态-暂停测试
						Uart0_Send(0xa0);
					break;
				}
				V_DA_out(0);
				Sing_out_C(0);
				Short_out(0);
				FRB_out(0);
				Uart0_Send(0xa0);
				break;

			case FRAME_WRITE_SN://写序列号
				break;
			
			case FRAME_CLR_BOOT_NUM://清开机次数
				break;
			case FRAME_ITEM://设置测试项目
				switch(sec_king)
				{
					case 0x00:
						U9001_Save_sys.U9001_save.all_step = 1;
						U9001_Save_sys.U9001_save.U9001_Setup[1].parameter = AC;
						break;
					case 0x01:
						U9001_Save_sys.U9001_save.all_step = 1;
						U9001_Save_sys.U9001_save.U9001_Setup[1].parameter = IR;
						break;
					case 0x02:
						U9001_Save_sys.U9001_save.all_step = 2;
						U9001_Save_sys.U9001_save.U9001_Setup[1].parameter = AC;
						U9001_Save_sys.U9001_save.U9001_Setup[2].parameter = IR;
						break;
					case 0x03:
						U9001_Save_sys.U9001_save.all_step = 2;
						U9001_Save_sys.U9001_save.U9001_Setup[1].parameter = IR;
						U9001_Save_sys.U9001_save.U9001_Setup[2].parameter = AC;
						break;
					default:
						break;
				}
				SetDate_Comp();
//				Savetoeeprom();
//				SaveGroup();//保存组别
//				Store_set_flash();
//				Disp_Idle_Menu();//显示待测界面
				break;
			case FRAME_DATA://数据帧
				U9001_Save_sys.U9001_save.current_step = str[1]-1;

				switch(sec_king)
				{
					case 0xAC:
						if(U9001_Save_sys.U9001_save.all_step == 2 && U9001_Save_sys.U9001_save.U9001_Setup[2].parameter == IR)
						{
							U9001_Save_sys.U9001_save.U9001_Setup[1].parameter = AC;
							U9001_Save_sys.U9001_save.U9001_Setup[1].V_out = (u16)BCDtoHex1(str[2],1)*1000+BCDtoHex1(str[3],1)*10;
							U9001_Save_sys.U9001_save.U9001_Setup[1].Upper = (u16)BCDtoHex1(str[4],1)*1000+BCDtoHex1(str[5],1)*10;
							U9001_Save_sys.U9001_save.U9001_Setup[1].lower = (u16)BCDtoHex1(str[6],1)*1000+BCDtoHex1(str[7],1)*10;
							U9001_Save_sys.U9001_save.U9001_Setup[1].rise_time = (u16)BCDtoHex1(str[8],1)*100+BCDtoHex1(str[9],1);
							U9001_Save_sys.U9001_save.U9001_Setup[1].time = (u16)BCDtoHex1(str[10],1)*100+BCDtoHex1(str[11],1);
							U9001_Save_sys.U9001_save.U9001_Setup[1].range_arc = str[12];
							if(str[13]==0x50)
								U9001_Save_sys.U9001_Testconfg.hz=0;
							else
								U9001_Save_sys.U9001_Testconfg.hz=1;
							break;
						}else if(U9001_Save_sys.U9001_save.all_step == 2 && U9001_Save_sys.U9001_save.U9001_Setup[1].parameter == IR){
							U9001_Save_sys.U9001_save.U9001_Setup[2].parameter = AC;
							U9001_Save_sys.U9001_save.U9001_Setup[2].V_out = (u16)BCDtoHex1(str[2],1)*1000+BCDtoHex1(str[3],1)*10;
							U9001_Save_sys.U9001_save.U9001_Setup[2].Upper = (u16)BCDtoHex1(str[4],1)*1000+BCDtoHex1(str[5],1)*10;
							U9001_Save_sys.U9001_save.U9001_Setup[2].lower = (u16)BCDtoHex1(str[6],1)*1000+BCDtoHex1(str[7],1)*10;
							U9001_Save_sys.U9001_save.U9001_Setup[2].rise_time = (u16)BCDtoHex1(str[8],1)*100+BCDtoHex1(str[9],1);
							U9001_Save_sys.U9001_save.U9001_Setup[2].time = (u16)BCDtoHex1(str[10],1)*100+BCDtoHex1(str[11],1);
							U9001_Save_sys.U9001_save.U9001_Setup[2].range_arc = str[12];
							if(str[13]==0x50)
								U9001_Save_sys.U9001_Testconfg.hz=0;
							else
								U9001_Save_sys.U9001_Testconfg.hz=1;
							
						}else if(U9001_Save_sys.U9001_save.all_step == 1){
							U9001_Save_sys.U9001_save.U9001_Setup[1].parameter = AC;
							U9001_Save_sys.U9001_save.U9001_Setup[1].V_out = (u16)BCDtoHex1(str[2],1)*1000+BCDtoHex1(str[3],1)*10;
							U9001_Save_sys.U9001_save.U9001_Setup[1].Upper = (u16)BCDtoHex1(str[4],1)*1000+BCDtoHex1(str[5],1)*10;
							U9001_Save_sys.U9001_save.U9001_Setup[1].lower = (u16)BCDtoHex1(str[6],1)*1000+BCDtoHex1(str[7],1)*10;
							U9001_Save_sys.U9001_save.U9001_Setup[1].rise_time = (u16)BCDtoHex1(str[8],1)*100+BCDtoHex1(str[9],1);
							U9001_Save_sys.U9001_save.U9001_Setup[1].time = (u16)BCDtoHex1(str[10],1)*100+BCDtoHex1(str[11],1);
							U9001_Save_sys.U9001_save.U9001_Setup[1].range_arc = str[12];
							if(str[13]==0x50)
								U9001_Save_sys.U9001_Testconfg.hz=0;
							else
								U9001_Save_sys.U9001_Testconfg.hz=1;
						}
					break;
					case 0xDC:
						if(U9001_Save_sys.U9001_save.all_step == 2 && U9001_Save_sys.U9001_save.U9001_Setup[2].parameter == IR)
						{
							U9001_Save_sys.U9001_save.U9001_Setup[1].parameter = DC;
							U9001_Save_sys.U9001_save.U9001_Setup[1].V_out = (u16)BCDtoHex1(str[2],1)*1000+BCDtoHex1(str[3],1)*10;
							U9001_Save_sys.U9001_save.U9001_Setup[1].Upper = (u16)BCDtoHex1(str[4],1)*1000+BCDtoHex1(str[5],1)*10;
							U9001_Save_sys.U9001_save.U9001_Setup[1].lower = (u16)BCDtoHex1(str[6],1)*1000+BCDtoHex1(str[7],1)*10;
							U9001_Save_sys.U9001_save.U9001_Setup[1].rise_time = (u16)BCDtoHex1(str[8],1)*100+BCDtoHex1(str[9],1);
							U9001_Save_sys.U9001_save.U9001_Setup[1].time = (u16)BCDtoHex1(str[10],1)*100+BCDtoHex1(str[11],1);
							U9001_Save_sys.U9001_save.U9001_Setup[1].range_arc = str[12];

						}else if(U9001_Save_sys.U9001_save.all_step == 2 && U9001_Save_sys.U9001_save.U9001_Setup[1].parameter == IR){
							U9001_Save_sys.U9001_save.U9001_Setup[2].parameter = DC;
							U9001_Save_sys.U9001_save.U9001_Setup[2].V_out = (u16)BCDtoHex1(str[2],1)*1000+BCDtoHex1(str[3],1)*10;
							U9001_Save_sys.U9001_save.U9001_Setup[2].Upper = (u16)BCDtoHex1(str[4],1)*1000+BCDtoHex1(str[5],1)*10;
							U9001_Save_sys.U9001_save.U9001_Setup[2].lower = (u16)BCDtoHex1(str[6],1)*1000+BCDtoHex1(str[7],1)*10;
							U9001_Save_sys.U9001_save.U9001_Setup[2].rise_time = (u16)BCDtoHex1(str[8],1)*100+BCDtoHex1(str[9],1);
							U9001_Save_sys.U9001_save.U9001_Setup[2].time = (u16)BCDtoHex1(str[10],1)*100+BCDtoHex1(str[11],1);
							U9001_Save_sys.U9001_save.U9001_Setup[2].range_arc = str[12];

							
						}else if(U9001_Save_sys.U9001_save.all_step == 1){
							U9001_Save_sys.U9001_save.U9001_Setup[1].parameter = DC;
							U9001_Save_sys.U9001_save.U9001_Setup[1].V_out = (u16)BCDtoHex1(str[2],1)*1000+BCDtoHex1(str[3],1)*10;
							U9001_Save_sys.U9001_save.U9001_Setup[1].Upper = (u16)BCDtoHex1(str[4],1)*1000+BCDtoHex1(str[5],1)*10;
							U9001_Save_sys.U9001_save.U9001_Setup[1].lower = (u16)BCDtoHex1(str[6],1)*1000+BCDtoHex1(str[7],1)*10;
							U9001_Save_sys.U9001_save.U9001_Setup[1].rise_time = (u16)BCDtoHex1(str[8],1)*100+BCDtoHex1(str[9],1);
							U9001_Save_sys.U9001_save.U9001_Setup[1].time = (u16)BCDtoHex1(str[10],1)*100+BCDtoHex1(str[11],1);
							U9001_Save_sys.U9001_save.U9001_Setup[1].range_arc = str[12];

						}

//						SaveData.Setup.Item=1;
//						SaveData.Setup.Output=(u16)BCDtoHex1(str[2],1)*100+BCDtoHex1(str[3],1);
//						SaveData.Setup.High=(u16)BCDtoHex1(str[4],1)*100+BCDtoHex1(str[5],1);
//						SaveData.Setup.Low=(u16)BCDtoHex1(str[6],1)*100+BCDtoHex1(str[7],1);
//						SaveData.Setup.RampDelay=(u16)BCDtoHex1(str[8],1)*100+BCDtoHex1(str[9],1);
//						SaveData.Setup.TestTime=(u16)BCDtoHex1(str[10],1)*100+BCDtoHex1(str[11],1);
//						SaveData.Setup.Arc=str[12];
						break;
					case 0xAD:
						if(U9001_Save_sys.U9001_save.all_step == 2 && U9001_Save_sys.U9001_save.U9001_Setup[2].parameter == IR)
						{
							U9001_Save_sys.U9001_save.U9001_Setup[2].parameter = IR;
							U9001_Save_sys.U9001_save.U9001_Setup[2].V_out = (u16)BCDtoHex1(str[2],1)*1000+BCDtoHex1(str[3],1)*10;
							U9001_Save_sys.U9001_save.U9001_Setup[2].Upper = (u16)BCDtoHex1(str[4],1)*100+BCDtoHex1(str[5],1)*1;
							U9001_Save_sys.U9001_save.U9001_Setup[2].lower = (u16)BCDtoHex1(str[6],1)*100+BCDtoHex1(str[7],1)*1;
							U9001_Save_sys.U9001_save.U9001_Setup[2].time = (u16)BCDtoHex1(str[8],1)*100+BCDtoHex1(str[9],1);
						}else if(U9001_Save_sys.U9001_save.all_step == 2 && U9001_Save_sys.U9001_save.U9001_Setup[1].parameter == IR){
							U9001_Save_sys.U9001_save.U9001_Setup[1].parameter = IR;
							U9001_Save_sys.U9001_save.U9001_Setup[1].V_out = (u16)BCDtoHex1(str[2],1)*1000+BCDtoHex1(str[3],1)*10;
							U9001_Save_sys.U9001_save.U9001_Setup[1].Upper = (u16)BCDtoHex1(str[4],1)*100+BCDtoHex1(str[5],1)*1;
							U9001_Save_sys.U9001_save.U9001_Setup[1].lower = (u16)BCDtoHex1(str[6],1)*100+BCDtoHex1(str[7],1)*1;
							U9001_Save_sys.U9001_save.U9001_Setup[1].time = (u16)BCDtoHex1(str[8],1)*100+BCDtoHex1(str[9],1);							
						}else if(U9001_Save_sys.U9001_save.all_step == 1){
							U9001_Save_sys.U9001_save.U9001_Setup[1].parameter = IR;
							U9001_Save_sys.U9001_save.U9001_Setup[1].V_out = (u16)BCDtoHex1(str[2],1)*1000+BCDtoHex1(str[3],1)*10;
							U9001_Save_sys.U9001_save.U9001_Setup[1].Upper = (u16)BCDtoHex1(str[4],1)*100+BCDtoHex1(str[5],1)*1;
							U9001_Save_sys.U9001_save.U9001_Setup[1].lower = (u16)BCDtoHex1(str[6],1)*100+BCDtoHex1(str[7],1)*1;
							U9001_Save_sys.U9001_save.U9001_Setup[1].time = (u16)BCDtoHex1(str[8],1)*100+BCDtoHex1(str[9],1);

						}
//					SaveData.Setup.I_Volt=(u16)BCDtoHex1(str[2],1)*100+BCDtoHex1(str[3],1);
//						SaveData.Setup.I_High=(u16)BCDtoHex1(str[4],1)*100+BCDtoHex1(str[5],1);
//						SaveData.Setup.I_Low=(u16)BCDtoHex1(str[6],1)*100+BCDtoHex1(str[7],1);
//						SaveData.Setup.I_Delay=(u16)BCDtoHex1(str[8],1)*100+BCDtoHex1(str[9],1);
//						
						break;
					
				
				}
				SetDate_Comp();
//				Savetoeeprom();
				break;
			case FRAME_SELECT_GROUP:
//				SaveData.Group=sec_king-1;
//				SaveGroup();
//				SetSystemStatus(SYS_STATUS_IDLE);//待机状态
//				ReadSetByGroup();
//				Parameter_valuecomp();//比较设置参数
//				Disp_Idle_Menu();//显示待测界面
				break;		
			default:
				break;
			
		}
//	Disp_Idle_Menu();//显示待测界面
	}
}


/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
/*********************************************************************//**
 * @brief		UART0 interrupt handler sub-routine
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void UART0_IRQHandler(void)//UART0_IRQn
{
	 static vu8 count = 0;
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
//	Res=UART_ReceiveByte(LPC_UART3);
	if(U9001_Save_sys.U9001_SYS.bussmode == 0)
	{
		
    g_mods_timeout = 2;
		if (g_tModS.RxCount < S_RX_BUF_SIZE)
		{
			g_tModS.RxBuf[g_tModS.RxCount++] = UART_ReceiveByte(LPC_UART3);
		}
		if(g_tModS.RxCount > 7 && g_tModS.RxBuf[1] == 6)
		{
			MODS_SendAckOk();
		}
	}else if(U9001_Save_sys.U9001_SYS.bussmode == 1){
		Res=UART_ReceiveByte(LPC_UART3);
		SCPI_Input(&scpi_context, &Res, 1);
	}else if(U9001_Save_sys.U9001_SYS.bussmode == 2){
		if (!ComBuf3.rec.end)//接收没结束
		{
			SetRecTimeOut(REC_TIME_OUT);//设置接收超时周期
			Res=UART_ReceiveByte(LPC_UART3);
			if (/*dat==(u8)(UART_REC_BEGIN)*/ComBuf3.rec.ptr==0)//帧头
			{
				if(Res!=(u8)(UART_REC_BEGIN)) //首字节
				{
					ComBuf3.rec.ptr=0;//重新接收 
				}
				else
				{
					ComBuf3.rec.buf[ComBuf3.rec.ptr++]=Res;
				}
			}
			else if (Res==(u8)(UART_REC_END))//帧尾
			{
				ComBuf3.rec.buf[ComBuf3.rec.ptr++]=Res;
				ComBuf3.rec.end=TRUE;//接收结束
				ComBuf3.rec.len=ComBuf3.rec.ptr;//存接收数据长度
				ComBuf3.rec.ptr=0;//指针清零重新开始新的一帧接收
				ComBuf3.rec.TimeOut=0;//接收超时清零
			}
			else
			{
				if (ComBuf3.rec.ptr>=REC_LEN_MAX)//最大接收帧长度
				{
					ComBuf3.rec.ptr=0;//重新接收
				}
				else
				{
					ComBuf3.rec.buf[ComBuf3.rec.ptr++]=Res;
				}
			}
		}
		if(ComBuf3.rec.end == TRUE)
		{
			Uart3_Process();//串口处理
		}
	}
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

void RecHandle(void)
{
    u8 crec[6];
    u8 *csend;
    static u8 *sendbuf;
    u8 sendlen;
    static u16 recrc;
    static u16 scrc;
    u8 i;
    switch(g_tModS.RxBuf[1])
    {
        case 0x03:
        {
            MODS_03H();
        }break;
        case 0x06:
        {
            MODS_06H();
        }break;

        default:break;
    }
}
/*
*********************************************************************************************************
*	函 数 名: MODS_ReadRegValue
*	功能说明: 读取保持寄存器的值
*	形    参: reg_addr 寄存器地址
*			  reg_value 存放寄存器结果
*	返 回 值: 1表示OK 0表示错误
*********************************************************************************************************
*/
static uint8_t MODS_ReadRegValue(uint16_t reg_addr, uint8_t *reg_value)
{
    uint16_t value;
	switch (reg_addr)									/* 判断寄存器地址 */
	{
        case SLAVE_REG_P00:
//			C_SW(1);
			value=U9001_Save_sys.U9001_save.U9001_Setup[1].parameter+1;
			break;
		case SLAVE_REG_P01:
			value=Save_TestValue[0].Text_vot;
			break;

		case SLAVE_REG_P02:
			value=Save_TestValue[0].Text_value;
			break;
		case SLAVE_REG_P03: 
			value=Save_TestValue[0].text_unit+1;
			break;

		case SLAVE_REG_P04:
			value=Save_TestValue[0].text_flag;
			break;
		case SLAVE_REG_P05:
//			C_SW(1);
			value=U9001_Save_sys.U9001_save.U9001_Setup[2].parameter+1;
			break;
		case SLAVE_REG_P06:
			value=Save_TestValue[1].Text_vot;
			break;

		case SLAVE_REG_P07:
			value=Save_TestValue[1].Text_value;
			break;
		case SLAVE_REG_P08: 
			value=Save_TestValue[1].text_unit+1;
			break;

		case SLAVE_REG_P09:
			value=Save_TestValue[1].text_flag;
			break;
		case SLAVE_REG_P10:
//			C_SW(1);
			value=U9001_Save_sys.U9001_save.U9001_Setup[3].parameter+1;
			break;
		case SLAVE_REG_P11:
			value=Save_TestValue[2].Text_vot;
			break;

		case SLAVE_REG_P12:
			value=Save_TestValue[2].Text_value;
			break;
		case SLAVE_REG_P13: 
			value=Save_TestValue[2].text_unit+1;
			break;

		case SLAVE_REG_P14:
			value=Save_TestValue[2].text_flag;
			break;
		case SLAVE_REG_P15:
//			C_SW(1);
			value=U9001_Save_sys.U9001_save.U9001_Setup[4].parameter+1;
			break;
		case SLAVE_REG_P16:
			value=Save_TestValue[3].Text_vot;
			break;

		case SLAVE_REG_P17:
			value=Save_TestValue[3].Text_value;
			break;
		case SLAVE_REG_P18: 
			value=Save_TestValue[3].text_unit+1;
			break;

		case SLAVE_REG_P19:
			value=Save_TestValue[3].text_flag;
			break;
		case SLAVE_REG_P20:
//			C_SW(1);
			value=U9001_Save_sys.U9001_save.U9001_Setup[5].parameter+1;
			break;
		case SLAVE_REG_P21:
			value=Save_TestValue[4].Text_vot;
			break;

		case SLAVE_REG_P22:
			value=Save_TestValue[4].Text_value;
			break;
		case SLAVE_REG_P23: 
			value=Save_TestValue[4].text_unit+1;
			break;

		case SLAVE_REG_P24:
			value=Save_TestValue[4].text_flag;
			break;
		case SLAVE_REG_P25:
//			C_SW(1);
			value=U9001_Save_sys.U9001_save.U9001_Setup[6].parameter+1;
			break;
		case SLAVE_REG_P26:
			value=Save_TestValue[5].Text_vot;
			break;

		case SLAVE_REG_P27:
			value=Save_TestValue[5].Text_value;
			break;
		case SLAVE_REG_P28: 
			value=Save_TestValue[5].text_unit+1;
			break;

		case SLAVE_REG_P29:
			value=Save_TestValue[5].text_flag;
			break;
		case SLAVE_REG_P30:
//			C_SW(1);
			value=U9001_Save_sys.U9001_save.U9001_Setup[7].parameter+1;
			break;
		case SLAVE_REG_P31:
			value=Save_TestValue[6].Text_vot;
			break;

		case SLAVE_REG_P32:
			value=Save_TestValue[6].Text_value;
			break;
		case SLAVE_REG_P33: 
			value=Save_TestValue[6].text_unit+1;
			break;

		case SLAVE_REG_P34:
			value=Save_TestValue[6].text_flag;
			break;
		case SLAVE_REG_P35:
//			C_SW(1);
			value=U9001_Save_sys.U9001_save.U9001_Setup[8].parameter+1;
			break;
		case SLAVE_REG_P36:
			value=Save_TestValue[7].Text_vot;
			break;

		case SLAVE_REG_P37:
			value=Save_TestValue[7].Text_value;
			break;
		case SLAVE_REG_P38: 
			value=Save_TestValue[7].text_unit+1;
			break;

		case SLAVE_REG_P39:
			value=Save_TestValue[7].text_flag;
			break;
		case SLAVE_REG_P40:
//			C_SW(1);
			value=U9001_Save_sys.U9001_save.U9001_Setup[9].parameter+1;
			break;
		case SLAVE_REG_P41:
			value=Save_TestValue[8].Text_vot;
			break;

		case SLAVE_REG_P42:
			value=Save_TestValue[8].Text_value;
			break;
		case SLAVE_REG_P43: 
			value=Save_TestValue[8].text_unit+1;
			break;

		case SLAVE_REG_P44:
			value=Save_TestValue[8].text_flag;
			break;
		case SLAVE_REG_P45:
//			C_SW(1);
			value=U9001_Save_sys.U9001_save.U9001_Setup[10].parameter+1;
			break;
		case SLAVE_REG_P46:
			value=Save_TestValue[9].Text_vot;
			break;

		case SLAVE_REG_P47:
			value=Save_TestValue[9].Text_value;
			break;
		case SLAVE_REG_P48: 
			value=Save_TestValue[9].text_unit+1;
			break;

		case SLAVE_REG_P49:
			value=Save_TestValue[9].text_flag;
			break;
		case SLAVE_REG_P50: 
			value=testflag;
			break;

		case SLAVE_REG_P51:
			value=!testflag;
			break;
		default:
			return 0;
    }
	reg_value[0] = value >> 8;
	reg_value[1] = value;

	return 1;											/* 读取成功 */
}

/*
*********************************************************************************************************
*	函 数 名: MODS_WriteRegValue
*	功能说明: 读取保持寄存器的值
*	形    参: reg_addr 寄存器地址
*			  reg_value 寄存器值
*	返 回 值: 1表示OK 0表示错误
*********************************************************************************************************
*/
static uint8_t MODS_WriteRegValue(uint16_t reg_addr, uint16_t reg_value)
{

	switch (reg_addr)							/* 判断寄存器地址 */
	{	
		case SLAVE_REG_P50: 
			if(reg_value == 1)
			{
				SetSystemStatus(SYS_STATUS_START);
				SetSystemMessage(MSG_RAMP);
				Uart0_Send(0xa1);
				PLC_OutProg();//开PLC启动
			}
			break;

		case SLAVE_REG_P51:
			if(reg_value == 1)
			{
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
					break;
					case SYS_STATUS_START:   
						Beep_Off();
						SetSystemMessage(MSG_PAUSE);//系统信息-暂停测试
						SetSystemStatus(SYS_STATUS_ABORT);//系统状态-暂停测试
						Uart0_Send(0xa0);
					break;
				}
				V_DA_out(0);
				Sing_out_C(0);
				Short_out(0);
				FRB_out(0);
				Uart0_Send(0xa0);
			}
			break;
		default:
			return 0;
	}

	return 1;
}

static void MODS_03H(void)
{
    uint16_t reg;
	uint16_t num;
	uint16_t i;
	uint8_t reg_value[64];

    
    g_tModS.RspCode = RSP_OK;

	if (g_tModS.RxCount != 8)								/* 03H命令必须是8个字节 */
	{
		g_tModS.RspCode = RSP_ERR_VALUE;					/* 数据值域错误 */
		goto err_ret;
	}

	reg = BEBufToUint16(&g_tModS.RxBuf[2]); 				/* 寄存器号 */
	num = BEBufToUint16(&g_tModS.RxBuf[4]);					/* 寄存器个数 */
	if (num > sizeof(reg_value) / 2)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;					/* 数据值域错误 */
		goto err_ret;
	}

	for (i = 0; i < num; i++)
	{
		if (MODS_ReadRegValue(reg, &reg_value[2 * i]) == 0)	/* 读出寄存器值放入reg_value */
		{
			g_tModS.RspCode = RSP_ERR_REG_ADDR;				/* 寄存器地址错误 */
			break;
		}
		reg++;
	}

err_ret:
	if (g_tModS.RspCode == RSP_OK)							/* 正确应答 */
	{
		g_tModS.TxCount = 0;
		g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[0];
		g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[1];
		g_tModS.TxBuf[g_tModS.TxCount++] = num * 2;			/* 返回字节数 */

		for (i = 0; i < num; i++)
		{
			g_tModS.TxBuf[g_tModS.TxCount++] = reg_value[2*i];
			g_tModS.TxBuf[g_tModS.TxCount++] = reg_value[2*i+1];
		}
//         if(usartocflag == 0)
//         {
             MODS_SendWithCRC(g_tModS.TxBuf, g_tModS.TxCount);	/* 发送正确应答 */
//         }
	}
	else
	{
//		MODS_SendAckErr(g_tModS.RspCode);					/* 发送错误应答 */
	}
    
}

static void MODS_06H(void)
{
    uint16_t reg;
	uint16_t value;

	g_tModS.RspCode = RSP_OK;

	if (g_tModS.RxCount != 8)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* 数据值域错误 */
		goto err_ret;
	}

	reg = BEBufToUint16(&g_tModS.RxBuf[2]); 	/* 寄存器号 */
	value = BEBufToUint16(&g_tModS.RxBuf[4]);	/* 寄存器值 */
    
//    if(reg == 0x0E)
//    {
//        if(value == 00)
//        {
//            GPIO_SetBits(GPIOC,GPIO_Pin_10);//CC
//            flag_Load_CC = 1;
//        }else if(value == 01){
//            GPIO_ResetBits(GPIOC,GPIO_Pin_10);//CV
//            flag_Load_CC = 0;
//        }
//    }
 	if (MODS_WriteRegValue(reg, value) == 1)	/* 该函数会把写入的值存入寄存器 */
 	{
 		;
 	}
 	else
 	{
 		g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* 寄存器地址错误 */
 	}

err_ret:
	if (g_tModS.RspCode == RSP_OK)				/* 正确应答 */
	{
		
	}
	else
	{
		MODS_SendAckErr(g_tModS.RspCode);		/* 告诉主机命令错误 */
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

