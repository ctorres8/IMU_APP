/*
 * operation_modes.c
 *
 *  Created on: Oct 30, 2025
 *      Author: Cristian
 */

#include <stdint.h>

#define FALSE 0
#define TRUE 1

#define NO_CALIB 0
#define CALIB_ACC_GYRO 1
#define CALIB_MAG 2

typedef enum {
	IDLE=0,
	CONECTADO=1,
	MIDIENDO=2,
	CALIBRANDO_ACCGYRO=3,
	CALIBRANDO_MAG=4,
}Operation_Modes_t;

typedef struct{
	uint8_t isConnected;
	uint8_t startCalib;
	uint8_t sendPercentToUart;
	uint8_t refreshValues;
	uint8_t dataSentOK;
}Flags_Operation_t;


void StateMachine(void);
void CheckCommand(void);
void ResetFlags(void);

