#include "wifi.h"

// Function to connect to WiFi
void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");

  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < wifiTimeout) {
    delay(1000);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi");
  } else {
    Serial.println("\nFailed to connect to WiFi");
  }
}

// Reconnect to WiFi if disconnected
void checkWifiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  }
}
