#ifndef NSPRS_PLATFORM_NUCLEO_G070RB_H_
#define NSPRS_PLATFORM_NUCLEO_G070RB_H_

#include "stm32g0xx_hal.h"


#define MCO_Pin GPIO_PIN_0
#define MCO_GPIO_Port GPIOF
#define in_nespresso_Pin GPIO_PIN_0
#define in_nespresso_GPIO_Port GPIOC
#define in_nespresso_EXTI_IRQn EXTI0_1_IRQn
#define in_hot_water_Pin GPIO_PIN_8
#define in_hot_water_GPIO_Port GPIOC
#define in_hot_water_EXTI_IRQn EXTI4_15_IRQn
#define in_corto_Pin GPIO_PIN_2
#define in_corto_GPIO_Port GPIOC
#define in_corto_EXTI_IRQn EXTI2_3_IRQn
#define in_lungo_Pin GPIO_PIN_3
#define in_lungo_GPIO_Port GPIOC
#define in_lungo_EXTI_IRQn EXTI2_3_IRQn
#define LED_GREEN_Pin GPIO_PIN_5
#define LED_GREEN_GPIO_Port GPIOA
//#define out_pmp_Pin GPIO_PIN_13
//#define out_pmp_GPIO_Port GPIOB
//#define out_tb_Pin GPIO_PIN_14
//#define out_tb_GPIO_Port GPIOB
//#define out_tb2_Pin GPIO_PIN_15
//#define out_tb2_GPIO_Port GPIOB
#define in_fm_Pin GPIO_PIN_6
#define in_fm_GPIO_Port GPIOC
#define in_fm_EXTI_IRQn EXTI4_15_IRQn
#define in_line_Pin GPIO_PIN_7
#define in_line_GPIO_Port GPIOC
#define in_line_EXTI_IRQn EXTI4_15_IRQn
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
//===== Temp =====
#define TBL_NTC_MAX_ADC_VALUE   1006
#define TBL_NTC_MIN_ADC_VALUE    281  //460  // Adaption auf 127500 (127,5∞C)KRB
#define TBL_NTC_MIN_TEMP    	0 //∞C (*100)
#define TBL_NTC_MAX_TEMP    	12750 // 10200 //∞C (*100)
#define TBL_NTC_TABLE_STEP  	50 // 40 // Adaption auf 127500 (127,5∞C)KRB
#define TBL_NTC_maxtableindex	255
#define TBL_NTC_s32_MinPhyVal	500		/*!< Minimal physical value multiply by u16_ScaleValue*/
#define TBL_NTC_s32_MaxPhyVal	12500	/*!< Maximal physical value multiply by u16_ScaleValue*/
#define TBL_NTC_u16_ScaleValue	100		/*!< Scale value. s32_PhyicalValue = (exact value)*u16_ScaleValue */



extern const uint16_t __RR_NTC_TABLE[256];






char  UART_rxChar;
ADC_HandleTypeDef hadc1;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim7;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
//
////=== hw initialization ===
void _platform_init_hw(void);

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM6_Init(void);
static void MX_TIM7_Init(void);
static void MX_ADC1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
void Error_Handler(void);

void EXTI0_1_IRQHandler(void);
void EXTI2_3_IRQHandler(void);
void EXTI4_15_IRQHandler(void);
//
//=== timers ===
unsigned int _platform_timer_readFreqKHz(int timer_id);
unsigned int _platform_timer_readPeriodMs(int timer_id);
unsigned int _platform_timer_readPeriodUs(int timer_id);
unsigned int _platform_timer_readMaxMs(int timer_id);
unsigned int _platform_timer_readMaxUs(int timer_id);
unsigned int _platform_timer_readMax(int timer_id);
unsigned int _platform_timer_readMs(int timer_id);
unsigned int _platform_timer_readUs(int timer_id);
void _platform_timer_stopReset(int timer_id);
void _platform_timer_count(int timer_id, unsigned int count_val);

//=== adc ===
int _platform_readTemp(void);
static int adc2temp(unsigned int adc_val);

//==== gpio
//void _platform_setPump1(void);
//void _platform_resetPump1(void);
//void _platform_setTB1(void);
//void _platform_resetTB1(void);
//void _platform_setTB2(void);
//void _platform_resetTB2(void);

//======================
//==== temperature
//======================
static uint8_t _NTCTableLookUp(uint16_t value);



#endif //NSPRS_PLATFORM_ARMCM0_NUCLEO_F070RB_H_
