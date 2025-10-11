#include <Wire.h>
#include "Adafruit_AS7341.h"

Adafruit_AS7341 as7341;

// Map of color names for F1–F8 (channels 1–8)
const char* colorNames[8] = {
  "Violet",       // F1 - 415nm
  "Blue",         // F2 - 445nm
  "Cyan",         // F3 - 480nm
  "Green",        // F4 - 515nm
  "Yellow-Green", // F5 - 555nm
  "Orange",       // F6 - 590nm
  "Red",          // F7 - 630nm
  "Deep Red"      // F8 - 680nm
};

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("Initializing AS7341...");

  if (!as7341.begin()) {
    Serial.println("AS7341 not found. Check wiring!");
    while (1) delay(10);
  }

  Serial.println("AS7341 initialized. Beginning continuous color detection...");
}

void loop() {
  if (!as7341.readAllChannels()) {
    Serial.println("Sensor read failed!");
    delay(1000);
    return;
  }

  int maxChannel = 0;
  uint16_t maxValue = 0;

  Serial.println("Spectral Readings:");
    uint16_t value = as7341.getChannel(2);
    Serial.print("F"); Serial.print(2); Serial.print(" (");
    //Serial.print(colorNames[i - 1]); Serial.print("): ");
    Serial.println(value);

    /*if (value > maxValue) {
      maxValue = value;
      maxChannel = i;
    }*/
    delay(1000); 
  }

  /*const char* detectedColor = colorNames[maxChannel - 1];
  Serial.print("→ Dominant Color: ");
  Serial.println(detectedColor);
  Serial.println("-----------------------------------");*/

  // Wait 1 second before next reading

