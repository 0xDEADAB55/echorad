// P0.15 is a built in led, on the nice nano clones from aliexpress it is red.

#include <Arduino.h>
#include "variant.h"
#include <Wire.h>
#include <Display.h>
#include <CG_RadSens.h>
#include <EchoController.h>

static constexpr uint8_t RED_LED = PIN_015;            // Pin P0.15 called "LED".
static constexpr uint8_t GEIGER_COUNTER_PIN = PIN_106; // Pin P1.00 for reacting to pulses from the Geiger counter.

// Oled buttons
static constexpr uint8_t PIN_K1 = PIN_104;
static constexpr uint8_t PIN_K2 = PIN_011;
static constexpr uint8_t PIN_K3 = PIN_100;
static constexpr uint8_t PIN_K4 = PIN_024;

static constexpr uint8_t BUZZER_PIN = PIN_002; // P0.02 on common nice!nano v2 mappings

constexpr uint32_t RADIATION_MEASURE_INTERVAL_MS = 1000; // Measure radiation every 1000 milliseconds (1 second)
uint32_t lastMeasurementTime = 0;

bool isPulseDetected = false;

CG_RadSens radSens(RS_DEFAULT_I2C_ADDRESS);
EchoController::Controller controller(&radSens);
EchoDisplay::Display display(&Wire, &controller);

void onFall()
{
  isPulseDetected = true;
}

void setup()
{
  pinMode(BUZZER_PIN, OUTPUT);

  // Setup oled buttons
  pinMode(PIN_K1, INPUT_PULLUP);
  pinMode(PIN_K2, INPUT_PULLUP);
  pinMode(PIN_K3, INPUT_PULLUP);
  pinMode(PIN_K4, INPUT_PULLUP);

  // attach interrupts to buttons for better responsiveness (optional, can be removed if not needed)
  attachInterrupt(PIN_K1, []()
                  { controller.onSoundButtonPressed(); }, FALLING);
  attachInterrupt(PIN_K2, []()
                  { controller.onLightButtonPressed(); }, FALLING);
  attachInterrupt(PIN_K3, []()
                  { controller.onUnitsButtonPressed(); }, FALLING);
  attachInterrupt(PIN_K4, []()
                  { controller.onBluetoothButtonPressed(); }, FALLING);

  // Setup i2c and display
  Wire.setPins(PIN_115, PIN_113); // Set the I2C pins to P1.15 (SDA) and P1.13 (SCL) for the nice!nano v2
  Wire.setClock(400000);          // Set I2C clock speed to 400 kHz for faster communication with the display and sensor
  display.init();
  Serial.begin(115200);

  // setup geiger counter pulse detection
  pinMode(GEIGER_COUNTER_PIN, INPUT_PULLUP);
  attachInterrupt(
      digitalPinToInterrupt(GEIGER_COUNTER_PIN),
      onFall,
      FALLING);
}

void buzz()
{
  if (isPulseDetected && controller.getIsSoundOn())
  {
    isPulseDetected = false;
    tone(BUZZER_PIN, 800, 50);
  }
}

void loop()
{
  display.update();
  controller.tick();
  buzz();
  delay(10); // Delay for 10 ms to reduce CPU usage
}
