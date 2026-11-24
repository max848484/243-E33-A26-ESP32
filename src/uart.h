/*
  Fichier uart.h

  Fichier d'en-tête pour la gestion du port série UART sur l'ESP32-C3.
*/

#ifndef UART_H
#define UART_H

#include <Arduino.h>

//#defines nécessaires au uart 
#define RX_BUFFER_SIZE 128


//Prototype de fonction
bool lireUART(char *rxBuffer);


#endif //UART_H