//P0.15 is a built in led, on the nice nano clones from aliexpress it is red.

#include <Arduino.h>
#include "variant.h"
#include <Wire.h>
#include <Display.h>


#define RED_LED PIN_015 //Set a definiton on pin P0.15 called "LED".


constexpr uint8_t BUZZER_PIN = PIN_002; // P0.02 on common nice!nano v2 mappings



void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  Wire.setPins(PIN_115, PIN_113); // Set the I2C pins to P1.15 (SDA) and P1.13 (SCL) for the nice!nano v2
  EchoDisplay::EchoDisplay display(&Wire);
  display.init();
}

void loop() {
  tone(BUZZER_PIN, 4000);
  delay(200);

  noTone(BUZZER_PIN);
  delay(500);
}