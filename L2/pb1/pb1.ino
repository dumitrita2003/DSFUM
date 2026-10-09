// Pinii la care sunt conectate cele 6 LED-uri (de la bitul cel mai putin semnificativ LSB la MSB)
const int ledPins[6] = {8, 9, 10, 11, 12, 13};

void setup() {
  for (int i = 0; i < 6; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
}

void loop() {
  // Numaratoare binara de la 0 la 63 (6 biti: 2^6 = 64 combinatii)
  for (int numar = 0; numar <= 63; numar++) {
    for (int bit = 0; bit < 6; bit++) {
      // Extragem valoarea bitului curent (0 sau 1)
      int stareBit = (numar >> bit) & 1;
      digitalWrite(ledPins[bit], stareBit);
    }
    
    // Pauza intre numere (500 ms = jumatate de secunda)
    delay(500);
  }
}