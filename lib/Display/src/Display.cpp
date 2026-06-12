#include <Display.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <FontTimesNR.h>
#include <FontFreeMono.h>
#include <FontBat6.h>

namespace EchoDisplay
{
    void Display::init()
    {
        display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
        display.setFont(&TimesNRCyr6pt8b);
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        drawSplash();
        display.display();
        delay(3000); // Show splash screen for 2 seconds
    }
    void Display::clear()
    {
        display.clearDisplay();
    }
    void Display::print(const char *text)
    {
        display.println(text);
        display.display();
    }

    void Display::update()
    {
        if (controller->needUpdateUI())
        {
            repaint();
        }
    }

    void Display::repaint()
    {
        display.clearDisplay();
        drawBatteryLevel();
        drawRadiationLevel();
        drawSound();
        drawLight();
        drawUnits();
        drawBluetooth();
        display.display();
    }

    void Display::drawBatteryLevel()
    {

        const auto percentage = controller->getBatteryLevelPercentage();
        const float voltage = controller->getBatteryLevelVoltage();

        display.setCursor(0, 10);
        display.setFont(&Bat6pt7b);
        char level = '0';
        if (percentage <= 100)
        {
            level = '4';
        }
        if (percentage <= 80)
        {
            level = '3';
        }
        if (percentage <= 50)
        {
            level = '2';
        }
        if (percentage <= 30)
        {
            level = '1';
        }
        if (percentage <= 15)
        {
            level = '0';
        }
        display.print(level);
        display.setFont(DIGIT_FONT);
        display.print(' ');
        display.print(voltage, 2);
        display.print('B');
    }

    void Display::drawRadiationLevel()
    {
        const float dynLevelSv = controller->getDynamicRadiationLevel();
        const float statLevelSv = controller->getStaticRadiationLevel();

        if (controller->getIsUsingMicroSieverts())
        {
            char dynRadiationStr[16];
            formatFloat(dynRadiationStr, sizeof(dynRadiationStr), dynLevelSv, 7, 3);

            char statRadiationStr[16];
            formatFloat(statRadiationStr, sizeof(statRadiationStr), statLevelSv, 7, 3);
            display.setCursor(0, 20);
            display.setFont(MAIN_FONT);
            display.print("дин ");
            display.setFont(DIGIT_FONT);
            display.print(dynRadiationStr);
            display.setFont(MAIN_FONT);
            display.println(" мкЗв/ч");
            display.print("срд ");
            display.setFont(DIGIT_FONT);
            display.print(statRadiationStr);
            display.setFont(MAIN_FONT);
            display.println(" мкЗв/ч");
        }
        else
        {
            const float dynLevelR = controller->getDynamicRadiationLevel() / SV_TO_R_RATIO;
            const float statLevelR = controller->getStaticRadiationLevel() / SV_TO_R_RATIO;
            char dynRadiationStr[16];
            formatFloat(dynRadiationStr, sizeof(dynRadiationStr), dynLevelR, 7, 1);

            char statRadiationStr[16];
            formatFloat(statRadiationStr, sizeof(statRadiationStr), statLevelR, 7, 1);
            display.setCursor(0, 20);
            display.setFont(MAIN_FONT);
            display.print("дин ");
            display.setFont(DIGIT_FONT);
            display.print(dynRadiationStr);
            display.setFont(MAIN_FONT);
            display.println(" мкР/ч");
            display.print("срд ");
            display.setFont(DIGIT_FONT);
            display.print(statRadiationStr);
            display.setFont(MAIN_FONT);
            display.println(" мкР/ч");
        }
    }

    void Display::drawSound()
    {
        if (controller->getIsSoundOn())
        {
            // volume_sound
            display.drawBitmap(113, 1, image_volume_sound_bits, 14, 15, 1);
        }
        else
        {
            // volume_no_sound
            display.drawBitmap(112, 1, image_volume_no_sound_bits, 18, 16, 1);
        }
    }

    void Display::drawLight()
    {
        // Placeholder for drawing light icon
        if (controller->getIsLightOn())
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

    void Display::drawUnits()
    {
        if (controller->getIsUsingMicroSieverts())
        {
            // mkSv/h units
            display.drawBitmap(112, 32, image_unit_bits, 15, 15, 1);
        }
        else
        {
            // mkR/H units
            display.drawBitmap(112, 32, image_no_unit_bits, 15, 15, 1);
        }
    }

    void Display::drawBluetooth()
    {
        // Placeholder for drawing Bluetooth icon
        if (controller->getIsBluetoothOn())
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

    void Display::drawSplash()
    {
        // echo_splash
        display.drawBitmap(0, 0, image_echo_splash_bits, 128, 64, 1);
    }

    void Display::formatFloat(char *out,
                              size_t outSize,
                              float value,
                              uint8_t width,
                              uint8_t decimals)
    {
        char format[16];
        snprintf(format, sizeof(format), "%%%d.%df", width, decimals);
        snprintf(out, outSize, format, value);
    };
}