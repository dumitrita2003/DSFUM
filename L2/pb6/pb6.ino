const int nonPwmPin = 8; // Pin non-PWM (standard digital)

// Parametri pentru emularea Software PWM (frecventa rapida)
const unsigned long pwmPeriod = 2000; // 2000 us = 2 ms (~500 Hz)
unsigned long cycleStartMicros = 0;

// Parametri pentru efectul de Fade
int brightness = 0;           // Nivelul curent de luminozitate (0 - 255)
int fadeAmount = 5;           // Cu cat creste/scade intensitatea
const unsigned long fadeInterval = 20; // Timpul intre pasi de fade (in ms)
unsigned long previousFadeMillis = 0;

void setup() {
  pinMode(nonPwmPin, OUTPUT);
}

void loop() {
  unsigned long currentMicros = micros();
  unsigned long currentMillis = millis();

  // 1. Modificam luminozitatea treptat (Fade in / Fade out)
  if (currentMillis - previousFadeMillis >= fadeInterval) {
    previousFadeMillis = currentMillis;

    brightness += fadeAmount;

    // Inversam sensul cand ajungem la capete
    if (brightness <= 0 || brightness >= 255) {
      fadeAmount = -fadeAmount;
    }
  }

  // 2. Generam Software PWM pe pinul non-PWM
  unsigned long highTime = ((unsigned long)brightness * pwmPeriod) / 255;
  unsigned long elapsedInCycle = currentMicros - cycleStartMicros;

  if (elapsedInCycle >= pwmPeriod) {
    cycleStartMicros = currentMicros;
    elapsedInCycle = 0;
  }

  if (elapsedInCycle < highTime) {
    digitalWrite(nonPwmPin, HIGH);
  } else {
    digitalWrite(nonPwmPin, LOW);
  }
}