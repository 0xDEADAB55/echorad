#include <EchoController.h>
#include <math.h>

namespace EchoController
{

    void EchoController::Controller::tick()
    {
        _tick++;
        if (_tick % 200 == 0)
        {
            Serial.print("Raw dynamic uR/h: ");
            Serial.println(radSens->getRadIntensyDynamic());
            dynamicRadiationLevel = radSens->getRadIntensyDynamic() * SV_TO_R_RATIO;
            staticRadiationLevel = radSens->getRadIntensyStatic() * SV_TO_R_RATIO;
            updateUI();
        }
        if (_tick % 500 == 0)
        {
            batteryLevelVoltage = readBatteryVoltage();
            updateUI();
        }
    }

    bool EchoController::Controller::needUpdateUI()
    {
        auto needUpdate = this->needUIUpdate;
        needUIUpdate = false;
        return needUpdate;
    }

    void EchoController::Controller::updateUI()
    {
        needUIUpdate = true;
    }

    void EchoController::Controller::onLightButtonPressed()
    {
        isLightOn = !isLightOn;
        updateUI();
    }

    void EchoController::Controller::onSoundButtonPressed()
    {
        isSoundOn = !isSoundOn;
        updateUI();
    }

    void EchoController::Controller::onUnitsButtonPressed()
    {
        isUsingMicroSieverts = !isUsingMicroSieverts;
        updateUI();
    }

    void EchoController::Controller::onBluetoothButtonPressed()
    {
        isBluetoothOn = !isBluetoothOn;
        updateUI();
    }

    float EchoController::Controller::getBatteryLevelVoltage() const
    {
        return batteryLevelVoltage;
    }

    uint8_t EchoController::Controller::getBatteryLevelPercentage() const
    {
        const float zero_level = 3.0f;
        const float max_level = 4.2f;
        const uint8_t percentage = static_cast<uint8_t>(round(batteryLevelVoltage / (max_level - zero_level) * 100));
        if (percentage > 100)
        {
            return 100;
        }
        return percentage;
    }

    bool EchoController::Controller::getIsSoundOn() const
    {
        return isSoundOn;
    }

    float EchoController::Controller::getDynamicRadiationLevel() const
    {
        return dynamicRadiationLevel;
    }

    float EchoController::Controller::getStaticRadiationLevel() const
    {
        return EchoController::Controller::staticRadiationLevel;
    }

    bool EchoController::Controller::getIsUsingMicroSieverts() const
    {
        return isUsingMicroSieverts;
    }

    bool EchoController::Controller::getIsLightOn() const
    {
        return isLightOn;
    }

    bool EchoController::Controller::getIsBluetoothOn() const
    {
        return isBluetoothOn;
    }

    float EchoController::Controller::readBatteryVoltage()
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
        NRF_SAADC->ENABLE = 0;

        // Force explicit double-precision calculations
        double raw_double = (double)raw_value;
        double step1 = raw_double * 2.4;
        double step2 = step1 / 4095.0;
        double vddh = 5.0 * step2;

        return (float)vddh;
    }

}