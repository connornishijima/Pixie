# Pixie Python driver for Raspberry Pi

This is a (mostly) complete port of version 1.2.2 of the Pixie Arduino library to a Python/SPI-based module for Raspberry Pi!

## Notable differences:

Since this is based on Hardware SPI, only the SPI pins can be used to drive the Pixies:

    PIXIE DAT = RPi MOSI
    PIXIE CLK = RPi SCLK
    (No Chip Select or MISO pins used)
    
- **pix.print()** is **pix.printout()** instead to avoid conflict with Python's **print()** function.
- The driver uses SPI at 20,000Hz by default, which can be increased or decreased in **pixie.py**
- **pixie.py** is the Pixie driver, (library equivalent) and must be in the same folder of any scripts importing it.
- This port contains all of the icons included with version 1.2.2, and newer ones may be added by request.
- Custom icons must be defined in brackets when given to a printing function. (Instead of *pix.write(127,127,127,127,127)* you'll need *pix.write(**[**127,127,127,127,127**]**)*
- There is a slightly longer-form way of fetching built-in icons. Instead of pix.write(PIX_HEART) use pix.write(pix.icons["PIX_HEART"])
- pix.flipped() (upside down display), pix.set_pix() and pix.draw_line() are not yet supported.

Other than that, usage is identical to the original library!

### SPI must be enabled using "sudo raspi-config" under "Interfacing Options" (reboot after changing)

### This library depends on the spidev library. To install it, just run "pip3 install -r requirements.txt"

#### SPI HOOKUP:
    -----------------+
                     |
     PIX_VCC     X   |
           X     X   |
           X     PIX_GND
           X     X   |
           X     X   |
           X     X   |
           X     X   |
           X     X   |
           X     X   |
     PIX_DAT     X   |
           X     X   |
     PIX_CLK     X   |
           X     X   |
           X     X   |
           X     X   |
           X     X   |
           X     X   |
           X     X   |
           X     X   |
           X     X   |
                     |
