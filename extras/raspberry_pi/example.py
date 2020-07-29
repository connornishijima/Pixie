#   Pixie for Raspberry Pi Example
#
#	UPDATE: This module now has a Python-only mode that avoids C usage.
#	This mode is slower, but should be more compatible across various platforms
#	You can experiment with this using the "transfer_mode" variable below.
#	WIRINGPI FOR PYTHON IS REQUIRED for Python mode, install it using the command:
#
#		sudo pip install wiringpi2
#
#   Steps to replicate:
#       1. Add "import pixie_write as pix" to the top of your python code
#       2. Call pix.write(DAT_pin, CLK_pin, num_pixies, *INPUT*) at any time to write to the displays!
#
#   *INPUT* can be any string or number casted to a string, such as str(123).
#   The two "pixie_write" files MUST be in the same directory as your own code!
#   
#   There are three files in this directory:
#       - "example.py"
#           This is a stand-in for your own script
#       - "pixie_write.py"
#           This script (imported below) converts your input string to a
#           binary bitstream to be blasted to the Pixies
#       - "pixie_write" (no file extension)
#           Compiled machine code that blasts the bitstream to the Pixies
#   
#   The DAT_pin and CLK_pin numbers are the WiringPi pinout, NOT the
#   BCM GPIO pinout, meaning DAT_pin is actually Pin 17, and CLK_pin is 27.
#   Wiring Pi pinout is here: http://wiringpi.com/pins/
#
#   Hookup Guide:
#
#         PI HEADER : (https://cdn.sparkfun.com/assets/learn_tutorials/4/2/4/header_pinout.jpg)
#   -----------------+
#                    |
#    PIX_VCC     X   |
#          X     X   |
#          X     PIX_GND
#          X     X   |
#          X     X   |
#    PIX_DAT     X   |
#    PIX_CLK     X   |
#          X     X   |
#          X     X   |
#          X     X   |
#          X     X   |
#          X     X   |
#          X     X   |
#          X     X   |
#          X     X   |
#          X     X   |
#          X     X   |
#          X     X   |
#          X     X   |
#          X     X   |
#                    |

import pixie_write as pix

DAT_pin = 0
CLK_pin = 2
num_pixies = 2
transfer_mode = "PYTHON" # Can be "C" or "PYTHON". "C" is faster, "PYTHON" is more compatible.

for i in range(100):
	pix.write(DAT_pin, CLK_pin, num_pixies, str( i/10.0 ), transfer_mode) #counts to from 0 to 10 in steps of 0.1
