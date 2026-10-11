// ---------------------------------------------------------------------------
// TFT_eSPI User_Setup for Clicky — GC9A01 1.28" 240x240 round display
// on the Seeed XIAO ESP32-C3.
//
// HOW TO INSTALL (Arduino IDE):
//   1) Install the TFT_eSPI library (Library Manager).
//   2) Open in your file manager:
//        ~/Arduino/libraries/TFT_eSPI/User_Setup_Select.h
//   3) Comment out the line(s) that `#include <User_Setup.h>` and add:
//        #include <User_Setups/Clicky_UserSetup.h>
//   4) Copy this file into  ~/Arduino/libraries/TFT_eSPI/User_Setups/
//      (or edit the include path above accordingly).
//
// Pins come from the Clicky KiCad schematic:
//   SCK=D8 (GPIO8), MOSI=D10 (GPIO10), CS=D3 (GPIO3), DC=D6 (GPIO6).
//   RST is tied to 3V3 on the board (no reset pin), so TFT_RST = -1.
// ---------------------------------------------------------------------------

#define USER_SETUP_INFO "Clicky GC9A01 on XIAO ESP32-C3"

// --- Driver ----------------------------------------------------------------
#define GC9A01_DRIVER
#define TFT_WIDTH  240
#define TFT_HEIGHT 240

// --- SPI pins (XIAO ESP32-C3, from the schematic) --------------------------
#define TFT_SCLK  8   // D8  (CPU: SCK)
#define TFT_MOSI 10   // D10 (GPIO10), display MOSI
#define TFT_CS    3   // D3  (display CS)
#define TFT_DC    6   // D6  (display DC/RS)
#define TFT_RST  -1   // tied to 3V3 on the board

// --- SPI clock --------------------------------------------------------------
#define SPI_FREQUENCY       27000000
#define SPI_READ_FREQUENCY  20000000
#define SPI_TOUCH_FREQUENCY  2500000

// --- Fonts -----------------------------------------------------------------
#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_FONT8N
#define LOAD_GFXFF
