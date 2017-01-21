/************************************************************
   File: c:\asterix/system/kernel/include/os_tasks.h
   This file was created by the
   Obelix Configuration Compiler
   Fri Jan 13 14:03:19 2017
   Do not edit!
*************************************************************/

#ifndef __TASKS_H__
#define __TASKS_H__

#include <stdlib.h>
#include <sys_spec.h>
#include <os_kernel.h>

/************************************************************
   Mode mode_1
   Set up each individual mode and references needed to
   access each mode.
*************************************************************/

/************************************************************
   Function prototypes
*************************************************************/

extern C_task void idletask(void *);
extern void errorH(uint16);
extern C_task void tock(void *);
extern C_task void getLeftSensor(void *);
extern C_task void getOptSensor(void *);
extern C_task void getRightSensor(void *);
extern C_task void motorFunction(void *);
extern C_task void motorLeft(void *);
extern C_task void motorRight(void *);
extern C_task void irq1_isr(void *);
extern C_task void irqerror(uint16);
extern C_task void irq0_isr(void *);
/************************************************************
   End Function prototypes
*************************************************************/

/************************************************************
   Clock resolution
*************************************************************/

extern uint16 timerVal_mode_1;

/************************************************************
   Semaphores
*************************************************************/

#define __SEMAPHORE__
#include <os_semaphore.h>
extern semid_t semLeft;
extern semid_t semOpt;
extern semid_t semRight;

/************************************************************
   End Semaphores
*************************************************************/

/************************************************************
   Signals
*************************************************************/

#define __SIGNALS__
#include <os_signals.h>

extern signal_t signals_mode_1[4];
extern sigid_t IRQSIG0_mode_1;
extern sigid_t IRQSIG1_mode_1;
extern sigid_t leftSignal;
extern sigid_t rightSignal;

/************************************************************
   End Signals
*************************************************************/

/************************************************************
   List of semaphores taken for each task 
*************************************************************/

extern semaphorelist_t task_GETLEFTSENSOR_semaphoretaken_mode_1[2];
extern semaphorelist_t task_GETOPTSENSOR_semaphoretaken_mode_1[2];
extern semaphorelist_t task_GETRIGHTSENSOR_semaphoretaken_mode_1[2];
extern semaphorelist_t task_MOTORFUNCTION_semaphoretaken_mode_1[4];

/************************************************************
   End List of semaphores taken for each task 
*************************************************************/

/************************************************************
   TCB list
   T, O, DL, void (*startAddress)(void*), void *param,
   void (*errorHandler)(int), stackpointer, CCR default value,
   pointer to list of taken semaphores
*************************************************************/

extern TCB_t TCBList_mode_1[10];


/************************************************************
   End TCB list
*************************************************************/

/************************************************************
   Dispatch list
   STATE, Ticks to deadline, SP, void (*PC)(void), CCR, R0...R6,
   Ticks to new period, current priority, original priority
*************************************************************/

extern dispatch_t displist_mode_1[10];

/************************************************************
   End Dispatch list
*************************************************************/

/************************************************************
   Interrupts
*************************************************************/

#define __INTERRUPTS__
#define __IRQ__

/************************************************************
   End Interrupts
*************************************************************/

/************************************************************

   End mode mode_1
*************************************************************/

/************************************************************
   Mode setup
   Set up each individual mode and references needed to
   access each mode
*************************************************************/

extern uint16 timerVal;

extern uint16 NRTCBS;
extern uint16 NRTASKS;

extern uint16 mode_1;

extern uint16 curr_mode;

extern TCB_t *TCBList;
extern TCB_t *TCBList_arr[];

extern dispatch_t *displist;
extern dispatch_t *displist_arr[];

extern uint16 no_tasks_arr[];

extern uint16 timerVal_arr[];

extern uint16 stacksize;

extern stack_t stack_mem[620];

extern uint16 no_signals;
extern uint16 no_sig_arr[];

extern signal_t *signals;
extern signal_t *signals_arr[];

extern uint16 no_semaphores;
extern uint16 no_sem_arr[];

extern semaphore_t *semaphore;
extern semaphore_t *semaphore_arr[];

/************************************************************
   End mode setup
*************************************************************/

#endif  /* __TASKS_H__ */

/************************************************************
   End of file: c:\asterix/system/kernel/include/os_tasks.h
*************************************************************/

