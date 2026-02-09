/*
 * kalman.c
 *
 *  Created on: Nov 21, 2025
 *      Author: Cristian
 */


#include <kalman_imu_axes.h>
#include "imu_values.h"


Kalman_Axes_t Kalman_Axes;

extern IMU_Values_t IMU;

float Kalman_Update_Accel(Kalman_t *K, float Zn)
{
	/*
	 * @brief 	Estimador de Kalman por eje
	 	 	 	 * Kn: Ganancia de Kalman
	 	 	 	 * Pn: Error de la estimación un instante anterior
	 	 	 	 * Rn: Ruido de la medición
	 	 	 	 * state: estimado actual
	 * @param:
	  			* K: Puntero de Kalman a la estructura con los valores
	  			* Zn: Medición actual.
	 * @return:
	 			* La nueva estimación (float)
	 */

	// PREDICCION
	K->Pn = K->Pn + Q_KALMAN;


	// CORRECCION
	K->Kn = K->Pn/(K->Pn + Rn_ACC);

	K->Pn = ( 1 - K->Kn)*(K->Pn);

	K->state = K->state + K->Kn*(Zn - K->state);

	return K->state;
}

void Kalman_Filter(void)
{
	/*
	 * @brief Cargo los valores estimados a la struct IMU principal
	 */

	Kalman_Axes_t *p_axes = &Kalman_Axes;


	IMU.Ax_est = Kalman_Update_Accel(&(p_axes->Ax),IMU.Ax);
	IMU.Ay_est = Kalman_Update_Accel(&(p_axes->Ay),IMU.Ay);
	IMU.Az_est = Kalman_Update_Accel(&(p_axes->Az),IMU.Az);

	IMU.Gx_est = Kalman_Update_Accel(&(p_axes->Gx),IMU.Gx);
	IMU.Gy_est = Kalman_Update_Accel(&(p_axes->Gy),IMU.Gy);
	IMU.Gz_est = Kalman_Update_Accel(&(p_axes->Gz),IMU.Gz);

	IMU.Mx_est = Kalman_Update_Accel(&(p_axes->Mx),IMU.Mx);
	IMU.My_est = Kalman_Update_Accel(&(p_axes->My),IMU.My);
	IMU.Mz_est = Kalman_Update_Accel(&(p_axes->Mz),IMU.Mz);
}

void Kalman_Reset_Values(Kalman_t *K)
{
	/*
	 * @brief Reinicio valores del estimado de Kalman para cada eje.
	 */
	K->Kn=0;
	K->Pn=0;
	K->state=0;
	K->Q=Q_KALMAN;
	K->Rn= Rn_ACC;
}

void Kalman_Init(void)
{
	/*
	 * @brief	Reseteo los ejes de cada sensor.
	 	 	 	 * Ax,Ay,Az : Acelerometro
	 	 	 	 * Gx,Gy,Gz : Giróscopo
	 	 	 	 * Mx,My,Mz : Magnetómetro
	 */
	Kalman_Axes_t *p_axes = &Kalman_Axes;

	Kalman_Reset_Values(&(p_axes->Ax));
	Kalman_Reset_Values(&(p_axes->Ay));
	Kalman_Reset_Values(&(p_axes->Az));

	Kalman_Reset_Values(&(p_axes->Gx));
	Kalman_Reset_Values(&(p_axes->Gy));
	Kalman_Reset_Values(&(p_axes->Gz));

	Kalman_Reset_Values(&(p_axes->Mx));
	Kalman_Reset_Values(&(p_axes->My));
	Kalman_Reset_Values(&(p_axes->Mz));
}
