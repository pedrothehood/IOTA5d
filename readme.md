// Sensor-Anzeige Radar ESP32-S3 mit Radarsensor RD-03D
// System-Optimierung / weniger Störungen
// Dateien neu geordnet/Buzzer?

Fehler in TinyString.cpp
c:\Users\peter\Documents\Arduino\libraries\TinyConsole\src\TinyString.cpp
Suchen Sie nach Zeile 75. Dort steht
TinyString& TinyString::operator +=(int i)
Ändern Sie das int zu int32_t, sodass die Zeile exakt so aussieht
TinyString& TinyString::operator +=(int32_t i)

// Einstellungen für ESP32S3:
Board: XIAO_ESP32S3
Board: "ESP32S3 Dev Module" (oder Ihr exaktes Modell wie XIAO ESP32S3 / Arduino Nano ESP32).
USB CDC On Boot: Enabled. (Essentiell, damit der serielle Monitor über den internen USB-Port Text ausgibt).
Flash Size: Passend zu Ihrer Hardware, meist 8MB oder 16MB (Kennzeichnung N8 oder N16 auf dem Chip).
Partition Scheme: Entsprechend der Flash-Grösse, z. B. 16MB Flash (3MB APP/9.9MB FATFS) oder 8MB mit SPIFFS.
PSRAM: Passend zu Ihrer RAM-Erweiterung (Kennzeichnung R2 / R8). Oft OPI PSRAM bei N16R8-Boards oder Disabled, falls kein PSRAM verbaut ist.
Flash Mode: QIO 80MHz

Board ESP32S3 Supermini
Board ESP32S3 Dev Module Grundarchitektur
USB CDC On Boot Enabled Serieller Monitor über den USB-C Port
Flash Size 4MB (32Mb) Speichergröße des Moduls
Partition Scheme Default 4MB mit spiffs oder Minimal SPIFFS Aufteilung des Flashs
PSRAM Disabled Die SuperMini-Boards haben meist kein PSRAM
Upload ModeUART0 / Hardware CDC Standard-Übertragungsmethode
USB Mode Hardware CDC and JTAG USB-Schnittstelle

