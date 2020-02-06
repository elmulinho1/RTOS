#ifndef __OSKERNEL_H
#define __OSKERNEL_H

#include <stdint.h>



void osKernelLaunch(uint32_t quanta);
void osKernelInit(void);
uint8_t osKernelAddThreads(void(*task0)(void),
						   void(*task1)(void),
						   void(*task2)(void));
void osKernelLaunch(uint32_t quanta);
void osSchedulerLaunch(void);
void SysTick_Handler(void);



#endif
