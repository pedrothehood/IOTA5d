#ifndef WIFI_INIT_H
#define WIFI_INIT_H
#include <Arduino.h>  // Wichtig, um Arduino-Befehle wie digitalWrite zu nutzen

// init wifi
void wifiInit(String &ssid, String &password, RD03D &radar) {
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
    WiFi.softAP("WT32_SETUP");
    retry = 0;
    while (WiFi.status() != WL_CONNECTED && retry < 10) {
      delay(500);
      retry++;
    }

    if (WiFi.status() != WL_CONNECTED) return;
    Serial.println(">>> MODUS: AP RADAR <<<");

  } else {  // Connected to STA
    Serial.println(">>> MODUS: STA RADAR <<<");
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