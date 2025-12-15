// Définition des variables --------------------------------------------------------------------------------------------------------------------------------------------------------------

bool ButtonBPArm() {
  int currentStateARM = digitalRead(Bp_Arm);

  // FRONT MONTANT (pression) : on détecte HIGH -> LOW
  if (lastStateARM == HIGH && currentStateARM == LOW) {
    Serial.println("Front detecté ARM!");
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
    Serial.println("Front detecté CONF!");
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
    Serial.println("Front detecté Auto!");
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
    Serial.println("Front detecté Retard!");
    lastStateRetard = currentStateRetard;
    return true;  // bouton pressé
  }

  lastStateRetard = currentStateRetard;
  return false;   // bouton non pressé
}

// Définition des fonctions ----------------------------------------------------------------------------------------------------------------------------------------------------------------

// Récupération distance

int detection() {
  a=sr04.Distance();
  return a;
}


//Gestion des états

void MachineEtat::handleVEILLE(){
  a = detection();
  if (a < 15) {
    currentState = State::PREPA_GEN;
    entryTimePREPA_GEN = currentTime; // mémoriser le temps d'entrée
    Serial.println("Entrée PREPA_GEN !");
    }
  
}


void MachineEtat::handlePREPA_GEN(){
  detection();

  // Vérification du temps
  if ((currentTime - entryTimePREPA_GEN) >= maxSecondsPREPA_GEN) {
    Serial.println("Temps dépassé PREPA_GEN -> retour VEILLE !");
    currentState = State::VEILLE;
    // ici tu peux désactiver servo, LED, buzzer
  }

  // Vérification bouton ARM pour passer à l'état suivant
  if (ButtonBPArm()) {
    currentState = State::PREPA_ACT;
    Serial.println("Bouton CONF pressé -> PREPA_ACT");
    //Allumage système (indication écran)
  }
}


void MachineEtat::handlePREPA_ACT(){
  if (ButtonBPArm()) {
    currentState = State::PREPA_GEN; // ATTENTION !!!!!!! VERIF NON ENCHAINEMENT VALIDATION CONDITION BPArm
    Serial.println("Bouton CONF pressé -> PREPA_GEN");
    //Extinction système
  }
  if (ButtonBPAuto()) {
    currentState = State::PREPA_MODE_AUTO;
    Serial.println("Bouton Auto pressé -> PREPA_MODE_AUTO");
    //affichage texte
  }
  if (ButtonBPRetard()) {
    currentState = State::PREPA_MODE_RETARD;
    Serial.println("Bouton Retard pressé -> PREPA_MODE_RETARD");
    //affichage texte
  }
}


void MachineEtat::handlePREPA_MODE_AUTO(){
  if (ButtonBPConf()) {
    currentState = State::ARM_AUTO;
    Serial.println("Bouton CONF pressé -> ARM_AUTO");
    //Allumage système (indication écran, timer avant armement)
  }
}


void MachineEtat::handlePREPA_MODE_RETARD(){
  
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

    case State::PREPA_MODE_AUTO:
      handlePREPA_MODE_AUTO();
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
