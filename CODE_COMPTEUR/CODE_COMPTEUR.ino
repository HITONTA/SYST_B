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
// AJOUT : BOUTONS JOUR
// ---------------------------
#define BTN_PLUS 44
#define BTN_MINUS 42
#define BTN_VALIDATE 46

int dayOffset = 0;            // <<< AJOUT
bool selectingDay = true;     // <<< AJOUT
unsigned long lastButtonCheck = 0; // <<< AJOUT
const unsigned long debounceTime = 50; // <<< AJOUT

// --- pour front montant
bool lastStatePlus = HIGH;       // <<< AJOUT
bool lastStateMinus = HIGH;      // <<< AJOUT
bool lastStateValidate = HIGH;   // <<< AJOUT

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

  // <<< AJOUT : écran de sélection du jour
  lcd.setCursor(0,0);
  lcd.print("Choisir le jour");
  lcd.setCursor(0,1);
  lcd.print("J + 0");

  clock.begin();
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  Serial.println("System Ready");

  // <<< AJOUT : initialisation boutons
  pinMode(BTN_PLUS, INPUT_PULLUP);
  pinMode(BTN_MINUS, INPUT_PULLUP);
  pinMode(BTN_VALIDATE, INPUT_PULLUP);
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
  unsigned long currentMillis = millis();

  // --------------------------------------------------------
  // <<< AJOUT : GESTION SELECTION JOUR AVEC FRONT MONTANT
  // --------------------------------------------------------
  if (selectingDay) {
    bool statePlus = digitalRead(BTN_PLUS);
    bool stateMinus = digitalRead(BTN_MINUS);
    bool stateValidate = digitalRead(BTN_VALIDATE);

    if (statePlus == LOW && lastStatePlus == HIGH) { // front montant
      dayOffset++;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Choisir le jour");
      lcd.setCursor(0,1);
      lcd.print("J + "); lcd.print(dayOffset);
    }
    if (stateMinus == LOW && lastStateMinus == HIGH) {
      if (dayOffset > 0) dayOffset--;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Choisir le jour");
      lcd.setCursor(0,1);
      lcd.print("J + "); lcd.print(dayOffset);
    }
    if (stateValidate == LOW && lastStateValidate == HIGH) {
      selectingDay = false;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("taper l'HH:MM");
      lcd.setCursor(0,1);
      lcd.print("target");
    }

    lastStatePlus = statePlus;
    lastStateMinus = stateMinus;
    lastStateValidate = stateValidate;

    return; // empêche d’aller plus loin tant que pas validé
  }

  // --------------------------------------------------------
  // TON CODE ORIGINAL REPREND ICI (INTACT)
  // --------------------------------------------------------

  char key = customKeypad.getKey();

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

      // <<< AJOUT : appliquer le décalage J+N
      targetSeconds += (long)dayOffset * 86400L;

      if (targetSeconds <= nowSeconds) {
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

  // --- GESTION CLIGNOTEMENT ---
  if (waitingNewInput) {
    if (currentMillis - lastBlinkTime >= 500) {
      lastBlinkTime = currentMillis;
      blinkState = !blinkState;
      if (blinkState) display.showNumberDec(0, true);
      else display.clear();
    }

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

    // <<< AJOUT : gestion affichage TM1637 si >9999
    if (remaining > 9999) {
      if (currentMillis - lastBlinkTime >= 1000) {
        lastBlinkTime = currentMillis;
        blinkState = !blinkState;
        if (blinkState) display.showNumberDec(9999, true);
        else display.clear();
      }
    } else {
      display.showNumberDec(remaining, true);
    }

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
      digitalWrite(LED_PIN, HIGH);
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

      if (currentMillis - lastBlinkTime >= 500) {
        lastBlinkTime = currentMillis;
        blinkState = !blinkState;
        if (blinkState) display.showNumberDec(0, true);
        else display.clear();
      }

      buffer[0] = buffer[1] = buffer[2] = buffer[3] = '0';
      displayBuffer();
      Serial.println("Countdown finished, buffer reset.");

      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("taper l'HH:MM");
      lcd.setCursor(0,1);
      lcd.print("target");
    }
  }
}
