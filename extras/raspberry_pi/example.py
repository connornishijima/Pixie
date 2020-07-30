#   Pixie for Raspberry Pi Example
#
#   Steps to replicate:
#       1. Add "import pixie_write as pix" to the top of your python code
#       2. Call pix.write(num_pixies, *INPUT*) at any time to write to the displays!
#
#   *INPUT* can be any string or number casted to a string, such as str(123).
#   The "pixie_write.py" file MUST be in the same directory as your own code!
#
#   There are two files in this directory:
#       - "example.py"
#           This is a stand-in for your own script
#       - "pixie_write.py"
#           This script (imported below) converts your input string to a
#           binary bitstream to be blasted to the Pixies
#
#   The RasPi Pixie driver uses the SPI pins marked below, and
#   needs SPI enabled using:
#
#     sudo raspi-config
#
#   ...and navigating to Interfacing -> SPI to enable it (and reboot)
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
#          X     X   |
#          X     X   |
#          X     X   |
#          X     X   |
#    PIX_DAT     X   |
#          X     X   |
#    PIX_CLK     X   |
#          X     X   |
#          X     X   |
#          X     X   |
#          X     X   |
#          X     X   |
#          X     X   |
#          X     X   |
#          X     X   |
#                    |

import time
import pixie_write as pix
num_pixies = 2

# Write string, wait 3 seconds
pix.write(num_pixies, "TEST")
time.sleep(3)

# Counts to from 0 to 99.9 in steps of 0.1
for i in range(1000):
	pix.write(num_pixies, str( i/10.0 ))
