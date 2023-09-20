#include "nsprs_params.h"
#include "nsprs_common.h"

void set__bit(unsigned int * regAddr, char bit){
	*regAddr = *regAddr | (1<<bit);
}
void clear__bit(unsigned int * regAddr, char bit){
	*regAddr = *regAddr & ~(1<<bit);
}
char check__bit(unsigned int * regAddr, char bit){
	if ((*regAddr & (1<<bit)) == 0){
		return 0;
	}
	else{
		return 1;
	}
}
void load_reg(unsigned int * regAddr, unsigned int value){
	* regAddr = value;
}

int log_id = 0;
unsigned int log[20];
void write_on_log(unsigned int state){
	xSemaphoreTake( xLogMutex, portMAX_DELAY );
	{
		log[log_id] = state;
		log_id++;
	}
	xSemaphoreGive( xLogMutex );

}
