// Définition des variables --------------------------------------------------------------------------------------------------------------------------------------------------------------

bool ButtonBPArm() {
  int currentStateARM = digitalRead(Bp_Arm);

  // FRONT MONTANT (pression) : on détecte HIGH -> LOW
  if (lastStateARM == HIGH && currentStateARM == LOW) {
    Serial.println(F("Front detecté ARM!"));
    lastStateARM = currentStateARM;
    return true;  // bouton pressé
  }

  lastStateARM = currentStateARM;
  return false;   // bouton non pressé
}



bool ButtonBPConf() {
  int currentStateCONF = digitalRead(Bp_Confirm);

  // FRONT MONTANT (pression) : on détecte HIGH -> LOW
  if (lastStateCONF == HIGH && currentStateCONF == LOW) {
    Serial.println(F("Front detecté CONF!"));
    lastStateCONF = currentStateCONF;
    return true;  // bouton pressé
  }

  lastStateCONF = currentStateCONF;
  return false;   // bouton non pressé
}



bool ButtonBPAuto() {
  int currentStateAuto = digitalRead(Bp_Auto);

  // FRONT MONTANT (pression) : on détecte HIGH -> LOW
  if (lastStateAuto == HIGH && currentStateAuto == LOW) {
    Serial.println(F("Front detecté Auto!"));
    lastStateAuto = currentStateAuto;
    return true;  // bouton pressé
  }

  lastStateAuto = currentStateAuto;
  return false;   // bouton non pressé
}



bool ButtonBPRetard() {
  int currentStateRetard = digitalRead(Bp_Retard);

  // FRONT MONTANT (pression) : on détecte HIGH -> LOW
  if (lastStateRetard == HIGH && currentStateRetard == LOW) {
    Serial.println(F("Front detecté Retard!"));
    lastStateRetard = currentStateRetard;
    return true;  // bouton pressé
  }

  lastStateRetard = currentStateRetard;
  return false;   // bouton non pressé
}

unsigned NoteB = NOTE_A5; //Note validation
unsigned NotePB = NOTE_B5; //Note pas bien

// Définition des fonctions ----------------------------------------------------------------------------------------------------------------------------------------------------------------

// Récupération distance

int detection() {
  a=sr04.Distance();
  return a;
}

// Fonctions annexes

void MachineEtat::desarm() {
  currentState = State::PREPA_GEN; // ATTENTION !!!!!!! VERIF NON ENCHAINEMENT VALIDATION CONDITION BPArm
  Serial.println(F("Bouton Arm pressé -> PREPA_GEN"));
  entryTimePREPA_GEN = currentTime; // mémorise le temps d'entrée
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(F("Bienvenue|Armer"));
  digitalWrite(ledPin, HIGH);
  stateLED = true;
  tone(11, NoteB, 1000);
}

//Gestion des états

void MachineEtat::handleVEILLE(){
  a = detection();

  if (stateLED and ((currentTime - entryFlashLED) >= 1)) {
    digitalWrite(ledPin, LOW);
    stateLED = false;
  }
  
  if (a < 15) {
    currentState = State::PREPA_GEN;
    entryTimePREPA_GEN = currentTime; // mémorise le temps d'entrée
    Serial.println(F("Entrée PREPA_GEN !"));
    myservo.write(90); //                                                Vérif si mouvement servo = non bloquant
    display.setBrightness(0x0f); // Luminosité max (0x00 à 0x0f)
    lcd.begin(16,2);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(F("Bienvenue|Armer"));
    digitalWrite(ledPin, HIGH);
    stateLED = true;
    tone(11, NoteB, 1000);
    }
  
}


void MachineEtat::handlePREPA_GEN(){//                       VERIF currentTime est en secondes

  if (stateLED and ((currentTime - entryTimePREPA_GEN) >= 1)) {
    digitalWrite(ledPin, LOW);
    stateLED = false;
  }

  
  // Vérification du temps
  if ((currentTime - entryTimePREPA_GEN) >= maxSecondsPREPA_GEN) {
    Serial.println(F("Temps dépassé PREPA_GEN -> retour VEILLE !"));
    currentState = State::VEILLE;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(F("Attention"));
    lcd.setCursor(0,1);
    lcd.print(F("Fermeture"));//Inutile...
    tone(11, NoteB, 500);
    entryFlashLED = currentTime;
    digitalWrite(ledPin, HIGH);
    stateLED = true;
    myservo.write(0);
    display.setBrightness(0x00);
    lcd.clear();
  
  } else {
    // Avertissement fermeture
    lcd.setCursor(0,1);
    lcd.print(F("                ")); //effacement ligne 2
    lcd.print(F("Veille dans "));
    lcd.setCursor(12,1);
    lcd.print(maxSecondsPREPA_GEN - (currentTime - entryTimePREPA_GEN));
    if ((currentTime - entryTimePREPA_GEN) <= maxSecondsPREPA_GEN - 9) {
      lcd.setCursor(14,1);
    } else {
      lcd.setCursor(13,1);
    }
    lcd.print(F("s"));
  }

  // Vérification bouton ARM pour passer à l'état suivant
  if (ButtonBPArm()) {
    currentState = State::PREPA_ACT;
    Serial.println(F("Bouton CONF pressé -> PREPA_ACT"));
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(F("Veuillez choisir"));
    lcd.setCursor(0,1);
    lcd.print(F("le mode"));
    tone(11, NoteB, 1000);
    digitalWrite(ledPin, HIGH);
  }
}


void MachineEtat::handlePREPA_ACT(){
  if (ButtonBPArm()) {
    stateChoiceAuto = false;
    desarm();
  }
  if (ButtonBPAuto()) {
    
    stateChoiceAuto = true;
    Serial.println(F("Bouton Auto pressé"));
    tone(11, NoteB, 1000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(F("Etes-vous sur ?"));
    lcd.setCursor(0,1);
    lcd.print(F("Presser Confirm"));
  }
  
  if(ButtonBPConf() and stateChoiceAuto) {
    currentState = State::ARM_AUTO;
    Serial.println(F("Passage ARM_AUTO"));
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(F("Dispositif armé"));
    lcd.setCursor(0,1);
    lcd.print(F("Fermeture"));
    tone(11, NoteB, 1000);
    delay(5000);
    display.setBrightness(0x00);
    myservo.write(0);
    digitalWrite(ledPin,LOW);
  }

  if (ButtonBPRetard()) {
    currentState = State::PREPA_MODE_RETARD_CODE;
    Serial.println(F("Bouton Retard pressé, passage prepa code"));
    tone(11, NoteB, 1000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(F("Choisir code"));
  }
}

void MachineEtat::handlePREPA_MODE_RETARD_CODE(){
  if (ButtonBPArm()) {
    IsCode = false;
    desarm();
  }
  
  char key = customKeypad.getKey();
  if (key) {
    if (IsCode) {
      if (key == '*') {
        tone(11,NotePB,1000);
        if (EssaiCode.length() >= 0) {
          EssaiCode.remove((EssaiCode.length())-1,1);
          Serial.println(F("Caractère supprimé"));
        }
      } else if ((key == '#')){
        if (Code == EssaiCode) {
          currentState = State::PREPA_MODE_RETARD;
          Serial.println(F("Code bon, passage prepa retard"));
          tone(11,NoteB,1000);
          IsCode = false;
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(F("Choisir un jour"));
          lcd.setCursor(0,1);
          lcd.print(F("J + 0"));
          digitalWrite(ledPin,LOW);
        } else {
          tone(11,NotePB,1000);
          Serial.println(F("Code pas bon"));
          EssaiCode = "";
          lcd.setCursor(0,1);
          lcd.print(F("                "));
        }
        
      } else if (EssaiCode.length() >= 15) {
        tone(11,NotePB,1000);
        Serial.println(F("Code trop long"));
      } else {
        tone(11,NoteB,1000);
        EssaiCode += key;
        Serial.println(F("Caractère ajouté"));
        lcd.setCursor(0,1);
        lcd.print(F("                "));
        Serial.println(EssaiCode);
        lcd.print(EssaiCode);
      }
    } else {
      if (key == '*') {
        tone(11,NotePB,1000);
        if (Code.length() >= 0) {
          Code.remove((Code.length())-1,1);
          Serial.println(F("Caractère supprimé"));
          Serial.println(Code);
        }
      } else if (key == '#'){
        if (Code.length() == 0) {
          tone(11,NotePB,1000);
          Serial.println(F("Code vide"));
        } else {
          IsCode = true;
          tone(11,NotePB,1000);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(F("Confirmer code"));
          Serial.println(F("code validé"));
        }
      } else if (Code.length() >= 15) {
        tone(11,NotePB,1000);
        Serial.println(F("Code trop long"));  
      } else {
        tone(11,NoteB,1000);
        Code += key;
        Serial.println(F("Caractère ajouté"));
        lcd.setCursor(0,1);
        lcd.print(F("                "));
        Serial.println(Code);
        lcd.print(Code);
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////// ICI
void MachineEtat::handlePREPA_MODE_RETARD(){
  if (ButtonBPArm()) {
    desarm();
  }
  
}


void MachineEtat::handleARM_AUTO(){
  //led,buzzer
  //if detect -> state::boom
}


void MachineEtat::handleARM_RETARD(){
  
}


void MachineEtat::handleBOOM(){
  //buzzer rapide, timer 10 sec, led, boom
  //fin après 10sec
  //standby 5sec puis retour prepa_gen
}


void MachineEtat::handleDESARM(){
  //buzzer victoire, led
  //tandby 5sec puis retour prepa_gen
}


// Mise à jour des états -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void MachineEtat::update() {
  // Lecture temps RTC
  dt = clock.getDateTime();
  currentTime = dt.hour * 3600UL + dt.minute * 60UL + dt.second;

  switch (currentState) {

    case State::VEILLE:
      MachineEtat::handleVEILLE ();
      break; // très important pour éviter le fall-through
      
    case State::PREPA_GEN:
      MachineEtat::handlePREPA_GEN ();
      break;

    case State::PREPA_ACT:
      handlePREPA_ACT();
      break;

    case State::PREPA_MODE_RETARD_CODE:
      handlePREPA_MODE_RETARD_CODE();
      break;

    case State::PREPA_MODE_RETARD:
      handlePREPA_MODE_RETARD();
      break;

    case State::ARM_AUTO:
      handleARM_AUTO();
      break;

    case State::ARM_RETARD:
      handleARM_RETARD();
      break;

    case State::BOOM:
      handleBOOM();
      break;

    case State::DESARM:
      handleDESARM();
      break;
  }
}
