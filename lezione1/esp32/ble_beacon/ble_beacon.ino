/*
   EddystoneURL beacon by BeeGee
   EddystoneURL frame specification https://github.com/google/eddystone/blob/master/eddystone-url/README.md

*/

/*
   Create a BLE server that will send periodic Eddystone URL frames.
   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create advertising data
   3. Start advertising.
   4. wait
   5. Stop advertising.
   6. deep sleep

*/
#include "sys/time.h"

#include <Arduino.h>

#include "BLEDevice.h"
#include "BLEUtils.h"
#include "BLEBeacon.h"
#include "BLEAdvertising.h"
#include "BLEEddystoneURL.h"

#include "esp_sleep.h"

#define GPIO_DEEP_SLEEP_DURATION 5    // sleep x seconds and then wake up
RTC_DATA_ATTR static time_t last;    // remember last boot in RTC Memory
RTC_DATA_ATTR static uint32_t bootcount; // remember number of boots in RTC Memory

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
BLEAdvertising *pAdvertising;
struct timeval now;

#define BEACON_UUID "0c40b713-b68f-4136-be27-e4475f6020a0" // UUID 1 128-Bit (may use linux tool uuidgen or random numbers via https://www.uuidgenerator.net/)
#define SERVICE_UUID "f5953c69-1425-4134-be32-378c4b9e3c87"

std::string beacon_data = "0";
std::string service_data = "0";

void setBeacon()
{
  BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
  BLEAdvertisementData oScanResponseData = BLEAdvertisementData();


  oAdvertisementData.setServiceData(BLEUUID(BEACON_UUID),beacon_data);
  oScanResponseData.setServiceData(BLEUUID(SERVICE_UUID),service_data);

  pAdvertising->setAdvertisementData(oAdvertisementData);
  pAdvertising->setScanResponseData(oScanResponseData);
}

void setup()
{

  Serial.begin(115200);
  gettimeofday(&now, NULL);

  beacon_data = std::to_string(bootcount);
  service_data = std::to_string(touchRead(T0));

  Serial.printf("start ESP32 %d\n", bootcount++);

  Serial.printf("deep sleep (%lds since last reset, %lds since last boot)\n", now.tv_sec, now.tv_sec - last);

  last = now.tv_sec;

  // Create the BLE Device
  BLEDevice::init("TOUCH-BEACON");

  BLEDevice::setPower(ESP_PWR_LVL_N12);

  // Create the BLE Server
  // BLEServer *pServer = BLEDevice::createServer(); // <-- no longer required to instantiate BLEServer, less flash and ram usage

  pAdvertising = BLEDevice::getAdvertising();

  setBeacon();
  // Start advertising
  pAdvertising->start();
  Serial.println("Advertizing started...");
  delay(2000);
  pAdvertising->stop();
  Serial.printf("enter deep sleep\n");
  esp_deep_sleep(1000000LL * GPIO_DEEP_SLEEP_DURATION);
  Serial.printf("in deep sleep\n");
}

void loop()
{
}