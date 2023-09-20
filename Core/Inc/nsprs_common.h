#ifndef NSPRS_COMMON_H_
#define NSPRS_COMMON_H_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "nsprs_params.h"

//=========== bitwise functions
void set__bit(unsigned int * regAddr, char bit);
void clear__bit(unsigned int * regAddr, char bit);
char check__bit(unsigned int * regAddr, char bit);
void load_reg(unsigned int * regAddr, unsigned int value);

//just for debugging purpose
void write_on_log(unsigned int state);
extern unsigned int log[20];//log[500];
extern int log_id;
extern SemaphoreHandle_t xLogMutex;




#endif //NSPRS_COMMON_H_
