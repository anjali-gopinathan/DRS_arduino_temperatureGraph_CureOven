#include <SPI.h>                
#include "Adafruit_GFX.h"       //graphics for tft display
#include "Adafruit_RA8875.h"    //display driver
#include <Adafruit_MAX31856.h>  //thermocouple amplifier
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

tsPoint_t       _tsLCDPoints[3]; 
tsPoint_t       _tsTSPoints[3]; 
tsMatrix_t      _tsMatrix;

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
  tft.touchEnable(true);

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

  Serial.println("Calling tsCalibrate()...");
  tsCalibrate();
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
//  if (tft.touched())
//  {
//    tft.touchRead(&tx, &ty);
//
//    Serial.print("Touch: ");
//    Serial.print(tx); Serial.print(", "); Serial.println(ty);
//
//    if(tx >=0 && tx <= 50 && ty >=440 && ty <= 480){
//       tft.fillRect(0,440,50,40,WHITECOLOR);  //blue
////       myFile.println(storedTemperature[index]);
//         Serial.print("Writing to file: "); 
//         Serial.println(storedTemperature[index]);
//    }
//  }
//  else
//  {
//    tx = 0;
//    ty = 0;
//  }
  waitForTouchEvent(&raw);

    index++;

}
void tsCalibrate(void)
{
  tsPoint_t data;

  /* --------------- Welcome Screen --------------- */
  Serial.println("Starting the calibration process");
  data = renderCalibrationScreen(tft.width() / 2, tft.height() / 2, 5);
  delay(250);

  /* ----------------- First Dot ------------------ */
  // 10% over and 10% down
  data = renderCalibrationScreen(tft.width() / 10, tft.height() / 10, 5);
  _tsLCDPoints[0].x = tft.width() / 10;
  _tsLCDPoints[0].y = tft.height() / 10;
  _tsTSPoints[0].x = data.x;
  _tsTSPoints[0].y = data.y;
  Serial.print("Point 1 - LCD");
  Serial.print(" X: ");
  Serial.print(_tsLCDPoints[0].x);
  Serial.print(" Y: ");
  Serial.print(_tsLCDPoints[0].y); 
  Serial.print(" TS X: ");
  Serial.print(_tsTSPoints[0].x); 
  Serial.print(" Y: ");
  Serial.println(_tsTSPoints[0].y); 
  delay(250);

  /* ---------------- Second Dot ------------------ */
  // 50% over and 90% down
  data = renderCalibrationScreen(tft.width() / 2, tft.height() - tft.height() / 10, 5);
  _tsLCDPoints[1].x = tft.width() / 2;
  _tsLCDPoints[1].y = tft.height() - tft.height() / 10;
  _tsTSPoints[1].x = data.x;
  _tsTSPoints[1].y = data.y;
  Serial.print("Point 2 - LCD");
  Serial.print(" X: ");
  Serial.print(_tsLCDPoints[1].x);
  Serial.print(" Y: ");
  Serial.print(_tsLCDPoints[1].y);
  Serial.print(" TS X: ");
  Serial.print(_tsTSPoints[1].x);
  Serial.print(" Y: ");
  Serial.println(_tsTSPoints[1].y);
  delay(250);

  /* ---------------- Third Dot ------------------- */
  // 90% over and 50% down
  data = renderCalibrationScreen(tft.width() - tft.width() / 10, tft.height() / 2, 5);
  _tsLCDPoints[2].x = tft.width() - tft.width() / 10;
  _tsLCDPoints[2].y = tft.height() / 2;
  _tsTSPoints[2].x = data.x;
  _tsTSPoints[2].y = data.y;
  Serial.print("Point 3 - LCD");
  Serial.print(" X: ");
  Serial.print(_tsLCDPoints[2].x);
  Serial.print(" Y: ");
  Serial.print(_tsLCDPoints[2].y);
  Serial.print(" TS X: ");
  Serial.print(_tsTSPoints[2].x);
  Serial.print(" Y: ");
  Serial.println(_tsTSPoints[2].y);
  delay(250);
  
  /* Clear the screen */
  //tft.fillScreen(RA8875_WHITE);

  // Do matrix calculations for calibration and store to EEPROM
  setCalibrationMatrix(&_tsLCDPoints[0], &_tsTSPoints[0], &_tsMatrix);
}

tsPoint_t renderCalibrationScreen(uint16_t x, uint16_t y, uint16_t radius)
{
//  tft.fillScreen(RA8875_WHITE);
//  tft.drawCircle(x, y, radius, RA8875_RED);
//  tft.drawCircle(x, y, radius + 2, 0x8410);  /* 50% Gray */

  // Wait for a valid touch events
  tsPoint_t point = { 0, 0 };
  
  /* Keep polling until the TS event flag is valid */
  bool valid = false;
  while (!valid)
  {
    Serial.println("calling waitforTouchEvent inside renderCalibrationScreen method");
    waitForTouchEvent(&point);
    if (point.x || point.y) 
    {
      valid = true;
    }
  }
  
  return point;
}
int setCalibrationMatrix( tsPoint_t * displayPtr, tsPoint_t * screenPtr, tsMatrix_t * matrixPtr)
{
  int  retValue = 0;
  
  matrixPtr->Divider = ((screenPtr[0].x - screenPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) - 
                       ((screenPtr[1].x - screenPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) ;
  
  if( matrixPtr->Divider == 0 )
  {
    retValue = -1 ;
  }
  else
  {
    matrixPtr->An = ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) - 
                    ((displayPtr[1].x - displayPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) ;
  
    matrixPtr->Bn = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].x - displayPtr[2].x)) - 
                    ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].x - screenPtr[2].x)) ;
  
    matrixPtr->Cn = (screenPtr[2].x * displayPtr[1].x - screenPtr[1].x * displayPtr[2].x) * screenPtr[0].y +
                    (screenPtr[0].x * displayPtr[2].x - screenPtr[2].x * displayPtr[0].x) * screenPtr[1].y +
                    (screenPtr[1].x * displayPtr[0].x - screenPtr[0].x * displayPtr[1].x) * screenPtr[2].y ;
  
    matrixPtr->Dn = ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].y - screenPtr[2].y)) - 
                    ((displayPtr[1].y - displayPtr[2].y) * (screenPtr[0].y - screenPtr[2].y)) ;
  
    matrixPtr->En = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].y - displayPtr[2].y)) - 
                    ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].x - screenPtr[2].x)) ;
  
    matrixPtr->Fn = (screenPtr[2].x * displayPtr[1].y - screenPtr[1].x * displayPtr[2].y) * screenPtr[0].y +
                    (screenPtr[0].x * displayPtr[2].y - screenPtr[2].x * displayPtr[0].y) * screenPtr[1].y +
                    (screenPtr[1].x * displayPtr[0].y - screenPtr[0].x * displayPtr[1].y) * screenPtr[2].y ;

    // Persist data to EEPROM
    // eepromWriteS32(CFG_EEPROM_TOUCHSCREEN_CAL_AN, matrixPtr->An);
    // eepromWriteS32(CFG_EEPROM_TOUCHSCREEN_CAL_BN, matrixPtr->Bn);
    // eepromWriteS32(CFG_EEPROM_TOUCHSCREEN_CAL_CN, matrixPtr->Cn);
    // eepromWriteS32(CFG_EEPROM_TOUCHSCREEN_CAL_DN, matrixPtr->Dn);
    // eepromWriteS32(CFG_EEPROM_TOUCHSCREEN_CAL_EN, matrixPtr->En);
    // eepromWriteS32(CFG_EEPROM_TOUCHSCREEN_CAL_FN, matrixPtr->Fn);
    // eepromWriteS32(CFG_EEPROM_TOUCHSCREEN_CAL_DIVIDER, matrixPtr->Divider);
    // eepromWriteU8(CFG_EEPROM_TOUCHSCREEN_CALIBRATED, 1);
  }

  return( retValue ) ;
} 

/**************************************************************************/
/*!
    @brief  Waits for a touch event
*/
/**************************************************************************/
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
    Serial.println("waiting for digital interrupt");
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
