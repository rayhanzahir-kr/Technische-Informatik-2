# Technische Informatik 2 – Gesamt­dokumentation  
Dokumentation aller Übungen (Übung 1, Übung 2, Übung 3)  
Autor: Student – Abgabe für Dozent  
Semester: Informatik 2. Semester  

---

# Inhaltsverzeichnis
1. [Einleitung](#einleitung)  
2. [Übung 1 – Grundlagen C](#übung-1--grundlagen-c)  
3. [Übung 2 – LED‑Steuerungen & Taster](#übung-2--ledsteuerungen--taster)  
   - Versuch 1  
   - Versuch 2  
   - Versuch 3.1  
   - Versuch 3.2  
   - Versuch 4 (Ampel mit State Machine)  
4. [Übung 3 – Serielle Kommunikation + Ampel‑State‑Machine](#übung-3--serielle-kommunikation--state-machine)  
5. [Zusammenfassung](#zusammenfassung)

---

# Einleitung
Dieses Dokument enthält eine vollständige Erklärung aller Aufgaben, die im Rahmen der Veranstaltung **Technische Informatik 2** bearbeitet wurden.  
Alle Programme, Dateien und Codes werden erklärt, sodass der Dozent nachvollziehen kann:

- welche technischen Konzepte verwendet wurden  
- wie die Programme aufgebaut sind  
- wie die LED‑ und Ampelsteuerungen funktionieren  
- wie State Machines im Projekt eingesetzt wurden  
- wie serielle Kommunikation und Event‑Handling integriert wurden  

Ziel: **Nachvollziehbare, klare Dokumentation aller Arbeitsschritte.**

---

# Übung 1 – Grundlagen C
Die erste Übung enthält mehrere kleine C‑Programme. Ziel: Grundlagen der Sprache C üben.

## Üb1.c – Eingabe & Ausgabe
```c
#include <stdio.h>

int main(){
    int age;
    printf("Wie alt bist du?");
    scanf("%d",&age);
    printf("Sie sind %d Jahre alt",age);
}
```
**Erklärung:**  
- `scanf` liest eine Zahl  
- Ausgabe über `printf`  
- Übung zu Variablen & I/O

---

## Üb2.c – Schleifen
```c
for(int i=0;i<5;i++){
    printf("Zahl %d : %d
",i+1, i);
}
```
**Erklärung:**  
Einfaches Beispiel für for‑Schleifen.

---

## Üb3.c – If/Else + Modulo
```c
if(num%2==0)
    printf("Angkanya Genap");
else
    printf("Angkanya Ganjil");
```
**Erklärung:**  
Modulo zur Bestimmung gerade/ungerade.

---

## Üb4.c – Funktionen
```c
int sum (int a,int b){
    return a+b;
}
```
**Erklärung:**  
Einführung in Funktionsdefinitionen.

---

## Üb5.c – Pointer Grundlagen
```c
int a = 10;
int *b = &a;
*b = *b + 5;
```
**Erklärung:**  
- `b` zeigt auf `a`  
- Änderungen an `*b` ändern `a` direkt

---

## Üb6.c – Pointer als Funktionsargument
```c
void ubah(int *a){
    *a = *a + 10;
}
```
**Erklärung:**  
Call‑by‑Reference mit Zeigern.

---

## Üb7.c – Union
```c
union SensorValue {
    int intValue;
    float floatValue;
    char charValue;
};
```
**Erklärung:**  
Mehrere Datentypen teilen denselben Speicherbereich.

---

## AmpelSteuerung.plantuml – State‑Machine Modell
Ein Ampelmodell mit Zuständen:  
- ROT  
- ROT‑GELB  
- GRÜN  
- GELB  

Diagramm wird später in Code umgesetzt.

---

# Übung 2 – LED‑Steuerungen & Taster

## Versuch 1 – LED blinkt im 1‑Sekunden‑Takt
```cpp
digitalWrite(LED_PIN, HIGH);
delay(1000);
digitalWrite(LED_PIN, LOW);
delay(1000);
```

---

## Versuch 2 – 8 LEDs mit Mustern
### LED‑Pins
```cpp
const int LED_PINS[] = {4,5,16,17,19,21,22,23};
```

### Muster 1 – Alle LEDs an/aus
```cpp
setAllLeds(true);
delay(1000);
setAllLeds(false);
```

### Muster 2 – Wischen links->rechts und rechts->links
```cpp
for(int i=0;i<NUM_LEDS;i++){
    digitalWrite(LED_PINS[i], HIGH);
    delay(250);
}
```

### Muster 3 – Lauflicht
```cpp
for(int i=0;i<NUM_LEDS;i++){
    digitalWrite(LED_PINS[i], HIGH);
    delay(500);
}
```

---

## Versuch 3.1 – Momentanmodus (LED folgt Taster direkt)
```cpp
void updateGreenLedMomentan() {
    int buttonState = digitalRead(BUTTON_PIN);
    digitalWrite(GREEN_LED_PIN, (buttonState == HIGH) ? HIGH : LOW);
}
```

**Erklärung:**  
- Taster HIGH = LED an  
- Taster LOW  = LED aus  
- Nicht blockierende Delay‑Funktion mit `millis()` verwendet

---

## Versuch 3.2 – Toggle‑Modus (LED schaltet bei jedem Klick)
```cpp
if (buttonState == LOW && lastButtonState == HIGH) {
    ledState = !ledState;
    digitalWrite(GREEN_LED_PIN, ledState);
}
```

**Erklärung:**  
Flankenerkennung (HIGH→LOW) führt zu Zustandwechsel.

---

## Versuch 4 – Ampel mit State Machine
Dateien:
- `Trafficlight.c`
- `Trafficlight.h`
- `main.cpp`

### State‑Definitionen
```c
typedef enum TrafficLightSM_StateId {
    INIT,
    TRAFFICLIGHTRED,
    TRAFFICLIGHTREDYELLOW,
    TRAFFICLIGHTGREEN,
    TRAFFICLIGHTYELLOW
} TrafficLightSM_StateId;
```

### Events
```c
REQUESTGREEN
TICK
```

### Arbeitsweise  
- `TICK` lässt die Ampel automatisch weiterlaufen  
- `REQUESTGREEN` (Taster) erzwingt Umstieg von Rot auf Rot‑Gelb  

---

# Übung 3 – Serielle Kommunikation + State Machine

## Hauptkomponenten:
- **main.cpp**  
- **FreeRTOS‑Tasks**  
- **Event‑Queue**  
- **TrafficLightSM.c/h** (generierter Code)

---

## SerialTask – Menü für Benutzer
```cpp
Serial.println("1. Ampelzustand auslesen");
Serial.println("2. Request setzen");
```

---

## ButtonTask – Tastereingabe
```cpp
if (button LOW && lastButton HIGH)
    sende EVENT_REQUESTGREEN
```

---

## AmpelTask – State Machine Verarbeitung
```cpp
TrafficLightSM_dispatch_event(&g_sm, TrafficLightSM_EventId_TICK);
```

---

## LED‑Steuerung
```cpp
switch(g_sm.state_id){
    case RED: digitalWrite(PIN_RED, HIGH);
}
```

---

# Zusammenfassung
Dieses Projekt demonstriert mehrere Kernkonzepte der technischen Informatik:

- Grundlagen der C‑Programmierung  
- Arbeiten mit GPIO‑Pins  
- LED‑Mustersteuerung  
- Tasterauswertung (momentan & toggle)  
- Zeitsteuerung ohne blocking `delay()`  
- State‑Machine‑Modellierung  
- Generierter C‑Code aus UML  
- Serielle Menüsteuerung  
- FreeRTOS‑basierte Eventarchitektur  

Die Abgabe zeigt ein vollständiges Verständnis über Hardware‑Ansteuerung, Software‑Architektur und systemnahe Programmierung.

---

_Abgabe vollständig – erstellt als Markdown-Datei für den Dozenten._
