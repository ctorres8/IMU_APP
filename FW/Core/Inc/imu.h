/*
 * ICM20602.h
 *
 *  Created on: Sep 23, 2025
 *      Author: Cristian
 */

#include <stdint.h>

#define I2C_IMU_ADDR 0xD0

#define SMPLRT_DIV_REG 0x19
#define TEMP_OUT_H_REG 0x41

#define USER_CTRL_REG 0x6A

//I2C SLAVES REGISTERS
#define I2C_MST_CTRL 0x24
#define I2C_SLV0_ADDR 0x25
#define I2C_SLV0_REG 0x26
#define I2C_SLV0_CTRL 0x27
#define I2C_SLV1_ADDR 0x28
#define I2C_SLV1_REG 0x29
#define I2C_SLV1_CTRL 0x2A
#define I2C_SLV2_ADDR 0x2B
#define I2C_SLV2_REG 0x2C
#define I2C_SLV2_CTRL 0x2D
#define I2C_SLV3_ADDR 0x2E
#define I2C_SLV3_REG 0x2F
#define I2C_SLV3_CTRL 0x30
#define I2C_SLV4_ADDR 0x31
#define I2C_SLV4_REG 0x32
#define I2C_SLV4_DO 0x33
#define I2C_SLV4_CTRL 0x34
#define I2C_SLV4_DI 0x35
#define I2C_MST_STATUS 0x36
//I2C SLAVE D/O
#define I2C_SLV0_DO 0x63
#define I2C_SLV1_DO 0x64
#define I2C_SLV2_DO 0x65
#define I2C_SLV3_DO 0x66
#define I2C_MST_DELAY_CTRL_REG 0x67

//EXT SENS DATA
#define EXT_SENS_DATA_00 0x49
#define EXT_SENS_DATA_01 0x4A
#define EXT_SENS_DATA_02 0x4B
#define EXT_SENS_DATA_03 0x4C
#define EXT_SENS_DATA_04 0x4D
#define EXT_SENS_DATA_05 0x4E
#define EXT_SENS_DATA_06 0x4F
#define EXT_SENS_DATA_07 0x50
#define EXT_SENS_DATA_08 0x51
#define EXT_SENS_DATA_09 0x52
#define EXT_SENS_DATA_10 0x53
#define EXT_SENS_DATA_11 0x54
#define EXT_SENS_DATA_12 0x55
#define EXT_SENS_DATA_13 0x56
#define EXT_SENS_DATA_14 0x57
#define EXT_SENS_DATA_15 0x58
#define EXT_SENS_DATA_16 0x59
#define EXT_SENS_DATA_17 0x5A
#define EXT_SENS_DATA_18 0x5B
#define EXT_SENS_DATA_19 0x5C
#define EXT_SENS_DATA_20 0x5D
#define EXT_SENS_DATA_21 0x5E
#define EXT_SENS_DATA_22 0x5F
#define EXT_SENS_DATA_23 0x60



//ACCELEROMETER REGISTERS
#define ACCEL_CONFIG_REG 0x1C
#define ACCEL_XOUT_H_REG 0x3B
#define ACCEL_XOUT_L_REG 0x3C
#define ACCEL_YOUT_H_REG 0x3D
#define ACCEL_YOUT_L_REG 0x3E
#define ACCEL_ZOUT_H_REG 0x3F
#define ACCEL_ZOUT_L_REG 0x40

#define XA_OFFSET_H 0x77 // XA_OFFS[14:7] contiene 8 bits
#define XA_OFFSET_L 0x78 // XA_OFFS[6:0] contiene 7 bits
#define YA_OFFSET_H 0x7A // YA_OFFS[14:7] contiene 8 bits
#define YA_OFFSET_L 0x7B // YA_OFFS[6:0] contiene 7 bits
#define ZA_OFFSET_H 0x7D // ZA_OFFS[14:7] contiene 8 bits
#define ZA_OFFSET_L 0x7E // ZA_OFFS[6:0] contiene 7 bits

//*****************************//
//GYROSCOPE REGISTERS
#define GYRO_CONFIG_REG 0x1B
#define GYRO_XOUT_H_REG 0x43
#define GYRO_XOUT_L_REG 0x44
#define GYRO_YOUT_H_REG 0x45
#define GYRO_YOUT_L_REG 0x46
#define GYRO_ZOUT_H_REG 0x47
#define GYRO_ZOUT_L_REG 0x48

#define XG_OFFSET_H 0x13 // X_OFFS_USR[15:8]
#define XG_OFFSET_L 0x14 // X_OFFS_USR[7:0]
#define YG_OFFSET_H 0x15 // Y_OFFS_USR[15:8]
#define YG_OFFSET_L 0x16 // Y_OFFS_USR[7:0]
#define ZG_OFFSET_H 0x17 // Y_OFFS_USR[15:8]
#define ZG_OFFSET_L 0x18 // Y_OFFS_USR[7:0]

//*****************************//

//*****************************//
//MAGNETOMETER REGISTERS
#define MAG_WIA_REG 0x00 //Device ID
#define MAG_INFO_REG 0x01 //Information
#define MAG_ST1_REG 0x02// STATUS1
#define MAG_XOUT_L_REG 0x03 //
#define MAG_XOUT_H_REG 0x04 //
#define MAG_YOUT_L_REG 0x05 //
#define MAG_YOUT_H_REG 0x06 //
#define MAG_ZOUT_L_REG 0x07 //
#define MAG_ZOUT_H_REG 0x08 //
#define MAG_ST2_REG 0x09 //STATUS 2
#define MAG_CONTL_REG 0x0A // Control[R/W]
#define MAG_ASTC_REG 0x0C //Self Test[R/W]
#define MAG_I2CDIS_REG 0x0F // I2C Disable [R/W]
#define MAG_ASAX_REG 0x10 // X-axis sensitivity adjustment value
#define MAG_ASAY_REG 0x11 // X-axis sensitivity adjustment value
#define MAG_ASAZ_REG 0x12 // X-axis sensitivity adjustment value


#define PWR_MGMT_1_REG 0x6B
#define WHO_AM_I_REG 0x75

#define INT_PIN_CFG_REG 0x37 // b1: Bypass_En  (Habilita la comunicación con el Sensor Magnetómetro AK8963)

#define AK8963_ADDR (0x0C<<1) //0xD1//0x18//

#define WHOAMI_MPU9250_LOW 0x71 //PIN AD0 a GND
#define WHOAMI_MPU9250_HIGH 0x73 // PIN AD0 a VCC
#define WHOAMI_MPU6050_LOW 0x68 //PIN AD0 a GND
#define WHOAMI_MPU6050_HIGH 0x69 // PIN AD0 a VCC

#define ACC_SENS_2g 16384.0 // LSB/g
#define ACC_SENS_4g 8192.0 // LSB/g
#define ACC_SENS_8g 4096.0 // LSB/g
#define ACC_SENS_16g 2048.0 // LSB/g

#define GYRO_SENS_250DPS 131.0 // LSB/(º/seg)
#define GYRO_SENS_500DPS 65.5 // LSB/(º/seg)
#define GYRO_SENS_1000DPS 32.8 // LSB/(º/seg)
#define GYRO_SENS_2000DPS 16.4 // LSB/(º/seg)


#define ACC_SCALE_2g 0x00
#define ACC_SCALE_4g 0x08
#define ACC_SCALE_8g 0x10
#define ACC_SCALE_16g 0x18

#define GYRO_SCALE_250DPS 0x00
#define GYRO_SCALE_500DPS 0x08
#define GYRO_SCALE_1000DPS 0x10
#define GYRO_SCALE_2000DPS 0x18



void MPUxx50_Init (void);
uint8_t MPUxx50_WhoIAm(void);

void MPUxx50_Acc_Read (void);
void MPUxx50_Gyro_Read (void);
void MPUxx50_Mag_Read(void);
void MPUxx50_ReadValues(void);

void MPUxx50_Calibration (void);

void MPUxx50_Offset(void);

void AK8963_Init(void);
uint8_t AK8963_WhoIAm(void);

void AK8963_Calibration(void);

void AK8963_Sens_Adjustment(void);
uint8_t AK8963_CheckOverflow(void);

//void MPU9250_Mag_Read2(void);

void CheckForID_I2C(void);
