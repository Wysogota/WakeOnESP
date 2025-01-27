#ifndef ROUTES_H
#define ROUTES_H

#include <Arduino_JSON.h>

#define SUSPENDED false
#define AWAKENED true

unsigned long last_suspended = 0;
unsigned long last_awakened = 0;

extern WebServer server;

void routes();

#endif
