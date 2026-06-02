#ifndef WIFI_INIT_H
#define WIFI_INIT_H
#include <Arduino.h>  // Wichtig, um Arduino-Befehle wie digitalWrite zu nutzen
#include <ESPmDNS.h>
#include "globals.h"

void reconnect() {
  while (!client.connected()) {
    // Client-ID generieren
    String clientId = "ESP32S3-Radar-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      // Verbindung erfolgreich
    } else {
      delay(5000);
    }
  }
}

// init wifi
void wifiInit(String &ssid, String &password, RD03D &radar, bool &ap_success, String sensorid) {
  Serial.print("Wifi verbinden");
  WiFi.begin(ssid, password);
  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 10) {
    delay(500);
    retry++;
  }
  if (WiFi.status() != WL_CONNECTED) {

    // AP versuchen:
    WiFi.mode(WIFI_AP);
    ap_success = WiFi.softAP("WT32_SETUP");

    //retry = 0;
    /*while (WiFi.status() != WL_CONNECTED && retry < 10) {
      delay(500);
      retry++;
    }   */

    if (ap_success == false) return;
    Serial.println(">>> MODUS: AP RADAR <<<");
    wifiMode = "AP";

  } else {  // Connected to STA
    Serial.println(">>> MODUS: STA RADAR <<<");
    wifiMode = "STA";
    // mDNS starten (Hostname: mein-esp32.local)
    if (sensorid > "") {
      if (!MDNS.begin(sensorid)) {
        Serial.println("Fehler beim Starten von mDNS");
      } else {
        Serial.println("mDNS gestartet: http://" + sensorid + ".local");
      }
    }
  }


  /*
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }  */


  Serial.println("\nIP: " + WiFi.localIP().toString());
  Serial.print("Wifi verbunden, initialisiere Sensor...");

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