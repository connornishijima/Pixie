/*
	Pixie MILLIS Example
	-----------------------
	
	A good example of your PIXIE / controller's max refresh rate,
	this also shows how to print floating-point variables to certain
	numbers of decimal places.
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
  pix.clear();
  pix.print( millis()/1000.0, 2 ); // Show floating-point number to two decimal places (hundredths of a second)
  pix.show();
}