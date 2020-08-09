/*
  Pixie NTP CLOCK Example
  -----------------------
  
  A full NTP clock system for Pixies, built for the
  ESP8266. Best used with 6 Pixies (12 characters),
  it has runtime configuration of your WiFi
  credentials and can automatically dim the displays
  at night.

  This example *relies* on using an ESP8266 controller, which is used to pull
  UTC time from an NTP server over WiFi. There are also a few external libraries
  required for NTP timekeeping, which are provided as GitHub links in the 
  #include's of this sketch and can also be installed by name using the
  Arduino Library Manager.

  To get started:

  - Connect your first Pixie to the CLK and DATA pins
    defined below
  - Connect the Pixie's VCC pin to your ESP8266 5V pin
  - Connect the Pixie's GND pin to your ESP8266 GND pin
  - Connect the rest of the Pixie chain according to 
    the Getting Started Guide (https://github.com/connornishijima/Pixie/blob/master/getting_started.md)
  - Set your UTC_OFFSET (timezone) accordingly, accounting
    for Daylight Savings, if applicable.
  - Connect a buzzer or speaker between the BUZZER pin
    defined below, and GND.
  - Upload the Sketch to your ESP8266

  Upon booting, it will fail to connect to WiFi as it doesn't yet know your
  credentials. It will then host it's own WiFi access point named "PIXIE CONFIG"
  that you can connect your phone to.
  Once connected, go to http://192.168.4.1/ in your phone's browser, and you
  will see a menu that will let you send the WiFi details down to the clock,
  to be remembered from this point forward. (Even through power cycles!)
*/

#include "Pixie.h"

// 4-6 Pixies are supported, 5-6 is best and supports AM/PM display.
#define NUM_PIXIES      6     // PCBs, not matrices
#define UTC_OFFSET      -6    // America/Denver timezone (-7), +1 for DST
#define NIGHT_DIMMING   true  // Dim clock from 9PM to 6AM?
#define MIN_BRIGHTNESS  32    // 7-bit brightness during NIGHT_DIMMING
#define MAX_BRIGHTNESS  127   // 7-bit brightness during daytime

#define CLK_PIN         14    // (D5) Any digital pin
#define DATA_PIN        12    // (D6) Any digital pin
#define BUZZER          13    // (D7) Any digital pin

// Stupid ESP8266/32 Compatibility
#ifdef ESP8266
  #include <ESP8266WiFi.h>
  #include <ESP8266WebServer.h>
#elif defined(ESP32)
  #include <WiFi.h>
  #include <WebServer.h>
#endif
// Other WiFi libraries that don't have stupid names
#include <DNSServer.h>
#include <WiFiUdp.h>
#include <FS.h>

#include <WiFiManager.h>         // https://github.com/tzapu/WiFiManager            <--------------------------- EXTERNAL LIBRARY NEEDED
#include <NTPClient.h>           // https://github.com/arduino-libraries/NTPClient  <--------------------------- EXTERNAL LIBRARY NEEDED

Pixie pix(NUM_PIXIES, CLK_PIN, DATA_PIN); // Set up display buffer
WiFiUDP ntp_UDP;
NTPClient time_client(ntp_UDP, "pool.ntp.org", 3600, 60000);
#define SECONDS_PER_HOUR 3600

uint32_t t_now = 0;
uint8_t last_seconds = 0;

uint8_t hh = 0;
uint8_t mm = 0;
uint8_t ss = 0;

uint8_t bright = 127;
uint8_t target_brightness = 127;
uint32_t last_frame = 0;
bool colon_flip = false;
uint8_t colon_index = 0;
uint8_t colon[10][5] = {
  {0x00, 0x24, 0x36, 0x00, 0x00},
  {0x00, 0x24, 0x24, 0x00, 0x00},
  {0x00, 0x24, 0x00, 0x00, 0x00},
  {0x00, 0x36, 0x00, 0x00, 0x00},
  {0x00, 0x36, 0x12, 0x00, 0x00},
  {0x00, 0x36, 0x36, 0x00, 0x00},
  {0x00, 0x36, 0x36, 0x00, 0x00},
  {0x00, 0x36, 0x36, 0x00, 0x00},
  {0x00, 0x36, 0x36, 0x00, 0x00},
  {0x00, 0x36, 0x36, 0x00, 0x00}
};

void setup() {
  Serial.begin(115200);
  init_displays();
  init_wifi();
  init_ntp();
}

void loop() {
  t_now = millis();
  if (bright < target_brightness) {
    bright++;
  }
  else if (bright > target_brightness) {
    bright--;
  }
  pix.brightness(bright);
  
  if (t_now - last_frame >= 25) {
    last_frame = t_now;
    //colon_flip = !colon_flip;
    //if (colon_flip) {
      colon_index++;
      if (colon_index == 5 || colon_index == 10) {
        show_time();
        if (colon_index >= 10) {
          colon_index = 0;
        }
      //}
    }

    if (NUM_PIXIES == 6) {
      pix.write(colon[colon_index][0], colon[colon_index][1], colon[colon_index][2], colon[colon_index][3], colon[colon_index][4], 3);
      pix.write(colon[colon_index][0], colon[colon_index][1], colon[colon_index][2], colon[colon_index][3], colon[colon_index][4], 6);
      pix.show();
    }
    else if (NUM_PIXIES == 5) {
      pix.write(colon[colon_index][0], colon[colon_index][1], colon[colon_index][2], colon[colon_index][3], colon[colon_index][4], 2);
      pix.write(colon[colon_index][0], colon[colon_index][1], colon[colon_index][2], colon[colon_index][3], colon[colon_index][4], 5);
      pix.show();
    }
    else if (NUM_PIXIES == 4) {
      pix.write(colon[colon_index][0], colon[colon_index][1], colon[colon_index][2], colon[colon_index][3], colon[colon_index][4], 2);
      pix.write(colon[colon_index][0], colon[colon_index][1], colon[colon_index][2], colon[colon_index][3], colon[colon_index][4], 5);
      pix.show();
    }
  }
  
  pix.show();
  delay(10);
}

void show_time() {
  time_client.update();
  hh = time_client.getHours();
  mm = time_client.getMinutes();
  ss = time_client.getSeconds();

  if(NIGHT_DIMMING == true){
    if (hh < 6 || hh >= 21) { // Dim overnight from 9PM to 6AM (21:00 to 06:00)
      target_brightness = MIN_BRIGHTNESS;
    }
    else { // If daytime, full brightness
      target_brightness = MAX_BRIGHTNESS;
    }
  }
  else{ // If not enabled, full brightness
    target_brightness = MAX_BRIGHTNESS;
  }

  char meridian = 'A';

  if (hh >= 12) {
    meridian = 'P';
  }

  // 12 hour mode
  if (hh > 12) {
    hh -= 12;
  }
  if (hh == 0) {
    hh = 12;
  }

  char hh_ch[3] = {0, 0, 0};
  itoa(hh, hh_ch, 10);
  if (hh_ch[1] == 0) {
    hh_ch[1] = hh_ch[0];
    hh_ch[0] = '0';
  }

  char mm_ch[3] = {0, 0, 0};
  itoa(mm, mm_ch, 10);
  if (mm_ch[1] == 0) {
    mm_ch[1] = mm_ch[0];
    mm_ch[0] = '0';
  }

  char ss_ch[3] = {0, 0, 0};
  itoa(ss, ss_ch, 10);
  if (ss_ch[1] == 0) {
    ss_ch[1] = ss_ch[0];
    ss_ch[0] = '0';
  }

  if (NUM_PIXIES == 6) {
    char output[13];
    sprintf(output, " %s:%s:%s %c", hh_ch, mm_ch, ss_ch, meridian);
    pix.clear();
    pix.write(output); // Update numerals
  }
  else if (NUM_PIXIES == 5) {
    char output[13];
    sprintf(output, "%s:%s:%s %c", hh_ch, mm_ch, ss_ch, meridian);
    pix.clear();
    pix.write(output); // Update numerals
  }
  else if (NUM_PIXIES == 4) {
    char output[13];
    sprintf(output, "%s:%s:%s", hh_ch, mm_ch, ss_ch);
    pix.clear();
    pix.write(output); // Update numerals
  }

  last_seconds = ss;
}

void enter_config_mode() {
  pix.clear();
  pix.write("   CONFIG");
  pix.show();
  beep_dual(2000, 1000, 500);
}

void config_mode_callback (WiFiManager * myWiFiManager) {
  enter_config_mode();
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void init_wifi() {
  WiFiManager wifiManager;
  wifiManager.setAPCallback(config_mode_callback);
  wifiManager.setTimeout(300); // Five minutes
  beep(2000, 100);

  pix.clear();
  pix.write(" Connecting");
  for (uint8_t i = 0; i < 127; i++) {
    pix.brightness(i);    
    pix.show();
  }
  
  while (!wifiManager.autoConnect("PIXIE CONFIG")) {}
  beep_dual(1000, 2000, 100);
  
  for (uint8_t i = 127; i > 0; i--) {
    pix.brightness(i);
    pix.show();
  }
  bright = 0;
  pix.brightness(bright);
  pix.show();
  
  pix.clear();
}

void init_ntp() {
  time_client.begin();
  time_client.setTimeOffset(UTC_OFFSET * SECONDS_PER_HOUR);
}

void init_displays() {
  pix.begin(); // Init display drivers
}

void beep(uint16_t freq, uint16_t len_ms) {
  tone(BUZZER, freq, len_ms);
}

void beep_dual(uint16_t freq1, uint16_t freq2, uint16_t len) {
  beep(freq1, len);
  beep(freq2, len);
}
