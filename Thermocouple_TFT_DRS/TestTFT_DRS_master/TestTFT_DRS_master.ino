#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_RA8875.h"
#include <Adafruit_MAX31856.h>

// Library only supports hardware SPI at this time
// Connect SCLK to UNO Digital #13 (Hardware SPI clock)
// Connect MISO to UNO Digital #12 (Hardware SPI MISO)
// Connect MOSI to UNO Digital #11 (Hardware SPI MOSI)
//#define RA8875_INT 3
//#define RA8875_CS 10
//#define RA8875_RESET 9

#define RA8875_INT 18
#define RA8875_CS 53
#define RA8875_RESET 16


Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);
Adafruit_MAX31856 tc1 = Adafruit_MAX31856(10, 11, 12, 13);

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

  /* Switch to text mode */  
  tft.textMode();
  

  
  /* Set a solid for + bg color ... */
  
  /* ... or a fore color plus a transparent background */

  
  /* Set the cursor location (in pixels) */
  tft.textSetCursor(10, 10);
  
  /* Render some text! */
  tft.setTextSize(2);
  //tft.textWrite("Hello world Hello world Hello world Hello world Hello world Hello world Hello world Hello world\nHello world Hello world Hello world Hello world Hello world Hello world Hello world Hello world");
  //Serial.print("Hello world message has been sent to screen\n");


  char string[15] = "Hello, World! ";
//  tft.textTransparent(RA8875_WHITE);
//  tft.textWrite(string);
//  tft.textColor(RA8875_WHITE, RA8875_RED);
//  tft.textWrite(string);
//  tft.textTransparent(RA8875_CYAN);
//  tft.textWrite(string);
  tft.textTransparent(RA8875_GREEN);
//  tft.textWrite(string);
//  tft.textColor(RA8875_YELLOW, RA8875_CYAN);
//  tft.textWrite(string);
 
//  tft.textColor(RA8875_BLACK, RA8875_MAGENTA);
//  tft.textWrite(string);

//  /* Change the cursor location and color ... */  
//  tft.textSetCursor(100, 100);
//  tft.textTransparent(RA8875_RED);
//  /* If necessary, enlarge the font */
//  tft.textEnlarge(1);
//  /* ... and render some more text! */
//  tft.textWrite(string);
//  tft.textSetCursor(100, 150);
//  tft.textEnlarge(2);
//  tft.textWrite(string);


  
  tft.textTransparent(RA8875_WHITE);

  tft.textSetCursor(50, 250);
  tft.textEnlarge(2);
//  tft.textWrite("Current temp: ");
}

void loop() 
{
    tft.fillScreen(RA8875_BLACK);

  float currentTemp = tc1.readThermocoupleTemperature() * 1.8 + 32;
  char temperature_string[5];
//100.1
  dtostrf(currentTemp, 5, 1, temperature_string);

  Serial.print("Current temp: ");
  Serial.println(currentTemp);
  tft.textTransparent(RA8875_WHITE);

  tft.textWrite(temperature_string);

  tft.textSetCursor(50, 250);
  tft.textWrite("      ");

  tft.textSetCursor(50, 250);

  delay(500);
//  
}
//
//char doubleToChar (double d){
//  for(int i = 0; i<d; i++){
//    
//  }
//}
