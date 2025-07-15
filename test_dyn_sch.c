/*
 * The things should be tested include: semaphors, queues
 *
 * */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

#define queue_rcv								1		//done
#define preempting_lower_reldeadline_per		2		//done
#define preempting_lower_reldeadline_event		3
#define sem_rcv									4		//done
#define queue_send								5		//done
#define sem_rcv_cnting							6		//done
#define mutex									7		//not successfull
#define preempting_lower_reldeadline_per_different_period	8
#define softdeadline_maxdelay


#define test_mode	preempting_lower_reldeadline_per_different_period

/*
 * We want to check while it recieve queue it does the pre-emption or not
 * */
#if (test_mode == queue_rcv)

	QueueHandle_t xQueue1;

	void T1(void *pvParameters){
		char buffer;
		buffer = 'c';
		while(1){
			for (int i = 0; i<10; i++){
				vTaskDelay(pdMS_TO_TICKS(1000));
				printf("Task1\r\n");
			}
			xQueueSend( xQueue1, &buffer, 0);
			printf("Task1-after sending in queue\r\n");
		}
	}

	void T2(void *pvParameters){
		int flag = 0;
		char buffer;
		while(1){
			if ( xQueueReceive(xQueue1, &buffer, portMAX_DELAY) == pdPASS){
//				while(1){
					printf("Task88\r\n");
//				}
			}
		}
	}

	void adding_tasks (void){
		TaskHandle_t T1Handler;
		TaskHandle_t T2Handler;

		xTaskCreate	(T1, (const char *) "test task 1", configMINIMAL_STACK_SIZE, (void *) NULL, 2, &T1Handler, 300);
		xTaskCreate	(T2, (const char *) "test task 2", configMINIMAL_STACK_SIZE, (void *) NULL, 3, &T2Handler, 50);

		xQueue1 = xQueueCreate( 1, sizeof( char ) );

		printf("Hey!\r\n");
	}
/*
 * =====================================================================================================
 * =====================================================================================================
 * also I want to check that after a while a task with longer relative deadline pre-empt the other one or not
 * both I can show with periodic tasks and event tasks
 * periodic tasks: create tasks, in a priority system just based on relative deadline, the order is always same, however, in a dynamic system, tasks with lower rel deadline may run sooner bc their abs deadline is close
 * */
#elif (test_mode == preempting_lower_reldeadline_per)

	int a = 1;

	void T1(void *pvParameters){
		vTaskDelay(23);
		while(1){
			for (int i =0; i<a; i++)
				for (int j =0; j<a; j++);
			printf("dead 1\r\n");
//			vTaskDelay(pdMS_TO_TICKS(2));
			vTaskDelay(2);
		}
	}
	void T2(void *pvParameters){
		vTaskDelay(4);
		while(1){
			for (int i =0; i<a; i++)
				for (int j =0; j<a; j++);
			printf("dead 2\r\n");
			vTaskDelay(2);
		}
	}
	void T3(void *pvParameters){
		vTaskDelay(5);
		while(1){
			for (int i =0; i<a; i++)
				for (int j =0; j<a; j++);
			printf("dead 3\r\n");
			vTaskDelay(2);
		}
	}
	void T4(void *pvParameters){
		vTaskDelay(7);
		while(1){
			for (int i =0; i<a; i++)
				for (int j =0; j<a; j++);
			printf("dead 4\r\n");
			vTaskDelay(2);
		}
	}
	void T5(void *pvParameters){
		vTaskDelay(11);
		while(1){
			for (int i =0; i<a; i++)
				for (int j =0; j<a; j++);
			printf("dead 5\r\n");
			vTaskDelay(2);
		}
	}


	void adding_tasks (void){

		xTaskCreate	(T1, (const char *) "test task 1", configMINIMAL_STACK_SIZE, (void *) NULL, 5, NULL, 1);
		xTaskCreate	(T2, (const char *) "test task 2", configMINIMAL_STACK_SIZE, (void *) NULL, 4, NULL, 2);
		xTaskCreate	(T3, (const char *) "test task 3", configMINIMAL_STACK_SIZE, (void *) NULL, 3, NULL, 3);
		xTaskCreate	(T4, (const char *) "test task 4", configMINIMAL_STACK_SIZE, (void *) NULL, 2, NULL, 4);
		xTaskCreate	(T5, (const char *) "test task 5", configMINIMAL_STACK_SIZE, (void *) NULL, 1, NULL, 5);

	}

/*
 * =====================================================================================================
 * =====================================================================================================
 * Checking pre-emption is done after a task with shorter abs deadline recieve semaphore (immediately after another task giving it)
 */
#elif (test_mode == sem_rcv)

	SemaphoreHandle_t xQueue1;

	void T1(void *pvParameters){
		while(1){
			for (int i = 0; i<10; i++){
				vTaskDelay(pdMS_TO_TICKS(1000));
				printf("Task1\r\n");
			}
			xSemaphoreGive( xQueue1);
			printf("Task1-after giving\r\n");

		}
	}

	void T2(void *pvParameters){
		while(1){
			if ( xSemaphoreTake(xQueue1, portMAX_DELAY) == pdPASS){
					printf("Task88\r\n");
			}
		}
	}

	void adding_tasks (void){
		TaskHandle_t T1Handler;
		TaskHandle_t T2Handler;

		xTaskCreate	(T1, (const char *) "test task 1", configMINIMAL_STACK_SIZE, (void *) NULL, 2, &T1Handler, 300);
		xTaskCreate	(T2, (const char *) "test task 2", configMINIMAL_STACK_SIZE, (void *) NULL, 3, &T2Handler, 50);

		xQueue1 = xSemaphoreCreateBinary();

		printf("Hey!\r\n");
	}

/*
 * =====================================================================================================
 * =====================================================================================================
 * Checking pre-emption is done after a task with shorter abs deadline send to a full queue after it get empty (immediately after another task giving it)
 */
#elif (test_mode == queue_send)

	QueueHandle_t xQueue1;

	void T1(void *pvParameters){
		char buffer;
		buffer = 'c';
		xQueueSend( xQueue1, &buffer, 0);
		while(1){
			for (int i = 0; i<10; i++){
				printf("Task1\r\n");
				vTaskDelay(pdMS_TO_TICKS(1000));
			}
			printf("Task1 go for receiving\r\n", buffer);
			if (xQueueReceive( xQueue1, &buffer, portMAX_DELAY)){
				printf("Task1-after recieving in queue %c\r\n", buffer);
			}

		}
	}

	void T2(void *pvParameters){
		int flag = 0;
		char buffer = 'd';
		while(1){
			vTaskDelay(pdMS_TO_TICKS(9000));
			xQueueSend(xQueue1, &buffer, portMAX_DELAY);
			printf("Task2 successfully Sent\r\n");
			vTaskDelay(portMAX_DELAY);
		}
	}

	void adding_tasks (void){
		TaskHandle_t T1Handler;
		TaskHandle_t T2Handler;

		xTaskCreate	(T1, (const char *) "test task 1", configMINIMAL_STACK_SIZE, (void *) NULL, 2, &T1Handler, 300);
		xTaskCreate	(T2, (const char *) "test task 2", configMINIMAL_STACK_SIZE, (void *) NULL, 3, &T2Handler, 50);

		xQueue1 = xQueueCreate( 1, sizeof( char ) );

		printf("Hey!\r\n");
	}

/*
 * =====================================================================================================
 * =====================================================================================================
 * check preemption is done on counting semaphors
 */
#elif (test_mode == sem_rcv_cnting)

	SemaphoreHandle_t xQueue1;

	void T1(void *pvParameters){
		while(1){
			for (int i = 0; i<2; i++){
				vTaskDelay(pdMS_TO_TICKS(1000));
				printf("Task1\r\n");
			}
			xSemaphoreGive( xQueue1);
			printf("Task1-after giving\r\n");

		}
	}

	void T2(void *pvParameters){
		while(1){
			if ( xSemaphoreTake(xQueue1, portMAX_DELAY) == pdPASS){
				printf("Task2-1\r\n");
			}
			if ( xSemaphoreTake(xQueue1, portMAX_DELAY) == pdPASS){
				printf("Task2-2\r\n");
			}
			vTaskDelay(pdMS_TO_TICKS(10000));
		}
	}

	void adding_tasks (void){
		TaskHandle_t T1Handler;
		TaskHandle_t T2Handler;

		xTaskCreate	(T1, (const char *) "test task 1", configMINIMAL_STACK_SIZE, (void *) NULL, 2, &T1Handler, 300);
		xTaskCreate	(T2, (const char *) "test task 2", configMINIMAL_STACK_SIZE, (void *) NULL, 3, &T2Handler, 50);

		xQueue1 = xSemaphoreCreateCounting( 2, 0 );

		printf("Hey!\r\n");
	}

/*
 * =====================================================================================================
 * =====================================================================================================
 * check mutexes works fine, also premeption is done :)
 */
#elif (test_mode == mutex)

	SemaphoreHandle_t xQueue1 = NULL;

	void T1(void *pvParameters){
		while(1){

			if (xSemaphoreTake(xQueue1, portMAX_DELAY)==pdTRUE){
				printf("Task1 is taken\r\n");
				for (int i=0; i<1000; i++)
					for (int j=0; j<1000; j++);
				xSemaphoreGive( xQueue1);
				printf("Task1 after given\r\n");
			}
			else{
				printf("Task1 is continue\r\n");
			}
			printf("still task1\r\n");
//			vTaskDelay(pdMS_TO_TICKS(2000));


		}
	}

	void T2(void *pvParameters){
		while(1){
			vTaskDelay(pdMS_TO_TICKS(1000));
			if (xSemaphoreTake(xQueue1, portMAX_DELAY)==pdTRUE){
				printf("Task2 is taken\r\n");
				xSemaphoreGive( xQueue1);
				printf("Task2 after given\r\n");
			}
			else{
				printf("Task2 is continue\r\n");
			}
			printf("still task2\r\n");

		}
	}

	void adding_tasks (void){
		TaskHandle_t T1Handler;
		TaskHandle_t T2Handler;

		xTaskCreate	(T1, (const char *) "test task 1", configMINIMAL_STACK_SIZE, (void *) NULL, 2, &T1Handler, 300);
		xTaskCreate	(T2, (const char *) "test task 2", configMINIMAL_STACK_SIZE, (void *) NULL, 3, &T2Handler, 50);

		xQueue1 = xSemaphoreCreateMutex();

		printf("Hey!\r\n");
	}

/*
 * =====================================================================================================
 * =====================================================================================================
 * Checking for an application with different periods
 * */
#elif (test_mode == preempting_lower_reldeadline_per_different_period)

	void T1(void *pvParameters){
		while(1){
//			for (int i =0; i<1000; i++)
//				for (int j =0; j<1000; j++);
			printf("dead 1\r\n");
//			vTaskDelay(pdMS_TO_TICKS(2));
			vTaskDelay(2);
		}
	}
	void T2(void *pvParameters){
		while(1){
//			for (int i =0; i<1000; i++)
//				for (int j =0; j<1000; j++);
			printf("dead 2\r\n");
			vTaskDelay(3);
		}
	}
	void T3(void *pvParameters){
		while(1){
//			for (int i =0; i<1000; i++)
//				for (int j =0; j<1000; j++);
			printf("dead 3\r\n");
			vTaskDelay(4);
		}
	}
	void T4(void *pvParameters){
		while(1){
//			for (int i =0; i<1000; i++)
//				for (int j =0; j<1000; j++);
			printf("dead 4\r\n");
			vTaskDelay(5);
		}
	}
	void T5(void *pvParameters){
		while(1){
//			for (int i =0; i<1000; i++)
//				for (int j =0; j<1000; j++);
			printf("dead 5\r\n");
			vTaskDelay(6);
		}
	}


	void adding_tasks (void){

		xTaskCreate	(T1, (const char *) "test task 1", configMINIMAL_STACK_SIZE, (void *) NULL, 5, NULL, 2);
		xTaskCreate	(T2, (const char *) "test task 2", configMINIMAL_STACK_SIZE, (void *) NULL, 4, NULL, 3);
		xTaskCreate	(T3, (const char *) "test task 3", configMINIMAL_STACK_SIZE, (void *) NULL, 3, NULL, 4);
		xTaskCreate	(T4, (const char *) "test task 4", configMINIMAL_STACK_SIZE, (void *) NULL, 2, NULL, 5);
		xTaskCreate	(T5, (const char *) "test task 5", configMINIMAL_STACK_SIZE, (void *) NULL, 1, NULL, 6);

	}

#endif


