# Clicky firmware — Arduino IDE setup

Sets up and builds the Clicky firmware for the **Seeed XIAO ESP32-C3**.

## 1. Install Arduino IDE

Download from https://www.arduino.cc/en/software (or your package manager).

## 2. Add the ESP32 board support

In Arduino IDE: `File → Preferences → Additional Boards Manager URLs` add:

```
https://espressif.github.io/arduino-esp32/package_esp32_index.json
```

Then `Tools → Board → Boards Manager`, search **esp32** and install
**esp32 by Espressif Systems**.

## 3. Choose the board

`Tools → Board → ESP32 Arduino → Seeed XIAO ESP32C3`.

Leave the other settings at their defaults (`USB CDC On Boot: Enabled` is fine —
it lets the serial monitor work over USB-C).

## 4. Install the libraries

Library Manager (`Tools → Manage Libraries…`), install:

| Library                      | Provides                     |
|------------------------------|------------------------------|
| **TFT_eSPI** by Bodmer       | GC9A01 round display driver  |
| **Adafruit MPU6050**         | MPU-6000/6050 motion sensor  |
| **Adafruit Unified Sensor**  | dependency of MPU6050        |
| **OneWire** by Paul Stoffregen | DS18B20 1-Wire bus         |
| **DallasTemperature**        | DS18B20 temperature          |

## 5. Configure TFT_eSPI for the GC9A01

1. Copy `firmware/TFT_eSPI_Setup/Clicky_UserSetup.h` into
   `~/Arduino/libraries/TFT_eSPI/User_Setups/`.
2. Edit `~/Arduino/libraries/TFT_eSPI/User_Setup_Select.h`,
   comment out the default `#include <User_Setup.h>` line and add:

   ```c
   #include <User_Setups/Clicky_UserSetup.h>
   ```

This points TFT_eSPI at the GC9A01 driver with the board's real pins:
SCK=D8, MOSI=D10, CS=D3, DC=D6, RST=3V3 (no reset pin).

## 6. Open and flash the sketch

1. `File → Open…` and pick the **firmware/clicky** folder (open `clicky.ino`).
2. With the XIAO plugged in via USB-C, under `Tools → Port` select the board.
3. `Sketch → Upload`. Open the Serial Monitor (115200 baud) — you should see
   `MPU-6000 OK` and `DS18B20 devices: 1` if the sensors are wired up.

## Wiring quick reference (matches the schematic)

| Function  | Signal   | XIAO pin |
|-----------|----------|----------|
| Display   | SCK      | D8       |
| Display   | MOSI     | D10      |
| Display   | CS       | D3       |
| Display   | DC       | D6       |
| Display   | RST      | 3V3      |
| MPU-6000  | SDA      | D4       |
| MPU-6000  | SCL      | D5       |
| DS18B20   | DQ       | D7       |
| Mic       | OUT      | D1 (ADC) |
| Pet btn   | button   | D0       |

## Known schematic issue

R1 (4.7k for the DS18B20) currently connects DQ to **GND**. For the DS18B20 to
work it must be a pull-UP to **3V3**. Fix this in KiCad before relying on the
temperature mood — otherwise cold-detection won't read correctly.

## Tuning

All mood thresholds/timers are `constexpr` at the top of `moods.h`
(shake, cold, loud, doze/sleep timeouts). Tweak there.
