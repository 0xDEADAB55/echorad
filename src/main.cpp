// P0.15 is a built in led, on the nice nano clones from aliexpress it is red.

#include <Arduino.h>
#include "variant.h"
#include <Wire.h>
#include <Display.h>
#include <CG_RadSens.h>
#include <EchoController.h>



#define RED_LED PIN_015 // Pin P0.15 called "LED".
#define GEIGER_COUNTER_PIN PIN_100 // Pin P1.00 for reacting to pulses from the Geiger counter.

constexpr uint8_t BUZZER_PIN = PIN_002; // P0.02 on common nice!nano v2 mappings

constexpr uint32_t RADIATION_MEASURE_INTERVAL_MS = 1000; // Measure radiation every 1000 milliseconds (1 second)
uint32_t lastMeasurementTime = 0;

bool isPulseDetected = false;

CG_RadSens radSens(RS_DEFAULT_I2C_ADDRESS);
EchoController::Controller controller(radSens);
EchoDisplay::EchoDisplay display(&Wire, controller);


void onFall() {
  isPulseDetected = true;
}

void setup()
{
  pinMode(BUZZER_PIN, OUTPUT);
  Wire.setPins(PIN_115, PIN_113); // Set the I2C pins to P1.15 (SDA) and P1.13 (SCL) for the nice!nano v2
  display.init();
  Serial.begin(115200);

  pinMode(GEIGER_COUNTER_PIN, INPUT_PULLUP);

  attachInterrupt(
    digitalPinToInterrupt(GEIGER_COUNTER_PIN),
    onFall,
    FALLING
  );


}

void buzz() {
  if (isPulseDetected) {
    isPulseDetected = false;
     tone(BUZZER_PIN, 800, 50
    );
  }
}

void loop()
{
  if (millis() - lastMeasurementTime >= RADIATION_MEASURE_INTERVAL_MS)
  {
    float radiationLevel = radSens.getRadIntensyDynamic();
    float staticRadationLevel = radSens.getRadIntensyStatic();
    uint32_t numberOfPulses = radSens.getNumberOfPulses();
    display.printRadiationLevel(radiationLevel, numberOfPulses, staticRadationLevel, 0);

    // if (radiationLevel > 0.5) { // If radiation level exceeds 0.5 uR/h, sound the buzzer
    //   tone(BUZZER_PIN, 1000, 500); // Sound the buzzer at 1000 Hz for 500 ms
    // }

    lastMeasurementTime = millis();
  }
  buzz();
  delay(10); // Delay for 10 ms to reduce CPU usage
}
