// Pins Boardübergreifend definieren
#pragma once
#include <Arduino.h>
// ==========================================
// Board-Auswahl -> nur EIN Board-Profil aktivieren!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// ==========================================
// nur ein Profil aktivieren !!!!!
#define YD_ESP32_S3
//#define ESP32_S3_SUPERMINI

// ==========================================
// Auswahl der Funktions-Gruppen: ACHTUNG: PULLUP-Widerstand für Taster weiter hinten einstellen!!!!
// ==========================================
// nur a1) oder a2) sind möglich!
// a1) Wird das Einlesen der Radardaten durch den Sensor RD-03D unterstützt?
#define ENABLE_RD_03D_READ  0 // Modus "Radardaten" erzeugen und MQTT erstellen  -> Radardaten darstellen
// a2) Wird das Einlesen von MQTT-Daten unterstützt ?  
#define ENABLE_MQTT_READ  1 // Modus MQTT-Radardaten einlesen  -> Radardaten darstellen (ohne Sensor)


#if (ENABLE_RD_03D_READ == 1)
    String radarDataOrigin = "SENSOR";
    // Dieser Code wird nur kompiliert, wenn Feature X aktiv ist
    ////void debugFunction();
#endif
#if (ENABLE_MQTT_READ == 1)
     String radarDataOrigin = "MQTT";
    // Dieser Code wird nur kompiliert, wenn Feature X aktiv ist
    ////void debugFunction();
#endif

// ==========================================
// KONFIGURATION DER PINS JE NACH BOARD
// ==========================================
#if defined(YD_ESP32_S3)

#if (ENABLE_RD_03D_READ == 1)
  const int SENSOR_RX = 16;
  const int SENSOR_TX = 17;
#endif

  const int CONFIG_BUTTON_PIN = 7;  // D5 oder GPIO7   auf RD06_MQ_01 kaputt ?
// const int CONFIG_BUTTON_PIN = 21; // nur für RD06_MQ_01 ????
  // Erlaubte Werte: INPUT (für externen Widerstand) oder INPUT_PULLUP (für internen Widerstand)
 // #define CONFIG_BUTTON_MODE INPUT_PULLUP  // OHNE externem 10kOhm Widerstand!
   #define CONFIG_BUTTON_MODE INPUT  // MIT externem 10kOhm Widerstand!
  
  const int LED_BLINK_PIN = 6;

#elif defined(ESP32_S3_SUPERMINI)

  const int SENSOR_RX = 4;
  const int SENSOR_TX = 5;
  const int CONFIG_BUTTON_PIN = 1;  // D5 oder GPIO7
  // Erlaubte Werte: INPUT (für externen Widerstand) oder INPUT_PULLUP (für internen Widerstand)
 #define CONFIG_BUTTON_MODE INPUT_PULLUP  // OHNE externem 10kOhm Widerstand!
 //  #define CONFIG_BUTTON_MODE INPUT  // MIT externem 10kOhm Widerstand!
  const int LED_BLINK_PIN = 2;

#else
  #error "Bitte definiere zuerst ein gültiges Board-Profil!"
#endif


// ==========================================
// VARIANTE 1: YD-ESP32-S3 (Wird als ESP32S3 Dev Module kompiliert)
// ==========================================
#if defined(CONFIG_IDF_TARGET_ESP32S3)
#define BOARD_TYPE "YD-ESP32-S3"

//const int SENSOR_RX = 16;
//const int SENSOR_TX = 17;
//const int CONFIG_BUTTON_PIN = 7;  // D5 oder GPIO7

// DIREKTE MODUS-KONFIGURATION
// Erlaubte Werte: INPUT (für externen Widerstand) oder INPUT_PULLUP (für internen Widerstand)
//#define CONFIG_BUTTON_MODE INPUT

//const int LED_BLINK_PIN = 6;

/* const int PIN_STATUS_LED = 2;       // Blaue LED auf der Rückseite
  const int PIN_USER_BUTTON = 0;      // Boot-Knopf
  const int PIN_I2C_SDA     = 4;      // Beispiel: Ihre I2C-Belegung für S3
  const int PIN_I2C_SCL     = 5;
  const int PIN_ADC_BATTERY = 1;      // Analoger Pin für Batterie-Messung  */

// ==========================================
// VARIANTE 2: ESP32-C3 SuperMini (Sehr kompakter RISC-V)
// ==========================================
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
#define BOARD_TYPE "ESP32-C3 SuperMini"

// ==========================================
// VARIANTE 3: Klassischer ESP32 DevKit v1 (30 oder 38 Pins)
// ==========================================
#elif defined(CONFIG_IDF_TARGET_ESP32) && !defined(CONFIG_IDF_TARGET_ESP32S3) && !defined(CONFIG_IDF_TARGET_ESP32C3)
#define BOARD_TYPE "Classic ESP32 DevKit"

#endif