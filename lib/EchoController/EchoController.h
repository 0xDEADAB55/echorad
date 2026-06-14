#pragma once

#include <Arduino.h>
#include <CG_RadSens.h>

namespace EchoController
{

    typedef void (*BlinkCallback)(void);
    typedef void (*BuzzerCallback)(uint32_t freq, uint32_t durationMs);

    static constexpr float SV_TO_R_RATIO = 0.00877f; // Sievert to Roentgen ratio

    enum class SoundMode : uint8_t
    {
        NoSound,
        Beep,
        Click,
        _Count
    };

    class Controller
    {
    public:
        explicit Controller(CG_RadSens *radSens, BuzzerCallback buzz, BlinkCallback blink) : radSens(radSens), buzzCallback(buzz), blinkCallback(blink) {}

        void init();

        // Tick should be called 100 times per second
        void tick();

        void onLightButtonPressed();
        void onSoundButtonPressed();
        void onUnitsButtonPressed();
        void onModeButtonPressed();
        void onGeigerPulseReceived();

        // Returns whether the UI changed between two consecutive calls to needUpdateUI() (i.e. whether the display needs to be updated)
        bool needUpdateUI();

        // Always in µSv/h
        float getDynamicRadiationLevel() const;

        // Always in µSv/h
        float getStaticRadiationLevel() const;

        SoundMode GetSoundMode() const;
        bool getIsUsingMicroSieverts() const;
        bool getIsLightOn() const;
        bool getIsModeObserveStatic() const;
        float getBatteryLevelVoltage() const;
        uint8_t getBatteryLevelPercentage() const;
        uint32_t getUptimeSeconds() const;

    private:
        CG_RadSens *radSens;
        BuzzerCallback buzzCallback;
        BlinkCallback blinkCallback;

        uint32_t lastMeasurementTime = 0;

        uint32_t _tick = 0;

        float dynamicRadiationLevel = 0;   // Micro Sieverts per hour (µSv/h)
        float staticRadiationLevel = 0;    // Micro Sieverts per hour (µSv/h)
        bool isUsingMicroSieverts = false; // true for µSv/h, false for mR/h
        SoundMode soundMode = SoundMode::Beep;
        bool isLightOn = true;
        bool isModeStatic = true;
        bool needUIUpdate = false;

        // Buttons and debounce
        static constexpr uint32_t DEBOUNCE_MS = 200;
        uint32_t lastSoundPress = 0;
        uint32_t lastLightPress = 0;
        uint32_t lastUnitsPress = 0;
        uint32_t lastModePress = 0;

        // Geiger impulse processing
        bool isGeigerPulseReceived = false;
        uint32_t geigerPulseReceivedAtTick = 0;

        // Battery
        float averageBatteryLevelVoltage = 0; // Battery level as voltage

        static constexpr uint16_t BATTERY_SAMPLE_COUNT = 100;
        static constexpr uint32_t BATTERY_SAMPLING_PERIOD_TICKS = 1000; // Battery sampling period
        float samples[BATTERY_SAMPLE_COUNT];
        float sampleSum = 0;

        uint8_t sampleIndex = 0;
        uint8_t samplesFilled = 0;
        void addBatterySample(float sample);
        static float readBatteryVoltage();
        static SoundMode nextSoundMode(SoundMode mode);
        // UI
        void updateUI();
    };

}
