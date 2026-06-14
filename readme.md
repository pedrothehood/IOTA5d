// Sensor-Anzeige Radar ESP32-S3 mit Radarsensor RD-03D
// System-Optimierung / weniger Störungen
// Dateien neu geordnet/Buzzer?

Fehler in TinyString.cpp
c:\Users\peter\Documents\Arduino\libraries\TinyConsole\src\TinyString.cpp
Suchen Sie nach Zeile 75. Dort steht
TinyString& TinyString::operator +=(int i)
Ändern Sie das int zu int32_t, sodass die Zeile exakt so aussieht
TinyString& TinyString::operator +=(int32_t i)