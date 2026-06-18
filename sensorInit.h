#ifndef SENSOR_INIT_H
#define SENSOR_INIT_H
#include <Arduino.h>
#include "telnet.h"
// Initialisieren Sensor
void radarInit(RD03D &radar) {
  printP("initialisiere Sensor...");
  if (radar.initialize(RD03D::RD03DMode::MULTI_TARGET)) {
    printlnP("Sensor erfolgreich initialisiert!");
  } else {
    printlnP("Initialisierung fehlgeschlagen! Check RX/TX & Power.");
    while (1) {
      printP(".");
      delay(1000);
    }
  }
}
#endif