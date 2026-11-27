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
void startBleTempera(void);
void startBleVentVit(void);
void startBleVentDir(void);
void startBlePluie(void);
void startBleLumino(void);



void setBleTempera(uint16_t value);
void setBleHum(uint16_t value);
void setBleVentVit(uint16_t value);
void setBleVentDir(uint16_t value);
void setBlePluie(uint16_t value);
void setBleLumino(uint32_t value);


void deviceDisconnected(void);

//void clearBLE(void);


#endif //BLE_H





