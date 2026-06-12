#ifndef CONFIG_SERVER_H
#define CONFIG_SERVER_H
#include <Arduino.h>  // Wichtig, um Arduino-Befehle wie digitalWrite zu nutzen
#include "globals.h"  // Lokale Header-Datei laden
#include <ESPAsyncWebServer.h>
#include <WebServer.h>
#include <Preferences.h>
#include "blinker.h"
#include "pins_config.h"
#include <esp_mac.h>
LedBlinker configBlinker;
void getPreferences(Preferences &prefs) {

  // Aktuelle Werte aus Preferences laden
  prefs.begin("config", true);
  sensorid = prefs.getString("sid", "");
  apiKey = prefs.getString("key", "");
  ssid = prefs.getString("ssid", "");
  password = prefs.getString("pass", "");
  servername = prefs.getString("srv", "");
   mqttint = prefs.getString("mqttint", "5000").toInt(); 
   mqttintString = String(mqttint);
  // mac = prefs.getString("mac", "");
  prefs.end();
}
String getHardwareMac() {
    uint8_t mac[6];
    // Liest die MAC-Adresse direkt aus dem Hardware-Register (eFuse)
    if (esp_read_mac(mac, ESP_MAC_WIFI_STA) == ESP_OK) {
        char macStr[18];
        snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X", 
                 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        return String(macStr);
    }
    return "00:00:00:00:00:00";
}
void handleRoot(WebServer &server, Preferences &prefs) {
  getPreferences(prefs);
/*
 String html = "<html><head><meta charset='UTF-8'><style>";
  html += "body { font-family: sans-serif; margin: 0; padding: 40px 0; background-color: #FFFFFF; display: flex; align-items: center; min-height: 100vh; box-sizing: border-box; }";
  html += ".container { max-width: 500px; padding: 30px; background-color: #F5F5F5; box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1); border-radius: 12px; width: 100%; margin: auto; box-sizing: border-box; }";
  html += "h1 { text-align: center; color: #333; font-size: 32px; margin-bottom: 20px; }";
  html += "label { display: block; width: 100%; margin-bottom: 6px; color: #333; font-weight: bold; font-size: 16px; text-align: left; }";
  
  html += "input { display: block; width: 100%; padding: 12px; margin-bottom: 18px; border: 2px solid #B0C4DE; background-color: #EBF3F9; border-radius: 8px; box-sizing: border-box; font-size: 16px; outline: none; transition: all 0.2s; }";
  html += "input:focus { border-color: #007BFF; background-color: #F4F8FA; }";
  html += "input[readonly] { background-color: #EAEAEA; border-color: #CCCCCC; cursor: not-allowed; }";
  
  // HIER SIND DIE ANPASSUNGEN FÜR DEN BUTTON-HOVER-EFFEKT:
  html += "input[type='submit'] { background-color: #007BFF; color: white; padding: 15px; font-size: 18px; border: none; border-radius: 8px; cursor: pointer; width: 100%; margin-top: 10px; transition: background-color 0.2s ease-in-out; }";
  html += "input[type='submit']:hover { background-color: #0056b3; }"; // Dunkleres Blau beim Drüberfahren
  
  html += "</style></head><body>";
  html += "<div class='container'>";
  html += "<h1>Geräte-Konfiguration</h1>";
  html += "<form action='/save' method='POST'>";
  html += "<label>Sensor-ID:</label> <input type='text' name='sensorid' value='" + sensorid + "'>";
  html += "<label>API-Key:</label> <input type='password' name='apiKey' value='" + apiKey + "'>";
  html += "<label>SSID:</label> <input type='text' name='ssid' value='" + ssid + "'>";
  html += "<label>Passwort:</label> <input type='password' name='password' value='" + password + "'>";
  html += "<label>Servername:</label> <input type='text' name='servername' value='" + servername + "'>";
  html += "<label>Macadresse:</label> <input type='text' name='mac' value='%%MAC%%' readonly>";
  html += "<input type='submit' value='Speichern und Neustarten'>";
  html += "</form></div></body></html>";  */

  String html = "<html><head><meta charset='UTF-8'>";
  // KORREKTUR 1: Dieser Tag verhindert das winzige Darstellen und das automatische Rauszoomen auf Handys!
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<style>";
  
  // KORREKTUR 2: Nutzung von rem-Einheiten für Schriften und Abstände (skaliert perfekt auf allen Geräten)
  html += "body { font-family: sans-serif; margin: 0; padding: 2rem 1rem; background-color: #FFFFFF; display: flex; align-items: center; min-height: 100vh; box-sizing: border-box; }";
  html += ".container { max-width: 550px; padding: 2.5rem; background-color: #F5F5F5; box-shadow: 0 4px 10px rgba(0, 0, 0, 0.08); border-radius: 16px; width: 100%; margin: auto; box-sizing: border-box; }";
  
  html += "h1 { text-align: center; color: #333; font-size: 2.2rem; margin-bottom: 2rem; line-height: 1.2; }";
  html += "label { display: block; width: 100%; margin-bottom: 0.5rem; color: #333; font-weight: bold; font-size: 1.2rem; text-align: left; }";
  
  html += "input { display: block; width: 100%; padding: 1rem; margin-bottom: 1.5rem; border: 2px solid #B0C4DE; background-color: #EBF3F9; border-radius: 10px; box-sizing: border-box; font-size: 1.2rem; outline: none; transition: all 0.2s; }";
  html += "input:focus { border-color: #007BFF; background-color: #F4F8FA; }";
  html += "input[readonly] { background-color: #EAEAEA; border-color: #CCCCCC; cursor: not-allowed; }";
  
  html += "input[type='submit'] { background-color: #007BFF; color: white; padding: 1.2rem; font-size: 1.3rem; font-weight: bold; border: none; border-radius: 10px; cursor: pointer; width: 100%; margin-top: 1rem; transition: background-color 0.2s ease-in-out; }";
  html += "input[type='submit']:hover { background-color: #0056b3; }";
  
  html += "</style></head><body>";
  html += "<div class='container'>";
  html += "<h1>Geräte-Konfiguration</h1>";
  html += "<form action='/save' method='POST'>";
  html += "<label>Sensor-ID:</label> <input type='text' name='sensorid' value='" + sensorid + "'>";
  html += "<label>API-Key:</label> <input type='password' name='apiKey' value='" + apiKey + "'>";
  html += "<label>SSID:</label> <input type='text' name='ssid' value='" + ssid + "'>";
  html += "<label>Passwort:</label> <input type='password' name='password' value='" + password + "'>";
  html += "<label>Servername:</label> <input type='text' name='servername' value='" + servername + "'>";
  html += "<label>MQTT Intervall (ms):</label> <input type=\"number\" name=\"mqttint\" value=\"" + mqttintString + "\">";
  html += "<label>Macadresse:</label> <input type='text' name='mac' value='%%MAC%%' readonly>";
  html += "<input type='submit' value='Speichern und Neustarten'>";
  html += "</form></div></body></html>";


 // html.replace("%%MAC%%", WiFi.macAddress());
  // Nutzt jetzt die zuverlässige Hardware-Auslesung
html.replace("%%MAC%%", getHardwareMac());
  server.send(200, "text/html", html);
}

void handleSave(WebServer &server, Preferences &prefs) {
  Serial.print("handleSave, server.method = ");
  Serial.println(String(server.method()));
  if (server.method() == 3)  // für HTTP Post
  {
    prefs.begin("config", false);
    Serial.println("Funktion handleSave: am Beginn:");
    Serial.print("SSID= ");
    Serial.println(server.arg("sensorid"));
    Serial.print("PASSWORD= ");
    Serial.println(server.arg("password"));

    // Werte aus dem Formular in den Speicher schreiben
    prefs.putString("sid", server.arg("sensorid"));
    prefs.putString("key", server.arg("apiKey"));
    prefs.putString("ssid", server.arg("ssid"));
    prefs.putString("pass", server.arg("password"));
    prefs.putString("srv", server.arg("servername"));
    //prefs.putString("srv", server.arg("servername"));
    prefs.putString("mqttint", server.arg("mqttint"));
    prefs.end();

    String msg = "<html><body><h1>Erfolgreich gespeichert!</h1>";
    msg += "<p>Das Modul startet jetzt neu...</p></body></html>";
    server.send(200, "text/html", msg);

    delay(2000);
    ESP.restart();  // Neustart, um die neuen Einstellungen zu übernehmen
  }
}

void startConfigPortal(WebServer &server, Preferences &prefs) {
  Serial.println("Starte Config Portal (AP: ESP32-C3_SETUP)...");
  WiFi.mode(WIFI_AP);
  WiFi.softAP("WT32_SETUP");
  configBlinker = { LED_BLINK_PIN, 0, 0, 150, 200, 1000 };
  pinMode(configBlinker.pin, OUTPUT);
  /* server.on("/", 1, handleRoot);
  server.on("/save", 3, handleSave);   */
  // Lambda-Funktionen verwenden, um Parameter zu übergeben
  server.on("/", (HTTPMethod)1, [&server, &prefs]() {
    handleRoot(server, prefs);
  });

  server.on("/save", (HTTPMethod)3, [&server, &prefs]() {
    handleSave(server, prefs);
  });
  server.begin();
  Serial.println("HTTP Server läuft.");
  while (true) {
    server.handleClient();
    updateBlink(configBlinker, 3);
    delay(10);
  }
}




#endif