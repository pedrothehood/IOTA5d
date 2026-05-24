#ifndef CONFIG_SERVER_H
#define CONFIG_SERVER_H
#include <Arduino.h>  // Wichtig, um Arduino-Befehle wie digitalWrite zu nutzen
#include "globals.h"  // Lokale Header-Datei laden
#include <ESPAsyncWebServer.h>
#include <WebServer.h>
#include <Preferences.h>
#include "blinker.h"
LedBlinker configBlinker;
void getPreferences(Preferences &prefs) {

  // Aktuelle Werte aus Preferences laden
  prefs.begin("config", true);
  sensorid = prefs.getString("sid", "");
  apiKey = prefs.getString("key", "");
  ssid = prefs.getString("ssid", "");
  password = prefs.getString("pass", "");
  servername = prefs.getString("srv", "");
  // mac = prefs.getString("mac", "");
  prefs.end();
}

void handleRoot(WebServer &server, Preferences &prefs) {
  getPreferences(prefs);
  /* String html = "<html><head><meta charset='UTF-8'><style>";
  html += "body { font-family: sans-serif; margin: 0; padding: 0; background-color: #FFFFFF; display: flex; justify-content: center; align-items: center; height: 100vh; }";
  html += ".container { max-width: 80%; padding: 20px; background-color: #F5F5F5; box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1); border-radius: 8px; text-align: center; width: 100%; }";
  html += "h1 { text-align: center; color: #333; font-size: 53.76px; margin-bottom: 11.2px; }"; // 30% kleiner als vorher
  html += "label { display: block; width: 100%; margin-bottom: 24px; color: #333; font-weight: bold; font-size: 48px; }"; // Doppelter Schriftgrößenwert
  html += "input { display: block; width: 100%; padding: 40px; margin-bottom: 24px; border: 2px solid #CCCCCC; border-radius: 16px; box-sizing: border-box; font-size: 48px; }"; // Doppelter Schriftgrößenwert
  html += "input[type='submit'] { background-color: #007BFF; color: white; padding: 60px 120px; font-size: 36px; border: none; border-radius: 16px; cursor: pointer; width: 100%; }"; // Doppelter Schriftgrößenwert
  html += "</style></head><body>";
  html += "<div class='container'>";
  html += "<h1>Geräte-Konfiguration</h1>";
  html += "<form action='/save' method='POST'>";

  html += "<label>Sensor-ID:</label> <input type='text' name='sensorid' value='" + sensorid + "'><br>";
  html += "<label>API-Key:</label> <input type='password' name='apiKey' value='" + apiKey + "'><br>";
  html += "<label>SSID:</label> <input type='text' name='ssid' value='" + ssid + "'><br>";
  html += "<label>Passwort:</label> <input type='password' name='password' value='" + password + "'><br>";
  html += "<label>Servername:</label> <input type='text' name='servername' value='" + servername + "'><br>";
  html += "<label>Macadresse:</label> <input type='text' name='mac' value='" + WiFi.macAddress() + "' readonly ><br>";

  html += "<br><input type='submit' value='Speichern und Neustarten'>";
  html += "</form></div></body></html>";  */

  String html = "<html><head><meta charset='UTF-8'><style>";
  html += "body { font-family: sans-serif; margin: 0; padding: 0; background-color: #FFFFFF; display: flex; justify-content: center; align-items: center; height: 100vh; }";
  html += ".container { max-width: 80%; padding: 20px; background-color: #F5F5F5; box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1); border-radius: 8px; text-align: center; width: 100%; }";
  html += "h1 { text-align: center; color: #333; font-size: 53.76px; margin-bottom: 11.2px; }";
  html += "label { display: block; width: 100%; margin-bottom: 24px; color: #333; font-weight: bold; font-size: 48px; }";
  html += "input { display: block; width: 100%; padding: 40px; margin-bottom: 24px; border: 2px solid #CCCCCC; border-radius: 16px; box-sizing: border-box; font-size: 48px; }";
  html += "input[type='submit'] { background-color: #007BFF; color: white; padding: 60px 120px; font-size: 36px; border: none; border-radius: 16px; cursor: pointer; width: 100%; }";
  html += "</style></head><body>";
  html += "<div class='container'>";
  html += "<h1>Geräte-Konfiguration</h1>";
  html += "<form action='/save' method='POST'>";
  html += "<label>Sensor-ID:</label> <input type='text' name='sensorid' value='" + sensorid + "'><br>";
  html += "<label>API-Key:</label> <input type='password' name='apiKey' value='" + apiKey + "'><br>";
  html += "<label>SSID:</label> <input type='text' name='ssid' value='" + ssid + "'><br>";
  html += "<label>Passwort:</label> <input type='password' name='password' value='" + password + "'><br>";
  html += "<label>Servername:</label> <input type='text' name='servername' value='" + servername + "'><br>";
  html += "<label>Macadresse:</label> <input type='text' name='mac' value='" + WiFi.macAddress() + "' readonly><br>";
  html += "<br><input type='submit' value='Speichern und Neustarten'>";
  html += "</form></div></body></html>";

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
 configBlinker = {6, 0, 0, 150, 200,1000};
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
    updateBlink(configBlinker,3);
    delay(10);
  }
}




#endif