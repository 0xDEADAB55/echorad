#include <Display.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <FontTimesNR.h>
#include <FontBat6.h>

namespace EchoDisplay
{
    EchoDisplay::EchoDisplay(TwoWire *twi, EchoController::Controller controller)
    {
        display = Adafruit_SSD1306(128, 64, twi);
        controller = controller;
    }

    void EchoDisplay::init()
    {
        display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
        display.setFont(&TimesNRCyr6pt8b);
        display.clearDisplay();
        // display.setTextSize(1);
        // display.setTextColor(SSD1306_WHITE);
        // display.setCursor(0, 10); // place where the text will be drawn
        // display.println("ECHO");
        // display.setCursor(0, 30);
        // display.println("Radiation Detector");
        drawSound();
        display.display();
    }
    void EchoDisplay::clear()
    {
        display.clearDisplay();
    }
    void EchoDisplay::print(const char *text)
    {
        display.println(text);
        display.display();
    }
    void EchoDisplay::printRadiationLevel(float level, uint32_t number_of_pulses, float staticRadiationLevel, float batteryLevel)
    {
        display.clearDisplay();
        display.setCursor(0, 10);
        display.setFont(&TimesNRCyr6pt8b);
        display.print("ДинУров: ");
        display.print(level, 0);
        display.println(" мкР/ч");
        display.print("СтатУров: ");
        display.print(staticRadiationLevel, 0);
        display.println(" мкР/ч");
        display.print("Пульсы: ");
        display.println(number_of_pulses);
        display.print("Батарея: ");
        display.setFont(&Bat6pt7b);
        this->batteryLevel = (this->batteryLevel + 1) % 5;
        display.print(this->batteryLevel + '0');

        // Draw icons
        drawSound();
        drawLight();
        drawUnits();
        drawBluetooth();

        display.display();
    }

    void EchoDisplay::drawSound()
    {
        if (controller.getIsSoundOn())
        {
            // volume_sound
            display.drawBitmap(113, 1, image_volume_sound_bits, 14, 15, 1);
        }
        else
        {
            // volume_no_sound
            display.drawBitmap(112, 1, image_volume_no_sound_bits, 18, 16, 1);
        }

        display.display();
    }

    void EchoDisplay::drawLight()
    {
        // Placeholder for drawing light icon
        if (controller.getIsLightOn())
        {
            // light
            display.drawBitmap(112, 16, image_light_bits, 16, 16, 1);
        }
        else
        {
            // no_ligth
            display.drawBitmap(116, 17, image_no_ligth_bits, 8, 14, 1);
        }
    }

    void EchoDisplay::drawUnits()
    {
        if (controller.getIsUsingMicroSieverts())
        {
            // mkSv/h units
            display.drawBitmap(112, 32, image_unit_bits, 15, 15, 1);
        }
        else
        {
            // mkR/H units
            display.drawBitmap(112, 32, image_unit_bits, 15, 15, 1);
        }
    }

    void EchoDisplay::drawBluetooth()
    {
        // Placeholder for drawing Bluetooth icon
        if (controller.getIsBluetoothOn())
        {
            // bluetooth_connected
            display.drawBitmap(114, 47, image_bluetooth_connected_bits, 14, 16, 1);
        }
        else
        {
            // bluetooth_not_connected
            display.drawBitmap(113, 47, image_bluetooth_not_connected_bits, 14, 16, 1);
        }
    }
}