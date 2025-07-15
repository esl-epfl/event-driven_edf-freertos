/*
 * retarget.h
 *
 *  Created on: 7 Jun 2021
 *      Author: hossein
 */

#ifndef INC_RETARGET_H_
#define INC_RETARGET_H_

// All credit to Carmine Noviello for this code
// https://github.com/cnoviello/mastering-stm32/blob/master/nucleo-f030R8/system/include/retarget/retarget.h

#ifndef _RETARGET_H__
#define _RETARGET_H__

//#include "stm32l4xx_hal.h"
#include "stm32g0xx_hal.h"
//#include "stm32f0xx_hal_conf.h"
//#include "stm32f0xx_it.h"
#include <sys/stat.h>

void RetargetInit(UART_HandleTypeDef *huart);
int _isatty(int fd);
int _write(int fd, char* ptr, int len);
int _close(int fd);
int _lseek(int fd, int ptr, int dir);
int _read(int fd, char* ptr, int len);
int _fstat(int fd, struct stat* st);

#endif //#ifndef _RETARGET_H__



#endif /* INC_RETARGET_H_ */
