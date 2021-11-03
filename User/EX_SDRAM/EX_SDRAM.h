

#ifndef __EX_SDRAM_h
#define __EX_SDRAM_h

#include "LPC177x_8x.h"
#include "lpc_types.h"
#include "system_LPC177x_8x.h" 
#include "gui.h"
#define SDRAM_CONFIG_16BIT
//#define SDRAM_CONFIG_32BIT	// 外接2颗16位SDRAM

#ifdef SDRAM_CONFIG_16BIT 
#define SDRAM_SIZE               0x2000000	// 256Mbit
/* LCD color */
#define White          0xFFFF
#define Black          0x0000
#define Blue           0xF800
#define Red            0x001F
#define Magenta        0xF81F
#define Green          0x07E0
#define Yellow         0x07FF

//#elif defined SDRAM_CONFIG_32BIT	 //
///* LCD color */
//#define White          0xFFFF
//#define Black          0x0000
//#define Blue           0xFF00
//#define Red            0x0000
//#define Green          0x00FF

/** 
  * @brief  LCD color  
  */ 


#define LCD_COLOR_WHITE          0xFFFF
#define LCD_COLOR_BLACK          0x0000
#define LCD_COLOR_GREY           0xC618
#define LCD_COLOR_BLUE           0xF800
#define LCD_COLOR_BLUE2          0x0014
#define LCD_COLOR_RED            0x001F
#define LCD_COLOR_MAGENTA        0xF81F
#define LCD_COLOR_GREEN          0x07E0
#define LCD_COLOR_CYAN           0xFFE0
#define LCD_COLOR_YELLOW         0x0EFF
//#define SDRAM_SIZE               0x4000000	// 512Mbit
#define	LCD_COLOR_TEST_MID  	 0x39AC
#define	LCD_COLOR_TEST_BACK 	 0x00800000//
#define	LCD_COLOR_TEST_BUTON     0x2A69//0X310038
#define	LCD_COLOR_TEST_LINE      LCD_COLOR_TEST_MID//0X694368
#define	LCD_COLOR_TEST_BAR       0x023F//0x0352F3
#define	LCD_COLOR_SELECT       	  GUI_RED//被选中的颜色
#else
	error Wrong SDRAM config, check ex_sdram.h
#endif







#define SDRAM_BASE               0xA0000000	   /* CS0 */


void SDRAM_Init (void);

#endif
