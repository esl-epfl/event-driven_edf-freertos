#include "nsprs_platform_nucleo_g070rb.h"
#include "nsprs_params.h"
#include "nsprs_common.h"
#include "nsprs_comm.h"

#include "nsprs_sys_srvc.h"

//#include "retarget.h"
//#include "main.h"

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
 set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

extern TaskHandle_t task1_handler; //todo: remove it :)



const uint16_t __RR_NTC_TABLE[256]=
{
		1006,1006,1004,1004,1003,1003,1002,1002,1001,1000,1000,999,998,998,997,996,/*0 ∞C*/
		996,995,994,993,992,992,991,990,989,988,987,986,985,984,983,982,
		981,980,979,978,977,976,975,973,972,971,970,968,967,966,964,963,
		962,960,959,957,956,954,953,951,950,948,946,945,943,941,939,937,
		936,934,932,930,928,926,924,922,920,918,916,914,911,909,907,905,
		902,900,897,895,893,890,888,885,883,880,877,875,872,869,866,864,
		861,858,855,852,849,846,843,840,837,834,831,828,825,821,818,815,
		812,808,805,801,798,795,791,788,784,781,777,773,770,766,763,759,
		755,751,748,744,740,736,732,729,725,721,717,713,709,705,701,697,
		693,689,685,681,677,673,669,665,661,657,652,648,644,640,636,632,
		628,623,619,615,611,607,603,598,594,590,586,582,578,573,569,565,
		561,557,553,549,544,540,536,532,528,524,520,516,512,508,504,500,
		496,492,488,484,480,476,472,468,464,460,456,452,449,445,441,437,
		433,430,426,422,419,415,411,408,404,400,397,393,390,386,383,379,
		376,373,369,366,362,359,356,353,349,346,343,340,336,333,330,327,
		324,321,318,315,312,309,306,303,300,298,295,292,289,286,284,281 /*bis 127,5 ∞C*/
};


//======================================================
//============= ADC ====================================
//======================================================
//adc functions
int _platform_readTemp(void){
	static unsigned int adc_raw1 = 1000;
	static unsigned int adc_raw2 = 1000;
	static unsigned int adc_raw3 = 1000;
	static unsigned int adc_raw4 = 1000;
	static unsigned int adc_raw5 = 1000;
	static unsigned int adc_raw_avg;
	int temp;

	adc_raw5 = adc_raw4;
	adc_raw4 = adc_raw3;
	adc_raw3 = adc_raw2;
	adc_raw2 = adc_raw1;

	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
	adc_raw1 = HAL_ADC_GetValue(&hadc1);

	adc_raw_avg = (adc_raw1 + adc_raw2 + adc_raw3 + adc_raw4 + adc_raw5)/5;
	temp = adc2temp(adc_raw_avg);
	return temp;

}
static int adc2temp(unsigned int adc_val){
	uint16_t temp;

	uint16_t value = adc_val;
	//calc index
	uint8_t Index;
	Index = _NTCTableLookUp(value);
	//calc temperature
	int16_t uADCTemperature;
	uADCTemperature = TBL_NTC_MIN_TEMP+((int16_t)Index*TBL_NTC_TABLE_STEP);
	//adjust it
	int32_t s32_PhyVal;
	s32_PhyVal = (int32_t)uADCTemperature;
	s32_PhyVal += (TBL_NTC_u16_ScaleValue>>2);
	temp = ( ( (uint16_t) s32_PhyVal )/(TBL_NTC_u16_ScaleValue>>1));
	temp = temp >> 1;	//because on the previous lines, it change it to 2*temp

	return temp;
}

static uint8_t _NTCTableLookUp(uint16_t value)
{
	uint16_t index = 0;/* variable to hold the index */
	uint16_t i=0,j=0;  /* lower and upper array bounds */
	uint16_t w1,w2;


	/* we must define a local pointer, otherwise it is impossible for the C18 to perform */
	/* pointer arithmetik. The result would be incorrect                                 */
	const uint16_t * ptr_u16data = __RR_NTC_TABLE;

	index=TBL_NTC_maxtableindex/2;
	j = TBL_NTC_maxtableindex;

	while ((i <= j)&&(0 < index)&&(TBL_NTC_maxtableindex > index))
	{
	   w2=*(ptr_u16data+index);
	   w1=*(ptr_u16data+index+1);
	   if ((value >=w1) && (value <=w2))
	   {
		  break;
	   }


	   if (value < w1)
	   {
		  i = index + 1;
	   }
	   else
	   {
		  j = index - 1;
	   }
	   index=(i+j)/2;
	}
     return (uint8_t)(index);
}

//======================================================
//============= Timers ==============================
//======================================================
unsigned int _platform_timer_readFreqKHz(timer_id){
	unsigned int freq_khz;
	if (timer_id == 1){
		freq_khz = 100;	//todo: wrong, its 0.1
	}
	else if(timer_id == 2){
		freq_khz = 1;
	}
	return freq_khz;
}
unsigned int _platform_timer_readPeriodMs(int timer_id){
	unsigned int period_ms;
	if (timer_id == 1){
		period_ms = 1;	//todo: wrong, its 0.1
	}
	else if(timer_id == 2){
		period_ms = 1;
	}
	return period_ms;
}
unsigned int _platform_timer_readPeriodUs(int timer_id){
	return 1000/(_platform_timer_readFreqKHz(timer_id));
}


unsigned int _platform_timer_readMaxMs(int timer_id){
	unsigned int MaxMs;
	if (timer_id == 1){
		MaxMs = 6553;
	}
	else if(timer_id == 2){
		MaxMs = 65535;
	}
	return MaxMs;
}
unsigned int _platform_timer_readMaxUs(int timer_id){
	unsigned int MaxUs;
	if (timer_id == 1){
		MaxUs = 6553500;
	}
	else if(timer_id == 2){
		MaxUs = 65535000;
	}
	return MaxUs;
}

unsigned int _platform_timer_readMax(int timer_id){
	unsigned int Max;
	if (timer_id == 1){
		Max = 65535;
	}
	else if(timer_id == 2){
		Max = 65535;
	}
	return Max;
}

unsigned int _platform_timer_readMs(int timer_id){
	unsigned int CR;
	unsigned int time_ms;
	if (timer_id == 1){
		CR = TIM6->CNT;
	}
	else if(timer_id == 2){
		CR = TIM7->CNT;
	}
	time_ms = CR * (_platform_timer_readPeriodUs(timer_id)/1000);
	return time_ms;
}
unsigned int _platform_timer_readUs(int timer_id){
	unsigned int CR;
	unsigned int time_us;
	if (timer_id == 1){
		CR = TIM6->CNT;
	}
	else if(timer_id == 2){
		CR = TIM7->CNT;
	}
	time_us = CR * (_platform_timer_readPeriodUs(timer_id));
	return time_us;
}

void _platform_timer_stopReset(int timer_id){
	if (timer_id == 1){
		TIM6->CNT = 0; 				//reset
		HAL_TIM_Base_Stop_IT(&htim6); //stop
		__TIM6_CLK_DISABLE();

	}
	else if(timer_id == 2){
		TIM7->CNT = 0; 				//reset
		HAL_TIM_Base_Stop_IT(&htim7); //stop
		__TIM7_CLK_DISABLE();
	}
}

void _platform_timer_count(int timer_id, unsigned int count_val){ //todo: change higher level function, put all the conversions here in low level :)
	if (timer_id == 1){
		__TIM6_CLK_ENABLE();
		TIM6->CNT = 0;	//reset
		TIM6->ARR = count_val; //set it up
		HAL_TIM_Base_Start_IT(&htim6); //start it
	}
	else if(timer_id == 2){
		__TIM7_CLK_ENABLE();
		TIM7->CNT = 0;	//reset
		TIM7->ARR = count_val; //set it up
		HAL_TIM_Base_Start_IT(&htim7); //start it
	}
}



//======================================================
//============= Interrupt Callbacks ====================
//======================================================

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
// if (htim->Instance == TIM1) {
//   HAL_IncTick();
// }
 if (htim->Instance == TIM6){
	 if (data_timer.en == 1){		//now it should check before putting in ready
		 BaseType_t xYieldRequired = xTaskResumeFromISR(ssTimer_task_handler);	//changed
		 portYIELD_FROM_ISR(xYieldRequired);
	 }
 }
 if (htim->Instance == TIM7){
	 xSemaphoreGive(intr2ssTimer2_sh);
 }
}



void EXTI0_1_IRQHandler(void) // interrupt routine
{
	//TODO: I should put xYieldRequired check for everything
	//Nespresso
	char p_data;
	  if ((EXTI->RPR1 & (in_nespresso_Pin)) != 0x00u){
		EXTI->RPR1 = (in_nespresso_Pin);
		// printf("nsprs release\r\n");
		if (data_hmi.en == 1){
			data_hmi.data_from_isr = 1<<NSPRS_RELEASED;
			xTaskResumeFromISR(ssHMI_b_nsprs_task_handler);
		}
	  }
	  else if ((EXTI->FPR1 & (in_nespresso_Pin)) != 0x00u){
		EXTI->FPR1 = (in_nespresso_Pin);
		// printf("nsprs press\r\n");
		if (data_hmi.en == 1){
			data_hmi.data_from_isr = 1<<NSPRS_PRESSED;
			xTaskResumeFromISR(ssHMI_b_nsprs_task_handler);
		}
	  }


}

void EXTI2_3_IRQHandler(void) // interrupt routine
{
	//espresso
	char p_data;
	  if ((EXTI->RPR1 & (in_corto_Pin)) != 0x00u){
		EXTI->RPR1 = (in_corto_Pin);
		// printf("esprs r\r\n");
		if (data_hmi.en == 1){
			data_hmi.data_from_isr = 1<<ESPRS_RELEASED;
			xTaskResumeFromISR(ssHMI_b_esprs_task_handler);
		}

	  }
	  else if ((EXTI->FPR1 & (in_corto_Pin)) != 0x00u){
		EXTI->FPR1 = (in_corto_Pin);
		// printf("esprs p\r\n");
		if (data_hmi.en == 1){
			data_hmi.data_from_isr = 1<<ESPRS_PRESSED;
			xTaskResumeFromISR(ssHMI_b_esprs_task_handler);
		}
	  }
//LUNGO
	  if ((EXTI->RPR1 & (in_lungo_Pin)) != 0x00u){
		EXTI->RPR1 = (in_lungo_Pin);
		// printf("lungo r\r\n");
		if (data_hmi.en == 1){
			data_hmi.data_from_isr = 1<<LUNGO_RELEASED;
			xTaskResumeFromISR(ssHMI_b_lungo_task_handler);
		}
	  }
	  else if ((EXTI->FPR1 & (in_lungo_Pin)) != 0x00u){
		EXTI->FPR1 = (in_lungo_Pin);
		// printf("lungo p\r\n");
		if (data_hmi.en == 1){
			data_hmi.data_from_isr = 1<<LUNGO_PRESSED;
			xTaskResumeFromISR(ssHMI_b_lungo_task_handler);
		}
	  }

}

void EXTI4_15_IRQHandler(void) // interrupt routine
{
	char p_data;
	  if ((EXTI->RPR1 & (in_line_Pin)) != 0x00u){
		EXTI->RPR1 = (in_line_Pin);
		// p_data = 1;
		// xQueueOverwrite(intr2ssTrgPump_qh, &(p_data));
		// xQueueOverwrite(intr2ssTrgHeater_qh, &(p_data));
		printf("zc: rise\r\n");
		if(data_trgpump.en == 1){
			data_trgpump.zc_edge = 1;
			xTaskResumeFromISR(ssTrgPump_task_handler);
		}
		printf("zc: rise after pump\r\n");
		if(data_trgheater.en == 1){
			data_trgheater.zc_edge = 1;
			xTaskResumeFromISR(ssTrgHeater_task_handler);
		}
		printf("zc: rise after heater\r\n");

	  }
	  else if ((EXTI->FPR1 & (in_line_Pin)) != 0x00u){
		EXTI->FPR1 = (in_line_Pin);
		// p_data = 0;
		printf("zc: fall\r\n");
		if(data_trgpump.en == 1){
			data_trgpump.zc_edge = 0;
			xTaskResumeFromISR(ssTrgPump_task_handler);
		}
		printf("zc: fall after pump\r\n");
		if(data_trgheater.en == 1){
			data_trgheater.zc_edge = 0;
			xTaskResumeFromISR(ssTrgHeater_task_handler);
		}
		printf("zc: fall after heater\r\n");
	  }


	  if ((EXTI->RPR1 & (in_fm_Pin)) != 0x00u){
		EXTI->RPR1 = (in_fm_Pin);
		if (data_flow.en == 1){		//now it should check before putting in ready
			xTaskResumeFromISR(ssFlow_task_handler); //p_data = 1; xQueueOverwrite(intr2ssFlow_qh, &(p_data));	//changed
		}
	  }
	  else if ((EXTI->FPR1 & (in_fm_Pin)) != 0x00u){
		EXTI->FPR1 = (in_fm_Pin);
		p_data = 0;
//		xQueueOverwrite(intr2ssFlow_qh, &(p_data));
	  }

	  if ((EXTI->RPR1 & (in_hot_water_Pin)) != 0x00u){
		EXTI->RPR1 = (in_hot_water_Pin);
		// printf("hw r\r\n");
		if (data_hmi.en == 1){
			data_hmi.data_from_isr = 1<<HW_RELEASED;
			xTaskResumeFromISR(ssHMI_b_hw_task_handler);
		}
	  }

	  else if ((EXTI->FPR1 & (in_hot_water_Pin)) != 0x00u){
		EXTI->FPR1 = (in_hot_water_Pin);
		// printf("hw p\r\n");
		if (data_hmi.en == 1){
			data_hmi.data_from_isr = 1<<HW_PRESSED;
			xTaskResumeFromISR(ssHMI_b_hw_task_handler);
		}
	  }

}

//======================================================
//============= HW Initialization ======================
//======================================================

void _platform_init_hw(void){
	 HAL_Init();
	 SystemClock_Config();
	  MX_GPIO_Init();
	  MX_TIM6_Init();
	  MX_TIM7_Init();
	  MX_ADC1_Init();
	  MX_USART1_UART_Init();
	  MX_USART2_UART_Init();
	  MX_USART3_UART_Init();
//	  RetargetInit(&huart2);
//	  HAL_UART_Receive_IT (&huart3, &UART_rxChar, 1); //todo: remove?!

}


static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET);


  GPIO_InitStruct.Pin = in_nespresso_Pin|in_corto_Pin|in_lungo_Pin|in_hot_water_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.Pin =  in_fm_Pin|in_line_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : LED_GREEN_Pin */
  GPIO_InitStruct.Pin = LED_GREEN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(LED_GREEN_GPIO_Port, &GPIO_InitStruct);


  NVIC_SetPriority(EXTI4_15_IRQn, 3);
  NVIC_SetVector(EXTI4_15_IRQn, (uint32_t)&EXTI4_15_IRQHandler);
  NVIC_EnableIRQ(EXTI4_15_IRQn);

  NVIC_SetPriority(EXTI2_3_IRQn, 3);
  NVIC_SetVector(EXTI2_3_IRQn, (uint32_t)&EXTI2_3_IRQHandler);
  NVIC_EnableIRQ(EXTI2_3_IRQn);

  NVIC_SetPriority(EXTI0_1_IRQn, 3);
  NVIC_SetVector(EXTI0_1_IRQn, (uint32_t)&EXTI0_1_IRQHandler);
  NVIC_EnableIRQ(EXTI0_1_IRQn);

}


void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the peripherals clocks
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2
                              |RCC_PERIPHCLK_ADC;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_SYSCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}



static void MX_USART1_UART_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }


}
static void MX_USART2_UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
  {
    Error_Handler();
  }

}
static void MX_USART3_UART_Init(void)
{
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }

}



static void MX_TIM6_Init(void)
{
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 159;		//100Khz
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 65535;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

}

static void MX_TIM7_Init(void)
{
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 15999;		//1KHz
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 65535;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

}


static void MX_ADC1_Init(void)
{

  ADC_ChannelConfTypeDef sConfig = {0};

  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_10B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.LowPowerAutoPowerOff = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_1CYCLE_5;
  hadc1.Init.SamplingTimeCommon2 = ADC_SAMPLETIME_1CYCLE_5;
  hadc1.Init.OversamplingMode = DISABLE;
  hadc1.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_HIGH;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

}

PUTCHAR_PROTOTYPE
{
 /* Place your implementation of fputc here */
 /* e.g. write a character to the USART2 and Loop until the end of transmission */
 HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);

return ch;
}



void Error_Handler(void)
{
	__disable_irq();
	while (1);
}


