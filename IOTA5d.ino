// Achtung: HTTP_POST könnte auch 3 statt 2 sein!!!!! (in configServer.h)

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <RD03D.h>
#include <websiteRadar2.h>
#include <ArduinoOTA.h>
#include "buzzerTask.h"      // Lokale Header-Datei laden
#include "serverInit.h"      // Lokale Header-Datei laden
#include "sensorDataToWs.h"  // Lokale Header-Datei laden
#include "wifiInit.h"        // Lokale Header-Datei laden
#include "configServer.h"    // Lokale Header-Datei laden
#include "globals.h"         // Lokale Header-Datei laden
#include "blinker.h"
LedBlinker wifiBlinker;
#include <Preferences.h>

Preferences prefs;
//const char *ssid = "TP-Link_2.4GHz_0494CA";
//const char *password = "pedrothehood007";
#include <WebServer.h>  // Für Config (Einfachheit)
#define SENSOR_RX 16
#define SENSOR_TX 17
// Pins definieren
// Buzzer - Pin
//#define BUZZER_PIN 18
// Physischer Button (Zieht gegen GND)
#define CONFIG_BUTTON_PIN 7  // D5 oder GPIO7
bool configMode = false;
// Globaler Status für den Buzzer
volatile float targetDistance = 800.0;  // füttern
volatile bool personDetected = false;   // füttern
volatile bool isMoving = false;         // füttern
unsigned long lastMoveTime = 0;
const unsigned long hysteresisDelay = 1000;
bool ap_success;

// RD03D radar(Serial1);
RD03D radar(SENSOR_RX, SENSOR_TX, 256000);  // RX, TX, Baudrate
//AsyncWebServer server(81);           // TEST !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//AsyncWebSocket ws("/ws");
//WebServer configServer(80);
void setup() {
  Serial.begin(115200);
  pinMode(CONFIG_BUTTON_PIN, INPUT_PULLUP);
  delay(1000);
  // 1. Taster-Abfrage beim Start (ca. 2 Sek warten oder sofort prüfen)
  if (digitalRead(CONFIG_BUTTON_PIN) == LOW) {
    configMode = true;
  }
  //buzzerTaskSetup();

  if (configMode) {  // ok
    // FALL 2: Konfig-Modus (AP ohne Passwort)
    Serial.println(">>> MODUS: KONFIGURATION (AP) <<<");
    startConfigPortal(configServer, prefs);
    return;
  } else {
    // STA-Normalfall oder AP mit Radar:  In diesen Fällen Asynchroner Webserver!
    getPreferences(prefs);
    wifiInit(ssid, password, radar, ap_success, sensorid);  // STA mit Radar oder AP mit Radar?

    if (WiFi.status() != WL_CONNECTED && ap_success == false) {
      // keine Verbindung-> Abbruch
      Serial.println(">>> Keine Verbindung geschafft! <<<");
      // Abbruch!
      //     Serial.println("Fehler, keine Verbindung geschafft... Gehe in den Tiefschlaf...");
      //   esp_deep_sleep_start();
      Serial.println("Fehler, keine Verbindung geschafft... Restart");
      delay(1000);
      configServer.stop();
      //Serial.println("Server instance gestoppt und Ressourcen freigegeben.");

      ESP.restart();
    }
  }
  // Blinker setzen
  if (wifiMode=="AP"){
      wifiBlinker = {6, 0, 0, 150, 200,1000};
  }else{
      wifiBlinker = {6, 0, 0, 150, 200,1000};
  }
  pinMode(wifiBlinker.pin, OUTPUT);
  serverInit(server, ws);
  // static TargetData*  ptrTarget ; //= radar.getTarget();    // get pointer to first target ( SINGLE DETECTION )
  // static bool detected = false;
  //sensorDataToWs(ws,radar,personDetected,targetDistance,isMoving);
  // Hostname im Netzwerk festlegen (so erscheint das Board in VS Code)
  ArduinoOTA.setHostname("esp32s3-iota4d");
  ArduinoOTA.begin();
}
void loop() {
  // WICHTIG: Prüft kontinuierlich auf eingehende Updates
  ArduinoOTA.handle();

  if (digitalRead(CONFIG_BUTTON_PIN) == LOW) {
    delay(2000);
    if (digitalRead(CONFIG_BUTTON_PIN) == LOW) {
      configServer.stop();
      ESP.restart();
    }
  }
  if (wifiMode=="AP"){
      updateBlink(wifiBlinker,2);
  }else{
      updateBlink(wifiBlinker,1);
  }
  sensorDataToWs(ws, radar, personDetected, targetDistance, isMoving);
}
