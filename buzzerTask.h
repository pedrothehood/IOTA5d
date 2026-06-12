#ifndef BUZZER_TASK_H
#define BUZZER_TASK_H
#include <Arduino.h> // Wichtig, um Arduino-Befehle wie digitalWrite zu nutzen
//#include "globals.h"
/**
 * Aktualisiert den Buzzer-Status und die Lautstärke komplett ohne delay().
 */
// für Buzzer:  -> untenstehende Variablen im Betrieb anpassen!
// GLOBALE VARIABLEN: Diese können im echten Betrieb jederzeit verändert werden
//volatile int currentDistance = 800; // Bereich: 0 bis 800 cm
//volatile int currentVolume   = 5;   // Bereich: 0 (lautlos) bis 10 (Maximum)
//const int BUZZER_PIN = 5; 
 // Zustände für die Buzzer-Steuerung (Zustandsautomat)
enum BuzzerState {
  TON_HAUPT,
  TON_SCHLENKER,
  PAUSE
};
void updateBuzzerNonBlocking(volatile int currentDistance, volatile int currentVolume, int BUZZER_PIN ) {
  static BuzzerState currentBuzzerState = PAUSE;
  static unsigned long previousBuzzerMillis = 0;
  static unsigned long stateDuration = 1; // Startet mit 1ms, um die Schleife anzustoßen

  unsigned long currentMillis = millis();
  
  // Prüfen, ob die Zeit für die aktuelle Phase abgelaufen ist
  if (currentMillis - previousBuzzerMillis >= stateDuration) {
    previousBuzzerMillis = currentMillis; // Zeitstempel erneuern
    
    // Lokale Backups holen und Werte auf sichere Grenzen begrenzen
    int distance = constrain(currentDistance, 0, 800);
    int volume = constrain(currentVolume, 0, 10);
    
    // Überproportionale (quadratische) Kurve berechnen
    float normDist = (float)distance / 800.0;
    float curve = normDist * normDist; 

    // Zielparameter für Frequenz und Intervalle berechnen
    int mainFrequency = 2200 - (curve * (2200 - 150));
    int intervalTotal = 40 + (curve * (1500 - 40));
    int totalToneDuration = intervalTotal * 0.35; // Ton ist 35% des Intervalls an

    // Zeiten für die einzelnen Phasen aufteilen
    int mainToneDuration = totalToneDuration * 0.80; // Die ersten 80% konstanter Ton
    int bendDuration = totalToneDuration * 0.20;     // Die letzten 20% Schlenker nach unten
    int pauseDuration = intervalTotal - totalToneDuration;

    // Lautstärke berechnen (PWM Duty Cycle: 0 bis 128 für 8-Bit Auflösung)
    // Überproportionaler Verlauf für das menschliche Gehör
    int dutyCycle = 0;
    if (volume > 0) {
      float normVol = (float)volume / 10.0;
      dutyCycle = (normVol * normVol) * 128; // 128 entspricht 50% maximaler Energie
    }

    // Zustandswechsel ausführen
    switch (currentBuzzerState) {
      
      case PAUSE:
        // Wechselt von Pause zu Hauptton
        currentBuzzerState = TON_HAUPT;
        stateDuration = mainToneDuration;
        
        if (volume > 0) {
          #if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
            ledcWriteTone(BUZZER_PIN, mainFrequency);
            ledcWriteChannel(BUZZER_PIN, dutyCycle);
          #else
            ledcWriteTone(0, mainFrequency);
            ledcWrite(0, dutyCycle);
          #endif
        }
        break;

      case TON_HAUPT:
        // Wechselt von Hauptton zum Schlenker (Pitch-Bend nach unten)
        currentBuzzerState = TON_SCHLENKER;
        stateDuration = bendDuration;
        
        if (volume > 0) {
          // Frequenz bricht im Schlenker auf 60% der Ursprungsfrequenz ein
          #if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
            ledcWriteTone(BUZZER_PIN, mainFrequency * 0.60);
            ledcWriteChannel(BUZZER_PIN, dutyCycle);
          #else
            ledcWriteTone(0, mainFrequency * 0.60);
            ledcWrite(0, dutyCycle);
          #endif
        }
        break;

      case TON_SCHLENKER:
        // Wechselt von Schlenker zur Pause (Ton ausschalten)
        currentBuzzerState = PAUSE;
        stateDuration = pauseDuration;
        
        // Ton vollständig stumm schalten (Duty Cycle = 0)
        #if ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
          ledcWriteChannel(BUZZER_PIN, 0); 
        #else
          ledcWrite(0, 0);
        #endif
        break;
    }
  }
}
#endif
