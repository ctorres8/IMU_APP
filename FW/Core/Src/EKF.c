/*
 * kalman9dof.c
 *
 *  Created on: Nov 25, 2025
 *      Author: Cristian
 */


#include <EKF.h>
#include <stdio.h>
#include <timer_handler.h>

EulerAngle_t KALMAN_IMU;

extern IMU_Values_t IMU;
extern Timer_Values_t timer;


void EKF_ResetValues(Kalman9DOF_t *K,uint8_t angle)
{
	/*
	 @brief Reinicia parámetros de estado del filtro de Kalman

	 @param K: la estructura con los valores de cada angulo de Euler
	 @param angle: el ángulo que se va reiniciar
	 */
	K->state = 0;
	K->bias = 0;

	if(angle!=YAW_ANGLE)
	{
		K->Q_angle = 0.01;
		K->Q_bias = 0.0003;
		K->R_measure =Rn_ACC;
	}
	else
	{
		K->Q_angle = 0.001;
		K->Q_bias = 0.0003;
		K->R_measure =1.5;
	}

	K->dt=0.001;


	for(int i=0;i<2;i++)
	{
		K->K[i]=0;
		for(int j=0;j<2;j++)
		{
			if(i==j) K->P[i][j]=1;
			else
				K->P[i][j]=0;
		}
	}
}

void EKF_Init(void)
{
	/*
	 @brief: Inicializaciones de todos los angulos de EKF
	 */
	EKF_ResetValues(&KALMAN_IMU.pitch,PITCH_ANGLE);
	EKF_ResetValues(&KALMAN_IMU.roll,ROLL_ANGLE);
	EKF_ResetValues(&KALMAN_IMU.yaw,YAW_ANGLE);
}

float GetPitchAngle(IMU_Values_t IMU)
{
	/*
	 @brief: Retorno el ángulo Pitch
	 */
	return atan2(-IMU.Ax,sqrt(IMU.Ay*IMU.Ay+IMU.Az*IMU.Az));
}

float GetRollAngle(IMU_Values_t IMU)
{
	/*
	 @brief: Retorno el ángulo Roll
	 */
	return atan2(IMU.Ay,IMU.Az);
}

float GetYawAngle(IMU_Values_t IMU,float roll_angle, float pitch_angle)
{
	/*
	 @brief: Retorno el ángulo Yaw
	 */

	float cos_phi = cos(roll_angle);
	float sin_phi = sin(roll_angle);
	float cos_theta = cos(pitch_angle);
	float sin_theta = sin(pitch_angle);

	float Xh = 0,Yh=0;

	Xh=IMU.Mx*cos_theta+IMU.My*sin_phi*sin_theta+IMU.Mz*cos_phi*sin_theta;

	Yh =  IMU.Mz*sin_phi - IMU.My*cos_phi; // IMU.My*cos_phi - IMU.Mz*sin_phi;

	return atan2(Yh,Xh);
}

void Calculate_dt(void)
{
	/*
	 @brief Calcula un diferencial de tiempo para poder medir la variación entre ángulos
	 */

	static uint32_t last_time_ms=0;
	uint32_t current_time_ms = timer.counter_1ms;

	// El tiempo transcurrido, en segundos (ms / 1000.0)
	float dt_sec = (float)(current_time_ms - last_time_ms) / 1000.0f;

	// Guardar el tiempo actual para el siguiente ciclo
	last_time_ms = current_time_ms;

	// Actualizar el dt en las tres estructuras EKF
	KALMAN_IMU.roll.dt = dt_sec;
	KALMAN_IMU.pitch.dt = dt_sec;
	KALMAN_IMU.yaw.dt = dt_sec;

}

float EKF_UpdateState(Kalman9DOF_t *KF,float gyroRate, float Zn)
{
	/*
	 @brief: Actualiza el estado actual y la matriz P de cada ángulo utilizando EKF
	 */
	float w = gyroRate*DEG_TO_RAD; // °/seg -> rad/seg

	//PREDICCIÓN

	// Predicción del estado (X_k- = X_k-1 + (omega - bias) * dt
	float rate = w - KF->bias;
	KF->state += KF->dt*rate;

	// Predicción de Covarianza P (matriz P_k- = F * P_k-1 * F_T + Q)
	KF->P[0][0] += KF->dt * (KF->dt * KF->P[1][1] - KF->P[0][1] - KF->P[1][0] + KF->Q_angle);
	KF->P[0][1] -= KF->dt * KF->P[1][1];
	KF->P[1][0] -= KF->dt * KF->P[1][1];
	KF->P[1][1] += KF->Q_bias * KF->dt;

	// CORRECCIÓN (Ganancia y Actualización)
	// S = H*P_k-*H_T + R. (H = [1 0])
	float S = KF->P[0][0] + KF->R_measure;

	// Ganancia de Kalman K (K = P_k- * H_T * inv(S))
	KF->K[0] = KF->P[0][0] / S; // K_angle
	KF->K[1] = KF->P[1][0] / S; // K_bias

	// Error (y = Z_n - H * X_k-)
	float y = Zn - KF->state;

	// Actualización del Estado (X_k = X_k- + K * y)
	KF->state += KF->K[0] * y;
	KF->bias  += KF->K[1] * y;

	// Actualización de Covarianza P (P_k = (I - K*H) * P_k-)
	float P00_temp = KF->P[0][0];
	float P01_temp = KF->P[0][1];

	KF->P[0][0] -= KF->K[0] * P00_temp;
	KF->P[0][1] -= KF->K[0] * P01_temp;
	KF->P[1][0] -= KF->K[1] * P00_temp;
	KF->P[1][1] -= KF->K[1] * P01_temp;

	return KF->state;
}

void EKF(void)
{
	/*
	 @brief: Cargo los valores a la estructura principal del IMU con el ángulo actual y su estimado
	 */
	float roll=0,pitch=0,yaw=0;

	roll = GetRollAngle(IMU);
	IMU.Roll = roll*RAD_TO_DEG;
	KALMAN_IMU.roll.state = EKF_UpdateState(&KALMAN_IMU.roll, IMU.Gx, roll);

	pitch = GetPitchAngle(IMU);
	IMU.Pitch = pitch*RAD_TO_DEG;
	KALMAN_IMU.pitch.state = EKF_UpdateState(&KALMAN_IMU.pitch, IMU.Gy, pitch);

	yaw = GetYawAngle(IMU,KALMAN_IMU.roll.state,KALMAN_IMU.pitch.state);
	IMU.Yaw = yaw*RAD_TO_DEG;
	KALMAN_IMU.yaw.state = EKF_UpdateState(&KALMAN_IMU.yaw, IMU.Gz, yaw);

	IMU.Roll_est= KALMAN_IMU.roll.state*RAD_TO_DEG;
	IMU.Pitch_est= KALMAN_IMU.pitch.state*RAD_TO_DEG;
	IMU.Yaw_est= KALMAN_IMU.yaw.state*RAD_TO_DEG;

	printf("\n **EKF MEDIDOS** Roll: %.2f ° \t Pitch: %.2f ° \t Yaw: %.2f ° \n",IMU.Roll,IMU.Pitch,IMU.Yaw);
	printf("\n **EKF ESTIMADOS** Roll: %.2f ° \t Pitch: %.2f ° \t Yaw: %.2f ° \n",IMU.Roll_est,IMU.Pitch_est,IMU.Yaw_est);
}
