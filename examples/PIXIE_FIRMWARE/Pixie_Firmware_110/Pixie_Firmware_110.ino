// -------------------------------------------------------------------------------------------
// Stock Firmware of the PIXIE driver (ATTINY45), not a Pixie Library example!
// VERSION 1.1.0
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
// Hardware settings:
// ATTINY45 @ 16MHz - millis()/micros() Enabled, BOD Disabled
//
// Required to compile:
// https://github.com/SpenceKonde/ATTinyCore    - ATTINY45 Support for Arduino
// https://github.com/felias-fogg/SoftI2CMaster - Bitbang 400kHz I2C using internal pullups
// -------------------------------------------------------------------------------------------

#include <avr/io.h>

#define I2C_TIMEOUT 10000
#define I2C_FASTMODE 1    // 400kHz
#define I2C_PULLUP 1      // Enable pullups to avoid more passives on board
#define I2C_NOINTERRUPT 1 // No interrupts allowed when sending I2C data
#define SDA_PORT PORTB
#define SCL_PORT PORTB
#define SDA_PIN 0         // PB0
#define SCL_PIN 2         // PB2

#include <SoftI2CMaster.h> // Include I2C Library
#define I2C_7BITADDR 0x63  // Each Pixie manages it's own display driver at a hard-coded I2C address

bool disp[128];            // Display buffer
bool updated = false;      // Have new bits been received?
uint8_t disp_index = 0;    // Index in disp[]
uint8_t pwm_val = 127;     // Brightness (127 is max)
volatile bool out_bit = 0; // Temporary storage for ASM loop

void(* reset_func) (void) = 0x00; // Declare reset function @ address 0

const uint8_t CLK_pin     = 1; //PB1
const uint8_t DAT_IN_pin  = 3; //PB3
const uint8_t DAT_OUT_pin = 4; //PB4

void setup() {
  delay(10); // Not sure why this works. (Maybe unstable connections when first powered?) Please figure it out.
  pinMode(CLK_pin,     INPUT);
  pinMode(DAT_IN_pin,  INPUT);
  pinMode(DAT_OUT_pin, OUTPUT);

  i2c_init();       // Init bitbang I2C
  init_display();   // Init idsplay driver
  // Zero display buffer, if not already zeroed for some stupid reason
  for (uint8_t i = 0; i < 128; i++) {
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
	// If CLK doesn't rise within 4096 iterations (about 2.3 ms) then we
	// consider our display buffer as final, and write it to the displays
	// by jumping to end_packet_timeout.
    "inc  r20                \n" // Increment r20
    "in   r16,%2             \n" // Load SREG in r16
    "sbrc r16,1              \n" // Skip next instruction if ZERO flag not set in SREG (r20 ovf)
    "inc  r21                \n" // If it was set, increment r21
    "cpi  r21,0x10           \n" // Check if r21 has reached 0x01
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
	// If CLK doesn't fall within 4096 iterations (about 2.3 ms) then we
	// enter a reset_function by jumping to reset_wait.
    "inc  r20          \n" // Increment r20
    "in   r16,%2       \n" // Load SREG in r16
    "sbrc r16,1        \n" // Skip next instruction if ZERO flag not set in SREG (r20 ovf)
    "inc  r21          \n" // If it was, increment r21
    "cpi  r21,0x10     \n" // Check if r21 has reached 0x01
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
  if (disp_index >= 128) {
    disp_index = 0;
  }
  asm("rjmp bit_wait \n"); // loop back until end_packet_timeout occurs
  
  // ------------------------------------------------------------------------
  // Reset
  asm(
    "reset_wait: \n"       // wait for CLK to fall before resetting (got here because it stayed HIGH longer than normal
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
  if(updated == true){
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

  //for (uint8_t i = 0; i < 2; i++) {
  i2c_start_wait((I2C_7BITADDR << 1) | I2C_WRITE); //  write i2c address 0x63
  i2c_write(0x00);                  // write to configuration register
  i2c_write(0x19);                  // write row data - sets dual display mode
  i2c_stop();

  i2c_start_wait((I2C_7BITADDR << 1) | I2C_WRITE); //  write i2c address 0x63
  i2c_write(0x0D);                  // write to LIGHTING EFFECT REGISTER
  i2c_write(B00000000);             // sets row current to 40mA
  i2c_stop();

  i2c_start_wait((I2C_7BITADDR << 1) | I2C_WRITE); //  write i2c address 0x63
  i2c_write(0x19);                  // write to PWM REGISTER
  i2c_write(B10000000);             // sets pwm to 128 (max)
  i2c_stop();


}

void update_display() {
  i2c_start_wait((I2C_7BITADDR << 1) | I2C_WRITE); //  write i2c address 0x63
  i2c_write(0x00);                  // write to configuration register
  i2c_write(0x19);                  // write row data - sets dual display mode
  i2c_stop();

  i2c_start_wait((I2C_7BITADDR << 1) | I2C_WRITE); //  write i2c address 0x63
  i2c_write(0x0D);                  // write to LIGHTING EFFECT REGISTER
  i2c_write(B00000000);             // sets row current to 40mA
  i2c_stop();

  if (disp[8] == 1) { // PWM BIT in PWM COLUMN
    for (uint8_t i = 0; i < 8; i++) {
      bitWrite(pwm_val, 7 - i, disp[8 + i]);
      bitWrite(pwm_val, 7, 0); // pwm_val - 128
    }
    i2c_start_wait((I2C_7BITADDR << 1) | I2C_WRITE); //  write i2c address 0x63
    i2c_write(0x19);                  // write to PWM REGISTER
    i2c_write(pwm_val);               // sets pwm to pwm_val
    i2c_stop();
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

  i2c_start_wait((I2C_7BITADDR << 1) | I2C_WRITE); //  write i2c address 0x63
  i2c_write(0x0c);                 // write to update display register
  i2c_write(0xFF);                 // write row data
  i2c_stop();

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

  i2c_start_wait((I2C_7BITADDR << 1) | I2C_WRITE); //  write i2c address 0x63
  i2c_write(0x0c);                 // write to update display register
  i2c_write(0xFF);                 // write row data
  i2c_stop();
}