#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_RA8875.h"
#include <Adafruit_MAX31856.h>
//#include <TFT.h>
//#include <SD.h>

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

//File myFile;

int TwentySecondPixel = 60, index=0;
uint16_t tx, ty;
float storedTemperature[720];

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

//  tft.setRotation(0);
  /* Render some text! */


//longest temp time = 230 min
//Draw Graph outline
  delay(5000);
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
  //button:
//  tft.fillRect(0,440,50,40,BLUECOLOR);  //blue
//  
//  tft.textSetCursor(0,440);
//  tft.textEnlarge(0.7);
//  tft.textTransparent(BLACKCOLOR);
//  tft.textWrite("Write to SD");
//    tft.drawPixel(780,10,LIMEGREENCOLOR);


//    pinMode(4, OUTPUT);
//    digitalWrite(4, HIGH);
//
//  Serial.print("Initializing SD card...");
//
//  if (!SD.begin(4)) {
//    Serial.println("initialization failed!");
//    while (1);
//  }
//  Serial.println("initialization done.");
//
//  if (SD.exists("thermocoupleTempData.txt")) {
//    Serial.println("thermocoupleTempData.txt exists.");
//  } else {
//    Serial.println("thermocoupleTempData.txt doesn't exist.");
//  }
//
//  // open a new file and immediately close it:
//  Serial.println("Creating thermocoupleTempData.txt...");
//  myFile = SD.open("thermocoupleTempData.txt", FILE_WRITE);
//
//  // Check to see if the file exists:
//  if (SD.exists("thermocoupleTempData.txt")) {
//    Serial.println("thermocoupleTempData.txt exists.");
//  } else {
//    Serial.println("thermocoupleTempData.txt doesn't exist.");
//  }

}

void loop() 
{
   //tft.fillScreen(RA8875_BLACK);
  tft.fillRect(100,440,480,40,BLACKCOLOR);  //refresh the current temp label
  tft.drawRect(60, 50, 730, 350, WHITECOLOR);    //drawRect(x0, y0, width, height, color)

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


  storedTemperature[index] = currentTemp;
  int TempPixel = 400- ((int)(currentTemp) *4);
  if(TwentySecondPixel >= 60 && TwentySecondPixel <=790){
    ////  tft.drawLine(TwentySecondPixel,); //drawLine(x0,y0,x1,y1,color);
    //  tft.drawPixel(TwentySecondPixel, TempPixel, LIMEGREENCOLOR);
    
    tft.drawLine(TwentySecondPixel, TempPixel, TwentySecondPixel+2, TempPixel, WHITECOLOR);
    TwentySecondPixel+=3;
  }
  delay(0.5*1000);//20 seconds refresh

//  if (tft.touched())
//  {
//    tft.touchRead(&tx, &ty);
////    point->tx = tx;
////    point->ty = ty;
//    Serial.print("Touch: ");
//    Serial.print(tx); Serial.print(", "); Serial.println(ty);
//
//    if(tx >=0 && tx <= 50 && ty >=440 && ty <= 480){
//       tft.fillRect(0,440,50,40,WHITECOLOR);  //blue
//       myFile.println(storedTemperature[index]);
//
//    }
//  }
//  else
//  {
//    tx = 0;
//    ty = 0;
//  }
    index++;

}
