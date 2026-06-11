#pragma once

#include <Arduino.h>
#include <CG_RadSens.h>

namespace EchoController
{

    static constexpr uint32_t RADIATION_MEASURE_INTERVAL_MS = 1000; // Measure radiation every 1000 milliseconds (1 second)

    class Controller
    {
    public:
        explicit Controller(CG_RadSens radSens) : radSens(radSens) {}

        void init();

        // Tick should be called 100 times per second
        void tick();

        void onLightButtonPressed();
        void onSoundButtonPressed();
        void onUnitsButtonPressed();
        void onBluetoothButtonPressed();

        uint32_t getDynamicRadiationLevel() const;
        uint32_t getStaticRadiationLevel() const;
        bool getIsSoundOn() const;
        bool getIsUsingMicroSieverts() const;
        bool getIsLightOn() const;
        bool getIsBluetoothOn() const;

    private:
        CG_RadSens radSens{RS_DEFAULT_I2C_ADDRESS};

        uint32_t lastMeasurementTime = 0;
        

        uint32_t dynamicRadiationLevel = 0; // Micro Sieverts per hour (µSv/h)
        uint32_t staticRadiationLevel = 0;  // Micro Sieverts per hour (µSv/h)

        bool isUsingMicroSieverts = false; // true for µSv/h, false for mR/h
        bool isSoundOn = true;
        bool isLightOn = true;
        bool isBluetoothOn = true;
        void measureBatteryLevel();

        void updateUI();
    };
}
