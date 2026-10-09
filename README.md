# Clicky

A portable, battery-powered heart-shaped "pet" in the style of STΛRBOY / Lil
Guy: a round animated eye, a personality driven by sensors, and a touch button
to pet it — it blushes when you give it affection. It runs on a single LiPo
battery and charges through the same USB-C port it uses to program.

## What it is

Clicky is a small companion you wear on your clothes or bag. Inside a
3D-printed heart lives a round screen that acts as its eye, and a set of
sensors give it a personality: it gets dizzy when you shake it, shivers when
it's cold, startles at loud noises, and falls asleep if you ignore it. Touch
it and it wakes up happy. It remembers its state across power-off.

## Design goals

- **Portable and battery-powered** — one small LiPo cell, no wall power.
- **Long battery life** — low-power sleeps, dimmed display when drowsy.
- **A living personality** — reacts to shake, cold, sound, and touch.
- **An original touch** — the heart blushes when you pet it.
- **Built by hand** — hand-wired to a tiny module, no complex PCB.

## Hardware

### Brain: Seeed XIAO ESP32-C3

A thumb-sized module with a built-in LiPo charger, WiFi/BLE, and plenty of
GPIOs, chosen over the nRF52840 for its lower cost and simplicity:

- **Built-in LiPo charger** — no external charging IC required. Plug in USB-C
  to charge; unplug to run on battery. Automatic.
- **WiFi + BLE** — headroom for a future phone companion or notifications.
- **USB-C** — programming and charging share the same port.
- **Low-power sleep** — dozes in µA range, waking on the touch button.

### The eye: GC9A01 1.28" round display

A 240x240 round SPI display that shows Clicky's expressions. Note: the
chosen module has no separate backlight pin — the backlight is tied to 3V3
internally, so the firmware should put the display to sleep instead of just
dimming it, to save battery when it dozes.

### The personality (sensors)

- **MPU-6000** (motion) — shake and tilt: dizzy, angry, look-downhill.
- **DS18B20** (temperature) — cold: shiver, then freeze with a blue tint.
- **MAX4466** (microphone amp) — loud sounds: startled, nervous.

### Interaction

- One tactile push button — pet the heart, it blushes and wakes up.

### Power

- A small certified LiPo (~320 mAh, e.g. 402535) with built-in protection,
  charged through the XIAO's own USB-C. Verify polarity before soldering
  (red = +, black = -).

## Schematic (current state)

The KiCad schematic is complete and validated by ERC. All nets are connected:
power (3V3/GND), I2C (MPU), 1-Wire (DS18B20 with 4.7k pull-up), SPI (display),
the microphone, the pet button, and the battery (VBAT). The remaining ERC
notices are intentional free pins and CLI false positives.

Note: some component footprints still need fixing before moving to PCB
(R1, J1, SW1, J2, J3, C1, C2 use default/placeholder footprints).

## Wiring quick reference

- Battery + → XIAO VBAT ; battery - → GND (via JST-PH 2.0).
- Display GC9A01 (7-pin): VCC→3V3, GND→GND, SCL→D8 (SCK), SDA→D10 (MOSI),
  DC→D6, CS→D3, RST→3V3.
- MPU-6000 (I2C): VDD→3V3, GND→GND, SDA→D4, SCL→D5, AD0→GND, CPOUT→100nF
  to GND, REGOUT→100nF to GND.
- DS18B20: VDD→3V3, GND→GND, DQ→D6 + 4.7kΩ pull-up to 3V3.
- Mic MAX4466: VCC→3V3, GND→GND, OUT→D1.
- Pet button: one pin→D0, other→GND.

## Firmware (planned)

Sensor-driven moods driven by simple timestamps (last interaction, timeout to
doze/sleep) plus sensor triggers. Libraries: TFT_eSPI (display), Adafruit
MPU6050 + Unified Sensor, DallasTemperature + OneWire, ADC for the mic.

## Bill of Materials

See [BOM.md](BOM.md) for the full parts list with prices (~$60 parts).

## Status

- [x] Architecture decided (heart-shaped pet, ESP32-C3, sensors + touch)
- [x] BOM finalized (~$60)
- [x] Schematic wired and ERC-validated
- [ ] Fix remaining component footprints (R1, J1, SW1, J2, J3, C1, C2)
- [ ] PCB layout
- [ ] 3D heart shell (OpenSCAD)
- [ ] Firmware
