#include "ntp.h"

WiFiUDP udp;
NTPClient timeClient(udp, "pool.ntp.org", 0, 60000);

// Function that gets current epoch time
unsigned long getTime() {
  timeClient.update();
  return timeClient.getEpochTime();
}
