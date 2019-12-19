#ifndef __ELEVATOR_H__
#define __ELEVATOR_H__

// Standard
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
// Others
#include "cmsis_os2.h"
#include "comandos.h"
#include "string_utils.h"

#define CR 0x0D

extern osThreadId_t threadElevadorIds[3];

void threadElevador(void *arg);
void encode(char *message);

#endif
