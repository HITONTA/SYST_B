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


// Définition des fonctions ----------------------------------------------------------------------------------------------------------------------------------------------------------------

// Récupération distance

void detection() {
  a=sr04.Distance();
}


//Gestion des états

void MachineEtat::handleVEILLE(){
  detection();
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
    //Allumage système
  }
}


void MachineEtat::handlePREPA_ACT(){
  if (ButtonBPArm()) {
    currentState = State::PREPA_GEN; // ATTENTION !!!!!!! VERIF NON ENCHAINEMENT VALIDATION CONDITION BPArm
    Serial.println("Bouton CONF pressé -> PREPA_ACT");
    //Extinction système
  }
  if ()
}


void MachineEtat::handlePREPA_MODE_AUTO(){
  
}


void MachineEtat::handlePREPA_MODE_RETARD(){
  
}


void MachineEtat::handleARM_AUTO(){
  
}


void MachineEtat::handleARM_RETARD(){
  
}


void MachineEtat::handleBOOM(){
  
}


void MachineEtat::handleDESARM(){
  
}


// Mise à jour des états

void MachineEtat::update() {
  // Lecture temps RTC
  dt = clock.getDateTime();
  unsigned long currentTime = dt.hour * 3600UL + dt.minute * 60UL + dt.second;

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
