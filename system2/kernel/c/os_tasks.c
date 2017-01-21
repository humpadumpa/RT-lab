/************************************************************
   File: c:\asterix/system/kernel/c/os_tasks.c
   This file was created by the
   Obelix Configuration Compiler
   Fri Jan 13 14:03:19 2017
   Do not edit!
*************************************************************/

#include <os_tasks.h>

/************************************************************
   Mode mode_1
   Set up each individual mode and references needed to
   access each mode.
*************************************************************/

/************************************************************
   Clock resolution
*************************************************************/

uint16 timerVal_mode_1 = 500;

/************************************************************
   Semaphores
*************************************************************/

taskid_t sem0_mode_1[] = {5,2};
semid_t semLeft = 0;
taskid_t sem1_mode_1[] = {5,3};
semid_t semOpt = 1;
taskid_t sem2_mode_1[] = {5,4};
semid_t semRight = 2;
semaphore_t semaphore_mode_1[3] = {{sem0_mode_1,2,6},{sem1_mode_1,2,7},{sem2_mode_1,2,8}};

/************************************************************
   End Semaphpores
*************************************************************/

/************************************************************
   Signals
*************************************************************/

taskid_t mode_1_sig0_1[] = {9};
taskid_t mode_1_sig0_2[] = {9};
sigid_t IRQSIG0_mode_1 = 0;
taskid_t mode_1_sig1_1[] = {8};
taskid_t mode_1_sig1_2[] = {8};
sigid_t IRQSIG1_mode_1 = 1;
taskid_t mode_1_sig2_1[] = {6,5};
taskid_t mode_1_sig2_2[] = {6,0};
sigid_t leftSignal = 2;
taskid_t mode_1_sig3_1[] = {7,5};
taskid_t mode_1_sig3_2[] = {7,0};
sigid_t rightSignal = 3;
signal_t signals_mode_1[4] = {{mode_1_sig0_1,1,mode_1_sig0_2,1},{mode_1_sig1_1,1,mode_1_sig1_2,1},{mode_1_sig2_1,2,mode_1_sig2_2,1},{mode_1_sig3_1,2,mode_1_sig3_2,1}};

/************************************************************
   End Signals
*************************************************************/

/************************************************************
   List of semaphores taken for each task 
*************************************************************/

semaphorelist_t task_GETLEFTSENSOR_semaphoretaken_mode_1[2]={1,0};
semaphorelist_t task_GETOPTSENSOR_semaphoretaken_mode_1[2]={1,2};
semaphorelist_t task_GETRIGHTSENSOR_semaphoretaken_mode_1[2]={1,4};
semaphorelist_t task_MOTORFUNCTION_semaphoretaken_mode_1[4]={3,0,2,4};

/************************************************************
   End List of semaphores taken for each task 
*************************************************************/

/************************************************************
   TCB list
   T, O, DL, void (*startAddress)(void*), void *param,
   void (*errorHandler)(int), stackpointer, CCR default value,
   pointer to list of taken semaphores
*************************************************************/

TCB_t TCBList_mode_1[10] = {
{0,0,0,idletask,NULL,(errorfunc_t)errorH,DEFAULT_CCR,NULL,NULL},
{10,0,10,tock,NULL,(errorfunc_t)errorH,DEFAULT_CCR,NULL,NULL},
{20,0,20,getLeftSensor,NULL,(errorfunc_t)errorH,DEFAULT_CCR,NULL,task_GETLEFTSENSOR_semaphoretaken_mode_1},
{20,0,20,getOptSensor,NULL,(errorfunc_t)errorH,DEFAULT_CCR,NULL,task_GETOPTSENSOR_semaphoretaken_mode_1},
{20,0,20,getRightSensor,NULL,(errorfunc_t)errorH,DEFAULT_CCR,NULL,task_GETRIGHTSENSOR_semaphoretaken_mode_1},
{50,0,50,motorFunction,NULL,(errorfunc_t)errorH,DEFAULT_CCR,NULL,task_MOTORFUNCTION_semaphoretaken_mode_1},
{0,0,10,motorLeft,NULL,(errorfunc_t)errorH,DEFAULT_CCR,NULL,NULL},
{0,0,10,motorRight,NULL,(errorfunc_t)errorH,DEFAULT_CCR,NULL,NULL},
{0,0,50,irq1_isr,NULL,(errorfunc_t)irqerror,DEFAULT_CCR,NULL,NULL},
{0,0,50,irq0_isr,NULL,(errorfunc_t)irqerror,DEFAULT_CCR,NULL,NULL}};

/************************************************************
   End TCB list
*************************************************************/

/************************************************************
   Dispatch list
   STATE, Ticks to deadline, SP, void (*PC)(void), CCR, R0...R6,
   Ticks to new period, current priority, original priority
*************************************************************/

dispatch_t displist_mode_1[10] = {
{SIG_BLOCKED,0,idletask,0,(cpureg_t)NULL,0,0,0,0,0,0,0,0,0,},
{WAITING,10,tock,0,(cpureg_t)NULL,0,0,0,0,0,0,0,1,1,},
{WAITING,20,getLeftSensor,0,(cpureg_t)NULL,0,0,0,0,0,0,0,2,2,},
{WAITING,20,getOptSensor,0,(cpureg_t)NULL,0,0,0,0,0,0,0,3,3,},
{WAITING,20,getRightSensor,0,(cpureg_t)NULL,0,0,0,0,0,0,0,4,4,},
{WAITING,50,motorFunction,0,(cpureg_t)NULL,0,0,0,0,0,0,0,5,5,},
{SIG_BLOCKED,10,motorLeft,0,(cpureg_t)NULL,0,0,0,0,0,0,0,9,9,},
{SIG_BLOCKED,10,motorRight,0,(cpureg_t)NULL,0,0,0,0,0,0,0,10,10,},
{SIG_BLOCKED,50,irq1_isr,0,(cpureg_t)NULL,0,0,0,0,0,0,0,11,11,},
{SIG_BLOCKED,50,irq0_isr,0,(cpureg_t)NULL,0,0,0,0,0,0,0,12,12,}};

/************************************************************
   End Dispatch list
*************************************************************/

/************************************************************

   End mode mode_1
*************************************************************/

/************************************************************
   Mode setup
   Set up each individual mode and references needed to
   access each mode
*************************************************************/

uint16 timerVal;

uint16 NRTCBS;
uint16 NRTASKS;

uint16 mode_1 = 0;

uint16 curr_mode;

TCB_t *TCBList;
TCB_t *TCBList_arr[] = { TCBList_mode_1 };

dispatch_t *displist;
dispatch_t *displist_arr[] = { displist_mode_1 };

uint16 no_tasks_arr[] = {10};

uint16 timerVal_arr[] = {500};

uint16 stacksize = 620;

stack_t stack_mem[620];

uint16 no_signals;
uint16 no_sig_arr[] = {4};

signal_t *signals;
signal_t *signals_arr[] = { signals_mode_1 };

uint16 no_semaphores;
uint16 no_sem_arr[] = {3};

semaphore_t *semaphore;
semaphore_t *semaphore_arr[] = { semaphore_mode_1 };

/************************************************************
   End mode setup
*************************************************************/

/************************************************************
   End of file: c:\asterix/system/kernel/c/os_tasks.c
*************************************************************/

