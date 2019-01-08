#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#include <Adafruit_NeoPixel.h>

#define LEDPIN 14
Adafruit_NeoPixel strip = Adafruit_NeoPixel(3, LEDPIN, NEO_GRB + NEO_KHZ800);

uint32_t colorConnected = strip.Color(255, 255, 255); 
uint32_t colorDisconnected = strip.Color(255, 0, 0); 

const String scannerName = "iron";

static BLEAddress *pServerAddress;
BLEScanResults foundDevices;

int scanTime = 3; // in seconds
const String tagAddress = "ff:ff:b1:a7:c2:93"; // mac address of keyring
uint32_t lastconnectedTime = 0;
uint32_t connectionTimeout = 10 * 1000;
bool deviceFound = false;


class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
   pServerAddress = new BLEAddress(advertisedDevice.getAddress());

    if (strcmp(pServerAddress->toString().c_str(), tagAddress.c_str()) == 0) {
      int bleRssi = advertisedDevice.getRSSI() * -1;
      Serial.print("rssi: ");
      Serial.println(bleRssi);
      if (bleRssi < 75) {
        lastconnectedTime = millis();
        deviceFound = true;
      }
    }
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("===== Starting Iron Mask =====");

  strip.begin();
  strip.show();
  strip.setBrightness(64);
  setNeoPixels();

  BLEDevice::init(scannerName.c_str());

}

void loop() {
  Serial.println("===== Scanning... =====");
  BLEScan* pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  foundDevices = pBLEScan->start(scanTime);

  if ((lastconnectedTime + connectionTimeout) < millis()) {
    deviceFound = false;
  }

  Serial.print("device connected: ");
  Serial.println(deviceFound);

  setNeoPixels();
  
  delay(1500);
}

void setNeoPixels() {
  for (uint8_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, ((deviceFound) ? colorConnected : colorDisconnected));
  }
  strip.show();
}

