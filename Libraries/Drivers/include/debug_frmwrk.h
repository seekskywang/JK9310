/**********************************************************************
* $Id$		debug_frmwrk.h			2011-06-02
*//**
* @file		debug_frmwrk.h
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

#ifndef __DEBUG_FRMWRK_H_
#define __DEBUG_FRMWRK_H_
#include "Globalvalue/GlobalValue.h"
#include "lpc177x_8x_uart.h"

#define USED_UART_DEBUG_PORT	3

#define NUM_SKIPPED_ALLOWED			(10)

#define DBG_GETVAL_IN_DEC			(0)
#define DBG_GETVAL_IN_HEX			(1)
//#define DBG_GETVAL_IN_CHARS			(2) 

#if (USED_UART_DEBUG_PORT == 0)
#define DEBUG_UART_PORT	(LPC_UART_TypeDef *)LPC_UART0
#elif (USED_UART_DEBUG_PORT == 1)
#define DEBUG_UART_PORT	(LPC_UART_TypeDef *)LPC_UART1
#elif (USED_UART_DEBUG_PORT == 2)
#define DEBUG_UART_PORT	(LPC_UART_TypeDef *)LPC_UART2
#elif (USED_UART_DEBUG_PORT == 3)
#define DEBUG_UART_PORT	(LPC_UART_TypeDef *)LPC_UART3
#elif (USED_UART_DEBUG_PORT == 4)
#define DEBUG_UART_PORT	(LPC_UART_TypeDef *)LPC_UART4
#else
 #error "Invalid UART port selection!"
#endif

#define _DBG(x)								_db_msg(DEBUG_UART_PORT, x)
#define _DBG_(x)							_db_msg_(DEBUG_UART_PORT, x)

#define _DBC(x)								_db_char(DEBUG_UART_PORT, x)

#define _DBD(x)								_db_dec(DEBUG_UART_PORT, x)
#define _DBD16(x)							_db_dec_16(DEBUG_UART_PORT, x)
#define _DBD32(x)							_db_dec_32(DEBUG_UART_PORT, x)

#define _DBH(x)								_db_hex(DEBUG_UART_PORT, x)
#define _DBH16(x)							_db_hex_16(DEBUG_UART_PORT, x)
#define _DBH32(x)							_db_hex_32(DEBUG_UART_PORT, x)

#define _DBH_(x)							_db_hex_(DEBUG_UART_PORT, x)
#define _DBH16_(x)							_db_hex_16_(DEBUG_UART_PORT, x)
#define _DBH32_(x)							_db_hex_32_(DEBUG_UART_PORT, x)

#define _DG									_db_get_char(DEBUG_UART_PORT)
#define _DGV(option, numCh, val)			_db_get_val(DEBUG_UART_PORT, option, numCh, val)


//==========================================================
//��������
#define _HW_CONFIG_FALSE	(0)
#define _HW_CONFIG_TRUE		(1)

//==========================================================
//�շ�֡���峤��
#define REC_LEN_MAX  	(60)//����֡����
#define SEND_LEN_MAX 	(18)//����֡����

////���ڷ��ͽṹ
typedef struct
{
	int8_t begin;//���Ϳ�ʼ��־
	int8_t ptr;//��ǰ����ָ��
	int8_t len;//�������ݳ���
	int8_t buf[SEND_LEN_MAX];//���ݷ��ͻ���
}ComSend_TypeDef;
#define TICKS_PER_SEC 1000	//ϵͳʱ��ÿ���������100x10mS(T0)=1S
//�������ʱ��,3*1000/50=60mS
#define REC_TIME_OUT	 (3*TICKS_PER_SEC/50)
//���ڽ��սṹ
typedef struct
{
	uint8_t end;//���ս�����־
	uint8_t ptr;	//��ǰ����ָ��
	uint8_t len;//�������ݳ���
	uint8_t buf[REC_LEN_MAX];//���ݽ��ջ���
	uint8_t TimeOut;//���ճ�ʱ(10ms����)
}ComRec_TypeDef;

////�����շ��ṹ
typedef struct
{
	ComSend_TypeDef send;
	ComRec_TypeDef  rec;
}Com_TypeDef;
extern Com_TypeDef ComBuf;//�����շ�����
extern Com_TypeDef ComBuf3;
//==========================================================
//ͨ��Э�飬8λ��9600������
//==========================================================
//��ʼ��  �ܳ���  Ŀ���ַ  ������  ���ݰ�  �ۼӺ�  ������
//1�ֽ�	  1�ֽ�	  1�ֽ�	    1�ֽ�	N�ֽ�   1�ֽ�   1�ֽ�
//==========================================================
#define UART_SEND_BEGIN 		(0xAA)
#define UART_SEND_END 			(0xBF)
#define FRAME_LEN_MAX 			(REC_LEN_MAX)//һ֡����ֽ�
#define FRAME_LEN_MIN 			(3)	//һ֡��С�ֽ�
#define UART_REC_BEGIN 			(0xAA)
#define UART_REC_END 			(0xBB)

//֡���Ͷ���
#define FRAME_READ_RESULT		(0xCE)	//��ȡ���
#define FRAME_START				(0xCC)	//����
#define FRAME_RESET				(0xDD)	//��λ
#define FRAME_DATA				(0xEE)	//����֡
#define FRAME_ITEM				(0xDE)	//���ò�����Ŀ
#define FRAME_CLR_BOOT_NUM		(0xF2)	//�忪������֡
#define FRAME_WRITE_SN			(0xF3)	//д���к�֡
#define FRAME_SELECT_GROUP		(0xF4)	//ѡ�����

//֡λ�ö���
#define PFRAMEHEAD				(0)	//֡ͷλ��
#define PFRAMEKIND				(1)	//֡����λ��
#define PDATASTART				(2)	//���ݰ��׵�ַλ��

//ͨ��״̬����
#define COM_SUCCESS				(0x00)	//�ɹ�
#define COM_ERR_BUSY	 	 	(0x01)	//�ŵ�æ
#define COM_ERR_CRC				(0x02)	//У�����
#define COM_ERR_NACK 			(0x03)	//��Ӧ��
#define COM_ERR_UNEXPECT 		(0x04)	//������֡
#define COM_ERR_FORMAT			(0x05)	//���ݸ�ʽ����
#define COM_ERR_SAVE 			(0x06)	//�޷���������
#define COM_ERR_ADDR_FIRE	 	(0x07)	//��ַ��ͻ
#define COM_ERR_UNDEFINE		(0x08)	//δ�������

//==========================================================
//ͨ��Э��2��8λ��9600������
//==========================================================
#define UART_SEND_BEGIN20 		(0x5A)
#define UART_SEND_BEGIN21 		(0x59)
#define UART_REC_BEGIN20 		(0x11)
#define UART_REC_BEGIN21 		(0x08)

//ͨѶ�����
#define TEST_RST				(0x00)	//����/��λ����
#define GROUPSET	 	 		(0x01)	//ͨ������
#define BASICSET				(0x02)	//��������

//�Ĵ�����ַ
#define SLAVE_REG_P00		0x0000       //R_VOLU
#define SLAVE_REG_P01		0x0001      //Load_Voltage
#define SLAVE_REG_P02		0x0002      //Load_Current
#define SLAVE_REG_P03		0x0003		//Change_Voltage
#define SLAVE_REG_P04		0x0004		//Change_Current
#define SLAVE_REG_P05		0x0005		//Load_OCP
#define SLAVE_REG_P06		0x0006		//Change_OCP
#define SLAVE_REG_P07		0x0007		//Short_Time
#define SLAVE_REG_P08		0x0008		//Leak_Current
#define SLAVE_REG_P09		0x0009		//R1_Volu
#define SLAVE_REG_P10		0x000A		//R2_Volu
#define SLAVE_REG_P11		0x000B		//Temper
#define SLAVE_REG_P12		0x000C		//DHQ_Voltage
#define SLAVE_REG_P13		0x000D		//MODE
#define SLAVE_REG_P14		0x000E		//Load_Mode
#define SLAVE_REG_P15		0x000F		//Load_SET_Voltage
#define SLAVE_REG_P16		0x0010		//Load_SET_Current
#define SLAVE_REG_P17		0x0011		//Change_SET_Voltage
#define SLAVE_REG_P18		0x0012		//Change_SET_Current
#define SLAVE_REG_P19		0x0013       //R_VOLU
#define SLAVE_REG_P20		0x0014      //Load_Voltage
#define SLAVE_REG_P21		0x0015      //Load_Current
#define SLAVE_REG_P22		0x0016		//Change_Voltage
#define SLAVE_REG_P23		0x0017		//Change_Current
#define SLAVE_REG_P24		0x0018		//Load_OCP
#define SLAVE_REG_P25		0x0019		//Change_OCP
#define SLAVE_REG_P26		0x001A		//Short_Time
#define SLAVE_REG_P27		0x001B		//Leak_Current
#define SLAVE_REG_P28		0x001C		//R1_Volu
#define SLAVE_REG_P29		0x001D		//R2_Volu
#define SLAVE_REG_P30		0x001E		//Temper
#define SLAVE_REG_P31		0x001F		//DHQ_Voltage
#define SLAVE_REG_P32		0x0020		//MODE
#define SLAVE_REG_P33		0x0021		//Load_Mode
#define SLAVE_REG_P34		0x0022		//Load_SET_Voltage
#define SLAVE_REG_P35		0x0023		//Load_SET_Current
#define SLAVE_REG_P36		0x0024		//Change_SET_Voltage
#define SLAVE_REG_P37		0x0025		//Change_SET_Current
#define SLAVE_REG_P38		0x0026       //R_VOLU
#define SLAVE_REG_P39		0x0027      //Load_Voltage
#define SLAVE_REG_P40		0x0028      //Load_Current
#define SLAVE_REG_P41		0x0029		//Change_Voltage
#define SLAVE_REG_P42		0x002A		//Change_Current
#define SLAVE_REG_P43		0x002B		//Load_OCP
#define SLAVE_REG_P44		0x002C		//Change_OCP
#define SLAVE_REG_P45		0x002D		//Short_Time
#define SLAVE_REG_P46		0x002E		//Leak_Current
#define SLAVE_REG_P47		0x002F		//R1_Volu
#define SLAVE_REG_P48		0x0030		//R2_Volu
#define SLAVE_REG_P49		0x0031		//Temper
#define SLAVE_REG_P50		0x0032		//R2_Volu
#define SLAVE_REG_P51		0x0033		//Temper

/* RTU Ӧ����� */
#define RSP_OK				0		/* �ɹ� */
#define RSP_ERR_CMD			0x01	/* ��֧�ֵĹ����� */
#define RSP_ERR_REG_ADDR	0x02	/* �Ĵ�����ַ���� */
#define RSP_ERR_VALUE		0x03	/* ����ֵ����� */
#define RSP_ERR_WRITE		0x04	/* д��ʧ�� */

#define S_RX_BUF_SIZE		30
#define S_TX_BUF_SIZE		128

//#define UART_SEND_BEGIN 		(0xAA)
//#define UART_SEND_END 			(0xBF)
//#define FRAME_LEN_MAX 			(REC_LEN_MAX)//һ֡����ֽ�
//#define FRAME_LEN_MIN 			(3)	//һ֡��С�ֽ�
//#define UART_REC_BEGIN 			(0xcc)//AA
//#define UART_REC_END 			(0xcd)//AF

////֡���Ͷ���
//#define FRAME_READ_RESULT		(0x00)	//��ȡ���
//#define FRAME_START				(0xfa)	//����
//#define FRAME_RESET				(0xfb)	//��λ
//#define FRAME_DATA				(0xF1)	//����֡
//#define FRAME_CLR_BOOT_NUM		(0xF2)	//�忪������֡
//#define FRAME_WRITE_SN			(0xF3)	//д���к�֡

////֡λ�ö���
//#define PFRAMEHEAD				(0)	//֡ͷλ��
//#define PFRAMEKIND				(1)	//֡����λ��
//#define PDATASTART				(2)	//���ݰ��׵�ַλ��

////ͨ��״̬����
//#define COM_SUCCESS				(0x00)	//�ɹ�
//#define COM_ERR_BUSY	 	 	(0x01)	//�ŵ�æ
//#define COM_ERR_CRC				(0x02)	//У�����
//#define COM_ERR_NACK 			(0x03)	//��Ӧ��
//#define COM_ERR_UNEXPECT 		(0x04)	//������֡
//#define COM_ERR_FORMAT			(0x05)	//���ݸ�ʽ����
//#define COM_ERR_SAVE 			(0x06)	//�޷���������
//#define COM_ERR_ADDR_FIRE	 	(0x07)	//��ַ��ͻ
//#define COM_ERR_UNDEFINE		(0x08)	//δ�������
//void  _printf (const  char *format, ...);
#define SetRecTimeOut(time) (ComBuf.rec.TimeOut=time)
extern struct MODS_T g_tModS;
struct MODS_T
{
	uint8_t RxBuf[S_RX_BUF_SIZE];
	uint8_t TxBuf[S_TX_BUF_SIZE];
	uint8_t RxCount;
	uint8_t RxStatus;
	uint8_t RxNewFlag;
	uint8_t RspCode;
	
	uint8_t TxCount;
};


extern const vu8 READDATA[7];
extern void (*_db_msg)(LPC_UART_TypeDef *UARTx, const void *s);
extern void (*_db_msg_)(LPC_UART_TypeDef *UARTx, const void *s);
extern void (*_db_char)(LPC_UART_TypeDef *UARTx, uint8_t ch);
extern void (*_db_dec)(LPC_UART_TypeDef *UARTx, uint8_t decn);
extern void (*_db_dec_16)(LPC_UART_TypeDef *UARTx, uint16_t decn);
extern void (*_db_dec_32)(LPC_UART_TypeDef *UARTx, uint32_t decn);
extern void (*_db_hex)(LPC_UART_TypeDef *UARTx, uint8_t hexn);
extern void (*_db_hex_16)(LPC_UART_TypeDef *UARTx, uint16_t hexn);
extern void (*_db_hex_32)(LPC_UART_TypeDef *UARTx, uint32_t hexn);
extern void (*_db_hex_)(LPC_UART_TypeDef *UARTx, uint8_t hexn);
extern void (*_db_hex_16_)(LPC_UART_TypeDef *UARTx, uint16_t hexn);
extern void (*_db_hex_32_)(LPC_UART_TypeDef *UARTx, uint32_t hexn);

extern uint8_t (*_db_get_char)(LPC_UART_TypeDef *UARTx);
extern uint8_t (*_db_get_val)(LPC_UART_TypeDef *UARTx, uint8_t option, uint8_t numCh, uint32_t * val);

uint8_t UARTGetValue (LPC_UART_TypeDef *UARTx, uint8_t option,
											uint8_t numCh, uint32_t* val);
void UARTPutChar (LPC_UART_TypeDef *UARTx, uint8_t ch);
extern void UARTPuts(LPC_UART_TypeDef *UARTx, const void *str);
void UARTPuts_(LPC_UART_TypeDef *UARTx, const void *str);
void UARTPutDec(LPC_UART_TypeDef *UARTx, uint8_t decnum);
void UARTPutDec16(LPC_UART_TypeDef *UARTx, uint16_t decnum);
void UARTPutDec32(LPC_UART_TypeDef *UARTx, uint32_t decnum);
void UARTPutHex (LPC_UART_TypeDef *UARTx, uint8_t hexnum);
void UARTPutHex16 (LPC_UART_TypeDef *UARTx, uint16_t hexnum);
void UARTPutHex32 (LPC_UART_TypeDef *UARTx, uint32_t hexnum);
void UARTPuts(LPC_UART_TypeDef *UARTx, const void *str);
uint8_t UARTGetChar (LPC_UART_TypeDef *UARTx);
//void  _printf (const  char *format, ...);
void debug_frmwrk_init(void);
void Uart3_init(uint32_t freq);
vu8 Uart_Process(void);
void Uart3_Send(void);
void Uart0_Send(u8 king);
void UARTPutChar (LPC_UART_TypeDef *UARTx, uint8_t ch);
void MODS_SendWithCRC(uint8_t *_pBuf, uint8_t _ucLen);
uint16_t CRC16(uint8_t *_pBuf, uint16_t _usLen);
//void Send_Freq(Send_Ord_Typedef *ord);

#endif /* __DEBUG_FRMWRK_H_ */
