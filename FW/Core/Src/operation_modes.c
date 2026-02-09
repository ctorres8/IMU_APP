/*
 * process_command.c
 *
 *  Created on: Oct 30, 2025
 *      Author: Cristian
 */
#include <imu.h>
#include <kalman_imu_axes.h>
#include "imu_calibration.h"
#include "main.h"
#include "operation_modes.h"
#include "uart_handler.h"
#include "EKF.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <timer_handler.h>


Operation_Modes_t system_state = IDLE;
Flags_Operation_t flagsSM;

extern IMU_Data_Rx_t imu_rx;
extern Timer_Values_t timer;




void StateMachine(void)
{
	/*
	 * @brief 	Máquina de estados principal del sistema.
	  		 	Cuenta con cinco bloques:
	 	 	 	 * IDLE: El micro está en standby, no transmite nada
	 	 	 	 * CONECTADO: El micro recibe conexión y envía un comando de status.
	 	 	 	 * MIDIENDO: El micro comienza a obtener datos del sensor y envía las mediciones vía UART
	 	 	 	 * CALIBRANDO_ACCGYRO: El micro comienza un proceso de calibración del acelerómetro y giróscopo.
	 	 	 	 * CALIBRANDO_MAG: El micro comienza un proceso de calibración del magnetómetro
	 */
	static uint8_t percent=0;
	uint8_t end_calibration=FALSE;

	Flags_Operation_t *flag = &flagsSM;


	switch(system_state)
	{

		case IDLE:
						//printf("\n :: MODO IDLE:: \n");
						if(flag->isConnected==TRUE)
						{
							flag->dataSentOK=FALSE;
							LoadInfoData();
							SendData(SEND_INFO);
							system_state=CONECTADO;
						}
						else
						{
							system_state=IDLE;
						}
						break;
		case CONECTADO:
						//printf("\n :: MODO CONECTADO :: \n");
						if(flag->dataSentOK==FALSE)
						{
							system_state = CONECTADO;
						}
						else
						{
							flag->dataSentOK=FALSE;
							timer.counter_measures=0;
							flag->refreshValues=FALSE;
							system_state=MIDIENDO;
						}
						break;
		case MIDIENDO:
						//printf("\n :: MODO MEDICION :: \n");

						if(flag->refreshValues==TRUE)
						{
							flag->dataSentOK=FALSE;
							flag->refreshValues=FALSE;
							Calculate_dt();
							MPUxx50_ReadValues(); //Lectura de valores
							Kalman_Filter();
							EKF();
							LoadTelemetricData();
							SendData(SEND_MEASURES);
						}

						if (flag->isConnected==TRUE)
						{
							if (flag->startCalib==CALIB_ACC_GYRO)
							{
								system_state=CALIBRANDO_ACCGYRO;
							}
							else if(flag->startCalib==CALIB_MAG)
							{
								system_state=CALIBRANDO_MAG;
							}
							else
							{
								system_state=MIDIENDO;
							}
						}
						else
						{
							system_state=IDLE;
						}

						break;
		case CALIBRANDO_ACCGYRO:
						//printf("\n :: CALIBRANDO ACC Y GYRO :: \n");
						end_calibration = Acc_Gyro_Calibration();

						if(flag->sendPercentToUart==TRUE)
						{
							percent+=10;
							LoadCalibData(percent);
							SendData(0x03);
							printf("\n::CALIB ACCYGYRO:: Completado %d %%",percent);
							if(percent>=100) percent=0;
							flag->sendPercentToUart=FALSE;
						}

						if(end_calibration==FALSE)
						{
							system_state = CALIBRANDO_ACCGYRO;
						}
						else
						{
							flag->startCalib=NO_CALIB;
							flag->sendPercentToUart=FALSE;
							percent=0;
							system_state=MIDIENDO;
						}

						break;

		case CALIBRANDO_MAG:
						//printf("\n :: CALIBRANDO MAG :: \n");
						end_calibration = Mag_Calibration();

						if(flag->sendPercentToUart==TRUE)
						{
							percent+=10;
							LoadCalibData(percent);
							SendData(0x03);
							printf("\n::CALIB MAG:: Completado %d %%",percent);
							if(percent>=100) percent=0;
							flag->sendPercentToUart=FALSE;
						}

						if(end_calibration==FALSE)
						{
							system_state = CALIBRANDO_MAG;
						}
						else
						{
							flag->startCalib=NO_CALIB;
							flag->sendPercentToUart=FALSE;
							percent=0;
							system_state=MIDIENDO;
						}

						break;
		default:
					break;
	}
}

void ResetFlags(void)
{
	Flags_Operation_t *flag = &flagsSM;

	flag->isConnected=FALSE;
	flag->startCalib=NO_CALIB;
	flag->sendPercentToUart=FALSE;
	flag->refreshValues=FALSE;
	flag->dataSentOK=FALSE;
}
