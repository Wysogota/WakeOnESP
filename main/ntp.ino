#include "ntp.h"

NTPClient timeClient(udp, "pool.ntp.org", utcOffset, 60000);

// Function that gets current epoch time
unsigned long getTime() {
  timeClient.update();
  return timeClient.getEpochTime();
}
