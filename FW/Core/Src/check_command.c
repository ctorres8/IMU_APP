/*
 * check_command.c
 *
 *  Created on: Jan 29, 2026
 *      Author: Cristian
 */
#include <stdint.h>
#include "operation_modes.h"
#include "uart_handler.h"

extern IMU_Data_Rx_t imu_rx;
extern Flags_Operation_t flagsSM;


void CheckCommand(void)
{
	/*
	 * @brief:	Verifica la trama recibida y ejecuta diferentes acciones.
	  		 	Lista de comandos:
	 	 	 	 * DEF00: CONNECT
	 	 	 	 * DEF01: STATUS
	 	 	 	 * DEF02: DISCONNECT
	 	 	 	 * DEF10: CALIB_AYG
	 	 	 	 * DEF11: CALIB_MAG
	 */

	Flags_Operation_t *flag = &flagsSM;

	if(!strcmp(imu_rx.buffer_rx,"DEF00"))
	{
		printf("\n ::COMANDO:: CONNECT");
		flag->isConnected=TRUE;
	}
	else if(!strcmp(imu_rx.buffer_rx,"DEF01"))
	{
		printf("\n ::COMANDO:: STATUS");
	}
	else if(!strcmp(imu_rx.buffer_rx,"DEF02"))
	{
		printf("\n ::COMANDO:: DISCONNECT");
		flag->isConnected=FALSE;
	}
	else if(!strcmp(imu_rx.buffer_rx,"DEF10"))
	{
		printf("\n ::COMANDO:: CALIB ACC y GYRO");
		flag->startCalib=CALIB_ACC_GYRO;
	}
	else if(!strcmp(imu_rx.buffer_rx,"DEF11"))
	{
		printf("\n ::COMANDO:: CALIB MAG");
		flag->startCalib=CALIB_MAG;
	}
	else
	{
		printf("\n ::COMANDO:: Desconocido.");
	}

	UART_MDE_Reset_Values(); //Reseteo la trama y los flags de recepción de la uart.
}
