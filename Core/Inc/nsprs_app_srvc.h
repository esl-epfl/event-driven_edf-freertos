#ifndef NSPRS_APP_SRVC_H_
#define NSPRS_APP_SRVC_H_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "nsprs_params.h"

SemaphoreHandle_t asSem1, asSem2, asSem3, asSem4;
TaskHandle_t asBasicMachine_task_handler;
void launch_asBasicMachine (void);
void delete_asBasicMachine (void);
void create_asSems (void);
void delete_asSems (void);
void asBasicMachine_t(void *pvParameters);

//typedef enum {
//	asStateInit,
//	asStateHeatup,
//	asStateReady,
//	asStateBrewing,
//	asStatePoweroff,
//	asStateTest
//} asStates;
//asStates asState;

//state functions
static asStates asInit(void);
static asStates asHeatup(void);
static asStates asReady(void);
static asStates asBrewing(void);
static asStates asPoweroff(void);
static asStates asTest(void);

//period
extern int SM_periodMs;	//period of this service

#endif //NSPRS_APP_SRVC_H_
