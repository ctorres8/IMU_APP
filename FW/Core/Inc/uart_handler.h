/*
 * uart_handler.h
 *
 *  Created on: Oct 19, 2025
 *      Author: Cristian
 */

#include <stdio.h>
#include <stdint.h>
#include "main.h"
#include "data_handler.h"
#include <string.h>

#define MAX_LEN_BUFFER_RX 40

#define ESPERANDO 0
#define RECIBIENDO 1
#define FIN 2



typedef struct{
	volatile uint8_t data_byte;
	uint8_t flag_newframe;
	char buffer_rx[MAX_LEN_BUFFER_RX];
}IMU_Data_Rx_t;



void UART_State_Init(void);
void UART_MDE_Reset_Values(void);
uint8_t SendData(uint8_t);
void ReceiveData(void);
void FrameRecover(void);



