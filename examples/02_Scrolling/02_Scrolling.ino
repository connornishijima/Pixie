/*
	Pixie SCROLLING Example
	-----------------------
	
	The pix.scroll_message() function is a
	blocking function, not requiring pix.show()
	to send data to update the displays.
	
	It can be customized to speed up scrolling
	or skip the scrolling animation.
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
  // scroll_message(char* input, uint16_t wait_ms = 100, bool instant = false);
  // Blocking function, doesn't require pix.show();
  
  pix.scroll_message("Hello, from your new PIXIES!"); // Standard
  pix.scroll_message("Hello, from your new PIXIES!", 0); // No waiting at each display
  pix.scroll_message("Hello, from your new PIXIES!", 250, true); // No scroll animation, wait 250ms at each display
}