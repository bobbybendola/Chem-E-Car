/*
Channels and their correlating colors
  "Violet",       // F1 - 415nm
  "Blue",         // F2 - 445nm
  "Cyan",         // F3 - 480nm
  "Green",        // F4 - 515nm
  "Yellow-Green", // F5 - 555nm
  "Orange",       // F6 - 590nm
  "Red",          // F7 - 630nm
  "Deep Red"      // F8 - 680nm
*/

#include <Adafruit_AS7341.h>

Adafruit_AS7341 as7341;

#define LED_pin 13
#define motor_L293D_VCC1 10
#define motor_pin 3
#define linear_actuator_pin6 6
#define linear_actuator_pin7 7

#define csvLogState 0 // Set to 1 to enable CSV logging
#if csvLogState
#define csv_print(...) Serial.print(__VA_ARGS__)
#define csv_println(...) Serial.println(__VA_ARGS__)
#define regular_print(...)
#define regular_println(...)
#else
#define csv_print(...)
#define csv_println(...)
#define regular_print(...) Serial.print(__VA_ARGS__)
#define regular_println(...) Serial.println(__VA_ARGS__)
#endif

unsigned long startTime = 0;
bool timerStarted = false;

void setup()
{
  Serial.begin(115200);

  if (!as7341.begin()) {
    Serial.println("Could not find AS7341");
    while (1) delay(10);
  }

  as7341.setATIME(100);
  as7341.setASTEP(999);
  as7341.setGain(AS7341_GAIN_256X);
}

void loop()
{
  uint16_t readings[12];

  if (!as7341.readAllChannels(readings)) {
    Serial.println("Error reading all channels!");
    return;
  }

  // Use selected channel for logic
  unsigned int measured_intensity = readings[control_channel];

  // Display all readings 
  regular_println("----- AS7341 Readings -----");
  //regular_print("F1 (415nm): ");  regular_println(readings[AS7341_CHANNEL_415nm_F1]);
  regular_print("F2 (445nm): ");  regular_println(readings[AS7341_CHANNEL_445nm_F2]);
  //regular_print("F3 (480nm): ");  regular_println(readings[AS7341_CHANNEL_480nm_F3]);
  //regular_print("F4 (515nm): ");  regular_println(readings[AS7341_CHANNEL_515nm_F4]);
  //regular_print("F5 (555nm): ");  regular_println(readings[AS7341_CHANNEL_555nm_F5]);
  //regular_print("F6 (590nm): ");  regular_println(readings[AS7341_CHANNEL_590nm_F6]);
  //regular_print("F7 (630nm): ");  regular_println(readings[AS7341_CHANNEL_630nm_F7]);
  //regular_print("F8 (680nm): ");  regular_println(readings[AS7341_CHANNEL_680nm_F8]);
  //regular_print("Clear: ");        regular_println(readings[AS7341_CHANNEL_CLEAR]);
  //regular_print("NIR: ");          regular_println(readings[AS7341_CHANNEL_NIR]);

  // Start timer if needed
  if (!timerStarted) {
    startTime = millis();
    timerStarted = true;
  }
  unsigned long currentTime = millis() - startTime;

  // CSV logging if enabled
  csv_print(currentTime); csv_print(",");
  csv_print(readings[AS7341_CHANNEL_415nm_F1]); csv_print(",");
  csv_print(readings[AS7341_CHANNEL_445nm_F2]); csv_print(",");
  csv_print(readings[AS7341_CHANNEL_480nm_F3]); csv_print(",");
  csv_print(readings[AS7341_CHANNEL_515nm_F4]); csv_print(",");
  csv_print(readings[AS7341_CHANNEL_555nm_F5]); csv_print(",");
  csv_print(readings[AS7341_CHANNEL_590nm_F6]); csv_print(",");
  csv_print(readings[AS7341_CHANNEL_630nm_F7]); csv_print(",");
  csv_print(readings[AS7341_CHANNEL_680nm_F8]); csv_print(",");
  csv_print(readings[AS7341_CHANNEL_CLEAR]);    csv_print(",");
  csv_println(readings[AS7341_CHANNEL_NIR]);

  delay(500); // Optional delay
}
