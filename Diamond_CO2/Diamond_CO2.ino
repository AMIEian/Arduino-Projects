#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_RA8875.h"
#include<Fonts/FreeSans24pt7b.h>
// Library only supports hardware SPI at this time
// Connect SCLK to UNO Digital #13 Mega #52 (Hardware SPI clock)
// Connect MISO to UNO Digital #12 Mega #50 (Hardware SPI MISO)
// Connect MOSI to UNO Digital #11 Mega #51 (Hardware SPI MOSI)
#define RA8875_INT 3
#define RA8875_CS 10
#define RA8875_RESET 9

// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define ROZ     0xFBE0
#define LIGHT_GRAY 0xBDF7
#define DARK_GRAY 0x7BEF

#define NON_INVERTED        false
#define INVERTED            true
#define NORMAL_BUTTON       false
#define ROUND_BUTTON        true

Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);
Adafruit_GFX_Button buttonAdd, buttonSub;
uint16_t tx, ty, touchX, touchY;
float xScale, yScale;
boolean screenTouched = false;
char screenDebouncer = 0;
//char string[] = "123456";
char x = 0;
void setup() 
{
  Serial.begin(9600);
  Serial.println("RA8875 start");

  /* Initialise the display using 'RA8875_480x272' or 'RA8875_800x480' */
  if (!tft.begin(RA8875_800x480)) {
    Serial.println("RA8875 Not Found!");
    while (1);
  }

  tft.displayOn(true);
  tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
  tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
  tft.PWM1out(255);
  pinMode(RA8875_INT, INPUT);
  digitalWrite(RA8875_INT, HIGH);
  
  tft.touchEnable(true);
  xScale = 1024.0F/tft.width();
  yScale = 1024.0F/tft.height();
  
  tft.fillScreen(WHITE);
  buttonAdd.initButton(&tft,125,30,50,50,BLACK,LIGHT_GRAY,WHITE,"ADD",2);
  buttonAdd.drawButton(NON_INVERTED, ROUND_BUTTON);
  buttonSub.initButton(&tft,125,tft.height()-30,50,50,BLACK,LIGHT_GRAY,WHITE,"SUB",2);
  buttonSub.drawButton(NON_INVERTED, ROUND_BUTTON);
  tft.drawSevenSegNumber(x,2,70,100,80,8,BLUE);
  //BootScreen();
  
}

void loop() 
{
  /* Wait around for touch events */
  if (! digitalRead(RA8875_INT)) 
  {
    if (tft.touched()) 
      { 
        screenTouched = true;
        screenDebouncer = 0;
        tft.touchRead(&tx, &ty);
        touchX = tx;
        touchY = ty;
         /* Draw a circle */
        tft.fillCircle((uint16_t)(tx/xScale), (uint16_t)(ty/yScale), 4, RA8875_RED);
      } 
    else
      {
	if(screenTouched)
	  {
	    screenDebouncer++;
	    if(screenDebouncer > 50)
	      {
		touchX = -1;
		touchY = -1;
		screenDebouncer = 0;
		screenTouched = false;
		if(buttonAdd.buttonClicked)
		  buttonAdd.released(ROUND_BUTTON);
		if(buttonSub.buttonClicked)
		  buttonSub.released(ROUND_BUTTON);
	      }
          }
      }
    if(screenTouched)
      {
        if(buttonAdd.buttonClicked == false)
	  {
	    if(buttonAdd.contains(touchX,touchY))
	      {
		buttonAdd.drawButton(INVERTED,ROUND_BUTTON);
		buttonAdd.buttonClicked = true;
		buttonAdd.buttonTimer = 0;
		x++;
		if(x == 100)
		  x = 0;
		tft.fillRect(0,95,200,120,WHITE);
		tft.drawSevenSegNumber(x,2,70,100,80,8,BLUE);
	      }
	  }
	else
	  {
	    buttonAdd.buttonTimer++;
	    if(buttonAdd.buttonTimer > 500)
		buttonAdd.released(ROUND_BUTTON);
	  }
			
	if(buttonSub.buttonClicked == false)
	  {
	    if(buttonSub.contains(touchX,touchY))
	      {
		buttonSub.drawButton(INVERTED,ROUND_BUTTON);
		buttonSub.buttonClicked = true;
		buttonSub.buttonTimer = 0;
		x--;
		if(x < 0)
		  x = 99;
		tft.fillRect(0,95,200,120,WHITE);
		tft.drawSevenSegNumber(x,2,70,100,80,8,BLUE);
	      }
	  }
	else
	  {
	    buttonSub.buttonTimer++;
	    if(buttonSub.buttonTimer > 500)
	      buttonSub.released(ROUND_BUTTON);
	  }
    }
  }
}

void BootScreen()
{
  //------------BOOT SCREEN----------------//
  //Display Background
  tft.fillScreen(RA8875_WHITE);
  
  //Diamond Logo
  tft.fillEllipse(400, 39+201, 155, 33, RA8875_YELLOW);
  tft.fillEllipse(400, 39+201, 150, 28, RA8875_BLUE);
  tft.setFont(&FreeSans24pt7b);
  tft.setTextColor(RA8875_YELLOW);
  tft.setCursor(290,55+201);
  tft.print("DIAMOND");
  delay(1000);
  tft.fillEllipse(400, 39+201, 155, 33, RA8875_GREEN);
  tft.fillEllipse(400, 39+201, 150, 28, RA8875_RED);
  tft.setFont(&FreeSans24pt7b);
  tft.setTextColor(RA8875_GREEN);
  tft.setCursor(290,55+201);
  tft.print("DIAMOND");
  delay(1000);
  tft.fillEllipse(400, 39+201, 155, 33, RA8875_BLUE);
  tft.fillEllipse(400, 39+201, 150, 28, RA8875_YELLOW);
  tft.setFont(&FreeSans24pt7b);
  tft.setTextColor(RA8875_BLUE);
  tft.setCursor(290,55+201);
  tft.print("DIAMOND");
  delay(1000);
  tft.fillEllipse(400, 39+201, 155, 33, RA8875_RED);
  tft.fillEllipse(400, 39+201, 150, 28, RA8875_GREEN);
  tft.setFont(&FreeSans24pt7b);
  tft.setTextColor(RA8875_RED);
  tft.setCursor(290,55+201);
  tft.print("DIAMOND");
  delay(1000);
  tft.fillEllipse(400, 39+201, 155, 33, RA8875_YELLOW);
  tft.fillEllipse(400, 39+201, 150, 28, RA8875_BLUE);
  tft.setFont(&FreeSans24pt7b);
  tft.setTextColor(RA8875_YELLOW);
  tft.setCursor(290,55+201);
  tft.print("DIAMOND");
  delay(1000);
  tft.fillEllipse(400, 39+201, 155, 33, RA8875_GREEN);
  tft.fillEllipse(400, 39+201, 150, 28, RA8875_RED);
  tft.setFont(&FreeSans24pt7b);
  tft.setTextColor(RA8875_GREEN);
  tft.setCursor(290,55+201);
  tft.print("DIAMOND");
  delay(1000);
  tft.fillEllipse(400, 39+201, 155, 33, RA8875_BLUE);
  tft.fillEllipse(400, 39+201, 150, 28, RA8875_YELLOW);
  tft.setFont(&FreeSans24pt7b);
  tft.setTextColor(RA8875_BLUE);
  tft.setCursor(290,55+201);
  tft.print("DIAMOND");
  delay(1000);
  tft.fillEllipse(400, 39+201, 155, 33, RA8875_RED);
  tft.fillEllipse(400, 39+201, 150, 28, RA8875_GREEN);
  tft.setFont(&FreeSans24pt7b);
  tft.setTextColor(RA8875_RED);
  tft.setCursor(290,55+201);
  tft.print("DIAMOND");
  delay(1000);
}

void MainScreen()
{
  //--------------MAIN SCREEN----------------//
  //Display Background
  tft.fillScreen(RA8875_CYAN);
  
  //Diamond Logo
  tft.fillEllipse(400, 39, 155, 33, RA8875_YELLOW);
  tft.fillEllipse(400, 39, 150, 28, RA8875_BLUE);
  tft.setFont(&FreeSans24pt7b);
  tft.setTextColor(RA8875_YELLOW);
  tft.setCursor(290,55);
  tft.print("DIAMOND");
  
  //--------------PRESSURE COLUMN---------------//
  tft.setFont();
  tft.setTextColor(RA8875_BLACK);
  tft.setTextSize(2);
  tft.setCursor(15,445);
  tft.print("PRESSURE (mmHg)");  
  //RED Box
  tft.fillRect(6, 60, 196, 159, RA8875_RED);
  //BLACK Box
  tft.fillRect(6+5, 60+5, 196-10, 159-10, RA8875_BLACK);
  //Actual String
  tft.setFont();
  tft.setTextColor(RA8875_WHITE);
  tft.setTextSize(2);
  tft.setCursor(65,75);
  tft.print("ACTUAL");  
  //Actual Pressure Reading
  tft.setFont(&FreeSans24pt7b);
  tft.setTextColor(RA8875_WHITE);
  tft.setTextSize(3);
  tft.setCursor(20,200);
  tft.print("12");
  //RED Box
  tft.fillRect(26, 230, 156, 125, RA8875_RED);
  //BLACK Box
  tft.fillRect(26+5, 230+5, 156-10, 125-10, RA8875_BLACK);
  //Set String
  tft.setFont();
  tft.setTextColor(RA8875_WHITE);
  tft.setTextSize(2);
  tft.setCursor(85,245);
  tft.print("SET");
  //Set Pressure Reading
  tft.setFont(&FreeSans24pt7b);
  tft.setTextColor(RA8875_WHITE);
  tft.setTextSize(2);
  tft.setCursor(50,340);
  tft.print("12");
  //RED Circle
  tft.fillEllipse(61, 400, 35, 35, RA8875_RED);
  //BLACK Circle
  tft.fillEllipse(61, 400, 30, 30, RA8875_BLACK);
  //Left Facing Triangle
  tft.fillTriangle(45, 400, 45+25, 400-25, 45+25, 400+25, RA8875_GREEN);
  //RED Circle
  tft.fillEllipse(145, 400, 35, 35, RA8875_RED);
  //BLACK Circle
  tft.fillEllipse(145, 400, 30, 30, RA8875_BLACK);
  //Right Facing Triangle
  tft.fillTriangle(161, 400, 161-25, 400-25, 161-25, 400+25, RA8875_GREEN);
  
  //--------------FLOW COLUMN---------------//
  tft.setFont();
  tft.setTextColor(RA8875_BLACK);
  tft.setTextSize(2);
  tft.setCursor(605,445);
  tft.print("FLOW (Ltr./ Min)");
  //RED Box
  tft.fillRect(598, 60, 196, 159, RA8875_RED);
  //BLACK Box
  tft.fillRect(598+5, 60+5, 196-10, 159-10, RA8875_BLACK);
  //Actual String
  tft.setFont();
  tft.setTextColor(RA8875_WHITE);
  tft.setTextSize(2);
  tft.setCursor(65 + 592,75);
  tft.print("ACTUAL");  
  //Actual Pressure Reading
  tft.setFont(&FreeSans24pt7b);
  tft.setTextColor(RA8875_WHITE);
  tft.setTextSize(3);
  tft.setCursor(20 + 592,200);
  tft.print("12");
  //RED Box
  tft.fillRect(26 + 592, 230, 156, 125, RA8875_RED);
  //BLACK Box
  tft.fillRect(26+5 + 592, 230+5, 156-10, 125-10, RA8875_BLACK);
  //Set String
  tft.setFont();
  tft.setTextColor(RA8875_WHITE);
  tft.setTextSize(2);
  tft.setCursor(677,245);
  tft.print("SET");
  //Set Pressure Reading
  tft.setFont(&FreeSans24pt7b);
  tft.setTextColor(RA8875_WHITE);
  tft.setTextSize(2);
  tft.setCursor(642,340);
  tft.print("12");
  //RED Circle
  tft.fillEllipse(61+592, 400, 35, 35, RA8875_RED);
  //BLACK Circle
  tft.fillEllipse(61+592, 400, 30, 30, RA8875_BLACK);
  //Left Facing Triangle
  tft.fillTriangle(45+592, 400, 45+25+592, 400-25, 45+25+592, 400+25, RA8875_GREEN);
  //RED Circle
  tft.fillEllipse(145+592, 400, 35, 35, RA8875_RED);
  //BLACK Circle
  tft.fillEllipse(145+592, 400, 30, 30, RA8875_BLACK);
  //Right Facing Triangle
  tft.fillTriangle(161+592, 400, 161-25+592, 400-25, 161-25+592, 400+25, RA8875_GREEN);
  
  //-------------VOLUME BOX---------------//
  //RED Box
  tft.fillRect(212, 60+45, 183, 125, RA8875_RED);
  //BLACK Box
  tft.fillRect(212+5, 60+5+45, 183-10, 125-10, RA8875_BLACK);
  //Volume Reading
  tft.setFont(&FreeSans24pt7b);
  tft.setTextColor(RA8875_WHITE);
  tft.setTextSize(2);
  tft.setCursor(223,153+45);
  tft.print("123");
  tft.setFont();
  tft.setTextColor(RA8875_BLACK);
  tft.setTextSize(2);
  tft.setCursor(255,190+50);
  tft.print("Vol. Ltr.");
  //------------TEMPERATURE BOX------------//
  //RED Box
  tft.fillRect(405, 60+45, 183, 125, RA8875_RED);
  //BLACK Box
  tft.fillRect(405+5, 60+5+45, 183-10, 125-10, RA8875_BLACK);
  //Temperature Reading
  tft.setFont(&FreeSans24pt7b);
  tft.setTextColor(RA8875_WHITE);
  tft.setTextSize(2);
  tft.setCursor(410,153+45);
  tft.print("14");
  tft.setCursor(530,153+45);
  tft.print("3");
  tft.setTextSize(1);
  tft.setCursor(516,150+45);
  tft.print(".");
  tft.setFont();
  tft.setTextColor(RA8875_BLACK);
  tft.setTextSize(2);
  tft.setCursor(460,190+50);
  tft.print("Temp `C");
  //Black strip for LED indicators
  tft.fillRect(212, 220+45, 376, 60, RA8875_RED);
  tft.fillRect(212+5, 220+5+45, 376-10, 60-10, RA8875_WHITE);
  //Air LED
  tft.fillEllipse(237, 250+45, 15, 15, RA8875_WHITE);
  tft.drawEllipse(237, 250+45, 15, 15, RA8875_BLACK);
  tft.setFont();
  tft.setTextColor(RA8875_BLACK);
  tft.setTextSize(2);
  tft.setCursor(262,242+45);
  tft.print("Air");
  //Start LED
  tft.fillEllipse(325+23, 250+45, 15, 15, RA8875_WHITE);
  tft.drawEllipse(325+23, 250+45, 15, 15, RA8875_BLACK);
  tft.setFont();
  tft.setTextColor(RA8875_BLACK);
  tft.setTextSize(2);
  tft.setCursor(350+23,242+45);
  tft.print("Start");
  //Heater LED
  tft.fillEllipse(425+57, 250+45, 15, 15, RA8875_WHITE);
  tft.drawEllipse(425+57, 250+45, 15, 15, RA8875_BLACK);
  tft.setFont();
  tft.setTextColor(RA8875_BLACK);
  tft.setTextSize(2);
  tft.setCursor(450+57,242+45);
  tft.print("Heater");
  //Vol. Reset Button
  tft.fillEllipse(262, 345+45, 50, 50, RA8875_RED);
  tft.fillEllipse(262, 345+45, 45, 45, RA8875_BLACK);
  tft.setFont();
  tft.setTextColor(RA8875_GREEN);
  tft.setTextSize(2);
  tft.setCursor(240,325+45);
  tft.print("Vol.");
  tft.setCursor(233,350+45);
  tft.print("Reset");
  //START-STOP Button
  tft.fillEllipse(412-12, 345+45, 50, 50, RA8875_RED);
  tft.fillEllipse(412-12, 345+45, 45, 45, RA8875_BLACK);
  tft.setFont();
  tft.setTextColor(RA8875_GREEN);
  tft.setTextSize(2);
  tft.setCursor(385-14,325+45);
  tft.print("START");
  tft.setCursor(390-14,350+45);
  tft.print("STOP");
  //CO2-Air Button
  //START-STOP Button
  tft.fillEllipse(538, 345+45, 50, 50, RA8875_RED);
  tft.fillEllipse(538, 345+45, 45, 45, RA8875_BLACK);
  tft.setFont();
  tft.setTextColor(RA8875_GREEN);
  tft.setTextSize(2);
  tft.setCursor(522,325+45);
  tft.print("CO2");
  tft.setCursor(522,350+45);
  tft.print("Air");
}
