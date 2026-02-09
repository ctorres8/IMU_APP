/*
 * ICM20602.c
 *
 *  Created on: Sep 23, 2025
 *      Author: Cristian
 */


#include <imu.h>
#include "main.h"
#include "imu_values.h"
#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "data_handler.h"
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>

extern I2C_HandleTypeDef hi2c3;

IMU_Values_t IMU;


float ASAX_adj,ASAY_adj,ASAZ_adj; // Sensibilidad ajustadas


/* INICIALIZACIONES */

void MPUxx50_Init(void)
{
	/*
	 * @brief	Inicializa el sensor MPU9250/MPU6050 (ACC y GYRO)
	 	 		En caso de ser MPU9250 también inicializa el Manetómetro AK8963
	 	 	 	 * Escalas elegidas: 2g (ACC) / 250dps (GYRO)
	 	 	 	 * Fs=100Hz
	 */

	uint8_t check=0, data=0;


	HAL_I2C_Mem_Read(&hi2c3, I2C_IMU_ADDR, WHO_AM_I_REG, 1, &check, 1, 1000);

	if (check == WHOAMI_MPU9250_LOW || check == WHOAMI_MPU9250_HIGH || check == WHOAMI_MPU6050_LOW || check == WHOAMI_MPU6050_HIGH)
	{
		IMU.wia=check;

		//PWR MGMNT
		data=0x00;
		HAL_I2C_Mem_Write(&hi2c3, I2C_IMU_ADDR, PWR_MGMT_1_REG, 1, &data, 1, 1000);

		//USER_CTRL_REG
		data=0x00;
		HAL_I2C_Mem_Write(&hi2c3, I2C_IMU_ADDR, USER_CTRL_REG, 1, &data, 1, 1000);

		// Set data rate de 100 hz
		data = 0x09;
		HAL_I2C_Mem_Write(&hi2c3, I2C_IMU_ADDR, SMPLRT_DIV_REG, 1, &data, 1, 1000);

		// SET ACC
		data=0x00; // 0x00: 2g , 0x08: 4g , 0x10: 8g , 0x18: 16g ,
		HAL_I2C_Mem_Write(&hi2c3, I2C_IMU_ADDR, ACCEL_CONFIG_REG, 1, &data, 1, 1000);

		// SET GYRO
		data=0x00; // 0x00: 250dps,0x08:500dps , 0x10: 1000dps, 0x18: 2000dps
		HAL_I2C_Mem_Write(&hi2c3, I2C_IMU_ADDR, GYRO_CONFIG_REG, 1, &data, 1, 1000);


		if(check == WHOAMI_MPU9250_LOW || check == WHOAMI_MPU9250_HIGH)
		{
			AK8963_Init();
		}

		IMU_Values_Reset(); // Inicializa la struct con las mediciones

	}

}

void AK8963_Init(void)
{
	/*
	 *@brief Inicializa el magnetómetro AK8963.
	 	 	 * 16 bits,
	 	 	 * Medición continua,
	 	 	 * 100Hz
	 */

    uint8_t data=0,msg=5;


    //Habilito  ByPass bit para la comunicación entre el MPU y el AK8963
	data=0x02;
	HAL_I2C_Mem_Write(&hi2c3, I2C_IMU_ADDR, INT_PIN_CFG_REG, 1, &data, 1, 1000);
	HAL_Delay(10);


	if(AK8963_WhoIAm()==0x48)
	{
		//Establezco el Modo PowerDown en el AK8963 para la manipulación en los registros

		data=0x00;
		msg=0x05;
		//msg=HAL_I2C_Mem_Write(&hi2c3, AK8963_ADDR<<1, MAG_CONTL_REG, 1, &data, 1, 1000);
		msg=HAL_I2C_Mem_Write(&hi2c3, AK8963_ADDR, MAG_CONTL_REG, 1, &data, 1, 1000);

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


		//Corrección de la sensibilidad del AK8963

		data=0x0F; // Modo ROM FUSE para medición de la sensibilidad
		msg=0x05;
		//msg=HAL_I2C_Mem_Write(&hi2c3, AK8963_ADDR<<1, MAG_CONTL_REG, 1, &data, 1, 1000);
		msg=HAL_I2C_Mem_Write(&hi2c3, AK8963_ADDR, MAG_CONTL_REG, 1, &data, 1, 1000);

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


		AK8963_Sens_Adjustment(); // Medición y ajuste de la sensibilidad del magnetómetro

		//data=0x00;
		//msg=0x05;
		//msg=HAL_I2C_Mem_Write(&hi2c3, AK8963_ADDR<<1, MAG_CONTL_REG, 1, &data, 1, 1000);


		//Establezco los 16bits + Medición en modo continua (100Hz)
		data=0x16; // 16bits + Modo continuo 2 (100Hz)
		msg=0x05;
		//msg=HAL_I2C_Mem_Write(&hi2c3, AK8963_ADDR<<1, MAG_CONTL_REG, 1, &data, 1, 1000);
		msg=HAL_I2C_Mem_Write(&hi2c3, AK8963_ADDR, MAG_CONTL_REG, 1, &data, 1, 1000);

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

	}

	//data=0x00; // deshabilito la comunicación entre el MPU9250 y el magnetómetro AK8963
	//HAL_I2C_Mem_Write(&hi2c3, I2C_IMU_ADDR, INT_PIN_CFG_REG, 1, &data, 1, 1000);
}

void IMU_Values_Reset (void)
{
	IMU.wia = 0;

	IMU.Ax = 0;
	IMU.Ax_est = 0;
	IMU.Ax_bias = 0;

	IMU.Ay = 0;
	IMU.Ay_est = 0;
	IMU.Ay_bias = 0;

	IMU.Az = 0;
	IMU.Az_est = 0;
	IMU.Az_bias = 0;

	IMU.Gx = 0;
	IMU.Gx_est = 0;
	IMU.Gx_bias = 0;

	IMU.Gy = 0;
	IMU.Gy_est = 0;
	IMU.Gy_bias = 0;

	IMU.Gz = 0;
	IMU.Gz_est = 0;
	IMU.Gz_bias = 0;

	IMU.Mx = 0;
	IMU.Mx_est = 0;
	IMU.Mx_bias = 0;

	IMU.My = 0;
	IMU.My_est = 0;
	IMU.My_bias = 0;

	IMU.Mz = 0;
	IMU.Mz_est = 0;
	IMU.Mz_bias = 0;

	IMU.Roll = 0;
	IMU.Roll_est = 0;

	IMU.Pitch = 0;
	IMU.Pitch_est = 0;

	IMU.Yaw = 0;
	IMU.Yaw_est = 0;
}

/*******************************************************************/
/*******************************************************************/

/* LECTURAS DEL IMU */

void MPUxx50_ReadValues(void)
{
	/*
	 * @brief: Lee los valores de todos los sensores.
	 */
	MPUxx50_Acc_Read();
	MPUxx50_Gyro_Read();
	MPUxx50_Mag_Read();
}

void MPUxx50_Acc_Read (void)
{
	/*
	 * @brief: Lee los valores del acelerómetro y los convierte en 'g'
	 */

	uint8_t Accel_x_h=0,Accel_x_l=0,Accel_y_h=0,Accel_y_l=0,Accel_z_h=0,Accel_z_l=0;
	int16_t Accel_X_RAW=0,Accel_Y_RAW=0,Accel_Z_RAW=0;


	// Leo 6 Bytes de datos desde el ACCEL_XOUT_H register

	HAL_I2C_Mem_Read(&hi2c3, I2C_IMU_ADDR, ACCEL_XOUT_H_REG, 1, &Accel_x_h, 1, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(&hi2c3, I2C_IMU_ADDR, ACCEL_XOUT_L_REG, 1, &Accel_x_l, 1, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(&hi2c3, I2C_IMU_ADDR, ACCEL_YOUT_H_REG, 1, &Accel_y_h, 1, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(&hi2c3, I2C_IMU_ADDR, ACCEL_YOUT_L_REG, 1, &Accel_y_l, 1, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(&hi2c3, I2C_IMU_ADDR, ACCEL_ZOUT_H_REG, 1, &Accel_z_h, 1, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(&hi2c3, I2C_IMU_ADDR, ACCEL_ZOUT_L_REG, 1, &Accel_z_l, 1, HAL_MAX_DELAY);

	Accel_X_RAW = (Accel_x_h <<8) | Accel_x_l;
	Accel_Y_RAW = (Accel_y_h <<8) | Accel_y_l;
	Accel_Z_RAW = (Accel_z_h <<8) | Accel_z_l;


	// Convierto los valores RAW en 'g' y le resto el sesgo
	IMU.Ax = Accel_X_RAW/ACC_SENS_2g - IMU.Ax_bias; // Divido por  la sensibilidad para 250 LSB/g
	IMU.Ay = Accel_Y_RAW/ACC_SENS_2g - IMU.Ay_bias; // Divido por  la sensibilidad para 250 LSB/g
	IMU.Az = Accel_Z_RAW/ACC_SENS_2g - IMU.Az_bias; // Divido por  la sensibilidad para 250 LSB/g


}


void MPUxx50_Gyro_Read (void)
{
	/*
	 * @brief: Lee valores crudos del giroscopo y los convierte en DPS (Degrees Per Seconds)
	 */

	uint8_t Rec_Data[6];
	int16_t Gyro_X_RAW=0,Gyro_Y_RAW=0,Gyro_Z_RAW=0;

	// Leo 6 Bytes de datos desde el GYRO_XOUT_H register

	HAL_I2C_Mem_Read(&hi2c3, I2C_IMU_ADDR, GYRO_XOUT_H_REG, 1, Rec_Data, 6, 1000);

	Gyro_X_RAW = (Rec_Data[0]<<8) | Rec_Data[1];
	Gyro_Y_RAW = (Rec_Data[2]<<8) | Rec_Data[3];
	Gyro_Z_RAW = (Rec_Data[4]<<8) | Rec_Data[5];

	 // Convierto los valores RAW en °/s y le resto el sesgo
	IMU.Gx = Gyro_X_RAW/GYRO_SENS_250DPS - IMU.Gx_bias; // Divido por  la sensibilidad para 250 LSB/(º/s)
	IMU.Gy = Gyro_Y_RAW/GYRO_SENS_250DPS - IMU.Gy_bias; // Divido por  la sensibilidad para 250 LSB/(º/s)
	IMU.Gz = Gyro_Z_RAW/GYRO_SENS_250DPS - IMU.Gz_bias; // Divido por  la sensibilidad para 250 LSB/(º/s)
}

void MPUxx50_Mag_Read(void)
{
	/*
	 * @brief: Lee los valores crudos del magnetómetro AK8963 y los convierte en uT
	 *
	 * >>>> SOLO PARA MPU9250 <<<<<<
	 */

	uint8_t Rec_Data[6];
	uint8_t flag_data_ready;
	int16_t Mag_X_RAW=0,Mag_Y_RAW=0,Mag_Z_RAW=0;

	//HAL_I2C_Mem_Read(&hi2c3, AK8963_ADDR<<1, MAG_XOUT_L_REG, 1, Rec_Data, 6, 1000);
	HAL_I2C_Mem_Read(&hi2c3, AK8963_ADDR, MAG_XOUT_L_REG, 1, Rec_Data, 6, 1000);


	Mag_X_RAW = (int16_t) ((Rec_Data[1]<<8) | Rec_Data[0]);
	Mag_Y_RAW = (int16_t) ((Rec_Data[3]<<8) | Rec_Data[2]);
	Mag_Z_RAW = (int16_t) ((Rec_Data[5]<<8) | Rec_Data[4]);



	// Convierto los valores RAW a uT
	IMU.Mx = Mag_X_RAW*ASAX_adj*0.15 - IMU.Mx_bias; // steps de 0.15 uT
	IMU.My = Mag_Y_RAW*ASAY_adj*0.15 - IMU.My_bias; // steps de 0.15 uT
	IMU.Mz = Mag_Z_RAW*ASAZ_adj*0.15 - IMU.Mz_bias; // steps de 0.15 uT

	//printf("\n::MEDICIONES MAG:: MagX: %d\t MagY: %d\t MagZ: %d \n",Mag_X_RAW,Mag_Y_RAW,Mag_Z_RAW);

	// Limpio el flag de lectura
	//HAL_I2C_Mem_Read(&hi2c3, AK8963_ADDR<<1, MAG_ST2_REG, 1, &flag_data_ready, 1, 1000);
	HAL_I2C_Mem_Read(&hi2c3, AK8963_ADDR, MAG_ST2_REG, 1, &flag_data_ready, 1, 1000);
}

//Offset de ACC y GYRO
void MPUxx50_Offset(void)
{
	int16_t Ax_o=0,Ay_o=0,Az_o=0;
	uint8_t ax_off[2],ay_off[2],az_off[2];

	HAL_I2C_Mem_Read(&hi2c3, I2C_IMU_ADDR, XA_OFFSET_H, 1, ax_off, 2, 1000);
	HAL_I2C_Mem_Read(&hi2c3, I2C_IMU_ADDR, YA_OFFSET_H, 1, ay_off, 2, 1000);
	HAL_I2C_Mem_Read(&hi2c3, I2C_IMU_ADDR, ZA_OFFSET_H, 1, az_off, 2, 1000);

	Ax_o=((ax_off[0]<<8) | ax_off[1])>>1;
	Ay_o=((ay_off[0]<<8) | ay_off[1])>>1;
	Az_o=((az_off[0]<<8) | az_off[1])>>1;

	printf("::OFFSET:: Ax=%.2f , Ay=%.2f , Az=%.2f\n",(float)Ax_o,(float)Ay_o,(float)Az_o);
}


void AK8963_Sens_Adjustment(void)
{
	/*
	 * @brief:	Ajusta la sensibilidad del magnetómetro
	 */
	uint8_t asa_x=0,asa_y=0,asa_z=0;

	//HAL_I2C_Mem_Read(&hi2c3, AK8963_ADDR<<1, MAG_ASAX_REG, 1, &asa_x, 1, 1000);
	//HAL_I2C_Mem_Read(&hi2c3, AK8963_ADDR<<1, MAG_ASAY_REG, 1, &asa_y, 1, 1000);
	//HAL_I2C_Mem_Read(&hi2c3, AK8963_ADDR<<1, MAG_ASAZ_REG, 1, &asa_z, 1, 1000);
	HAL_I2C_Mem_Read(&hi2c3, AK8963_ADDR, MAG_ASAX_REG, 1, &asa_x, 1, 1000);
	HAL_I2C_Mem_Read(&hi2c3, AK8963_ADDR, MAG_ASAY_REG, 1, &asa_y, 1, 1000);
	HAL_I2C_Mem_Read(&hi2c3, AK8963_ADDR, MAG_ASAZ_REG, 1, &asa_z, 1, 1000);

	ASAX_adj=((asa_x-128)*0.5)/128 + 1;
	ASAY_adj=((asa_y-128)*0.5)/128 + 1;
	ASAZ_adj=((asa_z-128)*0.5)/128 + 1;
}


// Verifica el desbordamiento en el magnetómetro
uint8_t AK8963_CheckOverflow(void)
{
	/*
	 * @brief: Verificar el overflow del magnetómetro
	 */
	uint8_t check_ov=0;
	HAL_I2C_Mem_Read(&hi2c3, AK8963_ADDR, MAG_ST2_REG, 1, &check_ov, 1, 1000);

	if((check_ov & 0x08))
	{
		return 1;
	}
	return 0;

}



/*******************************************************************/
/*******************************************************************/

/* WHO I AM */

uint8_t MPUxx50_WhoIAm(void)
{
	/*
	 * @brief: Obtiene los valores de ID del Sensor MPU
	 */
	uint8_t msg = 0xff;
	uint8_t wia = 0x00;

	msg=HAL_I2C_Mem_Read(&hi2c3, I2C_IMU_ADDR, WHO_AM_I_REG, 1, &wia, 1, 1000);

	if(msg==HAL_OK)
	{
		printf("\n::MPUxx50 WIA::** HAL_OK ** WIA: %x\n ",wia);
		return wia;
	}
	else if(msg==HAL_ERROR)
	{
		printf("\n::MPUxx50 WIA::** HAL_ERROR **\n ");
		return 0xff;
	}
	else
	{
		printf("\n::MPUxx50 WIA::** OTRO ** %d\n ",msg);
		return msg;
	}

	return 0xff;
}

uint8_t AK8963_WhoIAm(void)
{
	/*
	 * brief: Obtiene los valores de ID del Magnetómetro.
	 */
	uint8_t msg=0xff;
	uint8_t data=0x00;
	//AK8963_ADDR,
	//msg=HAL_I2C_Mem_Read(&hi2c3, AK8963_ADDR << 1, MAG_WIA_REG, 1, &data, 1, 1000);
	msg=HAL_I2C_Mem_Read(&hi2c3, AK8963_ADDR, MAG_WIA_REG, 1, &data, 1, 1000);

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

/*******************************************************************/
/*******************************************************************/

/* I2C Finder */

void CheckForID_I2C(void)
{
	/*
	 * @brief:	Scanner I2C para encontrar el AK8963 ADDRESS.
	 *  		Busca dispositivos conectados en el i2c Canal 2 y pregunta por su WIA
	 */
	uint8_t i=0;

	do
	{
		if(HAL_I2C_IsDeviceReady(&hi2c3, i, 1, 100) == HAL_OK) {
			printf("\n::CHECKFORID:: Dispositivo encontrado. ADDR: %x",i);
		}
		i++;
	}while(i<255);

	printf("\n::CHECKFORID:: Fin del scanner I2C.");
}


