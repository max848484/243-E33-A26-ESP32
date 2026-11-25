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
#include "BLE.h"


typedef enum {
  ETAT_INIT,
  ETAT_DEBUT_TROUVE,
  ETAT_CONFIG,
  ETAT_DATA,
  ETAT_START,
  ETAT_STOP,
  ETAT_CONFIG_TYPE,
  ETAT_CONFIG_CHECKSUM,
  ETAT_START_BLE,
  ETAT_SET_TEMP,
  ETAT_SET_HUM,
  ETAT_SET_VENT_VIT,
  ETAT_SET_VENT_DIR,
  ETAT_SET_PLUIE,
  ETAT_SET_LUM,
  ETAT_VALEUR,
  ETAT_DATA_CHECKSUM,
  ETAT_SET_VALUE

} etatTrame;

typedef enum {
  SUCCES = 0,
  ERR_DEFAUT = 1,
  ERR_TYPE_MESURE = 3,  
  ERR_NOM_COMMANDE=4,
  ERR_CHECKSUM = 6,
  ERR_BLE_NOT_STARTED = 7,
  ERR_SERVICE_NOT_STARTED = 8,
  ERR_FIN_TRAME1 = 11,
  ERR_FIN_TRAME2 = 12,
  ERR_FIN_TRAME3 = 13,
  ERR_FIN_TRAME4 = 14,
  ERR_FIN_TRAME5 = 15,
  ERR_FIN_TRAME6 = 16
} erreurTrame;

typedef enum {
  NON_DEFINI,
  TEMP,
  HUM,
  VENT_VIT,
  VENT_DIR,
  PLUIE,
  LUM
} typeMesure;

//#defines nécessaires au uart 



//Prototype de fonction
uint8_t analyseTrame(char *rxMessage);
erreurTrame supprimeSeparateur(char *rxMessage, uint16_t index, uint8_t *longueurChamp);
bool verifyChecksum(char *rxMessage, uint16_t indexFin);



#endif //COMM_BLE_METEO_H