#ifndef __AT45DB_H_
#define __AT45DB_H_

#include "lpc177x_8x_pinsel.h"
#include "lpc177x_8x_gpio.h"
#include "debug_frmwrk.h"
#include "lpc177x_8x_ssp.h"


//typedef uint8_t  u8;
//typedef uint16_t  u16;
//typedef uint32_t  u32;

extern uint8_t AT_RETURN_CODE[4];

#define BUF1_WRITE 0x84
#define BUF2_WRITE 0x87
#define BUF1_READ 0xD4
#define BUF2_READ 0xD6
#define BBUF1_TO_MM_PAGE_PROG_WITH_ERASE 0x83
#define BBUF2_TO_MM_PAGE_PROG_WITH_ERASE 0x86
#define MM_PAGE_TO_B1_XFER 0x53
#define MM_PAGE_TO_B2_XFER 0x55
#define PAGE_ERASE 0x81   // 512/528 bytes per page
#define SECTOR_ERASE 0x7C // 128k bytes per sector
#define READ_STATE_REGISTER 0xD7    ///0x57//0xD7
#define Read_ID 0x9F


#define SPI_FLASH_PageSize              256
#define SPI_FLASH_PerWritePageSize      256

/* Private define ------------------------------------------------------------*/
/*命令定义-开头*******************************/
#define W25X_WriteEnable		      0x06 
#define W25X_WriteDisable		      0x04 
#define W25X_ReadStatusReg		    0x05 
#define W25X_WriteStatusReg		  0x01 
#define W25X_ReadData			        0x03 
#define W25X_FastReadData		      0x0B 
#define W25X_FastReadDual		      0x3B 
#define W25X_PageProgram		      0x02 
#define W25X_BlockErase			      0xD8 
#define W25X_SectorErase		      0x20 
#define W25X_ChipErase			      0xC7 
#define W25X_PowerDown			      0xB9 
#define W25X_ReleasePowerDown	  0xAB 
#define W25X_DeviceID			        0xAB 
#define W25X_ManufactDeviceID   	0x90 
#define W25X_JedecDeviceID		    0x9F 

#define WIP_Flag                  0x01  /* Write In Progress (WIP) flag */
#define Dummy_Byte                0xFF

#define Open_SSP0
//#define Open_SSP1

#ifdef Open_SSP0
    #define LPC_SSPx LPC_SSP0
	/*SSP0 CS(P1.21)*/
	#define AT45DBXX_Enable 	 AT45DBXX_nCS_L
	#define AT45DBXX_Disable 	 AT45DBXX_nCS_H
	
	#define AT45DBXX_nCS_L      GPIO_SetDir (0,(1<<19),1);   \
	                            GPIO_ClearValue(0,(1<<19));

	#define AT45DBXX_nCS_H      GPIO_SetDir (0,(1<<19),1);   \
                                GPIO_SetValue(0,(1<<19));
#endif

#ifdef Open_SSP1
    #define LPC_SSPx LPC_SSP1
//	/*SSP1 CS(P0.14)*/
//	#define AT45DBXX_Enable 	 AT45DBXX_nCS_L
//	#define AT45DBXX_Disable 	 AT45DBXX_nCS_H
//	
//	#define AT45DBXX_nCS_L      GPIO_SetDir (0,(1<<14),1);   \
//	                            GPIO_ClearValue (0,(1<<14));
//
//	#define AT45DBXX_nCS_H      GPIO_SetDir (0,(1<<14),1);   \
//                                GPIO_SetValue(0,(1<<14));

	/*SSP1 CS(P0.14)*/
	#define AT45DBXX_Enable 	 AT45DBXX_nCS_L
	#define AT45DBXX_Disable 	 AT45DBXX_nCS_H
	
	#define AT45DBXX_nCS_L      GPIO_SetDir (0,(1<<6),1);   \
	                            GPIO_ClearValue (0,(1<<6));

	#define AT45DBXX_nCS_H      GPIO_SetDir (0,(1<<6),1);   \
                                GPIO_SetValue(0,(1<<6));

#endif
/*等待超时时间*/
#define SPIT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define SPIT_LONG_TIMEOUT         ((uint32_t)(10 * SPIT_FLAG_TIMEOUT))
void SSPIx_Init(void);
/*static */void AT45DBXX_BUSY(void);
void AT45DBXX_Init(void);
void AT45DBXX_Read_ID(u8 *IData);
void write_buffer(u16 BufferOffset,u8 Data);
u8 read_buffer(u16 BufferOffset);
void Save_fileDate(u8 Group);
void Read_FileDate(u8 Group);
u32 SPI_FLASH_ReadID(void);

#endif /* __AT45DB_H_ */
