#include <TM1637Display.h>
#include <Wire.h>
#include <DS3231.h>
#include <Keypad.h>
#include <LiquidCrystal.h>

// ---------------------------
// CONFIGURATION AFFICHEUR
// ---------------------------
#define CLK 13
#define DIO 12
TM1637Display display(CLK, DIO);

// ---------------------------
// LCD (PARALLEL) - NE TOUCHE PAS AUX PINS
// ---------------------------
LiquidCrystal lcd(2,3,4,5,6,7); // RS, EN, D4, D5, D6, D7

// ---------------------------
// CONFIGURATION RTC DS3231
// ---------------------------
DS3231 clock;
RTCDateTime dt;

// ---------------------------
// CONFIGURATION PAVE NUMERIQUE
// ---------------------------
const byte ROWS = 4;
const byte COLS = 4;
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {22, 24, 26, 28};
byte colPins[COLS] = {30, 32, 34, 36};
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

// ---------------------------
// CONFIGURATION BUZZER + LED
// ---------------------------
#define BUZZER_PIN 11
#define LED_PIN 40
#define BUZZER_FREQ 2000
#define BUZZER_SHORT 100

// ---------------------------
// VARIABLES
// ---------------------------
char buffer[4] = {'0','0','0','0'};
bool countdownStarted = false;
bool waitingNewInput = false;
long targetSeconds = 0;
long lastBeepSecond = -1;
unsigned long lastBlinkTime = 0;
bool blinkState = false;
bool ledState = false;

// ---------------------------
// SETUP
// ---------------------------
void setup() {
  Serial.begin(9600);
  display.setBrightness(0x0f);

  // --- LCD : affichage initial demandé ---
  lcd.begin(16,2);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("taper l'HH:MM");
  lcd.setCursor(0,1);
  lcd.print("target");

  clock.begin();
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  Serial.println("System Ready");
}

// ---------------------------
// CONVERSION SECONDES
// ---------------------------
long convertToSeconds(int h, int m, int s) {
  return (long)h*3600L + (long)m*60L + (long)s;
}

// ---------------------------
// AFFICHAGE BUFFER (TM1637)
// ---------------------------
void displayBuffer() {
  int num = (buffer[0]-'0')*1000 + (buffer[1]-'0')*100 + (buffer[2]-'0')*10 + (buffer[3]-'0');
  display.showNumberDec(num, true);
  Serial.print("Buffer: ");
  for(int i=0;i<4;i++) Serial.print(buffer[i]);
  Serial.println();
}

// ---------------------------
// LOOP
// ---------------------------
void loop() {
  char key = customKeypad.getKey();
  unsigned long currentMillis = millis();

  // --- GESTION SAISIE ---
  if (!countdownStarted && !waitingNewInput && key) {
    if (key >= '0' && key <= '9') {
      buffer[0] = buffer[1];
      buffer[1] = buffer[2];
      buffer[2] = buffer[3];
      buffer[3] = key;
      displayBuffer();
    } else if (key == 'D') {
      buffer[3] = buffer[2];
      buffer[2] = buffer[1];
      buffer[1] = buffer[0];
      buffer[0] = '0';
      displayBuffer();
    } else if (key == '*') {
      int hh = (buffer[0]-'0')*10 + (buffer[1]-'0');
      int mm = (buffer[2]-'0')*10 + (buffer[3]-'0');
      targetSeconds = convertToSeconds(hh, mm, 0);
      
      dt = clock.getDateTime();
      long nowSeconds = convertToSeconds(dt.hour, dt.minute, dt.second);

      if (targetSeconds <= nowSeconds) {
        // --- AJOUT LCD : erreur si target déjà passé ---
        waitingNewInput = true;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Erreur t<Ttarget");
        lcd.setCursor(0,1);
        lcd.print("Remettre target");

        Serial.println("Target time already passed, enter new value.");
      } else {
        countdownStarted = true;
        lastBeepSecond = -1;

        // --- AJOUT LCD : confirmation démarrage chrono ---
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Valider, compte");
        lcd.setCursor(0,1);
        lcd.print("a rebours go   ");

        Serial.print("Countdown started for ");
        Serial.print(hh); Serial.print(":"); Serial.println(mm);
      }
    }
  }

  // --- GESTION CLIGNOTEMENT SI TEMPS PASSE ---
  if (waitingNewInput) {
    if (currentMillis - lastBlinkTime >= 500) {
      lastBlinkTime = currentMillis;
      blinkState = !blinkState;
      if (blinkState) display.showNumberDec(0, true);
      else display.clear();
    }

    // Nouvelle saisie : remettre message initial sur LCD et reprise saisie TM
    if (key && key >= '0' && key <= '9') {
      waitingNewInput = false;
      buffer[0] = buffer[1] = buffer[2] = buffer[3] = '0';
      buffer[3] = key;
      displayBuffer();

      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("taper l'HH:MM");
      lcd.setCursor(0,1);
      lcd.print("target");
    }
  }

  // --- DECOMPTE NON BLOQUANT ---
  if (countdownStarted) {
    dt = clock.getDateTime();
    long nowSeconds = convertToSeconds(dt.hour, dt.minute, dt.second);
    long remaining = targetSeconds - nowSeconds;
    if (remaining < 0) remaining = 0;

    display.showNumberDec(remaining, true);
    Serial.print("Remaining seconds: "); Serial.println(remaining);

    // --- Gestion Buzzer + LED ---
    if (remaining > 10) {
      if (remaining % 10 == 0 && remaining != lastBeepSecond) {
        tone(BUZZER_PIN, BUZZER_FREQ, BUZZER_SHORT);
        digitalWrite(LED_PIN, HIGH);
        lastBlinkTime = currentMillis;
        lastBeepSecond = remaining;
      }
    } else if (remaining <= 10 && remaining > 3) {
      if (remaining != lastBeepSecond) {
        tone(BUZZER_PIN, BUZZER_FREQ, BUZZER_SHORT);
        digitalWrite(LED_PIN, HIGH);
        lastBlinkTime = currentMillis;
        lastBeepSecond = remaining;
      }
    } else if (remaining <= 3 && remaining > 2) {
      if ((currentMillis - lastBlinkTime >= 500) || lastBeepSecond != remaining) {
        tone(BUZZER_PIN, BUZZER_FREQ, BUZZER_SHORT);
        digitalWrite(LED_PIN, HIGH);
        lastBlinkTime = currentMillis;
        lastBeepSecond = remaining;
      }
    } else if (remaining <= 2 && remaining > 1) {
      if ((currentMillis - lastBlinkTime >= 250) || lastBeepSecond != remaining) {
        tone(BUZZER_PIN, BUZZER_FREQ, BUZZER_SHORT);
        digitalWrite(LED_PIN, HIGH);
        lastBlinkTime = currentMillis;
        lastBeepSecond = remaining;
      }
    } else if (remaining == 1 && lastBeepSecond != remaining) {
      tone(BUZZER_PIN, BUZZER_FREQ);
      digitalWrite(LED_PIN, HIGH); // LED allumée pendant la dernière seconde
      lastBeepSecond = remaining;
    }

    // Eteindre LED après bip court si pas dernière seconde
    if (remaining != 1 && digitalRead(LED_PIN) == HIGH && (currentMillis - lastBlinkTime >= BUZZER_SHORT)) {
      digitalWrite(LED_PIN, LOW);
    }

    // --- FIN CHRONO NON BLOQUANT ---
    if (remaining == 0) {
      noTone(BUZZER_PIN);
      digitalWrite(LED_PIN, LOW);
      countdownStarted = false;

      // Clignotement TM1637 toutes les 500ms pendant 3s
      if (currentMillis - lastBlinkTime >= 500) {
        lastBlinkTime = currentMillis;
        blinkState = !blinkState;
        if (blinkState) display.showNumberDec(0, true);
        else display.clear();
      }

      // Réinitialiser buffer
      buffer[0] = buffer[1] = buffer[2] = buffer[3] = '0';
      displayBuffer();
      Serial.println("Countdown finished, buffer reset.");

      // --- AJOUT LCD : remettre message initial ---
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("taper l'HH:MM");
      lcd.setCursor(0,1);
      lcd.print("target");
    }
  }
}
