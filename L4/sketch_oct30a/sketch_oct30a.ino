// Definitia frecventelor notelor muzicale (in Hz)
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define REST     0

// Pinul la care este conectat buzzer-ul
const int BUZZER_PIN = 3;

// Tempo-ul melodiei (Tempo di marcia)
const int TEMPO = 110;

// Notele melodiei (Deșteaptă-te, române!)
int melody[] = {
  // Masura 1-4: Deșteaptă-te, române, din somnul cel de moarte[cite: 1]
  NOTE_A4, NOTE_A4, NOTE_C5, NOTE_B4, NOTE_A4,
  NOTE_GS4, NOTE_A4, NOTE_B4, NOTE_E4,
  NOTE_A4, NOTE_B4, NOTE_C5, NOTE_D5,
  NOTE_C5, NOTE_B4, NOTE_A4,
  
  // Masura 5-8: În care te-adânciră barbarii de tirani![cite: 1]
  NOTE_A4, NOTE_A4, NOTE_C5, NOTE_B4, NOTE_A4,
  NOTE_GS4, NOTE_A4, NOTE_B4, NOTE_E4,
  NOTE_A4, NOTE_B4, NOTE_C5, NOTE_B4,
  NOTE_A4, NOTE_GS4, NOTE_A4,

  // Masura 9-12: Acum ori niciodată croiește-ți altă soartă[cite: 1]
  NOTE_C5, NOTE_D5, NOTE_E5, NOTE_E5, NOTE_E5,
  NOTE_D5, NOTE_C5, NOTE_B4, NOTE_C5, NOTE_D5,
  NOTE_E5, NOTE_D5, NOTE_C5, NOTE_B4,
  NOTE_C5, NOTE_B4, NOTE_A4,

  // Masura 13-16: La care să se-nchine și cruzii tăi dușmani![cite: 1]
  NOTE_A4, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5,
  NOTE_D5, NOTE_C5, NOTE_B4, NOTE_C5, NOTE_D5,
  NOTE_C5, NOTE_B4, NOTE_A4, NOTE_GS4,
  NOTE_A4
};

// Durata fiecarui note: 4 = patrime, 8 = optime, 16 = saisprezecime etc.
int noteDurations[] = {
  // Masura 1-4
  4, 8, 4, 8, 4,
  4, 8, 4, 4,
  4, 8, 4, 8,
  4, 8, 2,

  // Masura 5-8
  4, 8, 4, 8, 4,
  4, 8, 4, 4,
  4, 8, 4, 8,
  4, 8, 2,

  // Masura 9-12
  4, 8, 4, 8, 4,
  4, 8, 4, 8,
  4, 8, 4, 8,
  4, 8, 2,

  // Masura 13-16
  4, 8, 4, 8, 4,
  4, 8, 4, 8,
  4, 8, 4, 8,
  2
};

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  playAnthem();
}

void loop() {
  // Melodia ruleaza o singura data la pornire.
  // Daca se doreste repetarea continua, functia playAnthem() se poate muta aici.
}

void playAnthem() {
  int notesCount = sizeof(melody) / sizeof(melody[0]);
  
  // Calculul duratei unei patrimi in milisecunde
  int wholenote = (60000 * 4) / TEMPO;

  for (int thisNote = 0; thisNote < notesCount; thisNote++) {
    int noteDuration = 0;
    int durationValue = noteDurations[thisNote];

    if (durationValue > 0) {
      noteDuration = wholenote / durationValue;
    } else if (durationValue < 0) {
      // Note cu punct (durata crescuta cu 50%)
      noteDuration = wholenote / abs(durationValue);
      noteDuration *= 1.5;
    }

    // Redarea notei pe pinul setat
    if (melody[thisNote] != REST) {
      tone(BUZZER_PIN, melody[thisNote], noteDuration * 0.9);
    } else {
      noTone(BUZZER_PIN);
    }

    // Pauza scurta intre note pentru a le distinge clar
    delay(noteDuration);
    noTone(BUZZER_PIN);
  }
}