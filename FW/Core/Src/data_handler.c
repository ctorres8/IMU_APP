/*
 * estimador.c
 *
 *  Created on: Sep 26, 2025
 *      Author: Cristian
 */

#include <imu.h>
#include <stdio.h>
#include <stdint.h>
#include "EKF.h"
#include "data_handler.h"
#include "main.h"
#include <string.h>

#define VERSION_FW "v1.0.0"
#define SCALE_ACC 2//g
#define SCALE_GYRO 250//dps
#define SCALE_MAG 2400//uT


IMU_Send_Packet_t imu_tx_packets;

extern IMU_Values_t IMU;



void EstimateValues (void)
{
	/*
	@brief: Realiza la estimación de los valores

	 */

	static uint32_t i=0;

	IMU.Ax_est= IMU.Ax_est*i/(i+1) + IMU.Ax/(i+1);
	IMU.Ay_est= IMU.Ay_est*i/(i+1) + IMU.Ay/(i+1);
	IMU.Az_est= IMU.Az_est*i/(i+1) + IMU.Az/(i+1);

	IMU.Gx_est= IMU.Gx_est*i/(i+1) + IMU.Gx/(i+1);
	IMU.Gy_est= IMU.Gy_est*i/(i+1) + IMU.Gy/(i+1);
	IMU.Gz_est= IMU.Gz_est*i/(i+1) + IMU.Gz/(i+1);

	IMU.Mx_est= IMU.Mx_est*i/(i+1) + IMU.Mx/(i+1);
	IMU.My_est= IMU.My_est*i/(i+1) + IMU.My/(i+1);
	IMU.Mz_est= IMU.Mz_est*i/(i+1) + IMU.Mz/(i+1);

	i++;

	//printf("\n::ESTIMADOR:: Valores estimados. \n");
}


void ShowMeasures(void)
{
	/*
	@brief: Muestra los valores filtrados por consola.
	 */

	printf("\n**MEDICIONES**\n");
	printf("Ax: %.4f , Ay: %.4f , Az: %.4f\n",IMU.Ax,IMU.Ay,IMU.Az);
	printf("Gx: %.4f , Gy: %.4f , Gz: %.4f\n",IMU.Gx,IMU.Gy,IMU.Gz);
	printf("Mx: %.4f , My: %.4f , Mz: %.4f\n\n",IMU.Mx,IMU.My,IMU.Mz);
}

void ShowEstimatedValues(void)
{
	/*
	@brief: Muestra los valores estimados por consola.
	 */

	printf("\n**VALORES ESTIMADOS**\n");
	printf("Ax(est): %.4f , Ay(est): %.4f , Az(est): %.4f\n",IMU.Ax_est,IMU.Ay_est,IMU.Az_est);
	printf("Gx(est): %.4f , Gy(est): %.4f , Gz(est): %.4f\n",IMU.Gx_est,IMU.Gy_est,IMU.Gz_est);
	printf("Mx(est): %.4f , My(est): %.4f , Mz(est): %.4f\n\n",IMU.Mx_est,IMU.My_est,IMU.Mz_est);
}

void LoadTelemetricData(void)
{
	/*
	 @brief: Se cargan los valores medidos y estimados al buffer telemétrico para ser enviado por puerto serie
	 	 	 Consiste en:
	 	 	 * 1 byte de start,
	 	 	 * 1 byte de tipo de trama,
	 	 	 * 24 floats de datos,
	 	 	 * 1 byte de checksum
	 */

	IMU_Send_Packet_t *imu_tx = &imu_tx_packets;

	imu_tx->telemetric_packet.start = 0xAA;

	imu_tx->telemetric_packet.type_frame = 0x01; // PARA MEDICIONES

	imu_tx->telemetric_packet.Ax_f = IMU.Ax;
	imu_tx->telemetric_packet.Ay_f = IMU.Ay;
	imu_tx->telemetric_packet.Az_f = IMU.Az;

	imu_tx->telemetric_packet.Gx_f = IMU.Gx;
	imu_tx->telemetric_packet.Gy_f = IMU.Gy;
	imu_tx->telemetric_packet.Gz_f = IMU.Gz;

	imu_tx->telemetric_packet.Mx_f = IMU.Mx;
	imu_tx->telemetric_packet.My_f = IMU.My;
	imu_tx->telemetric_packet.Mz_f = IMU.Mz;

	imu_tx->telemetric_packet.Roll = IMU.Roll;
	imu_tx->telemetric_packet.Pitch = IMU.Pitch;
	imu_tx->telemetric_packet.Yaw = IMU.Yaw;


	imu_tx->telemetric_packet.Ax_est = IMU.Ax_est;
	imu_tx->telemetric_packet.Ay_est = IMU.Ay_est;
	imu_tx->telemetric_packet.Az_est = IMU.Az_est;

	imu_tx->telemetric_packet.Gx_est = IMU.Gx_est;
	imu_tx->telemetric_packet.Gy_est = IMU.Gy_est;
	imu_tx->telemetric_packet.Gz_est = IMU.Gz_est;

	imu_tx->telemetric_packet.Mx_est = IMU.Mx_est;
	imu_tx->telemetric_packet.My_est = IMU.My_est;
	imu_tx->telemetric_packet.Mz_est = IMU.Mz_est;

	imu_tx->telemetric_packet.Roll_est = IMU.Roll_est;
	imu_tx->telemetric_packet.Pitch_est = IMU.Pitch_est;
	imu_tx->telemetric_packet.Yaw_est = IMU.Yaw_est;

	imu_tx->telemetric_packet.checksum = 0x00;
}

void LoadInfoData(void)
{
	/*
	 @brief: Carga los valores de información al buffer de INFO para ser enviado por puerto serie.
	  		 Consiste en:
	 	 	 *	1 byte de start,
	 	 	 *	1 byte de tipo de trama,
	 	 	 *	1 int de velocidad,
	 	 	 *	1 string de escala,
	 	 	 *	1 string de version,
	 	 	 *	1 string de modelo de imu,
	 	 	 *	1 byte checksum
	 */

	IMU_Send_Packet_t *imu_tx = &imu_tx_packets;

	imu_tx->info_packet.start = 0xAA;
	imu_tx->info_packet.type_frame = 0x02; // PARA STATUS
	imu_tx->info_packet.velocidad= 115200;
	sprintf(imu_tx->info_packet.escala,"%d g/%d dps/%d uT",SCALE_ACC,SCALE_GYRO,SCALE_MAG);
	sprintf(imu_tx->info_packet.version,"%s",VERSION_FW);

	if(IMU.wia==WHOAMI_MPU6050_LOW || IMU.wia==WHOAMI_MPU6050_HIGH)
	{
		sprintf(imu_tx->info_packet.imu_model,"MPU6050");
	}
	else if(IMU.wia==WHOAMI_MPU9250_LOW || IMU.wia==WHOAMI_MPU9250_HIGH)
	{
		sprintf(imu_tx->info_packet.imu_model,"MPU9250");
	}
	else
	{
		sprintf(imu_tx->info_packet.imu_model,"DESCONOCIDO");
	}
	imu_tx->info_packet.checksum = 0x00;
}

void LoadCalibData(uint8_t percent)
{
	/*
	 @brief: Carga los valores de porcentaje de calibración al buffer CALIB  para ser enviado por puerto serie.
	 	 	 Consiste en:
	 	 	 * 1 byte de start
	 	 	 * 1 byte de tipo de trama
	 	 	 * 1 byte de porcentaje
	 	 	 * 1 byte de checksum
	 */

	IMU_Send_Packet_t *imu_tx = &imu_tx_packets;

	imu_tx->calib_packet.start = 0xAA;
	imu_tx->calib_packet.type_frame = 0x03; // PARA CALIB
	imu_tx->calib_packet.percent = percent;
	imu_tx->calib_packet.checksum = 0x00;
}

char* getDeviceID (void)
{
	/*
	 @brief: Devuelve el tipo de placa conectada.

	 @return: El tipo de placa STM32 en la cual se está trabajando.
	 */

	uint32_t uid = HAL_GetDEVID();

	if(uid==0x413)
	{
		return "STM32F405xx/F07xx/F15xx/F17xx";
	}

	if(uid==0x419)
	{
		return "STM32F42xxx/F43xxx";
	}

	if(uid==0x421)
	{
		return "STM32F7xx";
	}

	if(uid==0x423)
	{
		return "STM32F401xB/C";
	}

	if(uid==0x433)
	{
		return "STM32F401xD/E";
	}

	return "UNKNOW DEVICE";

}
