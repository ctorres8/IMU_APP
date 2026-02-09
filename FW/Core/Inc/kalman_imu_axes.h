/*
 * kalman.h
 *
 *  Created on: Nov 21, 2025
 *      Author: Cristian
 */

#ifndef INC_KALMAN_H_
#define INC_KALMAN_H_

#include <stdint.h>
#include <math.h>

#endif /* INC_KALMAN_H_ */

typedef struct{
	float Kn;
	float Pn;
	float state;
	float Q;
	float Rn;
}Kalman_t;


typedef struct{
	Kalman_t Ax,Ay,Az;
	Kalman_t Gx,Gy,Gz;
	Kalman_t Mx,My,Mz;
}Kalman_Axes_t;



#define NOISE_DENSITY_ACC (3e-3)//3000 ug para 100Hz
#define Rn_ACC pow(NOISE_DENSITY_ACC,2) // MEASURE NOISE ACC: (0.003 g)^2

#define NOISE_DENSITY_GYRO (0.1) // °/s para 100Hz
#define Rn_GYRO pow(NOISE_DENSITY_GYRO,2) // MEASURE NOISE GYRO: (0.1 °/s)^2

#define Q_KALMAN (100*Rn_ACC)

float Kalman_Update_Accel(Kalman_t *, float);
void Kalman_Filter(void);
void Kalman_Reset_Values(Kalman_t *);
void Kalman_Init(void);
float GetPitch(void);
float GetRoll(void);
float GetYaw(void);
