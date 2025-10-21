/*
 * data_handler.h
 *
 *  Created on: Sep 26, 2025
 *      Author: Cristian
 */

#pragma pack(push,1)
typedef struct {
	uint8_t start;
	float Ax_f,Ay_f,Az_f;
	float Gx_f,Gy_f,Gz_f;
	float Mx_f,My_f,Mz_f;
	float Ax_est,Ay_est,Az_est;
	float Gx_est,Gy_est,Gz_est;
	float Mx_est,My_est,Mz_est;
	uint8_t checksum;
}IMUData_t;
#pragma pack(pop)


void FilterValues(void);
void EstimateValues (void);
void ShowRAWValues(void);
void ShowFilteredValues(void);
void ShowEstimatedValues(void);
void LoadDataPacket(void);
uint8_t sendPacket(void);
uint8_t receivePacket(void);
