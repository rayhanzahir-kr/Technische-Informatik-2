#include <Arduino.h>
extern "C" {
  #include "TrafficLightSM.h"
}

// ====== PIN-KONFIGURATION (ANPASSEN WENN NÖTIG) ======
const int PIN_RED    = 2;
const int PIN_YELLOW = 4;
const int PIN_GREEN  = 5;
const int BUTTON_PIN = 0;   // Taster-Pin (mit Pullup)

// ====== Events für Queue ======
enum EventType {
  EVENT_REQUEST_FROM_BUTTON,
  EVENT_REQUEST_FROM_SERIAL
};

struct Event {
  EventType type;
};

QueueHandle_t g_eventQueue;

// ====== State Machine Instance & Request-Flag ======
TrafficLightSM g_sm;
volatile bool g_requestActive = false;

// ====== Prototypen Tasks ======
void ampelTask(void *parameter);
void buttonTask(void *parameter);
void serialTask(void *parameter);

// ====== Actions für die State Machine (C-Linkage) ======
extern "C" {

void initRedLight(void) {
  pinMode(PIN_RED, OUTPUT);
  digitalWrite(PIN_RED, LOW);
}

void initYellowLight(void) {
  pinMode(PIN_YELLOW, OUTPUT);
  digitalWrite(PIN_YELLOW, LOW);
}

void initGreenLight(void) {
  pinMode(PIN_GREEN, OUTPUT);
  digitalWrite(PIN_GREEN, LOW);
}

void turnOnRedLight(void) {
  digitalWrite(PIN_RED, HIGH);
}

void turnOffRedLight(void) {
  digitalWrite(PIN_RED, LOW);
}

void turnOnYellowLight(void) {
  digitalWrite(PIN_YELLOW, HIGH);
}

void turnOffYellowLight(void) {
  digitalWrite(PIN_YELLOW, LOW);
}

void turnOnGreenLight(void) {
  digitalWrite(PIN_GREEN, HIGH);
}

void turnOffGreenLight(void) {
  digitalWrite(PIN_GREEN, LOW);
}

} // extern "C"

// ====== Menü-Ausgabe und Hilfsfunktionen für Serial ======
void printMenu() {
  Serial.println();
  Serial.println("Wählen Sie einen Eintrag per Eingabe der Nummer:");
  Serial.println("1. Auslesen des Leucht-Zustands einer Ampel");
  Serial.println("2. Auslesen des Request-Zustands einer Ampel");
  Serial.println("3. Setzen des Requests bei einer Ampel");
  Serial.print("\nAuswahl: ");
}

void printAmpelState() {
  Serial.print("Aktueller Ampelzustand: ");
  switch (g_sm.state_id) {
    case TrafficLightSM_StateId_TRAFFICLIGHTSTATERED:
      Serial.println("ROT");
      break;
    case TrafficLightSM_StateId_TRAFFICLIGHTSTATEREDYELLOW:
      Serial.println("ROT-GELB");
      break;
    case TrafficLightSM_StateId_TRAFFICLIGHTGREEN:
      Serial.println("GRUEN");
      break;
    case TrafficLightSM_StateId_TRAFFICLIGHTYELLOW:
      Serial.println("GELB");
      break;
    case TrafficLightSM_StateId_INIT:
      Serial.println("INIT");
      break;
    default:
      Serial.println("UNBEKANNT");
      break;
  }
}

void printRequestState() {
  Serial.print("Request-Status: ");
  if (g_requestActive) {
    Serial.println("AKTIV");
  } else {
    Serial.println("NICHT AKTIV");
  }
}
void handleRequestEvent(EventType type) {
  g_requestActive = true;
  TrafficLightSM_dispatch_event(&g_sm, TrafficLightSM_EventId_REQUESTGREEN);
  if (type == EVENT_REQUEST_FROM_BUTTON) {
    Serial.println("Request wurde durch Taster gesetzt.");
  } else {
    Serial.println("Request wurde durch Serial-Kommando gesetzt.");
  }
}

// ====== SETUP & LOOP ======
void setup() {
  // Serielle Schnittstelle
  Serial.begin(115200);
  delay(200);
  Serial.println("\nTI2 - Aufgabe 3: Kommandointerpreter");

  // Pins initialisieren
  pinMode(PIN_RED,    OUTPUT);
  pinMode(PIN_YELLOW, OUTPUT);
  pinMode(PIN_GREEN,  OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);



  // Queue für Events
  g_eventQueue = xQueueCreate(10, sizeof(Event));
  if (g_eventQueue == NULL) {
    Serial.println("Fehler: Konnte Queue nicht erzeugen!");
    while (1) { }
  }
    // State Machine initialisieren
    TrafficLightSM_ctor(&g_sm);
    TrafficLightSM_start(&g_sm);
    // Init-State führt initRed/Yellow/Green aus & geht nach Red (per DO-Event)
    TrafficLightSM_dispatch_event(&g_sm, TrafficLightSM_EventId_DO);

    // FreeRTOS-Tasks starten
  xTaskCreate(
      ampelTask,
      "AmpelTask",
      4096,
      NULL,
      2,
      NULL
  );

  xTaskCreate(
      buttonTask,
      "ButtonTask",
      2048,
      NULL,
      1,
      NULL
  );

  xTaskCreate(
      serialTask,
      "SerialTask",
      4096,
      NULL,
      1,
      NULL
  );
}

void loop() {
  // wird nicht benutzt, alles laeuft in Tasks
  vTaskDelay(pdMS_TO_TICKS(1000));
}

  // ====== TASKS ======
void ampelTask(void *parameter) {
  for (;;) {
    // 1) Events aus Queue holen
    Event e;
    if (xQueueReceive(g_eventQueue, &e, 0) == pdPASS) {
      handleRequestEvent(e.type);
    }

    // 2) Tick-Event an State Machine (ca. alle 1ms)
    TrafficLightSM_dispatch_event(&g_sm, TrafficLightSM_EventId_TICK);

    vTaskDelay(pdMS_TO_TICKS(1)); // -> time++ entspricht etwa 1 ms
  }
}

void buttonTask(void *parameter) {
  bool lastState = digitalRead(BUTTON_PIN);

  for (;;) {
    bool currentState = digitalRead(BUTTON_PIN);

    // Flanke HIGH -> LOW (Taster gedrückt, da Pullup)
    if (lastState == HIGH && currentState == LOW) {
      Event e;
      e.type = EVENT_REQUEST_FROM_BUTTON;
      xQueueSend(g_eventQueue, &e, 0);
    }

    lastState = currentState;
    vTaskDelay(pdMS_TO_TICKS(20));  // Entprellung
  }
}

void serialTask(void *parameter) {
  printMenu();

  for (;;) {
    if (Serial.available() > 0) {
      char c = Serial.read();

      // Enter / newline ignorieren
      if (c == '\n' || c == '\r') {
        continue;
      }

      Serial.println(c); // Echo

      switch (c) {
        case '1':
          printAmpelState();
          break;

        case '2':
          printRequestState();
          break;

        case '3': {
          Event e;
          e.type = EVENT_REQUEST_FROM_SERIAL;
          xQueueSend(g_eventQueue, &e, portMAX_DELAY);
          break;
        }

        default:
          Serial.println("Ungültige Eingabe. Bitte 1, 2 oder 3 eingeben.");
          break;
      }

      // Nach jeder Eingabe wieder das Menü zeigen
      printMenu();
    }

    vTaskDelay(pdMS_TO_TICKS(20));
  }
}
