//On importe les librairies nécessaires

#include <TM1637Display.h>

#include "SR04.h" // detecteur distance

#include <Wire.h> // pour RTC 
#include <DS3231.h> // Pour RTC

#include <Keypad.h> // pour pad

#include "pitches.h" // pour buzzer pin 11

#include <Servo.h> // Pour servo pin 10

#define ledPin 6 

#define CLK 13 // ecran 4 chiffre
#define DIO 12 // ecran 4 chiffre

#define Bp_Arm 2 // def bouton arm 
#define Bp_Confirm 3 // def bouton confirm

#define TRIG_PIN 9 // detecteur distance
#define ECHO_PIN 8 // detecteur distance



Servo myservo; // Pour servo 

DS3231 clock;    // Pour RTC
RTCDateTime dt;  // Pour RTC

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns

byte rowPins[ROWS] = {22, 24, 26, 28}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {30, 32, 34, 36}; //connect to the column pinouts of the keypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

SR04 sr04 = SR04(ECHO_PIN,TRIG_PIN); // detecteur distance
long a; // detecteur distance



void setup() {
  // put your

}

void loop() {
  // put your main code here, to run repeatedly:

}
