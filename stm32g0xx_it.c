/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32g0xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32g0xx_it.h"
#include "FreeRTOS.h"
#include "task.h"
#include "devices_platform_nucleo_g070rb.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim14;
extern TIM_HandleTypeDef htim16;
extern TIM_HandleTypeDef htim17;
extern UART_HandleTypeDef huart3;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M0+ Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
#if (INCLUDE_xTaskGetSchedulerState == 1 )
  if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
  {
#endif /* INCLUDE_xTaskGetSchedulerState */
  xPortSysTickHandler();
#if (INCLUDE_xTaskGetSchedulerState == 1 )
  }
#endif /* INCLUDE_xTaskGetSchedulerState */
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32G0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32g0xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line 0 and line 1 interrupts.
  */
//void EXTI0_1_IRQHandler(void)
//{
//  /* USER CODE BEGIN EXTI0_1_IRQn 0 */
//
//  /* USER CODE END EXTI0_1_IRQn 0 */
//  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
//  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
//  /* USER CODE BEGIN EXTI0_1_IRQn 1 */
//
//  /* USER CODE END EXTI0_1_IRQn 1 */
//}

/**
  * @brief This function handles EXTI line 2 and line 3 interrupts.
  */
//void EXTI2_3_IRQHandler(void)
//{
//  /* USER CODE BEGIN EXTI2_3_IRQn 0 */
//
//  /* USER CODE END EXTI2_3_IRQn 0 */
//  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
//  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
//  /* USER CODE BEGIN EXTI2_3_IRQn 1 */
//
//  /* USER CODE END EXTI2_3_IRQn 1 */
//}

/**
  * @brief This function handles EXTI line 4 to 15 interrupts.
  */
//void EXTI4_15_IRQHandler(void)
//{
//  /* USER CODE BEGIN EXTI4_15_IRQn 0 */
//
//  /* USER CODE END EXTI4_15_IRQn 0 */
//	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);
//	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);
//  /* USER CODE BEGIN EXTI4_15_IRQn 1 */
//
//  /* USER CODE END EXTI4_15_IRQn 1 */
//}

/**
  * @brief This function handles TIM6 global interrupt.
  */
void TIM6_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim6);
}

/**
  * @brief This function handles TIM7 global interrupt.
  */
void TIM7_IRQHandler(void)
{
  /* USER CODE BEGIN TIM7_IRQn 0 */

  /* USER CODE END TIM7_IRQn 0 */
  HAL_TIM_IRQHandler(&htim7);
  /* USER CODE BEGIN TIM7_IRQn 1 */

  /* USER CODE END TIM7_IRQn 1 */
}

/**
  * @brief This function handles TIM14 global interrupt.
  */
//void TIM14_IRQHandler(void)
//{
//  /* USER CODE BEGIN TIM14_IRQn 0 */
//
//  /* USER CODE END TIM14_IRQn 0 */
//  HAL_TIM_IRQHandler(&htim14);
//  /* USER CODE BEGIN TIM14_IRQn 1 */
//
//  /* USER CODE END TIM14_IRQn 1 */
//}

/**
  * @brief This function handles TIM16 global interrupt.
  */
//void TIM16_IRQHandler(void)
//{
//  /* USER CODE BEGIN TIM16_IRQn 0 */
//
//  /* USER CODE END TIM16_IRQn 0 */
//  HAL_TIM_IRQHandler(&htim16);
//  /* USER CODE BEGIN TIM16_IRQn 1 */
//
//  /* USER CODE END TIM16_IRQn 1 */
//}

/**
  * @brief This function handles TIM17 global interrupt.
  */
//void TIM17_IRQHandler(void)
//{
//  /* USER CODE BEGIN TIM17_IRQn 0 */
//
//  /* USER CODE END TIM17_IRQn 0 */
//  HAL_TIM_IRQHandler(&htim17);
//  /* USER CODE BEGIN TIM17_IRQn 1 */
//
//  /* USER CODE END TIM17_IRQn 1 */
//}

/**
  * @brief This function handles USART3 and USART4 interrupts.
  */
void USART3_4_IRQHandler(void)
{
  /* USER CODE BEGIN USART3_4_IRQn 0 */

  /* USER CODE END USART3_4_IRQn 0 */
  HAL_UART_IRQHandler(&huart3);
  /* USER CODE BEGIN USART3_4_IRQn 1 */

  /* USER CODE END USART3_4_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
