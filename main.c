/**
 * @file main.c
 * @brief Demonstration of a low-overhead dynamic EDF scheduler for FreeRTOS.
 * @author Hossein Taji, et al.
 *
 * This file provides examples to showcase the benefits of the dynamic Earliest
 * Deadline First (EDF) scheduler compared to the traditional static priority-based
 * scheduler in FreeRTOS.
 *
 * This work is based on the paper:
 * "Dynamic Scheduling for Event-Driven Embedded Industrial Applications."
 * 2023 IFIP/IEEE 31st International Conference on Very Large Scale Integration (VLSI-SoC).
 *
 * To observe the different behaviors, compile the code with the macro
 * 'configDYNAMIC_SCHEDULING' set to 1 (for EDF) and 0 (for static priority)
 * in your FreeRTOSConfig.h file.
 */

/* Standard C includes. */
#include <stdio.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/*--------------------------- Demo Configuration ------------------------------*/

// Set DEMO_SELECTION to one of the following to choose the example to run:
// 1: Periodic Task Demo - Demonstrates EDF with multiple periodic tasks.
// 2: Event-Driven Demo - Demonstrates preemption based on an event from a queue.
#define DEMO_SELECTION 1

/*------------------------- Function Prototypes -------------------------------*/

// Hardware and communication initialization.
static void prvSetupHardware(void);

// Demo setup functions.
static void prvPeriodicTaskDemoSetup(void);
static void prvEventDrivenDemoSetup(void);

/*------------------------- Periodic Task Demo --------------------------------*/
#if (DEMO_SELECTION == 1)

// Task priorities are used ONLY when configDYNAMIC_SCHEDULING is 0.
// We use Deadline Monotonic Priority Assignment (DM): shorter deadline = higher priority.
#define PERIODIC_TASK_1_PRIORITY  (tskIDLE_PRIORITY + 5)
#define PERIODIC_TASK_2_PRIORITY  (tskIDLE_PRIORITY + 4)
#define PERIODIC_TASK_3_PRIORITY  (tskIDLE_PRIORITY + 3)
#define PERIODIC_TASK_4_PRIORITY  (tskIDLE_PRIORITY + 2)
#define PERIODIC_TASK_5_PRIORITY  (tskIDLE_PRIORITY + 1)

// Task parameters: Relative Deadline and Period in ticks.
#define PERIOD_TASK_1_DEADLINE    (2)
#define PERIOD_TASK_2_DEADLINE    (3)
#define PERIOD_TASK_3_DEADLINE    (4)
#define PERIOD_TASK_4_DEADLINE    (5)
#define PERIOD_TASK_5_DEADLINE    (6)

// Task function that simulates some work and then delays for its period.
void vPeriodicTask(void *pvParameters) {
    char *taskName = (char *)pvParameters;
    TickType_t xPeriod = (TickType_t)0;

    // Determine period from task name for simplicity.
    if (taskName[5] == '1') xPeriod = PERIOD_TASK_1_DEADLINE;
    else if (taskName[5] == '2') xPeriod = PERIOD_TASK_2_DEADLINE;
    else if (taskName[5] == '3') xPeriod = PERIOD_TASK_3_DEADLINE;
    else if (taskName[5] == '4') xPeriod = PERIOD_TASK_4_DEADLINE;
    else if (taskName[5] == '5') xPeriod = PERIOD_TASK_5_DEADLINE;


    for (;;) {
        // Simulate work by printing a message.
        printf("%s - Deadline: %u\r\n", taskName, (unsigned int)xPeriod);

        // Wait for the next cycle.
        vTaskDelay(xPeriod);
    }
}

// Creates the set of periodic tasks.
static void prvPeriodicTaskDemoSetup(void) {
    printf("--- Periodic Task Demo ---\r\n");
    printf("Creating 5 tasks with different periods and deadlines.\r\n");
    #if (configDYNAMIC_SCHEDULING == 1)
        printf("Running with DYNAMIC (EDF) scheduler.\r\n");
        printf("Expected output: Tasks are interleaved based on nearest absolute deadline.\r\n\n");
    #else
        printf("Running with STATIC priority scheduler.\r\n");
        printf("Expected output: Tasks run strictly by priority. Task 1 will dominate.\r\n\n");
    #endif

    // Note: The priority parameter is IGNORED when configDYNAMIC_SCHEDULING is 1.
    // The relative deadline parameter is IGNORED when configDYNAMIC_SCHEDULING is 0.
    xTaskCreate(vPeriodicTask, "Task 1", configMINIMAL_STACK_SIZE, (void *)"Task 1", PERIODIC_TASK_1_PRIORITY, NULL, PERIOD_TASK_1_DEADLINE);
    xTaskCreate(vPeriodicTask, "Task 2", configMINIMAL_STACK_SIZE, (void *)"Task 2", PERIODIC_TASK_2_PRIORITY, NULL, PERIOD_TASK_2_DEADLINE);
    xTaskCreate(vPeriodicTask, "Task 3", configMINIMAL_STACK_SIZE, (void *)"Task 3", PERIODIC_TASK_3_PRIORITY, NULL, PERIOD_TASK_3_DEADLINE);
    xTaskCreate(vPeriodicTask, "Task 4", configMINIMAL_STACK_SIZE, (void *)"Task 4", PERIODIC_TASK_4_PRIORITY, NULL, PERIOD_TASK_4_DEADLINE);
    xTaskCreate(vPeriodicTask, "Task 5", configMINIMAL_STACK_SIZE, (void *)"Task 5", PERIODIC_TASK_5_PRIORITY, NULL, PERIOD_TASK_5_DEADLINE);
}

#endif /* DEMO_SELECTION == 1 */

/*------------------------- Event-Driven Demo ---------------------------------*/
#if (DEMO_SELECTION == 2)

// A queue for communication between the tasks.
static QueueHandle_t xDemoQueue = NULL;

// Task priorities (used only in static mode).
#define EVENT_HANDLER_TASK_PRIORITY (tskIDLE_PRIORITY + 3)
#define BACKGROUND_TASK_PRIORITY    (tskIDLE_PRIORITY + 2)

// Task deadlines (used only in dynamic mode).
#define EVENT_HANDLER_TASK_DEADLINE (50)   // Short deadline, high urgency.
#define BACKGROUND_TASK_DEADLINE    (300)  // Long deadline, low urgency.

/**
 * @brief A background task that performs some work and occasionally posts an event.
 */
void vBackgroundWorkerTask(void *pvParameters) {
    const char *pcMessage = "Event Trigger!";
    const TickType_t xDelayBetweenSends = pdMS_TO_TICKS(2000);

    for (;;) {
        printf("Background Task: Doing some work...\r\n");
        vTaskDelay(pdMS_TO_TICKS(500));
        printf("Background Task: Still working...\r\n");
        vTaskDelay(pdMS_TO_TICKS(500));

        printf("Background Task: Sending an event to the queue.\r\n");
        xQueueSend(xDemoQueue, &pcMessage, 0U);

        // Wait before sending the next event.
        vTaskDelay(xDelayBetweenSends);
    }
}

/**
 * @brief A task that waits for an event and processes it. This task has a short
 * deadline, making it very important once activated.
 */
void vEventHandlerTask(void *pvParameters) {
    char *pcReceivedMessage;

    for (;;) {
        // Wait indefinitely for a message to arrive on the queue.
        if (xQueueReceive(xDemoQueue, &pcReceivedMessage, portMAX_DELAY) == pdPASS) {
            // When we receive the message, our deadline clock starts.
            // The EDF scheduler will see our short deadline and run us immediately.
            printf(">>> Event Handler: Received '%s'. Preempting background work! <<<\r\n", pcReceivedMessage);
            // Simulate processing the event.
            vTaskDelay(pdMS_TO_TICKS(100));
            printf(">>> Event Handler: Event processing complete. <<<\r\n");
        }
    }
}

// Creates the tasks and queue for the event-driven demo.
static void prvEventDrivenDemoSetup(void) {
    printf("--- Event-Driven Demo ---\r\n");
    #if (configDYNAMIC_SCHEDULING == 1)
        printf("Running with DYNAMIC (EDF) scheduler.\r\n");
        printf("Expected output: Event Handler immediately preempts Background Task upon receiving a message.\r\n\n");
    #else
        printf("Running with STATIC priority scheduler.\r\n");
        printf("Expected output: Event Handler preempts because it has a higher static priority.\r\n\n");
    #endif

    // Create the queue.
    xDemoQueue = xQueueCreate(1, sizeof(char *));

    if (xDemoQueue != NULL) {
        // Create the tasks.
        xTaskCreate(vEventHandlerTask, "Handler", configMINIMAL_STACK_SIZE, NULL, EVENT_HANDLER_TASK_PRIORITY, NULL, EVENT_HANDLER_TASK_DEADLINE);
        xTaskCreate(vBackgroundWorkerTask, "Worker", configMINIMAL_STACK_SIZE, NULL, BACKGROUND_TASK_PRIORITY, NULL, BACKGROUND_TASK_DEADLINE);
    }
}

#endif /* DEMO_SELECTION == 2 */

/*--------------------------- Main Function -----------------------------------*/

int main(void) {
    // Initialize hardware, such as UART for printf.
    prvSetupHardware();

    // Select and set up the desired demo.
    #if (DEMO_SELECTION == 1)
        prvPeriodicTaskDemoSetup();
    #elif (DEMO_SELECTION == 2)
        prvEventDrivenDemoSetup();
    #else
        printf("Error: Invalid DEMO_SELECTION.\r\n");
    #endif

    // Start the scheduler.
    printf("Starting scheduler...\r\n");
    vTaskStartScheduler();

    // The following line should never be reached.
    for (;;);
    return 0;
}

/*------------------------- Hardware Setup ------------------------------------*/

static void prvSetupHardware(void) {
    // This is a placeholder for your board-specific hardware initialization.
    // For example, setting up the system clock, UART for printf, etc.
    // launch_comm();
    // init_hw();
}
