#include "Pixie.h"
#include "Pixie_Icon_Pack.h"  // <--- NEEDED FOR BUILT-IN ICONS
#define NUM_PIXIES  6                     // PCBs, not matrices
#define CLK_PIN     14                    // Any digital pin
#define DATA_PIN    12                    // Any digital pin
Pixie pix(NUM_PIXIES, CLK_PIN, DATA_PIN); // Set up display buffer

void setup() {
  pix.begin(); // Init display drivers
}

void loop() {
  pix.clear();
  pix.shift("ICON");
  pix.shift(PIX_HEART);                // Built-in icon
  pix.shift(0x5D,0x55,0x55,0x55,0x77); // Custom icon (columns)
  pix.show();
}