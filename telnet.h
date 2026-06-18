#ifndef TELNET_H
#define TELNET_H

#include "ESPTelnet.h"


ESPTelnet telnet;


// 1. Die korrigierte printfP-Funktion
void printfP(const char *format, ...) {
  static bool isPrinting = false;
  if (isPrinting) return; 
  isPrinting = true;

  char txBuffer[256];
  va_list args;
  va_start(args, format);
  vsnprintf(txBuffer, sizeof(txBuffer), format, args);
  va_end(args);

  // USB-Monitor bedienen
  if (Serial && Serial.availableForWrite() > 0) {
    Serial.print(txBuffer);
  }

  // AN PuTTY SENDEN (Prüfung via IP-Adresse statt isConnected)
  if (telnet.getIP() != "") { 
    telnet.print(txBuffer);
  }

  isPrinting = false;
}

// 2. Die korrigierte printP-Funktion
void printP(String text) {
  static bool isPrinting = false;
  if (isPrinting) return; 
  isPrinting = true;

  if (Serial && Serial.availableForWrite() > 0) {
    Serial.print(text);
  }

  if (telnet.getIP() != "") {
    telnet.print(text);
  }

  isPrinting = false;
}

// 3. Die korrigierte printlnP-Funktion
void printlnP(String text) {
  static bool isPrinting = false;
  if (isPrinting) return; 
  isPrinting = true;

  if (Serial && Serial.availableForWrite() > 0) {
    Serial.println(text);
  }

  if (telnet.getIP() != "") {
    telnet.println(text);
  }

  isPrinting = false;
}

// Wird automatisch aufgerufen, wenn ein Telnet-Client Text an den ESP sendet
void onTelnetInput(String input) {
  //printlnP("Befehl empfangen: " + input); // leere Funktion schützt vor Eingabe
  
  // Hier können Sie Ihre Befehle auswerten (z.B. if (input == "an") ... )
} 
// Callback: Wenn sich ein Client per Telnet verbindet
void onTelnetConnect(String ip) {
  printP("\n[Telnet] Client verbunden von IP: ");
  printlnP(ip);
//  telnet.println("Willkommen beim ESP32 Telnet-Server!");
//  telnet.println("Tippen Sie 'status' fuer Infos.\n");
}

// Callback: Wenn die Verbindung getrennt wird
void onTelnetDisconnect(String ip) {
  printP("\n[Telnet] Client getrennt: ");
  printlnP(ip);
}

#endif