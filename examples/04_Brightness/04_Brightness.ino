/*
	Pixie BRIGHTNESS Example
	-----------------------
	
	The pix.brightness() function allows for
	7-bit-brightness control (0,127) of the
	Pixie display chain.
*/

#include "Pixie.h"
#define NUM_PIXIES  6                     // PCBs, not matrices
#define CLK_PIN     4                     // Any digital pin
#define DATA_PIN    5                     // Any digital pin
Pixie pix(NUM_PIXIES, CLK_PIN, DATA_PIN); // Set up display buffer

void setup() {
  pix.begin(); // Init display drivers
}

void loop() {
  fade_message("Hello!");
  fade_message("Fade in,");
  fade_message("or fade out");
  fade_message("any text");
  fade_message("you want!");
}

void fade_message(char* in) {
  for (uint8_t i = 0; i < 128; i += 4) { // Fade from black to full brightness
    pix.brightness(i);
    pix.clear();
    pix.print(in);
    pix.show();
  }
  delay(250); // Wait a moment
  for (uint8_t i = 0; i < 128; i += 4) { // Fade from full brightness to black
    pix.brightness(127 - i);
    pix.clear();
    pix.print(in);
    pix.show();
  }
  pix.brightness(0); // Fully black
  pix.clear();
  pix.show();
  delay(250); // Wait a moment
}