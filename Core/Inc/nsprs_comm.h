#ifndef NSPRS_COMM_H_
#define NSPRS_COMM_H_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "nsprs_params.h"

//============= functions: launch, delete ===============
void launch_comm(void);
void delete_comm(void);

//============= sys srvc data_struct, queus, and sems ==================
//ssTimer
typedef struct{
	SemaphoreHandle_t callingTaskSem; //calling routine wait on this to be given by driver
	int cmd;
	uint32_t timer_id;			//in case there are more than one timer
//	uint32_t time;				//in sec
	unsigned int time;			//in us
} ssTimer_d;
extern QueueHandle_t ssTimer_qh;
extern QueueHandle_t ssTimer2_qh;
extern QueueHandle_t ssTimer3_qh;
extern QueueHandle_t ssTimer4_qh;
extern QueueHandle_t ssTimer5_qh;
extern SemaphoreHandle_t intr2ssTimer_sh;
extern SemaphoreHandle_t intr2ssTimer2_sh;
extern SemaphoreHandle_t intr2ssTimer3_sh;
extern SemaphoreHandle_t intr2ssTimer4_sh;
extern SemaphoreHandle_t intr2ssTimer5_sh;

//ssHeatPump
typedef struct{
	SemaphoreHandle_t callingTaskSem; //calling routine wait on this to be given by driver
	int cmd;			//enable/disable pmp/heater and send signal based on recieving ZC sem (HIGH PRIORITY)
} ssHeatPump_d;
extern QueueHandle_t ssHeatPump_qh;
extern QueueHandle_t intr2ssHeatPump_qh;

//ssTrgPump
typedef struct{
	SemaphoreHandle_t callingTaskSem;
	int cmd;
} ssTrgPump_d;
extern QueueHandle_t ssTrgPump_qh;
extern QueueHandle_t intr2ssTrgPump_qh;

//ssHeater
typedef struct{
	SemaphoreHandle_t callingTaskSem;
	int cmd;
} ssTrgHeater_d;
extern QueueHandle_t ssTrgHeater_qh;
extern QueueHandle_t intr2ssTrgHeater_qh;

//ssHMI
typedef struct{
	SemaphoreHandle_t callingTaskSem;
	int cmd; // 0: make sleep, make active, 2 read
	uint32_t buttons_situation; //put back button situation in this (b[2:0] for button corto
	unsigned int time_corto_ms;
	unsigned int time_lungo_ms;
	char req_brew_espr;
	char req_brew_lungo;
	char req_hw;
} ssHMI_d;
extern QueueHandle_t ssHMI_qh;
extern QueueHandle_t intr2ssHMI_qh;

//ssFlow
typedef struct{
	SemaphoreHandle_t callingTaskSem;
	int cmd;	//continous (provide it every time), one-time read NO!, sleep
	int flow_rate;
	int total_vol;
} ssFlow_d;
extern QueueHandle_t ssFlow_qh;
extern QueueHandle_t intr2ssFlow_qh;

//ssTemp
typedef struct{
	SemaphoreHandle_t callingTaskSem;
	int cmd;	//continous (provide it every time), one-time read, sleep
	int updatedTemp; //flow will be put in this for giving to as (instead of using global variable
} ssTemp_d;
extern QueueHandle_t ssTemp_qh;

//ssHeatAlg
typedef struct{
	SemaphoreHandle_t callingTaskSem;
	int cmd;
} ssHeatAlg_d;
extern QueueHandle_t ssHeatAlg_qh;

//ssUart
extern QueueHandle_t intr2ssUart_qh;

extern QueueHandle_t intr2ssDS_qh;

#endif //NSPRS_COMM_H_
