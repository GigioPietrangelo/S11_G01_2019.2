#include "elevador.h"

osThreadId_t threadElevadorIds[3];

osMutexId_t osMutexId;
const osMutexAttr_t Thread_Mutex_attr = {
  "messageControlMutex",
  osMutexRecursive | osMutexPrioInherit,
  NULL,
  0U
};

void threadElevador(void *arg){
  int nElevador = ((int)arg);
  char outputString[10];
  char message[5];
  osStatus_t status;
  
  osMutexId = osMutexNew(&Thread_Mutex_attr); 
  
  while(true){
    status = osMessageQueueGet(messageQueueElevadores[nElevador], &message, NULL, NULL);
    if(status == osOK){
      switch (nElevador)
      {
      case 0:
        outputString[0] = 'e';
        break;
      case 1:
        outputString[0] = 'c';
        break;
      case 2:
        outputString[0] = 'd';
        break;
      default:
        break;
      }
      // Analisa comandos.
      switch (message[1])
      {
      case 'r': // INICIALIZA
        outputString[1] = message[1];
        outputString[2] = CR;
        break;
      case 'a': // ABRE PORTAS
        outputString[1] = message[1];
        outputString[2] = CR;
        break;
      case 'f': // FECHA PORTAS
        outputString[1] = message[1];
        outputString[2] = CR;
        break;
      case 's': // SOBE
        outputString[1] = message[1];
        outputString[2] = CR;
        break;
      case 'd': // DESCE
        outputString[1] = message[1];
        outputString[2] = CR;
        break;
      case 'p': // PARADA
        outputString[1] = message[1];  
        outputString[2] = CR;
        break;
      case 'L': // LIGA BOTÕES INTERNOS 
        outputString[1] = message[1];
        outputString[2] = message[2];
        outputString[3] = CR;
        break;
      case 'D': // DESLIGA BOTÕES INTERNOS 
        outputString[1] = message[1];
        outputString[2] = message[2];
        outputString[3] = CR;
        break;
      default:
        break;
      }
    
      osMutexAcquire(osMutexId, osWaitForever);
      sendString(outputString);
      osMutexRelease(osMutexId);
    }
  }
}
