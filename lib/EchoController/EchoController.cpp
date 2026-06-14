#include <EchoController.h>
#include <math.h>
#include <nrf52_erratas.h>

namespace EchoController
{
    void Controller::init()
    {
    }

    void Controller::tick()
    {

        if (_tick % 200 == 0)
        {
            dynamicRadiationLevel = radSens->getRadIntensyDynamic() * SV_TO_R_RATIO;
            staticRadiationLevel = radSens->getRadIntensyStatic() * SV_TO_R_RATIO;
            updateUI();
        }
        if (_tick % BATTERY_SAMPLING_PERIOD_TICKS == 0)
        {
            float batteryVoltage = readBatteryVoltage();
            addBatterySample(batteryVoltage);
            updateUI();
        }

        _tick++;
    }

    bool Controller::needUpdateUI()
    {
        auto needUpdate = this->needUIUpdate;
        needUIUpdate = false;
        return needUpdate;
    }

    void Controller::updateUI()
    {
        needUIUpdate = true;
    }

    void Controller::onLightButtonPressed()
    {
        uint32_t now = millis();
        if (now - lastLightPress < DEBOUNCE_MS)
            return;
        lastLightPress = now;

        isLightOn = !isLightOn;
        updateUI();
    }

    void Controller::onSoundButtonPressed()
    {
        uint32_t now = millis();
        if (now - lastSoundPress < DEBOUNCE_MS)
            return;
        lastSoundPress = now;

        soundMode = nextSoundMode(soundMode);
        updateUI();
    }

    void Controller::onUnitsButtonPressed()
    {
        uint32_t now = millis();
        if (now - lastUnitsPress < DEBOUNCE_MS)
            return;
        lastUnitsPress = now;

        isUsingMicroSieverts = !isUsingMicroSieverts;
        updateUI();
    }

    void Controller::onModeButtonPressed()
    {
        uint32_t now = millis();
        if (now - lastModePress < DEBOUNCE_MS)
            return;
        lastModePress = now;

        isModeStatic = !isModeStatic;
        updateUI();
    }

    float Controller::getBatteryLevelVoltage() const
    {
        return averageBatteryLevelVoltage;
    }

    uint8_t Controller::getBatteryLevelPercentage() const
    {
        const float zero_level = 3.0f;
        const float max_level = 4.2f;
        const uint8_t percentage = static_cast<uint8_t>(round((averageBatteryLevelVoltage - zero_level) / (max_level - zero_level) * 100));
        if (percentage > 100)
        {
            return 100;
        }
        return percentage;
    }

    SoundMode Controller::GetSoundMode() const
    {
        return soundMode;
    }

    float Controller::getDynamicRadiationLevel() const
    {
        return dynamicRadiationLevel;
    }

    float Controller::getStaticRadiationLevel() const
    {
        return Controller::staticRadiationLevel;
    }

    bool Controller::getIsUsingMicroSieverts() const
    {
        return isUsingMicroSieverts;
    }

    bool Controller::getIsLightOn() const
    {
        return isLightOn;
    }

    bool Controller::getIsModeObserveStatic() const
    {
        return isModeStatic;
    }

    void Controller::onGeigerPulseReceived()
    {
        switch (soundMode)
        {
        case SoundMode::NoSound:
            break;

        case SoundMode::Beep:
            buzzCallback(3000, 50);
            break;

        case SoundMode::Click:
            buzzCallback(3000, 3);
            break;

        default:
            break;
        }
        if (isLightOn)
            blinkCallback();
    }

    SoundMode Controller::nextSoundMode(SoundMode mode)
    {
        auto value = static_cast<uint8_t>(mode);

        value++;
        if (value >= static_cast<uint8_t>(SoundMode::_Count))
        {
            value = 0;
        }

        return static_cast<SoundMode>(value);
    }

    void Controller::addBatterySample(float voltage)
    {
        if (samplesFilled < BATTERY_SAMPLE_COUNT)
        {
            samples[sampleIndex] = voltage;
            sampleSum += voltage;
            samplesFilled++;
        }
        else
        {
            sampleSum -= samples[sampleIndex];
            samples[sampleIndex] = voltage;
            sampleSum += voltage;
        }

        sampleIndex++;
        if (sampleIndex >= BATTERY_SAMPLE_COUNT)
        {
            sampleIndex = 0;
        }

        averageBatteryLevelVoltage = sampleSum / samplesFilled;
    }

    uint32_t Controller::getUptimeSeconds() const
    {
        return millis() / 1000;
    }

    float Controller::readBatteryVoltage()
    {

        // The volatile keyword is a type qualifier in C/C++ that tells the compiler a variable's value might change in ways that the compiler cannot detect from the code alone.
        // Essentially, it says: "Don't optimize access to this variable because its value might change unexpectedly."
        volatile uint32_t raw_value = 0;
        // Configure SAADC
        NRF_SAADC->ENABLE = 1;
        NRF_SAADC->RESOLUTION = SAADC_RESOLUTION_VAL_12bit;

        NRF_SAADC->CH[0].CONFIG =
            (SAADC_CH_CONFIG_GAIN_Gain1_4 << SAADC_CH_CONFIG_GAIN_Pos) |
            (SAADC_CH_CONFIG_MODE_SE << SAADC_CH_CONFIG_MODE_Pos) |
            (SAADC_CH_CONFIG_REFSEL_Internal << SAADC_CH_CONFIG_REFSEL_Pos);

        NRF_SAADC->CH[0].PSELP = SAADC_CH_PSELP_PSELP_VDDHDIV5;
        NRF_SAADC->CH[0].PSELN = SAADC_CH_PSELN_PSELN_NC;

        // Sample
        NRF_SAADC->RESULT.PTR = (uint32_t)&raw_value;
        NRF_SAADC->RESULT.MAXCNT = 1;
        NRF_SAADC->TASKS_START = 1;
        while (!NRF_SAADC->EVENTS_STARTED)
            ;
        NRF_SAADC->EVENTS_STARTED = 0;
        NRF_SAADC->TASKS_SAMPLE = 1;
        while (!NRF_SAADC->EVENTS_END)
            ;

        NRF_SAADC->EVENTS_END = 0;
        NRF_SAADC->TASKS_STOP = 1;
        while (!NRF_SAADC->EVENTS_STOPPED)
            ;
        NRF_SAADC->EVENTS_STOPPED = 0;

        // Errata 212 workaround
        // https://github.com/NordicPlayground/nRF52-ADC-examples/blob/master/nrfx_saadc_simple_low_power_app_timer_multichannel_oversample/main.c
        if (nrf52_errata_212())
        {
            volatile uint32_t temp1 = *(volatile uint32_t *)0x40007640ul;
            volatile uint32_t temp2 = *(volatile uint32_t *)0x40007644ul;
            volatile uint32_t temp3 = *(volatile uint32_t *)0x40007648ul;
            *(volatile uint32_t *)0x40007FFCul = 0ul;
            *(volatile uint32_t *)0x40007FFCul;
            *(volatile uint32_t *)0x40007FFCul = 1ul;
            *(volatile uint32_t *)0x40007640ul = temp1;
            *(volatile uint32_t *)0x40007644ul = temp2;
            *(volatile uint32_t *)0x40007648ul = temp3;
        }

        NRF_SAADC->ENABLE = 0;

        // Force explicit double-precision calculations
        double raw_double = (double)raw_value;
        double step1 = raw_double * 2.4;
        double step2 = step1 / 4095.0;
        double vddh = 5.0 * step2;

        return (float)vddh;
    }
}