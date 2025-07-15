
#ifndef INC_STM32_TRG_DRIVER_H_
#define INC_STM32_TRG_DRIVER_H_

#define TRG_MODE_LOW				0
#define TRG_MODE_HIGH				1
#define TRG_MODE_T1HIGH_LOW			2
#define TRG_MODE_T1LOW_HIGH			3
#define TRG_MODE_T1LOW_T2HIGH_LOW	4
#define TRG_MODE_T1HIGH_T2LOW_HIGH	5
#define TRG_MODE_T1PWM_LOW			6
#define TRG_MODE_T1PWM_HIGH			7
#define TRG_MODE_T1PWM_T2HIGH_LOW	8
#define TRG_MODE_T1PWM_T2LOW_HIGH	9
#define TRG_MODE_T1HIGH_T2PWM_LOW	10
#define TRG_MODE_T1LOW_T2PWM_HIGH	11


void trg_init_tmr16(void);
void trg_impl_tmr16(int mode, unsigned int t1, unsigned int t2, unsigned int l_pwm, unsigned int h_pwm);
//void trg0_isr(void);
void TIM16_IRQHandler(void);
int mode_tmr16;
unsigned int t1_tmr16, t2_tmr16, l_pwm_tmr16, h_pwm_tmr16, num_pulse_pwm_tmr16;

void trg_init_tmr17(void);
void trg_impl_tmr17(int mode, unsigned int t1, unsigned int t2, unsigned int l_pwm, unsigned int h_pwm);
void TIM17_IRQHandler(void);
int mode_tmr17;
unsigned int t1_tmr17, t2_tmr17, l_pwm_tmr17, h_pwm_tmr17, num_pulse_pwm_tmr17;

void trg_init_tmr14(void);
void trg_impl_tmr14(int mode, unsigned int t1, unsigned int t2, unsigned int l_pwm, unsigned int h_pwm);
void TIM14_IRQHandler(void);
int mode_tmr14;
unsigned int t1_tmr14, t2_tmr14, l_pwm_tmr14, h_pwm_tmr14, num_pulse_pwm_tmr14;

void trg_init_tmr15(void);
void trg_impl_tmr15(int mode, unsigned int t1, unsigned int t2, unsigned int l_pwm, unsigned int h_pwm);
void TIM15_IRQHandler(void);
int mode_tmr15;
unsigned int t1_tmr15, t2_tmr15, l_pwm_tmr15, h_pwm_tmr15, num_pulse_pwm_tmr15;


#endif /* INC_STM32_TRG_DRIVER_H_ */
