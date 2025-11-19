Übungsaufgabe 3 – Serielle Kommunikation & Kommandointerpreter
Technische Informatik 2 – Praxisübungen
📌 Ziel der Aufgabe
In dieser Übung wird ein einfacher Kommandointerpreter entwickelt, der über die UART-Schnittstelle Eingaben empfängt, interpretiert und entsprechende Aktionen auf dem ESP32 ausführt.
Zusätzlich sollen mehrere FreeRTOS-Tasks und Message Queues genutzt werden, um eine Ampel-State-Machine sowie einen Taster auszuwerten.
🧰 Verwendete Technologien
NodeMCU ESP32
PlatformIO (VS Code Extension)
Arduino Framework
FreeRTOS
UART / Serial Communication
📚 Voraussetzungen
Lesen und verstehen Sie die folgenden Grundlagen:
Arduino Serial Reference
👉 https://www.arduino.cc/reference/en/
ESP32 NodeMCU Datasheet
👉 https://cdn-reichelt.de/documents/datenblatt/A300/SBC-NODEMCU-ESP32-DATASHEET_V1.2.pdf
PlatformIO Grundlagen
👉 https://platformio.org/
Optional: Einführungsvideo in PlatformIO (YouTube)
👉 https://www.youtube.com/watch?v=0lCem8XTKMs
🛠️ Aufgabenbeschreibung
1. Ausgabe auf einem Terminal
Ziel:
Kennenlernen der Serial-Schnittstelle (Serial.print, Serial.println)
Ausgabe von Texten, Variablen und Debug-Informationen
Aufgabe:
Serial initialisieren (Serial.begin(115200))
Erste Textausgaben auf das Terminal schreiben
Funktionsweise testen
2. Einlesen von Informationen (UART Input)
Ziel:
Benutzer*innen sollen über das Terminal Befehle eingeben können
Verarbeitung der Eingaben in FreeRTOS-Tasks
🧵 FreeRTOS-Tasks, die implementiert werden müssen
1. Task: Ampel-State-Machine
verarbeitet die Logik der Ampel
arbeitet parallel und unabhängig
reagiert auf Events aus der Queue
2. Task: Button Handling
überwacht Taster
sendet Events in die Message Queue
3. Task: Serial Kommando-Interpreter
zeigt Menü an
liest Benutzereingaben
führt Befehle aus oder erzeugt Events
📝 Kommandointerpreter – Anforderungen
Beim Start soll das folgende Menü ausgegeben werden:
Wählen Sie einen Eintrag per Eingabe der Nummer:
1. Auslesen des Leucht-Zustands einer Ampel
2. Auslesen des Request-Zustands einer Ampel
3. Setzen des Requests bei einer Ampel

Auswahl: ___
Erwartetes Verhalten
Menü ausgeben
Eingabe per Serial einlesen
Eingabe auswerten
Wenn 1 → Leuchtzustand ausgeben
Wenn 2 → Request-Zustand ausgeben
Wenn 3 → Request-Event über Queue senden
Danach das Menü erneut anzeigen
🔄 Message Queue
Die Kommunikation zwischen:
Ampel-Task
Taster-Task
Kommandointerpreter-Task
erfolgt ausschließlich über Queues (FreeRTOS xQueueSend, xQueueReceive).
✔️ Testen
ESP32 via USB verbinden
Seriellen Monitor öffnen (115200 Baud)
Menü testen
Eingabe 1 → korrekte Daten?
Eingabe 3 → sendet Event?
Funktion der Ampel beobachten
📦 Ordnerstruktur (Vorschlag)
/src
  |- main.cpp
  |- ampel.cpp
  |- ampel.h


/include
/lib
/platformio.ini
/README.md
🧑‍💻 Hinweise
Serial-Eingaben müssen gepuffert werden (z.B. readStringUntil('\n'))
Task-Delays verwenden, um CPU-Last zu vermeiden
Debug-Ausgaben nutzen, um Logik zu prüfen
Queue-Größen und Eventstrukturen sauber definieren
📄 Lizenz
Optional (falls benötigt für Abgabe).
→ MIT / GPL / „Nur für Studienzwecke“ – nach Wunsch.
