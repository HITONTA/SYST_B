// Fonctionnement de la machine à état

enum class State {
  VEILLE,
  PREPA_GEN,
  PREPA_ACT,
  PREPA_MODE_AUTO,
  PREPA_MODE_RETARD,
  ARM_AUTO,
  ARM_RETARD,
  BOOM,
  DESARM,
};

class MachineEtat {
public:
    MachineEtat();          // Constructeur
    void init();            // Setup interne
    void update();          // À appeler dans loop()

private:
    State currentState;
    unsigned long lastStateChange;
    
    // Méthodes privées pour chaque état
    void handleVeille();
    void handlePrepaGen();
    void handlePrepaAct();
    void handlePrepaModeAuto();
    void handlePrepaModeRetard();
    void handleArmAuto();
    void handleArmRetard();
    void handleBoom();
    void handleDesarm();
};

void updateFSM() {
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


