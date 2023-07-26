#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include "TouchScreen.h"

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

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

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
TSPoint p;

Adafruit_GFX_Button buttonAdd, buttonSub;
int x = 0;
boolean screenTouched = false;
char screenDebouncer = 0;

void setup(void) {
	Serial.begin(9600);
	
	tft.reset();

	uint16_t identifier = tft.readID();
	pinMode(13,OUTPUT);
	tft.begin(identifier);
	//tft.setRotation(2);
	tft.fillScreen(WHITE);
	buttonAdd.initButton(&tft,125,30,50,50,BLACK,LIGHT_GRAY,WHITE,"ADD",2);
	buttonAdd.drawButton(NON_INVERTED, ROUND_BUTTON);
	buttonSub.initButton(&tft,125,tft.height()-30,50,50,BLACK,LIGHT_GRAY,WHITE,"SUB",2);
	buttonSub.drawButton(NON_INVERTED, ROUND_BUTTON);
	tft.drawSevenSegNumber(x,2,70,100,80,8,BLUE);
	p.x = -1;
	p.y = -1;
	p.z = -1;
	screenDebouncer = 0;
	screenTouched = false;
}

#define MINPRESSURE 100
#define MAXPRESSURE 1000

void loop() {
	digitalWrite(13, HIGH);
	p = ts.getPoint();
	digitalWrite(13, LOW);

	pinMode(XM, OUTPUT);
	pinMode(YP, OUTPUT);
	
	if (p.z > MINPRESSURE && p.z < MAXPRESSURE) 
		{
			screenTouched = true;
			screenDebouncer = 0;
			p.x = tft.width()-(map(p.x, TS_MINX, TS_MAXX, tft.width(), 0));
			p.y = tft.height()-(map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
		}
	else
		{
			if(screenTouched)
				{
					screenDebouncer++;
					if(screenDebouncer > 50)
						{
							p.x = -1;
							p.y = -1;
							p.z = -1;
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
					if(buttonAdd.contains(p.x,p.y))
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
					if(buttonSub.contains(p.x,p.y))
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
