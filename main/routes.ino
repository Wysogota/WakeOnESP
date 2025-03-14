#include "routes.h"

void routes() {
  server.on("/suspend/on", HTTP_GET, handleSuspendOn);
  server.on("/suspend/off", HTTP_GET, handleSuspendOff);
  server.on("/status", HTTP_GET, handleStatus);
  server.on("/", HTTP_GET, handleHome);
}

bool waitUntil(bool suspended) {
  unsigned long startTime = millis();

  while (TinyUSBDevice.suspended() != suspended) {
    if (millis() - startTime > 10000) {
      return false;
    }
    delay(50);
  }

  return true;
}

String formatTimestamp(long timestamp) {
  if (timestamp == 0) return "Never";

  time_t rawTime = timestamp;
  struct tm* timeInfo = localtime(&rawTime);

  char formattedTime[20];
  strftime(formattedTime, sizeof(formattedTime), "%d %b %Y %H:%M:%S", timeInfo);

  return String(formattedTime);
}

auto createResponse(char* status, char* message) {
  JSONVar response;
  response["status"] = status;
  response["message"] = message;
  response["timestamp"] = getTime();
  return response;
}

/* STATUS */
auto getStatus() {
  JSONVar response;
  response["status"] = TinyUSBDevice.suspended() ? "suspended" : "awaken";
  response["usb_mounted"] = TinyUSBDevice.mounted();
  response["last_awakened"] = last_awakened;
  response["last_suspended"] = last_suspended;
  response["timestamp"] = getTime();
  return response;
}

void handleStatus() {
  JSONVar response = getStatus();
  String sResponse = JSON.stringify(response);
  server.send(200, "application/json", sResponse);
}

/* WAKE UP */
void handleSuspendOn() {
  int statusCode;
  JSONVar response;

  if (!TinyUSBDevice.suspended()) {
    statusCode = 409;
    response = createResponse("error", "Operation conflict: system already awake.");

  } else {
    const bool succeeded = sendWakeUpSignal();
    if (succeeded) {
      statusCode = 200;
      response = createResponse("ok", "Operation successful: system is now awaken.");
      last_awakened = response["timestamp"];
    } else {
      statusCode = 408;
      response = createResponse("error", "Operation error: Timeout");
    }
  }

  String sResponse = JSON.stringify(response);
  server.send(statusCode, "application/json", sResponse);
}

bool sendWakeUpSignal() {
  if (!TinyUSBDevice.suspended()) return false;
  TinyUSBDevice.remoteWakeup();  // REMOTE_WAKEUP feature should be enabled by host (See README)
  return waitUntil(SUSPENDED);
}

/* SUSPEND */
void handleSuspendOff() {
  int statusCode;
  JSONVar response;

  if (TinyUSBDevice.suspended()) {
    statusCode = 409;
    response = createResponse("error", "Operation conflict: system already suspended.");
  } else {
    const bool succeeded = sendSuspendSignal();
    if (succeeded) {
      statusCode = 200;
      response = createResponse("ok", "Operation successful: system is now suspended.");
      last_suspended = response["timestamp"];
    } else {
      statusCode = 408;
      response = createResponse("error", "Operation error: Timeout");
    }
  }

  String sResponse = JSON.stringify(response);
  server.send(statusCode, "application/json", sResponse);
}

bool sendSuspendSignal() {
  if (TinyUSBDevice.suspended()) return false;

  uint8_t const report_id = 0;
  // For keycode definition check out https://github.com/hathach/tinyusb/blob/master/src/class/hid/hid.h
  uint8_t modifiers = KEYBOARD_MODIFIER_LEFTCTRL | KEYBOARD_MODIFIER_LEFTSHIFT;  // Ctrl + Shift
  uint8_t keycode[1] = { HID_KEY_S };                                            // 'S'
  usb_hid.keyboardReport(report_id, modifiers, keycode);
  delay(100);
  usb_hid.keyboardRelease(0);

  return waitUntil(AWAKENED);
}

/* HOME */
void handleHome() {
  JSONVar res = getStatus();
  String status = res["status"];
  bool usbMounted = res["usb_mounted"];
  String lastAwakened = formatTimestamp(res["last_awakened"]);
  String lastSuspended = formatTimestamp(res["last_suspended"]);

  String page = homePage;
  String dynamicCSS = "";

  if (status == "awaken") {
    dynamicCSS = "button.wake { pointer-events: none; opacity: 0.5; }";
  } else if (status == "suspended") {
    dynamicCSS = "button.suspend { pointer-events: none; opacity: 0.5; }";
  }

  page.replace("{CSS}", dynamicCSS);
  page.replace("{STATUS}", status);
  page.replace("{USB}", usbMounted ? "Yes" : "No");
  page.replace("{AWAKENED}", lastAwakened);
  page.replace("{SUSPENDED}", lastSuspended);

  server.send(200, "text/html", page);
}
