#ifndef GLOBALS_H
#define GLOBALS_H
#include <Arduino.h> // Wichtig, um Arduino-Befehle wie digitalWrite zu nutzen
#include <ESPAsyncWebServer.h>
#include <WebServer.h>
AsyncWebServer server(80);           // TEST !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
AsyncWebSocket ws("/ws");
WebServer configServer(80);

// Variablen zum Zwischenspeichern der Werte
String sensorid, apiKey, ssid, password, servername, mac;

String wifiMode = "";  // AP oder STA

#endif