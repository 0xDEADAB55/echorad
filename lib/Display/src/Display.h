#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

namespace EchoDisplay {

class EchoDisplay {
public:
    explicit EchoDisplay(TwoWire* twi = &Wire);

    void init();
    void clear();
    void print(const char* text);

private:
    Adafruit_SSD1306 display;
};

} // namespace Display