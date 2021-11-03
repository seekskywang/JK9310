#include "AT45DB.h"
#include "timer.h"
#define BUF_SIZE    0x01   // 1 Byte

static __IO uint32_t  SPITimeout = SPIT_LONG_TIMEOUT;   
// SPI Data Setup structure variable
SSP_DATA_SETUP_Type dataCfg;

uint8_t SPIx_communication(uint8_t  data)
{
    uint8_t data_rx[BUF_SIZE];
    uint8_t rl;

    dataCfg.length = BUF_SIZE;
    dataCfg.tx_data = &data;
    rl = SSP_ReadWrite (LPC_SSPx,&dataCfg,SSP_TRANSFER_POLLING);
    if(rl != 0)
    {
        dataCfg.rx_data = &data_rx;
        return  data_rx[0];
//        _printf("%d\n",rl);
        //return (uint8_t)((uint8_t*)(dataCfg->rx_data));
    }
    return 0;

}

void SSPIx_Init(void)
{

    SSP_CFG_Type SSP_ConfigStruct; 
/*
* SSPI0 MISO(P1_23) MOSI(P1_24) SCK(P1_20) SSEL(P1_21)
* SSPI1 MISO(P0_12) MOSI(P0_13) SCK(P1_19) SSEL(P0_14)
*
*/
#ifdef Open_SSP0
   	PINSEL_ConfigPin(0, 15, 2);
	PINSEL_ConfigPin(0, 17, 2);
	PINSEL_ConfigPin(0, 18, 2);
#endif


#ifdef Open_SSP1
	PINSEL_ConfigPin(0, 7, 2);
	PINSEL_SetFilter(0, 7, 0);

	PINSEL_ConfigPin(0, 8, 2);
	PINSEL_SetFilter(0, 8, 0);

	PINSEL_ConfigPin(0, 9, 2);
	PINSEL_SetFilter(0, 9, 0);

////	PINSEL_ConfigPin(0, 12, 2);
////	PINSEL_SetFilter(0, 12, 0);
////
////	PINSEL_ConfigPin(0, 13, 2);
////	PINSEL_SetFilter(0, 13, 0);
////
////	PINSEL_ConfigPin(1, 19, 5);
////	PINSEL_SetFilter(1, 19, 0);

#endif  
    /*SPIx configuration*/
    
    SSP_ConfigStruct.Databit = SSP_DATABIT_8;
    SSP_ConfigStruct.CPHA = SSP_CPHA_FIRST ;
    SSP_ConfigStruct.CPOL = SSP_CPOL_HI;
    SSP_ConfigStruct.Mode = SSP_MASTER_MODE;    
    SSP_ConfigStruct.FrameFormat = SSP_FRAME_SPI;
    SSP_ConfigStruct.ClockRate = 400000; //400KHz

	/* Initializing SSP device section ------------------------------------------------------ */
	// initialize SSP configuration structure to default
	SSP_ConfigStructInit(&SSP_ConfigStruct);

	// Initialize SSP peripheral with parameter given in structure above
	SSP_Init(LPC_SSPx, &SSP_ConfigStruct);

	// Enable SSP peripheral
	SSP_Cmd(LPC_SSPx, ENABLE);
}

void AT45DBXX_Init(void)
{
    AT45DBXX_nCS_H;
	SSPIx_Init();        
}

/*static */
void AT45DBXX_BUSY(void)
{
	AT45DBXX_Enable;
    while(!(0x80 & (SPIx_communication(READ_STATE_REGISTER))));
	AT45DBXX_Disable;	
}

void AT45DBXX_Read_ID(u8 *IData)
{
	u8 i;
//	AT45DBXX_BUSY();
	AT45DBXX_Enable;	
    SPIx_communication(Read_ID);
    for(i=0;i<4;i++)
        IData[i]=SPIx_communication(0xff);
  	AT45DBXX_Disable;
}

void write_buffer(u16 BufferOffset,u8 Data)
{			
	AT45DBXX_Enable;
	SPIx_communication(0x84);			
	SPIx_communication(0xff);
	SPIx_communication((u8)BufferOffset>>8);
	SPIx_communication((u8)BufferOffset);		
	SPIx_communication(Data);
	AT45DBXX_Disable;
}

u8 read_buffer(u16 BufferOffset)
{		
u8 temp=0;

	AT45DBXX_Enable;
 	SPIx_communication(0xd4);
	SPIx_communication(0xff);
	SPIx_communication((u8)BufferOffset>>8);
	SPIx_communication((u8)BufferOffset);  
	SPIx_communication(0xff);
	temp=SPIx_communication(0xff);	
	AT45DBXX_Disable;
    return temp;		
}

 /**
  * @brief  ��FLASH���� дʹ�� ����
  * @param  none
  * @retval none
  */
void SPI_FLASH_WriteEnable(void)
{
  /* ͨѶ��ʼ��CS�� */
  AT45DBXX_Enable;

  /* ����дʹ������*/
  SPIx_communication(W25X_WriteEnable);

  /*ͨѶ������CS�� */
  AT45DBXX_Disable;
}
 /**
  * @brief  �ȴ�WIP(BUSY)��־����0�����ȴ���FLASH�ڲ�����д�����
  * @param  none
  * @retval none
  */
void SPI_FLASH_WaitForWriteEnd(void)
{
  u8 FLASH_Status = 0;

  /* ѡ�� FLASH: CS �� */
  AT45DBXX_Enable;

  /* ���� ��״̬�Ĵ��� ���� */
  SPIx_communication(W25X_ReadStatusReg);

  SPITimeout = SPIT_LONG_TIMEOUT;
  /* ��FLASHæµ����ȴ� */
  do
  {
    /* ��ȡFLASHоƬ��״̬�Ĵ��� */
    FLASH_Status = SPIx_communication(Dummy_Byte);	 

    {
      if((SPITimeout--) == 0) 
      {
//        SPI_TIMEOUT_UserCallback(4);
          FLASH_Status=0;
        return;
      }
    } 
  }
  while ((FLASH_Status & WIP_Flag) == SET); /* ����д���־ */

  /* ֹͣ�ź�  FLASH: CS �� */
  AT45DBXX_Disable;
}
 /**
  * @brief  ����FLASH��������Ƭ����
  * @param  ��
  * @retval ��
  */
void SPI_FLASH_BulkErase(void)
{
  /* ����FLASHдʹ������ */
  SPI_FLASH_WriteEnable();

  /* ���� Erase */
  /* ѡ��FLASH: CS�͵�ƽ */
  AT45DBXX_Enable;
  /* �����������ָ��*/
  SPIx_communication(W25X_ChipErase);
  /* ֹͣ�ź� FLASH: CS �ߵ�ƽ */
  AT45DBXX_Disable;

  /* �ȴ��������*/
  SPI_FLASH_WaitForWriteEnd();
}


 /**
  * @brief  ����FLASH����
  * @param  SectorAddr��Ҫ������������ַ
  * @retval ��
  */
void SPI_FLASH_SectorErase(u32 SectorAddr)
{
  /* ����FLASHдʹ������ */
  SPI_FLASH_WriteEnable();
  SPI_FLASH_WaitForWriteEnd();
  /* �������� */
  /* ѡ��FLASH: CS�͵�ƽ */
  AT45DBXX_Enable;
  /* ������������ָ��*/
  SPIx_communication(W25X_SectorErase);
  /*���Ͳ���������ַ�ĸ�λ*/
  SPIx_communication((SectorAddr & 0xFF0000) >> 16);
  /* ���Ͳ���������ַ����λ */
  SPIx_communication((SectorAddr & 0xFF00) >> 8);
  /* ���Ͳ���������ַ�ĵ�λ */
  SPIx_communication(SectorAddr & 0xFF);
  /* ֹͣ�ź� FLASH: CS �ߵ�ƽ */
  AT45DBXX_Disable;
  /* �ȴ��������*/
  SPI_FLASH_WaitForWriteEnd();
}
 /**
  * @brief  ��FLASH��ҳд�����ݣ����ñ�����д������ǰ��Ҫ�Ȳ�������
  * @param	pBuffer��Ҫд�����ݵ�ָ��
  * @param WriteAddr��д���ַ
  * @param  NumByteToWrite��д�����ݳ��ȣ�����С�ڵ���SPI_FLASH_PerWritePageSize
  * @retval ��
  */
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
  /* ����FLASHдʹ������ */
  SPI_FLASH_WriteEnable();

  /* ѡ��FLASH: CS�͵�ƽ */
  AT45DBXX_Enable;
  /* дҳдָ��*/
  SPIx_communication(W25X_PageProgram);
  /*����д��ַ�ĸ�λ*/
  SPIx_communication((WriteAddr & 0xFF0000) >> 16);
  /*����д��ַ����λ*/
  SPIx_communication((WriteAddr & 0xFF00) >> 8);
  /*����д��ַ�ĵ�λ*/
  SPIx_communication(WriteAddr & 0xFF);

  if(NumByteToWrite > SPI_FLASH_PerWritePageSize)
  {
     NumByteToWrite = SPI_FLASH_PerWritePageSize;
//     FLASH_ERROR("SPI_FLASH_PageWrite too large!");
  }

  /* д������*/
  while (NumByteToWrite--)
  {
    /* ���͵�ǰҪд����ֽ����� */
    SPIx_communication(*pBuffer);
    /* ָ����һ�ֽ����� */
    pBuffer++;
  }

  /* ֹͣ�ź� FLASH: CS �ߵ�ƽ */
  AT45DBXX_Disable;

  /* �ȴ�д�����*/
  SPI_FLASH_WaitForWriteEnd();
}


/**
  * @brief  ��FLASHд�����ݣ����ñ�����д������ǰ��Ҫ�Ȳ�������
  * @param	pBuffer��Ҫд�����ݵ�ָ��
  * @param  WriteAddr��д���ַ
  * @param  NumByteToWrite��д�����ݳ���
  * @retval ��
  */
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
  u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;
	
	/*mod�������࣬��writeAddr��SPI_FLASH_PageSize��������������AddrֵΪ0*/
  Addr = WriteAddr % SPI_FLASH_PageSize;
	
	/*��count������ֵ���պÿ��Զ��뵽ҳ��ַ*/
  count = SPI_FLASH_PageSize - Addr;	
	/*�����Ҫд��������ҳ*/
  NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
	/*mod�������࣬�����ʣ�಻��һҳ���ֽ���*/
  NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

	 /* Addr=0,��WriteAddr �պð�ҳ���� aligned  */
  if (Addr == 0) 
  {
		/* NumByteToWrite < SPI_FLASH_PageSize */
    if (NumOfPage == 0) 
    {
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
			/*�Ȱ�����ҳ��д��*/
      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }
			
			/*���ж���Ĳ���һҳ�����ݣ�����д��*/
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
    }
  }
	/* ����ַ�� SPI_FLASH_PageSize ������  */
  else 
  {
		/* NumByteToWrite < SPI_FLASH_PageSize */
    if (NumOfPage == 0) 
    {
			/*��ǰҳʣ���count��λ�ñ�NumOfSingleС��д����*/
      if (NumOfSingle > count) 
      {
        temp = NumOfSingle - count;
				
				/*��д����ǰҳ*/
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;
				
				/*��дʣ�������*/
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
      }
      else /*��ǰҳʣ���count��λ����д��NumOfSingle������*/
      {				
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
			/*��ַ����������count�ֿ������������������*/
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
      NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

      SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;
			
			/*������ҳ��д��*/
      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }
			/*���ж���Ĳ���һҳ�����ݣ�����д��*/
      if (NumOfSingle != 0)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}

 /**
  * @brief  ��ȡFLASH����
  * @param 	pBuffer���洢�������ݵ�ָ��
  * @param   ReadAddr����ȡ��ַ
  * @param   NumByteToRead����ȡ���ݳ���
  * @retval ��
  */
void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
  /* ѡ��FLASH: CS�͵�ƽ */
  AT45DBXX_Enable;

  /* ���� �� ָ�� */
  SPIx_communication(W25X_ReadData);

  /* ���� �� ��ַ��λ */
  SPIx_communication((ReadAddr & 0xFF0000) >> 16);
  /* ���� �� ��ַ��λ */
  SPIx_communication((ReadAddr& 0xFF00) >> 8);
  /* ���� �� ��ַ��λ */
  SPIx_communication(ReadAddr & 0xFF);
  
	/* ��ȡ���� */
  while (NumByteToRead--)
  {
    /* ��ȡһ���ֽ�*/
    *pBuffer = SPIx_communication(Dummy_Byte);
    /* ָ����һ���ֽڻ����� */
    pBuffer++;
  }

  /* ֹͣ�ź� FLASH: CS �ߵ�ƽ */
  AT45DBXX_Disable;
}
 /**
  * @brief  ��ȡFLASH ID
  * @param 	��
  * @retval FLASH ID
  */
u32 SPI_FLASH_ReadID(void)
{
  u32 Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

  /* ��ʼͨѶ��CS�͵�ƽ */
  AT45DBXX_Enable;

  /* ����JEDECָ���ȡID */
  SPIx_communication(W25X_JedecDeviceID);

  /* ��ȡһ���ֽ����� */
  Temp0 = SPIx_communication(Dummy_Byte);

  /* ��ȡһ���ֽ����� */
  Temp1 = SPIx_communication(Dummy_Byte);

  /* ��ȡһ���ֽ����� */
  Temp2 = SPIx_communication(Dummy_Byte);

  /* ֹͣͨѶ��CS�ߵ�ƽ */
  AT45DBXX_Disable;

	/*�����������������Ϊ�����ķ���ֵ*/
  Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

  return Temp;
}

void Save_fileDate(u8 Group)
{
    u32 addr;
//    u8 temp[4];
//    uint8_t Tx_Buffer[] = "http://firestm32.taobao.com";
//uint8_t Rx_Buffer[200];
//    for(addr=0;addr<200;addr++)
//    Rx_Buffer[addr]=0;
    addr=4096*Group;
    
//    SPI_FLASH_BulkErase();
    SPI_FLASH_SectorErase(addr);
//    delayMs(0,100);
//    AT45DBXX_Read_ID(temp);
//for(i=0;i<0xffffff;i++)	
    ;
    SPI_FLASH_BufferWrite((u8*)&U9001_save, addr, sizeof(U9001_save));
//    SPI_FLASH_BufferWrite(Tx_Buffer, addr, sizeof(Tx_Buffer));
////    SPI_FLASH_PageWrite(Tx_Buffer, addr, sizeof(Tx_Buffer));
////    SPI_FLASH_BufferRead(Rx_Buffer, addr, sizeof(Rx_Buffer));
//    addr=4096*0;
//    SPI_FLASH_BufferRead(Rx_Buffer, addr, sizeof(Rx_Buffer));

}
void Read_FileDate(u8 Group)
{
    u32 addr;
    addr=4096*Group;
    SPI_FLASH_BufferRead((u8*)&U9001_save, addr, sizeof(U9001_save));
}
