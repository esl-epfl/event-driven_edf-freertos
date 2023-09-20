///* FreeRTOS includes. */
//#include "FreeRTOS.h"
//#include "task.h"
//#include "queue.h"
//#include "timers.h"
//#include "semphr.h"
/////* nespresso(nsprs) headers */
//#include "nsprs_params.h"
//#include "nsprs_drivers.h"
//#include "nsprs_sys_srvc.h"
//#include "nsprs_app_srvc.h"
//#include "nsprs_comm.h"
//#include "nsprs_common.h"
//#include "nsprs_platform_nucleo_g070rb.h"

//
//void task1(void *pvParameters);
//
//TaskHandle_t task1_handler;
//int a = 0;






int main(void)


{
	//=======================
	launch_comm();
	init_hw();
	launch_ssAll();
	// launch_asBasicMachine();


//	xTaskCreate	(task1, (const char *) "task1", configMINIMAL_STACK_SIZE, (void *) NULL, 3, &task1_handler, portMAX_DELAY-1);


	vTaskStartScheduler();

	while (1);

	//============ main_test_edf_static ======
//	main_test_edf_static();

}