const int nonPwmPin = 8; // Pin digital standard (fara PWM hardware)

// Perioada totala a unui ciclu PWM: 2000 microsecunde (2 ms -> ~500 Hz)
const unsigned long totalPeriod = 2000; 

// Timpul cat pinul sta in HIGH in cadrul perioadei
unsigned long highTime = 0; 
unsigned long cycleStartMicros = 0;

void setup() {
  Serial.begin(9600);
  pinMode(nonPwmPin, OUTPUT);
  digitalWrite(nonPwmPin, LOW);

  Serial.println("Introdu tensiunea dorita in volti (ex: 1.25, 2.5, 3.3, 5.0):");
}

void loop() {
  // 1. Citim tensiunea dorita de pe Serial
  if (Serial.available() > 0) {
    float voltage = Serial.parseFloat();

    // Limitare intre 0V si 5V
    if (voltage < 0.0) voltage = 0.0;
    if (voltage > 5.0) voltage = 5.0;

    // Calculam cat timp trebuie sa stea in HIGH din perioada totala
    highTime = (unsigned long)((voltage / 5.0) * totalPeriod);

    Serial.print("Tensiune setata: ");
    Serial.print(voltage, 2);
    Serial.print(" V (Timp HIGH: ");
    Serial.print(highTime);
    Serial.println(" us)");
  }

  // 2. Generare Software PWM folosind micros() (fara delay)
  unsigned long currentMicros = micros();
  unsigned long elapsedInCycle = currentMicros - cycleStartMicros;

  // Daca s-a terminat perioada curenta de 2 ms, incepem una noua
  if (elapsedInCycle >= totalPeriod) {
    cycleStartMicros = currentMicros;
    elapsedInCycle = 0;
  }

  // Comutam starea pinului conform duty cycle-ului
  if (elapsedInCycle < highTime) {
    digitalWrite(nonPwmPin, HIGH);
  } else {
    digitalWrite(nonPwmPin, LOW);
  }
}