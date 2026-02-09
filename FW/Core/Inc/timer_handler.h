/*
 * timer.h
 *
 *  Created on: Jan 30, 2026
 *      Author: Cristian
 */

#ifndef INC_TIMER_HANDLER_H_
#define INC_TIMER_HANDLER_H_

#include "main.h"
#include <stdio.h>
#include <stdint.h>

#define FALSE 0
#define TRUE 1

#define TIME_500MS 500
#define TIME_200MS 200
#define TIME_100MS 100
#define TIME_1SEG 1000

typedef struct{
	volatile uint32_t counter_1ms;
	uint16_t counter_measures;
	uint8_t counter_calib;
	uint8_t measurement_time_end;
}Timer_Values_t;

void Timer_Init(void);

#endif /* INC_TIMER_HANDLER_H_ */
