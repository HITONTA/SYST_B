//On importe les librairies nécessaires --------------------------------------------------------------------------------------------------------------------------------------------------
#include "Class.h"
#include <TM1637Display.h>

#include "SR04.h" // detecteur distance

#include <Wire.h> // pour RTC 

#include <DS3231.h> // Pour RTC

#include <Keypad.h> // pour pad

#include "pitches.h" // pour buzzer pin 11

#include <Servo.h> // Pour servo pin 10

#include <LiquidCrystal.h>

#define ledPin 40 

#define CLK 13 // ecran 4 chiffre
#define DIO 12 // ecran 4 chiffre
TM1637Display display(CLK, DIO);

#define Bp_Arm 48 // def bouton arm -- FIL ORANGE
#define Bp_Confirm 46 // def bouton confirm 66 FIL BLANC
#define Bp_Auto 42 // def bouton sélection auto 
#define Bp_Retard 44 // def bouton sélection retard 66 FIL JAUNE

#define TRIG_PIN 9 // detecteur distance
#define ECHO_PIN 8 // detecteur distance

#define SEUIL 50 // distance de detection pour capteur

#define BUZZER_PIN 11
#define LED_PIN 40
#define BUZZER_FREQ 2000
#define BUZZER_SHORT 100

// Introduction des variables ---------------------------------------------------------------------------------------------------------------------------------------------------------

bool lastStateARM = HIGH;  // état précédent BOUTON BPARm
bool currentStateARM;
bool lastStateCONF = HIGH;  // état précédent BOUTON BpConfirm
bool currentStateCONF;
bool lastStateAuto = HIGH;  // état précédent BOUTON BpAuto
bool currentStateAuto;
bool lastStateRetard = HIGH;  // état précédent BOUTON BpRetard
bool currentStateRetard;

Servo myservo; // Pour servo 

LiquidCrystal lcd(2,3,4,5,6,7); // RS, EN, D4, D5, D6, D7

DS3231 clock;    // Pour RTC
RTCDateTime dt;  // Pour RTC

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns

byte rowPins[ROWS] = {22, 24, 26, 28}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {30, 32, 34, 36}; //connect to the column pinouts of the keypad
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

SR04 sr04 = SR04(ECHO_PIN,TRIG_PIN); // detecteur distance
long a; // detecteur distance capteur

unsigned long entryTimePREPA_GEN = 0; // Pour RTC
unsigned long entryFlashLED = 0;
bool stateLED = false;
bool stateChoiceAuto = false;
int jour = 0;
const unsigned long maxSecondsPREPA_GEN = 30L; // durée max autorisée
bool IsCode = false;
String Code = "";
String EssaiCode = "";
int heure_ret = 0;
int minute_ret = 0;
long int last_affich = 0;
bool mustBlink = false;
long int BlinkSec = 0L;

char buffer[4] = {'0','0','0','0'};
bool countdownStarted = false;
bool waitingNewInput = false;
long targetSeconds = 0;
long lastBeepSecond = -1;
unsigned long lastBlinkTime = 0;
bool blinkState = false;
bool ledState = false;
long targetUnixTime = 0;



// Corps de code ----------------------------------------------------------------------------------------------------------------------------------------------------------------------


void setup() {
  Serial.begin(115200);
  clock.begin();
  pinMode(Bp_Arm, INPUT_PULLUP);
  pinMode(Bp_Confirm, INPUT_PULLUP);
  pinMode(Bp_Auto, INPUT_PULLUP);
  pinMode(Bp_Retard, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  lcd.begin(16,2);
  myservo.attach(10);
  myservo.write(0); // move servos to center position -> 0°
  // Définit un timeout de 25ms (25000 us)
  // Le paramètre 'true' force un reset automatique du bus I2C en cas de timeout
  Wire.setWireTimeout(25000, true);
  Serial.println(F("lets go"));
  delay(3000);
}

void loop() {
  machine.update();


}
