#include <Arduino.h>
#include <Wire.h>

#include "CG_RadSens.h"
#include <GyverOLED.h>
#include <TimerMs.h>

#define buz 19
#define BLINK_LED LED_BUILTIN
#define IMPULSE_PIN 2
#define BATTERY_VOLDAGE_PIN A1

#define BATTERY_LOW_VOLTAGE_VOLTS 3.3

#define INTERNAL_REF_VOLTAGE_VOLTS 1.074
#define ADC_RESOLUTION_BITS 10
#define V_DIVIDER_R1_OHMS 4637
#define V_DIVIDER_R2_OHMS 981

CG_RadSens radSens(RS_DEFAULT_I2C_ADDRESS);
GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> oled;

void onImpulse(); // interrupt handler
void blink();

void updateRadiationValues();
void updateBatteryVoltage();
void display();

String animation[3] = {".  ", " . ", "  ."};
int cur_animation = 0;

TimerMs t_cnt = TimerMs(1000);  // get data from RadSens
TimerMs t_oled = TimerMs(1000); // draw on oled screen
TimerMs t_bat = TimerMs(10000); // update battery voltage

uint32_t timer_imp; // blink interrupt

volatile uint32_t timer_blinker = 0;
bool is_blinker_on = false;

float battery_voltage = 100;
bool battery_low = false;

float dynval;
float statval;
uint32_t impval;

void setup()
{
  oled.init();
  oled.clear();
  oled.setScale(5);
  oled.print("ECHO");
  oled.setScale(1);
  delay(3000);
  oled.clear();
  pinMode(BLINK_LED, OUTPUT);
  pinMode(IMPULSE_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(IMPULSE_PIN), onImpulse, FALLING);
  analogReference(INTERNAL);
  Serial.begin(9600);

  t_cnt.attach(updateRadiationValues);
  t_cnt.start();

  t_oled.attach(display);
  t_oled.start();

  t_bat.attach(updateBatteryVoltage);
  t_bat.start();
  analogRead(BATTERY_VOLDAGE_PIN);
}

void loop()
{
  t_cnt.tick();
  t_oled.tick();
  t_bat.tick();

  blink();
}

void updateRadiationValues()
{
  dynval = radSens.getRadIntensyDynamic();
  statval = radSens.getRadIntensyStatic();
  impval = radSens.getNumberOfPulses();
}

void display()
{
  String dynint = "Dyn:";
  dynint += dynval;
  dynint += "   mkR/h";
  String statint = "Stat:";
  statint += statval;
  statint += "   mkR/h ";
  String nimp = "Imp:";
  nimp += impval;
  nimp += " ";
  String nbat = "Batt: ";
  cur_animation = (cur_animation + 1) % 3;
  if (battery_voltage > 15)
  {
    nbat += animation[cur_animation];
  }
  else
  {
    nbat += battery_voltage;
    if (battery_low && cur_animation > 0)
    {
      nbat += "V LOW!  ";
    }
    else
    {
      nbat += "V      ";
    }
  }

  oled.setCursor(0, 1);
  oled.print(dynint);
  oled.setCursor(0, 3);
  oled.print(statint);
  oled.setCursor(0, 5);
  oled.print(nimp);
  oled.setCursor(0, 7);
  oled.print(nbat);
}

void updateBatteryVoltage()
{
  int val = analogRead(A1);
  float input_voltage = val * INTERNAL_REF_VOLTAGE_VOLTS / (1 << ADC_RESOLUTION_BITS);
  battery_voltage = input_voltage * (V_DIVIDER_R1_OHMS + V_DIVIDER_R2_OHMS) / V_DIVIDER_R2_OHMS;
  battery_low = battery_voltage < BATTERY_LOW_VOLTAGE_VOLTS;
}

void blink()
{
  if ((millis() - timer_blinker) > 100)
  {
    if (is_blinker_on)
    {
      digitalWrite(BLINK_LED, false);
      oled.setCursor(92, 5);
      oled.print("[ ]");
      oled.home();
      is_blinker_on = false;
    }
  }
  else
  {
    if (!is_blinker_on)
    {
      digitalWrite(BLINK_LED, true);
      oled.setCursor(92, 5);
      oled.print("[#]");
      oled.home();
    }
    is_blinker_on = true;
  }
}

void asyncBlink()
{
  timer_blinker = millis();
}

void onImpulse()
{
  asyncBlink();
}