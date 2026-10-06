# Clicky — Bill of Materials (BOM)

A portable, battery-powered "pet" device: 6 MX switches, an OLED face, and a
low-power nRF52840 brain that keeps track of time and remembers its state.

Prices are indicative AliExpress/retail quotes gathered on 2026-10-06. Verify
the live listing price before ordering — AliExpress prices move daily. All
links returned HTTP 200 at the time of writing.

## Summary

| # | Component | Qty | Unit (USD) | Total (USD) | Purpose |
|---|-----------|-----|-----------|-------------|---------|
| 1 | Seeed XIAO nRF52840 | 1 | 9.00 | 9.00 | MCU: BLE, RTC, flash, built-in LiPo charger |
| 2 | LiPo battery (PS4 controller, ~1000 mAh) | 1 | 0.00* | 0.00* | Power source (reused from a DualShock 4) |
| 3 | OLED 0.96" 128x64 SSD1306 (I2C) | 1 | 2.00 | 2.00 | The pet's face / status display |
| 4 | Outemu Silent Lemon V3 (tactile, silent) | 10 | 0.30 | 3.00 | 6 switches + 4 spares |
| 5 | 1N4148 switching diode (DO-35) | 100 | 1.00 | 1.00 | Matrix diodes (6 used) |
| 6 | Tactile push button (6x6mm) | 1 | 0.10 | 0.10 | Wake / sleep button |
| 7 | Supercapacitor 0.1 F (5.5 V) | 1 | 0.50 | 0.50 | Keeps RTC alive if battery fully drains |
| 8 | JST-PH 2.0 female connector | 1 | 0.50 | 0.50 | Battery connector on the PCB |
| 9 | Resistor 4.7 kΩ (SMD 0603) | 2 | 0.05 | 0.10 | I2C pull-ups (if OLED module lacks them) |
|    | **TOTAL** | | | **~16.20** | |

\* Battery is reused from a PS4 controller you already own. If bought new, add
~$8.00.

## Notes

- **No external charger IC needed.** The XIAO nRF52840 has a built-in LiPo
  charger (BQ25101) with BAT+/BAT- pads on the underside. Plugging USB-C
  charges the battery; unplugging switches to battery automatically. Charge
  current is 50 mA (default) or 100 mA (set via firmware) — expect ~10-12 h
  to fully charge a 1000 mAh cell. Fine for overnight charging.
- **No external RTC chip needed.** The nRF52840 has an internal low-power RTC
  that keeps running in sleep (~2 µA). The supercap (item 7) preserves the
  clock if the battery ever fully drains.
- **No external EEPROM needed.** The XIAO's onboard flash stores the pet's
  state (last feed time, last wash time, mood).
- **Switches consume ~0 mA at rest** — they are mechanical contacts. Battery
  life is dominated by the OLED (~25 mA when lit), so the firmware should
  light the screen only when the pet is being interacted with.
- **Battery safety:** the PS4 cell has built-in protection (overcharge,
  over-discharge, short-circuit). Verify polarity when connecting (red = +,
  black = -).

## Wiring (quick reference)

- Battery + → XIAO BAT+ ; battery - → XIAO BAT- (via JST-PH 2.0 connector).
- OLED VCC → 3V3, GND → GND, SDA → D4 (P0.04), SCL → D5 (P0.05).
- 6 switches in a 3x2 matrix: 3 row GPIOs (e.g. D0, D1, D2) + 2 column GPIOs
  (e.g. D3, D6), one 1N4148 per switch (cathode to the switch).
- Wake button: one GPIO (e.g. D7) to GND, interrupt-driven.
- Supercap 0.1 F between VBAT and GND (parallel with battery).

## Firmware libraries (Arduino)

- Board package: "Seeed nRF52 Boards" (Arduino Boards Manager).
- Display: Adafruit SSD1306 + Adafruit GFX Library.
- I2C: Wire (built-in).
- Time / sleep / flash: built into the Seeed BSP — no extra libraries.
