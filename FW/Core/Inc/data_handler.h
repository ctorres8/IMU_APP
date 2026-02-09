/*
 * data_handler.h
 *
 *  Created on: Sep 26, 2025
 *      Author: Cristian
 */


#pragma pack(push,1)
typedef struct{
	uint8_t start;
	uint8_t type_frame;
	float Ax_f,Ay_f,Az_f;
	float Gx_f,Gy_f,Gz_f;
	float Mx_f,My_f,Mz_f;
	float Roll,Pitch,Yaw;
	float Ax_est,Ay_est,Az_est;
	float Gx_est,Gy_est,Gz_est;
	float Mx_est,My_est,Mz_est;
	float Roll_est,Pitch_est,Yaw_est;
	uint8_t checksum;
}IMUDataPacket_t;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct{
	uint8_t start;
	uint8_t type_frame;
	int velocidad;
	char version[18];
	char imu_model[20];
	char escala[30];
	uint8_t checksum;
}IMUInfoPacket_t;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct{
	uint8_t start;
	uint8_t type_frame;
	uint8_t percent;
	uint8_t checksum;
}IMUCALIB_t;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct{
	IMUDataPacket_t telemetric_packet;
	IMUInfoPacket_t info_packet;
	IMUCALIB_t calib_packet;
}IMU_Send_Packet_t;
#pragma pack(pop)


void EstimateValues (void);
void ShowMeasures(void);
void ShowEstimatedValues(void);
char* getDeviceID (void);
void LoadTelemetricData(void);
void LoadCalibData(uint8_t);
void LoadInfoData(void);
