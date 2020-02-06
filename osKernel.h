/*
 * osKernel.h
 *
 *  Created on: Feb 6, 2020
 *      Author: elmulinho
 */

#ifndef INC_OSKERNEL_H_
#define INC_OSKERNEL_H_

#include <stdint.h>



void osKernelLaunch(uint32_t quanta);
void osKernelInit(void);
uint8_t osKernelAddThreads(void(*task0)(void),
						   void(*task1)(void),
						   void(*task2)(void));
void osKernelLaunch(uint32_t quanta);
void osSchedulerLaunch(void);
void SysTick_Handler(void);
void osThreadYield(void);
void osPeriodicScheduler(void);
void osSemaphoreInit(int32_t* semaphore, int32_t value);
void osSignalSet(int32_t* semaphore);
void osSignalWait(int32_t* semaphore);

#endif /* INC_OSKERNEL_H_ */
