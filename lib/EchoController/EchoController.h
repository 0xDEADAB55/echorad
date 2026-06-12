#pragma once

#include <Arduino.h>
#include <CG_RadSens.h>

namespace EchoController
{

    static constexpr uint32_t RADIATION_MEASURE_INTERVAL_MS = 1000; // Measure radiation every 1000 milliseconds (1 second)
    static constexpr float SV_TO_R_RATIO = 0.00877f;                // Sievert to Roentgen ratio

    class Controller
    {
    public:
        explicit Controller(CG_RadSens *radSens) : radSens(radSens) {}

        void init();

        // Tick should be called 100 times per second
        void tick();

        void onLightButtonPressed();
        void onSoundButtonPressed();
        void onUnitsButtonPressed();
        void onBluetoothButtonPressed();

        // Returns whether the UI changed between two consecutive calls to needUpdateUI() (i.e. whether the display needs to be updated)
        bool needUpdateUI();

        // Always in µSv/h
        float getDynamicRadiationLevel() const;

        // Always in µSv/h
        float getStaticRadiationLevel() const;

        bool getIsSoundOn() const;
        bool getIsUsingMicroSieverts() const;
        bool getIsLightOn() const;
        bool getIsBluetoothOn() const;
        float getBatteryLevelVoltage() const;
        uint8_t getBatteryLevelPercentage() const;

    private:
        CG_RadSens *radSens;

        uint32_t lastMeasurementTime = 0;

        uint32_t _tick = 0;

        float dynamicRadiationLevel = 0;   // Micro Sieverts per hour (µSv/h)
        float staticRadiationLevel = 0;    // Micro Sieverts per hour (µSv/h)
        float batteryLevelVoltage = 0;     // Battery level as voltage
        bool isUsingMicroSieverts = false; // true for µSv/h, false for mR/h
        bool isSoundOn = true;
        bool isLightOn = true;
        bool isBluetoothOn = true;
        bool needUIUpdate = false;

        void measureBatteryLevel();
        void updateUI();

        static float readBatteryVoltage();
    };

}
