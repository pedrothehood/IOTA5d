#ifndef MQTTSERVICE_H
#define MQTTSERVICE_H
#include <Arduino.h> // Wichtig, um Arduino-Befehle wie digitalWrite zu nutzen
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <vector>
#include "globals.h"

// Datenstruktur für eine MQTT-Verbindung aus der API
struct MqttConnection {
  int connid;
  String sensorid;
  String variant;
  bool active;
  String brokerurl;
  String topic;
  String userid;
  String password;
  int port;
  WiFiClient wifiClient;
  PubSubClient mqttClient;
};

// Globaler Vektor speichert alle vorbereiteten Verbindungen
//std::vector<MqttConnection> mqttConnections;
std::vector<MqttConnection*> mqttConnections;
// REST API Konfiguration
const char* wifiSsid     = "DEIN_WIFI_SSID";
const char* wifiPassword = "DEIN_WIFI_PASSWORT";
//const char* apiKey       = "DEIN_API_KEY";

// Funktions-Prototypen
void initializeMqttConnections(const char* key, const char* sensorId, const char* variant = "");
String getMqttData(const char* key, const char* sensorId, const char* variant = "");
void parseAndPrepareConnections(String jsonString);
void sendMqttMessageByVariant(String variant, String message);

// Holt die JSON-Daten vom PHP REST Service
String getMqttData(const char* key, const char* sensorId, const char* variant) {
  if (WiFi.status() != WL_CONNECTED) return "";

  HTTPClient http;
  // URL mit Pflichtparametern aufbauen
  String url = "https://mediabegleitung.ch/iotph1/getmqttdata.php?api_key=" + String(key) + "&active=X&sensorid=" + String(sensorId);
  Serial.println(url);
  // Fakultativer Parameter
  if (variant != nullptr && String(variant).length() > 0) {
    url += "&variant=" + String(variant);
  }

  http.begin(url);
  int httpCode = http.GET();
  
  String payload = "";
  if (httpCode == HTTP_CODE_OK) {
    payload = http.getString();
  } else {
    Serial.printf("HTTP Fehler: %d\n", httpCode);
  }
  http.end();
  return payload;
}

// Verarbeitet das JSON und baut die persistenten Verbindungen im Vektor auf
void parseAndPrepareConnections(String jsonString) {
  if (jsonString.length() == 0) return;

  JsonDocument doc; 
  DeserializationError error = deserializeJson(doc, jsonString);

  if (error) {
    Serial.printf("JSON Parsing fehlgeschlagen: %s\n", error.c_str());
    return;
  }

  JsonArray array = doc.as<JsonArray>();
  for (JsonVariant v : array) {
    // GEÄNDERT: Objekt dynamisch auf dem Heap via 'new' anlegen
    MqttConnection* conn = new MqttConnection();
    
    conn->connid    = v["connid"];
    conn->sensorid  = v["sensorid"].as<String>();
    conn->variant   = v["variant"].as<String>();
    conn->active    = (v["active"].as<String>() == "X");
    conn->brokerurl = v["brokerurl"].as<String>();
    conn->topic     = v["topic"].as<String>();
    conn->userid    = v["userid"].as<String>();
    conn->password  = v["password"].as<String>();
    conn->port      = v["port"].as<int>();

    if (conn->active) {
      // Verbindung konfigurieren (Zeiger-Syntax '->' statt '.')
      conn->mqttClient.setClient(conn->wifiClient);
      conn->mqttClient.setServer(conn->brokerurl.c_str(), conn->port);
      
      Serial.printf("Initialisiere Verbindung für Variant %s zu Broker %s\n", conn->variant.c_str(), conn->brokerurl.c_str());
      if (conn->mqttClient.connect(conn->variant.c_str(), conn->userid.c_str(), conn->password.c_str())) {
        Serial.printf("Erfolgreich verbunden mit Topic: %s\n", conn->topic.c_str());
      } else {
        Serial.printf("Verbindung fehlgeschlagen. Status: %d\n", conn->mqttClient.state());
      }
      
      // GEÄNDERT: Den Pointer in den Vektor pushen (kein Speicher-Verschieben mehr!)
      mqttConnections.push_back(conn);
    } else {
      // Wenn nicht aktiv, ungenutzten Speicher sofort freigeben
      delete conn;
    }
  }
}

// Kapselung für das Setup (holt Daten und bereitet Verbindungen vor)
void initializeMqttConnections(const char* key, const char* sensorId, const char* variant) {
  Serial.println("Rufe REST Service auf...");
  String jsonResponse = getMqttData(key, sensorId, variant);
  
  if (jsonResponse.length() > 0) {
    parseAndPrepareConnections(jsonResponse);
  } else {
    Serial.println("Keine Daten vom REST Service erhalten.");
  }
}

// Massenversand: Sucht die vorbereitete Verbindung anhand der Variant und sendet sofort
void sendMqttMessageByVariant(String variant, String message) {
  // GEÄNDERT: Schleife liest nun Pointer aus dem Vektor aus
  for (auto conn : mqttConnections) {
    if (conn->variant == variant && conn->active) {
      if (conn->mqttClient.connected()) {
        conn->mqttClient.publish(conn->topic.c_str(), message.c_str());
        //Serial.printf("Nachricht an [%s] gesendet: %s\n", conn->topic.c_str(), message.c_str());
      } else {
        //Serial.printf("Senden fehlgeschlagen. Client für %s ist offline.\n", variant.c_str());
      }
      return; 
    }
  }
 // Serial.printf("Keine aktive Verbindung für Variant %s konfiguriert.\n", variant.c_str());
}
#endif