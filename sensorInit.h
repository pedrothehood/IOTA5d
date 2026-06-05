#ifndef SENSOR_INIT_H
#define SENSOR_INIT_H
#include <Arduino.h>

// Initialisieren Sensor
void radarInit(RD03D &radar) {
  Serial.print("initialisiere Sensor...");
  if (radar.initialize(RD03D::RD03DMode::MULTI_TARGET)) {
    Serial.println("Sensor erfolgreich initialisiert!");
  } else {
    Serial.println("Initialisierung fehlgeschlagen! Check RX/TX & Power.");
    while (1) {
      Serial.print(".");
      delay(1000);
    }
  }
}
#endif