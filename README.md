# Clicky

A portable, battery-powered personal "pet" — a tiny Tamagotchi-style device
with six MX switches, an OLED face, and a low-power brain that keeps track of
time and remembers its state even when it's been off for days.

## What it is

Clicky is a small handheld companion. You feed it, wash it, and play with it
through six mechanical switches; it shows its mood on a small OLED screen. It
knows when it last ate or was cleaned because it keeps a real clock running
even while asleep, and it stores its state in non-volatile memory. The whole
thing runs on a single LiPo battery and charges through the same USB-C port it
uses to program.

## Design goals

- **Portable and battery-powered** — one LiPo cell, no wall power needed.
- **At least a week of battery life** on a single charge.
- **Knows the time** — can tell "2 hours since last meal" or "a whole day
  since last wash" and react (hungry, dirty, happy).
- **Remembers its state** across power-off.
- **As simple as possible** — the fewest components that still do the job.

## Hardware

### Brain: Seeed XIAO nRF52840

The nRF52840 was chosen over the RP2040 for one decisive reason: power. It
sleeps at ~2 µA with its RTC still running, which is what makes a week of
battery life realistic. It also bundles everything the project needs on one
tiny board:

- **Built-in LiPo charger** (BQ25101) — no external charging IC required.
  Plug in USB-C to charge; unplug to run on battery. Automatic.
- **Internal low-power RTC** — keeps the clock alive in sleep, so Clicky
  always knows how much time has passed.
- **Onboard flash** — stores the pet's state (last feed, last wash, mood)
  without an external EEPROM.
- **BLE** — headroom for a future phone companion app.
- **USB-C** — programming and charging share the same port.

### Power

- Battery: a ~1000 mAh LiPo reused from a PS4 controller (has built-in
  protection). Connected to the XIAO's BAT+/BAT- pads via a JST-PH 2.0
  connector.
- Charging: handled by the XIAO's onboard charger at 50-100 mA (~10-12 h for
  a full charge — fine for overnight).
- A 0.1 F supercapacitor across the battery keeps the RTC alive if the cell
  ever fully drains.

### Input & display

- 6 MX switches in a 3x2 matrix with 1N4148 diodes (standard keyboard matrix).
  Switches draw ~0 mA at rest — they don't affect battery life.
- OLED 128x64 (SSD1306) over I2C — the pet's face. This is the biggest power
  consumer (~25 mA lit), so firmware lights it only during interaction.
- One tactile button to wake/sleep the device.

### Why no extra chips

Every "extra" a naive design would add is already inside the nRF52840:
charger, RTC, and flash. The final BOM is just the XIAO, the battery, the
OLED, six switches, six diodes, a button, a supercap, and a connector.

## Firmware

The logic is simple timestamp math, not live counting:

1. When you feed or wash Clicky, store the current time as a timestamp.
2. On wake, read the RTC, compute `delta = now - lastEvent`.
3. Map the delta to a state (e.g. >2 h since feed = hungry, >24 h since wash
   = dirty) and show it on the OLED.
4. Save state to flash, then sleep at ~2 µA until the button wakes it.

### Libraries (Arduino)

- Board package: **Seeed nRF52 Boards** (Arduino Boards Manager).
- Display: **Adafruit SSD1306** + **Adafruit GFX Library**.
- I2C: **Wire** (built-in).
- Time / sleep / flash: built into the Seeed BSP — no extra libraries.

## Wiring quick reference

- Battery + → XIAO BAT+ ; battery - → XIAO BAT- (via JST-PH 2.0).
- OLED: VCC → 3V3, GND → GND, SDA → D4 (P0.04), SCL → D5 (P0.05).
- Matrix: 3 row GPIOs (D0, D1, D2) + 2 column GPIOs (D3, D6), one diode per
  switch.
- Wake button: D7 → GND, interrupt-driven.
- Supercap 0.1 F between VBAT and GND.

## Bill of Materials

See [BOM.md](BOM.md) for the full parts list with prices (~$16 total, or
~$24 if the battery is bought new).

## Status

- [x] Requirements and architecture decided
- [x] BOM finalized
- [ ] Schematic updated from RP2040 to XIAO nRF52840 (in progress)
- [ ] PCB layout
- [ ] Firmware (state machine + RTC + sleep)
- [ ] Enclosure
