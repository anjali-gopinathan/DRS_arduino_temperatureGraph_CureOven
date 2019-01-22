#include <SPI.h>    
#include "Adafruit_GFX.h"
#include "Adafruit_RA8875.h"
#include <Adafruit_MAX31856.h>
#include <SD.h>

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

File myFile;

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
  tft.fillScreen(BLACKCOLOR);
  tft.textMode();
  
  tft.textSetCursor(10, 10);

//Draw Graph outline
  delay(5000);

 Serial.print("Initializing SD card...");

  if (!SD.begin(5)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("tcData.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to tcData.txt...");
    myFile.println("testing 1, 2, 3.");
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening tcData.txt");
  }

  // re-open the file for reading:
  myFile = SD.open("tcData.txt");
  if (myFile) {
    Serial.println("tcData.txt:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening tcData.txt");
  }



  
  tft.drawRect(60, 50, 730, 350, WHITECOLOR);    //drawRect(x0, y0, width, height, color)

  tft.textTransparent(RA8875_WHITE);
  tft.textSetCursor(635, 440);
  tft.textEnlarge(1);
  tft.textWrite("Time (min)");
  tft.textSetCursor(0,0);
  tft.textWrite("Temp (C)        Temperature vs time");
  
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
  tft.fillRect(0,440,50,40,BLUECOLOR);  //blue
  
  tft.textSetCursor(0,440);
  tft.textEnlarge(0.7);
  tft.textTransparent(BLACKCOLOR);
  tft.textWrite("Write to SD");
    tft.drawPixel(780,10,LIMEGREENCOLOR);

}

void loop() 
{
    tsPoint_t raw;

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
    
    tft.drawLine(TwentySecondPixel, TempPixel, TwentySecondPixel+2, TempPixel, WHITECOLOR);
    TwentySecondPixel+=3;
  }
  delay(0.5*1000);//20 seconds refresh
//before index++ in loop() method
  if (tft.touched())
  {
    tft.touchRead(&tx, &ty);

    Serial.print("Touch: ");
    Serial.print(tx); Serial.print(", "); Serial.println(ty);

    if(tx >=0 && tx <= 50 && ty >=440 && ty <= 480){
       tft.fillRect(0,440,50,40,WHITECOLOR);  //blue
//       myFile.println(storedTemperature[index]);
         Serial.print("Writing to file: "); 
         Serial.println(storedTemperature[index]);
    }
  }
  else
  {
    tx = 0;
    ty = 0;
  }
  waitForTouchEvent(&raw);

    index++;

}

void waitForTouchEvent(tsPoint_t * point)
{
  /* Clear the touch data object and placeholder variables */
  memset(point, 0, sizeof(tsPoint_t));

  
  /* Clear any previous interrupts to avoid false buffered reads */
  uint16_t x, y;
  tft.touchRead(&x, &y);
  delay(1);

  /* Wait around for a new touch event (INT pin goes low) */
  while (digitalRead(RA8875_INT))
  {
    Serial.println("waiting for RA8875 interrupt");
  }
  
  /* Make sure this is really a touch event */
  if (tft.touched())
  {
    tft.touchRead(&x, &y);
    point->x = x;
    point->y = y;
    Serial.print("Touch: ");
    Serial.print(point->x); Serial.print(", "); Serial.println(point->y);
  }
  else
  {
    point->x = 0;
    point->y = 0;
  }
}
