
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

BLEServer *server = nullptr;
BLEService *servizio_touch = nullptr;
BLECharacteristic *characteristic_touch = nullptr;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID "6c942cdd-e463-4567-8bf7-99948cb8c63e"
#define CHARACTERISTIC_UUID "27f867a7-3956-4295-aba0-b13f53e060dd"


int threshold = 40;
bool testingLower = true;

void gotTouchEvent(){
  std::string event = testingLower ? "pressed" : "released";
  testingLower = !testingLower;
  touchInterruptSetThresholdDirection(testingLower);
  characteristic_touch->setValue(event);
  characteristic_touch->notify();
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  BLEDevice::init("BLE-TOUCH");
  server = BLEDevice::createServer();
  servizio_touch = server->createService(SERVICE_UUID);
  characteristic_touch = servizio_touch->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  characteristic_touch->setValue("released");
  servizio_touch->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x06); 
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  // interrupt per l'evento touch
  touchAttachInterrupt(T0, gotTouchEvent, threshold);
  touchInterruptSetThresholdDirection(testingLower);

  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void loop() {
}