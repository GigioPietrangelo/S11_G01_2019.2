#ifndef __COMANDOS_H__
#define __COMANDOS_H__
// Standard
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
// Others
#include "cmsis_os2.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "inc/hw_memmap.h"

#define NUM_ANDARES 16

extern osMessageQueueId_t messageQueueElevadores[3];
extern osThreadId_t threadComandosId;

void threadComandos(void *arg);
void initAndares();
void decode(char *message);
bool setAndar(char *andar);
bool verifyAndar(char *andar);
char getAndar(char *andar);
void moveElevador(char *message);

#endif