const int ledPin = 13;
char currentMode = '2'; //  oprit

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  Serial.begin(9600);
}

void loop() {
  // verificam daca am primit o comanda noua pe serial
  if (Serial.available() > 0) {
    char option = Serial.read();

    if (option == '1') {
      currentMode = '1';
      digitalWrite(ledPin, HIGH);
      Serial.println("pornit");
    } 
    else if (option == '2') {
      currentMode = '2';
      digitalWrite(ledPin, LOW);
      Serial.println("oprit");
    } 
    else if (option == '3') {
      currentMode = '3';
      Serial.println("blink");
    }
  }

  // daca modul activ este blink la 10 secunde (10000 ms)
  if (currentMode == '3') {
    digitalWrite(ledPin, HIGH);
    delay(10000); // 10 secunde aprins
    digitalWrite(ledPin, LOW);
    delay(10000); // 10 secunde stins
  }
}