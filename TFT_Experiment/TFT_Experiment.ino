// This sketch has been Refurbished by BUHOSOFT
// IMPORTANT: Adafruit_TFTLCD LIBRARY MUST BE SPECIFICALLY
// CONFIGURED FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.
// SEE RELEVANT COMMENTS IN Adafruit_TFTLCD.h FOR SETUP.
//#define DEBUG
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <math.h>
#include <Fonts/FreeSansBold24pt7b.h>
// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// When using the BREAKOUT BOARD only, use these 8 data lines to the LCD:
// For the Arduino Uno, Duemilanove, Diecimila, etc.:
//   D0 connects to digital pin 8  (Notice these are
//   D1 connects to digital pin 9   NOT in order!)
//   D2 connects to digital pin 2
//   D3 connects to digital pin 3
//   D4 connects to digital pin 4
//   D5 connects to digital pin 5
//   D6 connects to digital pin 6
//   D7 connects to digital pin 7
// For the Arduino Mega, use digital pins 22 through 29
// (on the 2-row header at the end of the board).

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define PI 3.1416

int i, x, y, r, theta = 15;
String Message = "0123456789";//"MERA BHARAT MAHAN...!";

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
// If using the shield, all control and data lines are fixed, and
// a simpler declaration can optionally be used:
// Adafruit_TFTLCD tft;

void setup(void) {
  Serial.begin(9600);
  
  tft.reset();

  uint16_t identifier = tft.readID();

  tft.begin(identifier);
  
  tft.fillScreen(WHITE);
  tft.setCursor(0,0);
  tft.setTextColor(BLACK);
  tft.setTextSize(2);
  //tft.print("Nitin N. Sangale");
  //Draw Indian flag
  tft.setRotation(1);
  tft.fillRect(0,0,tft.width(),tft.height()/3,RED);
  tft.fillRect(0,tft.height()/3, tft.width(),2*tft.height()/3,WHITE);
  tft.fillRect(0,2*tft.height()/3, tft.width(),tft.height(), GREEN);
  
  tft.drawCircle(tft.width()/2, tft.height()/2, tft.height()/6, BLUE);
  
  r = tft.height()/6;
  
  for(i = 0; i <= 360; i = i+theta)
	{
		x = round(sin(i*PI/180)*r + tft.width()/2);
		y = round(cos(i*PI/180)*r + tft.height()/2);
		tft.drawLine(tft.width()/2,tft.height()/2, x, y, BLUE);
		//delay(1000);
	}
  
  //tft.setCursor(40, tft.height()/2 + tft.height()/3);
  tft.setFont(&FreeSansBold24pt7b);
  tft.setCursor(10, 70);
  //tft.setTextSize(4);
  tft.println(Message);
  //tft.drawCircle(0,0,10,BLUE);
}


void loop() {
  // put your main code here, to run repeatedly:
	/*
	tft.fillScreen(WHITE);
	delay(100);
	for(x = 0; x<tft.width();x++)
		{
			for(y = 0; y<tft.height(); y++)
				{
					tft.drawPixel(x,y,BLUE);
					delay(10);
				}
		}
	
	tft.fillRect(0,2*tft.height()/3, tft.width(),tft.height(), GREEN);
	delay(1000);
	tft.setCursor(40, tft.height()/2 + tft.height()/3);//tft.height()/2 + tft.height()/3);
	tft.println(Message);
	delay(1000);*/
}
