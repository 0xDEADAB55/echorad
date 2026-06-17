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

    void Display::printNumber(uint32_t number)
    {
        display.clearDisplay();
        display.setCursor(0, 10);
        display.setFont(DIGIT_FONT);
        display.print(number);
        display.display();
    }

    void Display::update()
    {
        frame++;
        if (controller->needUpdateUI())
        {
            repaint();
        }
    }

    void Display::repaint()
    {
        if (!controller->getIsDisplayOn())
        {
            turnOff();
            return;
        }
        turnOn();
        display.clearDisplay();
        drawBatteryLevel();
        drawUptime();
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
        if (percentage >= 20)
        {
            level = '1';
        }
        if (percentage >= 30)
        {
            level = '2';
        }
        if (percentage >= 60)
        {
            level = '3';
        }
        if (percentage >= 80)
        {
            level = '4';
        }

        display.print(level);
        if (controller->getIsBatteryCharging())
        {
            display.drawBitmap(17, 2, image_battery_charging_bits, 6, 10, 1);
            return;
        }
        display.setFont(DIGIT_FONT);
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

        if (controller->getIsModeObserveStatic())
        {
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
        auto soundMode = controller->GetSoundMode();

        switch (soundMode)
        {
        case EchoController::SoundMode::NoSound:
            display.drawBitmap(112, 1, image_volume_no_sound_bits, 18, 16, 1);
            break;

        case EchoController::SoundMode::Beep:
            display.drawBitmap(113, 1, image_volume_sound_bits, 14, 15, 1);
            break;

        case EchoController::SoundMode::Click:
            display.drawBitmap(113, 1, image_volume_click_bits, 13, 15, 1);
            break;

        default:
            display.drawBitmap(112, 1, image_volume_no_sound_bits, 18, 16, 1);
            break;
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

    void Display::turnOn()
    {
        display.ssd1306_command(SSD1306_DISPLAYON);
    }

    void Display::turnOff()
    {
        display.ssd1306_command(SSD1306_DISPLAYOFF);
    }

    void Display::drawUptime()
    {
        display.setFont(DIGIT_FONT);
        char uptimeStr[16];

        formatUptime(
            uptimeStr,
            sizeof(uptimeStr),
            controller->getUptimeSeconds());
        display.setCursor(58, 10);
        display.print(uptimeStr);
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

    void Display::formatUptime(
        char *out,
        size_t outSize,
        uint32_t uptimeSeconds)
    {
        const uint32_t days = uptimeSeconds / 86400;
        const uint32_t hours = (uptimeSeconds % 86400) / 3600;
        const uint32_t minutes = (uptimeSeconds % 3600) / 60;
        const uint32_t seconds = uptimeSeconds % 60;

        if (days > 0)
        {
            snprintf(out, outSize, "%luд%luч",
                     (unsigned long)days,
                     (unsigned long)hours);
        }
        else if (hours > 0)
        {
            snprintf(out, outSize, "%luч%luм",
                     (unsigned long)hours,
                     (unsigned long)minutes);
        }
        else if (minutes > 0)
        {
            snprintf(out, outSize, "%luм%luс",
                     (unsigned long)minutes,
                     (unsigned long)seconds);
        }
        else
        {
            snprintf(out, outSize, "%luс",
                     (unsigned long)seconds);
        }
    }
}
