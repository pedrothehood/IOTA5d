#ifndef BLINKER_H
#define BLINKER_H

#include <Arduino.h>

struct LedBlinker {
  int pin;
  unsigned long previousMillis;
  int step;
  int onTime;
  int offTime;
  int sequencePause;  // Jetzt auch variabel!
};

// Deklaration der Funktion
void updateBlink(LedBlinker &b, int flashCount) {
  unsigned long currentMillis = millis();
  int interval;

  if (b.step < flashCount * 2) {
    interval = (b.step % 2 == 0) ? b.onTime : b.offTime;
  } else {
    interval = b.sequencePause;
  }

  if (currentMillis - b.previousMillis >= interval) {
    b.previousMillis = currentMillis;
    b.step++;

    if (b.step >= (flashCount * 2) + 1) b.step = 0;

    bool ledStatus = (b.step < flashCount * 2 && b.step % 2 != 0);
    digitalWrite(b.pin, ledStatus);
  }
}
#endif
