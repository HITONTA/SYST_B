// Établissement des classes --------------------------------------------------------------------------------------------------------------------------------------------------------------------

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
    State currentState;
    unsigned long lastStateChange;

private:
    
    
    // Méthodes privées pour chaque état
    void handleVEILLE();
    void handlePREPA_GEN();
    void handlePREPA_ACT();
    void handlePREPA_MODE_AUTO();
    void handlePREPA_MODE_RETARD();
    void handleARM_AUTO();
    void handleARM_RETARD();
    void handleBOOM();
    void handleDESARM();
};

// Construction de la classe ------------------------------------------------------------------------------------------------------------------------------------------------------------------------

MachineEtat::MachineEtat() {
    currentState = State::VEILLE;
    lastStateChange = 0;
}

MachineEtat machine;
