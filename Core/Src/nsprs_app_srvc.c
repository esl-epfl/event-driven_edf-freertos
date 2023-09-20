#include "nsprs_app_srvc.h"
#include "nsprs_comm.h"
#include "nsprs_params.h"
#include "nsprs_common.h"
#include "stm32_trg_driver.h"

#include "nsprs_sys_srvc.h"

//period
//period
int SM_periodMs = 100;//2000;	//period of this service

//data pointers for interacting with system services
ssTimer_d ssTimerData;
ssTimer_d * const p_ssTimerData = &ssTimerData;
ssTimer_d ssTimer2Data;
ssTimer_d * const p_ssTimer2Data = &ssTimer2Data;
ssTimer_d ssTimer3Data;
ssTimer_d * const p_ssTimer3Data = &ssTimer3Data;
ssTimer_d ssTimer4Data;
ssTimer_d * const p_ssTimer4Data = &ssTimer4Data;
ssTimer_d ssTimer5Data;
ssTimer_d * const p_ssTimer5Data = &ssTimer5Data;
ssHeatPump_d ssHeatPumpData;
ssHeatPump_d * const p_ssHeatPumpData = &ssHeatPumpData;
ssTrgPump_d ssTrgPumpData;
ssTrgPump_d * const p_ssTrgPumpData = &ssTrgPumpData;
ssTrgHeater_d ssTrgHeaterData;
ssTrgHeater_d * const p_ssTrgHeaterData = &ssTrgHeaterData;
ssHMI_d ssHMIData;
ssHMI_d * const p_ssHMIData = &ssHMIData;
ssTemp_d ssTempData;
ssTemp_d * const p_ssTempData = &ssTempData;
ssFlow_d ssFlowData;
ssFlow_d * const p_ssFlowData = &ssFlowData;
ssHeatAlg_d ssHeatAlgData;
ssHeatAlg_d * const p_ssHeatAlgData = &ssHeatAlgData;

void launch_asBasicMachine (void){
	create_asSems();
	xTaskCreate( asBasicMachine_t, ( const char * ) "app srvc: main FSM task", configMINIMAL_STACK_SIZE, NULL, NORMAL_PRIORITY, &asBasicMachine_task_handler, portMAX_DELAY-1 );/*ht: adding deadling*/
}
void delete_asBasicMachine (void){
	//delete task
	vTaskDelete( asBasicMachine_task_handler );
	//delete semaphors
	delete_asSems();
}
void create_asSems (void){
	asSem1 = xSemaphoreCreateBinary();
	asSem2 = xSemaphoreCreateBinary();
	asSem3 = xSemaphoreCreateBinary();
	asSem4 = xSemaphoreCreateBinary();
	configASSERT(asSem1);
	configASSERT(asSem2);
	configASSERT(asSem3);
	configASSERT(asSem4);
}
void delete_asSems(void){
	vSemaphoreDelete(asSem1);
	vSemaphoreDelete(asSem2);
	vSemaphoreDelete(asSem3);
	vSemaphoreDelete(asSem4);

}
//============== application service, main high-level brewing states task
void asBasicMachine_t(void *pvParameters){

//	asState = asStateInit;
	asStates next_asState = asStateTest; //asStateTest;//asStateInit;
	while(1){

		vTaskDelay(pdMS_TO_TICKS(SM_periodMs));
		asState = next_asState;
		printf("temp: %d\r\nvol: %d\r\n", global_vars.temperature, global_vars.volume_passed);

		switch(asState){
		case asStateInit:
			next_asState = asInit();
			break;
		case asStateHeatup:
			next_asState = asHeatup();
			break;
		case asStateReady:
			next_asState = asReady();
			break;
		case asStateBrewing:
			next_asState = asBrewing();
			break;
		case asStatePoweroff:
			next_asState = asPoweroff();
			break;
		case asStateTest:
			next_asState = asTest();
			break;
		}
	}
}

static asStates asInit(void){
	printf("SM: Init\r\n");
	asStates nextState;

	//HMI: activate
	// p_ssHMIData->callingTaskSem = asSem1;
	// p_ssHMIData->cmd = params.sys_srvc.hmi.cmd.activate;
	// xQueueSend(ssHMI_qh, &p_ssHMIData, portMAX_DELAY);
	// xSemaphoreTake(p_ssHMIData->callingTaskSem ,portMAX_DELAY);

	//turn off signaling to pump
	// p_ssTrgPumpData->callingTaskSem = asSem1;
	// p_ssTrgPumpData->cmd = params.sys_srvc.trgpump.cmd.sleep;
	// xQueueSend(ssTrgPump_qh, &p_ssTrgPumpData, portMAX_DELAY);
	// xSemaphoreTake(p_ssTrgPumpData->callingTaskSem ,portMAX_DELAY);
	pump_disable();

	//flow: sleep
	// p_ssFlowData->callingTaskSem = asSem1;
	// p_ssFlowData->cmd = params.sys_srvc.flow.cmd.sleep;
	// xQueueSend(ssFlow_qh, &p_ssFlowData, portMAX_DELAY);
	// xSemaphoreTake(p_ssFlowData->callingTaskSem ,portMAX_DELAY);
	flow_disable();

	//timer2: freerunning
	// p_ssTimer2Data->callingTaskSem = asSem1;
	// p_ssTimer2Data->cmd = params.sys_srvc.timer.cmd.start_freerunning;
	// xQueueSend(ssTimer2_qh, &p_ssTimer2Data, portMAX_DELAY);
	// xSemaphoreTake(p_ssTimer2Data->callingTaskSem ,portMAX_DELAY);
	data_timer.en = 1;

	//Temp: start (activate temperature to be updated frequently)
	// p_ssTempData->callingTaskSem = asSem1;
	// p_ssTempData->cmd = params.sys_srvc.temp.cmd.activate;
	// xQueueSend(ssTemp_qh, &p_ssTempData, portMAX_DELAY);
	// xSemaphoreTake(p_ssTempData->callingTaskSem ,portMAX_DELAY);
	//todo

	//activate heatalg
	// p_ssHeatAlgData->callingTaskSem = asSem1;
	// p_ssHeatAlgData->cmd = params.sys_srvc.heatalg.cmd.activate;
	// xQueueSend(ssHeatAlg_qh, &p_ssHeatAlgData, portMAX_DELAY);
	// xSemaphoreTake(p_ssHeatAlgData->callingTaskSem ,portMAX_DELAY);
	heater_enable();

	//one timer ready for intr-based mode (there is a bug, that
	//it does not operate for only first cmd and return the sem
	//immediately, so I just call it here wo time to solve the problem)
	// p_ssTimerData->callingTaskSem = asSem2;
	// p_ssTimerData->time = 1;
	// p_ssTimerData->cmd = params.sys_srvc.timer.cmd.start_intr_based;
	// xQueueSend(ssTimer_qh, &p_ssTimerData, portMAX_DELAY);
	// xSemaphoreTake(p_ssTimerData->callingTaskSem ,portMAX_DELAY);

	nextState = asStatePoweroff;//asStateHeatup; //todo: change it to heatup
	return nextState;
}

static asStates asHeatup(void){

	printf("SM: Heatup\r\n");
	asStates nextState;

	/*
	 * heat alg is running based on state,
	 * so here just check temp, if reached go to READY
	 */

	switch(appSM_heatup_state){
	case appSM_heatup_state_init:
		printf("appSM_heatup_state_init\r\n");

		global_vars.temperature_target = TEMP_HEATUP; //put target temp as heatup temp
		appSM_heatup_state = appSM_heatup_state_waitTempReched;
		nextState = asStateHeatup;
		break;

	case appSM_heatup_state_waitTempReched:
		printf("appSM_heatup_state_waitTempReched\r\n");
		//all the alg is handled by heatingalg, so just check finished or not
		if (global_vars.temperature > global_vars.temperature_target){
			appSM_heatup_state = appSM_heatup_state_finished;
		}
		else{
			appSM_heatup_state = appSM_heatup_state_waitTempReched;
		}
		nextState = asStateHeatup;
		break;

	case appSM_heatup_state_finished:
		printf("appSM_heatup_state_finished\r\n");
		appSM_heatup_state = appSM_heatup_state_init;
		nextState = asStateReady;
		break;
	}

	return nextState;


}
static asStates asReady(void){

	printf("SM: Ready\r\n");
	asStates nextState;
	static unsigned int time_now = 0, time_start = 0;

	switch(appSM_ready_state) {
	case appSM_ready_state_init:
		printf("appSM_ready_state_init\r\n");
		//put starting time
		p_ssTimer2Data->callingTaskSem = asSem1;
		p_ssTimer2Data->cmd = params.sys_srvc.timer.cmd.read_freerunning;
		xQueueSend(ssTimer2_qh, &p_ssTimer2Data, portMAX_DELAY);
		xSemaphoreTake(p_ssTimer2Data->callingTaskSem ,portMAX_DELAY);
		time_start = p_ssTimer2Data->time;

		//turn off signalling to pump
		p_ssTrgPumpData->callingTaskSem = asSem1;
		p_ssTrgPumpData->cmd = params.sys_srvc.trgpump.cmd.sleep;
		xQueueSend(ssTrgPump_qh, &p_ssTrgPumpData, portMAX_DELAY);
		xSemaphoreTake(p_ssTrgPumpData->callingTaskSem ,portMAX_DELAY);

		appSM_ready_state = appSM_ready_state_waitAndCheck;
		nextState = asStateReady;
		break;

	case appSM_ready_state_waitAndCheck:
		printf("appSM_ready_state_waitAndCheck\r\n");
		//read current time
		p_ssTimer2Data->callingTaskSem = asSem1;
		p_ssTimer2Data->cmd = params.sys_srvc.timer.cmd.read_freerunning;
		xQueueSend(ssTimer2_qh, &p_ssTimer2Data, portMAX_DELAY);
		xSemaphoreTake(p_ssTimer2Data->callingTaskSem ,portMAX_DELAY);
		time_now = p_ssTimer2Data->time;

		//read HMI
		p_ssHMIData->callingTaskSem = asSem1;
		p_ssHMIData->cmd = params.sys_srvc.hmi.cmd.read;
		xQueueSend(ssHMI_qh, &p_ssHMIData, portMAX_DELAY);
		xSemaphoreTake(p_ssHMIData->callingTaskSem ,portMAX_DELAY);

		//is there any req?
		if (p_ssHMIData->req_brew_espr != 0 || p_ssHMIData->req_brew_lungo != 0){
			appSM_ready_state = appSM_ready_state_init;
			nextState = asStateBrewing;
			printf("req\r\n");
		}
		//time finished
		else if (time_now > time_start + READY_WAIT_TIME){
			appSM_ready_state = appSM_ready_state_init;
			nextState = asStatePoweroff;
			printf("time_finished\r\n");
		}
		else{
			appSM_ready_state = appSM_ready_state_waitAndCheck;
			nextState = asStateReady;
		}
		break;
	}


	return nextState;
}

static asStates asBrewing(void){
	printf("SM: Brewing\r\n");
	asStates nextState;

	enum BrewExitSituation{finished, button_pressed, problem_happened};
	enum BrewExitSituation brew_exit_situation = finished;

	switch(appSM_brew_state) {

	case appSM_brew_state_init:
		printf("appSM_brew_state_init\r\n");
		//check type and then set target volume and temperature
		p_ssHMIData->callingTaskSem = asSem1;
		p_ssHMIData->cmd = params.sys_srvc.hmi.cmd.read;
		xQueueSend(ssHMI_qh, &p_ssHMIData, portMAX_DELAY);
		xSemaphoreTake(p_ssHMIData->callingTaskSem ,portMAX_DELAY);
		if (p_ssHMIData->req_brew_espr != 0){
			global_vars.volume_target = CUP_SIZE_ESPR;
			global_vars.temperature_target = TEMP_ESPR;
			printf("corto\r\n");
		}
		else if (p_ssHMIData->req_brew_lungo != 0){
			global_vars.volume_target = CUP_SIZE_LUNGO;
			global_vars.temperature_target = TEMP_LUNGO;
			printf("lungo\r\n");
		}
		else{
			//nothing detected, back to ready
			appSM_brew_state = appSM_brew_state_init;
			nextState = asStateReady;
			break;
		}
		//reset HMI
		p_ssHMIData->callingTaskSem = asSem1;
		p_ssHMIData->cmd = params.sys_srvc.hmi.cmd.reset;
		xQueueSend(ssHMI_qh, &p_ssHMIData, portMAX_DELAY);
		xSemaphoreTake(p_ssHMIData->callingTaskSem ,portMAX_DELAY);
		printf("HMI reseted\r\n");
		//activate trg_pump
		p_ssTrgPumpData->callingTaskSem = asSem1;
		p_ssTrgPumpData->cmd = params.sys_srvc.trgpump.cmd.activate;
		xQueueSend(ssTrgPump_qh, &p_ssTrgPumpData, portMAX_DELAY);
		xSemaphoreTake(p_ssTrgPumpData->callingTaskSem ,portMAX_DELAY);
		printf("Trg Pump enabled\r\n");
		//flow: activate (update passed volume and flow_rate whenever there is a pulse)
		p_ssFlowData->callingTaskSem = asSem1;
		p_ssFlowData->cmd = params.sys_srvc.flow.cmd.rst_activate;
		xQueueSend(ssFlow_qh, &p_ssFlowData, portMAX_DELAY);
		xSemaphoreTake(p_ssFlowData->callingTaskSem ,portMAX_DELAY);
		printf("flow activated\r\n");

		appSM_brew_state = appSM_brew_state_checkExit;
		nextState = asStateBrewing;
		break;

	case appSM_brew_state_checkExit:
		printf("appSM_brew_state_checkExit\r\n");
		//update HMI
		p_ssHMIData->callingTaskSem = asSem1;
		p_ssHMIData->cmd = params.sys_srvc.hmi.cmd.read;
		xQueueSend(ssHMI_qh, &p_ssHMIData, portMAX_DELAY);
		xSemaphoreTake(p_ssHMIData->callingTaskSem ,portMAX_DELAY);
		//check volume passed then exit
		if (global_vars.volume_passed > global_vars.volume_target){
			brew_exit_situation = finished;
			appSM_brew_state = appSM_brew_state_exit;
			printf("brew finished\r\n");
			break;
		}
		//check any button pressed then exit
		else if (p_ssHMIData->req_brew_espr != 0 ||
				p_ssHMIData->req_brew_lungo != 0 ||
				p_ssHMIData->req_hw != 0){
			printf("button pressed while brewing\r\n");
			brew_exit_situation = button_pressed;
			appSM_brew_state = appSM_brew_state_exit;
		}
		else{
			//continue waiting
			appSM_brew_state = appSM_brew_state_checkExit;
		}
		nextState = asStateBrewing;
		break;

	case appSM_brew_state_exit:
		printf("appSM_brew_state_exit\r\n");
		//trg_pump: sleep
		p_ssTrgPumpData->callingTaskSem = asSem1;
		p_ssTrgPumpData->cmd = params.sys_srvc.trgpump.cmd.sleep;
		xQueueSend(ssTrgPump_qh, &p_ssTrgPumpData, portMAX_DELAY);
		xSemaphoreTake(p_ssTrgPumpData->callingTaskSem ,portMAX_DELAY);
		//flow: sleep
		p_ssFlowData->callingTaskSem = asSem1;
		p_ssFlowData->cmd = params.sys_srvc.flow.cmd.sleep;
		xQueueSend(ssFlow_qh, &p_ssFlowData, portMAX_DELAY);
		xSemaphoreTake(p_ssFlowData->callingTaskSem ,portMAX_DELAY);
		//reset HMI
		p_ssHMIData->callingTaskSem = asSem1;
		p_ssHMIData->cmd = params.sys_srvc.hmi.cmd.reset;
		xQueueSend(ssHMI_qh, &p_ssHMIData, portMAX_DELAY);
		xSemaphoreTake(p_ssHMIData->callingTaskSem ,portMAX_DELAY);

		appSM_brew_state = appSM_brew_state_init;
		nextState = asStatePoweroff;
		break;
	}
	return nextState;

}

/*
 * init: Turn off pump, temp(later), turn off flow, reset hmi
 * wait: just read hmi
 */
static asStates asPoweroff(void){
	printf("SM: APO\r\n");
	asStates nextState;

	switch(appSM_APO_state) {

	case appSM_APO_state_init:

		//trg pump: sleep
		// p_ssTrgPumpData->callingTaskSem = asSem1;
		// p_ssTrgPumpData->cmd = params.sys_srvc.trgpump.cmd.sleep;
		// xQueueSend(ssTrgPump_qh, &p_ssTrgPumpData, portMAX_DELAY);
		// xSemaphoreTake(p_ssTrgPumpData->callingTaskSem ,portMAX_DELAY);
		pump_disable();
		//flow: sleep
		// p_ssFlowData->callingTaskSem = asSem1;
		// p_ssFlowData->cmd = params.sys_srvc.flow.cmd.sleep;
		// xQueueSend(ssFlow_qh, &p_ssFlowData, portMAX_DELAY);
		// xSemaphoreTake(p_ssFlowData->callingTaskSem ,portMAX_DELAY);
		flow_disable();
		//reset HMI
		p_ssHMIData->callingTaskSem = asSem1;
		p_ssHMIData->cmd = params.sys_srvc.hmi.cmd.reset;
		xQueueSend(ssHMI_qh, &p_ssHMIData, portMAX_DELAY);
		xSemaphoreTake(p_ssHMIData->callingTaskSem ,portMAX_DELAY);
		//reading temp: sleep //todo

		appSM_APO_state = appSM_APO_state_checkForWakeup;
		nextState = asStatePoweroff;


		break;

	case appSM_APO_state_checkForWakeup:
		//read HMI
		p_ssHMIData->callingTaskSem = asSem1;
		p_ssHMIData->cmd = params.sys_srvc.hmi.cmd.read;
		xQueueSend(ssHMI_qh, &p_ssHMIData, portMAX_DELAY);
		xSemaphoreTake(p_ssHMIData->callingTaskSem ,portMAX_DELAY);

		//is there any req?
		if (p_ssHMIData->req_brew_espr != 0 || p_ssHMIData->req_brew_lungo != 0){
			appSM_APO_state = appSM_ready_state_init;
			nextState = asStateHeatup;
		}
		else{
			appSM_APO_state = appSM_APO_state_checkForWakeup;
			nextState = asStatePoweroff;
		}
		break;
	}

	return nextState;
}

static asStates asTest(void){
	asStates nextState;
	int j =0;

	while(1){
		j++;

		for (int i = 0; i<6; i++){
			printf("timee: %d, %d, %d\r\n", data_timer.time, xTaskGetTickCount(), pdMS_TO_TICKS(203));
			vTaskDelay(pdMS_TO_TICKS(1000));
		}
		// printf("esprs: %d, %lu\r\nlungo: %d, %lu\r\n",
		// data_hmi.req_brew_esprs, data_hmi.time_esprs_ms,
		// data_hmi.req_brew_lungo, data_hmi.time_lungo_ms);

		if (j%2 == 0){
			heater_disable();
			printf("heater disable\r\n");
		}
		else{
			heater_enable();
			printf("heater enable\r\n");
		}

		

		data_hmi.req_brew_esprs = 0;
		data_hmi.req_brew_lungo= 0;
		data_hmi.time_esprs_ms=0;
		data_hmi.time_lungo_ms=0;


	}

	//make one of timers as freerunning
	p_ssTimer2Data->callingTaskSem = asSem1;
	p_ssTimer2Data->cmd = params.sys_srvc.timer.cmd.start_freerunning;
	xQueueSend(ssTimer2_qh, &p_ssTimer2Data, portMAX_DELAY);
	xSemaphoreTake(p_ssTimer2Data->callingTaskSem ,portMAX_DELAY);

	//one timer ready for intr-based mode (there is a bug, that
	p_ssTimerData->callingTaskSem = asSem2;
	p_ssTimerData->time = 1;
	p_ssTimerData->cmd = params.sys_srvc.timer.cmd.start_intr_based;
	xQueueSend(ssTimer_qh, &p_ssTimerData, portMAX_DELAY);
	xSemaphoreTake(p_ssTimerData->callingTaskSem ,portMAX_DELAY);

	printf("FLow test\r\n");

	p_ssFlowData->callingTaskSem = asSem1;
	p_ssFlowData->cmd = params.sys_srvc.flow.cmd.rst_activate;
	xQueueSend(ssFlow_qh, &p_ssFlowData, portMAX_DELAY);
	xSemaphoreTake(p_ssFlowData->callingTaskSem ,portMAX_DELAY);


	while(1){


		p_ssFlowData->callingTaskSem = asSem1;
		p_ssFlowData->cmd = params.sys_srvc.flow.cmd.rst_activate;
		xQueueSend(ssFlow_qh, &p_ssFlowData, portMAX_DELAY);
		xSemaphoreTake(p_ssFlowData->callingTaskSem ,portMAX_DELAY);
		vTaskDelay(pdMS_TO_TICKS(5000));

		printf("vol from app: %d\r\n", global_vars.volume_passed);


		printf("now sleep\r\n");
		p_ssFlowData->callingTaskSem = asSem1;
		p_ssFlowData->cmd = params.sys_srvc.flow.cmd.sleep;
		xQueueSend(ssFlow_qh, &p_ssFlowData, portMAX_DELAY);
		xSemaphoreTake(p_ssFlowData->callingTaskSem ,portMAX_DELAY);
		vTaskDelay(pdMS_TO_TICKS(5000));


		printf("vol from app: %d\r\n", global_vars.volume_passed);

	}

	printf("test timer\r\n");
	while(1){
		vTaskDelay(pdMS_TO_TICKS(5000));
		p_ssTimer2Data->callingTaskSem = asSem1;
		p_ssTimer2Data->cmd = params.sys_srvc.timer.cmd.read_freerunning;
		xQueueSend(ssTimer2_qh, &p_ssTimer2Data, portMAX_DELAY);
		xSemaphoreTake(p_ssTimer2Data->callingTaskSem ,portMAX_DELAY);

		printf("current time: %d\r\n", p_ssTimer2Data->time);

	}


//	trg_init_tmr14();
//	trg_init_tmr15();
//	trg_init_tmr16();
//	trg_init_tmr17();
//
//
//	printf("hmm\r\n");
//
//	while(1){
//		vTaskDelay(pdMS_TO_TICKS(1000));
//		trg_impl_tmr14(2, 500000, 0, 0, 0);
//		trg_impl_tmr15(2, 500000, 0, 0, 0);
//		trg_impl_tmr16(2, 500000, 0, 0, 0);
//		trg_impl_tmr17(2, 500000, 0, 0, 0);
//
//
//	}
//
//	printf("lets start...\r\n");
//	p_ssHeatAlgData->callingTaskSem = asSem1; //HMI
//	while(1){
//		p_ssHeatAlgData->cmd = params.sys_srvc.heatalg.cmd.activate;
//		xQueueSend(ssHeatAlg_qh, &p_ssHeatAlgData, portMAX_DELAY);
//		xSemaphoreTake(p_ssHeatAlgData->callingTaskSem ,portMAX_DELAY);
//		vTaskDelay(pdMS_TO_TICKS(1000));
//
//		p_ssHeatAlgData->cmd = params.sys_srvc.heatalg.cmd.sleep;
//		xQueueSend(ssHeatAlg_qh, &p_ssHeatAlgData, portMAX_DELAY);
//		xSemaphoreTake(p_ssHeatAlgData->callingTaskSem ,portMAX_DELAY);
//		vTaskDelay(pdMS_TO_TICKS(1000));
//	}

	printf("HELLO\r\n");
	p_ssHMIData->callingTaskSem = asSem1; //HMI
	p_ssHMIData->cmd = params.sys_srvc.hmi.cmd.activate;
	xQueueSend(ssHMI_qh, &p_ssHMIData, portMAX_DELAY);
	xSemaphoreTake(p_ssHMIData->callingTaskSem ,portMAX_DELAY);

	p_ssTimer2Data->callingTaskSem = asSem2; //Timer2
	p_ssTimer2Data->cmd = params.sys_srvc.timer.cmd.start_freerunning;
	xQueueSend(ssTimer2_qh, &p_ssTimer2Data, portMAX_DELAY);
	xSemaphoreTake(p_ssTimer2Data->callingTaskSem ,portMAX_DELAY);

	p_ssHMIData->cmd = params.sys_srvc.hmi.cmd.reset;
	xQueueSend(ssHMI_qh, &p_ssHMIData, portMAX_DELAY);
	xSemaphoreTake(p_ssHMIData->callingTaskSem ,portMAX_DELAY);

	printf("Lets go...\r\n");
	while(1){
		//read
		p_ssHMIData->cmd = params.sys_srvc.hmi.cmd.read;
		xQueueSend(ssHMI_qh, &p_ssHMIData, portMAX_DELAY);
		xSemaphoreTake(p_ssHMIData->callingTaskSem ,portMAX_DELAY);

		printf("req_espr = %i\r\nreq_lungo = %i\r\nreq_hw = %i\r\n\n",
				p_ssHMIData->req_brew_espr, p_ssHMIData->req_brew_lungo, p_ssHMIData->req_hw);

		p_ssHMIData->cmd = params.sys_srvc.hmi.cmd.reset;
		xQueueSend(ssHMI_qh, &p_ssHMIData, portMAX_DELAY);
		xSemaphoreTake(p_ssHMIData->callingTaskSem ,portMAX_DELAY);

		vTaskDelay(pdMS_TO_TICKS(1000));
//		if (HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
	}
//	write_on_log(678);

//	/*
//	 * Pump test
//	 */
//
//	p_ssHMIData->callingTaskSem = asSem1; //HMI
//	p_ssPumpData->callingTaskSem = asSem1; //signaling to pump
////	p_ssTempData->callingTaskSem = asSem2;  //temp
////	p_ssFlowData->callingTaskSem = asSem3;  //flow
//	p_ssTimerData->callingTaskSem = asSem4;	//timer
//
//	//hmi, reset
////	p_ssHMIData->cmd = params.sys_srvc.hmi.cmd.reset;
////	xQueueSend(ssHMI_qh, &p_ssHMIData, portMAX_DELAY);
////	xSemaphoreTake(p_ssHMIData->callingTaskSem ,portMAX_DELAY);
//
//	//pmp signaling, sleep
//	p_ssPumpData->cmd = params.sys_srvc.pump.cmd.sleep;
//	xQueueSend(ssPump_qh, &p_ssPumpData, portMAX_DELAY);
//	xSemaphoreTake(p_ssPumpData->callingTaskSem ,portMAX_DELAY);
//
//	//timer
////	p_ssTimerData->cmd = params.sys_srvc.timer.cmd.sleep;
////	xQueueSend(ssTimer_qh, &p_ssTimerData, portMAX_DELAY);
////	xSemaphoreTake(p_ssTimerData->callingTaskSem ,portMAX_DELAY);
//
//	//enable pump
//	p_ssPumpData->cmd = params.sys_srvc.pump.cmd.en_pump;
//	xQueueSend(ssPump_qh, &p_ssPumpData, portMAX_DELAY);
//	xSemaphoreTake(p_ssPumpData->callingTaskSem ,portMAX_DELAY);
//
//	printf("done!\r\n");
//
//	while(1);

	/*
	 * timer test
	 */
	unsigned int time_ms1, time_ms2;
	p_ssTimerData->callingTaskSem = asSem4;	//timer
	p_ssTimer2Data->callingTaskSem = asSem3;	//timer

	//timer1: sleep
	p_ssTimerData->cmd = params.sys_srvc.timer.cmd.sleep;
	xQueueSend(ssTimer_qh, &p_ssTimerData, portMAX_DELAY);
	xSemaphoreTake(p_ssTimerData->callingTaskSem ,portMAX_DELAY);

	//timer2: sleep
	p_ssTimer2Data->cmd = params.sys_srvc.timer.cmd.sleep;
	xQueueSend(ssTimer2_qh, &p_ssTimer2Data, portMAX_DELAY);
	xSemaphoreTake(p_ssTimer2Data->callingTaskSem ,portMAX_DELAY);

	//timer1: FreeRunning start and then read
	p_ssTimerData->cmd = params.sys_srvc.timer.cmd.start_freerunning;
	xQueueSend(ssTimer_qh, &p_ssTimerData, portMAX_DELAY);
	xSemaphoreTake(p_ssTimerData->callingTaskSem ,portMAX_DELAY);

	//timer2: FreeRunning start and then read
	p_ssTimerData->cmd = params.sys_srvc.timer.cmd.start_freerunning;
	xQueueSend(ssTimer2_qh, &p_ssTimerData, portMAX_DELAY);
	xSemaphoreTake(p_ssTimerData->callingTaskSem ,portMAX_DELAY);


	while(1){

//		//timer2: FreeRunning start and then read
//			p_ssTimerData->cmd = params.sys_srvc.timer.cmd.start_freerunning;
//			xQueueSend(ssTimer2_qh, &p_ssTimerData, portMAX_DELAY);
//			xSemaphoreTake(p_ssTimerData->callingTaskSem ,portMAX_DELAY);

		p_ssTimerData->cmd = params.sys_srvc.timer.cmd.read_freerunning;
		xQueueSend(ssTimer2_qh, &p_ssTimerData, portMAX_DELAY);
		xSemaphoreTake(p_ssTimerData->callingTaskSem ,portMAX_DELAY);
		time_ms1 = p_ssTimerData->time;


		//timer2: start intr
		p_ssTimer2Data->cmd = params.sys_srvc.timer.cmd.start_intr_based;
		p_ssTimer2Data->time = 100000;
		xQueueSend(ssTimer_qh, &p_ssTimer2Data, portMAX_DELAY);
		xSemaphoreTake(p_ssTimer2Data->callingTaskSem ,portMAX_DELAY);
//		vTaskDelay(pdMS_TO_TICKS(2000));

		//timer1: FreeRunning read
		p_ssTimerData->cmd = params.sys_srvc.timer.cmd.read_freerunning;
		xQueueSend(ssTimer2_qh, &p_ssTimerData, portMAX_DELAY);
		xSemaphoreTake(p_ssTimerData->callingTaskSem ,portMAX_DELAY);
		time_ms2 = p_ssTimerData->time;

		printf("time: %lu\r\n", time_ms2-time_ms1);

//		//timer1: sleep
//			p_ssTimerData->cmd = params.sys_srvc.timer.cmd.sleep;
//			xQueueSend(ssTimer2_qh, &p_ssTimerData, portMAX_DELAY);
//			xSemaphoreTake(p_ssTimerData->callingTaskSem ,portMAX_DELAY);

	}

	/*
	 * timer lower level test
	 */

//	_platform_timer_stopReset(1);
//	_platform_timer_stopReset(2);
//
//		int timer_id = 3;
//
//		_platform_timer_stopReset(timer_id);
//		while(1){
//		vTaskDelay(pdMS_TO_TICKS(100));
//		printf("t: %lu\r\n", _platform_timer_readMs(timer_id));
//		printf("t: %lu\r\n", _platform_timer_readMs(timer_id));
//
//		_platform_timer_count(timer_id, 950);
//		printf("t: %lu\r\n", _platform_timer_readMs(timer_id));
//		vTaskDelay(pdMS_TO_TICKS(400));
//		printf("t: %lu\r\n", _platform_timer_readMs(timer_id));
//		vTaskDelay(pdMS_TO_TICKS(400));
//		printf("t: %lu\r\n", _platform_timer_readMs(timer_id));
//		vTaskDelay(pdMS_TO_TICKS(400));
//		printf("t: %lu\r\n", _platform_timer_readMs(timer_id));
////		printf("HI!!");
////		vTaskDelay(pdMS_TO_TICKS(1000));
//		}


	/*
	 * Heater test
	 */

//	p_ssHMIData->callingTaskSem = asSem1; //HMI
	p_ssTrgHeaterData->callingTaskSem = asSem1; //signaling to heater
	p_ssTrgPumpData->callingTaskSem = asSem2; //signaling to pump
//	p_ssTempData->callingTaskSem = asSem4;  //temp
//	p_ssFlowData->callingTaskSem = asSem3;  //flow
//	p_ssTimerData->callingTaskSem = asSem4;	//timer

	//hmi, reset
//	p_ssHMIData->cmd = params.sys_srvc.hmi.cmd.reset;
//	xQueueSend(ssHMI_qh, &p_ssHMIData, portMAX_DELAY);
//	xSemaphoreTake(p_ssHMIData->callingTaskSem ,portMAX_DELAY);

	//heater signaling, sleep
//	printf("we start it\r\n");
//	p_ssHeaterData->cmd = params.sys_srvc.heater.cmd.sleep;
//	xQueueSend(ssHeater_qh, &p_ssHeaterData, portMAX_DELAY);
//	xSemaphoreTake(p_ssHeaterData->callingTaskSem ,portMAX_DELAY);
//
//	//pmp signaling, sleep
////	p_ssPumpData->cmd = params.sys_srvc.pump.cmd.sleep;
////	xQueueSend(ssPump_qh, &p_ssPumpData, portMAX_DELAY);
////	xSemaphoreTake(p_ssPumpData->callingTaskSem ,portMAX_DELAY);
//
//
	//enable heater
	p_ssTrgHeaterData->cmd = params.sys_srvc.trgheater.cmd.activate;
	xQueueSend(ssTrgHeater_qh, &p_ssTrgHeaterData, portMAX_DELAY);
	xSemaphoreTake(p_ssTrgHeaterData->callingTaskSem ,portMAX_DELAY);


	//enable pump
	p_ssTrgPumpData->cmd = params.sys_srvc.trgpump.cmd.activate;
	xQueueSend(ssTrgPump_qh, &p_ssTrgPumpData, portMAX_DELAY);
	xSemaphoreTake(p_ssTrgPumpData->callingTaskSem ,portMAX_DELAY);

	//temp enable
	p_ssTempData->callingTaskSem = asSem4;



	printf("done!\r\n");

//	trg_init_tmr14();
//	int i = 0;
	int temperature;

	while(1){
		vTaskDelay(pdMS_TO_TICKS(1000));
		//reading temp

//		p_ssTempData->cmd = params.sys_srvc.temp.cmd.read;
//		xQueueSend(ssTemp_qh, &p_ssTempData, portMAX_DELAY);
//		xSemaphoreTake(p_ssTempData->callingTaskSem ,portMAX_DELAY);
//		temperature = p_ssTempData->updatedTemp;
//		printf("Temperature = %d!\r\n", temperature);
////		i++;
////		trg_init_tmr14();
////		trg_impl_tmr14(TRG_MODE_T1HIGH_T2PWM_LOW, 40000, 30000,  1000, 2000);
	}

	nextState = asStateTest;
	return nextState;
}

