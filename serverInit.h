#ifndef SERVER_INIT_H
#define SERVER_INIT_H
#include <Arduino.h> // Wichtig, um Arduino-Befehle wie digitalWrite zu nutzen
// Falls index_html in einer anderen .cpp/.ino Datei definiert ist, 
// müssen wir dem Compiler hier sagen, dass es existiert:
extern const char index_html[] PROGMEM; 
extern  String radarDataOrigin;
// Init Server
void serverInit(AsyncWebServer &server, AsyncWebSocket &ws, String sensorid) {
  Serial.print("Server-Handling/Add Handler/Server.on/server.begin");
  delay(500);  // Dem System Zeit geben, den Stack zu stabilisieren
  //Serial.println("Add handler");
  server.addHandler(&ws);
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  }); 
 
// Liefert den Wert genau einmal nach dem Laden der Seite
  server.on("/api/sensorid", HTTP_GET, [sensorid](AsyncWebServerRequest *request){
    request->send(200, "text/plain", sensorid);
  });
 String macAddress = WiFi.macAddress();
  // Liefert den Wert genau einmal nach dem Laden der Seite
  server.on("/api/macaddress", HTTP_GET, [macAddress](AsyncWebServerRequest *request){
    request->send(200, "text/plain", macAddress);
  });

    server.on("/api/radardataorigin", HTTP_GET, [radarDataOrigin](AsyncWebServerRequest *request){
    request->send(200, "text/plain",radarDataOrigin);
  });

// WiFi.macAddress()

  server.begin();
}
#endif