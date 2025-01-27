#include "routes.h"

void routes() {
  server.on("/suspend/on", HTTP_GET, handleSuspendOn);
  server.on("/suspend/off", HTTP_GET, handleSuspendOff);
  server.on("/status", HTTP_GET, handleStatus);
}

/* STATUS */
void handleStatus() {
  const String status = TinyUSBDevice.suspended() ? "suspended" : "active";

  JSONVar response;
  response["status"] = status;
  response["usb_mounted"] = TinyUSBDevice.mounted();
  response["timestamp"] = getTime();
  String sResponse = JSON.stringify(response);

  server.send(200, "application/json", sResponse);
}

/* WAKE UP */
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


bool sendWakeUpSignal() {
  if (TinyUSBDevice.suspended()) {
    // Wake up host if we are in suspend mode
    // and REMOTE_WAKEUP feature is enabled by host
    TinyUSBDevice.remoteWakeup();
    return true;
  }
  return false;
}

/* SUSPEND */
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
