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
#define LIMEGREENCOLOR 0x00FF00

Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);
Adafruit_MAX31856 tc1 = Adafruit_MAX31856(38, 39, 37, 36);

//uint16_t tx, ty;

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
//  tft.fillScreen(RA8875_BLACK);
  tft.fillScreen(BLACKCOLOR);
  tft.textMode();
  
  tft.textSetCursor(10, 10);

  //tft.setRotation(0);
  /* Render some text! */


//longest temp time = 230 min
//Draw Graph outline
  tft.drawRect(60, 50, 730, 350, WHITECOLOR);    //drawRect(x0, y0, width, height, color)

  tft.textTransparent(RA8875_WHITE);
  tft.textSetCursor(635, 440);
  tft.textEnlarge(1);
  tft.textWrite("Time (min)");
  tft.textSetCursor(0,0);
  tft.textWrite("Temp (C)        Temperature vs time");
  
//  tft.textSetCursor(10,55);
//  tft.textWrite("90");

  //x axis labels:
  char minuteNumber_String[3];
  tft.textEnlarge(0.6);
  
  for(int x=50; x<=790; x+=60){
    tft.textSetCursor(x,410);
    dtostrf(  ( (x-50)/3 )  , 3, 0, minuteNumber_String);

    tft.textWrite(minuteNumber_String);
  }

  //y axis labels:
  char tempValue_String[1];
  for(int y=390; y>=40; y-=20){
    tft.textSetCursor(10,y);
    dtostrf( (int)(( 390-y )/4  ), 2, 0, tempValue_String);
    tft.textWrite(tempValue_String);
  }
  //start/stop button:
//  tft.fillRect(0,275,80,50,BLUECOLOR);  //blue
//  
//  tft.textSetCursor(0,280);
//  tft.textEnlarge(0.7);
//  tft.textTransparent(GREENCOLOR);
//  tft.textWrite("Start");
    tft.drawPixel(780,10,LIMEGREENCOLOR);

}

void loop() 
{
   //tft.fillScreen(RA8875_BLACK);
  tft.fillRect(100,440,480,40,BLACKCOLOR);  //refresh the current temp label

  float currentTemp = tc1.readThermocoupleTemperature();  //degrees celsius
  char temperature_string[5];

  dtostrf(currentTemp, 5, 1, temperature_string);

  Serial.print("Current temp: ");
  Serial.println(currentTemp);

  tft.textEnlarge(1);
  tft.textTransparent(RA8875_WHITE);
  tft.textSetCursor(100,440);
  tft.textWrite("Current temp (C): ");
  tft.textWrite(temperature_string);


//  int TwentySecondPixel = 60;
//  int TempPixel = 400- ((int)(currentTemp) *4);
////  tft.drawLine(TwentySecondPixel,); //drawLine(x0,y0,x1,y1,color);
//  tft.drawPixel(TwentySecondPixel, TempPixel, LIMEGREENCOLOR);
//  TwentySecondPixel++;

  delay(1000);//20 seconds refresh

}
