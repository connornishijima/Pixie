/*
  Pixie.h - Library for controlling Pixie Displays!
  Created by Connor Nishijima, June 8th 2020.
  Released under the GPLv3 license.
*/

#ifndef pixie_h
#define pixie_h
#include "Arduino.h"

// FONT SELECTION ---------
	#include "font.h"
//	#include "font_alt.h"
// ------------------------

class Pixie{
  public:
    Pixie(uint8_t d_count, uint8_t c_pin, uint8_t d_pin);
    void begin();
	void flipped(bool enable);
	void show();
	void brightness(uint8_t b);
	void write_brightness(uint8_t bright, uint8_t pos);
	void clear();
	
	void write_char(char input, uint8_t pos = 0);
	void write(char input,    uint8_t pos = 0);
	void write(char* input,   uint8_t pos = 0);
	void write(int32_t input, uint8_t pos = 0);
	void write(uint32_t input, uint8_t pos = 0);
	#if defined(ESP8266) || defined(ESP32)
		void write(long unsigned int input, uint8_t pos = 0); // same as uint32_t, but Arduino is stupid
	#endif
	void write(float input, uint8_t places = 2, uint8_t pos = 0);
	void write(double input, uint8_t places = 2, uint8_t pos = 0);
	void write(uint8_t* icon, uint8_t pos = 0);
	void write(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t pos = 0);	
	void write_byte(uint8_t col, uint16_t pos);
	
	void print_char(char input);
	void print(char input);
	void print(char* input);
	void print(int32_t input);
	void print(uint32_t input);
	#if defined(ESP8266) || defined(ESP32)
		void print(long unsigned int input); // same as uint32_t, but Arduino is stupid
	#endif
	void print(float input, uint8_t places = 2);
	void print(double input, uint8_t places = 2);
	void print(uint8_t* icon);
	void print(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5);
	
	void set_cursor(uint8_t pos);

	void push_char(char chr);	
	void push_byte(uint8_t col);
	void push(char input);
	void push(char* input);
	void push(int32_t input);
	void push(uint32_t input);
	#if defined(ESP8266) || defined(ESP32)
		void push(long unsigned int input); // same as uint32_t, but Arduino is stupid
	#endif
	void push(float input, uint8_t places = 2);
	void push(double input, uint8_t places = 2);
	void push(uint8_t* icon);
	void push(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5);
	
	void shift_char(char chr);	
	void shift_byte(uint8_t col);
	void shift(char input);
	void shift(char* input);
	void shift(int32_t input);
	void shift(uint32_t input);
	#if defined(ESP8266) || defined(ESP32)
		void shift(long unsigned int input); // same as uint32_t, but Arduino is stupid
	#endif
	void shift(float input, uint8_t places = 2);
	void shift(double input, uint8_t places = 2);
	void shift(uint8_t* icon);
	void shift(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5);
	
	void draw_line(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
	
	uint8_t get_length(uint32_t input);
	#if defined(ESP8266) || defined(ESP32)
		uint8_t get_length(long unsigned int input);
	#endif
	uint8_t get_length(int32_t input);
	uint8_t get_length(float input, uint8_t places);
		
	void scroll_message(char* input, uint16_t wait_ms = 100, bool instant = false);
		
	void set_pix(uint16_t x, uint16_t y, uint8_t state);
	
	void dump_buffer();
	
	void reset();
	
  private:
	const uint8_t clk_us = 12;

	uint8_t pixie_count  = 6;
	uint8_t disp_count   = 12;
	
	uint8_t bright = 255;
	uint8_t CLK_pin;
	uint8_t DAT_pin;
	uint8_t *display_buffer;
	uint8_t cursor_pos = 0;
	
	bool display_flipped = false;
};

#endif
