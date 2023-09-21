#include "nsprs_drivers.h"
#include "nsprs_params.h"
#include "nsprs_common.h"
//#include "nsprs_com.h"

#if PLATFORM == MB_PYNQZ2
#include "nsprs_platform_mb.h"
#elif PLATFORM == ARMCM0_NUCLEO_F070RB
#include <nsprs_platform_nucleo_g070rb.h>
#elif PLATFORM == NUCLEO_G070RB
#include "nsprs_platform_nucleo_g070rb.h"
#endif

//all the hw initializations
void init_hw(void){
	_platform_init_hw();
}

//timer functions
unsigned int timer_readFreqKHz(int timer_id){
	return _platform_timer_readFreqKHz(timer_id);
}
unsigned int timer_readPeriodMs(int timer_id){
	return _platform_timer_readPeriodMs(timer_id);
}
unsigned int timer_readPeriodUs(int timer_id){
	return _platform_timer_readPeriodUs(timer_id);
}

unsigned int timer_readMaxMs(int timer_id){
	return _platform_timer_readMaxMs(timer_id);
}
unsigned int timer_readMaxUs(int timer_id){
	return _platform_timer_readMaxUs(timer_id);
}

unsigned int timer_readMax(int timer_id){
	return _platform_timer_readMax(timer_id);
}

unsigned int timer_readMs(int timer_id){
	return _platform_timer_readMs(timer_id);
}
unsigned int timer_readUs(int timer_id){
	return _platform_timer_readUs(timer_id);
}
unsigned int timer_read(int timer_id){
	return _platform_timer_read(timer_id);
}
void timer_stopReset(int timer_id){
	_platform_timer_stopReset(timer_id);
}
void timer_count(int timer_id, unsigned int count_val){
	_platform_timer_count(timer_id, count_val);
}
//===================================================================

//adc
int readTemp(void){
	return _platform_readTemp();
}

////signaling to pump and heater
//void sig2pump(char zc){
//	_platform_sig2pump(zc);
//}
//
//void sig2tb(char zc){
//	_platform_sig2tb(zc);
//}

//gpio
void setPump1(void){
	_platform_setPump1();
}
void resetPump1(void){
	_platform_resetPump1();
}
void setTB1(void){
	_platform_setTB1();
}
void resetTB1(void){
	_platform_resetTB1();
}
void setTB2(void){
	_platform_setTB2();
}
void resetTB2(void){
	_platform_resetTB2();
}





