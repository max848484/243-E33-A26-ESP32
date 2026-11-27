/*
  Fichier uart.cpp

  Fichier source pour la gestion du port série UART sur l'ESP32-C3.
*/
#include "uart.h"


/*
  La fonction lireUART() lit les données reçues sur le port série UART et les enregistre dans rxBuffer.
  Elle retourne true si le caractère de fin de trame (\n - 0x0A) a été reçu, sinon false.
*/
bool lireUART(char *rxBuffer) {
  static uint8_t rxIndex = 0;
  bool rxCompleted = false;


  while (Serial.available() > 0) {
    char c = (char)Serial.read();

    if (c == '\r') {
      continue;   // ignorer le retour chariot
    }

    if (c == '\n') {
      rxBuffer[rxIndex] = '\0';  // fin de chaîne
      rxIndex = 0;
      rxCompleted = true;
      break;  // sortir de la boucle while
    }

    if (rxIndex < RX_BUFFER_SIZE - 1) {
      rxBuffer[rxIndex] = c;
      rxIndex ++;
    } 
    else {
      rxBuffer[rxIndex - 1] = '\0';  // fin de chaîne
      rxIndex = 0;
      break;  // buffer plein, on ignore le reste de la trame;
    }
  }

  return rxCompleted;
}