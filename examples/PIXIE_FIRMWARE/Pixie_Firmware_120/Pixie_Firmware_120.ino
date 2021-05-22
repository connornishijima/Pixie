// -------------------------------------------------------------------------------------------
// Stock Firmware of the PIXIE driver (ATTINY45), not a Pixie Library example!
// VERSION 1.2.0
//
// Changelog:
//
// V 1.0.0:
//   Initial Release
//
// V 1.1.0:
//   Converted input polling loop to inline assembly,
//   largely increasing the max refresh rate! (39kHz -> 67kHz)
//
// V 1.2.0: (Pixie Pro!)
//   Added parity bit verification, command set, LED flip,
//   LED current selection, and reduced end_packet_timeout
//   to 1024 iterations (0.575ms). Display buffer now only
//   uses 104 bits per Pixie, instead of 128.
//
// Hardware settings:
// ATTINY45 @ 16MHz - millis()/micros() Enabled, BOD Disabled
//
// Required to compile:
// https://github.com/SpenceKonde/ATTinyCore    - ATTINY45 Support for Arduino
// https://github.com/felias-fogg/SoftI2CMaster - Bitbang 400kHz I2C using internal pullups
// -------------------------------------------------------------------------------------------

#include <avr/io.h>

#define I2C_TIMEOUT 10000
#define I2C_FASTMODE 1     // 400kHz
#define I2C_PULLUP 1       // Enable pullups to avoid more passives on board
#define I2C_NOINTERRUPT 1  // No interrupts allowed when sending I2C data
#define SDA_PORT PORTB
#define SCL_PORT PORTB
#define SDA_PIN 0          // PB0
#define SCL_PIN 2          // PB2

#include <SoftI2CMaster.h> // Include I2C Library
#define I2C_7BITADDR 0x63  // Each Pixie manages it's own display driver at a hard-coded I2C address

bool disp[104];            // Display buffer
bool updated = false;      // Have new bits been received?
uint8_t disp_index = 0;    // Index in disp[]
uint8_t pwm_val = 127;     // Brightness (127 is max)
volatile bool out_bit = 0; // Temporary storage for ASM loop

void(* reset_func) (void) = 0x00; // Declare reset function @ address 0

const uint8_t CLK_pin     = 1; // PB1
const uint8_t DAT_IN_pin  = 3; // PB3
const uint8_t DAT_OUT_pin = 4; // PB4

// These values are defaulted to the ideals for a non-Pro display.
// Marking your displays as PRO in the Pixie() library constructor will change these values to 10mA and led_flip = true, to suit the different hardware of the Pixie Pro.
uint8_t row_current = B00001110; // 35mA default
bool led_flip = false;
uint8_t pixie_brightness = 127;

// Command definitions
#define COMMAND_WRITE       0
#define COMMAND_LED_FLIP    1
#define COMMAND_ROW_CURRENT 2
#define COMMAND_RESET       3

void setup() {
  delay(10); // Not sure why this works. (Maybe unstable connections when first powered?) Please figure it out.
  pinMode(CLK_pin,     INPUT);
  pinMode(DAT_IN_pin,  INPUT);
  pinMode(DAT_OUT_pin, OUTPUT);

  i2c_init();       // Init bitbang I2C
  init_display();   // Init idsplay driver
  // Zero display buffer, if not already zeroed for some stupid reason
  for (uint8_t i = 0; i < 104; i++) {
    disp[i] = 0;
  }
  update_display(); // Blank display
}

void loop() {
  // And so begins the nasty AVR assembly. (It's my first time, be nice!)

  asm("bit_wait: \n");        // Returns here when looped
  out_bit = disp[disp_index]; // Get bit for DAT_OUT_pin on next CLK rise
  asm (
    "ldi r20,0 \n" // Reset counting registers
    "ldi r21,0 \n"

    // WAIT FOR CLK RISE
    "clk_high_wait:          \n" // Returns here when CLK is still LOW

    // This counts "iterations" since we started waiting for CLK to rise.
    // If CLK doesn't rise within 1024 iterations (about 0.575 ms) then we
    // consider our display buffer as final, and write it to the displays
    // by jumping to end_packet_timeout.
    "inc  r20                \n" // Increment r20
    "in   r16,%2             \n" // Load SREG in r16
    "sbrc r16,1              \n" // Skip next instruction if ZERO flag not set in SREG (r20 ovf)
    "inc  r21                \n" // If it was set, increment r21
    "cpi  r21,0x04           \n" // Check if r21 has reached 0x04
    "breq end_packet_timeout \n" // If so, jump to "end_packet_timeout"
    "sbis %0,1               \n" // if CLK not HIGH yet
    "rjmp clk_high_wait      \n" // loop again

    // CLK ROSE HIGH
    "lds  r16,(out_bit) \n" // load out_bit into r16
    "cpi  r16,0x01      \n" // see if the out_bit is HIGH
    "breq is_high       \n" // branch if it is
    "is_low:            \n"
    "cbi  %1,4          \n" // if it isn't, set PB2 (DAT_OUT) LOW
    "rjmp checked       \n"
    "is_high:           \n"
    "sbi  %1,4          \n" // if it is, set PB2 (DAT_OUT) HIGH

    "checked:           \n" // Skips here if out_bit was LOW

    "ldi  r20,0 \n" // reset counting registers
    "ldi  r21,0 \n"

    // WAIT FOR CLK FALL
    "clk_low_wait:   \n" // Returns here when CLK is still HIGH

    // This counts "iterations" since we started waiting for CLK to fall.
    // If CLK doesn't fall within 1024 iterations (about 0.575 ms) then we
    // enter a reset_function by jumping to reset_wait.
    "inc  r20          \n" // Increment r20
    "in   r16,%2       \n" // Load SREG in r16
    "sbrc r16,1        \n" // Skip next instruction if ZERO flag not set in SREG (r20 ovf)
    "inc  r21          \n" // If it was, increment r21
    "cpi  r21,0x04     \n" // Check if r21 has reached 0x04
    "breq reset_wait   \n" // If so, jump to "reset_wait"
    "sbic %0,1         \n" // Skip next instruction if Bit in IO is Clear (PINB/%0, bit 1)
    "rjmp clk_low_wait \n" // (If not skipped) Loop back to clk_low_wait

    // CLK FELL LOW
    "ldi  r16,0x01      \n" // Load "true" into r16
    "sts  (updated),r16 \n" // Sets updated bool to true

    "sbis %0,3          \n" // Skip next instruction if Bit in IO is Set (PINB/%0, bit 3) (DAT_IN)
    "ldi  r16,0x00      \n" // Skipped if DAT_IN pin is HIGH
    "sts  (out_bit),r16 \n" // Sets out_bit to value of DAT_IN pin

    : : "I" (_SFR_IO_ADDR(PINB)), "I" (_SFR_IO_ADDR(PORTB)), "I" (_SFR_IO_ADDR(SREG)) : "r16", "r20", "r21"
  );
  disp[disp_index] = out_bit;

  disp_index++;
  if (disp_index >= 104) {
    disp_index = 0;
  }
  asm("rjmp bit_wait \n"); // loop back until end_packet_timeout occurs

  // ------------------------------------------------------------------------
  // Reset
  asm(
    "reset_wait: \n"     // wait for CLK to fall before resetting (got here because it stayed HIGH longer than normal
    "sbic %0, 1 \n"      // Skip next instruction if Bit in IO is Clear (PINB/%0, bit 1) (CLK)
    "rjmp reset_wait \n" // (If not skipped) Loop to top of label
    : : "I" (_SFR_IO_ADDR(PINB))
  );
  // Arrive here immediately after reset_wait
  reset_func();

  // ------------------------------------------------------------------------
  // End Of Packet (timeout)
  asm("end_packet_timeout: \n"); // jumped here when timeout occured
  disp_index = 0;
  if (updated == true) {
    updated = false;
    update_display(); // Send display buffer to matrix driver
  }
  asm("rjmp bit_wait \n"); // Start looping again
}

void init_display() {
  i2c_start_wait((I2C_7BITADDR << 1) | I2C_WRITE); //  write i2c address 0x63
  i2c_write(0xFF);                  // write to reset register
  i2c_write(0x00);                  // write row data - sets dual display mode
  i2c_stop();

  i2c_start_wait((I2C_7BITADDR << 1) | I2C_WRITE); //  write i2c address 0x63
  i2c_write(0x00);                  // write to configuration register
  i2c_write(0x19);                  // write row data - sets dual display mode
  i2c_stop();

  i2c_start_wait((I2C_7BITADDR << 1) | I2C_WRITE); //  write i2c address 0x63
  i2c_write(0x0D);                  // write to LIGHTING EFFECT REGISTER
  i2c_write(row_current);           // sets row current
  i2c_stop();

  i2c_start_wait((I2C_7BITADDR << 1) | I2C_WRITE); //  write i2c address 0x63
  i2c_write(0x19);                  // write to PWM REGISTER
  i2c_write(pixie_brightness);      // sets pwm
  i2c_stop();
}

bool check_parity() { // Even parity
  uint8_t odd_ones;

  for (uint8_t i = 0; i <= 96; i += 8) {
    odd_ones = 0;
    odd_ones += disp[1 + i];
    odd_ones += disp[2 + i];
    odd_ones += disp[3 + i];
    odd_ones += disp[4 + i];
    odd_ones += disp[5 + i];
    odd_ones += disp[6 + i];
    odd_ones += disp[7 + i];
    if (bitRead(odd_ones, 0) == disp[0 + i]) {
      return false;
    }
  }

  return true;
}

void update_display() {
  if (check_parity() == true) { // If no glitches detected

    uint8_t command = 0;
    bitWrite(command, 6, disp[1]);
    bitWrite(command, 5, disp[2]);
    bitWrite(command, 4, disp[3]);
    bitWrite(command, 3, disp[4]);
    bitWrite(command, 2, disp[5]);
    bitWrite(command, 1, disp[6]);
    bitWrite(command, 0, disp[7]);

    pixie_brightness = 0;
    bitWrite(pixie_brightness, 6, disp[17]);
    bitWrite(pixie_brightness, 5, disp[18]);
    bitWrite(pixie_brightness, 4, disp[19]);
    bitWrite(pixie_brightness, 3, disp[20]);
    bitWrite(pixie_brightness, 2, disp[21]);
    bitWrite(pixie_brightness, 1, disp[22]);
    bitWrite(pixie_brightness, 0, disp[23]);

    if (command == COMMAND_WRITE) {
      i2c_start_wait((I2C_7BITADDR << 1) | I2C_WRITE); //  write i2c address 0x63
      i2c_write(0x00);                  // write to configuration register
      i2c_write(0x19);                  // write row data - sets dual display mode
      i2c_stop();

      i2c_start_wait((I2C_7BITADDR << 1) | I2C_WRITE); //  write i2c address 0x63
      i2c_write(0x19);                  // write to PWM REGISTER
      i2c_write(pixie_brightness);      // sets pwm
      i2c_stop();

      i2c_start_wait((I2C_7BITADDR << 1) | I2C_WRITE); //  write i2c
      if(led_flip){
        i2c_write(0x01);                 // write to DIGIT 1
      }
      else{
        i2c_write(0x0E);                 // write to DIGIT 1
      }

      // PICTURE DATA HERE IN COLUMN FORMAT (rotated 90deg clockwise)
      uint8_t col_out = 0;
      for (uint8_t i = 0; i < 7; i++) {
        bitWrite(col_out, 6 - i, disp[25 + i]);
      }
      i2c_write(col_out);

      for (uint8_t i = 0; i < 7; i++) {
        bitWrite(col_out, 6 - i, disp[33 + i]);
      }
      i2c_write(col_out);

      for (uint8_t i = 0; i < 7; i++) {
        bitWrite(col_out, 6 - i, disp[41 + i]);
      }
      i2c_write(col_out);

      for (uint8_t i = 0; i < 7; i++) {
        bitWrite(col_out, 6 - i, disp[49 + i]);
      }
      i2c_write(col_out);

      for (uint8_t i = 0; i < 7; i++) {
        bitWrite(col_out, 6 - i, disp[57 + i]);
      }
      i2c_write(col_out);
      // ----------------------------------

      i2c_stop();

      i2c_start_wait((I2C_7BITADDR << 1) | I2C_WRITE); //  write i2c address 0x63
      i2c_write(0x0c);                 // write to update display register
      i2c_write(0xFF);                 // write row data
      i2c_stop();

      i2c_start_wait((I2C_7BITADDR << 1) | I2C_WRITE); //  write i2c
      if(led_flip){
        i2c_write(0x0E);                 // write to DIGIT 1
      }
      else{
        i2c_write(0x01);                 // write to DIGIT 1
      }

      // PICTURE DATA HERE IN ROW FORMAT
      uint8_t col1 = 0;
      for (uint8_t i = 0; i < 7; i++) {
        bitWrite(col1, 6 - i, disp[65 + i]);
      }
      uint8_t col2 = 0;
      for (uint8_t i = 0; i < 7; i++) {
        bitWrite(col2, 6 - i, disp[73 + i]);
      }
      uint8_t col3 = 0;
      for (uint8_t i = 0; i < 7; i++) {
        bitWrite(col3, 6 - i, disp[81 + i]);
      }
      uint8_t col4 = 0;
      for (uint8_t i = 0; i < 7; i++) {
        bitWrite(col4, 6 - i, disp[89 + i]);
      }
      uint8_t col5 = 0;
      for (uint8_t i = 0; i < 7; i++) {
        bitWrite(col5, 6 - i, disp[97 + i]);
      }

      uint8_t row1 = 0;
      uint8_t row2 = 0;
      uint8_t row3 = 0;
      uint8_t row4 = 0;
      uint8_t row5 = 0;
      uint8_t row6 = 0;
      uint8_t row7 = 0;

      bitWrite(row1, 0, bitRead(col1, 0));
      bitWrite(row1, 1, bitRead(col2, 0));
      bitWrite(row1, 2, bitRead(col3, 0));
      bitWrite(row1, 3, bitRead(col4, 0));
      bitWrite(row1, 4, bitRead(col5, 0));

      bitWrite(row2, 0, bitRead(col1, 1));
      bitWrite(row2, 1, bitRead(col2, 1));
      bitWrite(row2, 2, bitRead(col3, 1));
      bitWrite(row2, 3, bitRead(col4, 1));
      bitWrite(row2, 4, bitRead(col5, 1));

      bitWrite(row3, 0, bitRead(col1, 2));
      bitWrite(row3, 1, bitRead(col2, 2));
      bitWrite(row3, 2, bitRead(col3, 2));
      bitWrite(row3, 3, bitRead(col4, 2));
      bitWrite(row3, 4, bitRead(col5, 2));

      bitWrite(row4, 0, bitRead(col1, 3));
      bitWrite(row4, 1, bitRead(col2, 3));
      bitWrite(row4, 2, bitRead(col3, 3));
      bitWrite(row4, 3, bitRead(col4, 3));
      bitWrite(row4, 4, bitRead(col5, 3));

      bitWrite(row5, 0, bitRead(col1, 4));
      bitWrite(row5, 1, bitRead(col2, 4));
      bitWrite(row5, 2, bitRead(col3, 4));
      bitWrite(row5, 3, bitRead(col4, 4));
      bitWrite(row5, 4, bitRead(col5, 4));

      bitWrite(row6, 0, bitRead(col1, 5));
      bitWrite(row6, 1, bitRead(col2, 5));
      bitWrite(row6, 2, bitRead(col3, 5));
      bitWrite(row6, 3, bitRead(col4, 5));
      bitWrite(row6, 4, bitRead(col5, 5));

      bitWrite(row7, 0, bitRead(col1, 6));
      bitWrite(row7, 1, bitRead(col2, 6));
      bitWrite(row7, 2, bitRead(col3, 6));
      bitWrite(row7, 3, bitRead(col4, 6));
      bitWrite(row7, 4, bitRead(col5, 6));

      i2c_write(row1); // mirrored on x-axis, right side up
      i2c_write(row2);
      i2c_write(row3);
      i2c_write(row4);
      i2c_write(row5);
      i2c_write(row6);
      i2c_write(row7);
      // -------------------------------

      i2c_stop();

      i2c_start_wait((I2C_7BITADDR << 1) | I2C_WRITE); //  write i2c address 0x63
      i2c_write(0x0c);                 // write to update display register
      i2c_write(0xFF);                 // write row data
      i2c_stop();
    }
    else if (command == COMMAND_ROW_CURRENT) {
      uint8_t data = 0;
      bitWrite(data, 0, disp[15]);
      bitWrite(data, 1, disp[14]);
      bitWrite(data, 2, disp[13]);
      bitWrite(data, 3, disp[12]);
      bitWrite(data, 4, disp[11]);
      bitWrite(data, 5, disp[10]);
      bitWrite(data, 6, disp[9]);

      if (data == 8) { // 5mA
        row_current = 8;
      }
      else if (data == 9) { // 10mA
        row_current = 9;
      }
      else if (data == 14) { // 35mA
        row_current = 14;
      }
      else if (data == 0) { // 40mA
        row_current = 0;
      }
      else if (data == 1) { // 45mA
        row_current = 1;
      }

      i2c_start_wait((I2C_7BITADDR << 1) | I2C_WRITE); //  write i2c address 0x63
      i2c_write(0x0D);                  // write to LIGHTING EFFECT REGISTER
      i2c_write(row_current);           // sets row current
      i2c_stop();
    }
    else if (command == COMMAND_LED_FLIP) {
      uint8_t data = 0;
      bitWrite(data, 0, disp[15]);
      bitWrite(data, 1, disp[14]);
      bitWrite(data, 2, disp[13]);
      bitWrite(data, 3, disp[12]);
      bitWrite(data, 4, disp[11]);
      bitWrite(data, 5, disp[10]);
      bitWrite(data, 6, disp[9]);
      led_flip = data;
    }
    else if (command == COMMAND_RESET) {
      reset_func();
    }
  }
}