![Pixie](extras/img/modules_banner.png)

# Pixie for Arduino

Pixies are chainable 5x7 microdot LED matrices great for showing... just about anything you can throw at them!

- Dual 5x7 LED matrices
- 2.7V to 5.5V supply
- Fast bitmap control
- 7-bit global PWM (128 brightness levels)
- Over-temperature protection
- Resettable from controlling device
- Reprogrammable ATTINY45 microcontroller
- 5.1mm * 7.6mm matrix size (19.92 ppi)
- 20.6mm * 34.7mm package
- M2.5 mouting holes

![PIXIE](https://i.imgur.com/s4ye4Uu.jpg)
*Pixies mounted into a home-made case!*

## Included in this repository:

**[PIXIE WEBSITE](https://connornishijima.github.io/Pixie/) *(Contains all documentation and tools in one spot)***

- Arduino Library Source Code
- Software Documentation
- Hardware Datasheet
- Pixie Icon Generator
- Raspberry Pi Python Driver (Beta)
- PCB Gerbers

## Software Documentation

Documentation for the Pixie library is available in both the /examples directory for Arduino, and at the site below:

**[Library Documentation & Usage](https://connornishijima.github.io/Pixie/extras/documentation.html)**

We also have a **[Getting Started Guide](https://github.com/connornishijima/Pixie/blob/master/getting_started.md)** to help you get set up for the very first time!

## Hardware Documentation

Pixie has a comprehensive hardware datasheet (including internal firmware documentation and package dimensions for mounting) available here:

**[PIXIE DATASHEET](https://connornishijima.github.io/Pixie/extras/datasheet.html)**

This library also contains a Pixie EAGLE library (In "extras/hardware") for adding Pixies into your own PCB layouts!

----------
# Arduino Library Installation

**With Arduino Library Manager:**

1. Open *Sketch > Include Library > Manage Libraries* in the Arduino IDE.
2. Search for "Pixie" by Connor Nishijima, and select the latest version.
3. Click the Install button and Arduino will prepare the library and examples for you!

**Manual Install:**

1. Click "Clone or Download" above to get an "Pixie-master.zip" file.
2. Extract its contents to the libraries folder in your Arduino sketchbook. ("C:/Users/**YOUR_USERNAME**/Documents/Arduino/libraries" on Windows)
3. Rename the folder from "Pixie-master" to "Pixie".

----------
# Contributing
Any advice or pull requests are welcome. :)

----------
# License and Credits
**Developed by Connor Nishijima (Copyright 2020)**

I work full-time as a Tindie seller providing income for my wife and I. You are free to make your own Pixies with the infomation provided in this repository, but **please be ethical and *do not sell clones***. If you modify any information from this repository, and have used it to make improvements to my hardware design that you truly think puts your version superior to the original (and thus could compete in sales due to these advantages), then at the very least, **please attribute** Lixie Labs LLC. if you sell them - as required by the GPLv3 License below and included in the root of this repository.

**Released under the [GPLv3 License](http://www.gnu.org/licenses/gpl-3.0.en.html).**
