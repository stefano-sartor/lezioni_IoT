#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

/* configurazione per la luminosità del led*/
#define LEDC_CHANNEL_0 0
#define LEDC_TIMER_12_BIT 12
#define LEDC_BASE_FREQ 5000
#define LED_PIN 2

#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

class MyServerCallbacks : public BLEServerCallbacks {

  void onDisconnect(BLEServer *pServer) {
    Serial.println("client disconnected, restart advertising");
    delay(500); // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
  }
};

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    rxValue = pCharacteristic->getValue();

    int val;
    if (sscanf(rxValue.c_str(), "R%i;", &val) == 1) {
      if (val < 0 || val >= 10) {
        // valore oltre ai limiti,ritorniamo errore
        send_nak();
        return;
      }
      /* comando riconosciuto, eseguiamo*/
      execute_read(val);
      return;
    }

    if (sscanf(rxValue.c_str(), "W%i;", &val) == 1) {
      if (val < 0 || val > 255) {
        // valore oltre ai limiti,ritorniamo errore
        send_nak();
        return;
      }
      /* comando riconosciuto, eseguiamo*/
      execute_write(val);
      return;
    }
  }

  void execute_write(int val) {
    constexpr uint32_t valueMax = 255;
    uint32_t value = val;
    // calculate duty, 4095 from 2 ^ 12 - 1
    uint32_t duty = (4095 / valueMax) * min(value, valueMax);

    // write duty to LEDC
    ledcWrite(LEDC_CHANNEL_0, duty);
    std::string resp = "ACK,";
    resp += std::to_string(value) + ";";
    send(resp);

    Serial.print("->");
    Serial.println(resp.c_str());
  }

  void execute_read(int pin) {
    uint8_t pin_name = 0;

    switch (pin) {
    case 0:
      pin_name = T0;
      break;
    case 1:
      pin_name = T1;
      break;
    case 2:
      pin_name = T2;
      break;
    case 3:
      pin_name = T3;
      break;
    case 4:
      pin_name = T4;
      break;
    case 5:
      pin_name = T5;
      break;
    case 6:
      pin_name = T6;
      break;
    case 7:
      pin_name = T7;
      break;
    case 8:
      pin_name = T8;
      break;
    case 9:
      pin_name = T9;
      break;
    default:
      break;
    }

    touch_value_t value = touchRead(pin_name);
    std::string resp = "ACK,";
    resp += std::to_string(value) + ";";
    send(resp);

    Serial.print("->");
    Serial.println(resp.c_str());
  }

  void send_nak() {
    Serial.print("->");
    Serial.println(rxValue.c_str());
    send("NAK;");
  }

  void send(const std::string &msg) {
    pTxCharacteristic->setValue(msg);
    pTxCharacteristic->notify();
  }

  std::string rxValue;
};

void setup() {
  /* setup led*/
  ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_12_BIT);
  ledcAttachPin(LED_PIN, LEDC_CHANNEL_0);

  Serial.begin(115200);

  // Create the BLE Device
  BLEDevice::init("BLE-UART");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);

  //  Client Characteristic Configuration Descriptor (CCCD)
  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);

  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

void loop() {}