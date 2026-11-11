#include <Arduino.h>

/*
  Ce code est utilisé pour programmer le ESP32-C3 dans le cadre de cours 243-E33 - Automne 2026

  Automne 2026 - Code modifié par Maxime Champagne pour le projet 243-E33.

  Pour L'instant:
  - Le code attend de recevoir un nom via le UART
  - Une fois le nom reçu, un serveur BLE portant le nom reçu est créé.
  - C'est une température simulée qui est transmise par BLE.

  Le code est initialement basé sur le code suivant:
  https://randomnerdtutorials.com/esp32-ble-server-environmental-sensing-service/

  Les UUID par défaut sont disponibles au lien suivant: https://www.bluetooth.com/specifications/assigned-numbers/
*/

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>


//defines pour le/les GPIOs utilisés
#define GPIO_USB_DM 18


//variables globales et #defines nécessaires au BLE
#define SERVICE_UUID (BLEUUID((uint16_t)0x181A))  // Default UUID for Environmental Sensing Service
#define bleServerName "MAX_TEMPERATURE"           //BLE server name
bool deviceConnected = false;
BLEServer *pServer = NULL;

BLECharacteristic temperaCharacteristic(BLEUUID((uint16_t)0x2A6E), BLECharacteristic::PROPERTY_READ);
//BLEDescriptor temperaDescriptor(BLEUUID((uint16_t)0x2902));

//variables globales et #defines nécessaires au décodage du UART
#define RX_BUFFER_SIZE 128

//variables globales et #defines nécessaires aux mesures
uint16_t temperature_u16 = 200;

//Prototype de fonction
void deviceDisconnected(void);
void initBLE(char *rxBuffer);
bool lireUART(char *rxBuffer);





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
  La fonction setup() est appelée une seule fois au démarrage du programme.
  Elle initialise la communication série, la communication BLE et les autres GPIOs
*/
void setup() {
  // Start serial communication 
  Serial.begin(115200);

  // Configure GPIO_USB_DM en sortie niveau logique haut - pour debug seulement
  pinMode(GPIO_USB_DM, OUTPUT);
  digitalWrite(GPIO_USB_DM, HIGH);
}




/*
  La fonction loop() est appelée en boucle après l'exécution de la fonction setup().
  Elle gère la connexion BLE et la mise à jour de la température simulée.
*/
void loop() {

  static char rxBuffer[RX_BUFFER_SIZE] = {0};
  static bool nomRecu = false;

  //Boucle qui fait clignotter un gpio au début de loop() pour indiquer que le code est vie
  for(uint8_t i = 0; i < 5; i++){
    digitalWrite(GPIO_USB_DM, !digitalRead(GPIO_USB_DM));
    delay(250);
  }

  if (false == nomRecu) {
    // Lire les données reçues sur le port série UART
    nomRecu = lireUART(rxBuffer);

    if (true == nomRecu){
      Serial.print("Trame reçue 3: ");
      Serial.println((char*)rxBuffer);      
      initBLE(rxBuffer);
    }
  }
  else{
    temperature_u16 = temperature_u16 + 100;
    temperaCharacteristic.setValue(temperature_u16);
  }

}

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
      rxIndex = 0;
      break;  // buffer plein, on ignore le reste de la trame;
    }
  }

  return rxCompleted;
}

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
  BLEService *bmeService = pServer->createService(SERVICE_UUID);

  // Create BLE Characteristics and corresponding Descriptors
  bmeService->addCharacteristic(&temperaCharacteristic);
  //temperaCharacteristic.addDescriptor(&temperaDescriptor);
  
  // Start the service
  bmeService->start();

  // Start advertising
  pServer->getAdvertising()->start(); // Start advertising

  temperaCharacteristic.setValue(temperature_u16);

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




