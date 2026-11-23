/*
  Fichier BLE.cpp

  Fichier source pour la gestion du serveur BLE sur l'ESP32-C3.
*/

#include "BLE.h"

// variables globales nécessaires au BLE
bool deviceConnected = false;
BLEServer *pServer = NULL;
BLEService *bmeService = NULL;

BLECharacteristic temperaCharacteristic(BLEUUID((uint16_t)0x2A6E), BLECharacteristic::PROPERTY_READ);
uint16_t bufMax = 200;
//BLEDescriptor temperaDescriptor(BLEUUID((uint16_t)0x2902));

/* 
  Classe MyServerCallbacks pour gérer les événements de connexion et de déconnexion du serveur BLE.
  Elle hérite de BLEServerCallbacks et redéfinit les méthodes onConnect et onDisconnect.
*/
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println("Device Connected");
  };
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("Device Disconnected");
  }
};



/*
  La fonction initBLE() initialise le serveur BLE
  En paramètre, elle reçoit un pointeur vers le buffer contenant le nom du serveur BLE à créer.
*/
void initBLE(char *rxBuffer){

  // Create the BLE Device
  BLEDevice::init(rxBuffer);
  //BLEDevice::init(bleServerName);

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  bmeService = pServer->createService(SERVICE_UUID);

  // Start advertising
  pServer->getAdvertising()->start(); // Start advertising

}


/*
  Cette fonction crée la caractéristique de température et l'associe au service BLE.
*/
void startTemperaCharacteristic(void) {

  if (bmeService != nullptr) {
    // Create BLE Characteristics and corresponding Descriptors
    bmeService->addCharacteristic(&temperaCharacteristic);
    //temperaCharacteristic.addDescriptor(&temperaDescriptor);

    // Start the service
    bmeService->start();

    temperaCharacteristic.setValue(bufMax);
  }
}

void setTemperaCharacteristicValue(uint16_t value) {
  if (bmeService != nullptr) {
    temperaCharacteristic.setValue(value);
  }
}




/*
  La fonction deviceDisconnected() est appelée lorsque le périphérique BLE est déconnecté.
  Elle démarre la publicité BLE et attend qu'un périphérique se connecte pendant 30 secondes.
*/
void deviceDisconnected(void){
  uint16_t iSecondes = 0;
  
  pServer->getAdvertising()->start(); // Start advertising

  Serial.println("Attente de connexion...");

  // Attendre la connexion d'un périphérique pendant 30 secondes
  while((iSecondes < 30) && (!deviceConnected)){
    iSecondes ++;
    Serial.print(iSecondes);
    Serial.print(" - ");
    delay(1000);
  }

  pServer->getAdvertising()->stop(); // Stop advertising
}
