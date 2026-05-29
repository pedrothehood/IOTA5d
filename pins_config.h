// Pins Boardübergreifend definieren
#pragma once

// ==========================================
// VARIANTE 1: YD-ESP32-S3 (Wird als ESP32S3 Dev Module kompiliert)
// ==========================================
#if defined(CONFIG_IDF_TARGET_ESP32S3)
  #define BOARD_TYPE "YD-ESP32-S3"
  
  const int  SENSOR_RX = 16;
  const int  SENSOR_TX = 17;
  const int  CONFIG_BUTTON_PIN = 7;  // D5 oder GPIO7
  const int  LED_BLINK_PIN = 6;

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