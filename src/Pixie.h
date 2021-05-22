/*!
 * @file Pixie.h
 *
 * Library for controlling Pixie Displays!
 * Created by Connor Nishijima, June 8th 2020.
 * Released under the GPLv3 license, all text here must be included in any redistribution.
 */

#ifndef pixie_h
#define pixie_h
#include "Arduino.h"

// FONT SELECTION ---------
// - There are two built-in fonts to choose from.
//   Uncomment a single option below to choose.
//   (font_alt has fancy numerals)

	#include "font.h"
//	#include "font_alt.h"
// ------------------------

#define LEGACY 0  // Original Pixies with 128 bit buffer
#define PRO    1  // Pixie Pro with 104-bit buffer

#define LEGACY_SPEED 12  // ~39kHz bitrate
#define FULL_SPEED   7   // ~67kHz bitrate (Firmware 1.1.0+)

#define PIX_WRITE       0
#define PIX_LED_FLIP    1
#define PIX_ROW_CURRENT 2
#define PIX_RESET 	3

#define mA_5   8
#define mA_10  9
#define mA_35 14
#define mA_40  0
#define mA_45  1

/**************************************************************************/
/*! 
    @brief	Pixie class, containing all of the coolest functions!
*/
/**************************************************************************/
class Pixie{
  public:
    Pixie(uint8_t d_count, uint8_t c_pin, uint8_t d_pin, uint8_t p_type = LEGACY);
        void begin(uint8_t speed = LEGACY_SPEED); // Defaults to LEGACY_SPEED
	void show(bool fill_com = true);
	void brightness(uint8_t b);
	void write_brightness(uint8_t bright, uint8_t pos);
	void clear();
	
	void command(uint8_t com, uint8_t data = 0);
	
	void write_char(char input, uint8_t pos = 0);
	void write(char     input, uint8_t pos = 0);
	void write(char*    input, uint8_t pos = 0);
	void write(int16_t  input, uint8_t pos = 0);
	void write(uint16_t input, uint8_t pos = 0);
	void write(int32_t  input, uint8_t pos = 0);
	void write(uint32_t input, uint8_t pos = 0);
	#if defined(ESP8266) || defined(ESP32)
		void write(long unsigned int input, uint8_t pos = 0); // same as uint32_t, but Arduino is stupid
	#endif
	void write(float  input, uint8_t places = 2, uint8_t pos = 0);
	void write(double input, uint8_t places = 2, uint8_t pos = 0);
	void write(uint8_t* icon, uint8_t pos = 0);
	void write(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t pos = 0);	
	void write_byte(uint8_t col, uint16_t pos);
	
	void print_char(char input);
	void print(char     input);
	void print(char*    input);
	void print(int16_t  input);
	void print(uint16_t input);
	void print(int32_t  input);
	void print(uint32_t input);
	#if defined(ESP8266) || defined(ESP32)
		void print(long unsigned int input); // same as uint32_t, but Arduino is stupid
	#endif
	void print(float  input, uint8_t places = 2);
	void print(double input, uint8_t places = 2);
	void print(uint8_t* icon);
	void print(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5);
	
	void set_cursor(uint8_t pos);

	void push_char(char chr);	
	void push_byte(uint8_t col);
	void push(char     input);
	void push(char*    input);
	void push(int16_t  input);
	void push(uint16_t input);
	void push(int32_t  input);
	void push(uint32_t input);
	#if defined(ESP8266) || defined(ESP32)
		void push(long unsigned int input); // same as uint32_t, but Arduino is stupid
	#endif
	void push(float  input, uint8_t places = 2);
	void push(double input, uint8_t places = 2);
	void push(uint8_t* icon);
	void push(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5);
	
	void shift_char(char chr);	
	void shift_byte(uint8_t col);
	void shift(char     input);
	void shift(char*    input);
	void shift(int16_t  input);
	void shift(uint16_t input);
	void shift(int32_t  input);
	void shift(uint32_t input);
	#if defined(ESP8266) || defined(ESP32)
		void shift(long unsigned int input); // same as uint32_t, but Arduino is stupid
	#endif
	void shift(float  input, uint8_t places = 2);
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
	void scroll(char* input);
	void smooth_scroll(char* input);
		
	void set_pix(uint16_t x, uint16_t y, uint8_t state);
	
	void dump_buffer();
	
	void reset();
	
  private:
	void calc_parity();
	void fill_commands();
	uint8_t clk_us = LEGACY_SPEED;

	uint8_t pix_type = LEGACY;
	uint8_t pixie_count  = 0;
	uint8_t disp_count   = 0;
	
	uint8_t bright = 255;
	uint8_t CLK_pin;
	uint8_t DAT_pin;
	uint8_t *display_buffer;
	uint8_t cursor_pos = 0;
	bool push_flip = false;
	
	bool display_flipped = false;
};

#endif
