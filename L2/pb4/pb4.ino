const int ledPin = 9;            // Pinul PWM (~9)
int brightness = 0;              // Nivelul curent de luminozitate (0 - 255)
int fadeAmount = 5;              // Pasul de modificare

// Variabila pentru viteza: intervalul in milisecunde dintre pasi (default 30 ms)
// O valoare mai mica inseamna tranzitie mai rapida
unsigned long interval = 30; 
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  Serial.println("Trimite un interval in ms (ex: 5 pt rapid, 50 pt lent):");
}

void loop() {
  // 1. Citim viteza (intervalul in ms) primita pe Serial
  if (Serial.available() > 0) {
    long valoareNoua = Serial.parseInt();
    if (valoareNoua > 0) {
      interval = (unsigned long)valoareNoua;
      Serial.print("Noua perioada per pas este: ");
      Serial.print(interval);
      Serial.println(" ms");
    }
  }

  // 2. Facem fade fara delay folosind millis()
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    analogWrite(ledPin, brightness);

    brightness = brightness + fadeAmount;

    // Inversam directia la capete
    if (brightness <= 0 || brightness >= 255) {
      fadeAmount = -fadeAmount;
    }
  }
}