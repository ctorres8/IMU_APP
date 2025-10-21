/*
 * ICM20602.c
 *
 *  Created on: Jul 28, 2025
 *      Author: Cristian
 */


#include "ICM20602.h"
#include "main.h"
#include <stdio.h>
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>


extern I2C_HandleTypeDef hi2c2;

float Ax_raw,Ay_raw,Az_raw;
float Gx_raw,Gy_raw,Gz_raw;
float Mx_raw,My_raw,Mz_raw;

float ASAX_adj,ASAY_adj,ASAZ_adj; // Sensibilidad ajustadas


double bias_ax,bias_ay,bias_az;
double bias_gx,bias_gy,bias_gz;
double bias_mx,bias_my,bias_mz;

#define CANT_MUESTRAS_CALIB 200


void MPU9250_Init(void)
{
	uint8_t check=0, data=0;

	HAL_I2C_Mem_Read(&hi2c2, MPU6050_ADDR, WHO_AM_I_REG, 1, &check, 1, 1000);

	if (check == WHOAMI_MPU9250)
	{
		//power management register
		data=0x00;
		HAL_I2C_Mem_Write(&hi2c2, MPU6050_ADDR, PWR_MGMT_1_REG, 1, &data, 1, 1000);

		// Set data rate de 1 khz
		//data = 0x07;
		//HAL_I2C_Mem_Write(&hi2c2, MPU6050_ADDR, SMPLRT_DIV_REG, 1, &data, 1, 1000);

		// Set acelerometro
		data=0x00; // 0x00: 2g , 0x08: 4g , 0x10: 8g , 0x18: 16g ,
		HAL_I2C_Mem_Write(&hi2c2, MPU6050_ADDR, ACCEL_CONFIG_REG, 1, &data, 1, 1000);

		// Set giroscopio
		data=0x00; // 0x00: 250dps,0x08:500dps , 0x10: 1000dps, 0x18: 2000dps
		HAL_I2C_Mem_Write(&hi2c2, MPU6050_ADDR, GYRO_CONFIG_REG, 1, &data, 1, 1000);


		//MPU9250_Calibration();
	}

}

void MPU9250_Acc_Read (void)
{
	//uint8_t Rec_Data[6];
	uint8_t Accel_x_h=0,Accel_x_l=0,Accel_y_h=0,Accel_y_l=0,Accel_z_h=0,Accel_z_l=0;
	int16_t Accel_X_RAW=0,Accel_Y_RAW=0,Accel_Z_RAW=0;

	// leo 6 Bytes de datos desde el ACCEL_XOUT_H register

	//HAL_I2C_Mem_Read(&hi2c2, MPU6050_ADDR, ACCEL_XOUT_H_REG, 1, Rec_Data, 6, 1000);
	HAL_I2C_Mem_Read(&hi2c2, MPU6050_ADDR, ACCEL_XOUT_H_REG, 1, &Accel_x_h, 1, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(&hi2c2, MPU6050_ADDR, ACCEL_XOUT_L_REG, 1, &Accel_x_l, 1, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(&hi2c2, MPU6050_ADDR, ACCEL_YOUT_H_REG, 1, &Accel_y_h, 1, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(&hi2c2, MPU6050_ADDR, ACCEL_YOUT_L_REG, 1, &Accel_y_l, 1, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(&hi2c2, MPU6050_ADDR, ACCEL_ZOUT_H_REG, 1, &Accel_z_h, 1, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(&hi2c2, MPU6050_ADDR, ACCEL_ZOUT_L_REG, 1, &Accel_z_l, 1, HAL_MAX_DELAY);

	//Accel_X_RAW = (Rec_Data[0]<<8) | Rec_Data[1];
	//Accel_Y_RAW = (Rec_Data[2]<<8) | Rec_Data[3];
	//Accel_Z_RAW = (Rec_Data[4]<<8) | Rec_Data[5];
	Accel_X_RAW = (Accel_x_h <<8) | Accel_x_l;
	Accel_Y_RAW = (Accel_y_h <<8) | Accel_y_l;
	Accel_Z_RAW = (Accel_z_h <<8) | Accel_z_l;


	 // Concertir los valores RAW en 'g' Mas detalles en ACCEL_CONFIG_REG

	Ax_raw = Accel_X_RAW/ACC_SENS_2g; // Divido por  la sensibilidad para 250 LSB/g
	Ay_raw = Accel_Y_RAW/ACC_SENS_2g; // Divido por  la sensibilidad para 250 LSB/g
	Az_raw = Accel_Z_RAW/ACC_SENS_2g; // Divido por  la sensibilidad para 250 LSB/g
}

void MPU9250_Gyro_Read (void)
{
	uint8_t Rec_Data[6];
	int16_t Gyro_X_RAW=0,Gyro_Y_RAW=0,Gyro_Z_RAW=0;

	// leo 6 Bytes de datos desde el GYRO_XOUT_H register

	HAL_I2C_Mem_Read(&hi2c2, MPU6050_ADDR, GYRO_XOUT_H_REG, 1, Rec_Data, 6, 1000);

	Gyro_X_RAW = (Rec_Data[0]<<8) | Rec_Data[1];
	Gyro_Y_RAW = (Rec_Data[2]<<8) | Rec_Data[3];
	Gyro_Z_RAW = (Rec_Data[4]<<8) | Rec_Data[5];

	 // Concertir los valores RAW en °/s Mas detalles en ACCEL_CONFIG_REG

	Gx_raw = Gyro_X_RAW/GYRO_SENS_250DPS; // Divido por  la sensibilidad para 250 LSB/(º/s)
	Gy_raw = Gyro_Y_RAW/GYRO_SENS_250DPS; // Divido por  la sensibilidad para 250 LSB/(º/s)
	Gz_raw = Gyro_Z_RAW/GYRO_SENS_250DPS; // Divido por  la sensibilidad para 250 LSB/(º/s)
}

void MPU9250_Mag_Read(void)
{
	uint8_t Rec_Data[6];
	int16_t Mag_X_RAW=0,Mag_Y_RAW=0,Mag_Z_RAW=0;
	//uint8_t msg=5;

	//HAL_I2C_Mem_Read(&hi2c2, AK8963_ADDR, MAG_XOUT_L_REG, 1, Rec_Data, 6, 1000);
	HAL_I2C_Mem_Read(&hi2c2, 0x0C, MAG_XOUT_L_REG, 1, Rec_Data, 6, 1000);

	/*
	if(msg==HAL_OK)
	{
		printf("\n::MAG INIT::** HAL_OK **\n ");
	}
	else if(msg==HAL_ERROR)
	{
		printf("\n::MAG INIT::** HAL_ERROR **\n ");
	}
	else
	{
		printf("\n::MAG INIT::** OTRO ** %d\n ",msg);
	}
	*/

	Mag_X_RAW = (int16_t) ((Rec_Data[1]<<8) | Rec_Data[0]);
	Mag_Y_RAW = (int16_t) ((Rec_Data[3]<<8) | Rec_Data[2]);
	Mag_Z_RAW = (int16_t) ((Rec_Data[5]<<8) | Rec_Data[4]);


	Mx_raw = Mag_X_RAW*ASAX_adj*0.15; // steps de 0.15 uT
	My_raw = Mag_Y_RAW*ASAY_adj*0.15; // steps de 0.15 uT
	Mz_raw = Mag_Z_RAW*ASAZ_adj*0.15; // steps de 0.15 uT

	//printf("MagX: %d\t MagY: %d\t MagZ: %d \n",Mag_X_RAW,Mag_Y_RAW,Mag_Z_RAW);
}

void Valores_Offset(void)
{
	int16_t Ax_o=0,Ay_o=0,Az_o=0;
	uint8_t ax_off[2],ay_off[2],az_off[2];

	HAL_I2C_Mem_Read(&hi2c2, MPU6050_ADDR, XA_OFFSET_H, 1, ax_off, 2, 1000);
	HAL_I2C_Mem_Read(&hi2c2, MPU6050_ADDR, YA_OFFSET_H, 1, ay_off, 2, 1000);
	HAL_I2C_Mem_Read(&hi2c2, MPU6050_ADDR, ZA_OFFSET_H, 1, az_off, 2, 1000);

	Ax_o=((ax_off[0]<<8) | ax_off[1])>>1;
	Ay_o=((ay_off[0]<<8) | ay_off[1])>>1;
	Az_o=((az_off[0]<<8) | az_off[1])>>1;

	printf("::OFFSET:: Ax=%.2f , Ay=%.2f , Az=%.2f\n",(float)Ax_o,(float)Ay_o,(float)Az_o);
}

void MPU9250_Calibration (void)
{
	double sum_ax=0,sum_ay=0,sum_az=0;
	double sum_gx=0,sum_gy=0,sum_gz=0;
	//double sum_mx=0,sum_my=0,sum_mz=0;
	int i=0;

	printf("\n *** CALIBRACIÓN MPU9250 ***\n");
	while(i<CANT_MUESTRAS_CALIB)
	{
		MPU9250_Acc_Read();
		MPU9250_Gyro_Read();
		//MPU9250_Mag_Read();

		sum_ax+=Ax_raw;
		sum_ay+=Ay_raw;
		sum_az+=Az_raw;

		sum_gx+=Gx_raw;
		sum_gy+=Gy_raw;
		sum_gz+=Gz_raw;

		//sum_mx+=Mx_raw;
		//sum_my+=My_raw;
		//sum_mz+=Mz_raw;

		printf("Calibrando Dispositivo... (%d de %d muestras)\n",i,CANT_MUESTRAS_CALIB);
		i++;
		HAL_Delay(100);
	}

	//Apoyado sobre una superficie horizontal 1g= 9.80655 m/s^2
	bias_ax= sum_ax/CANT_MUESTRAS_CALIB;
	bias_ay= sum_ay/CANT_MUESTRAS_CALIB;
	bias_az= 1-sum_az/CANT_MUESTRAS_CALIB;

	bias_gx= sum_gx/CANT_MUESTRAS_CALIB;
	bias_gy= sum_gy/CANT_MUESTRAS_CALIB;
	bias_gz= sum_gz/CANT_MUESTRAS_CALIB;

	//bias_mx= sum_mx/CANT_MUESTRAS_CALIB;
	//bias_my= sum_my/CANT_MUESTRAS_CALIB;
	//bias_mz= sum_mz/CANT_MUESTRAS_CALIB;


	//Valores_Offset();

	printf("\n\n *Dispositivo calibrado* \n");
	printf(":SESGO: Ax=%f , Ay=%f , Az=%f \n",bias_ax,bias_ay,bias_az);
	printf(":SESGO: Gx=%f , Gy=%f , Gz=%f \n",bias_gx,bias_gy,bias_gz);
	//printf(":SESGO: Mx=%f , My=%f , Mz=%f \n",bias_mx,bias_my,bias_mz);
}

/*
void AK8963_Calibration(void)
{
	int16_t max_mx=0,max_my=0,max_mz=0;
	int16_t min_mx=0,min_my=0,min_mz=0;
	int i=0;

	printf("\n *** CALIBRACIÓN AK8963 ***\n");

	while(i<CANT_MUESTRAS_CALIB)
	{
		MPU9250_Mag_Read();

		if(Mx_raw>max_mx)
		{
			max_mx=Mx_raw;
		}
		else if(Mx_raw<min_mx)
		{
			min_mx=Mx_raw;
		}
		else
		{
			max_mx=max_mx;
			min_mx=min_mx;
		}

		if(My_raw>max_my)
		{
			max_my=My_raw;
		}
		else if(My_raw<min_my)
		{
			min_my=My_raw;
		}
		else
		{
			max_my=max_my;
			min_my=min_my;
		}

		if(Mz_raw>max_mz)
		{
			max_mz=Mz_raw;
		}
		else if(Mz_raw<min_mz)
		{
			min_mz=Mz_raw;
		}
		else
		{
			max_mz=max_mz;
			min_mz=min_mz;
		}

		printf("Calibrando AK8963... (%d de %d muestras)\n",i,CANT_MUESTRAS_CALIB);
		i++;
		HAL_Delay(100);
	}

	bias_mx=(max_mx+min_mx)/2;
	bias_my=(max_my+min_my)/2;
	bias_mz=(max_mz+min_mz)/2;

	printf(":SESGO: Mx=%f , My=%f , Mz=%f \n",bias_mx,bias_my,bias_mz);
}
*/
void AK8963_Calibration(void)
{
	double sum_mx=0,sum_my=0,sum_mz=0;
	int i=0;

		printf("\n *** CALIBRACIÓN AK8963 ***\n");
		HAL_Delay(1000);
		i=0;
		while(i<CANT_MUESTRAS_CALIB)
		{
			MPU9250_Mag_Read();

			sum_mx+=Mx_raw;

			printf("Calibrando AK8963 Eje X... (%d de %d muestras)\n",i,CANT_MUESTRAS_CALIB);
			i++;
			HAL_Delay(100);
		}
		printf("\n\n ** Cambio a Eje Y **\n\n");
		HAL_Delay(1000);
		i=0;
		while(i<CANT_MUESTRAS_CALIB)
		{
			MPU9250_Mag_Read();

			sum_my+=My_raw;

			printf("Calibrando AK8963 Eje Y... (%d de %d muestras)\n",i,CANT_MUESTRAS_CALIB);
			i++;
			HAL_Delay(100);
		}

		printf("\n\n ** Cambio a Eje Z **\n\n");
		HAL_Delay(1000);
		i=0;
		while(i<CANT_MUESTRAS_CALIB)
		{
			MPU9250_Mag_Read();

			sum_mz+=Mz_raw;

			printf("Calibrando AK8963 Eje Z... (%d de %d muestras)\n",i,CANT_MUESTRAS_CALIB);
			i++;
			HAL_Delay(100);
		}

		bias_mx= sum_mx/CANT_MUESTRAS_CALIB;
		bias_my= sum_my/CANT_MUESTRAS_CALIB;
		bias_mz= sum_mz/CANT_MUESTRAS_CALIB;


		printf("\n\n *Dispositivo calibrado* \n");
		printf(":SESGO: Mx=%f , My=%f , Mz=%f \n",bias_mx,bias_my,bias_mz);
}

//Magnetometro
void Mag_Sensitivity_Adjustment(void)
{
	uint8_t asa_x=0,asa_y=0,asa_z=0;

	HAL_I2C_Mem_Read(&hi2c2, AK8963_ADDR, MAG_ASAX_REG, 1, &asa_x, 1, 1000);
	HAL_I2C_Mem_Read(&hi2c2, AK8963_ADDR, MAG_ASAY_REG, 1, &asa_y, 1, 1000);
	HAL_I2C_Mem_Read(&hi2c2, AK8963_ADDR, MAG_ASAZ_REG, 1, &asa_z, 1, 1000);

	ASAX_adj=((asa_x-128)*0.5)/128 + 1;
	ASAY_adj=((asa_y-128)*0.5)/128 + 1;
	ASAZ_adj=((asa_z-128)*0.5)/128 + 1;
}

int Mag_CheckOverflow(void)
{
	uint8_t check_ov=0;
	HAL_I2C_Mem_Read(&hi2c2, AK8963_ADDR, MAG_ST2_REG, 1, &check_ov, 1, 1000);

	if((check_ov & 0x08))
	{
		return 1;
	}
	return 0;

}

/*
void Mag_SelfTest(void)
{
	uint8_t data=0,i=0,msg=0;

	printf("**HAL OK: %d, HAL_ERROR: %d**\n\n",HAL_OK,HAL_ERROR);

	data=0x02; // Habilito la comunicación entre el MPU9250 y el magnetómetro AK8963
	msg=HAL_I2C_Mem_Write(&hi2c2, MPU6050_ADDR, INT_PIN_CFG_REG, 1, &data, 1, 1000);
	printf(":BYPASS: %d\n",msg);

	msg=HAL_I2C_Mem_Read(&hi2c2, AK8963_ADDR, MAG_WIA_REG, 1, &data, 1, 1000);
	if(data==0x48) printf(" ID del registro: 0x48 \n");
	else printf(" Error al obtener el ID del magnetometro, HAL: %d\n",msg);

	HAL_I2C_Mem_Read(&hi2c2, MPU6050_ADDR, INT_PIN_CFG_REG, 1, &msg, 1, 1000);
	if(msg==0x02) printf(" Registro ByPass escrito correctamente \n");
	else printf(" Error al escribir el registro ByPass");


	HAL_Delay(10);
	//(1)
	data=0x00;
	msg=HAL_I2C_Mem_Write(&hi2c2, AK8963_ADDR, MAG_CONTL_REG, 1, &data, 1, 1000);
	printf(":PWRMODE: %d\n",msg);

	//(2)
	data=0x40;
	msg=HAL_I2C_Mem_Write(&hi2c2, AK8963_ADDR, MAG_ASTC_REG, 1, &data, 1, 1000);
	printf(":ASTC SELF: %d\n",msg);

	//(3)
	data=0x08;
	msg=HAL_I2C_Mem_Write(&hi2c2, AK8963_ADDR, MAG_CONTL_REG, 1, &data, 1, 1000);
	printf(":SELFTEST MODE: %d\n",msg);

	HAL_Delay(100);
	//(4)
	while(i<3)
	{
		HAL_I2C_Mem_Read(&hi2c2, AK8963_ADDR, MAG_ST1_REG, 1, &data, 1, 1000);

		if (data&0x01)
		{
			i=3;
		}
		else
		{
			HAL_Delay(100);
			i++;
		}
	}

	//(5)
	MPU9250_Mag_Read();

	//(6)
	data=0x00;
	msg=HAL_I2C_Mem_Write(&hi2c2, AK8963_ADDR, MAG_ASTC_REG, 1, &data, 1, 1000);
	printf(":ASTC SELF: %d\n",msg);

	//(7)
	data=0x10;
	msg=HAL_I2C_Mem_Write(&hi2c2, AK8963_ADDR, MAG_CONTL_REG, 1, &data, 1, 1000);
	printf(":PWRMODE: %d\n",msg);
}
*/

void AK8963_Init(void) {
    uint8_t data=0,msg=5;

	//Mag_SelfTest();

	//*** Set Magnetómetro ***

    //Habilito  ByPass bit para la comunicación entre el MPU y el AK8963
	data=0x02;
	HAL_I2C_Mem_Write(&hi2c2, MPU6050_ADDR, INT_PIN_CFG_REG, 1, &data, 1, 1000);
	HAL_Delay(10);

	if(AK8963_WhoIAm()==0x48)
	{
		//Establezco el Modo PowerDown en el AK8963 para la manipulación en los registros
		data=0x00;
		msg=0x05;
		msg=HAL_I2C_Mem_Write(&hi2c2, AK8963_ADDR, MAG_CONTL_REG, 1, &data, 1, 1000);

		if(msg==HAL_OK)
		{
			printf("\n::MAG INIT::** HAL_OK **\n ");
		}
		else if(msg==HAL_ERROR)
		{
			printf("\n::MAG INIT::** HAL_ERROR **\n ");
		}
		else
		{
			printf("\n::MAG INIT::** OTRO ** %d\n ",msg);
		}
		//HAL_Delay(100);
		//Corrección de la sensibilidad de mediciones
		data=0x0F; // Modo ROM FUSE para medición de la sensibilidad
		msg=0x05;
		msg=HAL_I2C_Mem_Write(&hi2c2, AK8963_ADDR, MAG_CONTL_REG, 1, &data, 1, 1000);

		if(msg==HAL_OK)
		{
			printf("\n::MAG INIT FUSE::** HAL_OK **\n ");
		}
		else if(msg==HAL_ERROR)
		{
			printf("\n::MAG INIT FUSE::** HAL_ERROR **\n ");
		}
		else
		{
			printf("\n::MAG INIT FUSE::** OTRO ** %d\n ",msg);
		}
		//HAL_Delay(100);

		Mag_Sensitivity_Adjustment(); // Medición y ajuste de la sensibilidad del magnetómetro

		//Establezco los 16bits + Medición en modo continua (100Hz)
		data=0x16; // 16bits + Modo continuo 2 (100Hz)
		msg=0x05;
		msg=HAL_I2C_Mem_Write(&hi2c2, AK8963_ADDR, MAG_CONTL_REG, 1, &data, 1, 1000);

		if(msg==HAL_OK)
		{
			printf("\n::MAG INIT CTRL::** HAL_OK **\n ");
		}
		else if(msg==HAL_ERROR)
		{
			printf("\n::MAG INIT CTRL::** HAL_ERROR **\n ");
		}
		else
		{
			printf("\n::MAG INIT CTRL::** OTRO ** %d\n ",msg);
		}
		//HAL_Delay(100);

		//data=0x00; // deshabilito la comunicación entre el MPU9250 y el magnetómetro AK8963
		//HAL_I2C_Mem_Write(&hi2c2, MPU6050_ADDR, INT_PIN_CFG_REG, 1, &data, 1, 1000);

		//AK8963_Calibration();
	}

}

void CheckForID_I2C(void)
{
	/*
	 * @brief:	Scanner I2C para encontrar el AK8963 ADDRESS.
	 *  		Busca dispositivos conectados en el i2c Canal 2 y pregunta por su WIA
	 */
	uint8_t i=0;

	do
	{
		if(HAL_I2C_IsDeviceReady(&hi2c2, i, 1, 100) == HAL_OK) {
			printf("\n::CHECKFORID:: Dispositivo encontrado. ADDR: %x",i);
		}
		i++;
	}while(i<255);

	printf("\n::CHECKFORID:: Fin del scanner I2C.");
}

uint8_t AK8963_WhoIAm(void)
{
	uint8_t msg=0xff;
	uint8_t data=0x00;

	msg=HAL_I2C_Mem_Read(&hi2c2, AK8963_ADDR, MAG_WIA_REG, 1, &data, 1, 1000);

	if(msg==HAL_OK)
	{
		printf("\n::MAG WIA::** HAL_OK ** WIA: %x\n ",data);
		return data;
	}
	else if(msg==HAL_ERROR)
	{
		printf("\n::MAG WIA::** HAL_ERROR **\n ");
		return 0xff;
	}
	else
	{
		printf("\n::MAG WIA::** OTRO ** %d\n ",msg);
		return msg;
	}

	return 0xff;

}

void MPU9250_ReadValues(void)
{
	MPU9250_Acc_Read();
	MPU9250_Gyro_Read();
	MPU9250_Mag_Read();
}

/*
void MPU9250_Mag_Read2(void)
{
	uint8_t Rec_Data[6];
	int16_t Mag_X_RAW=0,Mag_Y_RAW=0,Mag_Z_RAW=0;

	HAL_Delay(10);

	HAL_I2C_Mem_Read(&hi2c2, MPU6050_ADDR, EXT_SENS_DATA_00, 1, Rec_Data, 6, 1000);

	Mag_X_RAW = (int16_t) ((Rec_Data[1]<<8) | Rec_Data[0]);
	Mag_Y_RAW = (int16_t) ((Rec_Data[3]<<8) | Rec_Data[2]);
	Mag_Z_RAW = (int16_t) ((Rec_Data[5]<<8) | Rec_Data[4]);


	Mx_raw = Mag_X_RAW*ASAX_adj*0.15; // steps de 0.15 uT
	My_raw = Mag_Y_RAW*ASAY_adj*0.15; // steps de 0.15 uT
	Mz_raw = Mag_Z_RAW*ASAZ_adj*0.15; // steps de 0.15 uT

	//printf("MagX: %d\t MagY: %d\t MagZ: %d \n",Mag_X_RAW,Mag_Y_RAW,Mag_Z_RAW);
}
*/
