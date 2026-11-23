/*
  Fichier BLE.h

  Fichier d'en-tête pour la gestion du serveur BLE sur l'ESP32-C3.
*/

#ifndef BLE_H
#define BLE_H

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Arduino.h>

//#defines nécessaires au BLE
#define SERVICE_UUID (BLEUUID((uint16_t)0x181A))  // Default UUID for Environmental Sensing Service
#define bleServerName "MAX_TEMPERATURE"           //BLE server name

//Prototype de fonction
void initBLE(char *rxBuffer);
void startTemperaCharacteristic(void);
void setTemperaCharacteristicValue(uint16_t value);
void deviceDisconnected(void);


#endif //BLE_H





