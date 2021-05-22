/*
	Pixie WAVE Example
	-----------------------
	
	The pix.shift_byte(uint8_t column) function
	can be used to slide in custom image data one
	column at a time! pix.push_byte() moves in the
	opposite direction.
*/

#include "Pixie.h"
#define NUM_PIXIES  6                     // PCBs, not matrices
#define CLK_PIN     4                     // Any digital pin
#define DATA_PIN    5                     // Any digital pin
Pixie pix(NUM_PIXIES, CLK_PIN, DATA_PIN); // Set up display buffer

uint8_t dots[7] = {
  B01000000,
  B00100000,
  B00010000,
  B00001000,
  B00000100,
  B00000010,
  B00000001
};

void setup() {
  pix.begin(FULL_SPEED); // Init display drivers at fastest display rate
}

void loop() {
  // Push a triangle wave (stored as a diagonal line in icon columns) across the displays as fast as possible:
  for (uint8_t x = 0; x < 20; x++) {
    for (uint8_t i = 0; i < 7; i++) {
      pix.shift_byte(dots[i]);
      pix.shift_byte(dots[i]);
      pix.show();
    }
    for (uint8_t i = 0; i < 7; i++) {
      pix.shift_byte(dots[6 - i]);
      pix.shift_byte(dots[6 - i]);
      pix.show();
    }
  }

  // Push blank columns until the wave is "off screen"
  for (uint8_t i = 0; i < 55; i++) {
    pix.shift_byte(0);
    pix.shift_byte(0);
    pix.show();
  }
}