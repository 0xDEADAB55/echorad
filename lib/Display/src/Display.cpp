#include <Display.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

namespace EchoDisplay {
    EchoDisplay::EchoDisplay(TwoWire *twi) : display(128, 64, twi, -1) {}

            void EchoDisplay::init() {
                  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
                display.clearDisplay();
                display.setTextSize(1);
                display.setTextColor(SSD1306_WHITE);
                display.setCursor(0, 0); //place where the text will be drawn
                display.println("By ICantMakeThings");
                display.setCursor(0, 30);
                display.println("And cat5e-inv");
                display.display();
                
            }
            void EchoDisplay::clear() {
                display.clearDisplay();
            }
            void EchoDisplay::print(const char* text) {
                display.println(text);
                display.display();
            }
}