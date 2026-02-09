/*
 * calibration_imu.c
 *
 *  Created on: Jan 22, 2026
 *      Author: Cristian
 */


#include "imu_calibration.h"
#include "imu_values.h"
#include "imu.h"
#include "timer_handler.h"
#include "operation_modes.h"

extern IMU_Values_t IMU;
extern Flags_Operation_t flagsSM;

extern Timer_Values_t timer;

Calib_States_t state = ACUMULACION;

uint8_t Acc_Gyro_Calibration (void)
{
	/*
	 @brief Máquina de estados que realiza la calibración del ACC y GYRO.
	 	 	Cuenta con 3 estados:
	 	 	 * ACUMULACION: Toma mediciones desde el IMU y la suma
	 	 	 * CALCULO_BIAS: Promedia la suma de todas las mediciones y obtiene el bias
	 	 	 * FIN_CALIBRACION: Reinicia los valores y avisa que finalizó la calibración

	 @return return_value: Si es TRUE la calibración terminó y avisa a la SM Principal que ya puede salir de modo CALIBRACION
	 */
	static float ax=0, ay=0, az=0, gx=0, gy=0, gz=0;
	static uint8_t n=0;
	uint8_t return_value=FALSE;
	Timer_Values_t *ptim = &timer;

	switch(state)
	{
		case ACUMULACION:
							if(ptim->measurement_time_end==FALSE) state=ACUMULACION;
							else
							{
								ptim->measurement_time_end=FALSE;
								MPUxx50_Acc_Read();
								MPUxx50_Gyro_Read();

								ax+=IMU.Ax;
								ay+=IMU.Ay;
								az+=IMU.Az;

								gx+=IMU.Gx;
								gy+=IMU.Gy;
								gz+=IMU.Gz;

								n++;

								if(n%10==0) flagsSM.sendPercentToUart=TRUE;

								if(n<N_MUESTRAS_MAX)
								{
									state = ACUMULACION;
								}
								else
								{
									state = CALCULO_BIAS;
								}
							}
							return_value=FALSE;
							break;
		case CALCULO_BIAS:
							IMU.Ax_bias = ax/n;
							IMU.Ay_bias = ay/n;
							IMU.Az_bias = az/n-1;

							IMU.Gx_bias = gx/n;
							IMU.Gy_bias = gy/n;
							IMU.Gz_bias = gz/n;

							state=FIN_CALIBRACION;
							return_value=FALSE;
							break;
		case FIN_CALIBRACION:
							ax=0;
							ay=0;
							az=0;
							gx=0;
							gy=0;
							gz=0;
							n=0;
							state=ACUMULACION;
							return_value=TRUE;
							break;
		default:
					ax=0;
					ay=0;
					az=0;
					gx=0;
					gy=0;
					gz=0;
					n=0;
					state=ACUMULACION;
					return_value=FALSE;
					break;
	}
	return return_value;
}


uint8_t Mag_Calibration (void)
{
	/*
	 @brief Máquina de estados que realiza la calibración del MAG.
	 	 	Cuenta con 3 estados:
	 	 	 * ACUMULACION: Toma mediciones desde el IMU y busca los max y min
	 	 	 * CALCULO_BIAS: Busca el valor medio sumando el max y min y dividiendo por 2
	 	 	 * FIN_CALIBRACION: Reinicia los valores y avisa que finalizó la calibración

	 @return return_value: Si es TRUE la calibración terminó y avisa a la SM Principal que ya puede salir de modo CALIBRACION
	 */
	static float mx_max=MAX_VALUE_uT, mx_min=-MAX_VALUE_uT;
	static float my_max=MAX_VALUE_uT, my_min=-MAX_VALUE_uT;
	static float mz_max=MAX_VALUE_uT, mz_min=-MAX_VALUE_uT;
	static uint8_t n=0;
	uint8_t return_value=FALSE;
	Timer_Values_t *ptim = &timer;

	switch(state)
	{
		case ACUMULACION:
							if(ptim->measurement_time_end==FALSE) state = ACUMULACION;
							else
							{
								ptim->measurement_time_end=FALSE;
								MPUxx50_Mag_Read();

								if(IMU.Mx>mx_max) mx_max = IMU.Mx;
								if(IMU.My>my_max) my_max = IMU.My;
								if(IMU.Mz>mz_max) mz_max = IMU.Mz;

								if(IMU.Mx<mx_min) mx_min = IMU.Mx;
								if(IMU.My<my_min) my_min = IMU.My;
								if(IMU.Mz<mz_min) mz_min = IMU.Mz;

								n++;

								if(n%10==0) flagsSM.sendPercentToUart=TRUE;

								if(n<N_MUESTRAS_MAX) state = ACUMULACION;
								else
								{
									state = CALCULO_BIAS;
								}

							}
							return_value = FALSE;
							break;
		case CALCULO_BIAS:
							IMU.Mx_bias = (mx_max + mx_min)/2;
							IMU.My_bias = (my_max + my_min)/2;
							IMU.Mz_bias = (mz_max + mz_min)/2;

							state=FIN_CALIBRACION;
							return_value = FALSE;
							break;
		case FIN_CALIBRACION:
							mx_max= MAX_VALUE_uT;
							my_max= MAX_VALUE_uT;
							mz_max= MAX_VALUE_uT;

							mx_min= -MAX_VALUE_uT;
							my_min= -MAX_VALUE_uT;
							mz_min= -MAX_VALUE_uT;

							n=0;
							state=ACUMULACION;
							return_value=TRUE;
							break;
		default:
					mx_max= MAX_VALUE_uT;
					my_max= MAX_VALUE_uT;
					mz_max= MAX_VALUE_uT;

					mx_min= -MAX_VALUE_uT;
					my_min= -MAX_VALUE_uT;
					mz_min= -MAX_VALUE_uT;

					n=0;
					state=ACUMULACION;
					return_value=FALSE;
					break;
	}

	return return_value;
}
