MachineEtat::MachineEtat() {
    currentState = State::VEILLE;
    lastStateChange = 0;
}

MachineEtat machine;

void detection() {
  a=sr04.Distance();
}


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

void MachineEtat::update() {
  // Lecture temps RTC
  dt = clock.getDateTime();
  unsigned long currentTime = dt.hour * 3600UL + dt.minute * 60UL + dt.second;

  switch (currentState) {

    case VEILLE:
      detection();
      if (a < 15) { // ATTENTION SENS < INVERSER POUR TEST
        currentState = PREPA_GEN;
        entryTimePREPA_GEN = currentTime; // mémoriser le temps d'entrée
        Serial.println("Entrée PREPA_GEN !");
      }
      break; // très important pour éviter le fall-through

    case PREPA_GEN:
      detection();

      // Vérification du temps
      if ((currentTime - entryTimePREPA_GEN) >= maxSecondsPREPA_GEN) {
        Serial.println("Temps dépassé PREPA_GEN -> retour VEILLE !");
        currentState = VEILLE;
        // ici tu peux désactiver servo, LED, buzzer
        break;
      }

      // Vérification bouton CONF pour passer à l'état suivant
      if (ButtonBPConf()) {
        currentState = PREPA_ACT;
        Serial.println("Bouton CONF pressé -> PREPA_ACT");
        break;
      }

      break;

    // Tu peux ajouter les autres états ici...
  }
}
