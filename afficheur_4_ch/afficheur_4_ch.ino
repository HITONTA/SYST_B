#include <TM1637Display.h>

// Définir les pins CLK et DIO
#define CLK 13
#define DIO 12

TM1637Display display(CLK, DIO);

void setup() {
  display.setBrightness(0x0f); // Luminosité max (0x00 à 0x0f)
}

void loop() {
  int value = 1234;            // Valeur à afficher
  display.showNumberDec(value); // Affiche la valeur

  delay(1000);

  // Test simple : clignotement
  display.clear();
  delay(500);
}
