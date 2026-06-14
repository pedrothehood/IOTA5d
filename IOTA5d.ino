// Achtung: HTTP_POST könnte auch 3 statt 2 sein!!!!! (in configServer.h)
#include "pins_config.h"
#include <ESPAsyncWebServer.h>
#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>

#if (ENABLE_RD_03D_READ == 1)
#include <RD03D.h>
#include "sensorInit.h"
#endif

#include <websiteRadar.h>
#include <ArduinoOTA.h>
#include "buzzerTask.h"      // Lokale Header-Datei laden
#include "serverInit.h"      // Lokale Header-Datei laden
#if (ENABLE_RD_03D_READ == 1)
#include "sensorDataToWs.h"  // Lokale Header-Datei laden
#endif
//ENABLE_MQTT_READ
#if (ENABLE_MQTT_READ == 1)
#include "mqttDataToWs.h"  // Lokale Header-Datei laden
#endif

#include "wifiInit.h"        // Lokale Header-Datei laden
#include "configServer.h"    // Lokale Header-Datei laden
#include "globals.h"         // Lokale Header-Datei laden
#include "blinker.h"
//#include "pins_config.h"   // nach vorne wegen Auflösungsreihenfolge der Preprozessor-Befehle
#include "mqttService.h"
//#include "sensorInit.h"    // nach vorne
LedBlinker wifiBlinker;
#include <Preferences.h>
#if (ENABLE_MQTT_BROKER == 1)
//#include <TinyMqtt.h>
// Erstellt den Broker auf dem Standard-Port 1883
// Die Zahl '10' legt fest, dass maximal 10 Retained Messages im RAM gespeichert werden
//MqttBroker broker(10); 
#endif
Preferences prefs;
//const char *ssid = "TP-Link_2.4GHz_0494CA";
//const char *password = "pedrothehood007";
#include <WebServer.h>  // Für Config (Einfachheit)
#if (ENABLE_RD_03D_READ == 1)
  // Wir erstellen einen globalen Pointer (Wegweiser), der vorerst auf "nichts" (nullptr) zeigt
  RD03D* radar = nullptr; 
#endif
//#define SENSOR_RX 16
//#define SENSOR_TX 17
// Pins definieren
// Buzzer - Pin
//#define BUZZER_PIN 18
// Physischer Button (Zieht gegen GND)
//#define CONFIG_BUTTON_PIN 7  // D5 oder GPIO7
bool configMode = false;
// Globaler Status für den Buzzer
volatile float targetDistance = 800.0;  // füttern
volatile bool personDetected = false;   // füttern
volatile bool isMoving = false;         // füttern
unsigned long lastMoveTime = 0;
const unsigned long hysteresisDelay = 1000;
bool ap_success;
unsigned long buttonPressedTime = 0;
bool buttonIsPressed = false;

void setup() {
  Serial.begin(115200);
  #if (ENABLE_RD_03D_READ == 1)
    #if defined(XIAO_ESP32_S3)
    // RD03D radar(Serial1);
    Serial1.begin(256000, SERIAL_8N1, SENSOR_RX, SENSOR_TX); 
      delay(500); // Dem UART-Port Zeit zum Stabilisieren geben
    #endif
      //RD03D radar(SENSOR_RX, SENSOR_TX, 256000);  // RX, TX, Baudrate
      // JETZT wird das Radar-Objekt dynamisch im Speicher erzeugt – exakt nach Serial1.begin()!
    radar = new RD03D(SENSOR_RX, SENSOR_TX, 256000);  // RX, TX, Baudrate
  #endif
  ////while (!Serial) { delay(10); }  // Wartet aktiv, bis der PC den USB-Port wieder geöffnet hat!
  // Verhindert das Einfrieren, falls der USB-Puffer voll ist:
  //Serial.setTxTimeoutMs(0);
  // Wartet maximal 3 Sekunden, bis der Serielle Monitor am PC geöffnet wird
  delay(1000);
  //while (!Serial && millis() < 4000) {
    //delay(10);
 // }
  // Warte 2 Sekunden, damit der USB-Port stabil steht, bevor Daten gesendet werden
  Serial.println("--- ESP32-S3 gestartet! ---");
 // delay(2000);
  //pinMode(CONFIG_BUTTON_PIN, INPUT_PULLUP);
  pinMode(CONFIG_BUTTON_PIN, CONFIG_BUTTON_MODE);  // 10kOhm Widerstand zwischen Pin und 3.3Volt
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
    //return;
  } else {
    // STA-Normalfall oder AP mit Radar:  In diesen Fällen Asynchroner Webserver!
    getPreferences(prefs);
   
    wifiInit(ssid, password, ap_success, sensorid);  // STA mit Radar oder AP mit Radar?
    #if (ENABLE_RD_03D_READ == 1)
    radarInit(*radar);
    #endif

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
  if (wifiMode == "AP") {
    wifiBlinker = { LED_BLINK_PIN, 0, 0, 150, 200, 1000 };
  } else {
    wifiBlinker = { LED_BLINK_PIN, 0, 0, 150, 200, 1000 };
    if (mqttActive == true) {
      //mqttQueueSensor = "Sensor-" + sensorid;
      //client.setServer(mqtt_server, 1883);
      // 1. MQTT-Verbindungen über API-Daten vorbereiten und starten
      #if (ENABLE_RD_03D_READ == 1 || ENABLE_MQTT_READ  == 1)
      // Aufruf-Beispiel mit API-Key, SensorID "TEST1" und ohne optionale Variant
      initializeMqttConnections(apiKey.c_str(), sensorid.c_str());
// Ev. werden logische URL's des Brokers zu phyischer URL umgewandelt
      resolveMdnsBrokerUrls();
      #endif
      #if (ENABLE_MQTT_READ == 1)
      registerMqttCallback(mqttDataToWs);
      
      // Empfang starten
      subscribeToAllActiveTopics();
      #endif
    }
  }
  pinMode(wifiBlinker.pin, OUTPUT);
  #if (ENABLE_WS_ASYNC_SERVER_INIT == 1)
  serverInit(server, ws, sensorid);
  #endif
  // static TargetData*  ptrTarget ; //= radar.getTarget();    // get pointer to first target ( SINGLE DETECTION )
  // static bool detected = false;
  //sensorDataToWs(ws,radar,personDetected,targetDistance,isMoving);
  // Hostname im Netzwerk festlegen (so erscheint das Board in VS Code)
  ArduinoOTA.setHostname("esp32s3-iota5d");
  ArduinoOTA.begin();
}
void loop() {
  if (configMode) {
    configServer.handleClient();
    return;
  }
  #if (ENABLE_MQTT_BROKER == 1)
   // Der Broker muss regelmässig im Loop aufgerufen werden,
  // um Datenpakete zu verarbeiten und Verbindungen zu halten.
  broker.loop();
  #endif
  if (mqttActive == true) {
    /*// Hält alle aktiven MQTT-Verbindungen am Leben   ----->>>>>>>>>>>> CHECK für Receive!!!!
    for (auto conn : mqttConnections) {
      if (conn != nullptr && conn->active) {
        conn->mqttClient.loop();
      }
    }  */
    maintainMqttConnections(); // ersetzt obigen Code
  }

  // WICHTIG: Prüft kontinuierlich auf eingehende Updates
  ArduinoOTA.handle();

  if (digitalRead(CONFIG_BUTTON_PIN) == LOW) {
    if (!buttonIsPressed) {
      buttonIsPressed = true;
      buttonPressedTime = millis();  // Startzeit merken
      //  Serial.println("Knopf gedrückt... Halten für Reset!");
    } else if (millis() - buttonPressedTime >= 2000) {
      // Wenn seit dem ersten Drücken 2000ms vergangen sind
      Serial.println("2 Sekunden erreicht! Starte Neu...");
      configServer.stop();
      delay(500);  // Kurz warten für serielle Ausgabe
      ESP.restart();
    }
  } else {
    // Knopf wurde losgelassen, bevor 2 Sekunden um waren
    if (buttonIsPressed) {
      Serial.println("Reset abgebrochen (Knopf zu kurz gedrückt).");
      buttonIsPressed = false;
    }
  }

  if (wifiMode == "AP") {
    updateBlink(wifiBlinker, 2);
  } else {
    updateBlink(wifiBlinker, 1);

  }
  #if (ENABLE_RD_03D_READ == 1)
  sensorDataToWs(ws, *radar, personDetected, targetDistance, isMoving, sensorid);
  #endif
}
