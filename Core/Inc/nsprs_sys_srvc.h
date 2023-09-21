#ifndef NSPRS_SYS_SRVC_H_
#define NSPRS_SYS_SRVC_H_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "nsprs_comm.h"

//handlers
TaskHandle_t ssHeatPump_task_handler;
TaskHandle_t ssTrgPump_task_handler;
TaskHandle_t ssTrgHeater_task_handler;
TaskHandle_t ssTimer_task_handler;
TaskHandle_t ssTimer2_task_handler;
TaskHandle_t ssTimer3_task_handler;
TaskHandle_t ssTimer4_task_handler;
TaskHandle_t ssTimer5_task_handler;
TaskHandle_t ssHMI_task_handler;
TaskHandle_t ssFlow_task_handler;
TaskHandle_t ssTemp_task_handler;
TaskHandle_t ssHeatAlg_task_handler;

TaskHandle_t ssHMI_b_nsprs_task_handler;
TaskHandle_t ssHMI_b_esprs_task_handler;
TaskHandle_t ssHMI_b_lungo_task_handler;
TaskHandle_t ssHMI_b_hw_task_handler;

// TaskHandle_t dummy0_task_handler;
// TaskHandle_t dummy1_task_handler;
// TaskHandle_t dummy2_task_handler;
// TaskHandle_t dummy3_task_handler;
// TaskHandle_t dummy4_task_handler;
// TaskHandle_t dummy5_task_handler;
// TaskHandle_t dummy6_task_handler;
// TaskHandle_t dummy7_task_handler;
// TaskHandle_t dummy8_task_handler;
// TaskHandle_t dummy9_task_handler;
TaskHandle_t dummy_task_handler[10];


TaskHandle_t ssTask1_task_handler;
TaskHandle_t ssTask2_task_handler;

TaskHandle_t ssTest2_1_task_handler;
TaskHandle_t ssTest2_2_task_handler;
TaskHandle_t ssTest2_highp_task_handler;



//create
void launch_ssHeatPump (void);
void launch_ssTrgPump (void);
void launch_ssTrgHeater (void);
void launch_ssTimer (void);
void launch_ssTimer2 (void);
void launch_ssTimer3 (void);
void launch_ssTimer4 (void);
void launch_ssTimer5 (void);
void launch_ssHMI (void);
void launch_ssFlow (void);
void launch_ssTemp (void);
void launch_ssHeatAlg (void);
void launch_ssAll(void);

void launch_ssHMI_b_nsprs (void);
void launch_ssHMI_b_esprs (void);
void launch_ssHMI_b_lungo (void);
void launch_ssHMI_b_hw (void);

void launch_ssTask1 (void);
void launch_ssTask2 (void);

void launch_ssTest2_1 (void);
void launch_ssTest2_2 (void);
void launch_ssTest2_highp (void);

void launch_dummy_tasks(void);

//delete
void delete_ssHeatPump (void);
void delete_ssTrgPump (void);
void delete_ssTrgHeater (void);
void delete_ssTimer (void);
void delete_ssTimer2 (void);
void delete_ssTimer3 (void);
void delete_ssTimer4 (void);
void delete_ssTimer5 (void);
void delete_ssHMI (void);
void delete_ssFlow (void);
void delete_ssTemp (void);
void delete_ssHeatAlg (void);
void delete_ssAll(void);

//tasks
void ssHeatPump_t( void *pvParameters);
void ssTrgPump_t( void *pvParameters);
void ssTrgHeater_t( void *pvParameters);
void ssTimer_t( void *pvParameters); //change to multi timer
void ssTimer2_t( void *pvParameters); //todo: remove it and change to one ss handles all the available timers
void ssTimer3_t( void *pvParameters);
void ssTimer4_t( void *pvParameters);
void ssTimer5_t( void *pvParameters);
void ssHMI_t( void *pvParameters); //
void ssFlow_t(void *pvParameters);
void ssTemp_t(void *pvParameters);
void ssHeatAlg_t(void *pvParameters);

void ssHMI_b_nsprs_t( void *pvParameters);
void ssHMI_b_esprs_t( void *pvParameters);
void ssHMI_b_lungo_t( void *pvParameters);
void ssHMI_b_hw_t( void *pvParameters);

void ssTask1_t(void *pvParameters);
void ssTask2_t(void *pvParameters);
void ssTest2_1_t(void *pvParameters);
void ssTest2_2_t(void *pvParameters);
void ssTest2_highp_t(void *pvParameters);

void task_general(void *pvParameters);

void dummy0(void *pvParameters);
void dummy1(void *pvParameters);
void dummy2(void *pvParameters);
void dummy3(void *pvParameters);
void dummy4(void *pvParameters);
void dummy5(void *pvParameters);
void dummy6(void *pvParameters);
void dummy7(void *pvParameters);
void dummy8(void *pvParameters);
void dummy9(void *pvParameters);



//=== internal functions ===
static int ssTemp_adc2temp(unsigned int adc_val);

static void put_hmi_infor(ssHMI_d * p_data, uint32_t buttons_situation, unsigned int time_corto, unsigned int time_lungo, unsigned int time_hw, unsigned int time_nsprs);

//==== states ====
typedef enum {
	sleep,
	active
}ssHeatAlgTaskStates;
extern ssHeatAlgTaskStates heatalg_task_state;

typedef enum{
	fast_heating,
	slow_heating,
	brew_heating,
	no_heating
}ssHeatAlg_AlgStates;
extern ssHeatAlg_AlgStates heatalg_alg_state;
extern ssHeatAlg_AlgStates heatalg_alg_next_state;

typedef enum{
	init_en_heater__fastheatupstate,
	wait__fastheatupstate
}ssHeatAlg_Alg_FastHeatingStates;
extern ssHeatAlg_Alg_FastHeatingStates heatalg_alg_fastheatup_state;

typedef enum{
	init_dis_heater__noheatupstate,
	wait__noheatupstate
}ssHeatAlg_Alg_NoHeatingStates;
extern ssHeatAlg_Alg_NoHeatingStates heatalg_alg_noheatup_state;

typedef enum{
	init__slowheatupstate,
	on_state__slowheatupstate,
	off_state__slowheatupstate
}ssHeatAlg_Alg_SlowHeatingStates;
extern ssHeatAlg_Alg_SlowHeatingStates heatalg_alg_slowheatup_state;

typedef enum{
	init___slowheatuponstate,
	wait___slowheatuponstate
}ssHeatAlg_Alg_SlowHeating_OnStates;
extern ssHeatAlg_Alg_SlowHeating_OnStates heatalg_alg_slowheatup_on_state;
typedef enum{
	init___slowheatupoffstate,
	wait___slowheatupoffstate
}ssHeatAlg_Alg_SlowHeating_OffStates;
extern ssHeatAlg_Alg_SlowHeating_OffStates heatalg_alg_slowheatup_off_state;


extern int ssHeatAlg_Alg_substate_firstrun;

//==== periods ====
extern int ssHeatAlg_periodMs;	//period of this service
extern int ssTemp_periodMs;


// ============================ a general operation =====================================================
typedef  void (*Operation)(void );
/*
 * ============================ Flow =====================================================
 */
void flow_disable(void);
void flow_enable(void);
void ssFlow_resetValues(void);
int ssFlow_time2flow(uint32_t time_us);

typedef struct DATA_Flow_ {
   unsigned int    	fr;
   unsigned int    	vol;
   unsigned int		en;
} DATA_Flow;
extern DATA_Flow data_flow;

/*
 * ============================ Timer =====================================================
 */
typedef struct DATA_Timer_{
	unsigned int 	time;
	unsigned int	en;
	unsigned int 	tick_freq;
	Operation 	reset;			//todo: this is for showing how to use function pointers, in future I put all the functions like this
} DATA_Timer;
extern DATA_Timer data_timer;

/*
 * ============================ HMI =====================================================
 */
void hmi_reset(void);
typedef struct DATA_HMI_{
	char en;
	unsigned int 	time_esprs_ms;
	unsigned int 	time_nsprs_ms;
	unsigned int 	time_lungo_ms;
	unsigned int 	time_hw_ms;
	char req_brew_esprs;
	char req_brew_lungo;
	char req_hw;
	char data_from_isr;
} DATA_HMI;
extern DATA_HMI data_hmi;

/*
 * ============================ TrgPump =====================================================
 */
void pump_disable(void);
void pump_enable(void);
typedef struct DATA_TRGPUMP_{
	char en;
	char zc_edge;
} DATA_TRGPUMP;
extern DATA_TRGPUMP data_trgpump;
/*
 * ============================ TrgHeater =====================================================
 */
void heater_disable(void);
void heater_enable(void);
void heating_alg(void);
static void ssHeatAlg_checkTargetTemp(void);
static void ssHeatAlg_checkAlgState(void);
static void ssHeatAlg_putAlgState(void);
static void ssHeatAlg_Alg(void);
static void ssHeatAlg_Alg_FastHeating(void);
static void ssHeatAlg_Alg_SlowHeating(void);
static void ssHeatAlg_Alg_BrewHeating(void);
static void ssHeatAlg_Alg_NoHeating(void);
static void ssHeatAlg_enHeater(void);
static void ssHeatAlg_disHeater(void);
typedef struct DATA_TRGHEATER_{
	char en;
	char zc_edge;
	char heating_alg_result;
} DATA_TRGHEATER;
extern DATA_TRGHEATER data_trgheater;

/*
 * ============================ Temp =====================================================
 */
typedef struct DATA_Temp_{
	unsigned int 	temp;
	unsigned int	en;
} DATA_Temp;
extern DATA_Temp data_temp;





#endif //NSPRS_SYS_SRVC_H_
