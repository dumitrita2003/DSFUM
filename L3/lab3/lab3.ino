const int NUM_LEDS = 6;
const int pins[NUM_LEDS] = {8, 9, 10, 11, 12, 13};

// Pinii 9, 10, 11 au PWM hardware pe Arduino Uno. Pinii 8, 12, 13 folosesc SoftPWM.
const bool hasHardwarePWM[NUM_LEDS] = {false, true, true, true, false, false};

// Tipurile de animații posibile
enum AnimationType { OFF, BLINK, FADEIN, FADEOUT, FADEINOUT };

// Structură pentru a reține starea fiecărui LED în mod independent
struct LEDState {
  int pin;
  AnimationType anim;
  unsigned long duration;   // Durata unei bucle de animație (în ms)
  unsigned long startTime;  // Momentul (millis) când a început animația
  int pwmValue;             // Nivelul curent de luminozitate (0 - 255)
};

LEDState leds[NUM_LEDS];
String inputBuffer = ""; // Buffer pentru acumularea caracterelor primite pe Serial

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].pin = pins[i];
    leds[i].anim = OFF;
    leds[i].duration = 0;
    leds[i].startTime = 0;
    leds[i].pwmValue = 0;
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], LOW);
  }
}

void loop() {
  readSerialNonBlocking(); // Citire din Serial fără delay()
  updateAnimations();      // Calculare luminozitate pentru fiecare LED
  handleSoftPWM();         // Generare semnal PWM software pentru pinii 8, 12, 13
}

// Citește caracterele pe măsură ce sosesc; procesează comanda când întâlnește Newline (\n)
void readSerialNonBlocking() {
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (inputBuffer.length() > 0) {
        processCommand(inputBuffer);
        inputBuffer = ""; // Resetează buffer-ul pentru următoarea comandă
      }
    } else {
      inputBuffer += c;
    }
  }
}

// Parsează comanda în formatul: nr_led,animatie,timp_in_ms
void processCommand(String input) {
  input.trim();
  int firstComma = input.indexOf(',');
  int secondComma = input.indexOf(',', firstComma + 1);

  if (firstComma != -1 && secondComma != -1) {
    int pinNum = input.substring(0, firstComma).toInt();
    String animStr = input.substring(firstComma + 1, secondComma);
    unsigned long duration = input.substring(secondComma + 1).toInt();

    int ledIdx = getLedIndex(pinNum);
    if (ledIdx != -1) {
      leds[ledIdx].duration = duration;
      leds[ledIdx].startTime = millis(); // Resetează temporizatorul pentru noua animație
      animStr.toLowerCase();

      if (animStr == "off") leds[ledIdx].anim = OFF;
      else if (animStr == "blink") leds[ledIdx].anim = BLINK;
      else if (animStr == "fadein") leds[ledIdx].anim = FADEIN;
      else if (animStr == "fadeout") leds[ledIdx].anim = FADEOUT;
      else if (animStr == "fadeinout") leds[ledIdx].anim = FADEINOUT;
    }
  }
}

// Caută indexul din tablou corespunzător numărului de pin (8-13)
int getLedIndex(int pin) {
  for (int i = 0; i < NUM_LEDS; i++) {
    if (pins[i] == pin) return i;
  }
  return -1;
}

// Setează valoarea PWM (pentru pinii hardware folosește analogWrite)
void setLEDDuty(int idx, int val) {
  leds[idx].pwmValue = val;
  if (hasHardwarePWM[idx]) {
    analogWrite(leds[idx].pin, val);
  }
}

// Calculează intensitatea fiecărui LED pe baza timpului scurs (millis)
void updateAnimations() {
  unsigned long now = millis();

  for (int i = 0; i < NUM_LEDS; i++) {
    if (leds[i].anim == OFF) {
      setLEDDuty(i, 0);
      continue;
    }

    if (leds[i].duration == 0) continue;

    // Poziția curentă în cadrul ciclului repetitiv
    unsigned long elapsed = (now - leds[i].startTime) % leds[i].duration;

    switch (leds[i].anim) {
      case BLINK:
        if (elapsed < leds[i].duration / 2) {
          setLEDDuty(i, 255);
        } else {
          setLEDDuty(i, 0);
        }
        break;

      case FADEIN: {
        int val = map(elapsed, 0, leds[i].duration, 0, 255);
        setLEDDuty(i, val);
        break;
      }

      case FADEOUT: {
        int val = map(elapsed, 0, leds[i].duration, 255, 0);
        setLEDDuty(i, val);
        break;
      }

      case FADEINOUT: {
        unsigned long half = leds[i].duration / 2;
        int val = 0;
        if (elapsed < half) {
          val = map(elapsed, 0, half, 0, 255);
        } else {
          val = map(elapsed - half, 0, half, 255, 0);
        }
        setLEDDuty(i, val);
        break;
      }
    }
  }
}

// Emulează PWM în software pentru pinii fără PWM HW (8, 12, 13)
void handleSoftPWM() {
  unsigned long currentMicros = micros();
  unsigned long cycleTime = currentMicros % 2550; // Ciclul SoftPWM are o perioadă de 2.55 ms
  int currentLevel = cycleTime / 10;               // Nivel curent între 0 și 255

  for (int i = 0; i < NUM_LEDS; i++) {
    if (!hasHardwarePWM[i]) {
      if (leds[i].pwmValue > currentLevel) {
        digitalWrite(leds[i].pin, HIGH);
      } else {
        digitalWrite(leds[i].pin, LOW);
      }
    }
  }
}