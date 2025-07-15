#include "stm32_trg_driver.h"
//#include "main.h"
//#include "devices_platform_nucleo_g070rb.h"
#include "stm32g0xx_hal.h"

void trg_init_tmr16(void){
	//clk enable
	  __HAL_RCC_TIM16_CLK_ENABLE();
	  __HAL_RCC_GPIOA_CLK_ENABLE();
	// TIM16 GPIO Configuration, PA6     ------> TIM16_CH1 */
	  GPIO_InitTypeDef GPIO_InitStruct = {0};
	  GPIO_InitStruct.Pin = GPIO_PIN_6;
	  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  GPIO_InitStruct.Alternate = GPIO_AF5_TIM16;
	  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	  //reg initialization
	  TIM16->CR1 &= ~TIM_CR1_CEN; //disable timer
	  TIM16->CR1 &= ~TIM_CR1_DIR; //up counter
	   TIM16->CR1 |= TIM_CR1_URS; //1: Only counter overflow/underflow generates an update interrupt or DMA request if enabled. (not setting UG bit)
//	   TIM16->CR1 |= TIM_CR1_UDIS;
	   TIM16->RCR = 0; //Repetition counter value
	   TIM16->CR1 &= ~TIM_CR1_ARPE; //AutoReloadPreload disabled
//	   TIM16->SR &= ~TIM_SR_UIF;
	     TIM16->DIER |= TIM_DIER_UIE; // enable update interrupts
	 	TIM16->BDTR |= (TIM_BDTR_MOE); //1: OC and OCN outputs are enabled if their respective enable bits are set (CCxE, CCxNE in TIMx_CCER register)
	   TIM16->CCMR1 &= ~TIM_CCMR1_CC1S_0;  //configuring as output
	   TIM16->CCMR1 &= ~TIM_CCMR1_CC1S_1;
	   TIM16->CCER |= TIM_CCER_CC1E; //Output enabled
	   TIM16->CCER &= ~TIM_CCER_CC1P; //0: OC1 active high
	   TIM16->PSC = 15; 	//resolution in 1us
//	   TIM16->CCER &= ~TIM_CCER_CC1NP;
	   TIM16->EGR |= TIM_EGR_UG;

	   NVIC_SetPriority(TIM16_IRQn, 3);
	   NVIC_SetVector(TIM16_IRQn, (uint32_t)&TIM16_IRQHandler);
}



void trg_impl_tmr16(int mode, unsigned int t1, unsigned int t2, unsigned int l_pwm, unsigned int h_pwm){

	mode_tmr16 = mode; //save to be used in isr
	 t1_tmr16 = t1;
	 t2_tmr16 = t2;
	 l_pwm_tmr16 = l_pwm;
	 h_pwm_tmr16 = h_pwm;
	num_pulse_pwm_tmr16 = 0; //initialize in the required mode

//#define 				0
//#define 				1
//#define 			2
//#define 			3
//#define TRG_MODE_T1LOW_T2HIGH_LOW	4
//#define TRG_MODE_T1HIGH_T2LOW_HIGH	5
//#define TRG_MODE_T1PWM_LOW			6
//#define TRG_MODE_T1PWM_HIGH			7
//#define TRG_MODE_T1PWM_T2HIGH_LOW	8
//#define TRG_MODE_T1PWM_T2LOW_HIGH	9
//#define TRG_MODE_T1HIGH_T2PWM_LOW	10
//#define TRG_MODE_T1LOW_T2PWM_HIGH	11


	if (mode_tmr16 == TRG_MODE_LOW){
		//==put low
//		   TIM16->PSC = 15999;
		   // shart bezaram agar az 1000 bozorg tar bud ms esh bokonam (ya bar asase pwm hast ya na)
		     TIM16->ARR = 1000;
		   // channel register
		   TIM16->CCR1 = 200;
		   //put low
		 	TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_0;
		 	TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_1;
		 	TIM16->CCMR1 |= TIM_CCMR1_OC1M_2;
		 	TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_3;
		 	// update generation: Re-initialize the counter and generates an update of the registers.

		   TIM16->EGR |= TIM_EGR_UG;
		   TIM16->CR1 |= TIM_CR1_CEN;

	}
	else if(mode_tmr16 == TRG_MODE_HIGH){
		//==put high
	 	TIM16->CCMR1 |= TIM_CCMR1_OC1M_0;
	 	TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_1;
	 	TIM16->CCMR1 |= TIM_CCMR1_OC1M_2;
	 	TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_3;

	   TIM16->EGR |= TIM_EGR_UG;
	   TIM16->CR1 |= TIM_CR1_CEN;
	}
	else if (mode_tmr16 == TRG_MODE_T1HIGH_LOW){
		//==put high for t1, then put low
		TIM16->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
		TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_1;
		TIM16->CCMR1 |= TIM_CCMR1_OC1M_2;
		TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_3;

//		TIM16->PSC = 15999; //us resolution
		TIM16->ARR = (uint32_t)t1_tmr16;
		TIM16->EGR |= TIM_EGR_UG;
	   TIM16->CR1 |= TIM_CR1_CEN;

	}
	else if (mode_tmr16 == TRG_MODE_T1LOW_HIGH){
		//==put low for t1, then put high
		//put low
		TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_0;
		TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_1;
		TIM16->CCMR1 |= TIM_CCMR1_OC1M_2;
		TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_3;

//		TIM16->PSC = 15999; //us resolution
		TIM16->ARR = (uint32_t)t1_tmr16;
		TIM16->EGR |= TIM_EGR_UG;
	   TIM16->CR1 |= TIM_CR1_CEN;
	}
	else if (mode_tmr16 == 4){
		//==put low for t1, then put high for t2, then put low
		//put low
		TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_0;
		TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_1;
		TIM16->CCMR1 |= TIM_CCMR1_OC1M_2;
		TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_3;

//		TIM16->PSC = 15999; //us resolution
		TIM16->ARR = (uint32_t)t1_tmr16;
		TIM16->EGR |= TIM_EGR_UG;
	   TIM16->CR1 |= TIM_CR1_CEN;
	}
	else if (mode_tmr16 == 5){
		//==put high for t1, then put low for t2, then put high
		TIM16->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
		TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_1;
		TIM16->CCMR1 |= TIM_CCMR1_OC1M_2;
		TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_3;

//		TIM16->PSC = 15999; //us resolution
		TIM16->ARR = (uint32_t)t1_tmr16;
		TIM16->EGR |= TIM_EGR_UG;
	   TIM16->CR1 |= TIM_CR1_CEN;
	}
	else if (mode_tmr16 == 6){
		//==start pwm for t1 (number of pulses should calculated), then put low
		num_pulse_pwm_tmr16 = (t1)/(l_pwm_tmr16+h_pwm_tmr16);

//		TIM16->PSC = 15999; //us resolution
		TIM16->ARR = (uint32_t)(h_pwm_tmr16+l_pwm_tmr16);
	  TIM16->CCR1 = (uint32_t)h_pwm_tmr16;

	TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_0; //pwm1
	TIM16->CCMR1 |= TIM_CCMR1_OC1M_1;
	TIM16->CCMR1 |= TIM_CCMR1_OC1M_2;
	TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_3;

	TIM16->EGR |= TIM_EGR_UG;
   TIM16->CR1 |= TIM_CR1_CEN;


	}
	else if (mode_tmr16 == 7){
		//==start pwm for t1, then put high
		num_pulse_pwm_tmr16 = (t1)/(l_pwm_tmr16+h_pwm_tmr16);

//		TIM16->PSC = 15999; //us resolution
		TIM16->ARR = (uint32_t)(h_pwm_tmr16+l_pwm_tmr16);
	  TIM16->CCR1 = (uint32_t)h_pwm_tmr16;

		TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_0; //pwm1
		TIM16->CCMR1 |= TIM_CCMR1_OC1M_1;
		TIM16->CCMR1 |= TIM_CCMR1_OC1M_2;
		TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_3;

		TIM16->EGR |= TIM_EGR_UG;
	   TIM16->CR1 |= TIM_CR1_CEN;
	}
	else if (mode_tmr16 == 8){
		//==start pwm for t1, then put high for t2, then put low
		num_pulse_pwm_tmr16 = (t1)/(l_pwm_tmr16+h_pwm_tmr16);

//		TIM16->PSC = 15999; //us resolution
		TIM16->ARR = (uint32_t)(h_pwm_tmr16+l_pwm_tmr16);
	  TIM16->CCR1 = (uint32_t)h_pwm_tmr16;

		TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_0; //pwm1
		TIM16->CCMR1 |= TIM_CCMR1_OC1M_1;
		TIM16->CCMR1 |= TIM_CCMR1_OC1M_2;
		TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_3;

		TIM16->EGR |= TIM_EGR_UG;
	   TIM16->CR1 |= TIM_CR1_CEN;

	}
	else if (mode_tmr16 == 9){
		//==start pwm for t1, then put low for t2, then put high
		num_pulse_pwm_tmr16 = (t1)/(l_pwm_tmr16+h_pwm_tmr16);

//		TIM16->PSC = 15999; //us resolution
		TIM16->ARR = (uint32_t)(h_pwm_tmr16+l_pwm_tmr16);
	  TIM16->CCR1 = (uint32_t)h_pwm_tmr16;

		TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_0; //pwm1
		TIM16->CCMR1 |= TIM_CCMR1_OC1M_1;
		TIM16->CCMR1 |= TIM_CCMR1_OC1M_2;
		TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_3;

		TIM16->EGR |= TIM_EGR_UG;
	   TIM16->CR1 |= TIM_CR1_CEN;
	}
	else if (mode_tmr16 == 10){
		//==put high for t1, then start pwm for t2, then put low
		num_pulse_pwm_tmr16 = (t2)/(l_pwm_tmr16+h_pwm_tmr16);

		TIM16->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
		TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_1;
		TIM16->CCMR1 |= TIM_CCMR1_OC1M_2;
		TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_3;

//		TIM16->PSC = 15999; //us resolution
		TIM16->ARR = (uint32_t)t1_tmr16;
		TIM16->EGR |= TIM_EGR_UG;
	   TIM16->CR1 |= TIM_CR1_CEN;

	}
	else if (mode_tmr16 == 11){
		//==put low for t1, then start pwm for t2, then put high
		num_pulse_pwm_tmr16 = (t2)/(l_pwm_tmr16+h_pwm_tmr16);

		//put low
		TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_0;
		TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_1;
		TIM16->CCMR1 |= TIM_CCMR1_OC1M_2;
		TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_3;

//		TIM16->PSC = 15999; //us resolution
		TIM16->ARR = (uint32_t)t1_tmr16;
		TIM16->EGR |= TIM_EGR_UG;
	   TIM16->CR1 |= TIM_CR1_CEN;
	}


	TIM16->SR &= ~TIM_SR_UIF;
   NVIC_EnableIRQ(TIM16_IRQn); // Enable interrupt from TIM3 (NVIC level)
}


void TIM16_IRQHandler(void) // interrupt routine
{
	  if(TIM16->SR & TIM_SR_UIF) // if UIF flag is set
	  {
	    TIM16->SR &= ~TIM_SR_UIF; // clear UIF flag


		if (mode_tmr16 == TRG_MODE_LOW){
			//do nothing
		}
		else if(mode_tmr16 == TRG_MODE_HIGH){
			//do nothing
		}
		else if (mode_tmr16 == TRG_MODE_T1HIGH_LOW){
			//==when recieve intr(t1 done), put low
			//put low
		 	TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_0;
		 	TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_1;
		 	TIM16->CCMR1 |= TIM_CCMR1_OC1M_2;
		 	TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_3;
		   TIM16->EGR |= TIM_EGR_UG;
		   TIM16->CR1 &= ~TIM_CR1_CEN; //disable

		}
		else if (mode_tmr16 == TRG_MODE_T1LOW_HIGH){
			//==when recieve put high
			TIM16->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
			TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_1;
			TIM16->CCMR1 |= TIM_CCMR1_OC1M_2;
			TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_3;

			TIM16->EGR |= TIM_EGR_UG;
		   TIM16->CR1 &= ~TIM_CR1_CEN; //disable
		}
		else if (mode_tmr16 == 4){
			//==put low for t1, then put high for t2, then put low
			static int mode4_cntr = 0;
			//first recieve: put high, set for t2
			//2nd rec: put low
			if (mode4_cntr == 0){
				TIM16->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
				TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_1;
				TIM16->CCMR1 |= TIM_CCMR1_OC1M_2;
				TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_3;

				TIM16->ARR = (uint32_t)t2_tmr16;
				TIM16->EGR |= TIM_EGR_UG;
			   TIM16->CR1 |= TIM_CR1_CEN;

			   mode4_cntr++;
			}
			else{
				//put low
				TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_0;
				TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_1;
				TIM16->CCMR1 |= TIM_CCMR1_OC1M_2;
				TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_3;
			   TIM16->EGR |= TIM_EGR_UG;
			   TIM16->CR1 &= ~TIM_CR1_CEN; //disable
				mode4_cntr = 0;
			}
		}
		else if (mode_tmr16 == 5){
			//==put high for t1, then put low for t2, then put high
			static int mode5_cntr = 0;
			//1st rec: put low, set for t2
			//2nd rec: put high
			if (mode5_cntr == 0){
				//put low
				TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_0;
				TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_1;
				TIM16->CCMR1 |= TIM_CCMR1_OC1M_2;
				TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_3;

				TIM16->ARR = (uint32_t)t2_tmr16;
				TIM16->EGR |= TIM_EGR_UG;
			   TIM16->CR1 |= TIM_CR1_CEN;

			   mode5_cntr++;
			}
			else{

				TIM16->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
				TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_1;
				TIM16->CCMR1 |= TIM_CCMR1_OC1M_2;
				TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_3;
			   TIM16->EGR |= TIM_EGR_UG;
			   TIM16->CR1 &= ~TIM_CR1_CEN; //disable
				mode5_cntr = 0;
			}

		}
		else if (mode_tmr16 == 6){
			//==start pwm for t1 (number of pulses should calculated), then put low
			//count numbers, when reached the value, then put low just
			static int mode6_cntr = 0;
			if(mode6_cntr<num_pulse_pwm_tmr16){
				mode6_cntr++;
			}
			else{
				//put low
				TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_0;
				TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_1;
				TIM16->CCMR1 |= TIM_CCMR1_OC1M_2;
				TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_3;
			   TIM16->EGR |= TIM_EGR_UG;
			   TIM16->CR1 &= ~TIM_CR1_CEN; //disable
				mode6_cntr=0;
			}

		}
		else if (mode_tmr16 == 7){
			//==start pwm for t1, then put high
			//count num of intr, when reached put high
			static int mode7_cntr = 0;
			if(mode7_cntr<num_pulse_pwm_tmr16){
				mode7_cntr++;
			}
			else{
				TIM16->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
				TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_1;
				TIM16->CCMR1 |= TIM_CCMR1_OC1M_2;
				TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_3;
			   TIM16->EGR |= TIM_EGR_UG;
			   TIM16->CR1 &= ~TIM_CR1_CEN; //disable
				mode7_cntr=0;
			}
		}
		else if (mode_tmr16 == 8){
			//==start pwm for t1, then put high for t2, then put low
			//count numbers, when reached put high, set timer for t2, pwm_dis, the intr after that put low
			static int mode8_cntr1 = 0;
			static int mode8_cntr2 = 0;
			if(mode8_cntr1 == 0){ //state 1
				if(mode8_cntr2<num_pulse_pwm_tmr16){ //pwm counter
					mode8_cntr2++;
				}
				else{
					TIM16->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
					TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_1;
					TIM16->CCMR1 |= TIM_CCMR1_OC1M_2;
					TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_3;

					TIM16->ARR = (uint32_t)t2_tmr16;
					TIM16->EGR |= TIM_EGR_UG;
				   TIM16->CR1 |= TIM_CR1_CEN;

				   mode8_cntr2=0;
				   mode8_cntr1++;
				}
			}
			else{	//state 2
				//put low
			 	TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_0;
			 	TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_1;
			 	TIM16->CCMR1 |= TIM_CCMR1_OC1M_2;
			 	TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_3;
			   TIM16->EGR |= TIM_EGR_UG;
			   TIM16->CR1 &= ~TIM_CR1_CEN; //disable
			   mode8_cntr2=0;
			   mode8_cntr1=0;
			}

		}
		else if (mode_tmr16 == 9){
			//==start pwm for t1, then put low for t2, then put high
			//count numbers, when reached put low, set timer for t2, pwm_dis, the intr after that put high
			static int mode9_cntr1 = 0;
			static int mode9_cntr2 = 0;
			if(mode9_cntr1 == 0){ //state 1
				if(mode9_cntr2<num_pulse_pwm_tmr16){ //pwm counter
					mode9_cntr2++;
				}
				else{
					//put low
				 	TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_0;
				 	TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_1;
				 	TIM16->CCMR1 |= TIM_CCMR1_OC1M_2;
				 	TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_3;

					TIM16->ARR = (uint32_t)t2_tmr16;
					TIM16->EGR |= TIM_EGR_UG;
				   TIM16->CR1 |= TIM_CR1_CEN;

				   mode9_cntr2=0;
				   mode9_cntr1++;
				}
			}
			else{	//state 2
				TIM16->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
				TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_1;
				TIM16->CCMR1 |= TIM_CCMR1_OC1M_2;
				TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_3;

			   TIM16->EGR |= TIM_EGR_UG;
			   TIM16->CR1 &= ~TIM_CR1_CEN; //disable
			   mode9_cntr2=0;
			   mode9_cntr1=0;
			}
		}
		else if (mode_tmr16 == 10){
			//==put high for t1, then start pwm for t2, then put low
			//1st: (pwm_disabled) set pwm,
			//count for t2, when done, put low
			static int mode10_cntr1 = 0;
			static int mode10_cntr2 = 0;

			if (mode10_cntr1 == 0){ //state1: t1 finished, set the t2
//				TIM16->PSC = 15999; //1us resolution
				TIM16->ARR = (uint32_t)(h_pwm_tmr16+l_pwm_tmr16);
			  TIM16->CCR1 = (uint32_t)h_pwm_tmr16;

				TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_0; //pwm1
				TIM16->CCMR1 |= TIM_CCMR1_OC1M_1;
				TIM16->CCMR1 |= TIM_CCMR1_OC1M_2;
				TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_3;

				TIM16->EGR |= TIM_EGR_UG;
			   TIM16->CR1 |= TIM_CR1_CEN;

				mode10_cntr1++; //go to next state
				mode10_cntr2 = 0;
			}

			else{ //state2: pwm
				if(mode10_cntr2<num_pulse_pwm_tmr16){ //pwm counter
					mode10_cntr2++;
				}
				else{
					//put low
				 	TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_0;
				 	TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_1;
				 	TIM16->CCMR1 |= TIM_CCMR1_OC1M_2;
				 	TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_3;

//					TIM16->ARR = (uint32_t)t2_tmr16;
					TIM16->EGR |= TIM_EGR_UG;
					TIM16->CR1 &= ~TIM_CR1_CEN; //disable

				   mode10_cntr2=0;
				   mode10_cntr1=0;
				}
			}

		}
		else if (mode_tmr16 == 11){
			//==put low for t1, then start pwm for t2, then put high
			//1st: (pwm_disabled) set pwm,
			//count for t2, when done, put high
			static int mode11_cntr1 = 0;
			static int mode11_cntr2 = 0;

			if (mode11_cntr1 == 0){ //state1: t1 finished, set the t2
//				TIM16->PSC = 15999; //us resolution
				TIM16->ARR = (uint32_t)(h_pwm_tmr16+l_pwm_tmr16);
			  TIM16->CCR1 = (uint32_t)h_pwm_tmr16;

				TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_0; //pwm1
				TIM16->CCMR1 |= TIM_CCMR1_OC1M_1;
				TIM16->CCMR1 |= TIM_CCMR1_OC1M_2;
				TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_3;

				TIM16->EGR |= TIM_EGR_UG;
			   TIM16->CR1 |= TIM_CR1_CEN;

				mode11_cntr1++; //go to next state
				mode11_cntr2 = 0;
			}

			else{ //state2: pwm
				if(mode11_cntr2<num_pulse_pwm_tmr16){ //pwm counter
					mode11_cntr2++;
				}
				else{
					TIM16->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
					TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_1;
					TIM16->CCMR1 |= TIM_CCMR1_OC1M_2;
					TIM16->CCMR1 &= ~TIM_CCMR1_OC1M_3;

//					TIM16->ARR = (uint32_t)t2_tmr16;
					TIM16->EGR |= TIM_EGR_UG;
					TIM16->CR1 &= ~TIM_CR1_CEN; //disable

				   mode11_cntr2=0;
				   mode11_cntr1=0;
				}
			}
		}

	  }
}


//================== TIM17 TRIGGER ====================

void trg_init_tmr17(void){
	//clk enable
	  __HAL_RCC_TIM17_CLK_ENABLE();

	  __HAL_RCC_GPIOA_CLK_ENABLE();
	  /**TIM17 GPIO Configuration, PA7     ------> TIM17_CH1 */
	  GPIO_InitTypeDef GPIO_InitStruct = {0};
	  GPIO_InitStruct.Pin = GPIO_PIN_7;
	  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  GPIO_InitStruct.Alternate = GPIO_AF5_TIM17;
	  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	  //reg initialization
	  TIM17->CR1 &= ~TIM_CR1_CEN; //disable timer
	  TIM17->CR1 &= ~TIM_CR1_DIR; //up counter
	   TIM17->CR1 |= TIM_CR1_URS; //1: Only counter overflow/underflow generates an update interrupt or DMA request if enabled. (not setting UG bit)
//	   TIM17->CR1 |= TIM_CR1_UDIS;
	   TIM17->RCR = 0; //Repetition counter value
	   TIM17->CR1 &= ~TIM_CR1_ARPE; //AutoReloadPreload disabled
//	   TIM17->SR &= ~TIM_SR_UIF;
	     TIM17->DIER |= TIM_DIER_UIE; // enable update interrupts
	 	TIM17->BDTR |= (TIM_BDTR_MOE); //1: OC and OCN outputs are enabled if their respective enable bits are set (CCxE, CCxNE in TIMx_CCER register)
	   TIM17->CCMR1 &= ~TIM_CCMR1_CC1S_0;  //configuring as output
	   TIM17->CCMR1 &= ~TIM_CCMR1_CC1S_1;
	   TIM17->CCER |= TIM_CCER_CC1E; //Output enabled
	   TIM17->CCER &= ~TIM_CCER_CC1P; //0: OC1 active high
//	   TIM17->CCER &= ~TIM_CCER_CC1NP;
	   TIM17->PSC = 15;			//1us resolution
	   TIM17->EGR |= TIM_EGR_UG;

	   NVIC_SetPriority(TIM17_IRQn, 3);
	   NVIC_SetVector(TIM17_IRQn, (uint32_t)&TIM17_IRQHandler);
}



void trg_impl_tmr17(int mode, unsigned int t1, unsigned int t2, unsigned int l_pwm, unsigned int h_pwm){

	mode_tmr17 = mode; //save to be used in isr
	 t1_tmr17 = t1;
	 t2_tmr17 = t2;
	 l_pwm_tmr17 = l_pwm;
	 h_pwm_tmr17 = h_pwm;
	num_pulse_pwm_tmr17 = 0; //initialize in the required mode


	if (mode_tmr17 == TRG_MODE_LOW){
		//==put low
//		   TIM17->PSC = 15999;
		   // shart bezaram agar az 1000 bozorg tar bud ms esh bokonam (ya bar asase pwm hast ya na)
		     TIM17->ARR = 1000;
		   // channel register
		   TIM17->CCR1 = 200;
		   //put low
		 	TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_0;
		 	TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_1;
		 	TIM17->CCMR1 |= TIM_CCMR1_OC1M_2;
		 	TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_3;
		 	// update generation: Re-initialize the counter and generates an update of the registers.

		   TIM17->EGR |= TIM_EGR_UG;
		   TIM17->CR1 |= TIM_CR1_CEN;

	}
	else if(mode_tmr17 == TRG_MODE_HIGH){
		//==put high
	 	TIM17->CCMR1 |= TIM_CCMR1_OC1M_0;
	 	TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_1;
	 	TIM17->CCMR1 |= TIM_CCMR1_OC1M_2;
	 	TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_3;

	   TIM17->EGR |= TIM_EGR_UG;
	   TIM17->CR1 |= TIM_CR1_CEN;
	}
	else if (mode_tmr17 == TRG_MODE_T1HIGH_LOW){
		//==put high for t1, then put low
		TIM17->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
		TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_1;
		TIM17->CCMR1 |= TIM_CCMR1_OC1M_2;
		TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_3;

//		TIM17->PSC = 15999; //us resolution
		TIM17->ARR = (uint32_t)t1_tmr17;
		TIM17->EGR |= TIM_EGR_UG;
	   TIM17->CR1 |= TIM_CR1_CEN;

	}
	else if (mode_tmr17 == TRG_MODE_T1LOW_HIGH){
		//==put low for t1, then put high
		//put low
		TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_0;
		TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_1;
		TIM17->CCMR1 |= TIM_CCMR1_OC1M_2;
		TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_3;

//		TIM17->PSC = 15999; //us resolution
		TIM17->ARR = (uint32_t)t1_tmr17;
		TIM17->EGR |= TIM_EGR_UG;
	   TIM17->CR1 |= TIM_CR1_CEN;
	}
	else if (mode_tmr17 == 4){
		//==put low for t1, then put high for t2, then put low
		//put low
		TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_0;
		TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_1;
		TIM17->CCMR1 |= TIM_CCMR1_OC1M_2;
		TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_3;

//		TIM17->PSC = 15999; //us resolution
		TIM17->ARR = (uint32_t)t1_tmr17;
		TIM17->EGR |= TIM_EGR_UG;
	   TIM17->CR1 |= TIM_CR1_CEN;
	}
	else if (mode_tmr17 == 5){
		//==put high for t1, then put low for t2, then put high
		TIM17->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
		TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_1;
		TIM17->CCMR1 |= TIM_CCMR1_OC1M_2;
		TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_3;

//		TIM17->PSC = 15999; //us resolution
		TIM17->ARR = (uint32_t)t1_tmr17;
		TIM17->EGR |= TIM_EGR_UG;
	   TIM17->CR1 |= TIM_CR1_CEN;
	}
	else if (mode_tmr17 == 6){
		//==start pwm for t1 (number of pulses should calculated), then put low
		num_pulse_pwm_tmr17 = (t1)/(l_pwm_tmr17+h_pwm_tmr17);

//		TIM17->PSC = 15999; //us resolution
		TIM17->ARR = (uint32_t)(h_pwm_tmr17+l_pwm_tmr17);
	  TIM17->CCR1 = (uint32_t)h_pwm_tmr17;

	TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_0; //pwm1
	TIM17->CCMR1 |= TIM_CCMR1_OC1M_1;
	TIM17->CCMR1 |= TIM_CCMR1_OC1M_2;
	TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_3;

	TIM17->EGR |= TIM_EGR_UG;
   TIM17->CR1 |= TIM_CR1_CEN;


	}
	else if (mode_tmr17 == 7){
		//==start pwm for t1, then put high
		num_pulse_pwm_tmr17 = (t1)/(l_pwm_tmr17+h_pwm_tmr17);

//		TIM17->PSC = 15999; //us resolution
		TIM17->ARR = (uint32_t)(h_pwm_tmr17+l_pwm_tmr17);
	  TIM17->CCR1 = (uint32_t)h_pwm_tmr17;

		TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_0; //pwm1
		TIM17->CCMR1 |= TIM_CCMR1_OC1M_1;
		TIM17->CCMR1 |= TIM_CCMR1_OC1M_2;
		TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_3;

		TIM17->EGR |= TIM_EGR_UG;
	   TIM17->CR1 |= TIM_CR1_CEN;
	}
	else if (mode_tmr17 == 8){
		//==start pwm for t1, then put high for t2, then put low
		num_pulse_pwm_tmr17 = (t1)/(l_pwm_tmr17+h_pwm_tmr17);

//		TIM17->PSC = 15999; //us resolution
		TIM17->ARR = (uint32_t)(h_pwm_tmr17+l_pwm_tmr17);
	  TIM17->CCR1 = (uint32_t)h_pwm_tmr17;

		TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_0; //pwm1
		TIM17->CCMR1 |= TIM_CCMR1_OC1M_1;
		TIM17->CCMR1 |= TIM_CCMR1_OC1M_2;
		TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_3;

		TIM17->EGR |= TIM_EGR_UG;
	   TIM17->CR1 |= TIM_CR1_CEN;

	}
	else if (mode_tmr17 == 9){
		//==start pwm for t1, then put low for t2, then put high
		num_pulse_pwm_tmr17 = (t1)/(l_pwm_tmr17+h_pwm_tmr17);

//		TIM17->PSC = 15999; //us resolution
		TIM17->ARR = (uint32_t)(h_pwm_tmr17+l_pwm_tmr17);
	  TIM17->CCR1 = (uint32_t)h_pwm_tmr17;

		TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_0; //pwm1
		TIM17->CCMR1 |= TIM_CCMR1_OC1M_1;
		TIM17->CCMR1 |= TIM_CCMR1_OC1M_2;
		TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_3;

		TIM17->EGR |= TIM_EGR_UG;
	   TIM17->CR1 |= TIM_CR1_CEN;
	}
	else if (mode_tmr17 == 10){
		//==put high for t1, then start pwm for t2, then put low
		num_pulse_pwm_tmr17 = (t2)/(l_pwm_tmr17+h_pwm_tmr17);

		TIM17->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
		TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_1;
		TIM17->CCMR1 |= TIM_CCMR1_OC1M_2;
		TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_3;

//		TIM17->PSC = 15999; //us resolution
		TIM17->ARR = (uint32_t)t1_tmr17;
		TIM17->EGR |= TIM_EGR_UG;
	   TIM17->CR1 |= TIM_CR1_CEN;

	}
	else if (mode_tmr17 == 11){
		//==put low for t1, then start pwm for t2, then put high
		num_pulse_pwm_tmr17 = (t2)/(l_pwm_tmr17+h_pwm_tmr17);

		//put low
		TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_0;
		TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_1;
		TIM17->CCMR1 |= TIM_CCMR1_OC1M_2;
		TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_3;

//		TIM17->PSC = 15999; //us resolution
		TIM17->ARR = (uint32_t)t1_tmr17;
		TIM17->EGR |= TIM_EGR_UG;
	   TIM17->CR1 |= TIM_CR1_CEN;
	}


	TIM17->SR &= ~TIM_SR_UIF;
   NVIC_EnableIRQ(TIM17_IRQn); // Enable interrupt from TIM3 (NVIC level)
}


void TIM17_IRQHandler(void) // interrupt routine
{
	  if(TIM17->SR & TIM_SR_UIF) // if UIF flag is set
	  {
	    TIM17->SR &= ~TIM_SR_UIF; // clear UIF flag


		if (mode_tmr17 == TRG_MODE_LOW){
			//do nothing
		}
		else if(mode_tmr17 == TRG_MODE_HIGH){
			//do nothing
		}
		else if (mode_tmr17 == TRG_MODE_T1HIGH_LOW){
			//==when recieve intr(t1 done), put low
			//put low
		 	TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_0;
		 	TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_1;
		 	TIM17->CCMR1 |= TIM_CCMR1_OC1M_2;
		 	TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_3;
		   TIM17->EGR |= TIM_EGR_UG;
		   TIM17->CR1 &= ~TIM_CR1_CEN; //disable

		}
		else if (mode_tmr17 == TRG_MODE_T1LOW_HIGH){
			//==when recieve put high
			TIM17->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
			TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_1;
			TIM17->CCMR1 |= TIM_CCMR1_OC1M_2;
			TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_3;

			TIM17->EGR |= TIM_EGR_UG;
		   TIM17->CR1 &= ~TIM_CR1_CEN; //disable
		}
		else if (mode_tmr17 == 4){
			//==put low for t1, then put high for t2, then put low
			static int mode4_cntr = 0;
			//first recieve: put high, set for t2
			//2nd rec: put low
			if (mode4_cntr == 0){
				TIM17->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
				TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_1;
				TIM17->CCMR1 |= TIM_CCMR1_OC1M_2;
				TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_3;

				TIM17->ARR = (uint32_t)t2_tmr17;
				TIM17->EGR |= TIM_EGR_UG;
			   TIM17->CR1 |= TIM_CR1_CEN;

			   mode4_cntr++;
			}
			else{
				//put low
				TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_0;
				TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_1;
				TIM17->CCMR1 |= TIM_CCMR1_OC1M_2;
				TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_3;
			   TIM17->EGR |= TIM_EGR_UG;
			   TIM17->CR1 &= ~TIM_CR1_CEN; //disable
				mode4_cntr = 0;
			}
		}
		else if (mode_tmr17 == 5){
			//==put high for t1, then put low for t2, then put high
			static int mode5_cntr = 0;
			//1st rec: put low, set for t2
			//2nd rec: put high
			if (mode5_cntr == 0){
				//put low
				TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_0;
				TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_1;
				TIM17->CCMR1 |= TIM_CCMR1_OC1M_2;
				TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_3;

				TIM17->ARR = (uint32_t)t2_tmr17;
				TIM17->EGR |= TIM_EGR_UG;
			   TIM17->CR1 |= TIM_CR1_CEN;

			   mode5_cntr++;
			}
			else{

				TIM17->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
				TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_1;
				TIM17->CCMR1 |= TIM_CCMR1_OC1M_2;
				TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_3;
			   TIM17->EGR |= TIM_EGR_UG;
			   TIM17->CR1 &= ~TIM_CR1_CEN; //disable
				mode5_cntr = 0;
			}

		}
		else if (mode_tmr17 == 6){
			//==start pwm for t1 (number of pulses should calculated), then put low
			//count numbers, when reached the value, then put low just
			static int mode6_cntr = 0;
			if(mode6_cntr<num_pulse_pwm_tmr17){
				mode6_cntr++;
			}
			else{
				//put low
				TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_0;
				TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_1;
				TIM17->CCMR1 |= TIM_CCMR1_OC1M_2;
				TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_3;
			   TIM17->EGR |= TIM_EGR_UG;
			   TIM17->CR1 &= ~TIM_CR1_CEN; //disable
				mode6_cntr=0;
			}

		}
		else if (mode_tmr17 == 7){
			//==start pwm for t1, then put high
			//count num of intr, when reached put high
			static int mode7_cntr = 0;
			if(mode7_cntr<num_pulse_pwm_tmr17){
				mode7_cntr++;
			}
			else{
				TIM17->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
				TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_1;
				TIM17->CCMR1 |= TIM_CCMR1_OC1M_2;
				TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_3;
			   TIM17->EGR |= TIM_EGR_UG;
			   TIM17->CR1 &= ~TIM_CR1_CEN; //disable
				mode7_cntr=0;
			}
		}
		else if (mode_tmr17 == 8){
			//==start pwm for t1, then put high for t2, then put low
			//count numbers, when reached put high, set timer for t2, pwm_dis, the intr after that put low
			static int mode8_cntr1 = 0;
			static int mode8_cntr2 = 0;
			if(mode8_cntr1 == 0){ //state 1
				if(mode8_cntr2<num_pulse_pwm_tmr17){ //pwm counter
					mode8_cntr2++;
				}
				else{
					TIM17->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
					TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_1;
					TIM17->CCMR1 |= TIM_CCMR1_OC1M_2;
					TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_3;

					TIM17->ARR = (uint32_t)t2_tmr17;
					TIM17->EGR |= TIM_EGR_UG;
				   TIM17->CR1 |= TIM_CR1_CEN;

				   mode8_cntr2=0;
				   mode8_cntr1++;
				}
			}
			else{	//state 2
				//put low
			 	TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_0;
			 	TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_1;
			 	TIM17->CCMR1 |= TIM_CCMR1_OC1M_2;
			 	TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_3;
			   TIM17->EGR |= TIM_EGR_UG;
			   TIM17->CR1 &= ~TIM_CR1_CEN; //disable
			   mode8_cntr2=0;
			   mode8_cntr1=0;
			}

		}
		else if (mode_tmr17 == 9){
			//==start pwm for t1, then put low for t2, then put high
			//count numbers, when reached put low, set timer for t2, pwm_dis, the intr after that put high
			static int mode9_cntr1 = 0;
			static int mode9_cntr2 = 0;
			if(mode9_cntr1 == 0){ //state 1
				if(mode9_cntr2<num_pulse_pwm_tmr17){ //pwm counter
					mode9_cntr2++;
				}
				else{
					//put low
				 	TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_0;
				 	TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_1;
				 	TIM17->CCMR1 |= TIM_CCMR1_OC1M_2;
				 	TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_3;

					TIM17->ARR = (uint32_t)t2_tmr17;
					TIM17->EGR |= TIM_EGR_UG;
				   TIM17->CR1 |= TIM_CR1_CEN;

				   mode9_cntr2=0;
				   mode9_cntr1++;
				}
			}
			else{	//state 2
				TIM17->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
				TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_1;
				TIM17->CCMR1 |= TIM_CCMR1_OC1M_2;
				TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_3;

			   TIM17->EGR |= TIM_EGR_UG;
			   TIM17->CR1 &= ~TIM_CR1_CEN; //disable
			   mode9_cntr2=0;
			   mode9_cntr1=0;
			}
		}
		else if (mode_tmr17 == 10){
			//==put high for t1, then start pwm for t2, then put low
			//1st: (pwm_disabled) set pwm,
			//count for t2, when done, put low
			static int mode10_cntr1 = 0;
			static int mode10_cntr2 = 0;

			if (mode10_cntr1 == 0){ //state1: t1 finished, set the t2
//				TIM17->PSC = 15999; //us resolution
				TIM17->ARR = (uint32_t)(h_pwm_tmr17+l_pwm_tmr17);
			  TIM17->CCR1 = (uint32_t)h_pwm_tmr17;

				TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_0; //pwm1
				TIM17->CCMR1 |= TIM_CCMR1_OC1M_1;
				TIM17->CCMR1 |= TIM_CCMR1_OC1M_2;
				TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_3;

				TIM17->EGR |= TIM_EGR_UG;
			   TIM17->CR1 |= TIM_CR1_CEN;

				mode10_cntr1++; //go to next state
				mode10_cntr2 = 0;
			}

			else{ //state2: pwm
				if(mode10_cntr2<num_pulse_pwm_tmr17){ //pwm counter
					mode10_cntr2++;
				}
				else{
					//put low
				 	TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_0;
				 	TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_1;
				 	TIM17->CCMR1 |= TIM_CCMR1_OC1M_2;
				 	TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_3;

//					TIM17->ARR = (uint32_t)t2_tmr17;
					TIM17->EGR |= TIM_EGR_UG;
					TIM17->CR1 &= ~TIM_CR1_CEN; //disable

				   mode10_cntr2=0;
				   mode10_cntr1=0;
				}
			}

		}
		else if (mode_tmr17 == 11){
			//==put low for t1, then start pwm for t2, then put high
			//1st: (pwm_disabled) set pwm,
			//count for t2, when done, put high
			static int mode11_cntr1 = 0;
			static int mode11_cntr2 = 0;

			if (mode11_cntr1 == 0){ //state1: t1 finished, set the t2
//				TIM17->PSC = 15999; //us resolution
				TIM17->ARR = (uint32_t)(h_pwm_tmr17+l_pwm_tmr17);
			  TIM17->CCR1 = (uint32_t)h_pwm_tmr17;

				TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_0; //pwm1
				TIM17->CCMR1 |= TIM_CCMR1_OC1M_1;
				TIM17->CCMR1 |= TIM_CCMR1_OC1M_2;
				TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_3;

				TIM17->EGR |= TIM_EGR_UG;
			   TIM17->CR1 |= TIM_CR1_CEN;

				mode11_cntr1++; //go to next state
				mode11_cntr2 = 0;
			}

			else{ //state2: pwm
				if(mode11_cntr2<num_pulse_pwm_tmr17){ //pwm counter
					mode11_cntr2++;
				}
				else{
					TIM17->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
					TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_1;
					TIM17->CCMR1 |= TIM_CCMR1_OC1M_2;
					TIM17->CCMR1 &= ~TIM_CCMR1_OC1M_3;

//					TIM17->ARR = (uint32_t)t2_tmr17;
					TIM17->EGR |= TIM_EGR_UG;
					TIM17->CR1 &= ~TIM_CR1_CEN; //disable

				   mode11_cntr2=0;
				   mode11_cntr1=0;
				}
			}
		}

	  }
}

//================== TIM14 TRIGGER ====================

void trg_init_tmr14(void){
	//clk enable
	  __HAL_RCC_TIM14_CLK_ENABLE();

	  __HAL_RCC_GPIOC_CLK_ENABLE();
	  /**TIM14 GPIO Configuration, PC12     ------> TIM14_CH1 */
	  GPIO_InitTypeDef GPIO_InitStruct = {0};
	  GPIO_InitStruct.Pin = GPIO_PIN_12;
	  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  GPIO_InitStruct.Alternate = GPIO_AF2_TIM14;
	  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);


	  //reg initialization
	  TIM14->CR1 &= ~TIM_CR1_CEN; //disable timer
	  TIM14->CR1 &= ~TIM_CR1_DIR; //up counter
	   TIM14->CR1 |= TIM_CR1_URS; //1: Only counter overflow/underflow generates an update interrupt or DMA request if enabled. (not setting UG bit)
//	   TIM14->CR1 |= TIM_CR1_UDIS;
	   TIM14->RCR = 0; //Repetition counter value
	   TIM14->CR1 &= ~TIM_CR1_ARPE; //AutoReloadPreload disabled
//	   TIM14->SR &= ~TIM_SR_UIF;
	     TIM14->DIER |= TIM_DIER_UIE; // enable update interrupts
	 	TIM14->BDTR |= (TIM_BDTR_MOE); //1: OC and OCN outputs are enabled if their respective enable bits are set (CCxE, CCxNE in TIMx_CCER register)
	   TIM14->CCMR1 &= ~TIM_CCMR1_CC1S_0;  //configuring as output
	   TIM14->CCMR1 &= ~TIM_CCMR1_CC1S_1;
	   TIM14->CCER |= TIM_CCER_CC1E; //Output enabled
	   TIM14->CCER &= ~TIM_CCER_CC1P; //0: OC1 active high
//	   TIM14->CCER &= ~TIM_CCER_CC1NP;
	   TIM14->PSC = 15;			//1us resolution
	   TIM14->EGR |= TIM_EGR_UG;

	   NVIC_SetPriority(TIM14_IRQn, 3);
	   NVIC_SetVector(TIM14_IRQn, (uint32_t)&TIM14_IRQHandler);
}



void trg_impl_tmr14(int mode, unsigned int t1, unsigned int t2, unsigned int l_pwm, unsigned int h_pwm){

	mode_tmr14 = mode; //save to be used in isr
	 t1_tmr14 = t1;
	 t2_tmr14 = t2;
	 l_pwm_tmr14 = l_pwm;
	 h_pwm_tmr14 = h_pwm;
	num_pulse_pwm_tmr14 = 0; //initialize in the required mode


	if (mode_tmr14 == TRG_MODE_LOW){
		//==put low
//		   TIM14->PSC = 15999;
		   // shart bezaram agar az 1000 bozorg tar bud ms esh bokonam (ya bar asase pwm hast ya na)
		     TIM14->ARR = 1000;
		   // channel register
		   TIM14->CCR1 = 200;
		   //put low
		 	TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_0;
		 	TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_1;
		 	TIM14->CCMR1 |= TIM_CCMR1_OC1M_2;
		 	TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_3;
		 	// update generation: Re-initialize the counter and generates an update of the registers.

		   TIM14->EGR |= TIM_EGR_UG;
		   TIM14->CR1 |= TIM_CR1_CEN;

	}
	else if(mode_tmr14 == TRG_MODE_HIGH){
		//==put high
	 	TIM14->CCMR1 |= TIM_CCMR1_OC1M_0;
	 	TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_1;
	 	TIM14->CCMR1 |= TIM_CCMR1_OC1M_2;
	 	TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_3;

	   TIM14->EGR |= TIM_EGR_UG;
	   TIM14->CR1 |= TIM_CR1_CEN;
	}
	else if (mode_tmr14 == TRG_MODE_T1HIGH_LOW){
		//==put high for t1, then put low
		TIM14->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
		TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_1;
		TIM14->CCMR1 |= TIM_CCMR1_OC1M_2;
		TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_3;

//		TIM14->PSC = 15999; //us resolution
		TIM14->ARR = (uint32_t)t1_tmr14;
		TIM14->EGR |= TIM_EGR_UG;
	   TIM14->CR1 |= TIM_CR1_CEN;

	}
	else if (mode_tmr14 == TRG_MODE_T1LOW_HIGH){
		//==put low for t1, then put high
		//put low
		TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_0;
		TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_1;
		TIM14->CCMR1 |= TIM_CCMR1_OC1M_2;
		TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_3;

//		TIM14->PSC = 15999; //us resolution
		TIM14->ARR = (uint32_t)t1_tmr14;
		TIM14->EGR |= TIM_EGR_UG;
	   TIM14->CR1 |= TIM_CR1_CEN;
	}
	else if (mode_tmr14 == 4){
		//==put low for t1, then put high for t2, then put low
		//put low
		TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_0;
		TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_1;
		TIM14->CCMR1 |= TIM_CCMR1_OC1M_2;
		TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_3;

//		TIM14->PSC = 15999; //us resolution
		TIM14->ARR = (uint32_t)t1_tmr14;
		TIM14->EGR |= TIM_EGR_UG;
	   TIM14->CR1 |= TIM_CR1_CEN;
	}
	else if (mode_tmr14 == 5){
		//==put high for t1, then put low for t2, then put high
		TIM14->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
		TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_1;
		TIM14->CCMR1 |= TIM_CCMR1_OC1M_2;
		TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_3;

//		TIM14->PSC = 15999; //us resolution
		TIM14->ARR = (uint32_t)t1_tmr14;
		TIM14->EGR |= TIM_EGR_UG;
	   TIM14->CR1 |= TIM_CR1_CEN;
	}
	else if (mode_tmr14 == 6){
		//==start pwm for t1 (number of pulses should calculated), then put low
		num_pulse_pwm_tmr14 = (t1)/(l_pwm_tmr14+h_pwm_tmr14);

//		TIM14->PSC = 15999; //us resolution
		TIM14->ARR = (uint32_t)(h_pwm_tmr14+l_pwm_tmr14);
	  TIM14->CCR1 = (uint32_t)h_pwm_tmr14;

	TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_0; //pwm1
	TIM14->CCMR1 |= TIM_CCMR1_OC1M_1;
	TIM14->CCMR1 |= TIM_CCMR1_OC1M_2;
	TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_3;

	TIM14->EGR |= TIM_EGR_UG;
   TIM14->CR1 |= TIM_CR1_CEN;


	}
	else if (mode_tmr14 == 7){
		//==start pwm for t1, then put high
		num_pulse_pwm_tmr14 = (t1)/(l_pwm_tmr14+h_pwm_tmr14);

//		TIM14->PSC = 15999; //us resolution
		TIM14->ARR = (uint32_t)(h_pwm_tmr14+l_pwm_tmr14);
	  TIM14->CCR1 = (uint32_t)h_pwm_tmr14;

		TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_0; //pwm1
		TIM14->CCMR1 |= TIM_CCMR1_OC1M_1;
		TIM14->CCMR1 |= TIM_CCMR1_OC1M_2;
		TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_3;

		TIM14->EGR |= TIM_EGR_UG;
	   TIM14->CR1 |= TIM_CR1_CEN;
	}
	else if (mode_tmr14 == 8){
		//==start pwm for t1, then put high for t2, then put low
		num_pulse_pwm_tmr14 = (t1)/(l_pwm_tmr14+h_pwm_tmr14);

//		TIM14->PSC = 15999; //us resolution
		TIM14->ARR = (uint32_t)(h_pwm_tmr14+l_pwm_tmr14);
	  TIM14->CCR1 = (uint32_t)h_pwm_tmr14;

		TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_0; //pwm1
		TIM14->CCMR1 |= TIM_CCMR1_OC1M_1;
		TIM14->CCMR1 |= TIM_CCMR1_OC1M_2;
		TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_3;

		TIM14->EGR |= TIM_EGR_UG;
	   TIM14->CR1 |= TIM_CR1_CEN;

	}
	else if (mode_tmr14 == 9){
		//==start pwm for t1, then put low for t2, then put high
		num_pulse_pwm_tmr14 = (t1)/(l_pwm_tmr14+h_pwm_tmr14);

//		TIM14->PSC = 15999; //us resolution
		TIM14->ARR = (uint32_t)(h_pwm_tmr14+l_pwm_tmr14);
	  TIM14->CCR1 = (uint32_t)h_pwm_tmr14;

		TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_0; //pwm1
		TIM14->CCMR1 |= TIM_CCMR1_OC1M_1;
		TIM14->CCMR1 |= TIM_CCMR1_OC1M_2;
		TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_3;

		TIM14->EGR |= TIM_EGR_UG;
	   TIM14->CR1 |= TIM_CR1_CEN;
	}
	else if (mode_tmr14 == 10){
		//==put high for t1, then start pwm for t2, then put low
		num_pulse_pwm_tmr14 = (t2)/(l_pwm_tmr14+h_pwm_tmr14);

		TIM14->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
		TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_1;
		TIM14->CCMR1 |= TIM_CCMR1_OC1M_2;
		TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_3;

//		TIM14->PSC = 15999; //us resolution
		TIM14->ARR = (uint32_t)t1_tmr14;
		TIM14->EGR |= TIM_EGR_UG;
	   TIM14->CR1 |= TIM_CR1_CEN;

	}
	else if (mode_tmr14 == 11){
		//==put low for t1, then start pwm for t2, then put high
		num_pulse_pwm_tmr14 = (t2)/(l_pwm_tmr14+h_pwm_tmr14);

		//put low
		TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_0;
		TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_1;
		TIM14->CCMR1 |= TIM_CCMR1_OC1M_2;
		TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_3;

//		TIM14->PSC = 15999; //us resolution
		TIM14->ARR = (uint32_t)t1_tmr14;
		TIM14->EGR |= TIM_EGR_UG;
	   TIM14->CR1 |= TIM_CR1_CEN;
	}


	TIM14->SR &= ~TIM_SR_UIF;
   NVIC_EnableIRQ(TIM14_IRQn); // Enable interrupt from TIM3 (NVIC level)
}


void TIM14_IRQHandler(void) // interrupt routine
{
	  if(TIM14->SR & TIM_SR_UIF) // if UIF flag is set
	  {
	    TIM14->SR &= ~TIM_SR_UIF; // clear UIF flag


		if (mode_tmr14 == TRG_MODE_LOW){
			//do nothing
		}
		else if(mode_tmr14 == TRG_MODE_HIGH){
			//do nothing
		}
		else if (mode_tmr14 == TRG_MODE_T1HIGH_LOW){
			//==when recieve intr(t1 done), put low
			//put low
		 	TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_0;
		 	TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_1;
		 	TIM14->CCMR1 |= TIM_CCMR1_OC1M_2;
		 	TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_3;
		   TIM14->EGR |= TIM_EGR_UG;
		   TIM14->CR1 &= ~TIM_CR1_CEN; //disable

		}
		else if (mode_tmr14 == TRG_MODE_T1LOW_HIGH){
			//==when recieve put high
			TIM14->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
			TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_1;
			TIM14->CCMR1 |= TIM_CCMR1_OC1M_2;
			TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_3;

			TIM14->EGR |= TIM_EGR_UG;
		   TIM14->CR1 &= ~TIM_CR1_CEN; //disable
		}
		else if (mode_tmr14 == 4){
			//==put low for t1, then put high for t2, then put low
			static int mode4_cntr = 0;
			//first recieve: put high, set for t2
			//2nd rec: put low
			if (mode4_cntr == 0){
				TIM14->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
				TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_1;
				TIM14->CCMR1 |= TIM_CCMR1_OC1M_2;
				TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_3;

				TIM14->ARR = (uint32_t)t2_tmr14;
				TIM14->EGR |= TIM_EGR_UG;
			   TIM14->CR1 |= TIM_CR1_CEN;

			   mode4_cntr++;
			}
			else{
				//put low
				TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_0;
				TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_1;
				TIM14->CCMR1 |= TIM_CCMR1_OC1M_2;
				TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_3;
			   TIM14->EGR |= TIM_EGR_UG;
			   TIM14->CR1 &= ~TIM_CR1_CEN; //disable
				mode4_cntr = 0;
			}
		}
		else if (mode_tmr14 == 5){
			//==put high for t1, then put low for t2, then put high
			static int mode5_cntr = 0;
			//1st rec: put low, set for t2
			//2nd rec: put high
			if (mode5_cntr == 0){
				//put low
				TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_0;
				TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_1;
				TIM14->CCMR1 |= TIM_CCMR1_OC1M_2;
				TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_3;

				TIM14->ARR = (uint32_t)t2_tmr14;
				TIM14->EGR |= TIM_EGR_UG;
			   TIM14->CR1 |= TIM_CR1_CEN;

			   mode5_cntr++;
			}
			else{

				TIM14->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
				TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_1;
				TIM14->CCMR1 |= TIM_CCMR1_OC1M_2;
				TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_3;
			   TIM14->EGR |= TIM_EGR_UG;
			   TIM14->CR1 &= ~TIM_CR1_CEN; //disable
				mode5_cntr = 0;
			}

		}
		else if (mode_tmr14 == 6){
			//==start pwm for t1 (number of pulses should calculated), then put low
			//count numbers, when reached the value, then put low just
			static int mode6_cntr = 0;
			if(mode6_cntr<num_pulse_pwm_tmr14){
				mode6_cntr++;
			}
			else{
				//put low
				TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_0;
				TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_1;
				TIM14->CCMR1 |= TIM_CCMR1_OC1M_2;
				TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_3;
			   TIM14->EGR |= TIM_EGR_UG;
			   TIM14->CR1 &= ~TIM_CR1_CEN; //disable
				mode6_cntr=0;
			}

		}
		else if (mode_tmr14 == 7){
			//==start pwm for t1, then put high
			//count num of intr, when reached put high
			static int mode7_cntr = 0;
			if(mode7_cntr<num_pulse_pwm_tmr14){
				mode7_cntr++;
			}
			else{
				TIM14->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
				TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_1;
				TIM14->CCMR1 |= TIM_CCMR1_OC1M_2;
				TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_3;
			   TIM14->EGR |= TIM_EGR_UG;
			   TIM14->CR1 &= ~TIM_CR1_CEN; //disable
				mode7_cntr=0;
			}
		}
		else if (mode_tmr14 == 8){
			//==start pwm for t1, then put high for t2, then put low
			//count numbers, when reached put high, set timer for t2, pwm_dis, the intr after that put low
			static int mode8_cntr1 = 0;
			static int mode8_cntr2 = 0;
			if(mode8_cntr1 == 0){ //state 1
				if(mode8_cntr2<num_pulse_pwm_tmr14){ //pwm counter
					mode8_cntr2++;
				}
				else{
					TIM14->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
					TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_1;
					TIM14->CCMR1 |= TIM_CCMR1_OC1M_2;
					TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_3;

					TIM14->ARR = (uint32_t)t2_tmr14;
					TIM14->EGR |= TIM_EGR_UG;
				   TIM14->CR1 |= TIM_CR1_CEN;

				   mode8_cntr2=0;
				   mode8_cntr1++;
				}
			}
			else{	//state 2
				//put low
			 	TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_0;
			 	TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_1;
			 	TIM14->CCMR1 |= TIM_CCMR1_OC1M_2;
			 	TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_3;
			   TIM14->EGR |= TIM_EGR_UG;
			   TIM14->CR1 &= ~TIM_CR1_CEN; //disable
			   mode8_cntr2=0;
			   mode8_cntr1=0;
			}

		}
		else if (mode_tmr14 == 9){
			//==start pwm for t1, then put low for t2, then put high
			//count numbers, when reached put low, set timer for t2, pwm_dis, the intr after that put high
			static int mode9_cntr1 = 0;
			static int mode9_cntr2 = 0;
			if(mode9_cntr1 == 0){ //state 1
				if(mode9_cntr2<num_pulse_pwm_tmr14){ //pwm counter
					mode9_cntr2++;
				}
				else{
					//put low
				 	TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_0;
				 	TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_1;
				 	TIM14->CCMR1 |= TIM_CCMR1_OC1M_2;
				 	TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_3;

					TIM14->ARR = (uint32_t)t2_tmr14;
					TIM14->EGR |= TIM_EGR_UG;
				   TIM14->CR1 |= TIM_CR1_CEN;

				   mode9_cntr2=0;
				   mode9_cntr1++;
				}
			}
			else{	//state 2
				TIM14->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
				TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_1;
				TIM14->CCMR1 |= TIM_CCMR1_OC1M_2;
				TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_3;

			   TIM14->EGR |= TIM_EGR_UG;
			   TIM14->CR1 &= ~TIM_CR1_CEN; //disable
			   mode9_cntr2=0;
			   mode9_cntr1=0;
			}
		}
		else if (mode_tmr14 == 10){
			//==put high for t1, then start pwm for t2, then put low
			//1st: (pwm_disabled) set pwm,
			//count for t2, when done, put low
			static int mode10_cntr1 = 0;
			static int mode10_cntr2 = 0;

			if (mode10_cntr1 == 0){ //state1: t1 finished, set the t2
//				TIM14->PSC = 15999; //us resolution
				TIM14->ARR = (uint32_t)(h_pwm_tmr14+l_pwm_tmr14);
			  TIM14->CCR1 = (uint32_t)h_pwm_tmr14;

				TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_0; //pwm1
				TIM14->CCMR1 |= TIM_CCMR1_OC1M_1;
				TIM14->CCMR1 |= TIM_CCMR1_OC1M_2;
				TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_3;

				TIM14->EGR |= TIM_EGR_UG;
			   TIM14->CR1 |= TIM_CR1_CEN;

				mode10_cntr1++; //go to next state
				mode10_cntr2 = 0;
			}

			else{ //state2: pwm
				if(mode10_cntr2<num_pulse_pwm_tmr14){ //pwm counter
					mode10_cntr2++;
				}
				else{
					//put low
				 	TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_0;
				 	TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_1;
				 	TIM14->CCMR1 |= TIM_CCMR1_OC1M_2;
				 	TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_3;

//					TIM14->ARR = (uint32_t)t2_tmr14;
					TIM14->EGR |= TIM_EGR_UG;
					TIM14->CR1 &= ~TIM_CR1_CEN; //disable

				   mode10_cntr2=0;
				   mode10_cntr1=0;
				}
			}

		}
		else if (mode_tmr14 == 11){
			//==put low for t1, then start pwm for t2, then put high
			//1st: (pwm_disabled) set pwm,
			//count for t2, when done, put high
			static int mode11_cntr1 = 0;
			static int mode11_cntr2 = 0;

			if (mode11_cntr1 == 0){ //state1: t1 finished, set the t2
//				TIM14->PSC = 15999; //us resolution
				TIM14->ARR = (uint32_t)(h_pwm_tmr14+l_pwm_tmr14);
			  TIM14->CCR1 = (uint32_t)h_pwm_tmr14;

				TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_0; //pwm1
				TIM14->CCMR1 |= TIM_CCMR1_OC1M_1;
				TIM14->CCMR1 |= TIM_CCMR1_OC1M_2;
				TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_3;

				TIM14->EGR |= TIM_EGR_UG;
			   TIM14->CR1 |= TIM_CR1_CEN;

				mode11_cntr1++; //go to next state
				mode11_cntr2 = 0;
			}

			else{ //state2: pwm
				if(mode11_cntr2<num_pulse_pwm_tmr14){ //pwm counter
					mode11_cntr2++;
				}
				else{
					TIM14->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
					TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_1;
					TIM14->CCMR1 |= TIM_CCMR1_OC1M_2;
					TIM14->CCMR1 &= ~TIM_CCMR1_OC1M_3;

//					TIM14->ARR = (uint32_t)t2_tmr14;
					TIM14->EGR |= TIM_EGR_UG;
					TIM14->CR1 &= ~TIM_CR1_CEN; //disable

				   mode11_cntr2=0;
				   mode11_cntr1=0;
				}
			}
		}

	  }
}


//================== TIM15 TRIGGER ====================

void trg_init_tmr15(void){
	//clk enable
	  __HAL_RCC_TIM15_CLK_ENABLE();

	    __HAL_RCC_GPIOC_CLK_ENABLE();
	    /**TIM15 GPIO Configuration, PC1     ------> TIM15_CH1 */
	    GPIO_InitTypeDef GPIO_InitStruct = {0};
	    GPIO_InitStruct.Pin = GPIO_PIN_1;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	    GPIO_InitStruct.Alternate = GPIO_AF2_TIM15;
	    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);


	  //reg initialization
	  TIM15->CR1 &= ~TIM_CR1_CEN; //disable timer
	  TIM15->CR1 &= ~TIM_CR1_DIR; //up counter
	   TIM15->CR1 |= TIM_CR1_URS; //1: Only counter overflow/underflow generates an update interrupt or DMA request if enabled. (not setting UG bit)
//	   TIM15->CR1 |= TIM_CR1_UDIS;
	   TIM15->RCR = 0; //Repetition counter value
	   TIM15->CR1 &= ~TIM_CR1_ARPE; //AutoReloadPreload disabled
//	   TIM15->SR &= ~TIM_SR_UIF;
	     TIM15->DIER |= TIM_DIER_UIE; // enable update interrupts
	 	TIM15->BDTR |= (TIM_BDTR_MOE); //1: OC and OCN outputs are enabled if their respective enable bits are set (CCxE, CCxNE in TIMx_CCER register)
	   TIM15->CCMR1 &= ~TIM_CCMR1_CC1S_0;  //configuring as output
	   TIM15->CCMR1 &= ~TIM_CCMR1_CC1S_1;
	   TIM15->CCER |= TIM_CCER_CC1E; //Output enabled
	   TIM15->CCER &= ~TIM_CCER_CC1P; //0: OC1 active high
//	   TIM15->CCER &= ~TIM_CCER_CC1NP;
	   TIM15->PSC = 15;			//1us resolution
	   TIM15->EGR |= TIM_EGR_UG;

	   NVIC_SetPriority(TIM15_IRQn, 3);
	   NVIC_SetVector(TIM15_IRQn, (uint32_t)&TIM15_IRQHandler);
}



void trg_impl_tmr15(int mode, unsigned int t1, unsigned int t2, unsigned int l_pwm, unsigned int h_pwm){

	mode_tmr15 = mode; //save to be used in isr
	 t1_tmr15 = t1;
	 t2_tmr15 = t2;
	 l_pwm_tmr15 = l_pwm;
	 h_pwm_tmr15 = h_pwm;
	num_pulse_pwm_tmr15 = 0; //initialize in the required mode


	if (mode_tmr15 == TRG_MODE_LOW){
		//==put low
//		   TIM15->PSC = 15999;
		   // shart bezaram agar az 1000 bozorg tar bud ms esh bokonam (ya bar asase pwm hast ya na)
		     TIM15->ARR = 1000;
		   // channel register
		   TIM15->CCR1 = 200;
		   //put low
		 	TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_0;
		 	TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_1;
		 	TIM15->CCMR1 |= TIM_CCMR1_OC1M_2;
		 	TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_3;
		 	// update generation: Re-initialize the counter and generates an update of the registers.

		   TIM15->EGR |= TIM_EGR_UG;
		   TIM15->CR1 |= TIM_CR1_CEN;

	}
	else if(mode_tmr15 == TRG_MODE_HIGH){
		//==put high
	 	TIM15->CCMR1 |= TIM_CCMR1_OC1M_0;
	 	TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_1;
	 	TIM15->CCMR1 |= TIM_CCMR1_OC1M_2;
	 	TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_3;

	   TIM15->EGR |= TIM_EGR_UG;
	   TIM15->CR1 |= TIM_CR1_CEN;
	}
	else if (mode_tmr15 == TRG_MODE_T1HIGH_LOW){
		//==put high for t1, then put low
		TIM15->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
		TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_1;
		TIM15->CCMR1 |= TIM_CCMR1_OC1M_2;
		TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_3;

//		TIM15->PSC = 15999; //us resolution
		TIM15->ARR = (uint32_t)t1_tmr15;
		TIM15->EGR |= TIM_EGR_UG;
	   TIM15->CR1 |= TIM_CR1_CEN;

	}
	else if (mode_tmr15 == TRG_MODE_T1LOW_HIGH){
		//==put low for t1, then put high
		//put low
		TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_0;
		TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_1;
		TIM15->CCMR1 |= TIM_CCMR1_OC1M_2;
		TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_3;

//		TIM15->PSC = 15999; //us resolution
		TIM15->ARR = (uint32_t)t1_tmr15;
		TIM15->EGR |= TIM_EGR_UG;
	   TIM15->CR1 |= TIM_CR1_CEN;
	}
	else if (mode_tmr15 == 4){
		//==put low for t1, then put high for t2, then put low
		//put low
		TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_0;
		TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_1;
		TIM15->CCMR1 |= TIM_CCMR1_OC1M_2;
		TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_3;

//		TIM15->PSC = 15999; //us resolution
		TIM15->ARR = (uint32_t)t1_tmr15;
		TIM15->EGR |= TIM_EGR_UG;
	   TIM15->CR1 |= TIM_CR1_CEN;
	}
	else if (mode_tmr15 == 5){
		//==put high for t1, then put low for t2, then put high
		TIM15->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
		TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_1;
		TIM15->CCMR1 |= TIM_CCMR1_OC1M_2;
		TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_3;

//		TIM15->PSC = 15999; //us resolution
		TIM15->ARR = (uint32_t)t1_tmr15;
		TIM15->EGR |= TIM_EGR_UG;
	   TIM15->CR1 |= TIM_CR1_CEN;
	}
	else if (mode_tmr15 == 6){
		//==start pwm for t1 (number of pulses should calculated), then put low
		num_pulse_pwm_tmr15 = (t1)/(l_pwm_tmr15+h_pwm_tmr15);

//		TIM15->PSC = 15999; //us resolution
		TIM15->ARR = (uint32_t)(h_pwm_tmr15+l_pwm_tmr15);
	  TIM15->CCR1 = (uint32_t)h_pwm_tmr15;

	TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_0; //pwm1
	TIM15->CCMR1 |= TIM_CCMR1_OC1M_1;
	TIM15->CCMR1 |= TIM_CCMR1_OC1M_2;
	TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_3;

	TIM15->EGR |= TIM_EGR_UG;
   TIM15->CR1 |= TIM_CR1_CEN;


	}
	else if (mode_tmr15 == 7){
		//==start pwm for t1, then put high
		num_pulse_pwm_tmr15 = (t1)/(l_pwm_tmr15+h_pwm_tmr15);

//		TIM15->PSC = 15999; //us resolution
		TIM15->ARR = (uint32_t)(h_pwm_tmr15+l_pwm_tmr15);
	  TIM15->CCR1 = (uint32_t)h_pwm_tmr15;

		TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_0; //pwm1
		TIM15->CCMR1 |= TIM_CCMR1_OC1M_1;
		TIM15->CCMR1 |= TIM_CCMR1_OC1M_2;
		TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_3;

		TIM15->EGR |= TIM_EGR_UG;
	   TIM15->CR1 |= TIM_CR1_CEN;
	}
	else if (mode_tmr15 == 8){
		//==start pwm for t1, then put high for t2, then put low
		num_pulse_pwm_tmr15 = (t1)/(l_pwm_tmr15+h_pwm_tmr15);

//		TIM15->PSC = 15999; //us resolution
		TIM15->ARR = (uint32_t)(h_pwm_tmr15+l_pwm_tmr15);
	  TIM15->CCR1 = (uint32_t)h_pwm_tmr15;

		TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_0; //pwm1
		TIM15->CCMR1 |= TIM_CCMR1_OC1M_1;
		TIM15->CCMR1 |= TIM_CCMR1_OC1M_2;
		TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_3;

		TIM15->EGR |= TIM_EGR_UG;
	   TIM15->CR1 |= TIM_CR1_CEN;

	}
	else if (mode_tmr15 == 9){
		//==start pwm for t1, then put low for t2, then put high
		num_pulse_pwm_tmr15 = (t1)/(l_pwm_tmr15+h_pwm_tmr15);

//		TIM15->PSC = 15999; //us resolution
		TIM15->ARR = (uint32_t)(h_pwm_tmr15+l_pwm_tmr15);
	  TIM15->CCR1 = (uint32_t)h_pwm_tmr15;

		TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_0; //pwm1
		TIM15->CCMR1 |= TIM_CCMR1_OC1M_1;
		TIM15->CCMR1 |= TIM_CCMR1_OC1M_2;
		TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_3;

		TIM15->EGR |= TIM_EGR_UG;
	   TIM15->CR1 |= TIM_CR1_CEN;
	}
	else if (mode_tmr15 == 10){
		//==put high for t1, then start pwm for t2, then put low
		num_pulse_pwm_tmr15 = (t2)/(l_pwm_tmr15+h_pwm_tmr15);

		TIM15->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
		TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_1;
		TIM15->CCMR1 |= TIM_CCMR1_OC1M_2;
		TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_3;

//		TIM15->PSC = 15999; //us resolution
		TIM15->ARR = (uint32_t)t1_tmr15;
		TIM15->EGR |= TIM_EGR_UG;
	   TIM15->CR1 |= TIM_CR1_CEN;

	}
	else if (mode_tmr15 == 11){
		//==put low for t1, then start pwm for t2, then put high
		num_pulse_pwm_tmr15 = (t2)/(l_pwm_tmr15+h_pwm_tmr15);

		//put low
		TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_0;
		TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_1;
		TIM15->CCMR1 |= TIM_CCMR1_OC1M_2;
		TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_3;

//		TIM15->PSC = 15999; //us resolution
		TIM15->ARR = (uint32_t)t1_tmr15;
		TIM15->EGR |= TIM_EGR_UG;
	   TIM15->CR1 |= TIM_CR1_CEN;
	}


	TIM15->SR &= ~TIM_SR_UIF;
   NVIC_EnableIRQ(TIM15_IRQn); // Enable interrupt from TIM3 (NVIC level)
}


void TIM15_IRQHandler(void) // interrupt routine
{
	  if(TIM15->SR & TIM_SR_UIF) // if UIF flag is set
	  {
	    TIM15->SR &= ~TIM_SR_UIF; // clear UIF flag


		if (mode_tmr15 == TRG_MODE_LOW){
			//do nothing
		}
		else if(mode_tmr15 == TRG_MODE_HIGH){
			//do nothing
		}
		else if (mode_tmr15 == TRG_MODE_T1HIGH_LOW){
			//==when recieve intr(t1 done), put low
			//put low
		 	TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_0;
		 	TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_1;
		 	TIM15->CCMR1 |= TIM_CCMR1_OC1M_2;
		 	TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_3;
		   TIM15->EGR |= TIM_EGR_UG;
		   TIM15->CR1 &= ~TIM_CR1_CEN; //disable

		}
		else if (mode_tmr15 == TRG_MODE_T1LOW_HIGH){
			//==when recieve put high
			TIM15->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
			TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_1;
			TIM15->CCMR1 |= TIM_CCMR1_OC1M_2;
			TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_3;

			TIM15->EGR |= TIM_EGR_UG;
		   TIM15->CR1 &= ~TIM_CR1_CEN; //disable
		}
		else if (mode_tmr15 == 4){
			//==put low for t1, then put high for t2, then put low
			static int mode4_cntr = 0;
			//first recieve: put high, set for t2
			//2nd rec: put low
			if (mode4_cntr == 0){
				TIM15->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
				TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_1;
				TIM15->CCMR1 |= TIM_CCMR1_OC1M_2;
				TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_3;

				TIM15->ARR = (uint32_t)t2_tmr15;
				TIM15->EGR |= TIM_EGR_UG;
			   TIM15->CR1 |= TIM_CR1_CEN;

			   mode4_cntr++;
			}
			else{
				//put low
				TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_0;
				TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_1;
				TIM15->CCMR1 |= TIM_CCMR1_OC1M_2;
				TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_3;
			   TIM15->EGR |= TIM_EGR_UG;
			   TIM15->CR1 &= ~TIM_CR1_CEN; //disable
				mode4_cntr = 0;
			}
		}
		else if (mode_tmr15 == 5){
			//==put high for t1, then put low for t2, then put high
			static int mode5_cntr = 0;
			//1st rec: put low, set for t2
			//2nd rec: put high
			if (mode5_cntr == 0){
				//put low
				TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_0;
				TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_1;
				TIM15->CCMR1 |= TIM_CCMR1_OC1M_2;
				TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_3;

				TIM15->ARR = (uint32_t)t2_tmr15;
				TIM15->EGR |= TIM_EGR_UG;
			   TIM15->CR1 |= TIM_CR1_CEN;

			   mode5_cntr++;
			}
			else{

				TIM15->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
				TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_1;
				TIM15->CCMR1 |= TIM_CCMR1_OC1M_2;
				TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_3;
			   TIM15->EGR |= TIM_EGR_UG;
			   TIM15->CR1 &= ~TIM_CR1_CEN; //disable
				mode5_cntr = 0;
			}

		}
		else if (mode_tmr15 == 6){
			//==start pwm for t1 (number of pulses should calculated), then put low
			//count numbers, when reached the value, then put low just
			static int mode6_cntr = 0;
			if(mode6_cntr<num_pulse_pwm_tmr15){
				mode6_cntr++;
			}
			else{
				//put low
				TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_0;
				TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_1;
				TIM15->CCMR1 |= TIM_CCMR1_OC1M_2;
				TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_3;
			   TIM15->EGR |= TIM_EGR_UG;
			   TIM15->CR1 &= ~TIM_CR1_CEN; //disable
				mode6_cntr=0;
			}

		}
		else if (mode_tmr15 == 7){
			//==start pwm for t1, then put high
			//count num of intr, when reached put high
			static int mode7_cntr = 0;
			if(mode7_cntr<num_pulse_pwm_tmr15){
				mode7_cntr++;
			}
			else{
				TIM15->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
				TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_1;
				TIM15->CCMR1 |= TIM_CCMR1_OC1M_2;
				TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_3;
			   TIM15->EGR |= TIM_EGR_UG;
			   TIM15->CR1 &= ~TIM_CR1_CEN; //disable
				mode7_cntr=0;
			}
		}
		else if (mode_tmr15 == 8){
			//==start pwm for t1, then put high for t2, then put low
			//count numbers, when reached put high, set timer for t2, pwm_dis, the intr after that put low
			static int mode8_cntr1 = 0;
			static int mode8_cntr2 = 0;
			if(mode8_cntr1 == 0){ //state 1
				if(mode8_cntr2<num_pulse_pwm_tmr15){ //pwm counter
					mode8_cntr2++;
				}
				else{
					TIM15->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
					TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_1;
					TIM15->CCMR1 |= TIM_CCMR1_OC1M_2;
					TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_3;

					TIM15->ARR = (uint32_t)t2_tmr15;
					TIM15->EGR |= TIM_EGR_UG;
				   TIM15->CR1 |= TIM_CR1_CEN;

				   mode8_cntr2=0;
				   mode8_cntr1++;
				}
			}
			else{	//state 2
				//put low
			 	TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_0;
			 	TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_1;
			 	TIM15->CCMR1 |= TIM_CCMR1_OC1M_2;
			 	TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_3;
			   TIM15->EGR |= TIM_EGR_UG;
			   TIM15->CR1 &= ~TIM_CR1_CEN; //disable
			   mode8_cntr2=0;
			   mode8_cntr1=0;
			}

		}
		else if (mode_tmr15 == 9){
			//==start pwm for t1, then put low for t2, then put high
			//count numbers, when reached put low, set timer for t2, pwm_dis, the intr after that put high
			static int mode9_cntr1 = 0;
			static int mode9_cntr2 = 0;
			if(mode9_cntr1 == 0){ //state 1
				if(mode9_cntr2<num_pulse_pwm_tmr15){ //pwm counter
					mode9_cntr2++;
				}
				else{
					//put low
				 	TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_0;
				 	TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_1;
				 	TIM15->CCMR1 |= TIM_CCMR1_OC1M_2;
				 	TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_3;

					TIM15->ARR = (uint32_t)t2_tmr15;
					TIM15->EGR |= TIM_EGR_UG;
				   TIM15->CR1 |= TIM_CR1_CEN;

				   mode9_cntr2=0;
				   mode9_cntr1++;
				}
			}
			else{	//state 2
				TIM15->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
				TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_1;
				TIM15->CCMR1 |= TIM_CCMR1_OC1M_2;
				TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_3;

			   TIM15->EGR |= TIM_EGR_UG;
			   TIM15->CR1 &= ~TIM_CR1_CEN; //disable
			   mode9_cntr2=0;
			   mode9_cntr1=0;
			}
		}
		else if (mode_tmr15 == 10){
			//==put high for t1, then start pwm for t2, then put low
			//1st: (pwm_disabled) set pwm,
			//count for t2, when done, put low
			static int mode10_cntr1 = 0;
			static int mode10_cntr2 = 0;

			if (mode10_cntr1 == 0){ //state1: t1 finished, set the t2
//				TIM15->PSC = 15999; //us resolution
				TIM15->ARR = (uint32_t)(h_pwm_tmr15+l_pwm_tmr15);
			  TIM15->CCR1 = (uint32_t)h_pwm_tmr15;

				TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_0; //pwm1
				TIM15->CCMR1 |= TIM_CCMR1_OC1M_1;
				TIM15->CCMR1 |= TIM_CCMR1_OC1M_2;
				TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_3;

				TIM15->EGR |= TIM_EGR_UG;
			   TIM15->CR1 |= TIM_CR1_CEN;

				mode10_cntr1++; //go to next state
				mode10_cntr2 = 0;
			}

			else{ //state2: pwm
				if(mode10_cntr2<num_pulse_pwm_tmr15){ //pwm counter
					mode10_cntr2++;
				}
				else{
					//put low
				 	TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_0;
				 	TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_1;
				 	TIM15->CCMR1 |= TIM_CCMR1_OC1M_2;
				 	TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_3;

//					TIM15->ARR = (uint32_t)t2_tmr15;
					TIM15->EGR |= TIM_EGR_UG;
					TIM15->CR1 &= ~TIM_CR1_CEN; //disable

				   mode10_cntr2=0;
				   mode10_cntr1=0;
				}
			}

		}
		else if (mode_tmr15 == 11){
			//==put low for t1, then start pwm for t2, then put high
			//1st: (pwm_disabled) set pwm,
			//count for t2, when done, put high
			static int mode11_cntr1 = 0;
			static int mode11_cntr2 = 0;

			if (mode11_cntr1 == 0){ //state1: t1 finished, set the t2
//				TIM15->PSC = 15999; //us resolution
				TIM15->ARR = (uint32_t)(h_pwm_tmr15+l_pwm_tmr15);
			  TIM15->CCR1 = (uint32_t)h_pwm_tmr15;

				TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_0; //pwm1
				TIM15->CCMR1 |= TIM_CCMR1_OC1M_1;
				TIM15->CCMR1 |= TIM_CCMR1_OC1M_2;
				TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_3;

				TIM15->EGR |= TIM_EGR_UG;
			   TIM15->CR1 |= TIM_CR1_CEN;

				mode11_cntr1++; //go to next state
				mode11_cntr2 = 0;
			}

			else{ //state2: pwm
				if(mode11_cntr2<num_pulse_pwm_tmr15){ //pwm counter
					mode11_cntr2++;
				}
				else{
					TIM15->CCMR1 |= TIM_CCMR1_OC1M_0; //put high
					TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_1;
					TIM15->CCMR1 |= TIM_CCMR1_OC1M_2;
					TIM15->CCMR1 &= ~TIM_CCMR1_OC1M_3;

//					TIM15->ARR = (uint32_t)t2_tmr15;
					TIM15->EGR |= TIM_EGR_UG;
					TIM15->CR1 &= ~TIM_CR1_CEN; //disable

				   mode11_cntr2=0;
				   mode11_cntr1=0;
				}
			}
		}

	  }
}


