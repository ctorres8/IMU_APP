/*
 * calibration_imu.h
 *
 *  Created on: Jan 22, 2026
 *      Author: Cristian
 */

#ifndef INC_IMU_CALIBRATION_H_
#define INC_IMU_CALIBRATION_H_
#include <stdint.h>
//Estados de la SM de calibración ACC y GYRO


typedef enum{
	ACUMULACION=0,
	CALCULO_BIAS=1,
	FIN_CALIBRACION = 2,
}Calib_States_t;

#define SI 1
#define NO 0

#define TRUE 1
#define FALSE 0

#define N_MUESTRAS_MAX 100
#define CANT_MAX_OFFSETS 5

#define MAX_VALUE_uT 2000

uint8_t Mag_Calibration (void);
uint8_t Acc_Gyro_Calibration (void);

#endif /* INC_IMU_CALIBRATION_H_ */
