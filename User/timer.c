/****************************************************************************
 *   $Id:: timer.c 5788 2010-12-02 01:13:25Z usb00423                       $
 *   Project: NXP LPC17xx Timer for PWM example
 *
 *   Description:
 *     This file contains timer code example which include timer 
 *     initialization, timer interrupt handler, and APIs for timer access.
 *
 ****************************************************************************
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
****************************************************************************/
#include "LPC177x_8x.h"
#include "lpc_types.h"
#include "timer.h"
#include "system_LPC177x_8x.h"
#include "Key/key.h"
#include "GlobalValue.h"
#include "debug_frmwrk.h"

volatile uint32_t timer0_counter = 0;
volatile uint32_t timer1_counter = 0;
volatile uint32_t timer2_counter = 0;
volatile uint32_t timer3_counter = 0;

u32 Tick_10ms=0;
u32 OldTick;
extern u8 g_mods_timeout;

void MODS_Poll(void)
{
	uint16_t addr;
	static uint16_t crc1;
    static u32 testi;
	/* 超过3.5个字符时间后执行MODH_RxTimeOut()函数。全局变量 g_rtu_timeout = 1; 通知主程序开始解砿*/
//	if (g_mods_timeout == 0)	
//	{
//		return;								/* 没有超时，继续接收。不要清雿g_tModS.RxCount */
//	}

    testi=g_tModS.RxCount;
    testi=g_tModS.RxCount;
    testi=g_tModS.RxCount;
	if(testi>7)				/* ??????С?4???????? */
	{
		testi=testi;
	}
	testi=g_tModS.RxCount;
    if(testi==8)				/* ??????С?4???????? */
	{
		testi=testi+1;
	}
	//??????ˇ???
	if(OldTick!=Tick_10ms)
  	{  
	  OldTick=Tick_10ms;
	   if(g_mods_timeout>0)
      { 
	    g_mods_timeout--;
      }
	  if(g_mods_timeout==0 && g_tModS.RxCount>0)   //??????
      { 
		// goto err_ret;
	
      }
      else if(g_mods_timeout==0 && g_tModS.RxCount==0) //?????
         return;
      else //????ì???
         return;
	}
	else   //???10msì?????
		return;
	//g_mods_timeout = 0;	 					/* ??? */

	if (g_tModS.RxCount < 4)				/* ??????С?4???????? */
	{
		goto err_ret;
	}

	/* ??CRCУ?? */
// 	crc1 = CRC16(g_tModS.RxBuf, g_tModS.RxCount);
// 	if (crc1 != 0)
// 	{
// 		goto err_ret;
// 	}

// 	/* ??? (1??é */
 	addr = g_tModS.RxBuf[0];				/* ?1?? ?? */
 	if (addr != U9001_Save_sys.U9001_SYS.buss_addr)		 			/* ???????????ˇ??? */
 	{
 		goto err_ret;
 	}

	/* 分析应用层协访*/
//    if(g_tModS.RxBuf[2] == 0xA5)
//    {
//        UART_Action();
//    }else{
//        usartocflag = 1;
        RecHandle();
//    }
							
	
err_ret:
#if 0										/* 此部分为了串口打印结枿实际运用中可不要 */
	g_tPrint.Rxlen = g_tModS.RxCount;
	memcpy(g_tPrint.RxBuf, g_tModS.RxBuf, g_tModS.RxCount);
#endif
	
 	g_tModS.RxCount = 0;					/* 必须清零计数器，方便下次帧同歿*/
}

/*****************************************************************************
** Function name:		delayMs
**
** Descriptions:		Start the timer delay in milo seconds
**						until elapsed
**
** parameters:			timer number, Delay value in milo second			 
** 						
** Returned value:		None
** 
*****************************************************************************/
void delayMs(uint8_t timer_num, uint32_t delayInMs)
{
  if ( timer_num == 0 )
  {
	LPC_TIM0->TCR = 0x02;		/* reset timer */
	LPC_TIM0->PR  = 0x00;		/* set prescaler to zero */
	LPC_TIM0->MR0 = delayInMs * (PeripheralClock / 1000 - 1);
	LPC_TIM0->IR  = 0xff;		/* reset all interrrupts */
	LPC_TIM0->MCR = 0x04;		/* stop timer on match */
	LPC_TIM0->TCR = 0x01;		/* start timer */
  
	/* wait until delay time has elapsed */
	while (LPC_TIM0->TCR & 0x01);
  }
  else if ( timer_num == 1 )
  {
	LPC_TIM1->TCR = 0x02;		/* reset timer */
	LPC_TIM1->PR  = 0x00;		/* set prescaler to zero */
	LPC_TIM1->MR0 = delayInMs * (PeripheralClock / 1000 - 1);
	LPC_TIM1->IR  = 0xff;		/* reset all interrrupts */
	LPC_TIM1->MCR = 0x04;		/* stop timer on match */
	LPC_TIM1->TCR = 0x01;		/* start timer */
  
	/* wait until delay time has elapsed */
	while (LPC_TIM1->TCR & 0x01);
  }
  else if ( timer_num == 2 )
  {
	LPC_TIM2->TCR = 0x02;		/* reset timer */
	LPC_TIM2->PR  = 0x00;		/* set prescaler to zero */
	LPC_TIM2->MR0 = delayInMs * (PeripheralClock / 1000 - 1);
	LPC_TIM2->IR  = 0xff;		/* reset all interrrupts */
	LPC_TIM2->MCR = 0x04;		/* stop timer on match */
	LPC_TIM2->TCR = 0x01;		/* start timer */
  
	/* wait until delay time has elapsed */
	while (LPC_TIM2->TCR & 0x01);
  }
  else if ( timer_num == 3 )
  {
	LPC_TIM3->TCR = 0x02;		/* reset timer */
	LPC_TIM3->PR  = 0x00;		/* set prescaler to zero */
	LPC_TIM3->MR0 = delayInMs * (PeripheralClock / 1000 - 1);
	LPC_TIM3->IR  = 0xff;		/* reset all interrrupts */
	LPC_TIM3->MCR = 0x04;		/* stop timer on match */
	LPC_TIM3->TCR = 0x01;		/* start timer */
  
	/* wait until delay time has elapsed */
	while (LPC_TIM3->TCR & 0x01);
  }
  return;
}

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void TIMER0_IRQHandler (void) 
{  
	static u8 num=0;
	static u8 modscount=0;
	u8 i;
    LPC_TIM0->IR = 0x1<<0;		/* clear interrupt flag */
	//timer0_counter++;
	
	
	
//    Key_Scan();
	if(SystemStatus==SYS_STATUS_TEST)
	{
		num++;
		if(num>9)//10mS??
		{
			num=0;
			F_100ms=TRUE;//100ms????
			
			Test_Value.Time++;
			
		}
	}
	else
	{
		if(num!=0)
			num=0;
	}	
	if(U9001_Save_sys.U9001_SYS.bussmode == 0)
	{
		modscount++;
		if(modscount > 9)
		{
			modscount=0;
			Tick_10ms ++;
					MODS_Poll();
		}
	}
	for (i=0;i<MAXTSOFTTIMER;i++)
	{
		if (SoftTimer[i])
			SoftTimer[i]--;
		
	}
  return;
}

/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void TIMER1_IRQHandler (void)  
{  
   static u8 HV_Ledcount=0; 
	LPC_TIM1->IR = 0x1<<0;		/* clear interrupt flag */
  //timer1_counter++; F_100ms==TRUE
	HV_Ledcount++;
	if(GetSystemStatus()==SYS_STATUS_TEST)
	{
		if(HV_Ledcount==10)
		{
			//HV_Ledcount=0;
			HV_Ledon();
		
		}
		else if(HV_Ledcount>=20)
		{
			HV_Ledcount=0;
			HV_Ledoff();
		
		}
	}
	
  return;
}

/******************************************************************************
** Function name:		Timer2_IRQHandler
**
** Descriptions:		Timer/Counter 2 interrupt handler
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void TIMER2_IRQHandler (void)  
{  
  LPC_TIM2->IR = 1;			/* clear interrupt flag */
  timer2_counter++;
  return;
}

/******************************************************************************
** Function name:		Timer3_IRQHandler
**
** Descriptions:		Timer/Counter 3 interrupt handler
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void TIMER3_IRQHandler (void)  
{  
  LPC_TIM3->IR = 1;			/* clear interrupt flag */
  timer3_counter++;
  return;
}

/******************************************************************************
** Function name:		enable_timer
**
** Descriptions:		Enable timer
**
** parameters:			timer number: 0 or 1 or 2 or 3
** Returned value:		None
** 
******************************************************************************/
void enable_timer( uint8_t timer_num )
{
  if ( timer_num == 0 )
  {
	LPC_TIM0->TCR = 1;
  }
  else if ( timer_num == 1 )
  {
	LPC_TIM1->TCR = 1;
  }
  else if ( timer_num == 2 )
  {
	LPC_TIM2->TCR = 1;
  }
  else if ( timer_num == 3 )
  {
	LPC_TIM3->TCR = 1;
  }
  return;
}

/******************************************************************************
** Function name:		disable_timer
**
** Descriptions:		Disable timer
**
** parameters:			timer number: 0 or 1 oe 2 or 3
** Returned value:		None
** 
******************************************************************************/
void disable_timer( uint8_t timer_num )
{
  if ( timer_num == 0 )
  {
	LPC_TIM0->TCR = 0;
  }
  else if ( timer_num == 1 )
  {
	LPC_TIM1->TCR = 0;
  }
  else if ( timer_num == 2 )
  {
	LPC_TIM2->TCR = 0;
  }
  else if ( timer_num == 3 )
  {
	LPC_TIM2->TCR = 0;
  }
  return;
}

/******************************************************************************
** Function name:		reset_timer
**
** Descriptions:		Reset timer
**
** parameters:			timer number: 0 or 1 or 2 or 3
** Returned value:		None
** 
******************************************************************************/
void reset_timer( uint8_t timer_num )
{
  uint32_t regVal;

  if ( timer_num == 0 )
  {
	regVal = LPC_TIM0->TCR;
	regVal |= 0x02;
	LPC_TIM0->TCR = regVal;
  }
  else if ( timer_num == 1 )
  {
	regVal = LPC_TIM1->TCR;
	regVal |= 0x02;
	LPC_TIM1->TCR = regVal;
  }
  else if ( timer_num == 2 )
  {
	regVal = LPC_TIM2->TCR;
	regVal |= 0x02;
	LPC_TIM2->TCR = regVal;
  }
  else if ( timer_num == 3 )
  {
	regVal = LPC_TIM3->TCR;
	regVal |= 0x02;
	LPC_TIM3->TCR = regVal;
  }
  return;
}

/******************************************************************************
** Function name:		init_timer
**
** Descriptions:		Initialize timer, set timer interval, reset timer,
**						install timer interrupt handler
**
** parameters:			timer number and timer interval
** Returned value:		true or false, if the interrupt handler can't be
**						installed, return false.
** 
******************************************************************************/
uint32_t init_timer ( uint8_t timer_num, uint32_t TimerInterval ) 
{
  if ( timer_num == 0 )
  {
	timer0_counter = 0;
	LPC_SC->PCONP |= (0x01<<1);
	LPC_TIM0->TCR |= (1<<1);//复位定时器
	LPC_TIM0->PR   = 54540-1;//分频
	LPC_TIM0->MR0 = TimerInterval;
	LPC_TIM0->MCR = 3;				/* Interrupt and Reset on MR0 */

	NVIC_EnableIRQ(TIMER0_IRQn);
	return (TRUE);
  }
  else if ( timer_num == 1 )
  {
	timer1_counter = 0;
	LPC_SC->PCONP |= (0x1<<2);
	  LPC_TIM1->TCR |= (1<<1);//复位定时器
	  LPC_TIM1->PR   = 60000-1;//分频
	LPC_TIM1->MR0 = TimerInterval;
	LPC_TIM1->MCR = 3;				/* Interrupt and Reset on MR0 */

	NVIC_EnableIRQ(TIMER1_IRQn);
	return (TRUE);
  }
  else if ( timer_num == 2 )
  {
	timer2_counter = 0;
	LPC_SC->PCONP |= (0x01<<22);
	LPC_TIM2->MR0 = TimerInterval;
	LPC_TIM2->MCR = 3;				/* Interrupt and Reset on MR0 */

	NVIC_EnableIRQ(TIMER2_IRQn);
	return (TRUE);
  }
  else if ( timer_num == 3 )
  {
	timer3_counter = 0;
	LPC_SC->PCONP |= (0x01<<23);
	LPC_TIM3->MR0 = TimerInterval;
	LPC_TIM3->MCR = 3;				/* Interrupt and Reset on MR0 */

	NVIC_EnableIRQ(TIMER3_IRQn);
	return (TRUE);
  }
  return (FALSE);
}

/******************************************************************************
**                            End Of File
******************************************************************************/
