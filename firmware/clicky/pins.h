// Clicky — pin assignments.
// Ground truth taken from the KiCad schematic netlist (clicky.kicad_sch), Oct 2026.
// XIAO ESP32-C3: silkscreen D0..D10 map 1:1 to GPIO0..GPIO10.
//
// Connector pins (J2 = display header, 7-pin):
//   J2:1 VCC  -> 3V3     J2:5 CS   -> D3  (GPIO3)
//   J2:2 GND  -> GND     J2:6 DC   -> D6  (GPIO6)
//   J2:3 SCK  -> D8      J2:7 RST  -> 3V3 (tied high, no RESET pin)
//   J2:4 MOSI -> D10     (in J2 pin order: VCC GND SCK MOSI CS DC RST)

#pragma once

// ---------------------------------------------------------------------------
// Memory / GPIO (XIAO ESP32-C3, Seeed)
// ---------------------------------------------------------------------------
#define PIN_BUTTON      0    // D0  pet/touch button (active-low to GND)

// Microphone (MAX4466) analog out, J3:3 -> D1
#define PIN_MIC         1    // D1  = ADC1_CH1

// MPU-6000 (I2C) : SDA = D4 (GPIO4), SCL = D5 (GPIO5)
#define PIN_MPU_SDA     4
#define PIN_MPU_SCL     5

// DS18B20 (1-Wire) : DQ = D7 (GPIO7). NOTE: schematic pulls DQ to GND via R1;
// it should be a 4.7k pull-UP to 3V3. Flagged for a schematic fix.
#define PIN_TEMP        7    // D7

// Display (TFT_eSPI) pins live in the TFT_eSPI user setup — see
// firmware/TFT_eSPI_Setup/Clicky_UserSetup.h:
//   TFT_CS = 3 (D3), TFT_DC = 6 (D6), TFT_MOSI = 10 (D10), TFT_SCLK = 8 (D8)
