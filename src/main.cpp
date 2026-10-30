#include <Arduino.h>

/*
  Automne 2025 - Code modifié par Maxime Champagne pour le projet 243-E33.
  Pour L'instant, seule la température est simulée.

  Le code original se trouve à l'adresse suivante:
  https://randomnerdtutorials.com/esp32-ble-server-environmental-sensing-service/

  Les UUID par défaut sont disponibles au lien suivant: https://www.bluetooth.com/specifications/assigned-numbers/
*/

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>


//variables globales et #defines nécessaires au BLE
#define SERVICE_UUID (BLEUUID((uint16_t)0x181A))  // Default UUID for Environmental Sensing Service
#define bleServerName "MAX_TEMPERATURE"           //BLE server name
bool deviceConnected = false;
BLEServer *pServer = NULL;

BLECharacteristic temperaCharacteristic(BLEUUID((uint16_t)0x2A6E), BLECharacteristic::PROPERTY_READ);
//BLEDescriptor temperaDescriptor(BLEUUID((uint16_t)0x2902));

//variables globales et #defines nécessaires au décodage du UART
#define RX_BUFFER_SIZE 128
uint8_t rxIndex = 0;
bool receiving = false;

//variables globales et #defines nécessaires aux mesures
uint8_t temperature = 22;



//Setup callbacks onConnect and onDisconnect
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

void setup() {
  // Start serial communication 
  Serial.begin(115200);

  // Create the BLE Device
  BLEDevice::init(bleServerName);

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

}





void loop() {




  if (!deviceConnected){
    uint16_t iSecondes = 0;
    
    pServer->getAdvertising()->start(); // Start advertising

    Serial.println("Waiting a client connection to notify...");
    while((iSecondes < 30) && (!deviceConnected)){
      iSecondes ++;
      Serial.print(iSecondes);
      Serial.print(" - ");
      delay(1000);
    }

    pServer->getAdvertising()->stop(); // Stop advertising
  }
  else{
    //Incrémenter la temperature de 2 deg C à chaque lecture
    temperature += 2;
    uint16_t temperature_u16 = (uint16_t)temperature;
 
    //Met à jout la valeur dans la caractéristique
    temperaCharacteristic.setValue(temperature_u16);

    //Afficher la vitesse du vent dans le moniteur série
    Serial.print("Vitesse du vent: ");
    Serial.print("Temperature    : ");
    Serial.print(temperature);
    Serial.println(" deg C");
       
    delay(10000);
  }
}





/* //CODE PROPOSÉ PAR CHAT GPT POUR LE DÉCODAGE UART


#define RX_BUFFER_SIZE 128

char rxBuffer[RX_BUFFER_SIZE];
uint8_t rxIndex = 0;
bool receiving = false;

void receiveUART()
{
    while (Serial2.available() > 0)
    {
        char c = Serial2.read();

        // Détection du début de trame
        if (c == '$')
        {
            rxIndex = 0;
            receiving = true;
        }

        if (!receiving)
            continue;

        // Fin de trame
        if (c == '\n')
        {
            rxBuffer[rxIndex] = '\0';

            // La trame complète est maintenant disponible
            decodeFrame(rxBuffer);

            rxIndex = 0;
            receiving = false;
        }
        else if (c != '\r')
        {
            // Accumulation des caractères
            if (rxIndex < RX_BUFFER_SIZE - 1)
            {
                rxBuffer[rxIndex++] = c;
            }
            else
            {
                // Buffer plein : trame rejetée
                rxIndex = 0;
                receiving = false;
            }
        }
    }
}

void setup()
{
    Serial.begin(115200);

    // RX GPIO 16, TX GPIO 17 (à adapter à ta carte)
    Serial2.begin(115200, SERIAL_8N1, 16, 17);
}

void loop()
{
    receiveUART();
}


*/