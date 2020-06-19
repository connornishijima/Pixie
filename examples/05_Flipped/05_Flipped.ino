/*
	Pixie FLIPPED Example
	-----------------------
	
	The pix.flipped(bool) function can be used
	to flip the display buffer upside-down to
	allow Pixies to be mounted upside-down if
	necessary!
*/

#include "Pixie.h"
#define NUM_PIXIES  6                     // PCBs, not matrices
#define CLK_PIN     14                    // Any digital pin
#define DATA_PIN    12                    // Any digital pin
Pixie pix(NUM_PIXIES, CLK_PIN, DATA_PIN); // Set up display buffer

void setup() {
  pix.begin(); // Init display drivers
}

void loop() {
  pix.flipped(false); // Right-side up
  pix.scroll_message("Normal...");
  pix.flipped(true); // Upside down
  pix.scroll_message("...and flipped!");
}