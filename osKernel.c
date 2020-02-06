/*
 * osKernel.c
 *
 *  Created on: Feb 6, 2020
 *      Author: elmulinho
 */


#include "osKernel.h"
#include "stm32f4xx.h"

#define NUM_OF_THREADS 	3
#define STACK_SIZE 		100
#define BUS_FREQ		16000000

#define SYSPRI3			(*((volatile uint32_t* )0xE000ED20))
#define INTCTRL			(*((volatile uint32_t* )0xE000ED04))

uint32_t mSecPrescaler;

struct tcb{
	int32_t* stackPt;
	struct tcb* nextPt;
};

typedef struct tcb tcbType;

tcbType tcbs[NUM_OF_THREADS];
tcbType* currentPt;

int32_t tcbStack[NUM_OF_THREADS][STACK_SIZE];

void osKernelStackInit(uint8_t taskNum) {
	tcbs[taskNum].stackPt = &tcbStack[taskNum][STACK_SIZE - 16];
	tcbStack[taskNum][STACK_SIZE - 1] = 0x01000000;
}

uint8_t osKernelAddThreads(void(*task0)(void),
						   void(*task1)(void),
						   void(*task2)(void)) {
	__disable_irq();

	tcbs[0].nextPt = &tcbs[1];
	tcbs[1].nextPt = &tcbs[2];
	tcbs[2].nextPt = &tcbs[0];

	osKernelStackInit(0);
	tcbStack[0][STACK_SIZE - 2] = (int32_t)(task0);

	osKernelStackInit(1);
	tcbStack[1][STACK_SIZE - 2] = (int32_t)(task1);

	osKernelStackInit(2);
	tcbStack[2][STACK_SIZE - 2] = (int32_t)(task2);

	currentPt = &tcbs[0];

	__enable_irq();

	return 1;
}

void osKernelInit(void) {
	__disable_irq();

	mSecPrescaler = BUS_FREQ / 1000;

}

void osKernelLaunch(uint32_t quanta) {
	SysTick->CTRL = 0;
	SysTick->VAL = 0;
	SYSPRI3 = (SYSPRI3 & 0x00FFFFFF) | 0xE0000000;
	SysTick->LOAD = (quanta * mSecPrescaler)-1;
	SysTick->CTRL = 0x00000007;
	osSchedulerLaunch();
}

void __attribute__ ((naked)) osSchedulerLaunch(void) {
	asm volatile("LDR		R0,=currentPt\n"
				 "LDR		R2,[R0]\n"
				 "LDR		SP,[R2]\n"
				 "POP		{R4-R11}\n"
				 "POP		{R0-R3}\n"
				 "POP		{R12}\n"
				 "ADD		SP,SP,#4\n"
				 "POP		{LR}\n"
				 "ADD		SP,SP,#4\n"
				 "CPSIE		I\n"
				 "BX		LR\n");
}

void __attribute__ ((naked)) SysTick_Handler(void) {
	asm volatile("CPSID		I\n"
				 "PUSH	{R4-R11}\n"
				 "LDR		R0,=currentPt\n"
				 "LDR		R1,[R0]\n"
				 "STR		SP,[R1]\n"
				 "LDR		R1,[R1,#4]\n"
				 "STR		R1,[R0]\n"
				 "LDR		SP,[R1]\n"
				 "POP		{R4-R11}\n"
				 "CPSIE		I\n"
				 "BX		LR\n");
}

void osThreadYield(void) {
	SysTick->VAL = 0;
	INTCTRL = 0x04000000;
}

void osSemaphoreInit(int32_t* semaphore, int32_t value) {
	*semaphore = value;
}

void osSignalSet(int32_t* semaphore) {
	__disable_irq();
	*semaphore += 1;
	__enable_irq();
}

void osSignalWait(int32_t* semaphore) {
	__disable_irq();
	while(*semaphore <= 0) {
		__disable_irq();
		osThreadYield();
		__enable_irq();
	}
	*semaphore -= 1;
	__enable_irq();
}
