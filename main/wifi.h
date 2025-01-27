
#ifndef WIFI_H
#define WIFI_H

#include <WiFi.h>

const char* ssid = "ssid";
const char* password = "password";

const unsigned long wifiTimeout = 30000;  // 30 seconds timeout

void connectToWiFi();
void checkWifiConnection();

#endif
