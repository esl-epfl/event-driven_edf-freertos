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

// #define number_of_tasks 6
// #define number_of_issues 100
// #define test_duration_ms 17073
// int ids[number_of_tasks] = {0,1,2,3,4,5};

// int exe_time_ms[number_of_tasks] = {2,1,2,10,20,90};

// int rel_deadline_ms[number_of_tasks] = {6,6,90,95,80,100};
// int priority[number_of_tasks] = {5,4,2,1,3,6};//{5,4,2,1,3,6};
// int issue_ms[number_of_issues] = {32,159,212,255,335,343,374,377,430,484,542,585,643,721,776,835,867,1022,1243,1270,1310,1329,1479,1491,1533,1536,1585,1612,1666,1963,2112,2183,2224,2274,2325,2346,2362,2484,2523,2565,2583,2588,2596,2738,2750,2944,3009,3260,3361,3475,3661,3833,3917,3971,3981,4085,4095,4283,4364,4426,4510,4546,4552,4685,4719,4732,4856,4898,4919,4986,5096,5133,5213,5262,5272,5384,5502,5511,5529,5539,5609,5706,5747,5751,5806,5845,5861,5934,6012,6216,6269,6429,6597,6598,6727,6730,6745,6752,6910,6983};
// int issue_id[number_of_issues] = {3,4,5,2,4,5,3,2,2,3,2,3,2,5,2,4,2,5,3,4,2,5,4,3,5,2,5,4,3,5,2,5,4,3,4,2,2,4,5,3,2,2,3,4,3,3,4,5,2,5,3,4,2,5,3,5,4,3,4,5,2,4,5,5,2,3,4,5,3,4,5,2,4,5,2,2,5,2,4,3,5,4,3,5,2,3,5,2,4,5,2,3,5,2,3,4,5,2,4,2};


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
	// xTaskCreate	(dummy0, (const char *) "0",configMINIMAL_STACK_SIZE, (void *) NULL, priority[0], &dummy_task_handler[0], rel_deadline_ms[0]);
	// xTaskCreate	(dummy1, (const char *) "1",configMINIMAL_STACK_SIZE, (void *) NULL, priority[1], &dummy_task_handler[1], rel_deadline_ms[1]);
	// xTaskCreate	(dummy2, (const char *) "2",configMINIMAL_STACK_SIZE, (void *) NULL, priority[2], &dummy_task_handler[2], rel_deadline_ms[2]);
	// xTaskCreate	(dummy3, (const char *) "3",configMINIMAL_STACK_SIZE, (void *) NULL, priority[3], &dummy_task_handler[3], rel_deadline_ms[3]);
	// xTaskCreate	(dummy4, (const char *) "4",configMINIMAL_STACK_SIZE, (void *) NULL, priority[4], &dummy_task_handler[4], rel_deadline_ms[4]);
	// xTaskCreate	(dummy5, (const char *) "5",configMINIMAL_STACK_SIZE, (void *) NULL, priority[5], &dummy_task_handler[5], rel_deadline_ms[5]);
	// xTaskCreate	(dummy6, (const char *) "6",configMINIMAL_STACK_SIZE, (void *) NULL, priority[6], &dummy_task_handler[6], rel_deadline_ms[6]);
	// xTaskCreate	(dummy7, (const char *) "7",configMINIMAL_STACK_SIZE, (void *) NULL, priority[7], &dummy_task_handler[7], rel_deadline_ms[7]);
	// xTaskCreate	(dummy8, (const char *) "8",configMINIMAL_STACK_SIZE, (void *) NULL, priority[8], &dummy_task_handler[8], rel_deadline_ms[8]);
	// xTaskCreate	(dummy9, (const char *) "9",configMINIMAL_STACK_SIZE, (void *) NULL, priority[9], &dummy_task_handler[9], rel_deadline_ms[9]);
	
	for (int i=0; i< number_of_tasks; i++){
		xTaskCreate	(task_general, (const char *) "",configMINIMAL_STACK_SIZE, (void*)&ids[i], priority[i], &dummy_task_handler[i], 10*rel_deadline_ms[i]);
	}
	

}

//void capsule_recogtion(void)









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



	

//	launch_ssHeatAlg();
//	launch_ssHMI();



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

void flow_disable(void){
	data_flow.en = 0;
	ssFlow_resetValues();
}
void flow_enable(void){
	data_flow.en = 1;
	ssFlow_resetValues();
}

void ssFlow_resetValues(){
	data_flow.fr = 0;
	data_flow.vol = 0;
}

int ssFlow_time2flow(uint32_t time_us){
	int f;
	if (time_us < 300000){
		f = 300;
	}
	else{
		f = 0;
	}
	return f;
}

void ssFlow_t(void *pvParameters){

	unsigned int		pre_time = 0;	//ms
	unsigned int		cur_time = 0;
	unsigned int		time;
	unsigned int		fr_reg 	= 0;
	vTaskSuspend( ssFlow_task_handler );

	while (1){

		cur_time = data_timer.time;
		if (cur_time>pre_time){
			time = cur_time - pre_time;
		}
		else{
			time = (0xffffffffUL)-pre_time+cur_time;	//overflow handling
		}
		pre_time = cur_time;
		data_flow.fr = fr_reg;		//put a register for flow
		fr_reg = ssFlow_time2flow(time*1000000/data_timer.tick_freq);
		data_flow.vol = data_flow.vol + 1;//total_vol + flow_rate; //todo: is this correct way?! or just add number of ticks?! it seems number of ticks is important!

		printf("Fr: %d, vol:%d\r\n", data_flow.fr, data_flow.vol);

//		for
//
//		if(data_timer.time > deadline){
//			missed :)
//		}
	
		vTaskSuspend( ssFlow_task_handler );

	}
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

		// if (data_timer.time%20==0){	
		// 	//heater, id=0, pump, id=1
		// 	for (int i=0; i<2; i++){
		// 		if (test_parameters.finished[i] == 0){	//issueing the task
		// 				test_parameters.missed[i]++;
		// 		}
		// 		else{
		// 			test_parameters.finished[i] = 0;
		// 			vTaskResume(dummy_task_handler[i]);
		// 		}
		// 	}
		// }


		// if (data_timer.time%520==0){
		// 	if (data_flow.en == 1){		
		// 		vTaskResume(ssFlow_task_handler); 
		// 	}
		// }
		// if (data_timer.time%620==0){
		// 	if (data_hmi.en == 1){		
		// 		vTaskResume(ssHMI_b_nsprs_task_handler ); 
		// 	}
		// }
		// if (data_timer.time%720==0){
		// 	if (data_trgpump.en == 1){		
		// 		vTaskResume(ssTrgPump_task_handler ); 
		// 	}
		// }
		// if (data_timer.time%820==0){
		// 	if (data_trgheater.en == 1){		
		// 		vTaskResume(ssTrgHeater_task_handler );
		// 	}	
		// }
		// if (data_timer.time%920==0){
		// 	if (data_temp.en == 1){		
		// 		vTaskResume(ssTemp_task_handler );
		// 	}
		// }			

		// if (data_timer.time == issue_ms[id_time]){
		// 	id_task = issue_id[id_time];					//getting the task
		// 	if (test_parameters.finished[id_task] == 0){	//issueing the task
		// 			test_parameters.missed[id_task]++;
		// 	}
		// 	else{
		// 		test_parameters.finished[id_task] = 0;
		// 		// printf("issue: %lu\r\n", timer_read(2));
		// 		// overal_issue_time = timer_read(2);
		// 		vTaskResume(dummy_task_handler[id_task]);
				
		// 	}
		// 	if (id_time < number_of_issues-1){			//checking for last one
		// 		id_time++;
		// 	}
		// }

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


		
		// for (int i = 0; i<number_of_tasks; i++){
		// 	if (data_timer.time == issue_ms[i]){
		// 		counter++;
		// 		if (test_parameters.finished[i] == 0){
		// 			test_parameters.missed[i]++;
		// 		}
		// 		else{
		// 			test_parameters.finished[i] = 0;
		// 			vTaskResume(dummy_task_handler[i]);
		// 		}
		// 		// issue_ms[i] = issue_ms[i]+500; //I make it periodic!!!	
		//  	}
		// }
		// if (counter == number_of_tasks){
		// 	if (period_after_last==0){
		// 		printf("======= test duration: %d, dynmaic scheduler: %d\r\n", 
		// 		test_duration_ms, configDYNAMIC_SCHEDULING);
		// 		int totall=0;
		// 		for (int i = 0; i<number_of_tasks; i++){
		// 			printf("task %d, number of misses: %d\r\n", i, test_parameters.missed[i]);
		// 			totall += test_parameters.missed[i];
		// 		}
		// 		printf ("totall misses: %d\r\n", totall);
		// 		while(1);
		// 	}
		// 	period_after_last--;
		// }


//		if (data_timer.time == test_duration_ms){
//			printf("======= test duration: %d, dynmaic scheduler: %d\r\n", test_duration_ms, configDYNAMIC_SCHEDULING);
//			int totall=0;
//			for (int i = 0; i<number_of_tasks; i++){
//				printf("task %d, number of misses: %d\r\n", i, test_parameters.missed[i]);
//				totall += test_parameters.missed[i];
//			}
//			printf ("totall misses: %d\r\n", totall);
//			while(1);
//		}
		
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
	unsigned int time_released = 0;
	unsigned int time_pressed = 0;
	unsigned int time_pressing = 0;
	vTaskSuspend( ssHMI_b_nsprs_task_handler );
	while(1){
		if ((data_hmi.data_from_isr & 1<<NSPRS_PRESSED) != 0){
//			data_hmi.req_brew_esprs = 0;	//cmd
			time_pressed = data_timer.time;	//recording time
			data_hmi.time_nsprs_ms = 0;
		}
		else if ((data_hmi.data_from_isr & 1<<NSPRS_RELEASED) != 0){
//			data_hmi.req_brew_esprs = 1;			//cmd
			time_released = data_timer.time;	//time
			if(time_released<time_pressed){
				time_pressing = time_released + (0xffffffffUL)-time_pressed;
			}
			else{
				time_pressing = time_released-time_pressed;
			}
			data_hmi.time_nsprs_ms = time_pressing;
		}
		printf("done :)\r\n");
		vTaskSuspend( ssHMI_b_nsprs_task_handler );
	}

}

void ssHMI_b_lungo_t( void *pvParameters ){
	unsigned int time_released = 0;
	unsigned int time_pressed = 0;
	unsigned int time_pressing = 0;
	vTaskSuspend( ssHMI_b_lungo_task_handler );
	while(1){
		if ((data_hmi.data_from_isr & 1<<LUNGO_PRESSED) != 0){
			data_hmi.req_brew_lungo = 0;	//cmd
			time_pressed = data_timer.time;	//recording time
			data_hmi.time_lungo_ms = 0;
		}
		else if ((data_hmi.data_from_isr & 1<<LUNGO_RELEASED) != 0){
			data_hmi.req_brew_lungo = 1;			//cmd
			time_released = data_timer.time;	//time
			if(time_released<time_pressed){
				time_pressing = time_released + (0xffffffffUL)-time_pressed;
			}
			else{
				time_pressing = time_released-time_pressed;
			}
			data_hmi.time_lungo_ms = time_pressing;
		}
		vTaskSuspend( ssHMI_b_lungo_task_handler );
	}
}

void ssHMI_b_esprs_t( void *pvParameters ){
	unsigned int time_released = 0;
	unsigned int time_pressed = 0;
	unsigned int time_pressing = 0;
	vTaskSuspend( ssHMI_b_esprs_task_handler );
	while(1){
		if ((data_hmi.data_from_isr & 1<<ESPRS_PRESSED) != 0){
			data_hmi.req_brew_esprs = 0;	//cmd
			time_pressed = data_timer.time;	//recording time
			data_hmi.time_esprs_ms = 0;
		}
		else if ((data_hmi.data_from_isr & 1<<ESPRS_RELEASED) != 0){
			data_hmi.req_brew_esprs = 1;			//cmd
			time_released = data_timer.time;	//time
			if(time_released<time_pressed){
				time_pressing = time_released + (0xffffffffUL)-time_pressed;
			}
			else{
				time_pressing = time_released-time_pressed;
			}
			data_hmi.time_esprs_ms = time_pressing;
		}
		vTaskSuspend( ssHMI_b_esprs_task_handler );
	}
}

void ssHMI_b_hw_t( void *pvParameters ){
	unsigned int time_released = 0;
	unsigned int time_pressed = 0;
	unsigned int time_pressing = 0;
	vTaskSuspend( ssHMI_b_hw_task_handler );
	while(1){
		if ((data_hmi.data_from_isr & 1<<HW_PRESSED) != 0){
			data_hmi.req_hw = 0;	//cmd
			time_pressed = data_timer.time;	//recording time
			data_hmi.time_hw_ms = 0;
		}
		else if ((data_hmi.data_from_isr & 1<<HW_RELEASED) != 0){
			data_hmi.req_hw = 1;			//cmd
			time_released = data_timer.time;	//time
			if(time_released<time_pressed){
				time_pressing = time_released + (0xffffffffUL)-time_pressed;
			}
			else{
				time_pressing = time_released-time_pressed;
			}
			data_hmi.time_hw_ms = time_pressing;
		}
		vTaskSuspend( ssHMI_b_hw_task_handler );
	}

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
void pump_disable(void){
	trg_impl_tmr14(TRG_MODE_LOW, 0, 0, 0, 0); 
	data_trgpump.en = 0;
}
void  pump_enable(void){
	trg_impl_tmr14(TRG_MODE_LOW, 0, 0, 0, 0); 
	data_trgpump.en = 1;
}

void ssTrgPump_t( void *pvParameters ){
	trg_init_tmr14(); //timer16 trigger initialization
	trg_init_tmr15(); //todo: used for the valve, change in future when hw was added
	trg_impl_tmr15(TRG_MODE_HIGH, 0, 0, 0, 0); //for the valve
	trg_impl_tmr14(TRG_MODE_LOW, 0, 0, 0, 0); //for the pump, also while disabled should be called TODO
	vTaskSuspend(ssTrgPump_task_handler);

	while(1){
		if(data_trgpump.zc_edge==1){
//		trg_impl_tmr15(TRG_MODE_HIGH, 0, 0, 0, 0); //for the valve
			//brewing
			trg_impl_tmr14(TRG_MODE_T1HIGH_LOW, 4000, 0,  0, 0);
			//HW
//			trg_impl_tmr14(TRG_MODE_LOW, 0, 0,  0, 0);
			printf("trg pump rise\r\n");
		}
		else if (data_trgpump.zc_edge==0){
			//brewing and HW
			trg_impl_tmr14(TRG_MODE_T1LOW_HIGH, 4000, 0,  0, 0);
			printf("trg pump fall\r\n");
		}
		vTaskSuspend(ssTrgPump_task_handler);
	}
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

void heater_disable(void){
	trg_impl_tmr16(TRG_MODE_LOW, 0, 0,  0, 0);
	trg_impl_tmr17(TRG_MODE_LOW, 0, 0,  0, 0);
	data_trgheater.en = 0;
}

void  heater_enable(void){
	trg_impl_tmr16(TRG_MODE_LOW, 0, 0,  0, 0);
	trg_impl_tmr17(TRG_MODE_LOW, 0, 0,  0, 0);
	data_trgheater.en = 1;
}

void ssTrgHeater_t( void *pvParameters ){

	trg_init_tmr16(); //timer16 trigger initialization
	trg_init_tmr17(); //timer17 ""
	trg_impl_tmr16(TRG_MODE_LOW, 0, 0,  0, 0);
	trg_impl_tmr17(TRG_MODE_LOW, 0, 0,  0, 0);

	vTaskSuspend(ssTrgHeater_task_handler);

	while (1)
	{
		if(data_trgheater.zc_edge==1){
			heating_alg();
			if (data_trgheater.heating_alg_result == 1){
				trg_impl_tmr16(TRG_MODE_T1HIGH_LOW, 2000, 0,  0, 0);
//					trg_impl_tmr17(TRG_MODE_T1HIGH_LOW, 2000, 0,  0, 0);
				trg_impl_tmr17(TRG_MODE_T1PWM_T2LOW_HIGH, 2000, 2000,  50, 25); //todo: correct it
				printf("trg heater rise\r\n");
			}	
		}
		else if (data_trgheater.zc_edge==0){
			heating_alg();
			if (data_trgheater.heating_alg_result == 1){
				trg_impl_tmr16(TRG_MODE_T1HIGH_LOW, 2000, 0,  0, 0);
	//					trg_impl_tmr17(TRG_MODE_T1HIGH_LOW, 2000, 0,  0, 0);
				trg_impl_tmr17(TRG_MODE_T1PWM_T2LOW_HIGH, 2000, 2000,  50, 25);
				printf("trg heater fall\r\n");
			}
		}
		
		vTaskSuspend(ssTrgHeater_task_handler);
	}
	

}

void heating_alg(void){
	ssHeatAlg_checkAlgState();
	ssHeatAlg_checkTargetTemp();
	ssHeatAlg_putAlgState();
	ssHeatAlg_Alg();


}

static void ssHeatAlg_checkAlgState(void){
	if (asState == asStateHeatup){
		heatalg_alg_next_state = fast_heating;
	}
	else if (asState == asStateReady){
		heatalg_alg_next_state = no_heating;
	}
	else if (asState == asStateBrewing){
//		heatalg_alg_next_state = brew_heating;
		heatalg_alg_next_state = slow_heating; //todo: correct this part
	}
	else if (asState == asStatePoweroff){
		heatalg_alg_next_state = no_heating;
	}
	else{
		heatalg_alg_next_state = no_heating;
	}
}

static void ssHeatAlg_checkTargetTemp(void){
	if (global_vars.temperature > global_vars.temperature_target){
		heatalg_alg_next_state = no_heating;
	}
//	printf("temp: %d\r\n", global_vars.temperature);
}

static void ssHeatAlg_putAlgState(void){
	if (heatalg_alg_next_state != heatalg_alg_state){
		ssHeatAlg_Alg_substate_firstrun = 1;
	}
	heatalg_alg_state = heatalg_alg_next_state;
}

static void ssHeatAlg_Alg(void){
	if (ssHeatAlg_Alg_substate_firstrun == 1){
		ssHeatAlg_Alg_substate_firstrun = 0;
		heatalg_alg_fastheatup_state = init_en_heater__fastheatupstate;
		heatalg_alg_noheatup_state = init_dis_heater__noheatupstate;
		heatalg_alg_slowheatup_state = init__slowheatupstate;
		//do the same thing for others
	}
	switch(heatalg_alg_state){
	case fast_heating:
		ssHeatAlg_Alg_FastHeating();
//		printf("fast Heating\r\n");
		break;
	case slow_heating:
		ssHeatAlg_Alg_SlowHeating();
//		printf("slow Heating\r\n");
		break;
	case brew_heating:
		ssHeatAlg_Alg_BrewHeating();
//		printf("brew Heating\r\n");
		break;
	case no_heating:
		ssHeatAlg_Alg_NoHeating();
//		printf("no Heating\r\n");
		break;
	default:
		heatalg_alg_next_state = slow_heating;
		break;
	}
}

static void ssHeatAlg_Alg_FastHeating(void){
	switch(heatalg_alg_fastheatup_state){
	case init_en_heater__fastheatupstate:
		ssHeatAlg_enHeater();
		heatalg_alg_fastheatup_state = wait__fastheatupstate;
		break;
	case wait__fastheatupstate:
		break;
	}
};


static void ssHeatAlg_Alg_SlowHeating(void){
	static unsigned int time_now = 0, time_start = 0;
	switch(heatalg_alg_slowheatup_state){
	case init__slowheatupstate:
		//nothing
		heatalg_alg_slowheatup_state = on_state__slowheatupstate;
		heatalg_alg_slowheatup_on_state = init___slowheatuponstate;
		heatalg_alg_slowheatup_off_state = init___slowheatupoffstate;
		break;

	case on_state__slowheatupstate:
		switch(heatalg_alg_slowheatup_on_state){
		case init___slowheatuponstate:
//			printf("on init\r\n");
			ssHeatAlg_enHeater();	//enable signaling
			time_start = data_timer.time;
			heatalg_alg_slowheatup_on_state = wait___slowheatuponstate; //go to wait substate
			break;
		case wait___slowheatuponstate:
//			printf("on wait\r\n");
			time_now = data_timer.time;
			if (time_now > time_start+HEATALG_SLOW_TIMEON){	//go to offstate if time finished
				heatalg_alg_slowheatup_on_state = init___slowheatuponstate;
				heatalg_alg_slowheatup_off_state = init___slowheatupoffstate;
				heatalg_alg_slowheatup_state = off_state__slowheatupstate;
			}
			break;
		}
		break;
	case off_state__slowheatupstate:
		switch(heatalg_alg_slowheatup_off_state){
		case init___slowheatupoffstate:
//			printf("off init\r\n");
			ssHeatAlg_disHeater();	//disable signaling
			time_start = data_timer.time;
			heatalg_alg_slowheatup_off_state = wait___slowheatupoffstate; //go to wait substate
			heatalg_alg_slowheatup_state = off_state__slowheatupstate;
			break;
		case wait___slowheatupoffstate:
//			printf("off wait\r\n");
			time_now = data_timer.time;
			if (time_now > time_start+HEATALG_SLOW_TIMEOFF){	//go to offstate if time finished
				heatalg_alg_slowheatup_off_state = init___slowheatupoffstate;
				heatalg_alg_slowheatup_on_state = init___slowheatuponstate;
				heatalg_alg_slowheatup_state = on_state__slowheatupstate;
			}
			break;
		}
		break;
	}
}

static void ssHeatAlg_Alg_BrewHeating(void){
}

static void ssHeatAlg_Alg_NoHeating(void){
	switch(heatalg_alg_noheatup_state){
	case init_dis_heater__noheatupstate:
		ssHeatAlg_disHeater();
		heatalg_alg_noheatup_state = wait__noheatupstate;
		break;
	case wait__noheatupstate:
		break;
	}
}

static void ssHeatAlg_enHeater(void){
	data_trgheater.heating_alg_result = 1;
}

static void ssHeatAlg_disHeater(void){
	data_trgheater.heating_alg_result = 0;
}


//=======================
DATA_Temp data_temp= {
	.en = 1,
	.temp = 0
};


void ssTemp_t(void *pvParameters){
	data_temp.temp = 0;
	unsigned int start, end;
	vTaskSuspend(ssTemp_task_handler);

	while(1){
		start = data_timer.time;
		// data_temp.temp = readTemp();
		global_vars.temperature = data_temp.temp;
		end = data_timer.time;
		printf("temp exe: %d\r\n", end-start);

		vTaskSuspend(ssTemp_task_handler);
	}


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
	// printf("initialize\tid:%d, exe:%d\r\n", id, exe_time_ms[id]);
	// int inner = 1029;//1000;
	// int outer = (exe_time_ms[id]*500)/(400);		//500 equal to 400ms
	int outer = exe_time_ms[id];
	unsigned int finish_time;
	unsigned int start, end; //r
	vTaskSuspend(dummy_task_handler[id]); 

	while(1){
		// start = xTaskGetTickCount(); //r
		// overal_run_time = timer_read(2);
		// if (overal_run_time>overal_issue_time){
		// 	overal_scheduling_time = overal_run_time-overal_issue_time;
		// }
		// else{
		// 	overal_scheduling_time = timer_readMax(2)-overal_issue_time+overal_run_time;
		// }
		
		// printf("scheduling time: %lu X10us\r\n", overal_scheduling_time);
		
		for (int j=0; j<outer; j++){
			for (int i =0; i<1070; i++){

			}
		}	
		finish_time = xTaskGetTickCount(); //r

		// printxf("id: %d, exe: %d\r\n", id, finish_time-start);

		// for (int i =0; i<outer; i++)
		// 	for (int j=0; j<inner; j++);
		// finish_time = xTaskGetTickCount();
		if(finish_time>uxTaskDeadlineAbsGet(dummy_task_handler[id])){
			test_parameters.missed[id]++;
			// printf("task%d, missed, deadline=%lu, finished=%lu\r\n", id, uxTaskDeadlineAbsGet(dummy_task_handler[id]), finish_time);
		}
		else{
			// printf("task%d, passed, deadline=%lu, finished=%lu\r\n", id, uxTaskDeadlineAbsGet(dummy_task_handler[id]), finish_time);
		}
		test_parameters.finished[id] = 1;

		vTaskSuspend(dummy_task_handler[id]);
	}
	
}


// void dummy0(void *pvParameters){
// 	int task_number = 0;
// 	int inner = 1029;
// 	int outer = (exe_time_ms[task_number]*500)/(400);		//500 equal to 400ms
// 	unsigned int finish_time;
// 	vTaskSuspend(dummy_task_handler[task_number]);

// 	while(1){
// 		for (int i =0; i<outer; i++)
// 			for (int j=0; j<inner; j++);
// 		finish_time = xTaskGetTickCount();
// 		if(finish_time>uxTaskDeadlineAbsGet(dummy_task_handler[task_number])){
// 			printf("task%d, missed, +%lu\r\n", task_number, (finish_time-uxTaskDeadlineAbsGet(dummy_task_handler[task_number])));
// 		}
// 		else{
// 			printf("task%d, passed, -%lu\r\n", task_number, (uxTaskDeadlineAbsGet(dummy_task_handler[task_number])-finish_time));
// 		}

// 		vTaskSuspend(dummy_task_handler[task_number]);
// 	}
// }
// void dummy1(void *pvParameters)
// {
// 	int task_number = 1;
// 	int inner = 1029;
// 	int outer = (exe_time_ms[task_number]*500)/(400);		//500 equal to 400ms
// 	unsigned int finish_time;
// 	vTaskSuspend(dummy_task_handler[task_number]);

// 	while(1){
// 		for (int i =0; i<outer; i++)
// 			for (int j=0; j<inner; j++);
// 		finish_time = xTaskGetTickCount();
// 		if(finish_time>uxTaskDeadlineAbsGet(dummy_task_handler[task_number])){
// 			printf("task%d, missed, +%lu\r\n", task_number, (finish_time-uxTaskDeadlineAbsGet(dummy_task_handler[task_number])));
// 		}
// 		else{
// 			printf("task%d, passed, -%lu\r\n", task_number, (uxTaskDeadlineAbsGet(dummy_task_handler[task_number])-finish_time));
// 		}
// 		vTaskSuspend(dummy_task_handler[task_number]);
// 	}
// }

// void dummy2(void *pvParameters){
// 	int task_number = 2;
// 	int inner = 1029;
// 	int outer = (exe_time_ms[task_number]*500)/(400);		//500 equal to 400ms
// 	unsigned int finish_time;
// 	vTaskSuspend(dummy_task_handler[task_number]);

// 	while(1){
// 		for (int i =0; i<outer; i++)
// 			for (int j=0; j<inner; j++);
// 		finish_time = xTaskGetTickCount();
// 		if(finish_time>uxTaskDeadlineAbsGet(dummy_task_handler[task_number])){
// 			printf("task%d, missed, +%lu\r\n", task_number, (finish_time-uxTaskDeadlineAbsGet(dummy_task_handler[task_number])));
// 		}
// 		else{
// 			printf("task%d, passed, -%lu\r\n", task_number, (uxTaskDeadlineAbsGet(dummy_task_handler[task_number])-finish_time));
// 		}
// 		vTaskSuspend(dummy_task_handler[task_number]);
// 	}
// }

// void dummy3(void *pvParameters){
// 	int task_number = 3;
// 	int inner = 1029;
// 	int outer = (exe_time_ms[task_number]*500)/(400);		//500 equal to 400ms
// 	unsigned int finish_time;
// 	vTaskSuspend(dummy_task_handler[task_number]);

// 	while(1){
// 		for (int i =0; i<outer; i++)
// 			for (int j=0; j<inner; j++);
// 		finish_time = xTaskGetTickCount();
// 		if(finish_time>uxTaskDeadlineAbsGet(dummy_task_handler[task_number])){
// 			printf("task%d, missed, +%lu\r\n", task_number, (finish_time-uxTaskDeadlineAbsGet(dummy_task_handler[task_number])));
// 		}
// 		else{
// 			printf("task%d, passed, -%lu\r\n", task_number, (uxTaskDeadlineAbsGet(dummy_task_handler[task_number])-finish_time));
// 		}
// 		vTaskSuspend(dummy_task_handler[task_number]);
// 	}
// }
// void dummy4(void *pvParameters){
// 	int task_number = 4;
// 	int inner = 1029;
// 	int outer = (exe_time_ms[task_number]*500)/(400);		//500 equal to 400ms
// 	unsigned int finish_time;
// 	vTaskSuspend(dummy_task_handler[task_number]);

// 	while(1){
// 		for (int i =0; i<outer; i++)
// 			for (int j=0; j<inner; j++);
// 		finish_time = xTaskGetTickCount();
// 		if(finish_time>uxTaskDeadlineAbsGet(dummy_task_handler[task_number])){
// 			printf("task%d, missed, +%lu\r\n", task_number, (finish_time-uxTaskDeadlineAbsGet(dummy_task_handler[task_number])));
// 		}
// 		else{
// 			printf("task%d, passed, -%lu\r\n", task_number, (uxTaskDeadlineAbsGet(dummy_task_handler[task_number])-finish_time));
// 		}
// 		vTaskSuspend(dummy_task_handler[task_number]);
// 	}
// }
// void dummy5(void *pvParameters){
// 	int task_number = 5;
// 	int inner = 1029;
// 	int outer = (exe_time_ms[task_number]*500)/(400);		//500 equal to 400ms
// 	unsigned int finish_time;
// 	vTaskSuspend(dummy_task_handler[task_number]);

// 	while(1){
// 		for (int i =0; i<outer; i++)
// 			for (int j=0; j<inner; j++);
// 		finish_time = xTaskGetTickCount();
// 		if(finish_time>uxTaskDeadlineAbsGet(dummy_task_handler[task_number])){
// 			printf("task%d, missed, +%lu\r\n", task_number, (finish_time-uxTaskDeadlineAbsGet(dummy_task_handler[task_number])));
// 		}
// 		else{
// 			printf("task%d, passed, -%lu\r\n", task_number, (uxTaskDeadlineAbsGet(dummy_task_handler[task_number])-finish_time));
// 		}
// 		vTaskSuspend(dummy_task_handler[task_number]);
// 	}
// }
// void dummy6(void *pvParameters){
// 	int task_number = 6;
// 	int inner = 1029;
// 	int outer = (exe_time_ms[task_number]*500)/(400);		//500 equal to 400ms
// 	unsigned int finish_time;
// 	vTaskSuspend(dummy_task_handler[task_number]);

// 	while(1){
// 		for (int i =0; i<outer; i++)
// 			for (int j=0; j<inner; j++);
// 		finish_time = xTaskGetTickCount();
// 		if(finish_time>uxTaskDeadlineAbsGet(dummy_task_handler[task_number])){
// 			printf("task%d, missed, +%lu\r\n", task_number, (finish_time-uxTaskDeadlineAbsGet(dummy_task_handler[task_number])));
// 		}
// 		else{
// 			printf("task%d, passed, -%lu\r\n", task_number, (uxTaskDeadlineAbsGet(dummy_task_handler[task_number])-finish_time));
// 		}
// 		vTaskSuspend(dummy_task_handler[task_number]);
// 	}
// }
// void dummy7(void *pvParameters){
// 	int task_number = 7;
// 	int inner = 1029;
// 	int outer = (exe_time_ms[task_number]*500)/(400);		//500 equal to 400ms
// 	unsigned int finish_time;
// 	vTaskSuspend(dummy_task_handler[task_number]);

// 	while(1){
// 		for (int i =0; i<outer; i++)
// 			for (int j=0; j<inner; j++);
// 		finish_time = xTaskGetTickCount();
// 		if(finish_time>uxTaskDeadlineAbsGet(dummy_task_handler[task_number])){
// 			printf("task%d, missed, +%lu\r\n", task_number, (finish_time-uxTaskDeadlineAbsGet(dummy_task_handler[task_number])));
// 		}
// 		else{
// 			printf("task%d, passed, -%lu\r\n", task_number, (uxTaskDeadlineAbsGet(dummy_task_handler[task_number])-finish_time));
// 		}
// 		vTaskSuspend(dummy_task_handler[task_number]);
// 	}
// }
// void dummy8(void *pvParameters){
// 	int task_number = 8;
// 	int inner = 1029;
// 	int outer = (exe_time_ms[task_number]*500)/(400);		//500 equal to 400ms
// 	unsigned int finish_time;
// 	vTaskSuspend(dummy_task_handler[task_number]);

// 	while(1){
// 		for (int i =0; i<outer; i++)
// 			for (int j=0; j<inner; j++);
// 		finish_time = xTaskGetTickCount();
// 		if(finish_time>uxTaskDeadlineAbsGet(dummy_task_handler[task_number])){
// 			printf("task%d, missed, +%lu\r\n", task_number, (finish_time-uxTaskDeadlineAbsGet(dummy_task_handler[task_number])));
// 		}
// 		else{
// 			printf("task%d, passed, -%lu\r\n", task_number, (uxTaskDeadlineAbsGet(dummy_task_handler[task_number])-finish_time));
// 		}
// 		vTaskSuspend(dummy_task_handler[task_number]);
// 	}
// }
// void dummy9(void *pvParameters){
// 	 int task_number = 9;
// 	int inner = 1029;
// 	int outer = (exe_time_ms[task_number]*500)/(400);		//500 equal to 400ms
// 	unsigned int finish_time;
// 	vTaskSuspend(dummy_task_handler[task_number]);

// 	while(1){
// 		for (int i =0; i<outer; i++)
// 			for (int j=0; j<inner; j++);
// 		finish_time = xTaskGetTickCount();
// 		if(finish_time>uxTaskDeadlineAbsGet(dummy_task_handler[task_number])){
// 			printf("task%d, missed, +%lu\r\n", task_number, (finish_time-uxTaskDeadlineAbsGet(dummy_task_handler[task_number])));
// 		}
// 		else{
// 			printf("task%d, passed, -%lu\r\n", task_number, (uxTaskDeadlineAbsGet(dummy_task_handler[task_number])-finish_time));
// 		}
// 		vTaskSuspend(dummy_task_handler[task_number]);
// 	}
// }


//================================================================================================================================
//================================================================================================================================
//=============================================== back up ========================================================================
//================================================================================================================================
//================================================================================================================================
//================================================================================================================================
//
//
////=== states
//ssHeatAlgTaskStates heatalg_task_state = sleep;
//ssHeatAlg_AlgStates heatalg_alg_state = slow_heating;
//ssHeatAlg_AlgStates heatalg_alg_next_state = slow_heating;
//ssHeatAlg_Alg_FastHeatingStates heatalg_alg_fastheatup_state = init_en_heater__fastheatupstate;
//ssHeatAlg_Alg_NoHeatingStates heatalg_alg_noheatup_state = init_dis_heater__noheatupstate;
//ssHeatAlg_Alg_SlowHeatingStates heatalg_alg_slowheatup_state = init__slowheatupstate;
//ssHeatAlg_Alg_SlowHeating_OnStates heatalg_alg_slowheatup_on_state = init___slowheatuponstate;
//ssHeatAlg_Alg_SlowHeating_OffStates heatalg_alg_slowheatup_off_state = init___slowheatupoffstate;
//
//extern int ssHeatAlg_Alg_substate_firstrun = 1;
//
////==== periods ====
//int ssHeatAlg_periodMs = 50;	//period of this service
//int ssTemp_periodMs = 100;
//
//
//
//
////============= launch tasks ====================
//void launch_ssHeatPump (void){
//	xTaskCreateDynSch	(ssHeatPump_t, (const char *) "sys srvc, heater and pump enabling",configMINIMAL_STACK_SIZE, (void *) NULL, HIGH_PRIORITY, &ssHeatPump_task_handler, portMAX_DELAY);
//}
//void launch_ssTrgPump (void){
//	xTaskCreateDynSch	(ssTrgPump_t, (const char *) "sys srvc, trg pump enabling",configMINIMAL_STACK_SIZE, (void *) NULL, HIGH_PRIORITY, &ssTrgPump_task_handler, portMAX_DELAY);
//}
//void launch_ssTrgHeater (void){
//	xTaskCreateDynSch	(ssTrgHeater_t, (const char *) "sys srvc, trg heater enabling",configMINIMAL_STACK_SIZE, (void *) NULL, HIGH_PRIORITY, &ssTrgHeater_task_handler, portMAX_DELAY);
//}
//void launch_ssTimer (void){
//	xTaskCreateDynSch	(ssTimer_t, (const char *) "sys srvc, timer", configMINIMAL_STACK_SIZE, (void *) NULL, HIGH_PRIORITY, &ssTimer_task_handler, portMAX_DELAY);
//}
//void launch_ssTimer2 (void){
//	xTaskCreateDynSch	(ssTimer2_t, (const char *) "sys srvc, timer2", configMINIMAL_STACK_SIZE, (void *) NULL, HIGH_PRIORITY, &ssTimer2_task_handler, portMAX_DELAY);
//}
//void launch_ssTimer3 (void){
//	xTaskCreateDynSch	(ssTimer3_t, (const char *) "sys srvc, timer3", configMINIMAL_STACK_SIZE, (void *) NULL, HIGH_PRIORITY, &ssTimer3_task_handler, portMAX_DELAY);
//}
//void launch_ssTimer4 (void){
//	xTaskCreateDynSch	(ssTimer4_t, (const char *) "sys srvc, timer4", configMINIMAL_STACK_SIZE, (void *) NULL, HIGH_PRIORITY, &ssTimer4_task_handler, portMAX_DELAY);
//}
//void launch_ssTimer5 (void){
//	xTaskCreateDynSch	(ssTimer5_t, (const char *) "sys srvc, timer5", configMINIMAL_STACK_SIZE, (void *) NULL, HIGH_PRIORITY, &ssTimer5_task_handler, portMAX_DELAY);
//}
//void launch_ssHMI (void){
//	xTaskCreateDynSch	(ssHMI_t, (const char *) "sys srvc, HMI", configMINIMAL_STACK_SIZE, (void *) NULL, HIGH_PRIORITY,  &ssHMI_task_handler, portMAX_DELAY);
//}
//void launch_ssFlow (void){
//	xTaskCreateDynSch	(ssFlow_t, (const char *) "sys srvc, Flow", configMINIMAL_STACK_SIZE, (void *) NULL, HIGH_PRIORITY, &ssFlow_task_handler, portMAX_DELAY);
//}
//void launch_ssTemp (void){
//	xTaskCreateDynSch	(ssTemp_t, (const char *) "sys srvc, Temperature", configMINIMAL_STACK_SIZE, (void *) NULL, HIGH_PRIORITY, &ssTemp_task_handler, portMAX_DELAY);
//}
//void launch_ssHeatAlg (void){
//	xTaskCreateDynSch	(ssHeatAlg_t, (const char *) "sys srvc, Heating Algorithm", configMINIMAL_STACK_SIZE, (void *) NULL, HIGH_PRIORITY, &ssHeatAlg_task_handler, portMAX_DELAY);
//}
//
//void launch_task1 (void){
//	xTaskCreateDynSch	(ssTask1_t, (const char *) "test task 1", configMINIMAL_STACK_SIZE, (void *) NULL, HIGH_PRIORITY, &ssTask1_task_handler, portMAX_DELAY);
//}
//void launch_task2 (void){
//	xTaskCreateDynSch	(ssTask2_t, (const char *) "test task 2", configMINIMAL_STACK_SIZE, (void *) NULL, NORMAL_PRIORITY, &ssTask2_task_handler, portMAX_DELAY);
//}
//
//void launch_test2_1 (void){
//	xTaskCreateDynSch	(ssTest2_1_t, (const char *) "test2 task 1", configMINIMAL_STACK_SIZE, (void *) NULL, NORMAL_PRIORITY, &ssTest2_1_task_handler, portMAX_DELAY);
//}
//void launch_test2_2 (void){
//	xTaskCreateDynSch	(ssTest2_2_t, (const char *) "test2 task 2", configMINIMAL_STACK_SIZE, (void *) NULL, NORMAL_PRIORITY, &ssTest2_2_task_handler, portMAX_DELAY);
//}
//
//void launch_test2_highp (void){
//	xTaskCreateDynSch	(ssTest2_highp_t, (const char *) "test2 task high p, for deferable isr", configMINIMAL_STACK_SIZE, (void *) NULL, 11, &ssTest2_highp_task_handler, portMAX_DELAY);
//}
///*ht: adding deadline to all tasks :)*/
//
//void launch_ssAll(void){
//	launch_ssTrgPump();
//	launch_ssTrgHeater();
//	launch_ssTimer();
////	launch_ssTimer2();
//	launch_ssHMI();
//	launch_ssFlow();
//	launch_ssTemp();
//	launch_ssHeatAlg();
//
//
//}
//
////============= delete tasks =================
//void delete_ssHeatPump (void){
//	vTaskDelete( ssHeatPump_task_handler );
//}
//void delete_ssTrgPump (void){
//	vTaskDelete( ssTrgPump_task_handler );
//}
//void delete_ssTrgHeater (void){
//	vTaskDelete( ssTrgHeater_task_handler );
//}
//void delete_ssTimer (void){
//	vTaskDelete( ssTimer_task_handler );
//}
//void delete_ssTimer2 (void){
//	vTaskDelete( ssTimer2_task_handler );
//}
//void delete_ssTimer3 (void){
//	vTaskDelete( ssTimer3_task_handler );
//}
//void delete_ssTimer4 (void){
//	vTaskDelete( ssTimer4_task_handler );
//}
//void delete_ssTimer5 (void){
//	vTaskDelete( ssTimer5_task_handler );
//}
//void delete_ssHMI (void){
//	vTaskDelete( ssHMI_task_handler );
//}
//void delete_ssFlow (void){
//	vTaskDelete( ssFlow_task_handler );
//}
//void delete_ssTemp (void){
//	vTaskDelete( ssTemp_task_handler );
//}
//void delete_ssHeatAlg (void){
//	vTaskDelete( ssHeatAlg_task_handler );
//}
//void delete_ssAll(void){
//	delete_ssHeatPump ();
//	delete_ssTrgPump ();
//	delete_ssTrgHeater ();
//	delete_ssTimer ();
//	delete_ssTimer2 ();
//	delete_ssTimer3 ();
//	delete_ssTimer4 ();
//	delete_ssTimer5 ();
//	delete_ssHMI ();
//	delete_ssFlow ();
//	delete_ssTemp ();
//	delete_ssHeatAlg ();
//}
//
////========== tasks =============
//void ssHeatPump_t( void *pvParameters ){
//	enum ssHeatPumpStates {
//		sleep,
//		detectZC,
//		rcvdCmd
//	};
//	enum ssHeatPumpStates state;
//	state = sleep;
//	ssHeatPump_d * p_data;
//	char enPump = 0;
//	char enHeater = 0;
//	char zc_edge;
//
//	while(1){
//		switch(state){
//		case sleep:
//			if(xQueueReceive(ssHeatPump_qh, &(p_data), portMAX_DELAY) == 1){
//				state = rcvdCmd;
//			}
//			break;
//
//		case rcvdCmd:
//			if (p_data->cmd & 1<<2){
//				state = sleep;
//			}
//			else{
//				enHeater = (p_data->cmd & 1<<0);
//				enPump = (p_data->cmd & 1<<1); 	//it is not egual to one, so should check with not equal to zero :)
//				state = detectZC;
//
//			}
//			xSemaphoreGive(p_data->callingTaskSem);
//			break;
//		case detectZC:
//			//todo: remove max_time, put a limited time, if not zc signal, report on log
//			if(xQueueReceive(intr2ssHeatPump_qh, &(zc_edge), portMAX_DELAY) == 1){
//				if (enPump != 0){
//					sig2pump(zc_edge);
//				}
//				if (enHeater != 0){
//					sig2tb(zc_edge);
//				}
//
//			}
//			else{
//			//esle bin=not_recieved
//				//log, error, not detecting ZC signal
//			}
//			if(xQueueReceive(ssHeatPump_qh, &(p_data), 0) == 1){
//				state = rcvdCmd;
//			}
//			break;
//		}
//	}
//
//}
//
//void ssTimer_t( void *pvParameters ){
//	//todo: supporting multi-timer
//	//todo: remove states related to setup, and use functions (which are timer driver functions)
//
//	enum states {
//		sleep,
//		timerSetup,
//		active,
//		checkAs,
//		rcvdCmd,
//		timerReset,
//		timerValue,
//		fr_active	//fr waiting
//	};
//	enum states state;
//
//	ssTimer_d * p_data;
//	unsigned int cnt_remainder, cnt_quotient;
//	state = sleep;
//	unsigned int count_val = 0;
//
////	unsigned int time_us_uint = 0;
//	unsigned int time_ms_uint = 0;
//
////	float time_ms_float = 0;
//	int timer_id = 1;
//
//
//
//	while(1){
//		switch(state){
//		case sleep:
//			if(xQueueReceive(ssTimer_qh, &(p_data), portMAX_DELAY) == 1){
//				xSemaphoreTake(intr2ssTimer_sh ,0); //reset semaphore
//				timer_stopReset(timer_id); //reset timer
//				state = rcvdCmd;
//			}
//			break;
//
//		case timerReset:
////			timer_rst(timer_id);
//			timer_stopReset(timer_id);
//			state = sleep;
//			break;
//
//		case timerSetup:
//			//calculate timer amount
//			//below if required for handling large amounts :)
//			if (cnt_quotient>0){
//				count_val = timer_readMax(timer_id);
//			}
//			else{
//				count_val = cnt_remainder;
//			}
////			printf("cnt_val: %lu", count_val);
//			timer_stopReset(timer_id);
//			timer_count(timer_id, count_val);
//			state = active;
//			break;
//
//		case timerValue:
//			//todo: change this part based on us
////			count_val = p_data->time;
//
////			count_val = (unsigned int)(p_data->time * 1000);
//			count_val = p_data->time; //in us
////			cnt_remainder = count_val % timer_readMaxMs(timer_id);
////			cnt_remainder = count_val % timer_readMaxUs(timer_id);
//			cnt_remainder = count_val % timer_readMaxMs(timer_id);
////			cnt_quotient  = count_val / timer_readMaxMs(timer_id);
////			cnt_quotient  = count_val / timer_readMaxUs(timer_id);
//			cnt_quotient  = count_val / timer_readMaxMs(timer_id);
////			cnt_remainder = cnt_remainder * (timer_readFreq(timer_id)/1000);
////			cnt_remainder = cnt_remainder / (timer_readPeriodUs(timer_id));
//			cnt_remainder = cnt_remainder / (timer_readPeriodMs(timer_id));
//			cnt_quotient  = cnt_quotient;
////			printf("rem: %lu, qu: %lu", cnt_remainder, cnt_quotient);
//			state = timerSetup; //timerSetup;
//			break;
//
//		case active:
//			if (xSemaphoreTake(intr2ssTimer_sh ,pdMS_TO_TICKS(50)) == 1){
////			if (xSemaphoreTake(intr2ssTimer_sh ,pdMS_TO_TICKS(1000)) == 1){
//				if (cnt_quotient>0){
//					//still needs running
//					state = timerSetup;
//					cnt_quotient = cnt_quotient - 1;
//				}
//				else{
//					xSemaphoreGive(p_data->callingTaskSem);
//					state = timerReset; //and then sleep
//				}
//			}
//			else{
//				state = checkAs;
//			}
//			break;
//
//		case checkAs:
//			if(xQueueReceive(ssTimer_qh, &(p_data), 0) == 1){
//				state = rcvdCmd;
//			}
//			else{
//				state = active;
//			}
//			break;
//
//		case fr_active:
//			//check inter for OF recording
//			if (xSemaphoreTake(intr2ssTimer_sh ,pdMS_TO_TICKS(100)) == 1){
//				// record overflowed happened
//				cnt_quotient = cnt_quotient + 1;
//				timer_stopReset(timer_id);
//				timer_count(timer_id, timer_readMax(timer_id));
//			}
//			// check command recieved or not
////			if(xQueueReceive(ssTimer_qh, &(p_data), pdMS_TO_TICKS(100)) == 1){
//			if(xQueueReceive(ssTimer_qh, &(p_data), 0) == 1){
//				state = rcvdCmd;
//			}
//			break;
//
//		case rcvdCmd:
//			if     (p_data->cmd == params.sys_srvc.timer.cmd.sleep){	//go to sleep
//				state = timerReset;
//				xSemaphoreGive(p_data->callingTaskSem); //free sem here
//			}
//			else if (p_data->cmd == params.sys_srvc.timer.cmd.start_freerunning){					//start freerunning
//				timer_stopReset(timer_id);
//				timer_count(timer_id, timer_readMax(timer_id));
//				cnt_quotient = 0; 						//for storing number of times it is overflowed :)
//				//give back sem
//				xSemaphoreGive(p_data->callingTaskSem); //free sem here
//				//go to fr-waiting (check cmd here)
//				state = fr_active;
//			}
//			else if (p_data->cmd == params.sys_srvc.timer.cmd.read_freerunning){					//read but still continue freerunning
//				//change time to MS
////				time_ms = timer_readMs(timer_id);
////				time_us_uint = timer_readUs(timer_id);
//				time_ms_uint = timer_readMs(timer_id);
//				if (cnt_quotient !=0 ){
////					time_ms = time_ms + cnt_quotient * timer_readMaxMs(timer_id);
////					time_us_uint = time_us_uint + cnt_quotient * timer_readMaxUs(timer_id);
//					time_ms_uint = time_ms_uint + cnt_quotient * timer_readMaxMs(timer_id);
//				}
//				//put time in data struct
////				p_data->time = time_ms;
////				time_ms_float = (float)(time_us_uint / 1000);
////				p_data->time = time_us_uint;
//				p_data->time = time_ms_uint;
//				//give back sem
//				xSemaphoreGive(p_data->callingTaskSem); //free sem here
//				//go to waiting
//				state = fr_active;
//
//			}
//			else if (p_data->cmd == params.sys_srvc.timer.cmd.finish_freerunning){					//finish freerunning
//				//change time counter to us
////				time_ms = timer_readMs(timer_id);
////				time_us_uint = timer_readUs(timer_id);
//				time_ms_uint = timer_readMs(timer_id);
//				if (cnt_quotient !=0 ){
////					time_ms = time_ms + cnt_quotient * timer_readMaxMs(timer_id);
////					time_us_uint = time_us_uint + cnt_quotient * timer_readMaxUs(timer_id);
//					time_ms_uint = time_ms_uint + cnt_quotient * timer_readMaxMs(timer_id);
//				}
//				//put time in data struct
////				p_data->time = time_ms;
////				time_ms_float = (float)(time_us_uint / 1000);
////				p_data->time = time_us_uint;
//				p_data->time = time_ms_uint;
//				timer_stopReset(timer_id);
//				//give back semaphore
//				xSemaphoreGive(p_data->callingTaskSem); //free sem here
//				//go to sleep
//				state = sleep;
//			}
//			else { 										//activate intr-based timer
//				state = timerValue;
//				//sem  will be freed after time passed
//			}
//			break;
//		}
//	}
//
//
//}
//
//
//void ssTimer2_t( void *pvParameters ){
//	//todo: supporting multi-timer
//	//todo: remove states related to setup, and use functions (which are timer driver functions)
//
//	enum states {
//		sleep,
//		timerSetup,
//		active,
//		checkAs,
//		rcvdCmd,
//		timerReset,
//		timerValue,
//		fr_active	//fr waiting
//	};
//	enum states state;
//
//	ssTimer_d * p_data;
//	unsigned int cnt_remainder, cnt_quotient;
//	state = sleep;
//	unsigned int count_val = 0;
//
////	unsigned int time_us_uint = 0;
//	unsigned int time_ms_uint = 0;
//
////	float time_ms_float = 0;
//	int timer_id = 2;
//
//
//
//	while(1){
//		switch(state){
//		case sleep:
//			if(xQueueReceive(ssTimer2_qh, &(p_data), portMAX_DELAY) == 1){
//				xSemaphoreTake(intr2ssTimer2_sh ,0); //reset semaphore
//				timer_stopReset(timer_id); //reset timer
//				state = rcvdCmd;
//			}
//			break;
//
//		case timerReset:
////			timer_rst(timer_id);
//			timer_stopReset(timer_id);
//			state = sleep;
//			break;
//
//		case timerSetup:
//			//calculate timer amount
//			//below if required for handling large amounts :)
//			if (cnt_quotient>0){
//				count_val = timer_readMax(timer_id);
//			}
//			else{
//				count_val = cnt_remainder;
//			}
////			printf("cnt_val: %lu", count_val);
//			timer_stopReset(timer_id);
//			timer_count(timer_id, count_val);
//			state = active;
//			break;
//
//		case timerValue:
//			//todo: change this part based on us
////			count_val = p_data->time;
//
////			count_val = (unsigned int)(p_data->time * 1000);
//			count_val = p_data->time; //in us
////			cnt_remainder = count_val % timer_readMaxMs(timer_id);
////			cnt_remainder = count_val % timer_readMaxUs(timer_id);
//			cnt_remainder = count_val % timer_readMaxMs(timer_id);
////			cnt_quotient  = count_val / timer_readMaxMs(timer_id);
////			cnt_quotient  = count_val / timer_readMaxUs(timer_id);
//			cnt_quotient  = count_val / timer_readMaxMs(timer_id);
////			cnt_remainder = cnt_remainder * (timer_readFreq(timer_id)/1000);
////			cnt_remainder = cnt_remainder / (timer_readPeriodUs(timer_id));
//			cnt_remainder = cnt_remainder / (timer_readPeriodMs(timer_id));
//			cnt_quotient  = cnt_quotient;
////			printf("rem: %lu, qu: %lu", cnt_remainder, cnt_quotient);
//			state = timerSetup; //timerSetup;
//			break;
//
//		case active:
//			if (xSemaphoreTake(intr2ssTimer2_sh ,pdMS_TO_TICKS(50)) == 1){
////			if (xSemaphoreTake(intr2ssTimer2_sh ,pdMS_TO_TICKS(1000)) == 1){
//				if (cnt_quotient>0){
//					//still needs running
//					state = timerSetup;
//					cnt_quotient = cnt_quotient - 1;
//				}
//				else{
//					xSemaphoreGive(p_data->callingTaskSem);
//					state = timerReset; //and then sleep
//				}
//			}
//			else{
//				state = checkAs;
//			}
//			break;
//
//		case checkAs:
//			if(xQueueReceive(ssTimer2_qh, &(p_data), 0) == 1){
//				state = rcvdCmd;
//			}
//			else{
//				state = active;
//			}
//			break;
//
//		case fr_active:
//			//check inter for OF recording
//			if (xSemaphoreTake(intr2ssTimer2_sh ,pdMS_TO_TICKS(100)) == 1){
//				// record overflowed happened
//				cnt_quotient = cnt_quotient + 1;
//				timer_stopReset(timer_id);
//				timer_count(timer_id, timer_readMax(timer_id));
//			}
//			// check command recieved or not
////			if(xQueueReceive(ssTimer2_qh, &(p_data), pdMS_TO_TICKS(100)) == 1){
//			if(xQueueReceive(ssTimer2_qh, &(p_data), 0) == 1){
//				state = rcvdCmd;
//			}
//			break;
//
//		case rcvdCmd:
//			if     (p_data->cmd == params.sys_srvc.timer.cmd.sleep){	//go to sleep
//				state = timerReset;
//				xSemaphoreGive(p_data->callingTaskSem); //free sem here
//			}
//			else if (p_data->cmd == params.sys_srvc.timer.cmd.start_freerunning){					//start freerunning
//				timer_stopReset(timer_id);
//				timer_count(timer_id, timer_readMax(timer_id));
//				cnt_quotient = 0; 						//for storing number of times it is overflowed :)
//				//give back sem
//				xSemaphoreGive(p_data->callingTaskSem); //free sem here
//				//go to fr-waiting (check cmd here)
//				state = fr_active;
//			}
//			else if (p_data->cmd == params.sys_srvc.timer.cmd.read_freerunning){					//read but still continue freerunning
//				//change time to MS
////				time_ms = timer_readMs(timer_id);
////				time_us_uint = timer_readUs(timer_id);
//				time_ms_uint = timer_readMs(timer_id);
//				if (cnt_quotient !=0 ){
////					time_ms = time_ms + cnt_quotient * timer_readMaxMs(timer_id);
////					time_us_uint = time_us_uint + cnt_quotient * timer_readMaxUs(timer_id);
//					time_ms_uint = time_ms_uint + cnt_quotient * timer_readMaxMs(timer_id);
//				}
//				//put time in data struct
////				p_data->time = time_ms;
////				time_ms_float = (float)(time_us_uint / 1000);
////				p_data->time = time_us_uint;
//				p_data->time = time_ms_uint;
//				//give back sem
//				xSemaphoreGive(p_data->callingTaskSem); //free sem here
//				//go to waiting
//				state = fr_active;
//
//			}
//			else if (p_data->cmd == params.sys_srvc.timer.cmd.finish_freerunning){					//finish freerunning
//				//change time counter to us
////				time_ms = timer_readMs(timer_id);
////				time_us_uint = timer_readUs(timer_id);
//				time_ms_uint = timer_readMs(timer_id);
//				if (cnt_quotient !=0 ){
////					time_ms = time_ms + cnt_quotient * timer_readMaxMs(timer_id);
////					time_us_uint = time_us_uint + cnt_quotient * timer_readMaxUs(timer_id);
//					time_ms_uint = time_ms_uint + cnt_quotient * timer_readMaxMs(timer_id);
//				}
//				//put time in data struct
////				p_data->time = time_ms;
////				time_ms_float = (float)(time_us_uint / 1000);
////				p_data->time = time_us_uint;
//				p_data->time = time_ms_uint;
//				timer_stopReset(timer_id);
//				//give back semaphore
//				xSemaphoreGive(p_data->callingTaskSem); //free sem here
//				//go to sleep
//				state = sleep;
//			}
//			else { 										//activate intr-based timer
//				state = timerValue;
//				//sem  will be freed after time passed
//			}
//			break;
//		}
//	}
//}
//
//
//void ssTimer3_t( void *pvParameters ){
//	//todo: supporting multi-timer
//	//todo: remove states related to setup, and use functions (which are timer driver functions)
//
//	enum states {
//		sleep,
//		timerSetup,
//		active,
//		checkAs,
//		rcvdCmd,
//		timerReset,
//		timerValue,
//		fr_active	//fr waiting
//	};
//	enum states state;
//
//	ssTimer_d * p_data;
//	unsigned int cnt_remainder, cnt_quotient;
//	state = sleep;
//	unsigned int count_val = 0;
//
//	unsigned int time_us_uint = 0;
////	float time_ms_float = 0;
//	int timer_id = 3;
//
//
//
//	while(1){
//		switch(state){
//		case sleep:
//			if(xQueueReceive(ssTimer3_qh, &(p_data), portMAX_DELAY) == 1){
//				xSemaphoreTake(intr2ssTimer3_sh ,0); //reset semaphore
//				timer_stopReset(timer_id); //reset timer
//				state = rcvdCmd;
//			}
//			break;
//
//		case timerReset:
////			timer_rst(timer_id);
//			timer_stopReset(timer_id);
//			state = sleep;
//			break;
//
//		case timerSetup:
//			//calculate timer amount
//			//below if required for handling large amounts :)
//			if (cnt_quotient>0){
//				count_val = timer_readMax(timer_id);
//			}
//			else{
//				count_val = cnt_remainder;
//			}
//			timer_stopReset(timer_id);
//			timer_count(timer_id, count_val);
//			state = active;
//			break;
//
//		case timerValue:
//			//todo: change this part based on us
////			count_val = p_data->time;
//
////			count_val = (unsigned int)(p_data->time * 1000);
//			count_val = p_data->time; //in us
////			cnt_remainder = count_val % timer_readMaxMs(timer_id);
//			cnt_remainder = count_val % timer_readMaxUs(timer_id);
////			cnt_quotient  = count_val / timer_readMaxMs(timer_id);
//			cnt_quotient  = count_val / timer_readMaxUs(timer_id);
////			cnt_remainder = cnt_remainder * (timer_readFreq(timer_id)/1000);
//			cnt_remainder = cnt_remainder / (timer_readPeriodUs(timer_id));
//			cnt_quotient  = cnt_quotient;
//			state = timerSetup; //timerSetup;
//			break;
//
//		case active:
//			if (xSemaphoreTake(intr2ssTimer3_sh ,pdMS_TO_TICKS(50)) == 1){
////			if (xSemaphoreTake(intr2ssTimer3_sh ,pdMS_TO_TICKS(1000)) == 1){
//				if (cnt_quotient>0){
//					//still needs running
//					state = timerSetup;
//					cnt_quotient = cnt_quotient - 1;
//				}
//				else{
//					xSemaphoreGive(p_data->callingTaskSem);
//					state = timerReset; //and then sleep
//				}
//			}
//			else{
//				state = checkAs;
//			}
//			break;
//
//		case checkAs:
//			if(xQueueReceive(ssTimer3_qh, &(p_data), 0) == 1){
//				state = rcvdCmd;
//			}
//			else{
//				state = active;
//			}
//			break;
//
//		case fr_active:
//			//check inter for OF recording
//			if (xSemaphoreTake(intr2ssTimer3_sh ,pdMS_TO_TICKS(100)) == 1){
//				// record overflowed happened
//				cnt_quotient = cnt_quotient + 1;
//				timer_stopReset(timer_id);
//				timer_count(timer_id, timer_readMax(timer_id));
//			}
//			// check command recieved or not
////			if(xQueueReceive(ssTimer3_qh, &(p_data), pdMS_TO_TICKS(100)) == 1){
//			if(xQueueReceive(ssTimer3_qh, &(p_data), 0) == 1){
//				state = rcvdCmd;
//			}
//			break;
//
//		case rcvdCmd:
//			if     (p_data->cmd == params.sys_srvc.timer.cmd.sleep){	//go to sleep
//				state = timerReset;
//				xSemaphoreGive(p_data->callingTaskSem); //free sem here
//			}
//			else if (p_data->cmd == params.sys_srvc.timer.cmd.start_freerunning){					//start freerunning
//				timer_stopReset(timer_id);
//				timer_count(timer_id, timer_readMax(timer_id));
//				cnt_quotient = 0; 						//for storing number of times it is overflowed :)
//				//give back sem
//				xSemaphoreGive(p_data->callingTaskSem); //free sem here
//				//go to fr-waiting (check cmd here)
//				state = fr_active;
//			}
//			else if (p_data->cmd == params.sys_srvc.timer.cmd.read_freerunning){					//read but still continue freerunning
//				//change time to MS
////				time_ms = timer_readMs(timer_id);
//				time_us_uint = timer_readUs(timer_id);
//				if (cnt_quotient !=0 ){
////					time_ms = time_ms + cnt_quotient * timer_readMaxMs(timer_id);
//					time_us_uint = time_us_uint + cnt_quotient * timer_readMaxUs(timer_id);
//				}
//				//put time in data struct
////				p_data->time = time_ms;
////				time_ms_float = (float)(time_us_uint / 1000);
//				p_data->time = time_us_uint;
//				//give back sem
//				xSemaphoreGive(p_data->callingTaskSem); //free sem here
//				//go to waiting
//				state = fr_active;
//
//			}
//			else if (p_data->cmd == params.sys_srvc.timer.cmd.finish_freerunning){					//finish freerunning
//				//change time counter to us
////				time_ms = timer_readMs(timer_id);
//				time_us_uint = timer_readUs(timer_id);
//				if (cnt_quotient !=0 ){
////					time_ms = time_ms + cnt_quotient * timer_readMaxMs(timer_id);
//					time_us_uint = time_us_uint + cnt_quotient * timer_readMaxUs(timer_id);
//				}
//				//put time in data struct
////				p_data->time = time_ms;
////				time_ms_float = (float)(time_us_uint / 1000);
//				p_data->time = time_us_uint;
//				timer_stopReset(timer_id);
//				//give back semaphore
//				xSemaphoreGive(p_data->callingTaskSem); //free sem here
//				//go to sleep
//				state = sleep;
//			}
//			else { 										//activate intr-based timer
//				state = timerValue;
//				//sem  will be freed after time passed
//			}
//			break;
//		}
//	}
//}
//
//void ssTimer4_t( void *pvParameters ){
//	//todo: supporting multi-timer
//	//todo: remove states related to setup, and use functions (which are timer driver functions)
//
//	enum states {
//		sleep,
//		timerSetup,
//		active,
//		checkAs,
//		rcvdCmd,
//		timerReset,
//		timerValue,
//		fr_active	//fr waiting
//	};
//	enum states state;
//
//	ssTimer_d * p_data;
//	unsigned int cnt_remainder, cnt_quotient;
//	state = sleep;
//	unsigned int count_val = 0;
//
//	unsigned int time_us_uint = 0;
////	float time_ms_float = 0;
//	int timer_id = 4;
//
//
//
//	while(1){
//		switch(state){
//		case sleep:
//			if(xQueueReceive(ssTimer4_qh, &(p_data), portMAX_DELAY) == 1){
//				xSemaphoreTake(intr2ssTimer4_sh ,0); //reset semaphore
//				timer_stopReset(timer_id); //reset timer
//				state = rcvdCmd;
//			}
//			break;
//
//		case timerReset:
////			timer_rst(timer_id);
//			timer_stopReset(timer_id);
//			state = sleep;
//			break;
//
//		case timerSetup:
//			//calculate timer amount
//			//below if required for handling large amounts :)
//			if (cnt_quotient>0){
//				count_val = timer_readMax(timer_id);
//			}
//			else{
//				count_val = cnt_remainder;
//			}
//			timer_stopReset(timer_id);
//			timer_count(timer_id, count_val);
//			state = active;
//			break;
//
//		case timerValue:
//			//todo: change this part based on us
////			count_val = p_data->time;
//
////			count_val = (unsigned int)(p_data->time * 1000);
//			count_val = p_data->time; //in us
////			cnt_remainder = count_val % timer_readMaxMs(timer_id);
//			cnt_remainder = count_val % timer_readMaxUs(timer_id);
////			cnt_quotient  = count_val / timer_readMaxMs(timer_id);
//			cnt_quotient  = count_val / timer_readMaxUs(timer_id);
////			cnt_remainder = cnt_remainder * (timer_readFreq(timer_id)/1000);
//			cnt_remainder = cnt_remainder / (timer_readPeriodUs(timer_id));
//			cnt_quotient  = cnt_quotient;
//			state = timerSetup; //timerSetup;
//			break;
//
//		case active:
//			if (xSemaphoreTake(intr2ssTimer4_sh ,pdMS_TO_TICKS(50)) == 1){
////			if (xSemaphoreTake(intr2ssTimer4_sh ,pdMS_TO_TICKS(1000)) == 1){
//				if (cnt_quotient>0){
//					//still needs running
//					state = timerSetup;
//					cnt_quotient = cnt_quotient - 1;
//				}
//				else{
//					xSemaphoreGive(p_data->callingTaskSem);
//					state = timerReset; //and then sleep
//				}
//			}
//			else{
//				state = checkAs;
//			}
//			break;
//
//		case checkAs:
//			if(xQueueReceive(ssTimer4_qh, &(p_data), 0) == 1){
//				state = rcvdCmd;
//			}
//			else{
//				state = active;
//			}
//			break;
//
//		case fr_active:
//			//check inter for OF recording
//			if (xSemaphoreTake(intr2ssTimer4_sh ,pdMS_TO_TICKS(100)) == 1){  //this is 100ms :)
//				// record overflowed happened
//				cnt_quotient = cnt_quotient + 1;
//				timer_stopReset(timer_id);
//				timer_count(timer_id, timer_readMax(timer_id));
//			}
//			// check command recieved or not
////			if(xQueueReceive(ssTimer4_qh, &(p_data), pdMS_TO_TICKS(100)) == 1){
//			if(xQueueReceive(ssTimer4_qh, &(p_data), 0) == 1){
//				state = rcvdCmd;
//			}
//			break;
//
//		case rcvdCmd:
//			if     (p_data->cmd == params.sys_srvc.timer.cmd.sleep){	//go to sleep
//				state = timerReset;
//				xSemaphoreGive(p_data->callingTaskSem); //free sem here
//			}
//			else if (p_data->cmd == params.sys_srvc.timer.cmd.start_freerunning){					//start freerunning
//				timer_stopReset(timer_id);
//				timer_count(timer_id, timer_readMax(timer_id));
//				cnt_quotient = 0; 						//for storing number of times it is overflowed :)
//				//give back sem
//				xSemaphoreGive(p_data->callingTaskSem); //free sem here
//				//go to fr-waiting (check cmd here)
//				state = fr_active;
//			}
//			else if (p_data->cmd == params.sys_srvc.timer.cmd.read_freerunning){					//read but still continue freerunning
//				//change time to MS
////				time_ms = timer_readMs(timer_id);
//				time_us_uint = timer_readUs(timer_id);
//				if (cnt_quotient !=0 ){
////					time_ms = time_ms + cnt_quotient * timer_readMaxMs(timer_id);
//					time_us_uint = time_us_uint + cnt_quotient * timer_readMaxUs(timer_id);
//				}
//				//put time in data struct
////				p_data->time = time_ms;
////				time_ms_float = (float)(time_us_uint / 1000);
//				p_data->time = time_us_uint;
//				//give back sem
//				xSemaphoreGive(p_data->callingTaskSem); //free sem here
//				//go to waiting
//				state = fr_active;
//
//			}
//			else if (p_data->cmd == params.sys_srvc.timer.cmd.finish_freerunning){					//finish freerunning
//				//change time counter to us
////				time_ms = timer_readMs(timer_id);
//				time_us_uint = timer_readUs(timer_id);
//				if (cnt_quotient !=0 ){
////					time_ms = time_ms + cnt_quotient * timer_readMaxMs(timer_id);
//					time_us_uint = time_us_uint + cnt_quotient * timer_readMaxUs(timer_id);
//				}
//				//put time in data struct
////				p_data->time = time_ms;
////				time_ms_float = (float)(time_us_uint / 1000);
//				p_data->time = time_us_uint;
//				timer_stopReset(timer_id);
//				//give back semaphore
//				xSemaphoreGive(p_data->callingTaskSem); //free sem here
//				//go to sleep
//				state = sleep;
//			}
//			else { 										//activate intr-based timer
//				state = timerValue;
//				//sem  will be freed after time passed
//			}
//			break;
//		}
//	}
//}
//
//void ssTimer5_t( void *pvParameters ){
//	//todo: supporting multi-timer
//	//todo: remove states related to setup, and use functions (which are timer driver functions)
//
//	enum states {
//		sleep,
//		timerSetup,
//		active,
//		checkAs,
//		rcvdCmd,
//		timerReset,
//		timerValue,
//		fr_active	//fr waiting
//	};
//	enum states state;
//
//	ssTimer_d * p_data;
//	unsigned int cnt_remainder, cnt_quotient;
//	state = sleep;
//	unsigned int count_val = 0;
//
//	unsigned int time_us_uint = 0;
////	float time_ms_float = 0;
//	int timer_id = 5;
//
//
//
//	while(1){
//		switch(state){
//		case sleep:
//			if(xQueueReceive(ssTimer5_qh, &(p_data), portMAX_DELAY) == 1){
//				xSemaphoreTake(intr2ssTimer5_sh ,0); //reset semaphore
//				timer_stopReset(timer_id); //reset timer
//				state = rcvdCmd;
//			}
//			break;
//
//		case timerReset:
////			timer_rst(timer_id);
//			timer_stopReset(timer_id);
//			state = sleep;
//			break;
//
//		case timerSetup:
//			//calculate timer amount
//			//below if required for handling large amounts :)
//			if (cnt_quotient>0){
//				count_val = timer_readMax(timer_id);
//			}
//			else{
//				count_val = cnt_remainder;
//			}
//			timer_stopReset(timer_id);
//			timer_count(timer_id, count_val);
//			state = active;
//			break;
//
//		case timerValue:
//			//todo: change this part based on us
////			count_val = p_data->time;
//
////			count_val = (unsigned int)(p_data->time * 1000);
//			count_val = p_data->time; //in us
////			cnt_remainder = count_val % timer_readMaxMs(timer_id);
//			cnt_remainder = count_val % timer_readMaxUs(timer_id);
////			cnt_quotient  = count_val / timer_readMaxMs(timer_id);
//			cnt_quotient  = count_val / timer_readMaxUs(timer_id);
////			cnt_remainder = cnt_remainder * (timer_readFreq(timer_id)/1000);
//			cnt_remainder = cnt_remainder / (timer_readPeriodUs(timer_id));
//			cnt_quotient  = cnt_quotient;
//			state = timerSetup; //timerSetup;
//			break;
//
//		case active:
//			if (xSemaphoreTake(intr2ssTimer5_sh ,pdMS_TO_TICKS(50)) == 1){
////			if (xSemaphoreTake(intr2ssTimer5_sh ,pdMS_TO_TICKS(1000)) == 1){
//				if (cnt_quotient>0){
//					//still needs running
//					state = timerSetup;
//					cnt_quotient = cnt_quotient - 1;
//				}
//				else{
//					xSemaphoreGive(p_data->callingTaskSem);
//					state = timerReset; //and then sleep
//				}
//			}
//			else{
//				state = checkAs;
//			}
//			break;
//
//		case checkAs:
//			if(xQueueReceive(ssTimer5_qh, &(p_data), 0) == 1){
//				state = rcvdCmd;
//			}
//			else{
//				state = active;
//			}
//			break;
//
//		case fr_active:
//			//check inter for OF recording
//			if (xSemaphoreTake(intr2ssTimer5_sh ,pdMS_TO_TICKS(100)) == 1){
//				// record overflowed happened
//				cnt_quotient = cnt_quotient + 1;
//				timer_stopReset(timer_id);
//				timer_count(timer_id, timer_readMax(timer_id));
//			}
//			// check command recieved or not
////			if(xQueueReceive(ssTimer5_qh, &(p_data), pdMS_TO_TICKS(100)) == 1){
//			if(xQueueReceive(ssTimer5_qh, &(p_data), 0) == 1){
//				state = rcvdCmd;
//			}
//			break;
//
//		case rcvdCmd:
//			if     (p_data->cmd == params.sys_srvc.timer.cmd.sleep){	//go to sleep
//				state = timerReset;
//				xSemaphoreGive(p_data->callingTaskSem); //free sem here
//			}
//			else if (p_data->cmd == params.sys_srvc.timer.cmd.start_freerunning){					//start freerunning
//				timer_stopReset(timer_id);
//				timer_count(timer_id, timer_readMax(timer_id));
//				cnt_quotient = 0; 						//for storing number of times it is overflowed :)
//				//give back sem
//				xSemaphoreGive(p_data->callingTaskSem); //free sem here
//				//go to fr-waiting (check cmd here)
//				state = fr_active;
//			}
//			else if (p_data->cmd == params.sys_srvc.timer.cmd.read_freerunning){					//read but still continue freerunning
//				//change time to MS
////				time_ms = timer_readMs(timer_id);
//				time_us_uint = timer_readUs(timer_id);
//				if (cnt_quotient !=0 ){
////					time_ms = time_ms + cnt_quotient * timer_readMaxMs(timer_id);
//					time_us_uint = time_us_uint + cnt_quotient * timer_readMaxUs(timer_id);
//				}
//				//put time in data struct
////				p_data->time = time_ms;
////				time_ms_float = (float)(time_us_uint / 1000);
//				p_data->time = time_us_uint;
//				//give back sem
//				xSemaphoreGive(p_data->callingTaskSem); //free sem here
//				//go to waiting
//				state = fr_active;
//
//			}
//			else if (p_data->cmd == params.sys_srvc.timer.cmd.finish_freerunning){					//finish freerunning
//				//change time counter to us
////				time_ms = timer_readMs(timer_id);
//				time_us_uint = timer_readUs(timer_id);
//				if (cnt_quotient !=0 ){
////					time_ms = time_ms + cnt_quotient * timer_readMaxMs(timer_id);
//					time_us_uint = time_us_uint + cnt_quotient * timer_readMaxUs(timer_id);
//				}
//				//put time in data struct
////				p_data->time = time_ms;
////				time_ms_float = (float)(time_us_uint / 1000);
//				p_data->time = time_us_uint;
//				timer_stopReset(timer_id);
//				//give back semaphore
//				xSemaphoreGive(p_data->callingTaskSem); //free sem here
//				//go to sleep
//				state = sleep;
//			}
//			else { 										//activate intr-based timer
//				state = timerValue;
//				//sem  will be freed after time passed
//			}
//			break;
//		}
//	}
//}
//
//void ssHMI_t( void *pvParameters ){
//
//	enum ssHMIStates{
//		sleep,
//		reset,
//		active,
//		rcvdCmd,
//		checkAs
//	};
//	enum ssHMIStates state;
//	state = sleep; //sleep;
//
//	uint32_t buttons_situation = 0;
//	ssHMI_d * p_data;
//
//	char uartChar;
//	char pressed_button;	//b[0] corto rising, b[1] corto falling, b[2] lungo rising, b[3] lungo falling
////	pre_stable_state = sleep; //its purpose: if it recieved not defined cmd, come back to active or sleep
//
//	//using for recording pressing time
//	unsigned int time_corto=0, time_lungo=0, time_nsprs=0, time_hw=0,
//			time_nsprs_temp, time_hw_temp, time_corto_temp, time_lungo_temp;
//	SemaphoreHandle_t ssHMISem; //this is sem is used for interacting with ssTimer
//	ssHMISem = xSemaphoreCreateBinary();
//	configASSERT(ssHMISem);
//	ssTimer_d ssTimerdata;		//todo: can make this global?! no bc at same time maybe some
//	ssTimer_d * const p_ssTimerdata = &ssTimerdata;
//	//cmd and sem always same
//	p_ssTimerdata->callingTaskSem = ssHMISem;
//	p_ssTimerdata->cmd = params.sys_srvc.timer.cmd.read_freerunning;
//
//	while(1){
//		switch(state){
//		case sleep:
//			buttons_situation = 0x00;
//			if(xQueueReceive(ssHMI_qh, &(p_data), portMAX_DELAY) == 1){
//				state = rcvdCmd;
//			}
//			break;
//
//		case reset:
//			buttons_situation = 0x00;
//			time_corto = 0;
//			time_lungo = 0;
//			time_hw = 0;
//			time_nsprs = 0;
//
//			state = active;
//			break;
//
//
//
//		case active:
//			//check recieved button
//			if (xQueueReceive(intr2ssHMI_qh, &(pressed_button), pdMS_TO_TICKS(100)) == 1){
//
//				//espresso
//				if ((pressed_button & 1<<ESPR_PRESSED) != 0){
//					buttons_situation = buttons_situation | (1<<ESPR_PRESSED);
//					buttons_situation = buttons_situation & ~(1<<ESPR_RELEASED);
//					//start recording time
//					xQueueSend(ssTimer2_qh, &p_ssTimerdata, portMAX_DELAY);
//					xSemaphoreTake(p_ssTimerdata->callingTaskSem ,portMAX_DELAY);
//					time_corto_temp = p_ssTimerdata->time;
//				}
//				else if ((pressed_button & 1<<ESPR_RELEASED) != 0){
//					buttons_situation = buttons_situation & ~(1<<ESPR_PRESSED);
//					buttons_situation = buttons_situation | (1<<ESPR_RELEASED);
//					//end recording time
//					xQueueSend(ssTimer2_qh, &p_ssTimerdata, portMAX_DELAY);
//					xSemaphoreTake(p_ssTimerdata->callingTaskSem ,portMAX_DELAY);
//					time_corto = (p_ssTimerdata->time) - time_corto_temp;
//				}
//				//lungo
//				if ((pressed_button & 1<<LUNGO_PRESSED) != 0){
//					buttons_situation = buttons_situation | (1<<LUNGO_PRESSED);
//					buttons_situation = buttons_situation & ~(1<<LUNGO_RELEASED);
//					//start recording time
//					xQueueSend(ssTimer2_qh, &p_ssTimerdata, portMAX_DELAY);
//					xSemaphoreTake(p_ssTimerdata->callingTaskSem ,portMAX_DELAY);
//					time_lungo_temp = p_ssTimerdata->time;
//				}
//				else if ((pressed_button & 1<<LUNGO_RELEASED) != 0){
//
//					buttons_situation = buttons_situation & ~(1<<LUNGO_PRESSED);
//					buttons_situation = buttons_situation | (1<<LUNGO_RELEASED);
//					//end recording time
//					xQueueSend(ssTimer2_qh, &p_ssTimerdata, portMAX_DELAY);
//					xSemaphoreTake(p_ssTimerdata->callingTaskSem ,portMAX_DELAY);
//					time_lungo = (p_ssTimerdata->time) - time_lungo_temp;
//				}
//				//hw
//				if ((pressed_button & 1<<HW_PRESSED) != 0){
//					buttons_situation = buttons_situation | (1<<HW_PRESSED);
//					buttons_situation = buttons_situation & ~(1<<HW_RELEASED);
//					//start recording time
//					xQueueSend(ssTimer2_qh, &p_ssTimerdata, portMAX_DELAY);
//					xSemaphoreTake(p_ssTimerdata->callingTaskSem ,portMAX_DELAY);
//					time_hw_temp = p_ssTimerdata->time;
//				}
//				else if ((pressed_button & 1<<HW_RELEASED) != 0){
//					buttons_situation = buttons_situation & ~(1<<HW_PRESSED);
//					buttons_situation = buttons_situation | (1<<HW_RELEASED);
//					//end recording time
//					xQueueSend(ssTimer2_qh, &p_ssTimerdata, portMAX_DELAY);
//					xSemaphoreTake(p_ssTimerdata->callingTaskSem ,portMAX_DELAY);
//					time_hw = (p_ssTimerdata->time) - time_lungo_temp;
//				}
//				//nespresso
//				if ((pressed_button & 1<<NSPRS_PRESSED) != 0){
//					buttons_situation = buttons_situation | (1<<NSPRS_PRESSED);
//					buttons_situation = buttons_situation & ~(1<<NSPRS_RELEASED);
//					//start recording time
//					xQueueSend(ssTimer2_qh, &p_ssTimerdata, portMAX_DELAY);
//					xSemaphoreTake(p_ssTimerdata->callingTaskSem ,portMAX_DELAY);
//					time_nsprs_temp = p_ssTimerdata->time;
//				}
//				else if ((pressed_button & 1<<NSPRS_RELEASED) != 0){
//					//lungo falling
//					//010 shows released
//					buttons_situation = buttons_situation & ~(1<<NSPRS_PRESSED);
//					buttons_situation = buttons_situation | (1<<NSPRS_RELEASED);
//					//end recording time
//					xQueueSend(ssTimer2_qh, &p_ssTimerdata, portMAX_DELAY);
//					xSemaphoreTake(p_ssTimerdata->callingTaskSem ,portMAX_DELAY);
//					time_nsprs = (p_ssTimerdata->time) - time_lungo_temp;
//				}
//
//			}
//			//check receiving char from uart
//			if (xQueueReceive(intr2ssUart_qh, &(uartChar), pdMS_TO_TICKS(100)) == 1){
//				if (uartChar == 'c'){
////					xil_printf("corto: %c\r\n", uartChar);
//					buttons_situation = buttons_situation | (1<<10);
//				}
//				else if (uartChar == 'l'){
////					xil_printf("lungo: %c\r\n", uartChar);
//					buttons_situation = buttons_situation | (1<<11);
//				}
//				else{
////					xil_printf("not-defined: %c\r\n", uartChar);
//					//not a valid command
//				}
//			}
//			state = checkAs;
//			break;
//
//		case checkAs:
//			if(xQueueReceive(ssHMI_qh, &(p_data), 0) == 1){
//				state = rcvdCmd;
//			}
//			else{
//				state = active;
//			}
//			break;
//
//		case rcvdCmd:
//			if     (p_data->cmd == params.sys_srvc.hmi.cmd.sleep){	//go to sleep
//				state = sleep;
//			}
//			else if(p_data->cmd == params.sys_srvc.hmi.cmd.activate){ //go to active state
//				state = active;
//			}
//			else if(p_data->cmd == params.sys_srvc.hmi.cmd.read){ //read data
////				p_data->buttons_situation = buttons_situation;
////				p_data->time_corto_ms = time_corto;
////				p_data->time_lungo_ms = time_lungo;
////				p_data->time_hw_ms = time_hw;
////				p_data->time_nsprs_ms = time_nsprs;
//				put_hmi_infor(p_data, buttons_situation, time_corto, time_lungo, time_hw, time_nsprs);
//				state = active;
//			}
//			else if(p_data->cmd == params.sys_srvc.hmi.cmd.reset){ //reset it
//				state = reset;
//			}
//			else{	//not a valid cmd
//				p_data->buttons_situation = 0x00;
//				state = active;
//			}
//			//give semaphor back
//			xSemaphoreGive(p_data->callingTaskSem);
//			break;
//
//		}
//	}
//
//}
//
//static void put_hmi_infor(ssHMI_d * p_data, uint32_t buttons_situation,
//		unsigned int time_corto, unsigned int time_lungo, unsigned int time_hw, unsigned int time_nsprs){
//	//check brew corto
//	if ((buttons_situation & (1<<ESPR_RELEASED)) != 0 ){
//		p_data->req_brew_espr = 1;
//	}
//	else{
//		p_data->req_brew_espr = 0;
//	}
//	//check brew lungo
//	if ((buttons_situation & (1<<LUNGO_RELEASED)) != 0 ){
//		p_data->req_brew_lungo = 1;
//	}
//	else{
//		p_data->req_brew_lungo = 0;
//	}
//	//check hw
//	if ((buttons_situation & (1<<HW_RELEASED)) != 0 ){
//		p_data->req_hw = 1;
//	}
//	else{
//		p_data->req_hw = 0;
//	}
//
//}
//
//void ssFlow_t(void *pvParameters){
//	ssFlow_d * p_data;
//	enum ssFlowStates{
//			sleep,
//			init,
//			active,
//			rcvdCmd,
//			checkAs
//		};
//	enum ssFlowStates state, pre_state;
//	state = sleep;//sleep;
//	pre_state = sleep;
////	uint32_t time;
//	int flow_rate = 0, flow_rate2 = 0, total_vol = 0;
//	char flow_sig;
//
//	unsigned int time, time_ms1, time_ms2;
//	SemaphoreHandle_t ssFlowSem; //this is sem is used for interacting with ssTimer
//	ssFlowSem = xSemaphoreCreateBinary();
//	configASSERT(ssFlowSem);
//	ssTimer_d ssTimerdata;
//	ssTimer_d * const p_ssTimerdata = &ssTimerdata;
//	//cmd and sem always same
//	p_ssTimerdata->callingTaskSem = ssFlowSem;
//	p_ssTimerdata->cmd = params.sys_srvc.timer.cmd.read_freerunning;
//
//	while(1){
//		switch(state){
//		case sleep:
//			pre_state = sleep;
//			ssFlow_resetValues(&flow_rate, &total_vol);
//			global_vars.flow = 0;
//			global_vars.volume_passed = 0;
////			ssFlow_stopTimer();
//			if(xQueueReceive(ssFlow_qh, &(p_data), portMAX_DELAY) == 1){
//				state = rcvdCmd;
//			}
//			break;
//
//		case init:
//			pre_state = init;
//			//do all the initializations then go to active
//			ssFlow_resetValues(&flow_rate, &total_vol); //rst flow rate and flow passed to zero
//			xQueueReceive(intr2ssFlow_qh, &(flow_sig), 0); //rst it
//
//			if (xQueueReceive(intr2ssFlow_qh, &(flow_sig), pdMS_TO_TICKS(200)) == 1){
////				ssFlow_startTimer();
//				xQueueSend(ssTimer2_qh, &p_ssTimerdata, portMAX_DELAY);
//				xSemaphoreTake(p_ssTimerdata->callingTaskSem ,portMAX_DELAY);
//				time_ms1 = p_ssTimerdata->time;
//				state = active;
//			}
//			else{
//				state = checkAs;
//			}
//
//			break;
//
//		case active:
//			pre_state = active;
//			flow_sig = 0;
//			if (xQueueReceive(intr2ssFlow_qh, &(flow_sig), pdMS_TO_TICKS(200)) == 1){ //todo: change to another form
//				if(flow_sig == 1){
////					time = (*TIM1CR)*(1000000.0/TIM1FREQ);
//					xQueueSend(ssTimer2_qh, &p_ssTimerdata, portMAX_DELAY);
//					xSemaphoreTake(p_ssTimerdata->callingTaskSem ,portMAX_DELAY);
//					time_ms2 = p_ssTimerdata->time;
//					time = time_ms2-time_ms1;
//
//					time_ms1 = time_ms2;
//
//					flow_rate = flow_rate2;		//put a register for flow
//					flow_rate2 = ssFlow_time2flow(time*1000);
//					total_vol = total_vol + 1;//total_vol + flow_rate; //todo: is this correct way?! or just add number of ticks?! it seems number of ticks is important!
//
//					global_vars.flow = flow_rate;
//					global_vars.volume_passed = total_vol;
//
////					printf("vol_passed: %d\r\n", global_vars.volume_passed);
//
//				}
//				else if(flow_sig == 0){
//				}
//			}
//
//			state = checkAs;
//			break;
//
//		case checkAs:
//			if(xQueueReceive(ssFlow_qh, &(p_data), 0) == 1){
//				state = rcvdCmd;
//			}
//			else{
//				state = pre_state;
//			}
//			break;
//
//		case rcvdCmd:
//			if (p_data->cmd == 	params.sys_srvc.flow.cmd.sleep){ //goto sleep
//				state = sleep;
//			}
//			else if (p_data->cmd == params.sys_srvc.flow.cmd.rst_activate){ //reset and activate
//				state = init;
////				printf("yes?!\r\n");
//			}
//			else if(p_data->cmd == params.sys_srvc.flow.cmd.read){ //rd value
//				state = active;
//			}
//			p_data->flow_rate = flow_rate;
//			p_data->total_vol = total_vol;
//			xSemaphoreGive(p_data->callingTaskSem);
//			break;
//		}
//	}
//}
//
////======= internal functions
//static void ssFlow_resetValues(int* flow_rate, int* total_vol){
//	/*
//	 * reset flowRate and total passed value
//	 */
//	*total_vol = 0;
//	*flow_rate = 0;
//}
//
//static int ssFlow_time2flow(uint32_t time_us){
//	/*
//	 * the function inverting time to flow!
//	 * large times are handled by timer (if overflowed) to this function just time are passed
//	 */
//	int f;
//	if (time_us < 300000){
//		f = 300;
//	}
//	else{
//		f = 0;
//	}
//	return f;
//}
//
//void ssTemp_t(void *pvParameters){
//	//modes: continous updating, one time measurement, sleep
//	ssTemp_d * p_data;
//	enum ssTempStates{
//		sleep,
//		active,
//		rcvdCmd
//	};
//	enum ssTempStates pre_stable_state, state;
//	state = sleep;
////	pre_stable_state = sleep;
//	int temp_deg = 0;
////	uint32_t updating_time = 150; //updating time in ms
//
//	while(1){
//		vTaskDelay(pdMS_TO_TICKS(ssTemp_periodMs));
//		switch(state){
//
//		case sleep:
//			if(xQueueReceive(ssTemp_qh, &(p_data), portMAX_DELAY) == 1){
//				state = rcvdCmd;
//			}
//			break;
//
//
//		case active:
//			temp_deg = readTemp();
////			temp_deg = 0;		//todo: for debuging, remove in future :)
//			global_vars.temperature = temp_deg;
//			if(xQueueReceive(ssTemp_qh, &(p_data), 0) == 1){
//				state = rcvdCmd;
//			}
//			break;
//
//		case rcvdCmd:
//			if (p_data->cmd == params.sys_srvc.temp.cmd.sleep){ //go to sleep
//				state = sleep;
//			}
//			else if (p_data->cmd == params.sys_srvc.temp.cmd.activate){
//				state = active;
//			}
//			else {
//				state = active;
//			}
//			p_data->updatedTemp = temp_deg;
//			xSemaphoreGive(p_data->callingTaskSem);
//			break;
//		}
//	}
//}
//
////=== pump task ===
////void ssTrgPump_t( void *pvParameters ){
////	enum ssTrgPumpStates {
////		sleep,
////		active,
////		rcvdCmd
////	};
////	enum ssTrgPumpStates state;
////
////	state = sleep; //sleep //todo: change again to sleep
////	ssTrgPump_d * p_data;
////	char enPump = 0;
////	char zc_edge;
////	trg_init_tmr14(); //timer16 trigger initialization
////	trg_init_tmr15(); //todo: used for the valve, change in future when hw was added
////
////	trg_impl_tmr15(TRG_MODE_HIGH, 0, 0, 0, 0); //for the valve
////
////	while(1){
////		switch(state){
////		case sleep:
//////			if(xQueueReceive(ssPump_qh, &(p_data), portMAX_DELAY) == 1){
////			if(xQueueReceive(ssTrgPump_qh, &(zc_edge), pdMS_TO_TICKS(200)) == 1 ){
////
////				state = rcvdCmd;
////			}
////			break;
////
//////		case rcvdCmd:
//////			if (p_data->cmd == params.sys_srvc.trgpump.cmd.sleep){
//////				state = sleep;
//////				printf("11\r\n");
//////			}
//////			printf("here2?\r\n");
////////			else if (p_data->cmd == params.sys_srvc.trgpump.cmd.activate){
//////			if (p_data->cmd == params.sys_srvc.trgpump.cmd.activate){
//////				state = active;
//////				printf("2\r\n");
//////			}
//////			printf("22\r\n");
//////			xSemaphoreGive(p_data->callingTaskSem);
//////			break;
////
////					case rcvdCmd:
////						printf("1");
////						if (p_data->cmd == params.sys_srvc.trgheater.cmd.sleep){
////							state = sleep;
////							printf("2");
////						}
////						else if (p_data->cmd == params.sys_srvc.trgheater.cmd.activate){
////							state = active;
////							printf("3");
////						}
////						xSemaphoreGive(p_data->callingTaskSem);
////						printf("4");
////						break;
////
////		case active:
////			//todo: remove max_time, put a limited time, if not zc signal, report on log
////			printf("3\r\n");
////			if(xQueueReceive(intr2ssTrgPump_qh, &(zc_edge), portMAX_DELAY) == 1 ){
////
////				if(zc_edge==1){
////					//brewing
////					trg_impl_tmr14(TRG_MODE_T1HIGH_LOW, 4000, 0,  0, 0);
////					//HW
//////					trg_impl_tmr14(TRG_MODE_LOW, 0, 0,  0, 0);
////				}
////				else if (zc_edge==0){
////					//brewing and HW
////					trg_impl_tmr14(TRG_MODE_T1LOW_HIGH, 4000, 0,  0, 0);
////				}
////			}
////			else{
////				//log, error, not detecting ZC signal
////			}
////
////			if(xQueueReceive(ssTrgPump_qh, &(p_data), 0) == 1){
////				state = rcvdCmd;
////			}
////			break;
////		}
////	}
////
////}
//
//void ssTrgPump_t( void *pvParameters ){
//	enum ssHeaterStates {
//		sleep,
//		active,
//		rcvdCmd
//	};
//	enum ssHeaterStates state;
//	state = sleep;
//	ssTrgPump_d * p_data;
//
//	char enPump = 0;
//	char zc_edge;
//	trg_init_tmr14(); //timer16 trigger initialization
//	trg_init_tmr15(); //todo: used for the valve, change in future when hw was added
//
//	trg_impl_tmr15(TRG_MODE_HIGH, 0, 0, 0, 0); //for the valve
//
//
//	while(1){
//		switch(state){
//		case sleep:
//			trg_impl_tmr14(TRG_MODE_LOW, 0, 0, 0, 0); //for the pump
//			if(xQueueReceive(ssTrgPump_qh, &(p_data), portMAX_DELAY) == 1){
//				state = rcvdCmd;
//				printf("0");
//			}
//			break;
//
//		case rcvdCmd:
////			printf("1");
//			if (p_data->cmd == params.sys_srvc.trgpump.cmd.sleep){
//				state = sleep;
////				printf("2");
//			}
//			else if (p_data->cmd == params.sys_srvc.trgpump.cmd.activate){
//				state = active;
////				printf("3");
//			}
//			xSemaphoreGive(p_data->callingTaskSem);
////			printf("4");
//			break;
//
//		case active:
////			printf("task_trgheater: active\r\n");
//			//todo: remove max_time, put a limited time, if not zc signal, report on log (pdMS_TO_TICKS(50) )
////			if(xQueueReceive(intr2ssTrgHeater_qh, &(zc_edge), portMAX_DELAY) == 1 ){
//			if(xQueueReceive(intr2ssTrgPump_qh, &(zc_edge), pdMS_TO_TICKS(200)) == 1 ){
//
//				if(zc_edge==1){
//
////					trg_impl_tmr15(TRG_MODE_HIGH, 0, 0, 0, 0); //for the valve
//
//					//brewing
//					trg_impl_tmr14(TRG_MODE_T1HIGH_LOW, 4000, 0,  0, 0);
//					//HW
////					trg_impl_tmr14(TRG_MODE_LOW, 0, 0,  0, 0);
//				}
//				else if (zc_edge==0){
//					//brewing and HW
//					trg_impl_tmr14(TRG_MODE_T1LOW_HIGH, 4000, 0,  0, 0);
//				}
//			}
//			else{
//				//log, error, not detecting ZC signal
//			}
//
//			if(xQueueReceive(ssTrgPump_qh, &(p_data), 0) == 1){
//				state = rcvdCmd;
//			}
//			break;
//
//		}
//	}
//
//}
//
//
//void ssTrgHeater_t( void *pvParameters ){
//	enum ssHeaterStates {
//		sleep,
//		active,
//		rcvdCmd
//	};
//	enum ssHeaterStates state;
//	state = sleep;
//	ssTrgHeater_d * p_data;
//	char enHeater = 0;
//	char zc_edge;
//
//	trg_init_tmr16(); //timer16 trigger initialization
//	trg_init_tmr17(); //timer17 ""
//
//	while(1){
//		switch(state){
//		case sleep:
////			printf("task_trgheater: sleep\r\n");
//			trg_impl_tmr16(TRG_MODE_LOW, 0, 0,  0, 0);
//			trg_impl_tmr17(TRG_MODE_LOW, 0, 0,  0, 0);
//			if(xQueueReceive(ssTrgHeater_qh, &(p_data), portMAX_DELAY) == 1){
//				state = rcvdCmd;
//			}
//			break;
//
//		case rcvdCmd:
//			if (p_data->cmd == params.sys_srvc.trgheater.cmd.sleep){
//				state = sleep;
//			}
//			else if (p_data->cmd == params.sys_srvc.trgheater.cmd.activate){
//				state = active;
//			}
//			xSemaphoreGive(p_data->callingTaskSem);
//			break;
//
//		case active:
////			printf("task_trgheater: active\r\n");
//			//todo: remove max_time, put a limited time, if not zc signal, report on log (pdMS_TO_TICKS(50) )
////			if(xQueueReceive(intr2ssTrgHeater_qh, &(zc_edge), portMAX_DELAY) == 1 ){
//			if(xQueueReceive(intr2ssTrgHeater_qh, &(zc_edge), pdMS_TO_TICKS(200)) == 1 ){
//
//				if(zc_edge==1){
//					trg_impl_tmr16(TRG_MODE_T1HIGH_LOW, 2000, 0,  0, 0);
////					trg_impl_tmr17(TRG_MODE_T1HIGH_LOW, 2000, 0,  0, 0);
//					trg_impl_tmr17(TRG_MODE_T1PWM_T2LOW_HIGH, 2000, 2000,  50, 25); //todo: correct it
//				}
//				else if (zc_edge==0){
//					trg_impl_tmr16(TRG_MODE_T1HIGH_LOW, 2000, 0,  0, 0);
////					trg_impl_tmr17(TRG_MODE_T1HIGH_LOW, 2000, 0,  0, 0);
//					trg_impl_tmr17(TRG_MODE_T1PWM_T2LOW_HIGH, 2000, 2000,  50, 25);
//				}
//			}
//			else{
//				//log, error, not detecting ZC signal
//			}
//
//			if(xQueueReceive(ssTrgHeater_qh, &(p_data), 0) == 1){
//				state = rcvdCmd;
//			}
//			break;
//		}
//	}
//
//}
//
//void ssHeatAlg_t( void *pvParameters ){
//
//	heatalg_task_state = sleep;
//	ssHeatAlg_d * p_data;
//
//	SemaphoreHandle_t ssHeatAlgSem;
//	ssHeatAlgSem = xSemaphoreCreateBinary();
//
//	while(1){
//
//		vTaskDelay(pdMS_TO_TICKS(ssHeatAlg_periodMs)); //running periodically
//
//		switch(heatalg_task_state){
//		case sleep:
////			printf("task_heatingalg: sleep\r\n");
//			if(xQueueReceive(ssHeatAlg_qh, &(p_data), portMAX_DELAY) == 1){
//				ssHeatAlg_rcvdCmd(p_data);
//			}
//			break;
//
//		case active:
////			printf("task_heatingalg: active\r\n");
//			ssHeatAlg_checkAlgState();
//			ssHeatAlg_checkTargetTemp();
//			ssHeatAlg_putAlgState();
//			ssHeatAlg_Alg(&ssHeatAlgSem);
//			if(xQueueReceive(ssHeatAlg_qh, &(p_data), 0) == 1){
//				ssHeatAlg_rcvdCmd(p_data);
//			}
//			break;
//
//		default:
//			heatalg_task_state = sleep;
//			break;
//		}
//	}
//
//}
//
//static void ssHeatAlg_rcvdCmd(ssHeatAlg_d * p_data){
//	if (p_data->cmd == params.sys_srvc.heatalg.cmd.sleep){
//		heatalg_task_state = sleep;
//	}
//	else if (p_data->cmd == params.sys_srvc.heatalg.cmd.activate){
//		heatalg_task_state = active;
//	}
//	xSemaphoreGive(p_data->callingTaskSem); //freesemaphore
//}
//
//static void ssHeatAlg_checkAlgState(void){
//	if (asState == asStateHeatup){
//		heatalg_alg_next_state = fast_heating;
//	}
//	else if (asState == asStateReady){
//		heatalg_alg_next_state = no_heating;
//	}
//	else if (asState == asStateBrewing){
////		heatalg_alg_next_state = brew_heating;
//		heatalg_alg_next_state = slow_heating; //todo: correct this part
//	}
//	else if (asState == asStatePoweroff){
//		heatalg_alg_next_state = no_heating;
//	}
//	else{
//		heatalg_alg_next_state = no_heating;
//	}
//}
//
//static void ssHeatAlg_checkTargetTemp(void){
//	if (global_vars.temperature > global_vars.temperature_target){
//		heatalg_alg_next_state = no_heating;
//	}
////	printf("temp: %d\r\n", global_vars.temperature);
//}
//
//static void ssHeatAlg_putAlgState(void){
//	if (heatalg_alg_next_state != heatalg_alg_state){
//		ssHeatAlg_Alg_substate_firstrun = 1;
//	}
//	heatalg_alg_state = heatalg_alg_next_state;
//
//}
//
//static void ssHeatAlg_Alg(SemaphoreHandle_t * ssHeatAlgSem){
//
//	if (ssHeatAlg_Alg_substate_firstrun == 1){
//		ssHeatAlg_Alg_substate_firstrun = 0;
//		heatalg_alg_fastheatup_state = init_en_heater__fastheatupstate;
//		heatalg_alg_noheatup_state = init_dis_heater__noheatupstate;
//		heatalg_alg_slowheatup_state = init__slowheatupstate;
//		//do the same thing for others
//	}
//
//	switch(heatalg_alg_state){
//	case fast_heating:
//		ssHeatAlg_Alg_FastHeating(*ssHeatAlgSem);
////		printf("fast Heating\r\n");
//		break;
//	case slow_heating:
//		ssHeatAlg_Alg_SlowHeating(*ssHeatAlgSem);
////		printf("slow Heating\r\n");
//		break;
//	case brew_heating:
//		ssHeatAlg_Alg_BrewHeating(*ssHeatAlgSem);
////		printf("brew Heating\r\n");
//		break;
//	case no_heating:
//		ssHeatAlg_Alg_NoHeating(*ssHeatAlgSem);
////		printf("no Heating\r\n");
//		break;
//	default:
//		heatalg_alg_next_state = slow_heating;
//		break;
//	}
//
//}
//
//
//static void ssHeatAlg_Alg_FastHeating(SemaphoreHandle_t * ssHeatAlgSem){
//	switch(heatalg_alg_fastheatup_state){
//	case init_en_heater__fastheatupstate:
//		ssHeatAlg_enHeater(ssHeatAlgSem);
//		heatalg_alg_fastheatup_state = wait__fastheatupstate;
//		break;
//	case wait__fastheatupstate:
//		break;
//	}
//};
//
//
//static void ssHeatAlg_Alg_SlowHeating(SemaphoreHandle_t * ssHeatAlgSem){
//
//	static unsigned int time_now = 0, time_start = 0;
//
//	ssTimer_d ssTimer2Data;		//todo: can make this global?! no bc at same time maybe some
//	ssTimer_d * const p_ssTimer2Data = &ssTimer2Data;
//
//	switch(heatalg_alg_slowheatup_state){
//	case init__slowheatupstate:
//		//nothing
//		heatalg_alg_slowheatup_state = on_state__slowheatupstate;
//		heatalg_alg_slowheatup_on_state = init___slowheatuponstate;
//		heatalg_alg_slowheatup_off_state = init___slowheatupoffstate;
//		break;
//
//	case on_state__slowheatupstate:
//		switch(heatalg_alg_slowheatup_on_state){
//		case init___slowheatuponstate:
////			printf("on init\r\n");
//			ssHeatAlg_enHeater(ssHeatAlgSem);	//enable signaling
//			p_ssTimer2Data->callingTaskSem =  *ssHeatAlgSem; //set start time
//			p_ssTimer2Data->cmd = params.sys_srvc.timer.cmd.read_freerunning;
//			xQueueSend(ssTimer2_qh, &p_ssTimer2Data, portMAX_DELAY);
//			xSemaphoreTake(p_ssTimer2Data->callingTaskSem ,portMAX_DELAY);
//			time_start = p_ssTimer2Data->time;
//			heatalg_alg_slowheatup_on_state = wait___slowheatuponstate; //go to wait substate
//			break;
//		case wait___slowheatuponstate:
////			printf("on wait\r\n");
//			p_ssTimer2Data->callingTaskSem =  *ssHeatAlgSem; //set current time
//			p_ssTimer2Data->cmd = params.sys_srvc.timer.cmd.read_freerunning;
//			xQueueSend(ssTimer2_qh, &p_ssTimer2Data, portMAX_DELAY);
//			xSemaphoreTake(p_ssTimer2Data->callingTaskSem ,portMAX_DELAY);
//			time_now = p_ssTimer2Data->time;
//			if (time_now > time_start+HEATALG_SLOW_TIMEON){	//go to offstate if time finished
//				heatalg_alg_slowheatup_on_state = init___slowheatuponstate;
//				heatalg_alg_slowheatup_off_state = init___slowheatupoffstate;
//				heatalg_alg_slowheatup_state = off_state__slowheatupstate;
//			}
//			break;
//		}
//		break;
//
//	case off_state__slowheatupstate:
//
//		switch(heatalg_alg_slowheatup_off_state){
//		case init___slowheatupoffstate:
////			printf("off init\r\n");
//			ssHeatAlg_disHeater(ssHeatAlgSem);	//disable signaling
//			p_ssTimer2Data->callingTaskSem =  *ssHeatAlgSem; //set start time
//			p_ssTimer2Data->cmd = params.sys_srvc.timer.cmd.read_freerunning;
//			xQueueSend(ssTimer2_qh, &p_ssTimer2Data, portMAX_DELAY);
//			xSemaphoreTake(p_ssTimer2Data->callingTaskSem ,portMAX_DELAY);
//			time_start = p_ssTimer2Data->time;
//			heatalg_alg_slowheatup_off_state = wait___slowheatupoffstate; //go to wait substate
//			heatalg_alg_slowheatup_state = off_state__slowheatupstate;
//			break;
//		case wait___slowheatupoffstate:
////			printf("off wait\r\n");
//			p_ssTimer2Data->callingTaskSem =  *ssHeatAlgSem; //set current time
//			p_ssTimer2Data->cmd = params.sys_srvc.timer.cmd.read_freerunning;
//			xQueueSend(ssTimer2_qh, &p_ssTimer2Data, portMAX_DELAY);
//			xSemaphoreTake(p_ssTimer2Data->callingTaskSem ,portMAX_DELAY);
//			time_now = p_ssTimer2Data->time;
//
//			if (time_now > time_start+HEATALG_SLOW_TIMEOFF){	//go to offstate if time finished
//				heatalg_alg_slowheatup_off_state = init___slowheatupoffstate;
//				heatalg_alg_slowheatup_on_state = init___slowheatuponstate;
//				heatalg_alg_slowheatup_state = on_state__slowheatupstate;
//			}
//			break;
//		}
//		break;
//	}
//
//
////		ssHeatAlg_enHeater(ssHeatAlgSem);
////		heatalg_alg_slowheatup_state = wait__fastheatupstate;
//
//}
//static void ssHeatAlg_Alg_BrewHeating(SemaphoreHandle_t * ssHeatAlgSem){
//
//}
//
//static void ssHeatAlg_Alg_NoHeating(SemaphoreHandle_t * ssHeatAlgSem){
//	switch(heatalg_alg_noheatup_state){
//	case init_dis_heater__noheatupstate:
//		ssHeatAlg_disHeater(ssHeatAlgSem);
//		heatalg_alg_noheatup_state = wait__noheatupstate;
//		break;
//	case wait__noheatupstate:
//		break;
//	}
//}
//
//static void ssHeatAlg_enHeater(SemaphoreHandle_t * ssHeatAlgSem){
//	ssTrgHeater_d ssTrgHeaterData;		//todo: can make this global?! no bc at same time maybe some
//	ssTrgHeater_d * const p_ssTrgHeaterData = &ssTrgHeaterData;
//	p_ssTrgHeaterData->callingTaskSem = *ssHeatAlgSem;
//	p_ssTrgHeaterData->cmd = params.sys_srvc.trgheater.cmd.activate;
//
//	xQueueSend(ssTrgHeater_qh, &p_ssTrgHeaterData, portMAX_DELAY);
//	xSemaphoreTake(p_ssTrgHeaterData->callingTaskSem ,portMAX_DELAY);
//}
//
//static void ssHeatAlg_disHeater(SemaphoreHandle_t * ssHeatAlgSem){
//	ssTrgHeater_d ssTrgHeaterData;		//todo: can make this global?! no bc at same time maybe some
//	ssTrgHeater_d * const p_ssTrgHeaterData = &ssTrgHeaterData;
//	p_ssTrgHeaterData->callingTaskSem = *ssHeatAlgSem;
//	p_ssTrgHeaterData->cmd = params.sys_srvc.trgheater.cmd.sleep;
//
//	xQueueSend(ssTrgHeater_qh, &p_ssTrgHeaterData, portMAX_DELAY);
//	xSemaphoreTake(p_ssTrgHeaterData->callingTaskSem ,portMAX_DELAY);
//}
//
////=============
//void ssTask1_t(void *pvParameters){
//
//
//	while(1){
////		vTaskPrioritySet( ssTask1_task_handler, HIGH_PRIORITY );
////		for (int i = 0; i<100000; i++);
////		printf("task1-high-1\r\n");
////		for (int i = 0; i<100000; i++);
////		printf("task1-high-2\r\n");
////		for (int i = 0; i<100000; i++);
////		printf("task1-high-3\r\n");
////		for (int i = 0; i<100000; i++);
////		printf("task1-high-4\r\n");
////		for (int i = 0; i<100000; i++);
////		printf("task1-high-5\r\n");
////		vTaskDelay(pdMS_TO_TICKS(5));
////
////
////		vTaskPrioritySet( ssTask1_task_handler, NORMAL_PRIORITY );
////		for (int i = 0; i<100000; i++);
////		printf("task1-normal-1\r\n");
////		for (int i = 0; i<100000; i++);
////		printf("task1-normal-2\r\n");
////		for (int i = 0; i<100000; i++);
////		printf("task1-normal-3\r\n");
////		for (int i = 0; i<100000; i++);
////		printf("task1-normal-4\r\n");
////		for (int i = 0; i<100000; i++);
////		printf("task1-normal-5\r\n");
////		vTaskDelay(pdMS_TO_TICKS(5));
//		printf("Task1\r\n");
//		vTaskDelay(pdMS_TO_TICKS(1000));
//
//	}
//}
//
//void ssTask2_t(void *pvParameters){
//
//	while(1){
////		printf("task2\r\n");
////		vTaskDelay(pdMS_TO_TICKS(5));
//		printf("Task22\r\n");
//		vTaskDelay(pdMS_TO_TICKS(950));
//
//	}
//}
//
////============
//void ssTest2_1_t(void *pvParameters){
//
//		enum ssTask1States{
//				low,
//				high,
//				check_state
//			};
//		enum ssTask1States state, pre_state;
//		state = low;
//		pre_state = low;
////		vTaskPrioritySet( ssTest2_1_task_handler, 1 );
//		char p_data;
//
//		vTaskPrioritySet( ssTest2_1_task_handler, 5 );
//
//
//	while(1){
//
//
////		vTaskDelay(pdMS_TO_TICKS(5));
//
//		switch(state){
//		case low:
//			for (int j=0; j<10; j++){
//				for (int i = 0; i<100000; i++);
//			}
//			printf("task1: low\r\n");
//			state = check_state;
//			pre_state = low;
////			vTaskDelay(pdMS_TO_TICKS(5));
//			vTaskDelay(pdMS_TO_TICKS(100));
//			break;
//
//		case high:
////			for (int k=0; k<10; k++){
//			for (int k=0; k<4; k++){
//				for (int j=0; j<10; j++){
//					for (int i = 0; i<100000; i++);
//				}
//			}
//			printf("task1: high\r\n");
//			state = check_state;
//			pre_state = high;
//			unsigned int t = timer_readMs(1);
//			printf("task1: %u\r\n", t);
////			vTaskDelay(pdMS_TO_TICKS(5));
//			break;
//
//		case check_state:
//			if(xQueueReceive(intr2ssFlow_qh, &(p_data), 0) == 1 ){
//				state = high;
////				vTaskPrioritySet( ssTest2_1_task_handler, 5 );
//			}
//			else{
//				state = low;
//				if (pre_state = high){
////					vTaskPrioritySet( ssTest2_1_task_handler, 1);
//				}
//			}
//
//			break;
//		}
//
//	}
//}
//
//
//
//
//void ssTest2_2_t(void *pvParameters){
//
//	enum ssTask1States{
//			low,
//			high,
//			check_state
//		};
//	enum ssTask1States state, pre_state;
//	state = low;
//	pre_state = low;
////	vTaskPrioritySet( ssTest2_2_task_handler, 1 );
//	char p_data;
//
//	vTaskPrioritySet( ssTest2_2_task_handler, 4 );
//
//	while(1){
//
//		switch(state){
//		case low:
//			for (int j=0; j<10; j++){
//				for (int i = 0; i<100000; i++);
//			}
//			printf("task2: low\r\n");
//			state = check_state;
//			pre_state = low;
////			vTaskDelay(pdMS_TO_TICKS(5));
//			vTaskDelay(pdMS_TO_TICKS(100));
//			break;
//
//		case high:
////			for (int k=0; k<10; k++){
//			for (int k=0; k<4; k++){
//				for (int j=0; j<10; j++){
//					for (int i = 0; i<100000; i++);
//				}
//			}
//			printf("task2: high\r\n");
//			state = check_state;
//			pre_state = high;
//			unsigned int t = timer_readMs(2);
//			printf("task2: %u\r\n", t);
//			break;
//
//		case check_state:
//			if(xQueueReceive(intr2ssTrgHeater_qh, &(p_data), 0) == 1 ){
//				state = high;
//				vTaskPrioritySet( ssTest2_2_task_handler, 4 );
//			}
//			else{
//				state = low;
//				if (pre_state = high){
//					vTaskPrioritySet( ssTest2_2_task_handler, 1);
//				}
//			}
//			break;
//		}
//
//	}
//}
//
//
//void ssTest2_highp_t(void *pvParameters){
//	char p_data;
//	while(1){
//		//wait for q
//		if(xQueueReceive(intr2ssDS_qh, &(p_data), portMAX_DELAY) == 1 ){
//			//if there is sth, go for releasing and ceiling, then wait again
//			// i can send structure here
////			printf("HEY!\r\n");
//			if (p_data==0){
////				EXTI->RPR1 = (in_fm_Pin);
//				timer_count(1, 0xFFFFFFFF);
//				unsigned int t = timer_readMs(1);
//
//				//todo: should be removed, for ceiling priority
////				printf("task1 - before: %u\r\n", t);
//				xQueueOverwrite(intr2ssFlow_qh, &(p_data));
//				vTaskPrioritySet( ssTest2_1_task_handler, 6 );
////				printf("task1 - after: %u\r\n", t);
//
//			}
//			else if (p_data==1){
//				//todo: should be removed
//				timer_count(2, 0xFFFFFFFF);
//		//		timer_stopReset(1); //reset timer
//				unsigned int t = timer_readMs(2);
////				printf("task2 - intr: %u\r\n", t);
//
//				//todo: should be removed, for ceiling priority
//
//
//				xQueueOverwrite(intr2ssTrgPump_qh, &(p_data));
//				xQueueOverwrite(intr2ssTrgHeater_qh, &(p_data));
//
//				vTaskPrioritySet( ssTest2_2_task_handler, 6 );
//
//			}
//
//		}
//
//
//	}
//}
//
