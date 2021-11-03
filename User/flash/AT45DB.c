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
  * @brief  向FLASH发送 写使能 命令
  * @param  none
  * @retval none
  */
void SPI_FLASH_WriteEnable(void)
{
  /* 通讯开始：CS低 */
  AT45DBXX_Enable;

  /* 发送写使能命令*/
  SPIx_communication(W25X_WriteEnable);

  /*通讯结束：CS高 */
  AT45DBXX_Disable;
}
 /**
  * @brief  等待WIP(BUSY)标志被置0，即等待到FLASH内部数据写入完毕
  * @param  none
  * @retval none
  */
void SPI_FLASH_WaitForWriteEnd(void)
{
  u8 FLASH_Status = 0;

  /* 选择 FLASH: CS 低 */
  AT45DBXX_Enable;

  /* 发送 读状态寄存器 命令 */
  SPIx_communication(W25X_ReadStatusReg);

  SPITimeout = SPIT_LONG_TIMEOUT;
  /* 若FLASH忙碌，则等待 */
  do
  {
    /* 读取FLASH芯片的状态寄存器 */
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
  while ((FLASH_Status & WIP_Flag) == SET); /* 正在写入标志 */

  /* 停止信号  FLASH: CS 高 */
  AT45DBXX_Disable;
}
 /**
  * @brief  擦除FLASH扇区，整片擦除
  * @param  无
  * @retval 无
  */
void SPI_FLASH_BulkErase(void)
{
  /* 发送FLASH写使能命令 */
  SPI_FLASH_WriteEnable();

  /* 整块 Erase */
  /* 选择FLASH: CS低电平 */
  AT45DBXX_Enable;
  /* 发送整块擦除指令*/
  SPIx_communication(W25X_ChipErase);
  /* 停止信号 FLASH: CS 高电平 */
  AT45DBXX_Disable;

  /* 等待擦除完毕*/
  SPI_FLASH_WaitForWriteEnd();
}


 /**
  * @brief  擦除FLASH扇区
  * @param  SectorAddr：要擦除的扇区地址
  * @retval 无
  */
void SPI_FLASH_SectorErase(u32 SectorAddr)
{
  /* 发送FLASH写使能命令 */
  SPI_FLASH_WriteEnable();
  SPI_FLASH_WaitForWriteEnd();
  /* 擦除扇区 */
  /* 选择FLASH: CS低电平 */
  AT45DBXX_Enable;
  /* 发送扇区擦除指令*/
  SPIx_communication(W25X_SectorErase);
  /*发送擦除扇区地址的高位*/
  SPIx_communication((SectorAddr & 0xFF0000) >> 16);
  /* 发送擦除扇区地址的中位 */
  SPIx_communication((SectorAddr & 0xFF00) >> 8);
  /* 发送擦除扇区地址的低位 */
  SPIx_communication(SectorAddr & 0xFF);
  /* 停止信号 FLASH: CS 高电平 */
  AT45DBXX_Disable;
  /* 等待擦除完毕*/
  SPI_FLASH_WaitForWriteEnd();
}
 /**
  * @brief  对FLASH按页写入数据，调用本函数写入数据前需要先擦除扇区
  * @param	pBuffer，要写入数据的指针
  * @param WriteAddr，写入地址
  * @param  NumByteToWrite，写入数据长度，必须小于等于SPI_FLASH_PerWritePageSize
  * @retval 无
  */
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
  /* 发送FLASH写使能命令 */
  SPI_FLASH_WriteEnable();

  /* 选择FLASH: CS低电平 */
  AT45DBXX_Enable;
  /* 写页写指令*/
  SPIx_communication(W25X_PageProgram);
  /*发送写地址的高位*/
  SPIx_communication((WriteAddr & 0xFF0000) >> 16);
  /*发送写地址的中位*/
  SPIx_communication((WriteAddr & 0xFF00) >> 8);
  /*发送写地址的低位*/
  SPIx_communication(WriteAddr & 0xFF);

  if(NumByteToWrite > SPI_FLASH_PerWritePageSize)
  {
     NumByteToWrite = SPI_FLASH_PerWritePageSize;
//     FLASH_ERROR("SPI_FLASH_PageWrite too large!");
  }

  /* 写入数据*/
  while (NumByteToWrite--)
  {
    /* 发送当前要写入的字节数据 */
    SPIx_communication(*pBuffer);
    /* 指向下一字节数据 */
    pBuffer++;
  }

  /* 停止信号 FLASH: CS 高电平 */
  AT45DBXX_Disable;

  /* 等待写入完毕*/
  SPI_FLASH_WaitForWriteEnd();
}


/**
  * @brief  对FLASH写入数据，调用本函数写入数据前需要先擦除扇区
  * @param	pBuffer，要写入数据的指针
  * @param  WriteAddr，写入地址
  * @param  NumByteToWrite，写入数据长度
  * @retval 无
  */
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
  u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;
	
	/*mod运算求余，若writeAddr是SPI_FLASH_PageSize整数倍，运算结果Addr值为0*/
  Addr = WriteAddr % SPI_FLASH_PageSize;
	
	/*差count个数据值，刚好可以对齐到页地址*/
  count = SPI_FLASH_PageSize - Addr;	
	/*计算出要写多少整数页*/
  NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
	/*mod运算求余，计算出剩余不满一页的字节数*/
  NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

	 /* Addr=0,则WriteAddr 刚好按页对齐 aligned  */
  if (Addr == 0) 
  {
		/* NumByteToWrite < SPI_FLASH_PageSize */
    if (NumOfPage == 0) 
    {
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
			/*先把整数页都写了*/
      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }
			
			/*若有多余的不满一页的数据，把它写完*/
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
    }
  }
	/* 若地址与 SPI_FLASH_PageSize 不对齐  */
  else 
  {
		/* NumByteToWrite < SPI_FLASH_PageSize */
    if (NumOfPage == 0) 
    {
			/*当前页剩余的count个位置比NumOfSingle小，写不完*/
      if (NumOfSingle > count) 
      {
        temp = NumOfSingle - count;
				
				/*先写满当前页*/
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;
				
				/*再写剩余的数据*/
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
      }
      else /*当前页剩余的count个位置能写完NumOfSingle个数据*/
      {				
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
			/*地址不对齐多出的count分开处理，不加入这个运算*/
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
      NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

      SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;
			
			/*把整数页都写了*/
      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }
			/*若有多余的不满一页的数据，把它写完*/
      if (NumOfSingle != 0)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}

 /**
  * @brief  读取FLASH数据
  * @param 	pBuffer，存储读出数据的指针
  * @param   ReadAddr，读取地址
  * @param   NumByteToRead，读取数据长度
  * @retval 无
  */
void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
  /* 选择FLASH: CS低电平 */
  AT45DBXX_Enable;

  /* 发送 读 指令 */
  SPIx_communication(W25X_ReadData);

  /* 发送 读 地址高位 */
  SPIx_communication((ReadAddr & 0xFF0000) >> 16);
  /* 发送 读 地址中位 */
  SPIx_communication((ReadAddr& 0xFF00) >> 8);
  /* 发送 读 地址低位 */
  SPIx_communication(ReadAddr & 0xFF);
  
	/* 读取数据 */
  while (NumByteToRead--)
  {
    /* 读取一个字节*/
    *pBuffer = SPIx_communication(Dummy_Byte);
    /* 指向下一个字节缓冲区 */
    pBuffer++;
  }

  /* 停止信号 FLASH: CS 高电平 */
  AT45DBXX_Disable;
}
 /**
  * @brief  读取FLASH ID
  * @param 	无
  * @retval FLASH ID
  */
u32 SPI_FLASH_ReadID(void)
{
  u32 Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

  /* 开始通讯：CS低电平 */
  AT45DBXX_Enable;

  /* 发送JEDEC指令，读取ID */
  SPIx_communication(W25X_JedecDeviceID);

  /* 读取一个字节数据 */
  Temp0 = SPIx_communication(Dummy_Byte);

  /* 读取一个字节数据 */
  Temp1 = SPIx_communication(Dummy_Byte);

  /* 读取一个字节数据 */
  Temp2 = SPIx_communication(Dummy_Byte);

  /* 停止通讯：CS高电平 */
  AT45DBXX_Disable;

	/*把数据组合起来，作为函数的返回值*/
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
