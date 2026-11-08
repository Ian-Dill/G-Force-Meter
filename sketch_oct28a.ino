/**
*Basic plan: 
*Initalize the screen and Accelerometer
*Draw a circular grid to the screen on startup
*Every few ms get the calibrated x and y accelertion data
*Delete the old dot and replace anything under it
*Draw a dot on the scren at the pixel values corresponding to the x and y data
*
*/

#include "Modulino.h"
#include <Adafruit_ILI9341.h>
#include <Adafruit_GFX.h>
#include "frame1.h"
#include "frame2.h"
#include "frame3.h"
#include "frame4.h"
#include "frame5.h"

//Definine pins for communication via SPI with the tft
#define TFT_CS  D10
#define TFT_DC  D9
#define TFT_RST D8

//Accelerometer Object
ModulinoMovement movement;

//TFT object
Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);

//Meter background object
GFXcanvas1 background(240, 320);

//global x y z acceleration values from accelerometer
float x, y, z;

void setup() {
    Serial.begin(9600);
    Modulino.begin();
    movement.begin();

    tft.begin();
    tft.setRotation(0);

    miotAnimation();
    delay(300);

    drawBackground();
}

void loop() {
    readAcceleration();
    printCoords();
    drawDot();
    delay(100);
    eraseDot();
}

// Read accelerometer data
void readAcceleration() {
    movement.update();

    x = movement.getX();
    y = movement.getY();
    z = movement.getZ();
}

// Print accelerometer data
void drawDot() {
    float scale = 0.0091;
    float rawX = 120 - (x / scale);
    uint16_t xCord = static_cast<uint16_t>(rawX);

    float rawY = 160 - (y / scale);
    uint16_t yCord = static_cast<uint16_t>(rawY);

    //just x for now
    tft.fillCircle(xCord, yCord, 4, ILI9341_RED);
}


void printCoords() {
    Serial.print("X: ");
    Serial.print(x);
    Serial.print(" | Y: ");
    Serial.println(y);
}

//creating the bg bitmat that we can refrence later
void drawBackground(){
    background.setRotation(0);
    background.fillScreen(0);
    
    background.drawCircle(120, 160, 110, 1);
    background.drawCircle(120, 160, 55, 1);
    background.drawFastVLine(120, 50, 220, 1);
    background.drawFastHLine(10, 160, 220, 1);

    background.setTextColor(1);
    background.setTextSize(1);
    background.setTextWrap(false);

    background.setCursor(125, 55);
    background.print("1.0");

    background.setCursor(125, 110);
    background.print("0.5");

    background.setCursor(125, 165);
    background.print("0");

    background.setCursor(125, 200);
    background.print("-0.5");

    background.setCursor(125, 255);
    background.print("-1.0");

    background.setCursor(110, 35);
    background.print("BRAKE");

    background.setCursor(110, 280);
    background.print("ACCEL");

    tft.drawBitmap(0, 0, background.getBuffer(), 240, 320, ILI9341_WHITE, ILI9341_BLACK);
}

//Function to get the coords of the current dot and replace all of its pixels from ones gotten fro the bg
void eraseDot(){
    ///first get the current dot pos in pixels
    float scale = 0.0091;
    float rawX = 120 - (x / scale);
    uint16_t xCord = static_cast<uint16_t>(rawX);

    float rawY = 160 - (y / scale);
    uint16_t yCord = static_cast<uint16_t>(rawY);

    Serial.print(" | X pixel : ");
    Serial.println(xCord);

    Serial.print(" | Y pixel : ");
    Serial.println(yCord);
    //then replace all the pixels that the dot currently occupies with ones from the bg object at the corresponding location
    for (int i = -4; i < 5; i++){
        for (int j = -4; j < 5; j++)
        {
            tft.drawPixel(xCord + i, yCord + j, getBgColor(xCord + i, yCord + j));
        }
    }
}

uint16_t getBgColor(uint16_t x, uint16_t y)
{
    if (background.getPixel(x, y)){
        return ILI9341_WHITE;
    }
    else{
        return  ILI9341_BLACK;
    }
}

void miotAnimation(){
    tft.fillScreen(ILI9341_BLACK);
    int16_t x = (tft.width()  - 240) / 2;
    int16_t y = (tft.height() - 240) / 2;

    tft.drawRGBBitmap(x, y, frame1, 240, 240);
    delay(200);
    tft.drawRGBBitmap(x, y, frame2, 240, 240);
    delay(200);
    tft.drawRGBBitmap(x, y, frame3, 240, 240);
    delay(200);
    tft.drawRGBBitmap(x, y, frame4, 240, 240);
    delay(300);
    tft.drawRGBBitmap(x, y, frame5, 240, 240);
}
