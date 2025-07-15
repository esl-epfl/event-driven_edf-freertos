#include "devices_comm.h"
#include "devices_params.h"
#include "devices_common.h"

void launch_comm(void){
	//== mutexes
//	xLogMutex = xSemaphoreCreateMutex();
//	configASSERT(xLogMutex);
//	//== queues
	ssHeatPump_qh = 		xQueueCreate(1, sizeof(struct ssHeatPump_d *));
	ssTrgPump_qh = 			xQueueCreate(1, sizeof(struct ssTrgPump_d *));
	ssTrgHeater_qh = 		xQueueCreate(1, sizeof(struct ssTrgHeater_d *));
	ssTimer_qh = 			xQueueCreate(1, sizeof(struct ssTimer_d *));
	ssTimer2_qh = 			xQueueCreate(1, sizeof(struct ssTimer_d *));
	ssTimer3_qh = 			xQueueCreate(1, sizeof(struct ssTimer_d *));
	ssTimer4_qh = 			xQueueCreate(1, sizeof(struct ssTimer_d *));
	ssTimer5_qh = 			xQueueCreate(1, sizeof(struct ssTimer_d *));
	ssHMI_qh = 				xQueueCreate(1, sizeof(struct ssHMI_d *));
	ssFlow_qh = 			xQueueCreate(1, sizeof(struct ssFlow_d *));
	ssTemp_qh =				xQueueCreate(1, sizeof(struct ssTemp_d *));
	ssHeatAlg_qh = 			xQueueCreate(1, sizeof(struct ssHeatAlg_d *));
	intr2ssHeatPump_qh = 	xQueueCreate(1, sizeof(char));
	intr2ssTrgPump_qh = 	xQueueCreate(1, sizeof(char));
	intr2ssTrgHeater_qh = 	xQueueCreate(1, sizeof(char));
	intr2ssTimer_sh  = 		xSemaphoreCreateBinary();
	intr2ssTimer2_sh  = 	xSemaphoreCreateBinary();
	intr2ssTimer3_sh  = 	xSemaphoreCreateBinary();
	intr2ssTimer4_sh  = 	xSemaphoreCreateBinary();
	intr2ssTimer5_sh  = 	xSemaphoreCreateBinary();
	intr2ssHMI_qh = 		xQueueCreate(1, sizeof(char));
	intr2ssFlow_qh = 		xQueueCreate(1, sizeof(char));
	intr2ssUart_qh = 		xQueueCreate(1, sizeof(char));

	//== test queues
//	intr2ssDS_qh = 		xQueueCreate(2, sizeof(char));
}

void delete_comm(void){
//	vSemaphoreDelete(xLogMutex);
	vQueueDelete(ssHeatPump_qh);
	vQueueDelete(ssTrgPump_qh);
	vQueueDelete(ssTrgHeater_qh);
	vQueueDelete(ssTimer_qh);
	vQueueDelete(ssTimer2_qh);
	vQueueDelete(ssTimer3_qh);
	vQueueDelete(ssTimer4_qh);
	vQueueDelete(ssTimer5_qh);
	vQueueDelete(ssHMI_qh);
	vQueueDelete(ssFlow_qh);
	vQueueDelete(ssTemp_qh);
	vQueueDelete(ssHeatAlg_qh);
	vQueueDelete(intr2ssHeatPump_qh);
	vQueueDelete(intr2ssTrgPump_qh);
	vQueueDelete(intr2ssTrgHeater_qh);
	vSemaphoreDelete(intr2ssTimer_sh);
	vSemaphoreDelete(intr2ssTimer2_sh);
	vSemaphoreDelete(intr2ssTimer3_sh);
	vSemaphoreDelete(intr2ssTimer4_sh);
	vSemaphoreDelete(intr2ssTimer5_sh);
	vQueueDelete(intr2ssHMI_qh);
	vQueueDelete(intr2ssFlow_qh);
	vQueueDelete(intr2ssUart_qh);

}



//Queues
QueueHandle_t ssHeatPump_qh = NULL;
QueueHandle_t ssTrgPump_qh = NULL;
QueueHandle_t ssTrgHeater_qh = NULL;
QueueHandle_t ssTimer_qh = NULL;
QueueHandle_t ssTimer2_qh = NULL;
QueueHandle_t ssTimer3_qh = NULL;
QueueHandle_t ssTimer4_qh = NULL;
QueueHandle_t ssTimer5_qh = NULL;
QueueHandle_t ssHMI_qh = NULL;
QueueHandle_t ssFlow_qh = NULL;
QueueHandle_t ssTemp_qh = NULL;
QueueHandle_t ssHeatAlg_qh = NULL;
QueueHandle_t intr2ssHeatPump_qh = NULL;
QueueHandle_t intr2ssTrgPump_qh = NULL;
QueueHandle_t intr2ssTrgHeater_qh = NULL;
QueueHandle_t intr2ssHMI_qh = NULL;
QueueHandle_t intr2ssFlow_qh = NULL;
QueueHandle_t intr2ssUart_qh = NULL;

//QueueHandle_t intr2ssDS_qh = NULL;

//== test queues





//Sems
SemaphoreHandle_t intr2ssTimer_sh = NULL;
SemaphoreHandle_t intr2ssTimer2_sh = NULL;
SemaphoreHandle_t intr2ssTimer3_sh = NULL;
SemaphoreHandle_t intr2ssTimer4_sh = NULL;
SemaphoreHandle_t intr2ssTimer5_sh = NULL;
//SemaphoreHandle_t xLogMutex = NULL;


