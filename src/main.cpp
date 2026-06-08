//P0.15 is a built in led, on the nice nano clones from aliexpress it is red.

#include <Arduino.h>
#include "variant.h"

#define RED_LED PIN_015 //Set a definiton on pin P0.15 called "LED".

void setup() {
  pinMode(RED_LED, OUTPUT); //Set the LED to output mode.
}

void loop() {
  digitalWrite(RED_LED, LOW); //Set the LED to low
  delay(1000); //wait 1s
  digitalWrite(RED_LED, HIGH); //Set the LED to high
  delay(3000); //wait 3s
}