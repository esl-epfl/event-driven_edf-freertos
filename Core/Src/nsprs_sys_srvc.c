#include "nsprs_params.h"
#include "nsprs_drivers.h"
#include "nsprs_sys_srvc.h"
#include "nsprs_comm.h"
#include "nsprs_params.h"
#include "nsprs_common.h"
#include "stm32_trg_driver.h"

//================================================================================================================================
//================================================================================================================================
//================================================================================================================================
//================================================================================================================================
//================================================================================================================================
//================================================================================================================================

//=== states
ssHeatAlgTaskStates heatalg_task_state = sleep;
ssHeatAlg_AlgStates heatalg_alg_state = slow_heating;
ssHeatAlg_AlgStates heatalg_alg_next_state = slow_heating;
ssHeatAlg_Alg_FastHeatingStates heatalg_alg_fastheatup_state = init_en_heater__fastheatupstate;
ssHeatAlg_Alg_NoHeatingStates heatalg_alg_noheatup_state = init_dis_heater__noheatupstate;
ssHeatAlg_Alg_SlowHeatingStates heatalg_alg_slowheatup_state = init__slowheatupstate;
ssHeatAlg_Alg_SlowHeating_OnStates heatalg_alg_slowheatup_on_state = init___slowheatuponstate;
ssHeatAlg_Alg_SlowHeating_OffStates heatalg_alg_slowheatup_off_state = init___slowheatupoffstate;

extern int ssHeatAlg_Alg_substate_firstrun = 1;

//==== periods ====
int ssHeatAlg_periodMs = 50;	//period of this service
int ssTemp_periodMs = 100;

//================= for the test
//=================================
#define number_of_tasks 10
#define number_of_issues 100
#define test_duration_ms 22653
int ids[number_of_tasks] = {0,1,2,3,4,5,6,7,8,9};
int exe_time_ms[number_of_tasks] = {443,209,302,253,1069,228,221,726,310,951};
int slack_ms[number_of_tasks] = {1258,3695,2420,946,3872,4772,723,3684,3652,2693};
int rel_deadline_ms[number_of_tasks] = {1701,3904,2722,1199,4941,5000,944,4410,3962,3644};
int priority[number_of_tasks] = {8,5,7,9,2,1,10,3,4,6};
int issue_ms[number_of_issues] = {91,474,574,1283,1314,1426,1450,1456,1483,1855,1858,2000,2112,2139,2158,2405,2488,2509,2516,2534,2811,2875,3051,3103,3349,3437,3497,3562,3662,3708,3798,3801,3818,3832,3887,3990,4059,4105,4118,4290,4779,4786,4849,4983,5027,5039,5041,5521,5613,5756,5784,5817,5972,6003,6090,6162,6220,6333,6478,6529,6549,6609,6663,6693,6779,6882,7142,7162,7217,7264,7352,7501,7528,7596,7696,7820,7942,7954,8207,8426,8499,8555,8803,8903,8937,9024,9128,9141,9194,9219,9243,9282,9316,9444,9446,9492,9653,9725,9810,9931};
int issue_id[number_of_issues] = {2,7,8,3,0,1,5,4,8,4,4,9,6,1,6,4,4,3,8,7,5,2,9,6,9,2,2,9,6,1,0,1,3,3,3,3,2,9,0,0,6,0,2,9,2,7,7,3,6,3,0,8,2,9,6,1,1,3,7,2,6,0,6,6,7,7,8,6,6,6,3,6,4,6,7,5,6,0,0,4,5,3,4,6,3,0,3,6,3,4,6,0,2,4,0,4,9,9,3,2};


typedef struct TEST_PARAMETERS_{
	int missed[number_of_tasks];
	int finished[number_of_tasks];
	
}TEST_PARAMETERS;

TEST_PARAMETERS test_parameters;

extern unsigned int time_start; //ht: timing
extern unsigned int time_end;
extern unsigned int time_overal;
extern int time_number;
extern unsigned int time_overal_add;
extern int time_number_add;









//============= launch tasks ====================
void launch_ssTimer (void){
	xTaskCreate	(ssTimer_t, (const char *) "sys srvc, timer",configMINIMAL_STACK_SIZE, (void *) NULL, configMAX_PRIORITIES, &ssTimer_task_handler, 2);
}
void launch_ssFlow (void){
	xTaskCreate	(ssFlow_t, (const char *) "sys srvc, Flow", configMINIMAL_STACK_SIZE, (void *) NULL, HIGH_PRIORITY, &ssFlow_task_handler, 100);
}
void launch_ssHMI_b_nsprs (void){
	xTaskCreate	(ssHMI_b_nsprs_t, (const char *) "sys srvc, HMI- nsprs", configMINIMAL_STACK_SIZE, (void *) NULL, HIGH_PRIORITY,  &ssHMI_b_nsprs_task_handler, 100);
}
void launch_ssHMI_b_esprs (void){
	xTaskCreate	(ssHMI_b_esprs_t, (const char *) "sys srvc, HMI- esprs", configMINIMAL_STACK_SIZE, (void *) NULL, HIGH_PRIORITY,  &ssHMI_b_esprs_task_handler, 100);
}
void launch_ssHMI_b_lungo (void){
	xTaskCreate	(ssHMI_b_lungo_t, (const char *) "sys srvc, HMI- lungo", configMINIMAL_STACK_SIZE, (void *) NULL, HIGH_PRIORITY,  &ssHMI_b_lungo_task_handler, 100);
}
void launch_ssHMI_b_hw (void){
	xTaskCreate	(ssHMI_b_hw_t, (const char *) "sys srvc, HMI- hw", configMINIMAL_STACK_SIZE, (void *) NULL, HIGH_PRIORITY,  &ssHMI_b_hw_task_handler, 100);
}
void launch_ssTrgPump (void){
	xTaskCreate	(ssTrgPump_t, (const char *) "sys srvc, trg pump enabling",configMINIMAL_STACK_SIZE, (void *) NULL, HIGH_PRIORITY, &ssTrgPump_task_handler, 9);
}
void launch_ssTrgHeater (void){
	xTaskCreate	(ssTrgHeater_t, (const char *) "sys srvc, trg heater enabling",configMINIMAL_STACK_SIZE, (void *) NULL, HIGH_PRIORITY, &ssTrgHeater_task_handler, 8);
}
void launch_ssTemp (void){
	xTaskCreate	(ssTemp_t, (const char *) "sys srvc, Temperature", configMINIMAL_STACK_SIZE, (void *) NULL, HIGH_PRIORITY, &ssTemp_task_handler, 100);
}

void launch_dummy_tasks(void){

	for (int i=0; i< number_of_tasks; i++){
		xTaskCreate	(task_general, (const char *) "",configMINIMAL_STACK_SIZE, (void*)&ids[i], priority[i], &dummy_task_handler[i], 10*rel_deadline_ms[i]);
	}

}


void launch_ssAll(void){
	// launch_ssFlow();
	launch_ssTimer();
	// launch_ssHMI_b_nsprs();
	// launch_ssHMI_b_esprs();
	// launch_ssHMI_b_lungo();
	// launch_ssHMI_b_hw();
	// launch_ssTrgPump();
	// launch_ssTrgHeater();
	// launch_ssTemp();

	 launch_dummy_tasks();

}

//============= delete tasks =================
void delete_ssTrgPump (void){
	vTaskDelete( ssTrgPump_task_handler );
}
void delete_ssTrgHeater (void){
	vTaskDelete( ssTrgHeater_task_handler );
}

void delete_ssHMI (void){
	vTaskDelete( ssHMI_task_handler );
}
void delete_ssFlow (void){
	vTaskDelete( ssFlow_task_handler );
}
void delete_ssTemp (void){
	vTaskDelete( ssTemp_task_handler );
}
void delete_ssHeatAlg (void){
	vTaskDelete( ssHeatAlg_task_handler );
}
void delete_ssAll(void){
	delete_ssTrgPump ();
	delete_ssTrgHeater ();
	delete_ssHMI ();
	delete_ssFlow ();
	delete_ssTemp ();
	delete_ssHeatAlg ();
}

/*
 * ================================================================
 * ================================================================
 * ================= Flow Task ====================================
 * ================================================================
 * ================================================================
 */

DATA_Flow data_flow= {
	.fr = 0,
	.vol = 0,
	.en = 1
};


void ssFlow_t(void *pvParameters){

}



/*
 * ================================================================
 * ================================================================
 * ================= Timer Task ===================================
 * ================================================================
 * ================================================================
 */
void reset_timer (void){
	data_timer.time = 0;
}

DATA_Timer data_timer= {
	.en = 1,
	.time = 0,
	.tick_freq = 1000,
	.reset = reset_timer
};


void ssTimer_t( void *pvParameters ){

	int timer_id = 1;
	unsigned int cnt_val = (1000*timer_readFreqKHz(timer_id))/(data_timer.tick_freq); //why should I multiply by
	timer_count(timer_id, cnt_val);

	timer_id = 2;
	cnt_val = timer_readMax(timer_id);
	timer_count(timer_id, cnt_val);



	printf("\r\n");
	for (int i=0; i<number_of_tasks; i++){
		test_parameters.finished[i]=1;
		test_parameters.missed[i]=0;
	}
	// int counter = 0;
	// int period_after_last = 10000;
	int id_time = 0;
	int id_task;
	
	vTaskSuspend( ssTimer_task_handler );

	

	while(1){
		data_timer.time = data_timer.time + 1;


		if (data_timer.time == test_duration_ms){
			printf("======= test duration: %d, dynmaic scheduler: %d, number of issues: %d, number of tasks: %d\r\n", 
				test_duration_ms, configDYNAMIC_SCHEDULING, number_of_issues, number_of_tasks);
				int totall=0;
				for (int i = 0; i<number_of_tasks; i++){
					// printf("task %d, number of misses: %d\r\n", i, test_parameters.missed[i]);
					totall += test_parameters.missed[i];
				}
				printf ("totall misses: %d\r\n", totall);
				printf("time totall cs:%lu, number:%d, avg:%lu\r\n", time_overal, time_number, time_overal/time_number);
				printf("time totall insert:	%lu, number:%d, avg:%lu\r\n", time_overal_add, time_number_add, time_overal_add/time_number_add);
				while(1);

		}
		
		vTaskSuspend( ssTimer_task_handler );
	}
}

/*
 * ================================================================
 * ================================================================
 * ================= HMI Tasks ===================================
 * ================================================================
 * ================================================================
 */
DATA_HMI data_hmi= {
	.en = 1,
	.time_esprs_ms = 0,
	.time_lungo_ms = 0,
	.time_hw_ms = 0,
	.time_nsprs_ms = 0,
	.req_brew_esprs = 0,
	.req_brew_lungo =0,
	.req_hw = 0,
	.data_from_isr = 0 //should be from isr (maybe we can define it better, however, not important at the moment)

};

void ssHMI_b_nsprs_t( void *pvParameters ){

}

void ssHMI_b_lungo_t( void *pvParameters ){

}

void ssHMI_b_esprs_t( void *pvParameters ){

}

void ssHMI_b_hw_t( void *pvParameters ){


}

/*
 * ================================================================
 * ================================================================
 * ================= TrgPump Tasks ================================
 * ================================================================
 * ================================================================
 */
DATA_TRGPUMP data_trgpump= {
	.en = 1,
	.zc_edge = 0
};


void ssTrgPump_t( void *pvParameters ){

}

/*
 * ================================================================
 * ================================================================
 * ================= TrgHeater Tasks ================================
 * ================================================================
 * ================================================================
 */
DATA_TRGHEATER data_trgheater= {
	.en = 1,
	.zc_edge = 0,
	.heating_alg_result = 0
};


void ssTrgHeater_t( void *pvParameters ){

	

}



static void ssHeatAlg_checkAlgState(void){

}

static void ssHeatAlg_checkTargetTemp(void){

}

static void ssHeatAlg_putAlgState(void){

}

static void ssHeatAlg_Alg(void){

}

static void ssHeatAlg_Alg_FastHeating(void){

};


static void ssHeatAlg_Alg_SlowHeating(void){

}

static void ssHeatAlg_Alg_BrewHeating(void){
}

static void ssHeatAlg_Alg_NoHeating(void){

}

static void ssHeatAlg_enHeater(void){

}

static void ssHeatAlg_disHeater(void){

}


//=======================
DATA_Temp data_temp= {
	.en = 1,
	.temp = 0
};


void ssTemp_t(void *pvParameters){

}

/*
 * ================================================================
 * ================================================================
 * ================= General Tasks =============================
 * ================================================================
 * ================================================================
 */
void task_general(void *pvParameters){

	// TASK_PARAMETERS * parameters = (TASK_PARAMETERS *) pvParameters;
	int * p_id =  (int *) pvParameters;
	int id = *p_id;

	int outer = exe_time_ms[id];
	unsigned int finish_time;
	unsigned int start, end; //r
	vTaskSuspend(dummy_task_handler[id]); 

	while(1){
		
		for (int j=0; j<outer; j++){
			for (int i =0; i<1070; i++){

			}
		}	
		finish_time = xTaskGetTickCount(); //r

		if(finish_time>uxTaskDeadlineAbsGet(dummy_task_handler[id])){
			test_parameters.missed[id]++;

		}
		else{
		}
		test_parameters.finished[id] = 1;
		vTaskSuspend(dummy_task_handler[id]);
	}
	
}
