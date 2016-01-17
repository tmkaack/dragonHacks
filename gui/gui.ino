/***************************************************
  This is our touchscreen painting example for the Adafruit ILI9341
  captouch shield
  ----> http://www.adafruit.com/products/1947

  Check out the links above for our tutorials and wiring diagrams

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/


#include <Adafruit_GFX.h>    // Core graphics library
#include <SPI.h>       // this is needed for display
#include <Adafruit_ILI9341.h>
#include <Wire.h>      // this is needed for FT6206
#include <Adafruit_FT6206.h>

#include <Adafruit_TFTLCD.h> // Hardware-specific library

// The FT6206 uses hardware I2C (SCL/SDA)
Adafruit_FT6206 ctp = Adafruit_FT6206();

// The display also uses hardware SPI, plus #9 & #10
#define TFT_CS 10
#define TFT_DC 9
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GRAY    0x7777

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

// Size of the color selection boxes and the paintbrush size
#define BOXSIZE 40
#define PENRADIUS 3
int oldcolor, currentcolor;

void setup(void) {
  while (!Serial);     // used for leonardo debugging
 
  Serial.begin(115200);
  Serial.println(F("Cap Touch Paint!"));
  
    Serial.println(F("TFT LCD test"));

#ifdef USE_ADAFRUIT_SHIELD_PINOUT
  Serial.println(F("Using Adafruit 2.8\" TFT Arduino Shield Pinout"));
#else
  Serial.println(F("Using Adafruit 2.8\" TFT Breakout Board Pinout"));
#endif

  Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());

  tft.reset();

  uint16_t identifier = tft.readID();

  if(identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if(identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if(identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Adafruit 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_ADAFRUIT_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Adafruit_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    return;
  }

  tft.begin(identifier);
  tft.setRotation(1);
  
  
  //tft.begin();

  if (! ctp.begin(40)) {  // pass in 'sensitivity' coefficient
    Serial.println("Couldn't start FT6206 touchscreen controller");
    while (1);
  }

  Serial.println("Capacitive touchscreen started");
  drawMain();
//  tft.fillScreen(BLACK);
//  
//  // make the color selection boxes
//  tft.fillRect(0, 0, BOXSIZE, BOXSIZE, RED);
//  tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, YELLOW);
//  tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, GREEN);
//  tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, CYAN);
//  tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, BLUE);
//  tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, MAGENTA);
// 
//  // select the current color 'red'
//  tft.drawRect(0, 0, BOXSIZE, BOXSIZE, WHITE);
//  currentcolor = RED;
}

void loop() {
//  // Wait for a touch
//  if (! ctp.touched()) {
//    return;
//  }
//
//  // Retrieve a point  
//  TS_Point p = ctp.getPoint();
//  
// /*
//  // Print out raw data from screen touch controller
//  Serial.print("X = "); Serial.print(p.x);
//  Serial.print("\tY = "); Serial.print(p.y);
//  Serial.print(" -> ");
// */
//
//  // flip it around to match the screen.
//  TS_Point p2 = p;
//  p.y = map(p2.x, 0, 240, 240, 0);
//  p.x = map(p2.y, 0, 320, 0, 320);
//
//  // Print out the remapped (rotated) coordinates
//  Serial.print("("); Serial.print(p.x);
//  Serial.print(", "); Serial.print(p.y);
//  Serial.println(")");
//  
//
//  if (p.y < BOXSIZE) {
//     oldcolor = currentcolor;
//
//     if (p.x < BOXSIZE) { 
//       currentcolor = RED; 
//       tft.drawRect(0, 0, BOXSIZE, BOXSIZE, WHITE);
//     } else if (p.x < BOXSIZE*2) {
//       currentcolor = YELLOW;
//       tft.drawRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, WHITE);
//     } else if (p.x < BOXSIZE*3) {
//       currentcolor = GREEN;
//       tft.drawRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, WHITE);
//     } else if (p.x < BOXSIZE*4) {
//       currentcolor = CYAN;
//       tft.drawRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, WHITE);
//     } else if (p.x < BOXSIZE*5) {
//       currentcolor = BLUE;
//       tft.drawRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, WHITE);
//     } else if (p.x <= BOXSIZE*6) {
//       currentcolor = MAGENTA;
//       tft.drawRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, WHITE);
//     }
//
//     if (oldcolor != currentcolor) {
//        if (oldcolor == RED) 
//          tft.fillRect(0, 0, BOXSIZE, BOXSIZE, RED);
//        if (oldcolor == YELLOW) 
//          tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, YELLOW);
//        if (oldcolor == GREEN) 
//          tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, GREEN);
//        if (oldcolor == CYAN) 
//          tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, CYAN);
//        if (oldcolor == BLUE) 
//          tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, BLUE);
//        if (oldcolor == MAGENTA) 
//          tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, MAGENTA);
//     }
//  }
//  if (((p.y-PENRADIUS) > BOXSIZE) && ((p.y+PENRADIUS) < tft.height())) {
//    tft.fillCircle(p.x, p.y, PENRADIUS, currentcolor);
//  }
}

void drawMain(){
  tft.fillScreen(BLACK);
  tft.setCursor(10, 10);
  tft.setTextColor(WHITE); tft.setTextSize(3);
  tft.print("IOB: ");
  tft.setCursor(10, 40);
  tft.print("CBG: ");
  tft.setCursor(200, 10);
  tft.print("TIME: ");
  tft.fillRect(10, 100, 140, 50, GRAY);
  tft.setCursor(35, 115);
  tft.print("BOLUS");
  tft.fillRect(170, 100, 140, 50, GRAY);
  tft.setCursor(180, 115);
  tft.print("Options");
}
