#include "HardwareSerial.h"
#ifndef SENSOR_DATA_TO_WS_H
#define SENSOR_DATA_TO_WS_H
#include <Arduino.h>  // Wichtig, um Arduino-Befehle wie digitalWrite zu nutzen
#include "mqttService.h"
#include "globals.h"  // Lokale Header-Datei laden
//#include <ESPAsyncWebServer.h>
//#include <RD03D.h>
// Sensor-Daten an Websocket senden


// Sensor-Daten an Websocket senden
void sensorDataToWs(AsyncWebSocket &ws, RD03D &radar, volatile bool &personDetected, volatile float &targetDistance, volatile bool &isMoving, String sensorid) {
  ws.cleanupClients();
  radar.tasks();

  unsigned long now = millis();
  static unsigned long lastWSUpdate = 0;
  static unsigned long lastMqttPublish = 0;  // Unabhängiger MQTT-Timer
  //static bool veryFirstTime = true;
  //static bool veryFirstTime = true;
  // Statische Variable, um den Zustand des vorherigen Durchlaufs zu speichern
  static bool lastPersonDetected = false;
  static bool firstRadarDataDone = false;  // NEU: Flag für den allerersten Durchlauf
  String json = "";                        // Scope nach oben verschoben, damit MQTT darauf zugreifen kann
  bool first = true;
  bool currentTargetFound = false;  // Verhindert das Überschreiben bei mehreren Targets

  // 1. JSON-GENERIERUNG (Aus den Timern herausgezogen, damit immer verfügbar)
  if (RD03D::MAX_TARGETS > 0) {
    json = "{\"targets\":[";
    first = true;
    firstRadarDataDone = true;
    for (int i = 0; i < RD03D::MAX_TARGETS; i++) {
      TargetData *t = radar.getTarget(i);

      if (t && t->isValid()) {
        currentTargetFound = true;

        if (i == 0) {
          personDetected = true;
          float rawDist = t->distance / 10.0;  // in cm
          targetDistance = (targetDistance * 0.8) + (rawDist * 0.2);
          // Nutze die Geschwindigkeit (speed) als Indikator für Bewegung
          isMoving = (abs(t->speed) > 0.1);
        }

        if (!first) json += ",";
        json += "{\"x\":" + String(t->x) + ",\"y\":" + String(t->y) + ",\"s\":" + String(t->speed) + ",\"id\":" + String(i) + "}";
        first = false;
      }
    }
    json += "]}";

    if (!currentTargetFound) {
      personDetected = false;
      isMoving = false;
      json = "{\"targets\":[]}";  // Definiert leeres Array für MQTT/WS bei Inaktivität
    }
  }
  String statePayload = personDetected ? "on" : "off";
  // 2. SOFORTIGES MQTT-SENDEN BEI STATUSÄNDERUNG (Echtzeit)
  if (firstRadarDataDone) {  // zuerst müssen die Radardaten einmal korrekt gesendet worden sein!
    if (personDetected != lastPersonDetected) {
      lastPersonDetected = personDetected;  // Zustand aktualisieren
                                            // String statePayload = personDetected ? "on" : "off";
      mqttQueue = mqttQueueSensor + "/occupancy";
      //String stateTopic = mqttQueue.c_str() + "/occupancy";
      //// client.publish(mqttQueue.c_str(), statePayload.c_str());
      sendMqttMessageByVariant("OCCUPANCY", statePayload.c_str());
      //Serial.print("statePayload = ");
      // Serial.println(statePayload);
      // if (ws.count() > 0 && ws.availableForWriteAll()) {
      //  ws.textAll(statePayload);
      //  Serial.print("statePayload = ");
      //  Serial.println(statePayload);
      //}
    }
  }


  // 2. WEBSOCKET-TIMER (Ihr Original-Timer)
  if (now - lastWSUpdate > 100) {
    lastWSUpdate = now;
    if (ws.count() > 0 && ws.availableForWriteAll()) {
      ws.textAll(statePayload);
    }



    if (ws.count() > 0 && ws.availableForWriteAll()) {
      ws.textAll(json);
    }
  }

  // 3. MQTT-TIMER (Ihr Original-Timer nutzt nun das befüllte JSON)
  if (now - lastMqttPublish > 5000) {  // Intervall nach Bedarf anpassen
    lastMqttPublish = now;

    // Hier Ihren MQTT-Sende-Befehl einfügen, z.B.:
    // mqttClient.publish(("esp32/" + sensorid).c_str(), json.c_str());

    //  Serial.println("first = " + String(first) + " sensorid.length = " + String(sensorid.length()) + " json = " + json);
    // Sendet nur, wenn die JSON-Daten gefüllt sind (also Ziele erkannt wurden)

    //if (!first && json.length() > 0) {
    if (wifiMode == "STA" && sensorid.length() > 0 && mqttActive == true) {
      mqttQueue = mqttQueueSensor + "/attributes";
      ////client.publish(mqttQueue.c_str(), json.c_str());
      sendMqttMessageByVariant("ATTRIBUTES", json.c_str());
    }
    //}
  }
}
#endif