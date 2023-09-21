#ifndef NSPRS_DRIVERS_H_
#define NSPRS_DRIVERS_H_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "nsprs_params.h"


//unsigned int tempChangeRes(unsigned int * regAddr, int bit);
//unsigned int tempChangeTriac(unsigned int * regAddr, int bit, SemaphoreHandle_t triacReadySem);
//
//void pumpEnable(void);

//signaling to tb & pmp triacs
//void sig2pump(char zc);
//void sig2tb(char zc);


//timer functions
unsigned int timer_readFreqKHz(int timer_id);
unsigned int timer_readPeriodMs(int timer_id);
unsigned int timer_readPeriodUs(int timer_id);
unsigned int timer_readMaxMs(int timer_id);
unsigned int timer_readMaxUs(int timer_id);
unsigned int timer_readMax(int timer_id);
unsigned int timer_readMs(int timer_id);
unsigned int timer_readUs(int timer_id);
unsigned int timer_read(int timer_id);
void timer_stopReset(int timer_id);
void timer_count(int timer_id, unsigned int count_val);

//gpio
void setPump1(void);
void resetPump1(void);
void setTB1(void);
void resetTB1(void);
void setTB2(void);
void resetTB2(void);

//temperature functions
int readTemp(void);

//all the hw initializations
void init_hw(void);


#endif //NSPRS_DRIVERS_H_
