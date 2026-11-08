/**
*@version: 11/8/2026
*Basic plan: 
*Initalize the screen and Accelerometer
*Draw a circular grid to the screen on startup
*Every few ms get the calibrated x and y accelertion data
*Delete the old dot and replace anything under it
*Draw a dot on the scren at the pixel values corresponding to the x and y data
*
*To Do: Calibration function that can ajust the x and y values to be correct
*by taking into account the roll, pitch, and yaw of the accelerometer
*To Do: a lateral and longitudinal g number duisplay to the bottom left and right of thr screen
*To Do: Write more comments to explkained every decision here
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

//Meter background object, just a binary bitmap that we can get color at pixel readings from
GFXcanvas1 background(240, 320);

//global x y z directional acceleration values from accelerometer, dont really matter which is which without
//Proper calibration implemented yet
float x, y, z;

/**
*Setup function to run everytime device is tured on, initializes comms for modulino accerometer
*as well as the tft, then plays the miotAnimation() startup animation before draewing the
*bullseye graph backgroung to the screen
*/
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

/**
*loop function runs continuously while devicew has power, F
*First, reads the acceleeration data from the modulino and updates the 3 global variables
*Second, Prints the data to the serial moniter for debugging purposes to know when the wires disconect which it a lot
*Third, draws the dot to the tft using drawdot() which draws it at a pont correspodnign to the current x and y readings from ther global vars
*Fourth, Lets the dot stay on the screen for 100ms before easing it with earasedot()
*/
void loop() {
    readAcceleration();
    printCoords();
    drawDot();
    delay(100);
    eraseDot();
}

/**
*Reads accelrometer data and updates the global variables
*/
void readAcceleration() {
    movement.update();
    //movement is the accelerometer object
    x = movement.getX();
    y = movement.getY();
    z = movement.getZ();
}

/**
*Takes the x and y values read from modulino, then converts them to to pixel values and prints then 
*to the screen
*/
void drawDot() {
    //Figured out scale factor by takiing 2 / 220 cause the radius of the circle is 220 and the muudlino
    //Reads forces between -1 and 1, so 2 total so 
    float scale = 0.0091;
    //Offsets the dot's position from the center of the screen at x:120
    float rawX = 120 - (x / scale);
    //coords need to be converted from a float to 16 bit unsigned int to be drawn to the screen
    uint16_t xCord = static_cast<uint16_t>(rawX);

    float rawY = 160 - (y / scale);
    uint16_t yCord = static_cast<uint16_t>(rawY);

    //Draw a red circle at the calculated coordinates
    tft.fillCircle(xCord, yCord, 4, ILI9341_RED);
}


/**
*Just prints current coords to serial moniter for debugging purposes 
*/
void printCoords() {
    Serial.print("X: ");
    Serial.print(x);
    Serial.print(" | Y: ");
    Serial.println(y);
}

/**
*Creates the bullseye background for the meter as a b&w bitmap and draws it to the screen
*Creating this as a gfx canvas means we dont have to redraw the whole screen whenevr we need to 
*erase the previous dot and can instead just draw the dot to the tft object and then replace
*it with the bg data that we can quickly read from the bitmap
*/
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

/**
*Erases the previously drawn dot by replacing it with the pixel from the background bitmap at the space it
*once occupied
*Feel like I can make this faster
*/
void eraseDot(){
    ///first get the current dot pos in pixels
    //Could probbaly chnage this so i dont do this calculation twice every loop
    float scale = 0.0091;
    float rawX = 120 - (x / scale);
    uint16_t xCord = static_cast<uint16_t>(rawX);

    float rawY = 160 - (y / scale);
    uint16_t yCord = static_cast<uint16_t>(rawY);

    Serial.print(" | X pixel : ");
    Serial.println(xCord);

    Serial.print(" | Y pixel : ");
    Serial.println(yCord);

    //Nested for loop replaces an 8x8 grid of pixels over the 8 diameter dot created by draw dot
    for (int i = -4; i < 5; i++){
        for (int j = -4; j < 5; j++)
        {
            tft.drawPixel(xCord + i, yCord + j, getBgColor(xCord + i, yCord + j));
        }
    }
}

/**
*Helper function to get the color of the bullseye background at a given location on the screen
*@param: x - the x cord of the pixel we want the color of
*@param: y = the y coord of the pixel we want the color of
*@return: uint16_t - a 16 bit rgb color value
*/
uint16_t getBgColor(uint16_t x, uint16_t y)
{
    if (background.getPixel(x, y)){
        return ILI9341_WHITE;
    }
    else{
        return  ILI9341_BLACK;
    }
}

/**
*Displays the 5 frames for the starup animaiton with delays in between
*/
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
