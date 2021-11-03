#include "lpc177x_8x_pwm.h"
#include "lpc177x_8x_clkpwr.h"
#include "lpc177x_8x_pinsel.h"
#include  "pwm.h"

void pwm_intt(uint32_t buad)
{
	PWM_TIMERCFG_Type PWMCfgDat;
	PWM_MATCHCFG_Type PWMMatchCfgDat;
	PWMCfgDat.PrescaleOption = PWM_TIMER_PRESCALE_TICKVAL;
	PWMCfgDat.PrescaleValue = 1;
	PWM_Init(1, PWM_MODE_TIMER, (void *) &PWMCfgDat);

	PINSEL_ConfigPin (1, 18, 2);
	PWM_MatchUpdate(1, 0, buad, PWM_MATCH_UPDATE_NOW);
	PWMMatchCfgDat.IntOnMatch = DISABLE;
	PWMMatchCfgDat.MatchChannel = 0;
	PWMMatchCfgDat.ResetOnMatch = ENABLE;
	PWMMatchCfgDat.StopOnMatch = DISABLE;
	PWM_ConfigMatch(1, &PWMMatchCfgDat);
	PWM_ChannelConfig(1, 1, PWM_CHANNEL_SINGLE_EDGE);
	PWM_MatchUpdate(1, 1, buad/2, PWM_MATCH_UPDATE_NOW);

		/* Configure match option */
//		PWMMatchCfgDat.IntOnMatch = DISABLE;
//		PWMMatchCfgDat.MatchChannel = 60;
//		PWMMatchCfgDat.ResetOnMatch = DISABLE;
//		PWMMatchCfgDat.StopOnMatch = DISABLE;
//		PWM_ConfigMatch(1, &PWMMatchCfgDat);
//        
//        PWM_ResetCounter(1);

	PWM_CounterCmd(1, ENABLE);

	/* Start PWM now */
	PWM_Cmd(1, ENABLE);

		/* Enable PWM Channel Output */
		PWM_ChannelCmd(1, 1, ENABLE);
}
