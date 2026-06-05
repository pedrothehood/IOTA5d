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
  bool retain;       
  String clientid;   
  WiFiClient wifiClient;
  PubSubClient mqttClient;
};

// Globaler Vektor speichert alle vorbereiteten Verbindungen
std::vector<MqttConnection*> mqttConnections;

// REST API Konfiguration
const char* wifiSsid     = "DEIN_WIFI_SSID";
const char* wifiPassword = "DEIN_WIFI_PASSWORT";

// Funktions-Prototypen (Bestehend)
void initializeMqttConnections(const char* key, const char* sensorId, const char* variant = "");
String getMqttData(const char* key, const char* sensorId, const char* variant = "");
void parseAndPrepareConnections(String jsonString);
void sendMqttMessageByVariant(String variant, String message);

// Funktions-Prototypen für den Empfang (Inbound)
void globalMqttCallback(char* topic, byte* payload, unsigned int length);
void subscribeToAllActiveTopics();
void maintainMqttConnections();

// NEU: Definition des Typs für die externe Funktion
// Akzeptiert: variant, topic, message
typedef void (*MqttMessageCallback)(String, String, String);

// NEU: Globaler Zeiger auf die externe Funktion (Standardmässig nullptr)
MqttMessageCallback externalCallback = nullptr;

// NEU: Funktion, um den externen Callback von ausserhalb anzumelden
void registerMqttCallback(MqttMessageCallback customFunction);


// Holt die JSON-Daten vom PHP REST Service
String getMqttData(const char* key, const char* sensorId, const char* variant) {
  if (WiFi.status() != WL_CONNECTED) return "";

  HTTPClient http;
  String url = "https://mediabegleitung.ch/iotph1/getmqttdata.php?api_key=" + String(key) + "&active=X&sensorid=" + String(sensorId);
  Serial.println(url);
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
    
    conn->retain    = (v["retain"].as<String>() == "X");
    conn->clientid  = v["clientid"].as<String>();

    if (conn->clientid.length() == 0) {
      conn->clientid = conn->variant;
    }

    if (conn->active) {
      conn->mqttClient.setClient(conn->wifiClient);
      conn->mqttClient.setServer(conn->brokerurl.c_str(), conn->port);

      Serial.printf("Initialisiere Verbindung für Variant %s zu Broker %s\n", conn->variant.c_str(), conn->brokerurl.c_str());
      
      if (conn->mqttClient.connect(conn->clientid.c_str(), conn->userid.c_str(), conn->password.c_str())) {
        Serial.printf("Erfolgreich verbunden mit Topic: %s (Client-ID: %s)\n", conn->topic.c_str(), conn->clientid.c_str());
      } else {
        Serial.printf("Verbindung fehlgeschlagen. Status: %d\n", conn->mqttClient.state());
      }

      mqttConnections.push_back(conn);
    } else {
      delete conn;
    }
  }
}

// Kapselung für das Setup
void initializeMqttConnections(const char* key, const char* sensorId, const char* variant) {
  Serial.println("Rufe REST Service auf...");
  String jsonResponse = getMqttData(key, sensorId, variant);

  if (jsonResponse.length() > 0) {
    parseAndPrepareConnections(jsonResponse);
  } else {
    Serial.println("Keine Daten vom REST Service erhalten.");
  }
}

// Massenversand
void sendMqttMessageByVariant(String variant, String message) {
  for (auto conn : mqttConnections) {
    if (conn->variant == variant && conn->active) {
      if (conn->mqttClient.connected()) {
        conn->mqttClient.publish(conn->topic.c_str(), message.c_str(), conn->retain);
      }
      return; 
    }
  }
}


// ==========================================
// FUNKTIONEN FÜR DEN NACHRICHTENEMPFANG
// ==========================================

/**
 * Zentrale Callback-Funktion für eingehende MQTT-Nachrichten.
 */
void globalMqttCallback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  String incomingTopic = String(topic);
  String foundVariant = "unknown";

  // Durchsuche die aktiven Verbindungen für das Topic
  for (auto conn : mqttConnections) {
    if (conn->active && conn->topic == incomingTopic) {
      foundVariant = conn->variant;
      break; 
    }
  }

  // GEÄNDERT: Rufe die externe Funktion auf, falls eine registriert wurde
  if (externalCallback != nullptr) {
    externalCallback(foundVariant, incomingTopic, message);
  } else {
    Serial.println("Warnung: Nachricht empfangen, aber kein externer Callback registriert!");
  }
}

/**
 * Aktiviert den Empfang für alle in der Liste konfigurierten Topics.
 */
void subscribeToAllActiveTopics() {
  for (auto conn : mqttConnections) {
    if (conn->active && conn->mqttClient.connected()) {
      conn->mqttClient.setCallback(globalMqttCallback);
      
      if (conn->mqttClient.subscribe(conn->topic.c_str())) {
        Serial.printf("Erfolgreich abonniert: %s\n", conn->topic.c_str());
      } else {
        Serial.printf("Abonnement fehlgeschlagen für: %s\n", conn->topic.c_str());
      }
    }
  }
}

/**
 * Hält die Hintergrund-Kommunikation für den Empfang aller Clients aufrecht.
 */
void maintainMqttConnections() {
  for (auto conn : mqttConnections) {
    if (conn->active && conn->mqttClient.connected()) {
      conn->mqttClient.loop();
    }
  }
}

/**
 * NEU: Erlaubt es dem Hauptprogramm, seine eigene Verarbeitungsfunktion zu übergeben.
 */
void registerMqttCallback(MqttMessageCallback customFunction) {
  externalCallback = customFunction;
}

#endif