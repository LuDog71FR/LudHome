#ifndef WIFI_H
#define WIFI_H

#include <Arduino.h>

extern bool wifi_status;
extern int rssi;

bool wifi_is_connected();
void disable_wifi();
void enable_wifi();

#endif
