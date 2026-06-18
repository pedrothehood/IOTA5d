#ifndef MQTT_DATA_TO_WS_H
#define MQTT_DATA_TO_WS_H
#include "HardwareSerial.h"
#include "globals.h"  // Lokale Header-Datei laden

void mqttDataToWs(String variant, String topic, String message) {
  //printlnP("--- Nachricht in meiner Hauptlogik angekommen! ---");
  
  if (variant == "OCCUPANCY") {
    ws.textAll(message);
  }
   if (variant == "ATTRIBUTES") {
    if (ws.count() > 0 && ws.availableForWriteAll()) {
        ws.textAll(message);
    }
  }
}
#endif