/*
 * estimador.c
 *
 *  Created on: Sep 26, 2025
 *      Author: Cristian
 */

#include <stdio.h>
#include <stdint.h>
#include "ICM20602.h"
#include "main.h"
#include "data_handler.h"

extern UART_HandleTypeDef huart3;

IMUData_t imu_data_tx;
char buffer_rx[10];

extern float Ax_raw,Ay_raw,Az_raw;
extern float Gx_raw,Gy_raw,Gz_raw;
extern float Mx_raw,My_raw,Mz_raw;

extern double bias_ax,bias_ay,bias_az;
extern double bias_gx,bias_gy,bias_gz;
extern double bias_mx,bias_my,bias_mz;

float Ax_f=0, Ay_f=0, Az_f=0;
float Gx_f=0, Gy_f=0, Gz_f=0;
float Mx_f=0, My_f=0, Mz_f=0;

float Ax_est=0, Ay_est=0, Az_est=0;
float Gx_est=0, Gy_est=0, Gz_est=0;
float Mx_est=0, My_est=0, Mz_est=0;

void FilterValues(void)
{
	//Obtengo los valores filtrados
	Ax_f=Ax_raw-bias_ax;
	Ay_f=Ay_raw-bias_ay;
	Az_f=Az_raw-bias_az;

	Gx_f=Gx_raw-bias_gx;
	Gy_f=Gy_raw-bias_gy;
	Gz_f=Gz_raw-bias_gz;

	Mx_f=Mx_raw-bias_mx;
	My_f=My_raw-bias_my;
	Mz_f=Mz_raw-bias_mz;

	printf("\n::FILTRO:: Valores filtrados. \n");

}

void EstimateValues (void)
{
	static uint32_t i=0;

	//estimacion

	Ax_est= Ax_est*i/(i+1) + Ax_f/(i+1);
	Ay_est= Ay_est*i/(i+1) + Ay_f/(i+1);
	Az_est= Az_est*i/(i+1) + Az_f/(i+1);

	Gx_est= Gx_est*i/(i+1) + Gx_f/(i+1);
	Gy_est= Gy_est*i/(i+1) + Gy_f/(i+1);
	Gz_est= Gz_est*i/(i+1) + Gz_f/(i+1);

	Mx_est= Mx_est*i/(i+1) + Mx_f/(i+1);
	My_est= My_est*i/(i+1) + My_f/(i+1);
	Mz_est= Mz_est*i/(i+1) + Mz_f/(i+1);

	i++;

	printf("\n::ESTIMADOR:: Valores estimados. \n");
}

void ShowRAWValues(void)
{
	printf("\n**VALORES RAW**\n");
	printf("Ax: %.4f , Ay: %.4f , Az: %.4f\n",Ax_raw,Ay_raw,Az_raw);
	printf("Gx: %.4f , Gy: %.4f , Gz: %.4f\n",Gx_raw,Gy_raw,Gz_raw);
	printf("Mx: %.4f , My: %.4f , Mz: %.4f\n\n",Mx_raw,My_raw,Mz_raw);
}

void ShowFilteredValues(void)
{
	printf("\n**VALORES FILTRADOS**\n");
	printf("Ax: %.4f , Ay: %.4f , Az: %.4f\n",Ax_f,Ay_f,Az_f);
	printf("Gx: %.4f , Gy: %.4f , Gz: %.4f\n",Gx_f,Gy_f,Gz_f);
	printf("Mx: %.4f , My: %.4f , Mz: %.4f\n\n",Mx_f,My_f,Mz_f);
}

void ShowEstimatedValues(void)
{
	printf("\n**VALORES ESTIMADOS**\n");
	printf("Ax(est): %.4f , Ay(est): %.4f , Az(est): %.4f\n",Ax_est,Ay_est,Az_est);
	printf("Gx(est): %.4f , Gy(est): %.4f , Gz(est): %.4f\n",Gx_est,Gy_est,Gz_est);
	printf("Mx(est): %.4f , My(est): %.4f , Mz(est): %.4f\n\n",Mx_est,My_est,Mz_est);
}

void LoadDataPacket(void)
{
	imu_data_tx.start = 0xAA;

	imu_data_tx.Ax_f = Ax_f;
	imu_data_tx.Ay_f = Ay_f;
	imu_data_tx.Az_f = Az_f;

	imu_data_tx.Gx_f = Gx_f;
	imu_data_tx.Gy_f = Gy_f;
	imu_data_tx.Gz_f = Gz_f;

	imu_data_tx.Mx_f = Mx_f;
	imu_data_tx.My_f = My_f;
	imu_data_tx.Mz_f = Mz_f;

	imu_data_tx.Ax_est = Ax_est;
	imu_data_tx.Ay_est = Ay_est;
	imu_data_tx.Az_est = Az_est;

	imu_data_tx.Gx_est = Gx_est;
	imu_data_tx.Gy_est = Gy_est;
	imu_data_tx.Gz_est = Gz_est;

	imu_data_tx.Mx_est = Mx_est;
	imu_data_tx.My_est = My_est;
	imu_data_tx.Mz_est = Mz_est;

	imu_data_tx.checksum = 0x00;
}

uint8_t sendPacket(void)
{
	uint8_t msg=0;
	msg=HAL_UART_Transmit(&huart3, (uint8_t *) &imu_data_tx, sizeof(imu_data_tx), HAL_MAX_DELAY);
	return msg;
}

uint8_t receivePacket(void)
{
	uint8_t msg=0;

	msg=HAL_UART_Receive(&huart3, (uint8_t *) buffer_rx, sizeof(buffer_rx), HAL_MAX_DELAY);

	//HAL_UART_RxCpltCallback(&huart3);

	//HAL_UART_Receive_IT(huart, pData, Size);

	return msg;
}
