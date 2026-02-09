/*
 * timer.c
 *
 *  Created on: Oct 27, 2025
 *      Author: Cristian
 */

//#include "main.h"
//#include <stdio.h>
#include "operation_modes.h"
#include <timer_handler.h>

Timer_Values_t timer;

extern Flags_Operation_t flagsSM;
extern TIM_HandleTypeDef htim4;



void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	/*
	 * @brief	Callback del timer que interrumpe cada 1 ms.
	 */

	Timer_Values_t *ptim = &timer;

	if (htim->Instance == htim4.Instance) // El timer 4 disparó una interrupción
	{
		// CÓDIGO QUE SE EJECUTARÁ CADA 1ms
		ptim->counter_1ms++;
		ptim->counter_measures++;
		ptim->counter_calib++;

		if(ptim->counter_calib>=TIME_100MS)
		{
			ptim->counter_calib=0;
			ptim->measurement_time_end = TRUE;

		}
		if(ptim->counter_measures>=TIME_200MS)
		{
			//printf("\n::TIMER :: Pasaron 200ms.\n");
			ptim->counter_measures=0;
			flagsSM.refreshValues=TRUE;
		}
	}
}

void Timer_Init(void)
{
	Timer_Values_t *ptim = &timer;
	ptim->counter_1ms=0;
	ptim->counter_measures=0;
	ptim->counter_calib=0;
	ptim->measurement_time_end=FALSE;
}
