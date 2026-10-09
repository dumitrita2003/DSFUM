const int NUM_LEDS = 6;
const int ledPins[NUM_LEDS] = {8, 9, 10, 11, 12, 13};

// Variabile de stare pentru fiecare LED
int brightness[NUM_LEDS] = {0, 50, 100, 150, 200, 255}; // Nivel initial usor decalat
int fadeAmount[NUM_LEDS] = {5, 5, 5, 5, 5, 5};

// Viteza proprie pentru fiecare LED (intervalul in ms intre pasi)
// Valori diferite => viteze diferite de fade
const unsigned long fadeIntervals[NUM_LEDS] = {15, 25, 35, 10, 30, 20};
unsigned long previousFadeMillis[NUM_LEDS] = {0, 0, 0, 0, 0, 0};

// Parametri pentru Software PWM comun
const unsigned long pwmPeriod = 2000; // 2 ms (frecventa de 500 Hz)
unsigned long cycleStartMicros = 0;

void setup() {
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
}

void loop() {
  unsigned long currentMicros = micros();
  unsigned long currentMillis = millis();

  // 1. Actualizam luminozitatea fiecarui LED conform vitezei sale proprii
  for (int i = 0; i < NUM_LEDS; i++) {
    if (currentMillis - previousFadeMillis[i] >= fadeIntervals[i]) {
      previousFadeMillis[i] = currentMillis;

      brightness[i] += fadeAmount[i];

      // Daca a atins limitele (0 sau 255), inversam sensul
      if (brightness[i] <= 0) {
        brightness[i] = 0;
        fadeAmount[i] = -fadeAmount[i];
      } else if (brightness[i] >= 255) {
        brightness[i] = 255;
        fadeAmount[i] = -fadeAmount[i];
      }
    }
  }

  // 2. Restart ciclu Software PWM la fiecare 2 ms
  unsigned long elapsedInCycle = currentMicros - cycleStartMicros;
  if (elapsedInCycle >= pwmPeriod) {
    cycleStartMicros = currentMicros;
    elapsedInCycle = 0;
  }

  // 3. Setam starea fiecarui pin in functie de duty-cycle-ul sau
  for (int i = 0; i < NUM_LEDS; i++) {
    unsigned long highTime = ((unsigned long)brightness[i] * pwmPeriod) / 255;
    
    if (elapsedInCycle < highTime) {
      digitalWrite(ledPins[i], HIGH);
    } else {
      digitalWrite(ledPins[i], LOW);
    }
  }
}