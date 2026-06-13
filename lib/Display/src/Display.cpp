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
        drawMode();
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
            formatFloat(dynRadiationStr, sizeof(dynRadiationStr), dynLevelSv, 6, 3);

            char statRadiationStr[16];
            formatFloat(statRadiationStr, sizeof(statRadiationStr), statLevelSv, 6, 3);
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

        float observedLevelSv = dynLevelSv;

        if (controller->getIsModeObserveStatic()) {
            observedLevelSv = statLevelSv;
        }

        const char *status;
        if (observedLevelSv < 0.3f)
            status = "НОРМ.ФОН";
        else if (observedLevelSv < 1.0f)
            status = "НОРМ+ФОН";
        else if (observedLevelSv < 3.0f)
            status = "ПОВЫШЕН";
        else if (observedLevelSv < 10.0f)
            status = "ВЫСОКИЙ";
        else if (observedLevelSv < 100.0f)
            status = "ОПАСНОСТЬ";
        else
            status = "АВАРИЯ";

        display.setCursor(0, 55);
        display.setFont(LARGE_FONT);
        display.println(status);
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

    void Display::drawMode()
    {
        // Placeholder for drawing Bluetooth icon
        if (controller->getIsModeObserveStatic())
        {
            // static mode
            display.drawBitmap(112, 47, image_mode_stat_bits, 15, 15, 1);
        }
        else
        {
            // dynamic mode
            display.drawBitmap(112, 47, image_mode_dyn_bits, 15, 15, 1);
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
        // Count digits in the whole part
        uint8_t wholeDigits = 1; // at least 1
        long whole = (long)abs(value);
        while (whole >= 10)
        {
            whole /= 10;
            wholeDigits++;
        }
        // Account for minus sign
        if (value < 0)
            wholeDigits++;

        // dot takes 1 character, so available space for decimals is:
        // width - wholeDigits - 1 (for the dot)
        int availableDecimals = (int)width - (int)wholeDigits - 1;

        // Clamp: don't exceed requested decimals, don't go below 0
        if (availableDecimals < 0)
            availableDecimals = 0;
        if (availableDecimals > decimals)
            availableDecimals = decimals;

        // If no room for decimals, skip the dot too
        if (availableDecimals == 0)
            snprintf(out, outSize, "%*ld", width, (long)value);
        else
            snprintf(out, outSize, "%*.*f", width, availableDecimals, value);
    }
}
