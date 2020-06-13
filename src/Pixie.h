/*
Pixie.h - Library for controlling Pixie Displays!
Created by Connor Nishijima, June 8th 2020.
Released under the GPLv3 license.
*/

#ifndef pixie_h
#define pixie_h
#include "Arduino.h"

class Pixie{
  public:
    Pixie(uint8_t d_count, uint8_t c_pin, uint8_t d_pin);
    void begin();
	void show();
	void brightness(uint8_t b);
	void clear();
	void push_byte(uint8_t col);
	void push_char(char chr);
	void set_char(char chr, uint8_t pos);
	void set_icon(uint8_t* icon, uint8_t pos);
	void set_icon(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t pos);
	void push_icon(uint8_t* icon);
	void push_icon(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5);
	
	void write(char input,    uint8_t pos = 0);
	void write(char* input,   uint8_t pos = 0);
	void write(int32_t input, uint8_t pos = 0);
	void write(uint32_t input, uint8_t pos = 0);
	void write(float input, uint8_t places = 2, uint8_t pos = 0);
	
	void push(char input);
	void push(char* input);
	void push(int32_t input);
	void push(uint32_t input);
	void push(float input, uint8_t places = 2);
	
	void scroll_message(char* input, uint16_t wait_ms = 150, bool instant = false);
		
	void set_pix(uint16_t x, uint16_t y, uint8_t state);
	
	void reset();
	
  private:
	uint8_t disp_count = 12;
	const uint8_t clk_us = 12;
	uint8_t bright = 255;
	uint8_t CLK_pin;
	uint8_t DAT_pin;
	uint8_t *display_buffer;
};

#endif
