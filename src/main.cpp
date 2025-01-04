//ВАЖНО! Схемы подключения по образцу вы можете найти в "extras/Example schemes/ESP32 & ESP8266/" и выбрать свою плату
// Подключаем необходимые библиотеки
#include <Arduino.h>
#include "CG_RadSens.h"  // Библиотека RadSens
#include <Wire.h>        // I2C-библиотека
#include <GyverOLED.h>   // Библиотека для OLED Gyver'а идеально подойдёт для понимания методики работы с OLED-экраном, к тому же тут сразу есть русский шрифт
#define buz 19   // Устанавливаем управляющий пин пьезоизлучателя. Если вы выбрали другой управляющий пин - замените значение
#define BLINK_LED 13
#define IMPULSE_PIN 2

CG_RadSens radSens(RS_DEFAULT_I2C_ADDRESS);   // Инициализируем RadSens
GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> oled;   // Инициализируем OLED-экран


uint32_t timer_cnt; // Таймер опроса интенсивности излучения и ипульсов для OLED-экрана
uint32_t timer_imp; // Таймер опроса импульсов для пьезоизлучателя
uint32_t timer_oled; // таймер обновления дисплея

volatile uint32_t timer_blinker = 0;
bool is_blinker_on = false;

float dynval;  // Переменная для динамического значения интенсивности
float statval; // Переменная для статического значения интенсивности
uint32_t impval;  // Переменная для кол-ва импульсов


void onImpulse();
void blink();

void setup() {
  oled.init();           // Инициализируем OLED в коде
  oled.clear();
  oled.setScale(5);      // Устанавливаем размер шрифта
  oled.print("ECHO");
  oled.setScale(2);
  delay(3000);
  oled.clear();
  pinMode(BLINK_LED, OUTPUT);
  pinMode(IMPULSE_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(IMPULSE_PIN), onImpulse, FALLING);
  analogReference(INTERNAL);
  Serial.begin(9600);
}



void loop() {
  if (millis() - timer_cnt > 1000) {      // Записываем в объявленные глобальные переменные необходимые значения
    timer_cnt = millis();
    dynval = radSens.getRadIntensyDynamic(); 
    statval = radSens.getRadIntensyStatic();
    impval = radSens.getNumberOfPulses();
  }

  if (millis() - timer_oled > 1000) {  //Записываем переменные в строки и выводим их на OLED-экран
    timer_oled = millis();
    String dynint = "Dyn:";
    dynint += dynval;
    dynint += "   mkR/h";
    String statint = "Stat:";
    statint += statval;
    statint += "   mkR/h ";
    String nimp = "Imp:";
    nimp += impval;
    nimp += " ";
    oled.setCursor(0, 1);
    oled.print(dynint);
    oled.setCursor(0, 3);
    oled.print(statint);
    oled.setCursor(0, 5);
    oled.print(nimp);
  }

  blink();

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