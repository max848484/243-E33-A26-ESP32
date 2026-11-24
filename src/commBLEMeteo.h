/*
  Fichier commBLEMeteo.h

  Fichier d'en-tête utilisé pour le décodage des messages qui sont nécessaires au contrôle du 
  service météo via le BLE sur l'ESP32-C3. 
*/

#ifndef COMM_BLE_METEO_H
#define COMM_BLE_METEO_H

#include <Arduino.h>
#include "uart.h"
#include <stdio.h>
#include <string.h>


typedef enum {
  ETAT_INIT,
  ETAT_DEBUT_TROUVE,
  ETAT_CONFIG,
  ETAT_DATA,
  ETAT_START,
  ETAT_STOP
} etatTrame;

typedef enum {
  SUCCES = 0,
  ERR_DEFAUT = 1,
  ERR_NOM_COMMANDE=4,
  ERR_FIN_TRAME1 = 11,
  ERR_FIN_TRAME2 = 12,
  ERR_FIN_TRAME3 = 13,
  ERR_FIN_TRAME4 = 14,
  ERR_FIN_TRAME5 = 15,
  ERR_FIN_TRAME6 = 16

} erreurTrame;

//#defines nécessaires au uart 



//Prototype de fonction
uint8_t analyseTrame(char *rxMessage);
erreurTrame supprimeSeparateur(char *rxMessage, uint16_t index, uint8_t *longueurChamp);



#endif //COMM_BLE_METEO_H