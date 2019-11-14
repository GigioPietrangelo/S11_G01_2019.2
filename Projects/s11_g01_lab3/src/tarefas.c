/*
Universidade Tecnológica Federal do Paraná
EL68E - Sistemas Embarcados
Prof. Hugo Vieira Neto  
Alunos: 
  Giovanni
  Juliana
  Rodrigo
Turma:  
  S11
Equipe: 
  G01
Semestre: 
  2019.2
*/

// Standard
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
// Driverlib
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/pin_map.h"
#include "utils/uartstdio.h"
 
#include "system_tm4c1294.h" // CMSIS-Core
#include "driverleds.h" // device drivers
#include "cmsis_os2.h" // CMSIS-RTOS

osThreadId_t thread1_id, thread2_id;

void thread1(void *arg){
  uint8_t state = 0;
  
  while(1){
    state ^= LED1;
    LEDWrite(LED1, state);
    osDelay(100);
  } // while
} // thread1

void thread2(void *arg){
  uint8_t state = 0;
  uint32_t tick;
  
  while(1){
    tick = osKernelGetTickCount();
    
    state ^= LED2;
    LEDWrite(LED2, state);
    
    osDelayUntil(tick + 100);
  } // while
} // thread2

void main(void){
  //UARTInit();
  
  LEDInit(LED2 | LED1);

  osKernelInitialize();

  thread1_id = osThreadNew(thread1, NULL, NULL);
  thread2_id = osThreadNew(thread2, NULL, NULL);

  if(osKernelGetState() == osKernelReady)
    osKernelStart();

  while(1);
} // main
