# Pixie RasPi driver demo!
# SPI must be enabled using "sudo raspi-config" under "Interfacing Options" (reboot after changing)

# SPI HOOKUP:
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

import pixie as pix # Import Pixie library
import time         # Optional timekeeping module

NUM_PIXIES = 3
pix.begin(NUM_PIXIES) # Init displays

# Count to 100
for i in range(100):
	pix.clear()
	pix.write(i)
	pix.show()

# Scroll message
pix.scroll_message("Scrolling message!")

# Scroll message without stopping
pix.scroll_message("Scrolling message!", 0)

# Scroll message without animation
pix.scroll_message("Scrolling message!", 250, True)

# Push input to left-side of display
pix.shift("PUSH")
pix.show()
time.sleep(1)

# Push input to right-side of display
pix.push("X")
pix.show()
time.sleep(1)

# Push input to arbitrary display
pix.write("Y",1)
pix.show()
time.sleep(1)

# Set brightness
pix.clear()
pix.brightness(8) # 0-127
pix.write("LOW")
pix.show()

time.sleep(1)

pix.clear()
pix.brightness(127) # 0-127
pix.write("HIGH")
pix.show()

time.sleep(1)

# Built in icons:
pix.clear()
pix.printout("L")
pix.printout(pix.icons["PIX_HEART"])
pix.printout("L")
pix.show()
time.sleep(2)

# Custom icons:
pix.push([0x5D,0x55,0x55,0x55,0x77])
pix.show()
time.sleep(2)
pix.push([0x7F,0x40,0x7F,0x01,0x7F])
pix.show()
time.sleep(2)

# Ta-da!
pix.clear()
pix.write("DONE")
pix.show()
