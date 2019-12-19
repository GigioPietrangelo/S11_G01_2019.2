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
#include "comandos.h"
#include "elevador.h"

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


void app_main (void *argument)
{
  UARTInit();

  threadComandosId = osThreadNew(threadComandos, NULL, NULL);

  for(int i = 0; i < 3; i++)
  {
    threadElevadorIds[i] = osThreadNew(threadElevador, (void*) i, NULL);
    messageQueueElevadores[i] = osMessageQueueNew(15, 5, NULL);
  }       

  char init[10];
  init[0] = 'e';
  init[1] = 'r';
  osMessageQueuePut (messageQueueElevadores[0], &init, 0, NULL);
  
  osDelay(300);
  
  init[0] = 'c';
  osMessageQueuePut (messageQueueElevadores[1], &init, 0, NULL);
  
  osDelay(300);
  
  init[0] = 'd';
  osMessageQueuePut (messageQueueElevadores[2], &init, 0, NULL);
  
  while(true){
    osDelay(osWaitForever);
  }
}
void main(void)
{
  SystemCoreClockUpdate();
  osKernelInitialize();
 
  osThreadNew(app_main, NULL, NULL);
  
  if(osKernelGetState() == osKernelReady) {
    osKernelStart();
  }
  
  while(1);
} // main
