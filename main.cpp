#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED Display Definition (I2C)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// LED Pins für Ampel 1
#define RED_1_PIN     19
#define YELLOW_1_PIN  18
#define GREEN_1_PIN    5

// LED Pins für Ampel 2
#define RED_2_PIN     16  // Statt RX2
#define YELLOW_2_PIN   4
#define GREEN_2_PIN    2

// Zusätzliche LED Pins (KURZ WEGEN FEHLER)
// #define BLUE_1_PIN     17  // Kommentiert falls Probleme
// #define BLUE_2_PIN     15
#define GREEN_3_PIN    32
#define GREEN_4_PIN    33
#define GREEN_5_PIN    25
#define GREEN_6_PIN    26

// Analoge Eingänge
#define PHOTORES_PIN    13   // Fotowiderstand (LDR) - GPIO13
#define POTENTIOM_PIN   14   // Potentiometer - GPIO14

// I2C Pins
#define I2C_SDA_PIN     21
#define I2C_SCL_PIN     22

// Ampelzustände
enum TrafficLightState {
  STATE_RED,
  STATE_RED_YELLOW,
  STATE_GREEN,
  STATE_YELLOW,
  STATE_NIGHT_MODE
};

// Globale Variablen
TrafficLightState currentState = STATE_RED;
unsigned long lastStateChange = 0;
unsigned long greenPhaseDuration = 5000;  // Standardwert 5 Sekunden
bool nightMode = false;
const int NIGHT_THRESHOLD = 1500;         // ADC-Wert für Nachtmodus

// LED-Arrays
const int ampel1Pins[] = {RED_1_PIN, YELLOW_1_PIN, GREEN_1_PIN};
const int ampel2Pins[] = {RED_2_PIN, YELLOW_2_PIN, GREEN_2_PIN};

// Funktionen
void setupPins();
void updateDisplay();
void readAnalogSensors();
void handleTrafficLight();
void setAmpel(int ampel, int red, int yellow, int green);
void allLightsOff();
void testLEDs();

void setup() {
  Serial.begin(115200);
  
  // OLED versuchen zu initialisieren (kann fehlschlagen)
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  delay(100);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 nicht gefunden - fahre ohne Display fort");
    // Kein display.display() aufrufen!
  } else {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.println("Ampelsteuerung");
    display.display();
    delay(1000);
  }
  
  setupPins();
  testLEDs();  // Teste alle LEDs zu Beginn
  
  Serial.println("System gestartet");
  Serial.println("=================");
}

void loop() {
  readAnalogSensors();
  handleTrafficLight();
  
  // Display nur updaten wenn es initialisiert wurde
  if (display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    updateDisplay();
  }
  
  delay(100);
}

void setupPins() {
  Serial.println("Initialisiere Pins...");
  
  // Ampel 1
  pinMode(RED_1_PIN, OUTPUT);
  pinMode(YELLOW_1_PIN, OUTPUT);
  pinMode(GREEN_1_PIN, OUTPUT);
  Serial.println("Ampel 1: GPIO 19,18,5");
  
  // Ampel 2
  pinMode(RED_2_PIN, OUTPUT);
  pinMode(YELLOW_2_PIN, OUTPUT);
  pinMode(GREEN_2_PIN, OUTPUT);
  Serial.println("Ampel 2: GPIO 16,4,2");
  
  // 4 Grüne LEDs
  pinMode(GREEN_3_PIN, OUTPUT);
  pinMode(GREEN_4_PIN, OUTPUT);
  pinMode(GREEN_5_PIN, OUTPUT);
  pinMode(GREEN_6_PIN, OUTPUT);
  Serial.println("Grüne LEDs: GPIO 32,33,25,26");
  
  // Alle erstmal AUS
  digitalWrite(RED_1_PIN, LOW);
  digitalWrite(YELLOW_1_PIN, LOW);
  digitalWrite(GREEN_1_PIN, LOW);
  digitalWrite(RED_2_PIN, LOW);
  digitalWrite(YELLOW_2_PIN, LOW);
  digitalWrite(GREEN_2_PIN, LOW);
  digitalWrite(GREEN_3_PIN, LOW);
  digitalWrite(GREEN_4_PIN, LOW);
  digitalWrite(GREEN_5_PIN, LOW);
  digitalWrite(GREEN_6_PIN, LOW);
}

void testLEDs() {
  Serial.println("\n=== TESTE LEDs ===");
  
  // Teste Ampel 1
  Serial.println("Ampel 1 Rot...");
  digitalWrite(RED_1_PIN, HIGH); delay(500); digitalWrite(RED_1_PIN, LOW);
  
  Serial.println("Ampel 1 Gelb...");
  digitalWrite(YELLOW_1_PIN, HIGH); delay(500); digitalWrite(YELLOW_1_PIN, LOW);
  
  Serial.println("Ampel 1 Grün...");
  digitalWrite(GREEN_1_PIN, HIGH); delay(500); digitalWrite(GREEN_1_PIN, LOW);
  
  // Teste Ampel 2
  Serial.println("Ampel 2 Rot...");
  digitalWrite(RED_2_PIN, HIGH); delay(500); digitalWrite(RED_2_PIN, LOW);
  
  Serial.println("Ampel 2 Gelb...");
  digitalWrite(YELLOW_2_PIN, HIGH); delay(500); digitalWrite(YELLOW_2_PIN, LOW);
  
  Serial.println("Ampel 2 Grün...");
  digitalWrite(GREEN_2_PIN, HIGH); delay(500); digitalWrite(GREEN_2_PIN, LOW);
  
  // Teste 4 grüne LEDs
  Serial.println("Grüne LED 32...");
  digitalWrite(GREEN_3_PIN, HIGH); delay(300); digitalWrite(GREEN_3_PIN, LOW);
  
  Serial.println("Grüne LED 33...");
  digitalWrite(GREEN_4_PIN, HIGH); delay(300); digitalWrite(GREEN_4_PIN, LOW);
  
  Serial.println("Grüne LED 25...");
  digitalWrite(GREEN_5_PIN, HIGH); delay(300); digitalWrite(GREEN_5_PIN, LOW);
  
  Serial.println("Grüne LED 26...");
  digitalWrite(GREEN_6_PIN, HIGH); delay(300); digitalWrite(GREEN_6_PIN, LOW);
  
  // Alle 4 grünen LEDs gleichzeitig
  Serial.println("Alle 4 grünen LEDs...");
  digitalWrite(GREEN_3_PIN, HIGH);
  digitalWrite(GREEN_4_PIN, HIGH);
  digitalWrite(GREEN_5_PIN, HIGH);
  digitalWrite(GREEN_6_PIN, HIGH);
  delay(1000);
  digitalWrite(GREEN_3_PIN, LOW);
  digitalWrite(GREEN_4_PIN, LOW);
  digitalWrite(GREEN_5_PIN, LOW);
  digitalWrite(GREEN_6_PIN, LOW);
  
  Serial.println("=== LED TEST BEENDET ===\n");
}

void readAnalogSensors() {
  // Potentiometer
  int potValue = analogRead(POTENTIOM_PIN);
  
  // Sicherheitsprüfung
  if (potValue < 0) potValue = 0;
  if (potValue > 4095) potValue = 4095;
  
  // Mapping: 0-4095 → 3000-10000ms
  greenPhaseDuration = map(potValue, 0, 4095, 3000, 10000);
  
  // Fotowiderstand
  int lightValue = analogRead(PHOTORES_PIN);
  nightMode = (lightValue < NIGHT_THRESHOLD);
  
  // Debug alle 2 Sekunden
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 2000) {
    Serial.print("Poti: ");
    Serial.print(potValue);
    Serial.print(" -> Grünphase: ");
    Serial.print(greenPhaseDuration);
    Serial.print("ms | LDR: ");
    Serial.print(lightValue);
    Serial.print(" | Nachtmodus: ");
    Serial.println(nightMode ? "JA" : "NEIN");
    lastPrint = millis();
  }
}

void handleTrafficLight() {
  if (nightMode) {
    if (currentState != STATE_NIGHT_MODE) {
      Serial.println("Nachtmodus aktiviert!");
      currentState = STATE_NIGHT_MODE;
      allLightsOff();
    }
  } else if (currentState == STATE_NIGHT_MODE) {
    Serial.println("Nachtmodus deaktiviert!");
    currentState = STATE_RED;
    lastStateChange = millis();
  }
  
  unsigned long currentTime = millis();
  
  switch(currentState) {
    case STATE_NIGHT_MODE:
      // Blinkendes Gelb
      if ((currentTime / 500) % 2 == 0) {
        digitalWrite(YELLOW_1_PIN, HIGH);
        digitalWrite(YELLOW_2_PIN, HIGH);
      } else {
        digitalWrite(YELLOW_1_PIN, LOW);
        digitalWrite(YELLOW_2_PIN, LOW);
      }
      break;
      
    case STATE_RED:
      setAmpel(1, 1, 0, 0);  // Ampel 1: Rot
      setAmpel(2, 0, 0, 1);  // Ampel 2: Grün
      
      // Zusätzliche LEDs: Grüne 3 und 5 an
      digitalWrite(GREEN_3_PIN, HIGH);
      digitalWrite(GREEN_4_PIN, LOW);
      digitalWrite(GREEN_5_PIN, HIGH);
      digitalWrite(GREEN_6_PIN, LOW);
      
      if (currentTime - lastStateChange >= greenPhaseDuration) {
        currentState = STATE_RED_YELLOW;
        lastStateChange = currentTime;
        Serial.println("-> Rot-Gelb");
      }
      break;
      
    case STATE_RED_YELLOW:
      setAmpel(1, 1, 1, 0);  // Ampel 1: Rot+Gelb
      setAmpel(2, 0, 1, 0);  // Ampel 2: Gelb
      
      // Zusätzliche LEDs blinken
      static bool blinkState = false;
      blinkState = !blinkState;
      digitalWrite(GREEN_3_PIN, blinkState);
      digitalWrite(GREEN_4_PIN, blinkState);
      digitalWrite(GREEN_5_PIN, blinkState);
      digitalWrite(GREEN_6_PIN, blinkState);
      
      if (currentTime - lastStateChange >= 2000) {
        currentState = STATE_GREEN;
        lastStateChange = currentTime;
        Serial.println("-> Grün");
      }
      break;
      
    case STATE_GREEN:
      setAmpel(1, 0, 0, 1);  // Ampel 1: Grün
      setAmpel(2, 1, 0, 0);  // Ampel 2: Rot
      
      // Zusätzliche LEDs: Grüne 4 und 6 an
      digitalWrite(GREEN_3_PIN, LOW);
      digitalWrite(GREEN_4_PIN, HIGH);
      digitalWrite(GREEN_5_PIN, LOW);
      digitalWrite(GREEN_6_PIN, HIGH);
      
      if (currentTime - lastStateChange >= greenPhaseDuration) {
        currentState = STATE_YELLOW;
        lastStateChange = currentTime;
        Serial.println("-> Gelb");
      }
      break;
      
    case STATE_YELLOW:
      setAmpel(1, 0, 1, 0);  // Ampel 1: Gelb
      setAmpel(2, 1, 1, 0);  // Ampel 2: Rot+Gelb
      
      // Zusätzliche LEDs abwechselnd
      static unsigned long lastBlink = 0;
      if (millis() - lastBlink > 300) {
        static bool alt = false;
        alt = !alt;
        digitalWrite(GREEN_3_PIN, alt);
        digitalWrite(GREEN_4_PIN, !alt);
        digitalWrite(GREEN_5_PIN, alt);
        digitalWrite(GREEN_6_PIN, !alt);
        lastBlink = millis();
      }
      
      if (currentTime - lastStateChange >= 2000) {
        currentState = STATE_RED;
        lastStateChange = currentTime;
        Serial.println("-> Rot");
      }
      break;
  }
}

void setAmpel(int ampel, int red, int yellow, int green) {
  if (ampel == 1) {
    digitalWrite(RED_1_PIN, red);
    digitalWrite(YELLOW_1_PIN, yellow);
    digitalWrite(GREEN_1_PIN, green);
  } else {
    digitalWrite(RED_2_PIN, red);
    digitalWrite(YELLOW_2_PIN, yellow);
    digitalWrite(GREEN_2_PIN, green);
  }
}

void allLightsOff() {
  digitalWrite(RED_1_PIN, LOW);
  digitalWrite(YELLOW_1_PIN, LOW);
  digitalWrite(GREEN_1_PIN, LOW);
  digitalWrite(RED_2_PIN, LOW);
  digitalWrite(YELLOW_2_PIN, LOW);
  digitalWrite(GREEN_2_PIN, LOW);
  digitalWrite(GREEN_3_PIN, LOW);
  digitalWrite(GREEN_4_PIN, LOW);
  digitalWrite(GREEN_5_PIN, LOW);
  digitalWrite(GREEN_6_PIN, LOW);
}

void updateDisplay() {
  display.clearDisplay();
  display.setCursor(0, 0);
  
  display.print("Zustand: ");
  switch(currentState) {
    case STATE_RED: display.println("Rot"); break;
    case STATE_RED_YELLOW: display.println("Rot-Gelb"); break;
    case STATE_GREEN: display.println("Gruen"); break;
    case STATE_YELLOW: display.println("Gelb"); break;
    case STATE_NIGHT_MODE: display.println("Nacht"); break;
  }
  
  display.print("Gruenphase: ");
  display.print(greenPhaseDuration / 1000.0, 1);
  display.println(" s");
  
  display.print("LDR: ");
  display.print(analogRead(PHOTORES_PIN));
  
  display.setCursor(0, 40);
  display.print("Poti: ");
  display.println(analogRead(POTENTIOM_PIN));
  
  display.display();
}