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

unsigned NoteB = NOTE_G5; //Note validation
unsigned NotePB = NOTE_C5; //Note pas bien

// Définition des fonctions ----------------------------------------------------------------------------------------------------------------------------------------------------------------

// AFFICHAGE BUFFER (TM1637)

void displayBuffer() {
  int num = (buffer[0]-'0')*1000 + (buffer[1]-'0')*100 + (buffer[2]-'0')*10 + (buffer[3]-'0');
  display.showNumberDec(num, true);
  Serial.print(F("Buffer: "));
  for(int i=0;i<4;i++) Serial.print(buffer[i]);
  Serial.println();
}


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
  tone(11, NoteB, 500);
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
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(F("Bienvenue|Armer"));
    digitalWrite(ledPin, HIGH);
    stateLED = true;
    tone(11, NoteB, 500);
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
    delay(3000);
  } else {
    // Avertissement fermeture
    lcd.setCursor(0,1);
    lcd.print(F("                ")); //effacement ligne 2
    lcd.setCursor(0,1);
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
    tone(11, NoteB, 500);
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
    tone(11, NoteB, 500);
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
    lcd.print(F("Dispositif arme"));
    lcd.setCursor(0,1);
    lcd.print(F("Fermeture"));
    tone(11, NoteB, 500);
    delay(5000);
    display.setBrightness(0x00);
    myservo.write(0);
    lastBlinkTime = currentTime;
    stateLED = true;
  }

  if (ButtonBPRetard()) {
    currentState = State::PREPA_MODE_RETARD_CODE;
    Serial.println(F("Bouton Retard pressé, passage prepa code"));
    tone(11, NoteB, 500);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(F("Choisir code"));
    Code = "";
    EssaiCode ="";
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
        tone(11,NotePB,500);
        if (EssaiCode.length() > 0) {
          EssaiCode.remove((EssaiCode.length())-1,1);
          Serial.println(F("Caractère supprimé"));
          lcd.setCursor(0,1);
          lcd.print(F("                "));
          Serial.println(EssaiCode);
          lcd.setCursor(0,1);
          lcd.print(EssaiCode);
        }
      } else if ((key == '#')){
        if (Code == EssaiCode) {
          currentState = State::PREPA_MODE_RETARD;
          countdownStarted = false;
          Serial.println(F("Code bon, passage prepa retard"));
          tone(11,NoteB,500);
          IsCode = false;
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(F("Choisir un jour"));
          lcd.setCursor(0,1);
          lcd.print(F("J + 0"));
          EssaiCode = "0";
        } else {
          tone(11,NotePB,500);
          Serial.println(F("Code pas bon"));
          EssaiCode = "";
          lcd.setCursor(0,1);
          lcd.print(F("                "));
        }
        
      } else if (EssaiCode.length() >= 15) {
        tone(11,NotePB,500);
        Serial.println(F("Code trop long"));
      } else {
        tone(11,NoteB,500);
        EssaiCode += key;
        Serial.println(F("Caractère ajouté"));
        lcd.setCursor(0,1);
        lcd.print(F("                "));
        Serial.println(EssaiCode);
        lcd.setCursor(0,1);
        lcd.print(EssaiCode);
      }
    } else {
      if (key == '*') {
        tone(11,NotePB,500);
        if (Code.length() > 0) {
          Code.remove((Code.length())-1,1);
          Serial.println(F("Caractère supprimé"));
          Serial.println(Code);
          lcd.setCursor(0,1);
          lcd.print(F("                "));
          lcd.setCursor(0,1);
          lcd.print(Code);
        }
      } else if (key == '#'){
        if (Code.length() == 0) {
          tone(11,NotePB,500);
          Serial.println(F("Code vide"));
        } else {
          IsCode = true;
          tone(11,NotePB,500);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(F("Confirmer code"));
          Serial.println(F("code validé"));
        }
      } else if (Code.length() >= 15) {
        tone(11,NotePB,500);
        Serial.println(F("Code trop long"));  
      } else {
        tone(11,NoteB,500);
        Code += key;
        Serial.println(F("Caractère ajouté"));
        lcd.setCursor(0,1);
        lcd.print(F("                "));
        Serial.println(Code);
        lcd.setCursor(0,1);
        lcd.print(Code);
      }
    }
  }
}


void MachineEtat::handlePREPA_MODE_RETARD(){

  if (countdownStarted and (currentTime - entryTimePREPA_GEN < 5)) { // fermeture
    myservo.write(0);
    lcd.clear();
    currentState = State::ARM_RETARD;
    Serial.println(F("passage arm retard"));
  }
  
  if (ButtonBPArm()) {
    IsCode = false; //sert a savoir si le jour a été renseigné
    countdownStarted = false;
    desarm();
  }
  char key = customKeypad.getKey();
  if (not(IsCode)) {
    if (key){
      if (key == '*') {
        tone(11,NotePB,500);
        EssaiCode = ""; // var contenant le jour
        Serial.println(F("Caractère supprimé"));
        lcd.setCursor(4,1);
        lcd.print(F(" "));
        Serial.println(EssaiCode);
        lcd.setCursor(4,1);
        lcd.print(EssaiCode);
      } else if ((key == '#')){
        IsCode = true;
        Serial.println(F("Date confirmée"));
        tone(11,NoteB,500);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(F("Choisir heure"));
        lcd.setCursor(0,1);
        lcd.print(F("Format HH:MM"));
        digitalWrite(ledPin,LOW);
        jour = EssaiCode.toInt();
        EssaiCode = "";
        displayBuffer();
      }  else if ((key == 'A') or (key == 'B') or (key == 'C') or (key == 'D')) {
          tone(11,NotePB,500);
          Serial.println(F("Pas bon"));
      } else {
        tone(11,NoteB,500);
        EssaiCode = key;
        Serial.println(F("Caractère ajouté"));
        lcd.setCursor(4,1);
        lcd.print(F(" "));
        Serial.println(EssaiCode);
        lcd.setCursor(4,1);
        lcd.print(EssaiCode);
      }
    }
  } else {
    if (key){
      if (key == '*') {
        tone(11,NotePB,500);
        if (EssaiCode != "") {
          EssaiCode.remove((EssaiCode.length())-1,1);
          buffer[EssaiCode.length()] = '0';
          Serial.println(F("Caractère supprimé"));
          Serial.println(EssaiCode);
          Serial.println(buffer);
          displayBuffer();
        }
      } else if ((key == '#')){
        if (EssaiCode.length() == 4) {
          heure_ret = EssaiCode.substring(0,2).toInt();
          minute_ret = EssaiCode.substring(2,4).toInt();
          Serial.println(heure_ret);
          Serial.println(minute_ret);
          if ((heure_ret < 24) and (minute_ret < 60)) {
            if ((jour == 0 and (((dt.hour == heure_ret) and (dt.minute < 58) and (minute_ret > (dt.minute + 2))) or (((dt.hour + 1) == heure_ret) and (((dt.minute > 58) and (minute_ret > 1)) or (minute_ret < 59))) or ((dt.hour + 1) < heure_ret))) or ((jour == 1) and (((dt.hour == 23) and ((dt.minute < 59) or ((dt.minute < 58) and ((heure_ret > 0) or (minute_ret > 1))))) or (dt.hour != 23))) or (jour > 1)) {
              tone(11,NoteB,500);
              Serial.println(F("heure validée : fermeture"));
              countdownStarted = true; // permet de savoir si le temps a été validé
              entryTimePREPA_GEN = currentTime; // temps depuis confirmation
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print(F("Fermeture"));
              EssaiCode = "";
              /////////////////////////////////////////////////////////////////////////////////////////////// calcul t explosion en sec
            } else {
              Serial.println(F("temps antérieur"));
              tone(11,NotePB,500);
            }
          } else {
            Serial.println(F("out of range time"));
            tone(11,NotePB,500);
          }
        } else {
          Serial.println(F("code trop court"));
          tone(11,NotePB,500);
        }
      } else if ((key == 'A') or (key == 'B') or (key == 'C') or (key == 'D')) {
        tone(11,NotePB,500);
        Serial.println(F("Pas bon"));
      } else if (EssaiCode.length() != 4) {
        tone(11,NoteB,500);
        buffer[EssaiCode.length()] = key;
        EssaiCode += key;
        Serial.println(F("Caractère ajouté"));
        Serial.println(EssaiCode);
        Serial.println(buffer);
        displayBuffer();
      } else {
        tone(11,NotePB,500);
        Serial.println(F("Pas bon"));
      }
    }
  }
}


void MachineEtat::handleARM_AUTO(){
  a = detection();

  if (a <= 15) {
    Serial.println(F("Kaboom dans 5s"));
    currentState = State::BOOM;
    //prepa boom /////////////////////////////////////////////////////////////////////////////// à faire
  }
  if ((currentTime - lastBlinkTime) >= 3) {
    lastBlinkTime = currentTime;
    if (stateLED) {
      digitalWrite(ledPin, LOW);
      stateLED = false;
      
    } else {
      tone(11, NoteB, 500);
      digitalWrite(ledPin, LOW);
      stateLED = true;
    }
  }
}


void MachineEtat::handleARM_RETARD(){
  ///////////////////////////////////////////////////////////////////////////////////////////////// à faire
}


void MachineEtat::handleBOOM(){ ////////////////////////////////////////////////////////////////// à faire
  //buzzer rapide, timer 10 sec, led, boom
  //fin après 10sec
  //standby 5sec puis retour prepa_gen
}


void MachineEtat::handleDESARM(){ ////////////////////////////////////////////////////////////// à faire
  //buzzer victoire, led
  //standby 5sec puis retour prepa_gen
}


// Mise à jour des états -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void MachineEtat::update() {
  // Lecture temps RTC
  dt = clock.getDateTime();
  //currentTime = dt.hour * 3600UL + dt.minute * 60UL + dt.second;
  currentTime = dt.unixtime;

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
