#ifndef ROUTES_H
#define ROUTES_H

#include <Arduino_JSON.h>
#include <TimeLib.h>

#define SUSPENDED false
#define AWAKENED true

unsigned long last_suspended = 0;
unsigned long last_awakened = 0;

const char homePage[] PROGMEM = R"rawliteral(
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<style>
body {
  font-family: Arial, sans-serif;
  margin: 0;
  padding: 0;
  background-color: #121212;
  color: white;
  text-align: center;
}
h1 {
  color: #ffffff;
  font-size: 24px;
  margin-top: 20px;
}
button {
  font-size: 20px;
  padding: 15px;
  margin: 10px;
  width: 90%;
  max-width: 400px;
  border: none;
  cursor: pointer;
  box-sizing: border-box;
  border-radius: 10px;
}
button.wake {
  background-color: #4CAF50;
  color: white;
}
button.suspend {
  background-color: #f44336;
  color: white;
}
{CSS}  /* Динамический CSS */
.status {
  font-size: 22px;
  margin-top: 20px;
  color: #bbbbbb;
}
</style>
</head>
<body>
<h1>Device Control</h1>
<button class="wake" onclick="sendRequest('/suspend/on')">Wake up</button>
<button class="suspend" onclick="sendRequest('/suspend/off')">Suspend</button>
<script>
function sendRequest(path) {
  fetch(path)
    .then(response => response.json())
    .then(() => location.reload())
    .catch(err => console.error('Error:', err));
}
</script>
<div class="status">
Status: {STATUS}<br>
USB Mounted: {USB}<br>
Last Awakened: {AWAKENED}<br>
Last Suspended: {SUSPENDED}<br>
</div>
</body>
</html>
)rawliteral";

extern WebServer server;

void routes();

#endif
