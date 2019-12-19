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
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/pin_map.h"
// Utils
#include "utils/uartstdio.h"
// Others
#include "inc/hw_memmap.h"
#include "system_tm4c1294.h" // CMSIS-Core
#include "driverleds.h" // device drivers
#include "cmsis_os2.h" // CMSIS-RTOS

extern void UARTStdioIntHandler(void);

void UARTInit(void){
  // Enable the GPIO Peripheral used by the UART.
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));

  // Enable UART0
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0));

  // Configure GPIO Pins for UART mode.
  GPIOPinConfigure(GPIO_PA0_U0RX);
  GPIOPinConfigure(GPIO_PA1_U0TX);
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

  // Initialize the UART for console I/O.
  UARTStdioConfig(0, 115200, SystemCoreClock);
  
} // UARTInit

void UART0_Handler(void){
  UARTStdioIntHandler();
} // UART0_Handler

/*osThreadId_t thread1_id, thread2_id;

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
  char var[10];
  char var1[5] = "dLc\r";
  while(1){
    tick = osKernelGetTickCount();
    
    state ^= LED2;
    LEDWrite(LED2, state);
    UARTgets(var, 10);
    UARTwrite(var1, 10);
    printf("O nome armazenado foi: %s", var);
    osDelayUntil(tick + 100);
  } // while
} // thread2
*/

void app_main (void *argument){

  for(uint8_t i = 0; i < 3; i++){
    threadElevatorIds[i] = osThreadNew(threadElevator, (void*) i, NULL);
  }
  
  while(true){
    osDelay(osWaitForever);
  }
}

void main(void){
  UARTInit();
  
  //LEDInit(LED2 | LED1);
  SystemCoreClockUpdate();
  osKernelInitialize();

  //thread1_id = osThreadNew(thread1, NULL, NULL);
  //thread2_id = osThreadNew(thread2, NULL, NULL);
	
  if(osKernelGetState() == osKernelReady)
    osKernelStart();
    
  while(1){
    
  }
    
} // main
