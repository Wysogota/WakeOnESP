#include <WebServer.h>

WebServer server(80);

void setup() {
  Serial.begin(115200);
  initUSBDevice();
  connectToWiFi();
  routes();
  server.begin();
}

void loop() {
  // Handle incoming client requests
  server.handleClient();

#ifdef TINYUSB_NEED_POLLING_TASK
  // Manual call tud_task since it isn't called by Core's background
  TinyUSBDevice.task();
#endif

  checkWifiConnection();
}
