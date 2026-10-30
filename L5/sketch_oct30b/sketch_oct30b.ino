#include <TimerOne.h> 

// --- PINI MULTIFUNCTION SHIELD ---
const int LATCH_PIN = 4;
const int CLK_PIN   = 7;
const int DATA_PIN  = 8;
const int BUZZER_PIN = 3;

// Tablou de codificare pentru afisajul 7-segmente (anod comun)
const byte SEGMENT_MAP[] = {
  0xC0, // 0
  0xF9, // 1
  0xA4, // 2
  0xB0, // 3
  0x99, // 4
  0x92, // 5
  0x82, // 6
  0xF8, // 7
  0x80, // 8
  0x90  // 9
};

const byte DIGIT_SELECT[] = { 0xF1, 0xF2, 0xF4, 0xF8 };

// --- DEFINIRE NOTE MUZICALE ---
#define NOTE_C4  262
#define NOTE_E4  330
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_E5  659
#define REST     0

// Melodia 0: Semnal scurt / Cântecel 1
int song0_notes[] = { NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5 };
int song0_durations[] = { 200, 200, 200, 400 };
int song0_length = 4;

// Melodia 1: Imn / Cântecel 2 (Fragment)
int song1_notes[] = { NOTE_A4, NOTE_A4, NOTE_C5, NOTE_B4, NOTE_A4, NOTE_GS4, NOTE_A4 };
int song1_durations[] = { 300, 150, 300, 150, 300, 300, 600 };
int song1_length = 7;

// --- VARIABILE STARE SI TIMP ---
enum DisplayMode { MODE_NONE, MODE_CLOCK, MODE_STOPWATCH };
DisplayMode currentMode = MODE_NONE;

// Ceas
int clockSeconds = -1; // -1 inseamna nesetat
int alarmClockSeconds = -1;
int alarmClockSong = 0;
bool alarmClockTriggered = false;

// Stopwatch
int stopwatchSeconds = -1; // -1 inseamna nesetat
int alarmStopwatchSong = 0;
bool stopwatchRunning = false;

// Afișaj
byte displayDigits[4] = {0xFF, 0xFF, 0xFF, 0xFF}; // Gol la început

void setup() {
  Serial.begin(9600);
  
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, HIGH); // Buzzer inactiv (Pnp/Inversat pe shield)

  // Timer la 1 secundă pentru incrementare/decrementare timp
  Timer1.initialize(1000000); 
  Timer1.attachInterrupt(timerIsr);

  Serial.println("Sistem pregatit. Introduceti comenzi:");
}

void loop() {
  // Parsare comenzi Serial
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    parseCommand(input);
  }

  // Update cifre de afisat pe 7-segmente
  updateDisplayDigits();
  
  // Refresh afisaj (multiplexare rapida)
  refreshDisplay();
}

// Interrupție executată în fiecare secundă
void timerIsr() {
  // Incrementare ceas
  if (clockSeconds >= 0) {
    clockSeconds = (clockSeconds + 1) % 86400; // Recilare la 24 ore (86400 secunde)

    // Verificare Alarma Ceas
    if (clockSeconds == alarmClockSeconds && !alarmClockTriggered) {
      playSong(alarmClockSong);
      alarmClockTriggered = true;
    }
  }

  // Decrementare cronometru (Stopwatch)
  if (stopwatchRunning && stopwatchSeconds > 0) {
    stopwatchSeconds--;
    if (stopwatchSeconds == 0) {
      stopwatchRunning = false;
      playSong(alarmStopwatchSong); // Cântă alarma la 00:00
    }
  }
}

// Analizarea comenzilor introduse pe Serial
void parseCommand(String cmd) {
  if (cmd.startsWith("clock ")) {
    String timeStr = cmd.substring(6);
    int parsedSec = timeToSeconds(timeStr);
    if (parsedSec >= 0) {
      if (clockSeconds == -1) { // Doar prima dată după reset
        clockSeconds = parsedSec;
        alarmClockTriggered = false;
        Serial.println("OK: Ora setata.");
      } else {
        Serial.println("Info: Ora a fost deja setata initial.");
      }
      currentMode = MODE_CLOCK;
    }
  } 
  else if (cmd == "clock") {
    currentMode = MODE_CLOCK;
    Serial.println("OK: Comutat pe modul CLOCK.");
  }
  else if (cmd.startsWith("stopwatch ")) {
    String timeStr = cmd.substring(10);
    int parsedSec = timeToSeconds(timeStr);
    if (parsedSec >= 0) {
      if (stopwatchSeconds == -1) { // Doar prima dată după reset
        stopwatchSeconds = parsedSec;
        stopwatchRunning = true;
        Serial.println("OK: Cronometru setat si pornit.");
      } else {
        Serial.println("Info: Cronometrul a fost deja setat initial.");
      }
      currentMode = MODE_STOPWATCH;
    }
  } 
  else if (cmd == "stopwatch") {
    currentMode = MODE_STOPWATCH;
    Serial.println("OK: Comutat pe modul STOPWATCH.");
  }
  else if (cmd.startsWith("alarm clock ")) {
    // Format: alarm clock 13:00 1
    int secondSpace = cmd.lastIndexOf(' ');
    String timeStr = cmd.substring(12, secondSpace);
    int songIdx = cmd.substring(secondSpace + 1).toInt();
    
    int parsedSec = timeToSeconds(timeStr);
    if (parsedSec >= 0) {
      alarmClockSeconds = parsedSec;
      alarmClockSong = songIdx;
      alarmClockTriggered = false;
      Serial.println("OK: Alarma ceas configurata.");
    }
  }
  else if (cmd.startsWith("alarm stopwatch ")) {
    // Format: alarm stopwatch 1
    int songIdx = cmd.substring(16).toInt();
    alarmStopwatchSong = songIdx;
    Serial.println("OK: Alarma cronometru configurata.");
  }
}

// Conversie format "MM:SS" sau "HH:MM" în secunde totale
int timeToSeconds(String t) {
  int colonIndex = t.indexOf(':');
  if (colonIndex == -1) return -1;
  
  int minsOrHours = t.substring(0, colonIndex).toInt();
  int secsOrMins  = t.substring(colonIndex + 1).toInt();
  
  return (minsOrHours * 3600) + (secsOrMins * 60); // interpretat ca Ore:Minute
}

// Actualizare valori de afișat în funcție de modul curent
void updateDisplayDigits() {
  int totalSec = -1;

  if (currentMode == MODE_CLOCK && clockSeconds >= 0) {
    totalSec = clockSeconds;
  } else if (currentMode == MODE_STOPWATCH && stopwatchSeconds >= 0) {
    totalSec = stopwatchSeconds;
  }

  if (totalSec >= 0) {
    int hoursOrMins = (totalSec / 3600) % 24;
    int minsOrSecs  = (totalSec / 60) % 60;

    displayDigits[0] = SEGMENT_MAP[hoursOrMins / 10];
    displayDigits[1] = SEGMENT_MAP[hoursOrMins % 10] & 0x7F; // Punctul decimal aprins
    displayDigits[2] = SEGMENT_MAP[minsOrSecs / 10];
    displayDigits[3] = SEGMENT_MAP[minsOrSecs % 10];
  } else {
    // Dacă nu s-a setat timpul încă, afișează "----"
    displayDigits[0] = 0xBF;
    displayDigits[1] = 0xBF;
    displayDigits[2] = 0xBF;
    displayDigits[3] = 0xBF;
  }
}

// Trimitere date către registrele de deplasare ale shield-ului
void refreshDisplay() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(LATCH_PIN, LOW);
    shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, displayDigits[i]);
    shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, DIGIT_SELECT[i]);
    digitalWrite(LATCH_PIN, HIGH);
    delay(2); // Timp scurt de afișare per digit
  }
}

// Funcție de redare melodii
void playSong(int songIndex) {
  int* notes;
  int* durations;
  int len = 0;

  if (songIndex == 1) {
    notes = song1_notes;
    durations = song1_durations;
    len = song1_length;
  } else {
    notes = song0_notes;
    durations = song0_durations;
    len = song0_length;
  }

  for (int i = 0; i < len; i++) {
    if (notes[i] != REST) {
      tone(BUZZER_PIN, notes[i], durations[i]);
    }
    delay(durations[i] * 1.3);
    noTone(BUZZER_PIN);
  }
}