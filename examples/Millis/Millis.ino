#include "Pixie.h"
#define NUM_PIXIES  6                     // PCBs, not matrices
#define CLK_PIN     14                    // Any digital pin
#define DATA_PIN    12                    // Any digital pin
Pixie pix(NUM_PIXIES, CLK_PIN, DATA_PIN); // Set up display buffer

void setup() {
  pix.begin(); // Init display drivers
}

void loop() {
  pix.clear();
  pix.push(millis()/1000.0, 2); // Hundredths of seconds (two decimal places)
  pix.show();
}