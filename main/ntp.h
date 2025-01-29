#ifndef NTP_H
#define NTP_H

#include <WiFiUdp.h>
#include <NTPClient.h>

WiFiUDP udp;
const int utcOffset = 3600 * 2;  // UTC +2  in seconds

unsigned long getTime();

#endif
