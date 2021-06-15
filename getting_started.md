# Getting Started with PIXIE and PIXIE PRO

![PIXIE](https://i.imgur.com/rmpfoyw.jpg)

### *I know you're excited, but we have a few things to cover to make sure you're ready to run Pixie displays!*

# THINGS TO NOTE:

## Microcontroller

Any AVR/Arduino, ESP8266/32/derivative, or SAMD microcontroller can be used to control Pixie displays. There is a Pixie library available for the Arduino IDE:

### Installing the Pixie Library

**With Arduino Library Manager: (RECOMMENDED)**

1. Open *Sketch > Include Library > Manage Libraries* in the Arduino IDE.
2. Search for "Pixie", and select the latest version.
3. Click the Install button and Arduino will prepare the library and examples for you!

**Manual Install:**

1. Click "Clone or Download" above to get an "Pixie-master.zip" file.
2. Extract its contents to the libraries folder in your Arduino sketchbook. ("C:/Users/**YOUR_USERNAME**/Documents/Arduino/libraries" on Windows)
3. Rename the folder from "Pixie-master" to "Pixie".

When finished, an example folder structure for Windows should be:

    C:\Users\[USERNAME]\Documents\Arduino\libraries\Pixie\src\Pixie.cpp

## Powering Pixies

### Pixies are best powered with 5 Volts.

The on-board ATTINY45 is nominally powered with 5 volts DC to maintain the 16MHz clock. If you have a 3V3-logic microcontroller (ESP8266/32), you may or may not have to use [a level shifter](https://www.adafruit.com/product/1787). However, I have successfully run Pixies off of a 3.3V Wemos D1 Mini (ESP8266 controller) without needing a level shifter - even with the ATTINY45 powered by the 5 volt line.

A potential workaround is underpowering the Pixies with a 3.3V power line, but this will only be 66% of the brightness 5V power provides, and *may* destabilize the 16MHz internal oscillator.

#### Full Brightness

If you want to allow for any possible lighting scenario, you'll need to have a power supply that can support ~60mA per Pixie. For a 6-Pixie display flashing every single LED at full-brightness, you would need a power supply capable of 60mA*6 = 360mA. However, most people won't be using Pixies as a lightbar for their off-road truck, so most USB supplies should be just fine for most cases. ;) (Plenty of USB charging blocks can supply this.)

## Wiring

Hookup looks like this:

    MICROCONTROLLER 5V        ---->   PIXIE VCC
    MICROCONTROLLER GND       ---->   PIXIE GND
    MICROCONTROLLER DAT_PIN   ---->   PIXIE DAT_IN
    MICROCONTROLLER CLK_PIN   ---->   PIXIE CLK

Pixies are designed to be daisy-chained, and run from right-to-left. Connecting one display to the next is like this:

![CHAIN](https://i.imgur.com/ikhDXa2.png)

*You can use Dupont female-to-female jumpers to connect the displays, or solder directly to the headers. We also have [an EAGLE library available](https://github.com/connornishijima/Pixie/tree/master/extras/hardware) to work Pixie into your own PCBs!*

## PIXIE PINOUT

(From [the hardware datasheet](https://connornishijima.github.io/Pixie/extras/datasheet.html))

![PINOUT](https://i.imgur.com/LnHboKm.png)

If you have 4 or more displays in the chain, I recommend connecting *BOTH* ends of the chain to the 5V/GND lines of the power supply to avoid having dimmer characters at the end of the chain. (Due to the voltage drop across displays) Without doing so, you may notice the displays on the far end of the chain gradually falling into a darker color.

## Writing Arduino code for Pixies

Once you've installed the Pixie library, go ahead and try this example code to make sure everything is working:

    #include "Pixie.h"
    #define NUM_PIXIES  6                          // PCBs, not matrices
    #define CLK_PIN     14                         // Any digital pin
    #define DATA_PIN    12                         // Any digital pin
    Pixie pix(NUM_PIXIES, CLK_PIN, DATA_PIN, PRO); // Set up display buffer
    
    void setup() {
      pix.begin(); // Init display drivers
    }
    
    void loop() {
      pix.clear();
      pix.push( millis()/1000.0, 2 ); // Show floating-point number to two decimal places (hundredths of a second)
      pix.show();
    }

**NOTE: If you're not using a Pixie Pro, you don't need the "PRO" in *Pixie pix();***

Change the **NUM_PIXIES** variable to match the number of Pixie PCBs you have wired up. When uploaded, you should see the seconds since boot (with hundreths) on your Pixies!

## Moving Forward

Once again, if you're using a ***Pixie Pro***, the **Pixie pix(NUM_PIXIES, CLK_PIN, DATA_PIN)** constructor *needs* a "PRO" at the end in all code uploaded, like so:
    
    Pixie pix(NUM_PIXIES, CLK_PIN, DATA_PIN, PRO); <-- HERE

Otherwise, the data will be sent out in a way that Pixie Pro does not recognize, and your image will be distorted!

From here, I suggest trying out the other examples included with the Pixie library, as it covers many of the neat writing and bitmap functions that Pixie is capable of!
