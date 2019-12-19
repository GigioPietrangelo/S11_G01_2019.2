#include "comandos.h"

osMessageQueueId_t messageQueueElevadores[3];
osThreadId_t threadComandosId;

int nAndarE[NUM_ANDARES], nAndarC[NUM_ANDARES], nAndarD[NUM_ANDARES];
int andarAtualE = 0, andarAtualC = 0, andarAtualD = 0; // andar atual dos elevadores E,C e D
int botoesEc = 0, botoesCc = 0, botoesDc = 0; // Representa quantos botoes internos foram apertados em andares acima da posicao atual
int botoesEb = 0, botoesCb = 0, botoesDb = 0;// Representa quantos botoes internos foram apertados em andares abaixo da posicao atual
char sentidoE = 'p', sentidoC = 'p', sentidoD = 'p'; // Representa o sentido do movimento do elevador -> p = parado, c = cima, b =baixo

void threadComandos(void *arg){
  char input[5];
  
  initAndares();
  
  while(true){
    UARTgets(input, 5);
    decode(input);
  }
}

void initAndares(){
  for (int i = 0; i < NUM_ANDARES; i++) {
    nAndarE[i] = 0;
    nAndarC[i] = 0;
    nAndarD[i] = 0;
  }
}

void decode(char *message){
  char inputString[10];
  int nElevador;
  bool isMoving = false;
  bool andarset = false;
  
  // Define o elevador.
  switch (message[0]){
  case 'e':
    inputString[0] = 'e';
    nElevador = 0;
    break;
  case 'c':
    inputString[0] = 'c';
    nElevador = 1;
    break;
  case 'd':
    inputString[0] = 'd';
    nElevador = 2;
    break;
  default:
    break;
  }
  
  // Define o comando.
  switch (message[1])
  {
  case 'F': // PORTAS FECHADAS
    break;
  case 'A': // PORTAS ABERTAS
    break;
  case 'I': // BOTOES INTERNOS
    andarset = setAndar(message); 
    if(andarset){ // so liga a luz se nao for no mesmo andar atual do elevador
      inputString[1] = 'L';
      inputString[2] = message[2];
      osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);
    }
    // Define o elevador.
    switch (message[0]){
    case 'e':
      if(andarset && sentidoE == 'p'){ // so ativa o movimento se o elevador estiver parado. Foi feito dessa forma pq quando o elevador esta parado e um botao interno eh apertado, colocamos ele para andar. Quando mais botoes sao apertados, tratamos quando o elevador para e decide se deve continuar se movendo.
        moveElevador(message);
      }
      break;
    case 'c':
      if(andarset && sentidoC == 'p'){
        moveElevador(message);
      }
      break;
    case 'd':
      if(andarset && sentidoD == 'p'){
        moveElevador(message);
      }
      break;
    default:
      break;
    }
    break;
  case 'E': // BOTOES EXTERNOS
    andarset = setAndar(message);
    // Define o elevador.
    switch (message[0]){
    case 'e':
      if(andarset && sentidoE == 'p'){ // so ativa o movimento se o elevador estiver parado. Foi feito dessa forma pq quando o elevador esta parado e um botao interno eh apertado, colocamos ele para andar. Quando mais botoes sao apertados, tratamos quando o elevador para e decide se deve continuar se movendo.
        moveElevador(message);
      }
      break;
    case 'c':
      if(andarset && sentidoC == 'p'){
        moveElevador(message);
      }
      break;
    case 'd':
      if(andarset && sentidoD == 'p'){
        moveElevador(message);
      }
      break;
    default:
      break;
    }
    break;
  default:
    isMoving = true;
    break;
  }
  
  if(isMoving){
    bool shouldStop = verifyAndar(message);
    if (shouldStop){
      inputString[1] = 'p';
      osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);
      osDelay(300);
      inputString[1] = 'a';
      osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);
      osDelay(300);
      inputString[1] = 'D';
      char andar = getAndar(message);
      inputString[2] = andar;
      osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);

      
      switch (message[0]){ //para cada elevador, quando o movimento para ele verifica se algum botao interno em andar inferior ou superior foi apertado.
      case 'e':
        if(botoesEb>0 || botoesEc>0){ // caso sim, o movimento continua
          osDelay(1500);
          moveElevador(message);
        }
        else{
          sentidoE = 'p'; // senao ele volta pro estado parado
        }
        break;
      case 'c':
        if(botoesCb>0 || botoesCc>0){
          osDelay(1500);
          moveElevador(message);
        }
        else{
          sentidoC = 'p';
        }
        break;
      case 'd':
        if(botoesDb>0 || botoesDc>0){
          osDelay(1500);
          moveElevador(message);
        }
        else{
          sentidoD = 'p';
        }
        break;
      default:
      break;
      }
      
    }
  }
}

void moveElevador(char *message){
  char inputString[10];
  int nElevador;
  switch (message[0]){
  case 'e': // para o elevador e
    nElevador = 0;
    if(sentidoE == 'p'){ //se estiver parado -> executa o primeiro comando da fila
      if(botoesEc>0){ //se tiver botoes pressionados em andares acima
          botoesEc--; //reduzimos um botao apertado
          sentidoE = 'c'; // sentido de movimentacao pra cima
          osDelay(300);
          inputString[1] = 'f';
          osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);
          osDelay(300);
          inputString[1] = 's'; //sobe
          osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);
      }
      else if(botoesEb>0){ //se tiver botoes pressionados em andares abaixo
          botoesEb--; //reduzimos um botao apertado
          sentidoE = 'b';
          osDelay(300);
          inputString[1] = 'f';
          osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);
          osDelay(300);
          inputString[1] = 'd'; //desce
          osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);
      }
    }
    
    else if(sentidoE == 'c'){ // se ja estiver subindo
      if(botoesEc>0){// se tiver mais botoes apertados em andares acima
          botoesEc--; //reduzimos um botao apertado
          osDelay(300);
          inputString[1] = 'f';
          osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);
          osDelay(300);
          inputString[1] = 's'; //continua subindo
          osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);
      }
      else if(botoesEc == 0 && botoesEb>0){ // se nao tiver mais botoes apertados em andares acima e tiver botoes internos apertados em andares abaixo
          botoesEb--;//reduzimos um botao apertado
          sentidoE = 'b'; // inverte o movimento para descida
          osDelay(300);
          inputString[1] = 'f';
          osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);
          osDelay(300);
          inputString[1] = 'd'; // desce
          osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);
      }
      
      else if(botoesEc == 0 && botoesEb == 0){ // se nao tiver mais botoes internos apertados, paramos
          sentidoE = 'p';
      }
      else{sentidoE = 'p';}
    }
    
    else if(sentidoE == 'b'){// se ja estiver descendo
      if(botoesEb>0){ // se tiver mais botoes apertados em andares abaixo
          botoesEb--;//reduzimos um botao apertado
          osDelay(300);
          inputString[1] = 'f';
          osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);
          osDelay(300);
          inputString[1] = 'd';// continua descendo
          osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);
      }
      else if(botoesEb == 0 && botoesEc>0){// se nao tiver mais botoes apertados em andares abaixo e tiver botoes internos apertados em andares acima
          botoesEc--;//reduzimos um botao apertado
          sentidoE = 'c';// inverte o movimento para subida
          osDelay(300);
          inputString[1] = 'f';
          osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);
          osDelay(300);
          inputString[1] = 's';//comeca a subir
          osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);
      }
      
      else if(botoesEc == 0 && botoesEb == 0){// se nao tiver mais botoes internos apertados, paramos
          sentidoE = 'p';
      }
      //else{sentidoE == 'p';}
    }
    //else{sentidoE == 'p';}
    inputString[0] = 'e';
    break;
    
  case 'c': //mesma logica para o elevador C
    nElevador = 1;
    if(sentidoC == 'p'){
      if(botoesCc>0){
          botoesCc--;
          sentidoC = 'c';
          osDelay(300);
          inputString[1] = 'f';
          osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);
          osDelay(300);
          inputString[1] = 's';
          osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);
      }
      else if(botoesCb>0){
          botoesCb--;
          sentidoE = 'b';
          osDelay(300);
          inputString[1] = 'f';
          osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);
          osDelay(300);
          inputString[1] = 'd';
          osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);
      }
    }
    
    else if(sentidoC == 'c'){
      if(botoesCc>0){
          botoesCc--;
          osDelay(300);
          inputString[1] = 'f';
          osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);
          osDelay(300);
          inputString[1] = 's';
          osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);
      }
      else if(botoesCc == 0 && botoesCb>0){
          botoesCb--;
          sentidoC = 'b';
          osDelay(300);
          inputString[1] = 'f';
          osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);
          osDelay(300);
          inputString[1] = 'd';
          osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);
      }
      
      else if(botoesCc == 0 && botoesCb == 0){
          sentidoC = 'p';
      }
      //else{sentidoC == 'p';}
    }
    
    else if(sentidoC == 'b'){
      if(botoesCb>0){ 
          botoesCb--;
          osDelay(300);
          inputString[1] = 'f';
          osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);
          osDelay(300);
          inputString[1] = 'd';
          osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);
      }
      else if(botoesCb == 0 && botoesCc>0){
          botoesCc--;
          sentidoC = 'c';
          osDelay(300);
          inputString[1] = 'f';
          osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);
          osDelay(300);
          inputString[1] = 's';
          osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);
      }
      
      else if(botoesCc == 0 && botoesCb == 0){
          sentidoC = 'p';
      }
      //else{sentidoC == 'p';}
    }
    //else{sentidoC == 'p';}
    inputString[0] = 'c';
    break;
    
  case 'd'://mesma logica para o elevador D
    nElevador = 2;
    if(sentidoD == 'p'){
      if(botoesDc>0){
          botoesDc--;
          sentidoD = 'c';
          osDelay(300);
          inputString[1] = 'f';
          osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);
          osDelay(300);
          inputString[1] = 's';
          osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);
      }
      else if(botoesDb>0){
          botoesDb--;
          sentidoD = 'b';
          osDelay(300);
          inputString[1] = 'f';
          osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);
          osDelay(300);
          inputString[1] = 'd';
          osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);
      }
    }
    
    else if(sentidoD == 'c'){
      if(botoesDc>0){
          botoesDc--;
          osDelay(300);
          inputString[1] = 'f';
          osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);
          osDelay(300);
          inputString[1] = 's';
          osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);
      }
      else if(botoesDc == 0 && botoesDb>0){
          botoesDb--;
          sentidoD = 'b';
          osDelay(300);
          inputString[1] = 'f';
          osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);
          osDelay(300);
          inputString[1] = 'd';
          osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);
      }
      
      else if(botoesDc == 0 && botoesDb == 0){
          sentidoD = 'p';
      }
      else{}
    }
    
    else if(sentidoD == 'b'){
      if(botoesDb>0){ 
          botoesDb--;
          osDelay(300);
          inputString[1] = 'f';
          osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);
          osDelay(300);
          inputString[1] = 'd';
          osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);
      }
      else if(botoesDb == 0 && botoesDc>0){
          botoesDc--;
          sentidoD = 'c';
          osDelay(300);
          inputString[1] = 'f';
          osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);
          osDelay(300);
          inputString[1] = 's';
          osMessageQueuePut (messageQueueElevadores[nElevador], &inputString, 0, NULL);
      }
      
      else if(botoesDc == 0 && botoesDb == 0){
          sentidoD = 'p';
      }
      else{}
    }
    else{
    }
    inputString[0] = 'd';
    break;
  }  
}

bool setAndar(char *andar){
  
  int andarSelecionado;
  
  if (andar[1] == 'E') { // Botões externos pressionados.
    int a = (int)andar[2]; // - 48;
    if (a == 48) { // == 0
      andarSelecionado = (int)andar[3] - 48; // andar(0..9)
    } else {
      andarSelecionado = ((int)andar[3] - 48) + 10; // andar(10..15)
    }
  } 
  else {
    switch (andar[2]){
    case 'a':
      andarSelecionado = 0;
      break;
    case 'b':
      andarSelecionado = 1;
      break;
    case 'c':
      andarSelecionado = 2;
      break;
    case 'd':
      andarSelecionado = 3;
      break;
    case 'e':
      andarSelecionado = 4;
      break;
    case 'f':
      andarSelecionado = 5;
      break;
    case 'g':
      andarSelecionado = 6;
      break;
    case 'h':
      andarSelecionado = 7;
      break;
    case 'i':
      andarSelecionado = 8;
      break;
    case 'j':
      andarSelecionado = 9;
      break;
    case 'k':
      andarSelecionado = 10;
      break;
    case 'l':
      andarSelecionado = 11;
      break;
    case 'm':
      andarSelecionado = 12;
      break;
    case 'n':
      andarSelecionado = 13;
      break;
    case 'o':
      andarSelecionado = 14;
      break;
    case 'p':
      andarSelecionado = 15;
      break;
    default:
      printf("Error: No floor selected.\n"); // DEBUG
      andarSelecionado = 0;
      break;
    }
  }
  
  switch (andar[0]){
  case 'e':
    nAndarE[andarSelecionado] = 1;
    if(andarSelecionado > andarAtualE){ //se o andar selecionado for maior que o atual
      botoesEc++; //aumentamos a quantidade de botoes para cima apertados
      return true;
    }
    else if(andarSelecionado == andarAtualE){//se o andar selecionado for o mesmo que o atual
      return false; // nao fazemos nada
    }
    else{ //se o andar selecionado for menor que o atual
      botoesEb++; //aumentamos a quantidade de botoes para baixo apertados
      return true;
    }
    break;
  case 'c':
    nAndarC[andarSelecionado] = 1;
    if(andarSelecionado > andarAtualC){
      botoesCc++;
      return true;
    }
    else if(andarSelecionado == andarAtualC){
      return false;
    }
    else{
      botoesCb++;
      return true;
    }
    break;
  case 'd':
    nAndarD[andarSelecionado] = 1;
    if(andarSelecionado > andarAtualD){
      botoesDc++;
      return true;
    }    
    else if(andarSelecionado == andarAtualD){
      return false;
    }
    else{
      botoesDb++;
      return true;
    }
    break;
  default:
    break;
  }
  return false;
}

bool verifyAndar(char *andar){
  
  int lenght = strlen(andar);
  
  if (lenght == 2) { // Andar < 10
    // Pega o andar atual.
    int a = (int)andar[1] - 48;
    // Verifica o elevador.
    if (andar[0] == 'e') {
      // Verifica se o andar foi selecionado.
      if (nAndarE[a] == 1) {
        andarAtualE = a;
        nAndarE[a] = 0;
        return true;
      } 
      else {
        return false;
      }
    }
    else if (andar[0] == 'c') {
       andarAtualC = a;
      // Verifica se o andar foi selecionado.
      if (nAndarC[a] == 1) {
        nAndarC[a] = 0;
        return true;
      } 
      else {
        return false;
      }
    } 
    else if (andar[0] == 'd'){
      andarAtualD = a;
      // Verifica se o andar foi selecionado.
      if (nAndarD[a] == 1) { 
        nAndarD[a] = 0;
        return true;
      } 
      else {
        return false;
      }
    }
    else {
      return false;
    }
  }
  else if (lenght == 3){ // Andar >= 10
    // Pega o andar atual.
    int a = ((int)andar[2] - 48) + 10;
    // Verifica o elevador.
    if (andar[0] == 'e') {
        andarAtualE = a;
      // Verifica se o andar foi selecionado.
      if (nAndarE[a] == 1) {
        nAndarE[a] = 0;
        return true;
      } 
      else {
        return false;
      }
    }
    else if (andar[0] == 'c') {
      nAndarC[a] = 0;
      // Verifica se o andar foi selecionado.
      if (nAndarC[a] == 1) {
        andarAtualC = a;
        return true;
      } 
      else {
        return false;
      }
    } 
    else if (andar[0] == 'd'){
      // Verifica se o andar foi selecionado.
      if (nAndarD[a] == 1) {
        andarAtualD = a;
        nAndarD[a] = 0;
        return true;
      } 
      else {
        return false;
      }
    }
    else {
      return false;
    }
  }
  else {
    return false;
  }
}

char getAndar(char *andar){
  
  int andarParado;
  char andarLetra;
  
  switch (andar[0]){
  case 'e':
    andarParado = andarAtualE;
    break;
  case 'c':
    andarParado = andarAtualC;
    break;
  case 'd':
    andarParado = andarAtualD;
    break;
  default:
    break;
  }
  
  switch (andarParado){
  case 0:
    andarLetra = 'a';
    break;
  case 1:
    andarLetra = 'b';
    break;
  case 2:
    andarLetra = 'c';
    break;
  case 3:
    andarLetra = 'd';
    break;
  case 4:
    andarLetra = 'e';
    break;
  case 5:
    andarLetra = 'f';
    break;
  case 6:
    andarLetra = 'g';
    break;
  case 7:
    andarLetra = 'h';
    break;
  case 8:
    andarLetra = 'i';
    break;
  case 9:
    andarLetra = 'j';
    break;
  case 10:
    andarLetra = 'k';
    break;
  case 11:
    andarLetra = 'l';
    break;
  case 12:
    andarLetra = 'm';
    break;
  case 13:
    andarLetra = 'n';
    break;
  case 14:
    andarLetra = 'o';
    break;
  case 15:
    andarLetra = 'p';
    break;
  default:
    printf("Error: No current floor.\n"); // DEBUG
    andarLetra = 'a';
    break;
  }
  
  return andarLetra;
}