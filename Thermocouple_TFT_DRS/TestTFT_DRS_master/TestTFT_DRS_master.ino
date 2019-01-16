#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_RA8875.h"
#include <Adafruit_MAX31856.h>
//#include <TFT.h>

#define RA8875_INT 18
#define RA8875_CS 53
#define RA8875_RESET 16

#define WHITECOLOR 0xFFFF // White
#define BLACKCOLOR 0x0000  // Black
#define BLUECOLOR 0x076eff //Blue
#define GREENCOLOR 0x45ad47 //Green

Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);
Adafruit_MAX31856 tc1 = Adafruit_MAX31856(38, 39, 37, 36);

uint16_t tx, ty;

void setup() 
{
  Serial.begin(9600);
  Serial.println("RA8875 start");
  
  /* Initialise the display using 'RA8875_480x272' or 'RA8875_800x480' */
  if (!tft.begin(RA8875_800x480)) {
    Serial.println("RA8875 Not Found!");
    while (1);
  }
  tc1.begin();
  tc1.setThermocoupleType(MAX31856_TCTYPE_K);

  tft.displayOn(true);
  tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
  tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
  tft.PWM1out(255);
  tft.fillScreen(RA8875_BLACK);

  tft.textMode();
  
  tft.textSetCursor(10, 10);

  tft.setRotation(1);
  /* Render some text! */

  //tft.drawLine(100, 50, 100, 380,0x000000); // draw a y-axis across the screen
//  for(int x=100; x<110; x++){
//    tft.drawFastVLine(x,50,230, WHITECOLOR);
//  }
//
//  tft.stroke(255, 255, 255); // set the stroke color to white
//  tft.line(98, 378, 780, 98); // draw a x-axis across the screen
//  for(int y=50; y<=380; y++){
//    tft.drawPixel(y,100,0x000000);
//  }
//longest temp time = 230 min

  tft.textTransparent(RA8875_WHITE);

  tft.textSetCursor(50, 250);
  tft.textEnlarge(2);
//  tft.textWrite("Current temp: ");
}

void loop() 
{
   tft.fillScreen(RA8875_BLACK);

//Draw Graph outline
  tft.drawRect(100, 50, 690, 330, WHITECOLOR);    //drawRect(x0, y0, width, height, color)

  tft.textTransparent(RA8875_WHITE);
  tft.textSetCursor(635, 440);
  tft.textEnlarge(1);
  tft.textWrite("Time (min)");
  tft.textSetCursor(0,0);
  tft.textWrite("Temp (C)        Temperature vs time");
  
  tft.textSetCursor(50,55);
  tft.textWrite("90");
  //tft.textSetCursor(100,390);
  //tft.textWrite("0");

  char minuteNumber_String[3];
  tft.textEnlarge(0.6);

  for(int x=80; x<=790; x+=60){
    tft.textSetCursor(x,390);
    dtostrf((x-80)/3, 3, 0, minuteNumber_String);

    tft.textWrite(minuteNumber_String);
  }
//  tft.fillRect(0,275,80,50,BLUECOLOR);  //blue
//  
//  tft.textSetCursor(0,280);
//  tft.textEnlarge(0.7);
//  tft.textTransparent(GREENCOLOR);
//  tft.textWrite("Start");

  float currentTemp = tc1.readThermocoupleTemperature();  //degrees celsius
  char temperature_string[5];

  dtostrf(currentTemp, 5, 1, temperature_string);

  Serial.print("Current temp: ");
  Serial.println(currentTemp);

  tft.textTransparent(RA8875_WHITE);
  tft.textSetCursor(100,440);
  tft.textWrite("Current temp (C): ");
  tft.textWrite(temperature_string);

  delay(700);

}
