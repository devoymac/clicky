/*
 * Clicky — a battery-powered heart "pet" with a round animated eye.
 *
 * Board      : Seeed XIAO ESP32-C3  (Arduino: "Seeed XIAO ESP32C3")
 * Display    : GC9A01 1.28" 240x240 round (TFT_eSPI)
 * Sensors    : MPU-6000 (I2C), DS18B20 (1-Wire), MAX4466 mic (ADC), pet button
 *
 * Open this whole "firmware/clicky" folder in Arduino IDE.
 * See firmware/SETUP-arduino-ide.md for board + library setup.
 */
#include <TFT_eSPI.h>   // requires the Clicky User_Setup (see SETUP doc)
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include "pins.h"
#include "moods.h"

/* ----------------------------- Displays ---------------------------------- */
enum EyeStyle : uint8_t { EYE_OPEN = 0, EYE_DIZZY, EYE_HAPPY, EYE_SHUT };

// Pupil/sclera geometry around the 240x240 round panel.
#define EYE_CENTRE  120
#define EYE_SCLERA  45
#define EYE_PUPIL   24

TFT_eSPI tft;
Mood     mood = MOOD_IDLE;
bool     screenAsleep = false;

/* ------------------------------ Sensors ---------------------------------- */
Adafruit_MPU6050 mpu;
sensors_event_t  accel, gyro, temp;

OneWire          ow(PIN_TEMP);
DallasTemperature ds18b20(&ow);

uint16_t micMax = 0;     // peak ADC since last loop

/* ------------------------------- Timing ---------------------------------- */
volatile bool petPending   = false;  // set by ISR, consumed in loop()
uint32_t      lastInteraction = 0;
uint32_t      lastBlink       = 0;
bool          eyesOpen        = true;

/* ----------------------------- Prototypes -------------------------------- */
void readSensors();
Mood   decideMood();
void   applyMood(Mood m);
void   drawFace();
void   drawEye(EyeStyle style);
void   IRAM_ATTR onButton();

/* ========================================================================= */
void setup() {
    Serial.begin(115200);

    // ---- Display ----
    tft.init();
    drawEye(EYE_OPEN);

    // ---- Pet button (active-low, internal pull-up) ----
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), onButton, FALLING);

    // ---- I2C MPU-6000 on the schematic pins (SDA=D4, SCL=D5) ----
    Wire.begin(PIN_MPU_SDA, PIN_MPU_SCL);
    if (mpu.begin(0x68)) {
        mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
        mpu.setGyroRange(MPU6050_RANGE_500_DEG);
        mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
        Serial.println("MPU-6000 OK");
    } else {
        Serial.println("MPU-6000 not found (check SDA/SCL wiring)");
    }

    // ---- DS18B20 ----
    ds18b20.begin();
    Serial.print("DS18B20 devices: ");
    Serial.println(ds18b20.getDeviceCount());

    lastInteraction = millis();
}

void loop() {
    if (petPending) {                      // pet = wake + blush
        petPending      = false;
        lastInteraction = millis();
        if (screenAsleep) {
            wakeScreen();
        }
        mood = MOOD_BLUSH;
    }

    readSensors();
    applyMood(decideMood());
    drawFace();
    micMax = 0;                            // reset mic peak each loop
    delay(30);
}

/* ============================== Sensors ================================== */

void readSensors() {
    mpu.getEvent(&accel, &gyro, &temp);    // ignored if MPU absent
    micMax = max(micMax, (uint16_t)analogRead(PIN_MIC));
}

/* ============================== Mood ===================================== */

Mood decideMood() {
    uint32_t now = millis();

    // No interaction for a long time -> sleep (unless we just got petted).
    if (now - lastInteraction >= MS_SLEEP_TIMEOUT) return MOOD_SLEEP;

    // Lingering blush right after a pet overrides sensor moods.
    if (now - lastInteraction < MS_BLUSH_LINGER)   return MOOD_BLUSH;

    // Shake.
    float a = sqrt(accel.acceleration.x * accel.acceleration.x +
                   accel.acceleration.y * accel.acceleration.y +
                   accel.acceleration.z * accel.acceleration.z);
    if (a > SHAKE_THRESHOLD) return MOOD_DIZZY;

    // Cold.
    if (ds18b20.getDeviceCount() > 0) {
        ds18b20.requestTemperatures();     // (throttle in a real implementation)
        if (ds18b20.getTempCByIndex(0) < COLD_THRESHOLD_C) return MOOD_COLD;
    }

    // Loud noise.
    if (micMax >= LOUD_THRESHOLD) return MOOD_STARTLED;

    return MOOD_IDLE;
}

void applyMood(Mood m) {
    if (m == mood) return;

    if (m == MOOD_SLEEP) {
        // Sleep now: blank + panel sleep-in (no backlight pin to save power).
        tft.fillScreen(TFT_BLACK);
        tft.writecommand(0x10);            // Display OFF / sleep in (GC9A01)
        screenAsleep = true;
        mood = MOOD_SLEEP;
        return;
    }

    if (screenAsleep) wakeScreen();        // leaving sleep
    mood = m;
}

void wakeScreen() {
    tft.writecommand(0x11);                // sleep out
    delay(120);
    tft.init();
    screenAsleep = false;
}

/* ============================== Rendering ================================ */

void drawFace() {
    bool happy = (mood == MOOD_BLUSH);

    EyeStyle style;
    switch (mood) {
        case MOOD_DIZZY:    style = EYE_DIZZY; break;
        case MOOD_COLD:
        case MOOD_SLEEP:    style = EYE_SHUT;  break;
        case MOOD_BLUSH:    style = EYE_HAPPY; break;
        case MOOD_STARTLED: style = EYE_OPEN;  break;
        case MOOD_IDLE:
        default:
            if (millis() - lastBlink > 3000) {
                eyesOpen = !eyesOpen;
                lastBlink = millis();
            }
            style = eyesOpen ? EYE_OPEN : EYE_SHUT;
    }
    drawEye(style);

    // Blush: a soft pink ring just outside the sclera when happy.
    if (happy) {
        tft.drawCircle(EYE_CENTRE, EYE_CENTRE, EYE_SCLERA + 10,
                       tft.color565(240, 110, 130));
        tft.drawCircle(EYE_CENTRE, EYE_CENTRE, EYE_SCLERA + 13,
                       tft.color565(220, 90, 110));
    }
}

void drawEye(EyeStyle style) {
    uint16_t c = EYE_CENTRE;
    tft.fillScreen(TFT_BLACK);

    switch (style) {
        case EYE_OPEN:                       // calm, looking straight
            tft.fillCircle(c, c, EYE_SCLERA, TFT_WHITE);
            tft.fillCircle(c, c, EYE_PUPIL, TFT_BLACK);
            tft.fillCircle(c + 12, c - 14, 7, TFT_WHITE);   // glint
            break;

        case EYE_DIZZY:                      // spiral
            for (int i = 0; i < 6; i++)
                tft.drawCircle(c, c, 8 + i * 7, TFT_WHITE);
            break;

        case EYE_HAPPY:                      // up-curving happy pupil
            tft.fillCircle(c, c, EYE_SCLERA, TFT_WHITE);
            tft.fillCircle(c, c, EYE_PUPIL, TFT_BLACK);
            tft.fillRect(c - EYE_PUPIL, c - EYE_PUPIL, EYE_PUPIL * 2, EYE_PUPIL, TFT_WHITE);
            tft.fillCircle(c, c, EYE_PUPIL - 6, TFT_BLACK);
            tft.fillCircle(c + 12, c - 14, 7, TFT_WHITE);
            break;

        case EYE_SHUT:                       // gentle line (asleep / cold)
        default:
            tft.drawLine(c - 34, c, c + 34, c, TFT_WHITE);
            break;
    }
}

/* ============================== Button ISR =============================== */

void IRAM_ATTR onButton() {
    petPending = true;                     // defer all millis()/SPI to loop()
}
