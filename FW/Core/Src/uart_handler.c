/*
 * uart_handler.c
 *
 *  Created on: Oct 19, 2025
 *      Author: Cristian
 */

#include "uart_handler.h"
#include "main.h"
#include "operation_modes.h"

extern UART_HandleTypeDef huart2;

extern IMU_Send_Packet_t imu_tx_packets;
extern Flags_Operation_t flagsSM;

IMU_Data_Rx_t imu_rx;



void UART_State_Init(void)
{
	/*
	 * @brief: Inicializa los valores por default del uart.
	 */

	imu_rx.flag_newframe = FALSE;
	memset(imu_rx.buffer_rx,0,MAX_LEN_BUFFER_RX);
}


uint8_t SendData(uint8_t frametx_type)
{
	/*
	 * @brief	Llama al HAL para enviar datos vía UART diferenciando entre tramas.
	 *
	 * @param
	 	 	 	* frametx_type: tipo de trama que se va a enviar.
	 	 	 	* SEND_MEASURES: Envía una struct de Mediciones telemétricas
	 	 	 	* SEND_INFO: Envía una struct de status
	 	 	 	* SEND_CALIB: Envía una struct de calibración
	 */
	uint8_t msg;

	if(frametx_type==SEND_MEASURES)
	{
		msg=HAL_UART_Transmit_IT(&huart2, (uint8_t *) &(imu_tx_packets.telemetric_packet), sizeof(imu_tx_packets.telemetric_packet));
	}
	if(frametx_type==SEND_INFO)
	{
		msg=HAL_UART_Transmit_IT(&huart2, (uint8_t *) &(imu_tx_packets.info_packet), sizeof(imu_tx_packets.info_packet));
	}
	if(frametx_type==SEND_CALIB)
	{
		msg=HAL_UART_Transmit_IT(&huart2, (uint8_t *) &(imu_tx_packets.calib_packet), sizeof(imu_tx_packets.calib_packet));
	}
	return msg;


}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	/*
	 * @brief: 	Callback de transmisión.
	 	 	 	Ingresa a esta función cuando se termina de enviar una trama completa.
	 */

	if(huart->Instance==huart2.Instance)
	{
		printf("\n::SEND:: Paquetes enviados perfectamente. \n");
		flagsSM.dataSentOK=TRUE;

		//printf("Ax:%.2f;Ay:%.2f;Az:%.2f;Gx:%.2f;Gy:%.2f;Gz:%.2f",Ax_f,Ay_f,Az_f,Gx_f,Gy_f,Gz_f);
	}
}

void ReceiveData(void)
{
	/*
	 * @brief: 	Recepción de datos.
	 * 			Llama al HAL encargado de la recepción de tramas.
	 */

	HAL_UART_Receive_IT(&huart2, (uint8_t *) &(imu_rx.data_byte), sizeof(imu_rx.data_byte));

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	/*
	 * @brief: 	Callback de Recepción.
	 * 			Cuando se termina de recibir un dato ingresa acá.
	 */

	if(huart->Instance==huart2.Instance)
	{
		FrameRecover();
		ReceiveData();
	}


}

void FrameRecover(void)
{
	/*
	 * @brief: 	Maquina de Estados que reconstruye la trama byte a byte.
	  			La Maq de Estados cuenta con 2 Bloques:
	 	 	 	 * ESPERANDO: Espera el byte de comienzo '#', cuando llega pasa al siguiente estado
	 	 	 	 * RECIBIENDO: Empieza a recibir byte a byte la trama y la va guardando un string.
	  			Cuando llega el byte de fin '&' termina de recibir y levanta el flag de nueva trama
	 */

	static uint8_t index_rx=0;
	static uint8_t status = ESPERANDO;
	IMU_Data_Rx_t *ptr_imu_rx = &imu_rx;

	switch(status)
	{
		case ESPERANDO :
					if(ptr_imu_rx->data_byte!='#')
					{
						ptr_imu_rx->flag_newframe=FALSE;
						status = ESPERANDO;
					}
					else
					{
						index_rx=0;
						status = RECIBIENDO;
					}
					break;
		case RECIBIENDO :
					if(ptr_imu_rx->data_byte=='#')
					{
						index_rx=0;
						status=RECIBIENDO;
						break;
					}
					if(ptr_imu_rx->data_byte=='&')
					{
						ptr_imu_rx->flag_newframe=TRUE;
						ptr_imu_rx->buffer_rx[index_rx]='\0';
						status = ESPERANDO;
						break;
					}
					if(index_rx<MAX_LEN_BUFFER_RX-1)
					{
						ptr_imu_rx->buffer_rx[index_rx++]=ptr_imu_rx->data_byte;
						status= RECIBIENDO;
					}
					else
					{
						index_rx=0;
						memset(ptr_imu_rx->buffer_rx,0,MAX_LEN_BUFFER_RX);
						status = ESPERANDO;
					}
					break;
		default :
					index_rx=0;
					status=ESPERANDO;
					UART_MDE_Reset_Values();
					break;

	}
}



void UART_MDE_Reset_Values(void)
{
	/*
	 * @brief: Reinicia los valores de la UART.
	 */
	imu_rx.data_byte=0;
	imu_rx.flag_newframe = FALSE;
	memset(imu_rx.buffer_rx,0,MAX_LEN_BUFFER_RX);

}

