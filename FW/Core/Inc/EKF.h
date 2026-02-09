/*
 * kalman_9dof.h
 *
 *  Created on: Nov 25, 2025
 *      Author: Cristian
 */

#ifndef INC_EKF_H_
#define INC_EKF_H_

#include <math.h>
#include "imu_values.h"

typedef struct{
	float state;
	float bias;

	float Q_angle;
	float Q_bias;
	float R_measure;
	float P[2][2];

	float K[2];
	float dt;
}Kalman9DOF_t;

typedef struct{
	Kalman9DOF_t pitch;
	Kalman9DOF_t roll;
	Kalman9DOF_t yaw;
}EulerAngle_t;


#define NOISE_DENSITY_ACC (3e-3)//3000 ug para 100Hz
#define Rn_ACC pow(NOISE_DENSITY_ACC,2) // MEASURE NOISE ACC: (0.003 g)^2

#define NOISE_DENSITY_GYRO (0.1) // °/s para 100Hz
#define Rn_GYRO pow(NOISE_DENSITY_GYRO,2) // MEASURE NOISE GYRO: (0.1 °/s)^2

#define Q_KALMAN (100*Rn_ACC)

#define RAD_TO_DEG (180.0/M_PI)
#define DEG_TO_RAD (M_PI/180.0)

#define PITCH_ANGLE 0
#define ROLL_ANGLE 1
#define YAW_ANGLE 2


void EKF_ResetValues(Kalman9DOF_t *K,uint8_t angle);
void EKF_Init(void);
void EKF(void);
float EKF_UpdateState(Kalman9DOF_t *,float, float);
float GetPitchAngle(IMU_Values_t);
float GetRollAngle(IMU_Values_t);
float GetYawAngle(IMU_Values_t IMU,float, float);

void Calculate_dt(void);


#endif /* INC_EKF_H_ */

