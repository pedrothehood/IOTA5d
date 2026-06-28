#ifndef WIFI_INIT_H
#define WIFI_INIT_H
#include <Arduino.h>  // Wichtig, um Arduino-Befehle wie digitalWrite zu nutzen
#include <ESPmDNS.h>
#include "telnet.h"
  #if (ENABLE_MQTT_BROKER == 1)
//#include <TinyMqtt.h>
//extern MqttBroker broker(10); 
  #endif
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
void wifiInit(String &ssid, String &password, bool &ap_success, String sensorid) {
  printlnP("Wifi verbinden");
  printlnP(" SSID = " + String(ssid) + " password = " + String(password));
  // V3-Fix: Sicherstellen, dass alte Verbindungen sauber getrennt sind
  WiFi.disconnect(true, true);
  delay(100);
  WiFi.mode(WIFI_STA);  // Explizit als Station starten
  WiFi.begin(ssid, password);
  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 10) {
    delay(500);
    printP(".");
    retry++;
  }
  if (WiFi.status() != WL_CONNECTED) {
    printlnP("\nSTA fehlgeschlagen. Schalte um auf AP...");
    // V3-Fix: STA-Modus sauber beenden, bevor AP gestartet wird!
    WiFi.disconnect(true);
    delay(200);
    // AP versuchen:
    WiFi.mode(WIFI_AP);
    ap_success = WiFi.softAP("WT32_SETUP");

    //retry = 0;
    /*while (WiFi.status() != WL_CONNECTED && retry < 10) {
      delay(500);
      retry++;
    }   */

    if (ap_success == false) return;
    printlnP(">>> MODUS: AP RADAR <<<");
    wifiMode = "AP";

  } else {  // Connected to STA
    printlnP(">>> MODUS: STA RADAR <<<");
    wifiMode = "STA";
    // mDNS starten (Hostname: mein-esp32.local)
    if (sensorid.length() > 0) {
      #if (ENABLE_MQTT_BROKER == 0)
       if (!MDNS.begin(sensorid)) {
        MDNS.addService("http", "tcp", 80);
          printlnP("Fehler beim Starten von mDNS");
      } else {
        printlnP("mDNS gestartet: http://" + sensorid + ".local");
      }
      #endif
      #if (ENABLE_MQTT_BROKER == 1)
       if (!MDNS.begin("mqttbroker")) {
        MDNS.addService("http", "tcp", 80);
         printlnP("Fehler beim Starten von mDNS im MQTTBROKER-MODE");
      } else {
        printlnP("mDNS gestartet: http://" + sensorid + ".local");
        // 3. Den Dienst "mqtt" im Netzwerk registrieren, damit Clients ihn finden
         MDNS.addService("mqtt", "tcp", 1883);
        printlnP("MQTT-Dienst via mDNS angekündigt (Port 1883).");
         // 4. Den MQTT-Broker starten
       broker.begin();
      printlnP("MQTT-Broker läuft und wartet auf Clients...");
      }
      #endif
    
    }
  }

// V3-Fix: IP nur ausgeben, wenn wir nicht im fehlerhaften AP-Modus sind
  if (WiFi.status() == WL_CONNECTED) {
    printlnP("IP (STA): " + WiFi.localIP().toString());
  } else if (wifiMode == "AP") {
    printlnP("IP (AP): " + WiFi.softAPIP().toString());
  }
  
  printlnP("Wifi-Initialisierung beendet.");
}

#endif