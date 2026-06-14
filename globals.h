#ifndef GLOBALS_H
#define GLOBALS_H
#include <Arduino.h> // Wichtig, um Arduino-Befehle wie digitalWrite zu nutzen
#include <ESPAsyncWebServer.h>
#include <WebServer.h>
#include <PubSubClient.h>
  #if (ENABLE_MQTT_BROKER == 1)
#include <TinyMqtt.h>
MqttBroker broker(10); 
  #endif
AsyncWebServer server(80);           
AsyncWebSocket ws("/ws");
WebServer configServer(80);

// Variablen zum Zwischenspeichern der Werte
String sensorid, apiKey, ssid, password, servername, mac;
unsigned long mqttint ;  // mqtt Intervall-Zeit
String mqttintString;
bool mqttActive = true; 
String mqttQueue = "";
String mqttQueueSensor = "";

String wifiMode = "";  // AP oder STA
const char* mqtt_server = "10.0.0.37";
WiFiClient espClient;
PubSubClient client(espClient);
#endif