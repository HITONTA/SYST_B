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
    State currentState;
    unsigned long lastStateChange;

private:
    
    
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
