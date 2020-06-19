/*
Pixie.cpp - Library for controlling Pixie Displays!
Created by Connor Nishijima, June 8th 2020.
Released under the GPLv3 license.
*/

#include "Pixie.h"
#include "chars.h"

Pixie::Pixie(uint8_t p_count, uint8_t c_pin, uint8_t d_pin){
	CLK_pin = c_pin;
	DAT_pin = d_pin;
	pixie_count = p_count;
	disp_count  = pixie_count*2;
	display_buffer = new uint8_t[disp_count*8];  
}

void Pixie::begin(){
	pinMode(CLK_pin, OUTPUT);
	pinMode(DAT_pin, OUTPUT);
	reset();
}

void Pixie::flipped(bool enable){
	display_flipped = enable;
}

void Pixie::show(){
	uint16_t byte_count = 0;
	for (uint16_t b = 0; b < disp_count * 8; b++) {
		for (uint8_t i = 0; i < 8; i++) {
			if(bitRead(display_buffer[b], 7 - i)){
				#ifdef ESP8266
					GPOS = (1 << DAT_pin);
				#endif
				#ifdef ESP32
					GPIO.out_w1ts = ((uint32_t)1 << DAT_pin);
				#endif
				
				#if !defined(ESP8266) && !defined(ESP32)
					digitalWrite(DAT_pin, HIGH);
				#endif
			}
			else{
				#ifdef ESP8266
					GPOC = (1 << DAT_pin);
				#endif
				#ifdef ESP32
					GPIO.out_w1tc = ((uint32_t)1 << DAT_pin);
				#endif
				
				#if !defined(ESP8266) && !defined(ESP32)
					digitalWrite(DAT_pin, LOW);
				#endif
			}

			#ifdef ESP8266
				GPOS = (1 << CLK_pin);
			#endif
			#ifdef ESP32
				GPIO.out_w1ts = ((uint32_t)1 << CLK_pin);
			#endif
			#if !defined(ESP8266) && !defined(ESP32)
				digitalWrite(CLK_pin, HIGH);
			#endif
			delayMicroseconds(clk_us);
			#ifdef ESP8266
				GPOC = (1 << CLK_pin);
			#endif
			#ifdef ESP32
				GPIO.out_w1tc = ((uint32_t)1 << CLK_pin);
			#endif
			#if !defined(ESP8266) && !defined(ESP32)
				digitalWrite(CLK_pin, LOW);
			#endif
			delayMicroseconds(clk_us);
		}
		delayMicroseconds(clk_us);
	}
	delay(5);
}

void Pixie::brightness(uint8_t b){
	bright = b;
	bitWrite(bright,7,1); // set bit 7 (MSB) to 1 to signify PWM byte to PIXIE
}

void Pixie::clear(){
	for (uint16_t d = 0; d < disp_count*8; d++) {
		display_buffer[d] = 0;
	}
}

void Pixie::set_pix(uint16_t x, uint16_t y, uint8_t state){
	uint16_t max_x = disp_count*8-1;
	uint8_t max_y = 6;
	
	x = max_x - x;
	y = max_y - y;
	
	if(x >= 0 && y >= 0){
		if(x < disp_count*8 && y < 8){
			bitWrite(display_buffer[x],y,state);
		}
	}
}

void Pixie::write(float input, uint8_t places, uint8_t pos){
	char char_buf[48];	
	sprintf(char_buf, "%.*f", places, input);
	write(char_buf, pos);
}

void Pixie::write(double input, uint8_t places, uint8_t pos){
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

#if defined(ESP8266) || defined(ESP32)
	void Pixie::write(long unsigned int input, uint8_t pos){
		char char_buf[48];
		ultoa(input,char_buf,10);
		write(char_buf, pos);
	}
#endif

void Pixie::write(char input, uint8_t pos){
	write_char(input, pos);
}

void Pixie::write(uint8_t* icon, uint8_t pos) {
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

void Pixie::write(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5, uint8_t pos){
	if(pos < disp_count){		
		display_buffer[8*pos+0] = 0;
		display_buffer[8*pos+1] = bright;
		display_buffer[8*pos+2] = 0;
		
		display_buffer[8*pos+3] = byte1;
		display_buffer[8*pos+4] = byte2;
		display_buffer[8*pos+5] = byte3;
		display_buffer[8*pos+6] = byte4;
		display_buffer[8*pos+7] = byte5;
	}
}

void Pixie::write(char* input, uint8_t pos){
	uint8_t len = strlen(input);
	if(len > disp_count-pos){
		len = disp_count-pos;
	}
	for(uint8_t i = 0; i < len; i++){
		write_char(input[i], i+pos);
	}
}

void Pixie::write_char(char chr, uint8_t pos) {
	
	/*
	Flip columns (whole byte)
	Flip rows (B0-B6)
	Send chars in reverse order
	*/

	if(pos < disp_count){		
		if (chr >= 32) {
			chr -= 32;
		}
		
		if(display_flipped){
			pos = disp_count-1-pos;
			
			write_byte(0,      8*pos+0);
			write_byte(bright, 8*pos+1);
			write_byte(0,      8*pos+2);
			write_byte(pgm_read_byte_far(col+(chr * 5 + 0)),  8*pos+7);
			write_byte(pgm_read_byte_far(col+(chr * 5 + 1)),  8*pos+6);
			write_byte(pgm_read_byte_far(col+(chr * 5 + 2)),  8*pos+5);
			write_byte(pgm_read_byte_far(col+(chr * 5 + 3)),  8*pos+4);
			write_byte(pgm_read_byte_far(col+(chr * 5 + 4)),  8*pos+3);
		}
		else{
			write_byte(0,      8*pos+0);
			write_byte(bright, 8*pos+1);
			write_byte(0,      8*pos+2);
			write_byte(pgm_read_byte_far(col+(chr * 5 + 0)),  8*pos+3);
			write_byte(pgm_read_byte_far(col+(chr * 5 + 1)),  8*pos+4);
			write_byte(pgm_read_byte_far(col+(chr * 5 + 2)),  8*pos+5);
			write_byte(pgm_read_byte_far(col+(chr * 5 + 3)),  8*pos+6);
			write_byte(pgm_read_byte_far(col+(chr * 5 + 4)),  8*pos+7);
		}
	}
}

void Pixie::write_byte(uint8_t col, uint16_t pos) {
	uint8_t temp_col = col;
	if(display_flipped){
		bitWrite(temp_col, 6, bitRead(col,0));
		bitWrite(temp_col, 5, bitRead(col,1));
		bitWrite(temp_col, 4, bitRead(col,2));
		bitWrite(temp_col, 3, bitRead(col,3));
		bitWrite(temp_col, 2, bitRead(col,4));
		bitWrite(temp_col, 1, bitRead(col,5));
		bitWrite(temp_col, 0, bitRead(col,6));
	}
		
	display_buffer[pos] = temp_col;
}

void Pixie::push(float input, uint8_t places){
	char char_buf[48];
	sprintf(char_buf, "%.*f", places, input);
	for(uint8_t i = 0; i < 48; i++){
		if(char_buf[i] == 0){
			break;
		}
		push_char(char_buf[i]);
	}
}

void Pixie::push(double input, uint8_t places){
	char char_buf[48];
	sprintf(char_buf, "%.*f", places, input);
	for(uint8_t i = 0; i < 48; i++){
		if(char_buf[i] == 0){
			break;
		}
		push_char(char_buf[i]);
	}
}

void Pixie::push(int32_t input){
	char char_buf[48];
	ltoa(input,char_buf,10);
	for(uint8_t i = 0; i < 48; i++){
		if(char_buf[i] == 0){
			break;
		}
		push_char(char_buf[i]);
	}
}

void Pixie::push(uint32_t input){
	char char_buf[48];
	ultoa(input,char_buf,10);
	for(uint8_t i = 0; i < 48; i++){
		if(char_buf[i] == 0){
			break;
		}
		push_char(char_buf[i]);
	}
}

#if defined(ESP8266) || defined(ESP32)
	void Pixie::push(long unsigned int input){
		char char_buf[48];
		ultoa(input,char_buf,10);
		for(uint8_t i = 0; i < 48; i++){
			if(char_buf[i] == 0){
				break;
			}
			push_char(char_buf[i]);
		}
	}
#endif

void Pixie::push(char input){
	push_char(input);
}

void Pixie::push(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5){
	if(display_flipped){
		push_byte(byte1);
		push_byte(byte2);
		push_byte(byte3);
		push_byte(byte4);
		push_byte(byte5);
		push_byte(0);
		push_byte(bright);
		push_byte(0);
	}
	else{
		push_byte(0);
		push_byte(bright);
		push_byte(0);
		push_byte(byte1);
		push_byte(byte2);
		push_byte(byte3);
		push_byte(byte4);
		push_byte(byte5);
	}
}

void Pixie::push(uint8_t* icon){
	if(display_flipped){
		push_byte(icon[0]);
		push_byte(icon[1]);
		push_byte(icon[2]);
		push_byte(icon[3]);
		push_byte(icon[4]);
		push_byte(0);
		push_byte(bright);
		push_byte(0);
	}
	else{
		push_byte(0);
		push_byte(bright);
		push_byte(0);
		push_byte(icon[0]);
		push_byte(icon[1]);
		push_byte(icon[2]);
		push_byte(icon[3]);
		push_byte(icon[4]);
	}
}

void Pixie::push(char* input){
	uint8_t len = strlen(input);
	for(uint8_t i = 0; i < len; i++){
		push_char(input[i]);
	}
}

void Pixie::push_byte(uint8_t col) {
	uint8_t temp_col = col;
	if(display_flipped){
		bitWrite(temp_col, 6, bitRead(col,0));
		bitWrite(temp_col, 5, bitRead(col,1));
		bitWrite(temp_col, 4, bitRead(col,2));
		bitWrite(temp_col, 3, bitRead(col,3));
		bitWrite(temp_col, 2, bitRead(col,4));
		bitWrite(temp_col, 1, bitRead(col,5));
		bitWrite(temp_col, 0, bitRead(col,6));
	}
	
	uint16_t len = disp_count * 8;
	
	if(display_flipped){
		for (int16_t i = len-2; i >= 0; i--) {
			display_buffer[i+1] = display_buffer[i];
		}
		display_buffer[0] = temp_col;
	}
	else{
		for (int16_t i = 0; i < len - 1; i++) {
			display_buffer[i] = display_buffer[i + 1];
		}
		display_buffer[len - 1] = temp_col;
	}
}

void Pixie::push_char(char chr) {
	if (chr >= 32) {
		chr -= 32;
	}
	
	/*
	Flip columns (whole byte)
	Flip rows (B0-B6)
	Send chars in reverse order
	*/

	
	
	if(display_flipped){
		push_byte(pgm_read_byte_far(col+(chr * 5 + 0)));
		push_byte(pgm_read_byte_far(col+(chr * 5 + 1)));
		push_byte(pgm_read_byte_far(col+(chr * 5 + 2)));
		push_byte(pgm_read_byte_far(col+(chr * 5 + 3)));
		push_byte(pgm_read_byte_far(col+(chr * 5 + 4)));
		push_byte(0);
		push_byte(bright);
		push_byte(0);
	}
	else{
		push_byte(0);
		push_byte(bright);
		push_byte(0);
		push_byte(pgm_read_byte_far(col+(chr * 5 + 0)));
		push_byte(pgm_read_byte_far(col+(chr * 5 + 1)));
		push_byte(pgm_read_byte_far(col+(chr * 5 + 2)));
		push_byte(pgm_read_byte_far(col+(chr * 5 + 3)));
		push_byte(pgm_read_byte_far(col+(chr * 5 + 4)));
	}
}

void Pixie::shift(float input, uint8_t places){
	const uint8_t len = get_length((float)input, places);
	char char_buf[len];
	sprintf(char_buf, "%.*f", places, input);
	for(uint8_t i = 0; i < len; i++){
		shift_char(char_buf[len-1-i]);
	}
}

void Pixie::shift(double input, uint8_t places){
	const uint8_t len = get_length((float)input, places);
	char char_buf[len];
	sprintf(char_buf, "%.*f", places, input);
	for(uint8_t i = 0; i < len; i++){
		shift_char(char_buf[len-1-i]);
	}
}

void Pixie::shift(int32_t input){
	const uint8_t len = get_length((int32_t)input);
	char char_buf[len];
	ltoa(input,char_buf,10);
	for(uint8_t i = 0; i < len; i++){
		shift_char(char_buf[len-1-i]);
	}
}

void Pixie::shift(uint32_t input){
	const uint8_t len = get_length(input);
	char char_buf[len];
	ultoa(input,char_buf,10);
	for(uint8_t i = 0; i < len; i++){
		shift_char(char_buf[len-1-i]);
	}
}

#if defined(ESP8266) || defined(ESP32)
	void Pixie::shift(long unsigned int input){
		const uint8_t len = get_length(input);
		char char_buf[len];
		ultoa(input,char_buf,10);
		for(uint8_t i = 0; i < len; i++){
			shift_char(char_buf[len-1-i]);
		}
	}
#endif

void Pixie::shift(char input){
	shift_char(input);
}

void Pixie::shift(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5){
	if(display_flipped){
		shift_byte(0);
		shift_byte(bright);
		shift_byte(0);
		shift_byte(byte5);
		shift_byte(byte4);
		shift_byte(byte3);
		shift_byte(byte2);
		shift_byte(byte1);
	}
	else{
		shift_byte(byte5);
		shift_byte(byte4);
		shift_byte(byte3);
		shift_byte(byte2);
		shift_byte(byte1);
		shift_byte(0);
		shift_byte(bright);
		shift_byte(0);
	}
}

void Pixie::shift(uint8_t* icon){
	if(display_flipped){
		shift_byte(0);
		shift_byte(bright);
		shift_byte(0);
		shift_byte(icon[4]);
		shift_byte(icon[3]);
		shift_byte(icon[2]);
		shift_byte(icon[1]);
		shift_byte(icon[0]);
	}
	else{
		shift_byte(icon[4]);
		shift_byte(icon[3]);
		shift_byte(icon[2]);
		shift_byte(icon[1]);
		shift_byte(icon[0]);
		shift_byte(0);
		shift_byte(bright);
		shift_byte(0);
	}
}

void Pixie::shift(char* input){
	uint8_t len = strlen(input);
	for(int16_t i = len-1; i >= 0; i--){
		shift_char(input[i]);
	}
}

void Pixie::shift_byte(uint8_t col) {
	uint8_t temp_col = col;
	if(display_flipped){
		bitWrite(temp_col, 6, bitRead(col,0));
		bitWrite(temp_col, 5, bitRead(col,1));
		bitWrite(temp_col, 4, bitRead(col,2));
		bitWrite(temp_col, 3, bitRead(col,3));
		bitWrite(temp_col, 2, bitRead(col,4));
		bitWrite(temp_col, 1, bitRead(col,5));
		bitWrite(temp_col, 0, bitRead(col,6));
	}
	
	uint16_t len = disp_count * 8;
	
	if(display_flipped){
		for (uint16_t i = 0; i < len-1; i++) {
			display_buffer[i] = display_buffer[i+1];
		}
		display_buffer[len-1] = temp_col;
	}
	else{
		for (uint16_t i = len-1; i > 0; i--) {
			display_buffer[i] = display_buffer[i - 1];
		}
		display_buffer[0] = temp_col;
	}
}

void Pixie::shift_char(char chr) {
	if (chr >= 32) {
		chr -= 32;
	}

	if(display_flipped){
		shift_byte(0);
		shift_byte(bright);
		shift_byte(0);
		shift_byte(pgm_read_byte_far(col+(chr * 5 + 4)));
		shift_byte(pgm_read_byte_far(col+(chr * 5 + 3)));
		shift_byte(pgm_read_byte_far(col+(chr * 5 + 2)));
		shift_byte(pgm_read_byte_far(col+(chr * 5 + 1)));
		shift_byte(pgm_read_byte_far(col+(chr * 5 + 0)));
	}
	else{
		shift_byte(pgm_read_byte_far(col+(chr * 5 + 4)));
		shift_byte(pgm_read_byte_far(col+(chr * 5 + 3)));
		shift_byte(pgm_read_byte_far(col+(chr * 5 + 2)));
		shift_byte(pgm_read_byte_far(col+(chr * 5 + 1)));
		shift_byte(pgm_read_byte_far(col+(chr * 5 + 0)));
		shift_byte(0);
		shift_byte(bright);
		shift_byte(0);
	}
}

void Pixie::draw_line(int16_t x1, int16_t y1, int16_t x2, int16_t y2){
	//Bresenham's line algorithm
	uint8_t c = 1;
	
	int16_t x,y,dx,dy,dx1,dy1,px,py,xe,ye,i;
	dx=x2-x1;
	dy=y2-y1;
	dx1=fabs(dx);
	dy1=fabs(dy);
	px=2*dy1-dx1;
	py=2*dx1-dy1;
	if(dy1<=dx1){
		if(dx>=0){
			x=x1;
			y=y1;
			xe=x2;
		}
		else{
			x=x2;
			y=y2;
			xe=x1;
		}
		set_pix(x,y,c);
		for(i=0;x<xe;i++){
			x=x+1;
			if(px<0){
				px=px+2*dy1;
			}
			else{
				if((dx<0 && dy<0) || (dx>0 && dy>0)){
					y=y+1;
				}
				else{
					y=y-1;
				}
				px=px+2*(dy1-dx1);
			}
			delay(0);
			set_pix(x,y,c);
		}
	}
	else{
		if(dy>=0){
			x=x1;
			y=y1;
			ye=y2;
		}
		else{
			x=x2;
			y=y2;
			ye=y1;
		}
		set_pix(x,y,c);
		for(i=0;y<ye;i++){
			y=y+1;
			if(py<=0){
				py=py+2*dx1;
			}
			else{
				if((dx<0 && dy<0) || (dx>0 && dy>0)){
					x=x+1;
				}
				else{
					x=x-1;
				}
				py=py+2*(dx1-dy1);
			}
			set_pix(x,y,c);
		}
	}
}

uint8_t Pixie::get_length(float input, uint8_t dec){
	if(input == 0){
		return 2+dec;
	}
	uint8_t places = 1 + dec;
	if(input < 0){
		places++; // signed
	}
	int32_t input_whole = input;
	while(abs(input) >= 1){
		input /= 10;
		places++;
	}
	return places;
}

uint8_t Pixie::get_length(int32_t input){
	if(input == 0){
		return 1;
	}
	uint8_t places = 0;
	if(input < 0){
		places++; // signed
	}
	while(abs(input) >= 1){
		input /= 10;
		places++;
	}
	return places;
}

uint8_t Pixie::get_length(uint32_t input){
	if(input == 0){
		return 1;
	}
	uint8_t places = 0;
	while(abs(input) >= 1){
		input /= 10;
		places++;
	}
	return places;
}

#if defined(ESP8266) || defined(ESP32)
	uint8_t Pixie::get_length(long unsigned int input){
		if(input == 0){
			return 1;
		}
		uint8_t places = 0;
		while(abs(input) >= 1){
			input /= 10;
			places++;
		}
		return places;
	}
#endif

void Pixie::scroll_message(char* input, uint16_t wait_ms, bool instant){
	clear();
	uint16_t len = strlen(input);
	if(!instant){
		if(display_flipped){
			for(uint16_t c = 0; c < len; c++){
				char chr = input[c];
				if (chr >= 32) {
					chr -= 32;
				}			
				
				push_byte(pgm_read_byte_far(col+(chr * 5 + 0)));
				show();
				delay(2);
				push_byte(pgm_read_byte_far(col+(chr * 5 + 1)));
				show();
				delay(2);
				push_byte(pgm_read_byte_far(col+(chr * 5 + 2)));
				show();
				delay(2);
				push_byte(pgm_read_byte_far(col+(chr * 5 + 3)));
				show();
				delay(2);
				push_byte(pgm_read_byte_far(col+(chr * 5 + 4)));
				show();
				delay(2);
				push_byte(0);
				show();
				delay(2);
				push_byte(bright);
				show();
				delay(2);
				push_byte(0);
				show();
				delay(2);
				
				delay(wait_ms);
			}
			for(uint8_t i = 0; i < disp_count; i++){
				push_byte(0);
				show();
				delay(2);
				push_byte(0);
				show();
				delay(2);
				push_byte(0);
				show();
				delay(2);
				push_byte(0);
				show();
				delay(2);
				push_byte(0);
				show();
				delay(2);
				push_byte(0);
				show();
				delay(2);
				push_byte(bright);
				show();
				delay(2);
				push_byte(0);
				show();
				delay(2);
				
				delay(wait_ms);
			}
		}
		else{
			for(uint16_t c = 0; c < len; c++){
				char chr = input[c];
				if (chr >= 32) {
					chr -= 32;
				}			
				
				push_byte(0);
				show();
				delay(2);
				push_byte(bright);
				show();
				delay(2);
				push_byte(0);
				show();
				delay(2);
				push_byte(pgm_read_byte_far(col+(chr * 5 + 0)));
				show();
				delay(2);
				push_byte(pgm_read_byte_far(col+(chr * 5 + 1)));
				show();
				delay(2);
				push_byte(pgm_read_byte_far(col+(chr * 5 + 2)));
				show();
				delay(2);
				push_byte(pgm_read_byte_far(col+(chr * 5 + 3)));
				show();
				delay(2);
				push_byte(pgm_read_byte_far(col+(chr * 5 + 4)));
				show();
				delay(2);
				
				delay(wait_ms);
			}
			for(uint8_t i = 0; i < disp_count; i++){
				push_byte(0);
				show();
				delay(2);
				push_byte(bright);
				show();
				delay(2);
				push_byte(0);
				show();
				delay(2);
				push_byte(0);
				show();
				delay(2);
				push_byte(0);
				show();
				delay(2);
				push_byte(0);
				show();
				delay(2);
				push_byte(0);
				show();
				delay(2);
				push_byte(0);
				show();
				delay(2);
				
				delay(wait_ms);
			}
		}
	}
	else{
		if(display_flipped){
			for(uint16_t c = 0; c < len; c++){
				char chr = input[c];
				if (chr >= 32) {
					chr -= 32;
				}
				
				push_byte(pgm_read_byte_far(col+(chr * 5 + 0)));
				push_byte(pgm_read_byte_far(col+(chr * 5 + 1)));
				push_byte(pgm_read_byte_far(col+(chr * 5 + 2)));
				push_byte(pgm_read_byte_far(col+(chr * 5 + 3)));
				push_byte(pgm_read_byte_far(col+(chr * 5 + 4)));
				push_byte(0);
				push_byte(bright);
				push_byte(0);
				show();			
				delay(wait_ms);
			}
			for(uint8_t i = 0; i < disp_count; i++){
				push_byte(0);
				push_byte(0);
				push_byte(0);
				push_byte(0);
				push_byte(0);
				push_byte(0);
				push_byte(bright);
				push_byte(0);
				show();			
				delay(wait_ms);
			}
		}
		else{
			for(uint16_t c = 0; c < len; c++){
				char chr = input[c];
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
				show();			
				delay(wait_ms);
			}
			for(uint8_t i = 0; i < disp_count; i++){
				push_byte(0);
				push_byte(bright);
				push_byte(0);
				push_byte(0);
				push_byte(0);
				push_byte(0);
				push_byte(0);
				push_byte(0);
				show();			
				delay(wait_ms);
			}
		}
	}
}

void Pixie::dump_buffer(){
	for (uint16_t b = 0; b < disp_count * 8; b++) {
		for (uint8_t i = 0; i < 8; i++) {
			if(bitRead(display_buffer[b], 7 - i)){
				Serial.print(1);
			}
			else{
				Serial.print(0);
			}
		}
	}
	Serial.println("\n");
}

void Pixie::reset() {
	#ifdef ESP8266
		GPOS = (1 << CLK_pin);
	#endif
	#ifdef ESP32
		GPIO.out_w1ts = ((uint32_t)1 << CLK_pin);
	#endif
	#if !defined(ESP8266) && !defined(ESP32)
		digitalWrite(CLK_pin, HIGH);
	#endif
	delay(15);
	#ifdef ESP8266
		GPOC = (1 << CLK_pin);
	#endif
	#ifdef ESP32
		GPIO.out_w1tc = ((uint32_t)1 << CLK_pin);
	#endif
	#if !defined(ESP8266) && !defined(ESP32)
		digitalWrite(CLK_pin, LOW);
	#endif
	delay(10);
}

// function for line generation 
void bresenham(int16_t x1, int16_t y1, int16_t x2, int16_t y2){ 
	
} 