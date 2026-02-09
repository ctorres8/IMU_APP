/*
 * IMU_Data.h
 *
 *  Created on: Nov 21, 2025
 *      Author: Cristian
 */

#ifndef INC_IMU_DATA_H_
#define INC_IMU_DATA_H_

#include <stdint.h>

#endif /* INC_IMU_DATA_H_ */

typedef struct{
	uint8_t wia;
	float Ax, Ay, Az; // IMU Values ACC
	float Gx, Gy, Gz; // IMU Values GYRO
	float Mx, My, Mz; // IMU Values MAG

	float Ax_est, Ay_est, Az_est; // Estimates Values ACC
	float Gx_est, Gy_est, Gz_est; // Estimates Values GYRO
	float Mx_est, My_est, Mz_est; // Estimates Values MAG

	float Ax_bias, Ay_bias, Az_bias; // BIAS ACC
	float Gx_bias, Gy_bias, Gz_bias; // BIAS GYRO
	float Mx_bias, My_bias, Mz_bias; // BIAS MAG

	float Roll,Pitch,Yaw; // Euler Angles calculated
	float Roll_est,Pitch_est,Yaw_est; // Euler Angles estimates

}IMU_Values_t;

#define VERSION_FW "v1.0.0"
#define SCALE_ACC 2//g
#define SCALE_GYRO 250//dps
#define SCALE_MAG 2400//uT

void IMU_Values_Reset (void);
