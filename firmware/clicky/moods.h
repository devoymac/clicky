// Clicky — mood / personality state machine.
// Each mood renders a different expression on the round GC9A01 eye and
// changes the display brightness (full vs. dimmed) until it sleeps.

#pragma once

#include <stdint.h>

enum Mood : uint8_t {
    MOOD_IDLE = 0,      // calm, blinking slowly
    MOOD_DIZZY,         // was shaken
    MOOD_COLD,          // DS18B20 below threshold
    MOOD_STARTLED,      // loud noise from the mic
    MOOD_BLUSH,         // just petted
    MOOD_SLEEP,         // no interaction for a long time (display off)
    MOOD_COUNT,
};

// Trigger thresholds / timing (tune freely)
constexpr uint16_t SHAKE_THRESHOLD   = 2500;   // |accel| delta units
constexpr float    COLD_THRESHOLD_C  = 18.0f;  // below this: shiver / freeze
constexpr uint16_t LOUD_THRESHOLD    = 2000;   // microphone ADC reading

constexpr uint32_t MS_BLUSH_LINGER   = 4000;   // keep blushing after a pet
constexpr uint32_t MS_DOZE_TIMEOUT   = 60000;  // no interaction -> doze (dim)
constexpr uint32_t MS_SLEEP_TIMEOUT  = 300000; // no interaction -> sleep (off)

static inline const char* moodName(Mood m) {
    switch (m) {
        case MOOD_IDLE:    return "idle";
        case MOOD_DIZZY:   return "dizzy";
        case MOOD_COLD:    return "cold";
        case MOOD_STARTLED:return "startled";
        case MOOD_BLUSH:   return "blush";
        case MOOD_SLEEP:   return "sleep";
        default:           return "?";
    }
}
