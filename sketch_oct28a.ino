/**
*Basic plan: 
*Initalize the screen and Accelerometer
*Draw a circular grid to the screen on startup
*Every few ms get the calibrated x and y accelertion data
*Delete the old dot and replace anything under it
*Draw a dot on the scren at the pixel values corresponding to the x and y data
*
*
*/

#include "Modulino.h"
#include <Adafruit_ILI9341.h>
#include <Adafruit_GFX.h>


//Definine pins for communication via SPI with the tft
#define TFT_CS  D10
#define TFT_DC  D9
#define TFT_RST D8

//Accelerometer Object
ModulinoMovement movement;

//TFT object
Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);

//Meter background object
GFXcanvas1 background(320, 240);

//global x y z acceleration values from accelerometer
float x, y, z;

void setup() {
    Serial.begin(9600);
    Modulino.begin();
    movement.begin();

    tft.begin();
    tft.setRotation(1);

    background.setRotation(0);
    background.fillScreen(0);

    background.drawCircle(160, 120, 110, 1);
    background.drawFastVLine(160, 10, 220, 1);
    background.drawFastHLine(50, 120, 220, 1);

    tft.drawBitmap(
        0, 0,
        background.getBuffer(),
        320, 240,
        ILI9341_WHITE,
        ILI9341_BLACK
    );
}

void loop() {
    readAcceleration();
    printCoords();
    drawDot();
    //reDrawBackground();
    delay(34);
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
    float rawX = 160 - (x / scale);
    uint16_t xCord = static_cast<uint16_t>(rawX);

    float rawY = 120 - (y / scale);
    uint16_t yCord = static_cast<uint16_t>(rawY);

    //just x for now
    tft.fillCircle(xCord, yCord, 4, ILI9341_RED);
}

void redrawScreen(){
    tft.fillScreen(ILI9341_BLACK);
    tft.drawCircle(160, 120, 110, ILI9341_WHITE);
    tft.drawFastVLine(160, 10, 220, ILI9341_WHITE);
    tft.drawFastHLine(50, 120, 220, ILI9341_WHITE);
}

void printCoords() {
    Serial.print("X: ");
    Serial.print(x);
    Serial.print(" | Y: ");
    Serial.println(y);
}


void reDrawBackground(){
    background.fillScreen(ILI9341_BLACK);

    //x, y, rad, clor
    background.drawCircle(160, 120, 110, ILI9341_WHITE);
    background.drawFastVLine(160, 10, 220, ILI9341_WHITE);
    background.drawFastHLine(50, 120, 220, ILI9341_WHITE);

    // Copy the canvas to the actual TFT
    tft.drawBitmap(
        0, 0,
        background.getBuffer(),
        240, 320,
        ILI9341_WHITE,
        ILI9341_BLACK
    );
}