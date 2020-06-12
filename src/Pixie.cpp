/*
Pixie.cpp - Library for controlling Pixie Displays!
Created by Connor Nishijima, June 8th 2020.
Released under the GPLv3 license.
*/

#include "Pixie.h"
#include "chars.h"

Pixie::Pixie(uint8_t d_count, uint8_t c_pin, uint8_t d_pin){
	CLK_pin = c_pin;
	DAT_pin = d_pin;
	disp_count = d_count;
	display_buffer = new uint8_t[disp_count*8];  
}

void Pixie::begin(){
	pinMode(CLK_pin, OUTPUT);
	pinMode(DAT_pin, OUTPUT);
	reset();
}

void Pixie::show(){
	uint16_t byte_count = 0;
	for (uint16_t b = 0; b < disp_count * 8; b++) {
		for (uint8_t i = 0; i < 8; i++) {
			if(bitRead(display_buffer[b], 7 - i)){
				GPOS = (1 << DAT_pin);
			}
			else{
				GPOC = (1 << DAT_pin);
			}

			GPOS = (1 << CLK_pin);
			delayMicroseconds(clk_us);
			GPOC = (1 << CLK_pin);
			delayMicroseconds(clk_us);
		}
		delayMicroseconds(clk_us);
	}
	delay(5);
}

void Pixie::brightness(uint8_t b){
	b += 128;
	bright = b;
}

void Pixie::clear(){
	for (uint16_t d = 0; d < disp_count*8; d++) {
		display_buffer[d] = 0;
	}
}

void Pixie::set_pix(uint16_t x, uint16_t y, uint8_t state){
	if(x < disp_count*8 && y < 8){
		bitWrite(display_buffer[x],y,state);
	}
}

void Pixie::write(float input, uint8_t places, uint8_t pos){
	char char_buf[48];	
	sprintf(char_buf, "%.*f", places, input);
	write(char_buf, pos);
}

void Pixie::write(int32_t input, uint8_t pos){
	char char_buf[48];
	ltoa(input,char_buf,10);
	write(char_buf, pos);
}

void Pixie::write(uint32_t input, uint8_t pos){
	char char_buf[48];
	ultoa(input,char_buf,10);
	write(char_buf, pos);
}

void Pixie::write(char input, uint8_t pos){
	set_char(input, pos);
}

void Pixie::write(char* input, uint8_t pos){
	uint8_t len = strlen(input);
	if(len > disp_count-pos){
		len = disp_count-pos;
	}
	for(uint8_t i = 0; i < len; i++){
		set_char(input[i], i+pos);
	}
}

void Pixie::scroll_message(char* input, uint16_t wait_ms, bool instant){
	clear();
	uint16_t len = strlen(input);
	if(!instant){
		for(uint16_t c = 0; c < len; c++){
			char chr = input[c];
			if (chr >= 32) {
				chr -= 32;
			}
			
			push_byte(0);
			show();
			push_byte(bright);
			show();
			push_byte(0);
			show();
			push_byte(pgm_read_byte_far(col+(chr * 5 + 0)));
			show();
			push_byte(pgm_read_byte_far(col+(chr * 5 + 1)));
			show();
			push_byte(pgm_read_byte_far(col+(chr * 5 + 2)));
			show();
			push_byte(pgm_read_byte_far(col+(chr * 5 + 3)));
			show();
			push_byte(pgm_read_byte_far(col+(chr * 5 + 4)));
			show();
			
			delay(150);
		}
		for(uint8_t i = 0; i < disp_count; i++){
			for(uint8_t s = 0; s < 8; s++){
				push_byte(0);
				show();
			}
			delay(150);
		}
	}
}

void Pixie::push_byte(uint8_t col) {
	for (uint16_t i = 0; i < disp_count * 8 - 1; i++) {
		display_buffer[i] = display_buffer[i + 1];
	}
	display_buffer[disp_count * 8 - 1] = col;
}

void Pixie::push_char(char chr) {
	if (chr >= 32) {
		chr -= 32;
	}

	push_byte(0);
	push_byte(bright);
	push_byte(0);
	push_byte(pgm_read_byte_far(col+(chr * 5 + 0)));
	push_byte(pgm_read_byte_far(col+(chr * 5 + 1)));
	push_byte(pgm_read_byte_far(col+(chr * 5 + 2)));
	push_byte(pgm_read_byte_far(col+(chr * 5 + 3)));
	push_byte(pgm_read_byte_far(col+(chr * 5 + 4)));
}

void Pixie::set_char(char chr, uint8_t pos) {
	if(pos < disp_count){
		if (chr >= 32) {
			chr -= 32;
		}
		
		display_buffer[8*pos+0] = 0;
		display_buffer[8*pos+1] = bright;
		display_buffer[8*pos+2] = 0;
		
		display_buffer[8*pos+3] = pgm_read_byte_far(col+(chr * 5 + 0));
		display_buffer[8*pos+4] = pgm_read_byte_far(col+(chr * 5 + 1));
		display_buffer[8*pos+5] = pgm_read_byte_far(col+(chr * 5 + 2));
		display_buffer[8*pos+6] = pgm_read_byte_far(col+(chr * 5 + 3));
		display_buffer[8*pos+7] = pgm_read_byte_far(col+(chr * 5 + 4));
	}
}

void Pixie::set_icon(uint8_t* icon, uint8_t pos) {
	if(pos < disp_count){		
		display_buffer[8*pos+0] = 0;
		display_buffer[8*pos+1] = bright;
		display_buffer[8*pos+2] = 0;
		
		display_buffer[8*pos+3] = icon[0];
		display_buffer[8*pos+4] = icon[1];
		display_buffer[8*pos+5] = icon[2];
		display_buffer[8*pos+6] = icon[3];
		display_buffer[8*pos+7] = icon[4];
	}
}

void Pixie::reset() {
	GPOS = (1 << CLK_pin);
	delay(15);
	GPOC = (1 << CLK_pin);
	delay(10);
}

