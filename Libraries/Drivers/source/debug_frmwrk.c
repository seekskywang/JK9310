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
//Com_TypeDef ComBuf;//�����շ�����

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
//�������ƣ�Uart0RecTimeOut
//�������ܣ�����0���ճ�ʱ�б�
//��ڲ�������
//���ڲ�������
//�������ڣ�2014.04.07
//�޸����ڣ�2014.04.07
//��ע˵������Ҫ��ϵͳʱ��10mS��ʱ�е���
//==========================================================
void Uart0RecTimeOut(void)
{
	if (ComBuf.rec.TimeOut)//���ڽ��ճ�ʱ��ʱ
	{
		ComBuf.rec.TimeOut--;
	}
	else//���ճ�ʱ��������ǰ֡
	{
		if (ComBuf.rec.ptr)//��ǰ����ָ�벻Ϊ��
		{
			ComBuf.rec.end=TRUE;//���ݽ��ս���
			ComBuf.rec.len=ComBuf.rec.ptr;//�������ݻ��峤��
			ComBuf.rec.ptr=0;
		}
	}
}

//==========================================================
//�������ƣ�Uart_Process
//�������ܣ����ڴ��� ������
//��ڲ�������
//���ڲ�������
//�������ڣ�2015.10.26 
//�޸����ڣ�2015.10.26 10:02
//��ע˵������
//==========================================================
//vu8 Uart_Process(void)
//{
//	vu8 kind=0xff;
//    Send_Testvalue_Typedef *pt;
//		if (ComBuf.rec.end)//�������ݽ���
//		{
//				kind=ComBuf.rec.buf[PFRAMEKIND];//������
//			//׼��������һ֡����sprintf
//			ComBuf.rec.end=0;//���ջ���ɶ���־��λ
//			ComBuf.rec.ptr=0;//����ָ������
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
	 u8 str[(FRAME_LEN_MAX-FRAME_LEN_MIN)+1];//�շ����ݻ���
#else
	 u8 str[(FRAME_LEN_MAX-FRAME_LEN_MIN)+1];//�շ����ݻ���
#endif
if(FacBuf.rec.end)//ͨѶЭ��ѡ��
{
	U9001_Save_sys.U9001_SYS.pselect	= FacBuf.rec.buf[2];
//	SaveData.devaddr = FacBuf.rec.buf[3];
	memset(FacBuf.rec.buf,'\0',30);//��ջ���
	FacBuf.rec.end=FALSE;//���ջ���ɶ���־��λ
	FacBuf.rec.ptr=0;//����ָ������
	
	SavePselect();
//	Saveaddr();
}
if(U9001_Save_sys.U9001_SYS.pselect == 0)//ͨѶЭ��1
{
	//	if(SaveData.System.Uart)//������Ч
	//	{
			if (ComBuf.rec.end)//�������ݽ���
			{
				memset(str,'\0',(FRAME_LEN_MAX-FRAME_LEN_MIN+1));//��ջ���
				{
					memcpy(str,&ComBuf.rec.buf[PDATASTART],(FRAME_LEN_MAX-FRAME_LEN_MIN)+1);//ComBuf.send.len-FRAME_LEN_MIN);//���ݰ�
					kind=ComBuf.rec.buf[PFRAMEKIND];//������
					sec_king=ComBuf.rec.buf[PDATASTART];
				}
				//׼��������һ֡����
				ComBuf.rec.end=FALSE;//���ջ���ɶ���־��λ
				ComBuf.rec.ptr=0;//����ָ������
	//		}
	//	}

		switch(kind)
		{
			case FRAME_READ_RESULT://��ȡ���
				//���ڷ��Ͳ�������:��ѹ(5)+����(6)+ʱ��(4)+��ѡ(1)=16�ֽ�
				switch (GetSystemMessage())//ϵͳ��Ϣ
				{
					case MSG_IDLE:
						memcpy(strbuff,"----",5);
						break;
					case MSG_PAUSE:
						memcpy(strbuff,"----",5);
						//kind=0x9B;//������ֹ
						break;
					case MSG_PASS:
						//kind=0x91;//����ͨ��
						memcpy(strbuff,"PASS",5);
						break;
					case MSG_HIGH:
						//kind=0x92;//���ޱ���
						memcpy(strbuff,"HIGH",5);
						break;
					case MSG_LOW:
						//kind=0x92;//���ޱ���
						memcpy(strbuff,"LOW ",5);
						break;
					case MSG_OVER:
						memcpy(strbuff,"BRK ",5);
						break;
					case MSG_ARC:
						memcpy(strbuff,"ARC ",5);
						break;
					case MSG_OFL://��������
						memcpy(strbuff,"HIGH",5);
						break;
					default:
						//kind=0x90;//��������
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
						strcat((char*)ComBuf.send.buf,(char*)sendend);//β�����ӻس��ͻ��з�
						for(i=0;i<39;i++)
						{
							USART_SendData(USART1, ComBuf.send.buf[i]);
							while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
						}
						ComBuf.rec.end=FALSE;//���ջ���ɶ���־��λ
	//					USART_SendData(USART1, 0xBB);
	//					while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);

	//					break;
					}else{
						for(i=0;i<39;i++)
						{
							USART_SendData(USART1, ComBuf.send.buf[i]);
							while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
						}
						ComBuf.rec.end=FALSE;//���ջ���ɶ���־��λ
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
						
						strcat((char*)ComBuf.send.buf,(char*)sendend);//β�����ӻس��ͻ��з�
						
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
			
			case FRAME_START://����
				run_stemp=0;
				SetSystemStatus(SYS_STATUS_START);//ϵͳ״̬-��������
				break;

			case FRAME_RESET://��λ
				if(GetSystemStatus() == SYS_STATUS_TEST){
					SetSystemMessage(MSG_PAUSE);//ϵͳ��Ϣ-��ͣ����
					SetSystemStatus(SYS_STATUS_TEST_PAUSE);//ϵͳ״̬-����
					Plc_Start_Off();
				}else{
					SetSystemStatus(SYS_STATUS_IDLE);//ϵͳ״̬-����
				}
				break;

			case FRAME_WRITE_SN://д���к�
				break;
			
			case FRAME_CLR_BOOT_NUM://�忪������
				break;
			case FRAME_ITEM://���ò�����Ŀ
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
				SaveGroup();//�������
				Store_set_flash();
				Disp_Idle_Menu();//��ʾ�������
				break;
			case FRAME_DATA://����֡
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
				SaveGroup();//�������
				Store_set_flash();
				Disp_Idle_Menu();//��ʾ�������
				break;
			case FRAME_SELECT_GROUP:
				SaveData.Group=sec_king-1;
				SaveGroup();
				SetSystemStatus(SYS_STATUS_IDLE);//����״̬
				ReadSetByGroup();
				Parameter_valuecomp();//�Ƚ����ò���
				Disp_Idle_Menu();//��ʾ�������
				break;		
			default:
				break;
			
		}
//	Disp_Idle_Menu();//��ʾ�������
	}
}else if(SaveData.pselect == 1){//ͨѶЭ��2
	if (ComBuf.rec.end)//�������ݽ���
	{
		memcpy(recbuf,&ComBuf.rec.buf[0],30);
		lenth=recbuf[2];//���ݳ���
		kind=recbuf[3];//������
		ComBuf.rec.end=FALSE;//���ջ���ɶ���־��λ
		ComBuf.rec.ptr=0;//����ָ������
	}
	
	switch(kind)
	{
		case TEST_RST://����/��λ����
		{
			if(lenth == 2)
			{
				if(recbuf[4] == 'T')//��������
				{
					run_stemp=0;
					SetSystemStatus(SYS_STATUS_START);//ϵͳ״̬-��������
					respond = 1;
				}else if(recbuf[4] == 'R'){//��λ����
					if(GetSystemStatus() == SYS_STATUS_TEST){
						SetSystemMessage(MSG_PAUSE);//ϵͳ��Ϣ-��ͣ����
						SetSystemStatus(SYS_STATUS_TEST_PAUSE);//ϵͳ״̬-����
						Plc_Start_Off();
						respond = 1;
					}else{
						SetSystemStatus(SYS_STATUS_IDLE);//ϵͳ״̬-����
					}
				}else if(recbuf[4] == 'S'){//����������
					SendRes();
				}
			}else if(lenth == 3){
				if(recbuf[4] == 't')//ѡ����𲢲�������
				{
					run_stemp=0;
					SaveData.Group=recbuf[5];
					SaveGroup();
					SetSystemStatus(SYS_STATUS_IDLE);//����״̬
					ReadSetByGroup();
					Parameter_valuecomp();//�Ƚ����ò���					
					SetSystemStatus(SYS_STATUS_START);//ϵͳ״̬-��������
				}
			}
		}break;
		case GROUPSET://�������
		{
			SaveData.Group=recbuf[4];
			SaveGroup();
			SetSystemStatus(SYS_STATUS_IDLE);//����״̬
			ReadSetByGroup();
			Parameter_valuecomp();//�Ƚ����ò���
			Disp_Idle_Menu();//��ʾ�������
		}break;
		case BASICSET:	//����������
		{
			U2.w_ma = recbuf[4];
			SaveData.Setup.Group_Item = U2.BIT_FLAG.item;//������Ŀ
			SaveData.Setup.Item = U2.BIT_FLAG.ACDC;//��ֱ��
			SaveData.Setup.Freq = U2.BIT_FLAG.ACWF;//Ƶ��
			SaveData.Setup.Arc = U2.BIT_FLAG.ARC;//�绡
			
			SaveData.Setup.I_Volt = recbuf[5];//��Ե���������ѹ
			SaveData.Setup.Output = (u16)(recbuf[6]<<8) + recbuf[7];//��ѹ���������ѹ
			SaveData.Setup.High=(u16)(recbuf[8]<<8) + recbuf[9];//��ѹ����©������
			SaveData.Setup.Low=(u16)(recbuf[10]<<8) + recbuf[11];//��ѹ����©������
			SaveData.Setup.RampDelay=(u16)(recbuf[12]<<8) + recbuf[13];//��ѹ����ʱ��
			SaveData.Setup.TestTime=(u16)(recbuf[14]<<8) + recbuf[15];//��ѹ����ʱ��
			SaveData.Setup.I_High=(u16)(recbuf[16]<<8) + recbuf[17];//��Ե����
			SaveData.Setup.I_Low=(u16)(recbuf[18]<<8) + recbuf[19];//��Ե����
			SaveData.Setup.I_Delay=(u16)(recbuf[20]<<8) + recbuf[21];//��ʱ�ж�
			
			SaveGroup();//�������
			Store_set_flash();
			Disp_Idle_Menu();//��ʾ�������
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
	if(U9001_Save_sys.U9001_SYS.pselect == 0)//ԭʼЭ��
	{
		if (!ComBuf.rec.end)//����û����
		{
			SetRecTimeOut(REC_TIME_OUT);//���ý��ճ�ʱ����
			dat=Status;
			if (/*dat==(u8)(UART_REC_BEGIN)*/ComBuf.rec.ptr==0)//֡ͷ
			{
				if(dat!=(u8)(UART_REC_BEGIN)) //���ֽ�
				{
					ComBuf.rec.ptr=0;//���½��� 
				}
				else
				{
					ComBuf.rec.buf[ComBuf.rec.ptr++]=dat;
				}
			}
			else if (dat==(u8)(UART_REC_END))//֡β
			{
				ComBuf.rec.buf[ComBuf.rec.ptr++]=dat;
				ComBuf.rec.end=TRUE;//���ս���
				ComBuf.rec.len=ComBuf.rec.ptr;//��������ݳ���
				ComBuf.rec.ptr=0;//ָ���������¿�ʼ�µ�һ֡����
				ComBuf.rec.TimeOut=0;//���ճ�ʱ����
			}
			else
			{
				if (ComBuf.rec.ptr>=REC_LEN_MAX)//������֡����
				{
					ComBuf.rec.ptr=0;//���½���
				}
				else
				{
					ComBuf.rec.buf[ComBuf.rec.ptr++]=dat;
				}
			}
		}
	}else if(U9001_Save_sys.U9001_SYS.pselect == 1){//��Э��
		if (!ComBuf.rec.end)//����û����
			{
				SetRecTimeOut(REC_TIME_OUT);//���ý��ճ�ʱ����
				dat=Status;
				if (/*dat==(u8)(UART_REC_BEGIN20) && */ComBuf.rec.ptr == 0)//֡ͷ1
				{
					if(dat!=(u8)(UART_REC_BEGIN20)) //���ֽ�
					{
						ComBuf.rec.ptr=0;//���½��� 
					}
					else
					{
						ComBuf.rec.buf[ComBuf.rec.ptr++]=dat;
					}
				}
				else if (/*dat==SaveData.devaddr(u8)(UART_REC_BEGIN21) && */ComBuf.rec.ptr == 1)//֡ͷ2
				{
					if(dat!=UART_REC_BEGIN21) //�ڶ��ֽ�
					{
						ComBuf.rec.ptr=0;//���½��� 
					}
					else
					{
						ComBuf.rec.buf[ComBuf.rec.ptr++]=dat;
					}
				}else{
					if (ComBuf.rec.ptr>=REC_LEN_MAX)//������֡����
					{
						ComBuf.rec.ptr=0;//���½���
					}
					else
					{
						ComBuf.rec.buf[ComBuf.rec.ptr++]=dat;
						if(ComBuf.rec.ptr > 2 && (ComBuf.rec.ptr == ComBuf.rec.buf[2]+3))
						{
//							ComBuf.rec.buf[ComBuf.rec.ptr++]=dat;
							ComBuf.rec.end=TRUE;//���ս���
							ComBuf.rec.len=ComBuf.rec.ptr;//��������ݳ���
							ComBuf.rec.ptr=0;//ָ���������¿�ʼ�µ�һ֡����
							ComBuf.rec.TimeOut=0;//���ճ�ʱ����
						}
					}
				}
			}
	}
	
	
	//����Э����������
		if(!FacBuf.rec.end)
		{
			SetRecTimeOut(REC_TIME_OUT);//���ý��ճ�ʱ����
			dat=USART_ReceiveData(USART1);
			if (/*dat==0XFF*/FacBuf.rec.ptr==0)//֡ͷ1
			{
				if(dat!=0XFF) //���ֽ�
				{
					FacBuf.rec.ptr=0;//���½��� 
				}
				else
				{
					FacBuf.rec.buf[FacBuf.rec.ptr++]=dat;
				}
			}
			else if (/*dat==0XEE*/FacBuf.rec.ptr==1)//֡ͷ2
			{
				if(dat!=0XEE) //�ڶ��ֽ�
				{
					FacBuf.rec.ptr=0;//���½��� 
				}
				else
				{
					FacBuf.rec.buf[FacBuf.rec.ptr++]=dat;
				}
			}else if (/*dat==0XDD*/FacBuf.rec.ptr==4)//֡β
			{
				if(dat!=0XDD) //�����ֽ�
				{
					FacBuf.rec.ptr=0;//���½��� 
				}else{
					FacBuf.rec.buf[FacBuf.rec.ptr++]=dat;
					FacBuf.rec.end=TRUE;//���ս���
					FacBuf.rec.len=FacBuf.rec.ptr;//��������ݳ���
					FacBuf.rec.ptr=0;//ָ���������¿�ʼ�µ�һ֡����
					FacBuf.rec.TimeOut=0;//���ճ�ʱ����	
				}					
				
			}
			else
			{
				if (FacBuf.rec.ptr>=REC_LEN_MAX)//������֡����
				{
					FacBuf.rec.ptr=0;//���½���
				}
				else
				{
					FacBuf.rec.buf[FacBuf.rec.ptr++]=dat;
				}
			}
		}
			
	   }
	if (!ComBuf.rec.end)//����û����
		{
			SetRecTimeOut(REC_TIME_OUT);//���ý��ճ�ʱ����
			dat=Status;
			if (dat==(vu8)(UART_REC_BEGIN))//֡ͷ
			{
				if(ComBuf.rec.ptr!=0) //���ֽ�
				{
					ComBuf.rec.ptr=0;//���½��� 
				}
				else
				{
					ComBuf.rec.buf[ComBuf.rec.ptr++]=dat;
				}
			}
			else if (dat==(vu8)(UART_REC_END))//֡β
			{
				ComBuf.rec.buf[ComBuf.rec.ptr++]=dat;
				ComBuf.rec.end=TRUE;//���ս���
				ComBuf.rec.len=ComBuf.rec.ptr;//��������ݳ���
				ComBuf.rec.ptr=0;//ָ���������¿�ʼ�µ�һ֡����
				ComBuf.rec.TimeOut=0;//���ճ�ʱ����
                keyvalue=Uart_Process();
			}
			else
			{
				if (ComBuf.rec.ptr>=REC_LEN_MAX)//������֡����
				{
					ComBuf.rec.ptr=0;//���½���
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
//    if (!ComBuf.rec.end)//����û����
//    {
//        SetRecTimeOut(REC_TIME_OUT);//���ý��ճ�ʱ����
//        dat=Status;
//        if (dat==(vu8)(UART_REC_BEGIN))//֡ͷ
//        {
//            if(ComBuf3.rec.ptr!=0) //���ֽ�
//            {
//                ComBuf3.rec.ptr=0;//���½��� 
//            }
//            else
//            {
//                ComBuf3.rec.buf[ComBuf3.rec.ptr++]=dat;
//            }
//        }
//        else if (dat==(vu8)(UART_REC_END))//֡β
//        {
//            ComBuf3.rec.buf[ComBuf3.rec.ptr++]=dat;
//            ComBuf3.rec.end=TRUE;//���ս���
//            ComBuf3.rec.len=ComBuf3.rec.ptr;//��������ݳ���
//            ComBuf3.rec.ptr=0;//ָ���������¿�ʼ�µ�һ֡����
//            ComBuf3.rec.TimeOut=0;//���ճ�ʱ����
////           Uart3_Process();
//        }
//        else
//        {
//            if (ComBuf3.rec.ptr>=REC_LEN_MAX)//������֡����
//            {
//                ComBuf3.rec.ptr=0;//���½���
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
//�������ƣ�SendDataToCom
//�������ܣ����ʹ�������
//��ڲ�������
//���ڲ�����True(����)/False
//�������ڣ�2014.04.08 
//�޸����ڣ�2014.04.08 12:50
//��ע˵���������ڷ��ͽṹ����
//==========================================================
//bool SendDataToCom(void)
//{
//	if ((!ComBuf.send.begin)&&ComBuf.send.len)//�������ҷ���û��ʼ
//		{
//			ComBuf.send.begin=TRUE;//�÷���æ��־
//			ComBuf.send.ptr=0;//��ǰָ������
//			SBUF=ComBuf.send.buf[ComBuf.send.ptr++];//��������
//			return (1);
//		}
//	return (0);
//}

