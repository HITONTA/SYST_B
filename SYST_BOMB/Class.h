// Établissement des classes --------------------------------------------------------------------------------------------------------------------------------------------------------------------

enum class State {
  VEILLE,
  PREPA_GEN,
  PREPA_ACT,
  PREPA_MODE_RETARD_CODE,
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
    unsigned long currentTime;

private:
    
    
    // Méthodes privées pour chaque état
    void handleVEILLE();
    void handlePREPA_GEN();
    void handlePREPA_ACT();
    void handlePREPA_MODE_RETARD_CODE();
    void handlePREPA_MODE_RETARD();
    void handleARM_AUTO();
    void handleARM_RETARD();
    void handleBOOM();
    void handleDESARM();
    void desarm();
};

// Construction de la classe ------------------------------------------------------------------------------------------------------------------------------------------------------------------------

MachineEtat::MachineEtat() {
    currentState = State::VEILLE;
    lastStateChange = 0;
    currentTime = 0;
}

MachineEtat machine;
