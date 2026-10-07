# Clicky — Bill of Materials (BOM)

A portable, battery-powered heart-shaped "pet" in the style of STΛRBOY /
Lil Guy: a round animated eye, a personality driven by sensors (shake, cold,
sound), a touch button to pet it, and a long-life LiPo battery that charges
through the same USB-C port used to program it.

Prices are indicative quotes gathered on 2026-10-06, based on the verified
starboy-diy BOM and adapted to a single unit. Verify the live listing price
before ordering — AliExpress prices move daily.

## Summary

| # | Component | Qty | Unit (USD) | Total (USD) | Purpose |
|---|-----------|-----|-----------|-------------|---------|
| 1 | Seeed XIAO ESP32-C3 | 1 | 4.99 | 4.99 | MCU: WiFi/BLE, built-in LiPo charger |
| 2 | GC9A01 1.28" round TFT 240x240 (SPI) | 1 | 4.48 | 4.48 | The pet's animated eye |
| 3 | MPU6050 (GY-521) accel/gyro module | 1 | 1.88 | 1.88 | Shake + tilt sensing |
| 4 | DS18B20 TO-92 temperature sensor | 1 | 0.58 | 0.58 | Cold detection (shiver/freeze) |
| 5 | MAX4466 electret mic amplifier | 1 | 1.74 | 1.74 | Loud-sound detection |
| 6 | LiPo 402535 3.7V 320mAh (protected) | 1 | 9.99 | 9.99 | Power source |
| 7 | Tactile push button (6x6mm) | 1 | 0.10 | 0.10 | "Pet" / wake button |
| 8 | Resistor 4.7 kΩ (through-hole) | 2 | 0.01 | 0.02 | DS18B20 OneWire pull-up |
| 9 | JST-PH 2.0 female connector | 1 | 0.50 | 0.50 | Battery connector on the PCB |
| 10 | 30 AWG silicone wire (5 colors) | 1 | 6.96 | 6.96 | Internal wiring |
| 11 | Prototype board 5x7cm 1.2mm | 1 | 0.43 | 0.43 | Solder modules onto (cut to circle) |
| 12 | Spring carabiner keyrings (10) | 1 | 0.30 | 0.30 | Clip to pants/bag |
| 13 | Stainless split rings 25mm (10) | 1 | 0.15 | 0.15 | Through the keyring hole |
| 14 | PETG filament 1.75mm 1kg | 1 | 14.81 | 14.81 | 3D-printed heart shell |
| 15 | Plastic primer + chrome spray | 1 | 13.44 | 13.44 | Chrome finish (optional) |
|    | **TOTAL (parts)** | | | **~60.37** | |

## Notes

- **No external charger IC needed.** The XIAO ESP32-C3 has a built-in LiPo
  charger (380 mA fast / 40 mA trickle) with BAT+/BAT- pads on the underside.
  Plugging USB-C charges the battery; unplugging switches to battery
  automatically. The battery's own protection circuit handles over-charge,
  over-discharge and shorts.
- **Battery safety:** buy a certified/protected cell (UL or with a protection
  circuit), not a cheap uncertified one — it's lithium worn on a belt loop.
  Verify polarity with a multimeter before soldering (red = +, black = -).
- **The DS18B20 vent must be a through-hole to outside air.** If sealed
  inside, it reads the board's own heat and the cold/shiver behaviour never
  triggers.
- **Backlight pin (BL) is PWM-driven** (D2/GPIO4) so the firmware can dim it
  when the pet sleeps — do NOT tie it to 3.3V, or it wastes battery asleep.
- **GPIO2 and GPIO9 are left free** — they are boot-mode strapping pins.
- **Depth budget is tight:** XIAO (4.5) + GY-521 (3.0) + battery (4.3) =
  11.8 mm into 12.0 mm available. Measure the battery and GY-521 when they
  arrive and dry-fit before gluing.

## Wiring (quick reference)

- Battery + → XIAO BAT+ ; battery - → XIAO BAT- (via JST-PH 2.0 connector).
- GC9A01 (SPI): VCC→3V3, GND→GND, SCK→D8 (GPIO8), SDA/MOSI→D10 (GPIO10),
  DC→D6 (GPIO21), CS→D3 (GPIO5), RST→3V3, BL→D2 (GPIO4, PWM).
- MPU6050 (I2C): VCC→3V3, GND→GND, SDA→D4 (GPIO6), SCL→D5 (GPIO7), AD0→GND.
- DS18B20 (OneWire): VDD→3V3, GND→GND, DATA→D7 (GPIO20) + 4.7kΩ pull-up
  between DATA and VDD.
- MAX4466 (analog): VCC→3V3, GND→GND, OUT→D1 (GPIO3).
- Pet button: one GPIO (e.g. D0/GPIO0) to GND, interrupt-driven.

## Firmware libraries (Arduino)

- Board: **XIAO_ESP32C3** (ESP32 core), USB CDC On Boot: Enabled.
- Display: **TFT_eSPI** (Bodmer) — copy the project's `User_Setup.h` into its
  library folder (GC9A01, 240x240, exact SPI pins).
- Motion: **Adafruit MPU6050** + **Adafruit Unified Sensor**.
- Temperature: **DallasTemperature** + **OneWire**.
- Mic: read via ADC directly (no library needed).
