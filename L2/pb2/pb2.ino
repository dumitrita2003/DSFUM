// Pinii conectati la cele 6 LED-uri (8 = LSB, 13 = MSB)
const int ledPins[6] = {8, 9, 10, 11, 12, 13};

void setup() {
  // Initializam comunicarea seriala la 9600 baud
  Serial.begin(9600);

  // Setam pinii LED-urilor ca iesiri
  for (int i = 0; i < 6; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW); // Ne asiguram ca pornesc stinse
  }

  Serial.println("Trimite un numar intre 0 si 63:");
}

void loop() {
  // Verificam daca au sosit date pe portul serial
  if (Serial.available() > 0) {
    // Citim numarul intreg trimis din Serial Monitor
    int numar = Serial.parseInt();

    // Verificam daca numarul se incadreaza pe 6 biti (0 - 63)
    if (numar >= 0 && numar <= 63) {
      Serial.print("Afisez in binar valoarea: ");
      Serial.println(numar);

      // Setam fiecare LED corespunzator fiecarui bit
      for (int bit = 0; bit < 6; bit++) {
        int stareBit = (numar >> bit) & 1;
        digitalWrite(ledPins[bit], stareBit);
      }
    }
  }
}