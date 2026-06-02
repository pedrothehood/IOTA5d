#ifndef GLOBALS_H
#define GLOBALS_H
#include <Arduino.h> // Wichtig, um Arduino-Befehle wie digitalWrite zu nutzen
#include <ESPAsyncWebServer.h>
#include <WebServer.h>
#include <PubSubClient.h>
AsyncWebServer server(80);           // TEST !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
AsyncWebSocket ws("/ws");
WebServer configServer(80);

// Variablen zum Zwischenspeichern der Werte
String sensorid, apiKey, ssid, password, servername, mac;

String wifiMode = "";  // AP oder STA

bool mqttActive = true; 

const char* mqtt_server = "10.0.0.37";
WiFiClient espClient;
PubSubClient client(espClient);

#endif