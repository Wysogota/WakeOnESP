#include <WebServer.h>
#include "Adafruit_TinyUSB.h"
#include <Arduino_JSON.h>

WebServer server(80);
Adafruit_USBD_HID usb_hid;


void handleWakeup();
void handleStatus();
bool sendWakeUpSignal();
bool sendSuspendSignal();

// HID report descriptor using TinyUSB's template
// Single Report (no ID) descriptor
uint8_t const desc_hid_report[] = {
  TUD_HID_REPORT_DESC_KEYBOARD()
};

void setup() {
  Serial.begin(115200);

  usb_hid.setBootProtocol(HID_ITF_PROTOCOL_KEYBOARD);
  usb_hid.setPollInterval(2);
  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  usb_hid.setStringDescriptor("TinyUSB Keyboard");

  usb_hid.begin();

  // If already enumerated, additional class driverr begin() e.g msc, hid, midi won't take effect until re-enumeration
  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }

  connectToWiFi();

  // Setup HTTP server routes
  server.on("/suspend/on", HTTP_GET, handleSuspendOn);
  server.on("/suspend/off", HTTP_GET, handleSuspendOff);
  server.on("/status", HTTP_GET, handleStatus);
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



// /status route
void handleStatus() {
  const String status = TinyUSBDevice.suspended() ? "suspended" : "active";

  JSONVar response;
  response["status"] = status;
  response["usb_mounted"] = TinyUSBDevice.mounted();
  response["timestamp"] = getTime();
  String sResponse = JSON.stringify(response);

  server.send(200, "application/json", sResponse);
}

// /suspend/on route
void handleSuspendOn() {
  const bool succeeded = sendWakeUpSignal();
  JSONVar response;
  int statusCode;

  if (succeeded) {
    response["status"] = "ok";
    response["message"] = "Operation successful: system is now active.";
    statusCode = 200;
  } else {
    response["status"] = "error";
    response["message"] = "Operation conflict: system already awake.";
    statusCode = 409;
  }

  response["timestamp"] = getTime();
  String sResponse = JSON.stringify(response);
  server.send(200, "application/json", sResponse);
}

// /suspend/off route
void handleSuspendOff() {
  const bool succeeded = sendSuspendSignal();
  JSONVar response;
  int statusCode;

  if (succeeded) {
    response["status"] = "ok";
    response["message"] = "Operation successful: system is now suspended.";
    statusCode = 200;
  } else {
    response["status"] = "error";
    response["message"] = "Operation conflict: system already suspended.";
    statusCode = 409;
  }

  response["timestamp"] = getTime();
  String sResponse = JSON.stringify(response);
  server.send(200, "application/json", sResponse);
}

bool sendSuspendSignal() {
  if (!TinyUSBDevice.suspended()) {
    uint8_t const report_id = 0;
    // For keycode definition check out https://github.com/hathach/tinyusb/blob/master/src/class/hid/hid.h
    uint8_t modifiers = KEYBOARD_MODIFIER_LEFTCTRL | KEYBOARD_MODIFIER_LEFTSHIFT;  // Ctrl + Shift
    uint8_t keycode[1] = { HID_KEY_S };                                            // 'S'
    usb_hid.keyboardReport(report_id, modifiers, keycode);
    delay(100);
    usb_hid.keyboardRelease(0);

    return true;
  }
  return false;
}


bool sendWakeUpSignal() {
  if (TinyUSBDevice.suspended()) {
    // Wake up host if we are in suspend mode
    // and REMOTE_WAKEUP feature is enabled by host
    TinyUSBDevice.remoteWakeup();
    return true;
  }
  return false;
}
