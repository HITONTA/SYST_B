//On importe les librairies nécessaires --------------------------------------------------------------------------------------------------------------------------------------------------
#include "Class.h"
#include <TM1637Display.h>

#include "SR04.h" // detecteur distance

#include <Wire.h> // pour RTC 
#include <DS3231.h> // Pour RTC

#include <Keypad.h> // pour pad

#include "pitches.h" // pour buzzer pin 11

#include <Servo.h> // Pour servo pin 10

#define ledPin 40 

#define CLK 13 // ecran 4 chiffre
#define DIO 12 // ecran 4 chiffre

#define Bp_Arm 48 // def bouton arm -- FIL ORANGE
#define Bp_Confirm 46 // def bouton confirm 66 FIL BLANC
#define Bp_Auto 42 // def bouton sélection auto 
#define Bp_Retard 44 // def bouton sélection retard 66 FIL JAUNE

#define TRIG_PIN 9 // detecteur distance
#define ECHO_PIN 8 // detecteur distance

#define SEUIL 50 // distance de detection pour capteur

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
const unsigned long maxSecondsPREPA_GEN = 10; // durée max autorisée



// Corps de code ----------------------------------------------------------------------------------------------------------------------------------------------------------------------


void setup() {
  Serial.begin(9600);
  pinMode(Bp_Arm, INPUT_PULLUP);
  pinMode(Bp_Confirm, INPUT_PULLUP);
  pinMode(Bp_Auto, INPUT_PULLUP);
  pinMode(Bp_Retard, INPUT_PULLUP);
  Serial.println("lets go");

}

void loop() {
  
  machine.update();


}
