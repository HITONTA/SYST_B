
const int bouton1  = 2;
const int bouton2  = 3;
bool lastState1 = HIGH;  // état précédent
bool currentState1;
bool lastState2 = HIGH;  // état précédent
bool currentState2;


void setup() {
  // put your setup code here, to run once:
  pinMode(bouton1, INPUT_PULLUP);
  pinMode(bouton2, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println("power on ready to work");
}

void loop() {
  // put your main code here, to run repeatedly:
  currentState1 = digitalRead(bouton1);
  currentState2 = digitalRead(bouton2);
  // FRONT MONTANT (pression) : on détecte HIGH -> LOW
  if (lastState1 == HIGH && currentState1 == LOW) {
    Serial.println("Front detecté 1!");
  }

  if (lastState2 == HIGH && currentState2 == LOW) {
    Serial.println("Front detecté 2!");
  }

  lastState1 = currentState1;
  lastState2 = currentState2;

}
