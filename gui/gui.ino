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
#define MAIN 0
#define BOLUS 1
#define OPTIONS 2


int oldcolor, currentcolor, scene;
int currentLevel = 0, button = 0;
String levelString = "";

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
  drawBolus();
  scene = 1;
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
  if (!ctp.touched()){
    return;
  }
  TS_Point p = ctp.getPoint();
  // Print out raw data from screen touch controller
  Serial.print("X = "); Serial.print(p.x);
  Serial.print("\tY = "); Serial.println(p.y); 

  if(scene == MAIN){
    if(p.y > 10 && p.y < 150 && p.x > 100 && p.x < 150){
      scene = BOLUS;
      drawBolus();
      return;
    }
    else if(p.y > 170 && p.y < 310 && p.x > 100 && p.x < 150){
      scene = OPTIONS;
      drawOptions();
      return;
    }
  }
  else if(scene == OPTIONS){
    if(p.y > 10 && p.y < 80 && p.x > 190 && p.x < 230){
      scene = MAIN;
      drawMain();
      return;
    }
  }
  else if(scene == BOLUS){
    if(p.y > 10 && p.y < 80 && p.x > 190 && p.x < 230){
      scene = MAIN;
      drawMain();
    }
      // click detection
    int xb, yb, level = 0;
    int minX = 0, maxX = 120, minY = 0, maxY = 240;
    int width = 80, height = 30;
    int buttons[] = {-1, '0', -2, '7', '8', '9', '4', '5', '6', '1', '2', '3'};
    int tempLevel = 0;
    String oldLevel = "";
    if(p.y < maxY && p.y > minY && p.x < maxX && p.x > minX){
      yb = p.y/width;
      xb = p.x/height;
      level = buttons[yb + xb*3];
      tft.setTextColor(WHITE); tft.setTextSize(3);
      tft.fillRect(118, 60, 50, 40, GRAY);
      tft.setCursor(125, 70);
      if (level == -1){
        //currentLevel needs to be tracked
        if(currentLevel + button < 1000){
          currentLevel += button;
        }
        else{
          currentLevel = 999;
        }
        levelString = "";
        tft.print(currentLevel);
        button = currentLevel;
      }
      else if(level == -2){
        currentLevel = button;
        levelString = "";
        tft.print(currentLevel);
      }
      else{
        levelString += (char)level;
        tempLevel = levelString.toInt();
        if(tempLevel < 1000){
          button = tempLevel;
          tft.print(button);
          oldLevel = levelString;
        }
        else{
          tempLevel = 999;
          button = tempLevel;
          tft.print(button);
          oldLevel = levelString;
        }
          
      }
      Serial.println(yb);
      Serial.println(xb);
      Serial.println(level);
      Serial.println(levelString);
      Serial.println(currentLevel);
      Serial.println(button);
      delay(500);
    } 
      return;
  }
  delay(100);
}

void drawOptions(){
  tft.fillScreen(BLACK);
  drawBackButton();
  tft.fillRect(60, 60, 210, 130, RED);
  tft.setCursor(75, 95); tft.setTextSize(8);
  tft.print("STOP");
}  

void drawBolus(){
  int width = 80;
  int height = 30;
  int calcNumber = 1;
  currentLevel = 0;
  levelString = "";
  tft.fillScreen(BLACK);  
  tft.setCursor(10, 70);
  tft.setTextColor(WHITE); tft.setTextSize(3);
  tft.print("Carbs: ");
  tft.fillRect(118, 60, 100, 40, GRAY);
  tft.setCursor(125, 70);
  tft.print(currentLevel);
  drawBackButton();
  for(int i = 0; i < 4; i++){
    for(int j = 0; j < 3; j++){
      tft.fillRect(10+j*width, 120+i*height, width-2, height-2, GRAY);
      tft.setCursor(10+j*width+30, 120+i*height+8);
      tft.setTextColor(WHITE); tft.setTextSize(2);
      if(calcNumber < 10){
        tft.print(calcNumber);
        calcNumber++;
      }
      else{
        if(j == 0){
          tft.print("+");
        }
        else if(j == 1){
          tft.print("0");
        }
        else if(j == 2){
          tft.setCursor(10+j*width+10, 120+i*height+8);
          tft.print("Enter");
        }
      }
    }
  }
    //tft.fillRect(250, 
//  tft.setCursor(150, 50); tft.setTextSize(8);
//  tft.print("GET FUCKED");
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

void drawConfirmBolus(){
  
}

void drawBackButton(){
  tft.fillRect(10, 10, 80, 40, GRAY);
  tft.setCursor(20,20); tft.setTextSize(3);
  tft.print("Back");
}

