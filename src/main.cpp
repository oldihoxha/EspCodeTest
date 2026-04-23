#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// UUIDs
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// GPIO für LED
const int LED_PIN = 2;

// BLE Server Pointer
BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;

// Callback Klasse für Characteristic
class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      
      if (value.length() > 0) {
        Serial.print("Received data: ");
        for (int i = 0; i < value.length(); i++) {
          Serial.print((char)value[i]);
        }
        Serial.println();
        
        // Prüfe auf '1' oder '0'
        if (value[0] == '1') {
          Serial.println("LED AN (HIGH)");
          digitalWrite(LED_PIN, HIGH);
        } 
        else if (value[0] == '0') {
          Serial.println("LED AUS (LOW)");
          digitalWrite(LED_PIN, LOW);
        }
      }
    }
};

// Callback Klasse für Server
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      Serial.println("Client verbunden!");
    }

    void onDisconnect(BLEServer* pServer) {
      Serial.println("Client getrennt!");
      // Advertising neu starten
      BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
      pAdvertising->start();
    }
};

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\nESP32 BLE Server startet...");
  
  // LED-Pin konfigurieren
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  Serial.println("LED-Pin konfiguriert (GPIO 2)");
  
  // BLE Device initialisieren
  BLEDevice::init("EspAppTest");
  Serial.println("BLE Device initialisiert");
  
  // BLE Server erstellen
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  Serial.println("BLE Server erstellt");
  
  // Service erstellen
  BLEService *pService = pServer->createService(SERVICE_UUID);
  Serial.println("BLE Service erstellt");
  
  // Characteristic erstellen (WRITE)
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_WRITE
                    );
  pCharacteristic->setCallbacks(new MyCallbacks());
  Serial.println("BLE Characteristic erstellt");
  
  // Service starten
  pService->start();
  Serial.println("BLE Service gestartet");
  
  // Advertising konfigurieren
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x0);  // Set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  
  Serial.println("BLE Advertising gestartet!");
  Serial.println("Gerätename: EspAppTest");
  Serial.println("Service UUID: 4fafc201-1fb5-459e-8fcc-c5c9c331914b");
  Serial.println("Characteristic UUID: beb5483e-36e1-4688-b7f5-ea07361b26a8");
  Serial.println("Bereit für Verbindungen...\n");
}

void loop() {
  delay(1000);
}
