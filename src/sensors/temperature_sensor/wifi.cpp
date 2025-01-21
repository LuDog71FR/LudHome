#include <ESP8266WiFi.h> 
#include "wifi.h"

bool wifi_status;
int rssi;

const char* mySSID   = "SFR_03AF";
const char* mySecKey = "fywci4lz9h8qpfyk944c";

bool wifi_is_connected() {
  int tryNumber = 1;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    if (++tryNumber > 10) {
      Serial.println("Wifi connection failed !");
      wifi_status = false;
      return false;
    }
  }

  wifi_status = true;
  return true;
}

void disable_wifi() {
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
  delay(1);
}

void enable_wifi() {
  WiFi.forceSleepWake();
  delay(1);

  // Disable the WiFi persistence. The ESP8266 will not load and save WiFi settings in the flash memory.
  WiFi.persistent(false);

  WiFi.mode(WIFI_STA);
  WiFi.begin(mySSID, mySecKey);

  wifi_is_connected();

  rssi = WiFi.RSSI();
}

