/*
  Pixie BASIC Example
  -----------------------
  
  The Pixie library has three basic writing
  functions: write(), push(), and shift().
  
    pix.write(INPUT, POS);
	
	  Writes the variable INPUT at position POS without
	  affecting other displays. (POS is the position
	  from the left-most display, "0" being the left-most display.)
	  
	pix.push(INPUT);
	
	  Adds the variable INPUT to the RIGHT side of the
	  display chain, shifting all others LEFT.
	
	pix.shift(INPUT);
	
	  Adds the variable INPUT to the LEFT side of the
	  display chain, shifting all others RIGHT.
	  
  There are also two other important functions:
  
    pix.show();
	
	  The displays are not updated until pix.show() is
	  called, meaning multiple changes to the displays
	  can be made between updates, such as calling
	  pix.push() more than once.
	
	pix.clear();
	
	  Whenever you want to clear the display buffer of
	  previous data and start fresh, pix.clear() does that.
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
  // BASIC WRITING FUNCTIONS ---------------------------
  pix.clear();         // Clears the PIXIE chain
  pix.write('W', 2);   // Writes 'W' to the third display from the left (Index 2)
  pix.show();          // Pushes changes to the PIXIE chain
  
  delay(3000);         // Wait a moment
  
  pix.push('P');       // Writes 'P' to the right-most display, shifting all others LEFT one position.
  pix.show();          // Pushes changes to the PIXIE chain
  
  delay(3000);         // Wait a moment

  pix.shift('S');      // Writes 'S' to the left-most display, shifting all others RIGHT one position.
  pix.show();          // Pushes changes to the PIXIE chain
  
  delay(3000);         // Wait a moment

  // STRING WRITING -------------------------------------

  pix.clear();         // Clears the PIXIE chain
  pix.shift("Hello!"); // Shift in char array of "Hello!"
  pix.show();          // Pushes changes to the PIXIE chain

  delay(3000);         // Wait a moment

  // INTEGER WRITING FUNCTIONS -------------------------

  // Count from -100 to 100
  for(int16_t i = -100; i <= 100; i++){
    pix.clear();       // Clears the PIXIE chain
    pix.push(i);
    pix.show();        // Pushes changes to the PIXIE chain
  }

  delay(3000);         // Wait a moment

  // Count from -1.00 to 1.00 in 0.01 steps
  for(int16_t i = -100; i <= 100; i++){
    pix.clear();          // Clears the PIXIE chain
    pix.push(i/100.0, 2); // Writes floating-point number to two decimal places
    pix.show();           // Pushes changes to the PIXIE chain
  }

  delay(3000);         // Wait a moment
}