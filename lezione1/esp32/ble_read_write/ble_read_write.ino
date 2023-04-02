#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

/* configurazione per la luminosità del led*/
#define LEDC_CHANNEL_0 0
#define LEDC_TIMER_12_BIT 12
#define LEDC_BASE_FREQ 5000
#define LED_PIN 2

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVIZIO_LED "e7bb0cf1-e802-44cf-a91e-e34a76051f55"
#define CHARACTERISTIC_LED "180f1920-5f0a-495c-ac28-d7151991cd81"

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();

    int vali = std::atoi(value.c_str());

    if (vali >= 0 && vali <= 255) {
      uint32_t valu = vali;
      constexpr uint32_t valueMax = 255;
      // calculate duty, 4095 from 2 ^ 12 - 1
      uint32_t duty = (4095 / valueMax) * min(valu, valueMax);
      // write duty to LEDC
      ledcWrite(LEDC_CHANNEL_0, duty);

      Serial.printf("LED impostato a %d\n\r", valu);
    } else {
      float v = ledcRead(LEDC_CHANNEL_0);
      v = v/4095 * 255;
      pCharacteristic->setValue(std::to_string(int(v)));
    }
  }
};

void setup() {
  /* setup led*/
  ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_12_BIT);
  ledcAttachPin(LED_PIN, LEDC_CHANNEL_0);

  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  BLEDevice::init("BLE-LED");
  BLEServer *server = BLEDevice::createServer();
  BLEService *servizio_led = server->createService(SERVIZIO_LED);
  BLECharacteristic *characteristic_led = servizio_led->createCharacteristic(
      CHARACTERISTIC_LED,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  characteristic_led->setCallbacks(new MyCallbacks());
  characteristic_led->setValue(std::to_string(0));
  servizio_led->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVIZIO_LED);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(
      0x06); // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(2000);
}