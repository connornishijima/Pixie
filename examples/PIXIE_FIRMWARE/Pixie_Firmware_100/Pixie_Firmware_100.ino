// ----------------------------------------------------------------
// Stock Firmware of the PIXIE driver, not a Pixie Library example!
//
// VERSION 1.0.0
//
// ATTINY45 @ 16MHz - millis()/micros() Enabled, BOD Disabled
// Requires https://github.com/felias-fogg/SoftI2CMaster
// ----------------------------------------------------------------

#define I2C_TIMEOUT 10000
#define I2C_FASTMODE 1
#define I2C_PULLUP 1
#define I2C_NOINTERRUPT 1
#define SDA_PORT PORTB
#define SDA_PIN 0
#define SCL_PORT PORTB
#define SCL_PIN 2

#include <SoftI2CMaster.h>
#define I2C_7BITADDR 0x63

#define BIT(x) (0x01 << (x))
#define bit_get(p,m) ((p) & (m)) // Get the value of a bit, like bitGet(PORTB, BIT(5));
#define bit_set(p,m) ((p) |= (m)) // Set the value of a bit (set it to 1), like bitSet(PORTB, BIT(2));
#define bit_clear(p,m) ((p) &= ~(m)) // Clear a bit (set it to 0), like bitClear(PORTB, BIT(2));

#define CLK      1
#define DAT_IN   3
#define DAT_OUT  4

bool updated = false;
uint32_t last_bit = 0;
uint32_t high_start = 0;
bool is_high = false;
uint8_t disp_index = 0;
bool clk_state_last = LOW;
uint8_t pwm_val = 127;

uint8_t i2c_wait = 2;

uint8_t disp[128];

void(* reset_func) (void) = 0x00; //declare reset function @ address 0

// B7   0 1 1 1 0
// B6   1 0 0 0 1
// B5   1 0 0 1 1
// B4   1 0 1 0 1
// B3   1 1 0 0 1
// B2   1 0 0 0 1
// B1   0 1 1 1 0
// B0   - - - - -

//   C  1 2 3 4 5

void setup() {
  pinMode(CLK,     INPUT);
  pinMode(DAT_IN,  INPUT);
  pinMode(DAT_OUT, OUTPUT);

  i2c_init();
  for (uint8_t i = 0; i < 128; i++) {
    disp[i] = 0;
  }
  init_display();
  update_display();

  //show_test();
}

void loop() {
  bool clk_state = PINB & B00000010;    //isolate value of PB1 - CLK
  if (clk_state != clk_state_last) {
    if (clk_state == HIGH) {
      bool out_bit = disp[disp_index];
      if (out_bit == 1) {
        bit_set(PORTB, BIT(DAT_OUT));
      }
      else {
        bit_clear(PORTB, BIT(DAT_OUT));
      }
      high_start = millis();
    }
    else {
      delayMicroseconds(1);
      bool dat_in_state = PINB & B00001000;    //isolate value of PB3 - DAT_IN
      disp[disp_index] = dat_in_state;
      last_bit = millis();
      is_high = false;

      disp_index++;
      if (disp_index >= 128) {
        updated = true;
        disp_index -= 128;
      }

      if (millis() - high_start >= 10) {
        reset_func();
      }
    }
  }
  clk_state_last = clk_state;

  if (updated) {
    uint32_t t_now = millis();
    if (t_now - last_bit >= 3) {
      updated = false;
      last_bit = t_now;
      update_display();
    }
  }
}

void init_display() {
  i2c_start_wait((I2C_7BITADDR << 1) | I2C_WRITE); //  write i2c address 0x63
  i2c_write(0xFF);                  // write to reset register
  i2c_write(0x00);                  // write row data - sets dual display mode
  i2c_stop();
  delayMicroseconds(i2c_wait);

  //for (uint8_t i = 0; i < 2; i++) {
  i2c_start_wait((I2C_7BITADDR << 1) | I2C_WRITE); //  write i2c address 0x63
  i2c_write(0x00);                  // write to configuration register
  i2c_write(0x19);                  // write row data - sets dual display mode
  i2c_stop();
  delayMicroseconds(i2c_wait);

  i2c_start_wait((I2C_7BITADDR << 1) | I2C_WRITE); //  write i2c address 0x63
  i2c_write(0x0D);                  // write to LIGHTING EFFECT REGISTER
  i2c_write(B00000000);             // sets row current to 40mA
  i2c_stop();
  delayMicroseconds(i2c_wait);

  i2c_start_wait((I2C_7BITADDR << 1) | I2C_WRITE); //  write i2c address 0x63
  i2c_write(0x19);                  // write to PWM REGISTER
  i2c_write(B10000000);             // sets pwm to 128 (max)
  i2c_stop();
  delayMicroseconds(i2c_wait);


}

void update_display() {
  cli();
  disp_index = 0;

  //for (uint8_t i = 0; i < 2; i++) {
  i2c_start_wait((I2C_7BITADDR << 1) | I2C_WRITE); //  write i2c address 0x63
  i2c_write(0x00);                  // write to configuration register
  i2c_write(0x19);                  // write row data - sets dual display mode
  i2c_stop();
  delayMicroseconds(i2c_wait);

  i2c_start_wait((I2C_7BITADDR << 1) | I2C_WRITE); //  write i2c address 0x63
  i2c_write(0x0D);                  // write to LIGHTING EFFECT REGISTER
  i2c_write(B00000000);             // sets row current to 40mA
  i2c_stop();
  delayMicroseconds(i2c_wait);

  if (disp[8] == 1) { // PWM BIT in PWM COLUMN
    for (uint8_t i = 0; i < 8; i++) {
      bitWrite(pwm_val, 7 - i, disp[8 + i]);
      bitWrite(pwm_val, 7, 0); // pwm_val - 128
    }
    i2c_start_wait((I2C_7BITADDR << 1) | I2C_WRITE); //  write i2c address 0x63
    i2c_write(0x19);                  // write to PWM REGISTER
    i2c_write(pwm_val);               // sets pwm to pwm_val
    i2c_stop();
    delayMicroseconds(i2c_wait);
  }

  //for (uint8_t i = 0; i < 5; i++) {
  i2c_start_wait((I2C_7BITADDR << 1) | I2C_WRITE); //  write i2c
  i2c_write(0x0E);                 // write to DIGIT 1

  for (uint8_t i = 0; i < 5; i++) {
    uint8_t out_byte = 0;
    for (uint8_t b = 0; b < 8; b++) {
      bitWrite(out_byte, (7 - b), disp[(i * 8) + b + 24]);
    }
    if(out_byte >= 128){
      out_byte = 0;
    }
    i2c_write(out_byte);
  }

  i2c_stop();
  delayMicroseconds(i2c_wait);

  i2c_start_wait((I2C_7BITADDR << 1) | I2C_WRITE); //  write i2c address 0x63
  i2c_write(0x0c);                 // write to update display register
  i2c_write(0xFF);                 // write row data
  i2c_stop();
  delayMicroseconds(i2c_wait);

  //---------------------

  uint8_t row[8];

  for (uint8_t rowindex = 0; rowindex < 8; rowindex++) {
    row[rowindex] = 0;
    bitWrite(row[rowindex],0, disp[88 + (7 - rowindex)]);
    bitWrite(row[rowindex],1, disp[96 + (7 - rowindex)]);
    bitWrite(row[rowindex],2, disp[104 + (7 - rowindex)]);
    bitWrite(row[rowindex],3, disp[112 + (7 - rowindex)]);
    bitWrite(row[rowindex],4, disp[120 + (7 - rowindex)]);
  }
  for(uint8_t b = 0; b < 5; b++){
    if(bitRead(row[7],b) == 1){

      bitWrite(row[0],b,0);
      bitWrite(row[1],b,0);
      bitWrite(row[2],b,0);
      bitWrite(row[3],b,0);
      bitWrite(row[4],b,0);
      bitWrite(row[5],b,0);
      bitWrite(row[6],b,0);
    }
  }

  i2c_start_wait((I2C_7BITADDR << 1) | I2C_WRITE); //  write i2c
  i2c_write(0x01);                 // write to DIGIT 2

  for (uint8_t i = 0; i < 7; i++) {
    i2c_write(row[i]);
  }

  i2c_stop();
  delayMicroseconds(i2c_wait);

  i2c_start_wait((I2C_7BITADDR << 1) | I2C_WRITE); //  write i2c address 0x63
  i2c_write(0x0c);                 // write to update display register
  i2c_write(0xFF);                 // write row data
  i2c_stop();
  delayMicroseconds(i2c_wait);

  //}
  sei();

  disp_index = 0;
}

void show_test() {
  for (uint8_t i = 0; i < 128; i++) {
    for (uint8_t b = 0; b < 128; b++) {
      disp[b] = 0;
    }
    disp[i] = 1;
    update_display();
    delayMicroseconds(500);
  }
  disp[127] = 0;
  update_display();
  delay(500);
}