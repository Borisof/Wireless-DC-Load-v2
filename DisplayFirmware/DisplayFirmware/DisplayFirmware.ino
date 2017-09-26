/***************************************************
This is an example sketch for the Adafruit 2.2" SPI display.
This library works with the Adafruit 2.2" TFT Breakout w/SD card
----> http://www.adafruit.com/products/1480

Check out the links above for our tutorials and wiring diagrams
These displays use SPI to communicate, 4 or 5 pins are required to
interface (RST is optional)
Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada for Adafruit Industries.
MIT license, all text above must be included in any redistribution
****************************************************/
#include <Wire.h>
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9340.h"
#include "TDataInfo.h"


#if defined(__SAM3X8E__)
#undef __FlashStringHelper::F(string_literal)
#define F(string_literal) string_literal
#endif
// These are the pins used for the UNO
#define _sclk 13
#define _miso 12
#define _mosi 11
#define _cs 10
#define _dc 9
#define _rst 8

Adafruit_ILI9340 tft = Adafruit_ILI9340(_cs, _dc, _rst);
TDataInfo OpData;
uint8_t  OldMode = -1;

static const uint8_t Bluetooth_Icon[]  PROGMEM =
{
	0x00, 0x03, 0xE0, 0x00, 0x00, 0x0C, 0x18, 0x00, 0x00, 0x10, 0x04, 0x00, 0x00, 0x21, 0x82, 0x00,
	0x00, 0x41, 0xC1, 0x00, 0x00, 0x81, 0xE0, 0x80, 0x01, 0x01, 0xF0, 0x40, 0x01, 0x01, 0x98, 0x40,
	0x02, 0x01, 0x8C, 0x40, 0x02, 0x21, 0x86, 0x20, 0x02, 0x71, 0x87, 0x20, 0x02, 0x39, 0x8E, 0x20,
	0x04, 0x1D, 0x9C, 0x10, 0x04, 0x0F, 0xF8, 0x10, 0x04, 0x07, 0xF0, 0x10, 0x04, 0x03, 0xE0, 0x10,
	0x04, 0x01, 0xC0, 0x10, 0x04, 0x03, 0xE0, 0x10, 0x04, 0x07, 0xF0, 0x10, 0x04, 0x0F, 0xF8, 0x10,
	0x02, 0x1D, 0x9C, 0x20, 0x02, 0x39, 0x8E, 0x20, 0x02, 0x71, 0x87, 0x20, 0x02, 0x21, 0x86, 0x20,
	0x01, 0x01, 0x8C, 0x40, 0x01, 0x01, 0x98, 0x40, 0x00, 0x81, 0xF0, 0x80, 0x00, 0x41, 0xE1, 0x00,
	0x00, 0x21, 0xC2, 0x00, 0x00, 0x11, 0x84, 0x00, 0x00, 0x0C, 0x18, 0x00, 0x00, 0x03, 0xE0, 0x00


};
static const uint8_t Timer_ICon[]  PROGMEM =
{
	0x00, 0x01, 0x80, 0x00, 0x0F, 0x83, 0xC1, 0xF0, 0x3F, 0x80, 0x01, 0xFC, 0x3F, 0x00, 0x00, 0xFC,
	0x7E, 0x0F, 0xF0, 0x7E, 0x7C, 0x3F, 0xFC, 0x3E, 0x78, 0xFF, 0xFF, 0x1E, 0x71, 0xF9, 0x9F, 0x8E,
	0x63, 0xE1, 0x87, 0xC6, 0x07, 0x81, 0x81, 0xE0, 0x07, 0x01, 0x80, 0xE0, 0x0F, 0x01, 0x80, 0xF0,
	0x0E, 0x01, 0x80, 0x70, 0x1C, 0x01, 0x80, 0x38, 0x1C, 0x01, 0x80, 0x38, 0x1C, 0x01, 0x80, 0x38,
	0x1F, 0x03, 0xC0, 0xF8, 0x1F, 0x03, 0xC0, 0xF8, 0x1C, 0x01, 0xE0, 0x38, 0x1C, 0x00, 0x70, 0x38,
	0x1C, 0x00, 0x38, 0x38, 0x1E, 0x00, 0x18, 0x78, 0x0E, 0x00, 0x08, 0x70, 0x0F, 0x00, 0x00, 0xF0,
	0x07, 0x80, 0x01, 0xE0, 0x03, 0xC1, 0x83, 0xC0, 0x01, 0xF1, 0x8F, 0x80, 0x00, 0xFF, 0xFF, 0x00,
	0x00, 0xFF, 0xFF, 0x00, 0x00, 0xDF, 0xFB, 0x00, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80
};
static const uint8_t Temprature_Icon[]  PROGMEM =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x09, 0x00, 0x10, 0x80, 0x10, 0x80, 0x16, 0x9C,
	0x16, 0x80, 0x16, 0x80, 0x16, 0x9C, 0x16, 0x80, 0x16, 0x80, 0x16, 0x9C, 0x16, 0x80, 0x16, 0x80,
	0x16, 0x9C, 0x16, 0x80, 0x16, 0x80, 0x16, 0x9C, 0x16, 0x80, 0x16, 0x80, 0x36, 0x80, 0x26, 0x40,
	0x26, 0x40, 0x2F, 0x40, 0x26, 0x40, 0x20, 0xC0, 0x19, 0x80, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00
};
static const uint8_t Power_Icon[]  PROGMEM =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xF8, 0x00, 0x00, 0x7F, 0xFF, 0x00, 0x01, 0xF0, 0x0F, 0xC0,
	0x03, 0xC0, 0x01, 0xE0, 0x07, 0x00, 0xF8, 0xF0, 0x0E, 0x00, 0xF0, 0x78, 0x1C, 0x01, 0xF0, 0x3C,
	0x18, 0x01, 0xE0, 0x0C, 0x38, 0x03, 0xE0, 0x0E, 0x30, 0x03, 0xC0, 0x06, 0x70, 0x07, 0xC0, 0x06,
	0x60, 0x07, 0x80, 0x03, 0x60, 0x0F, 0xF8, 0x03, 0x60, 0x0F, 0xF8, 0x03, 0x60, 0x0F, 0xF8, 0x03,
	0x60, 0x1F, 0xF0, 0x03, 0x60, 0x1E, 0xF0, 0x03, 0x60, 0x00, 0xE0, 0x03, 0x60, 0x01, 0xC0, 0x03,
	0x60, 0x01, 0xC0, 0x07, 0x70, 0x03, 0x80, 0x06, 0x30, 0x03, 0x80, 0x06, 0x38, 0x0F, 0x00, 0x0E,
	0x18, 0x0F, 0x80, 0x1C, 0x1E, 0x0F, 0x80, 0x38, 0x0F, 0x0F, 0x00, 0x78, 0x07, 0x8C, 0x00, 0xF0,
	0x03, 0xE0, 0x03, 0xE0, 0x01, 0xF8, 0x1F, 0x80, 0x00, 0x3F, 0xFE, 0x00, 0x00, 0x0F, 0xF0, 0x00
};
static const uint8_t Current_Icon[]  PROGMEM =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xFC, 0x00, 0x00, 0x7F, 0xFF, 0x00, 0x01, 0xF0, 0x0F, 0xC0,
	0x03, 0xC0, 0x01, 0xE0, 0x07, 0x80, 0x00, 0xF0, 0x0E, 0x00, 0x80, 0x38, 0x1C, 0x01, 0xC0, 0x1C,
	0x1C, 0x03, 0xE0, 0x1C, 0x38, 0x03, 0xE0, 0x0E, 0x30, 0x07, 0xF0, 0x06, 0x70, 0x00, 0x80, 0x07,
	0x60, 0x00, 0x80, 0x07, 0x60, 0x00, 0x80, 0x03, 0x60, 0x00, 0x80, 0x03, 0x60, 0x00, 0x80, 0x03,
	0x60, 0x00, 0x80, 0x03, 0x60, 0x00, 0x80, 0x03, 0x60, 0x00, 0x80, 0x03, 0x60, 0x00, 0x80, 0x03,
	0x60, 0x00, 0x80, 0x03, 0x70, 0x00, 0x80, 0x07, 0x30, 0x00, 0x80, 0x06, 0x38, 0x00, 0x80, 0x0E,
	0x1C, 0x00, 0x80, 0x1C, 0x1C, 0x00, 0x80, 0x1C, 0x0E, 0x00, 0x80, 0x38, 0x07, 0x80, 0x00, 0xF0,
	0x03, 0xC0, 0x01, 0xE0, 0x01, 0xF0, 0x07, 0xC0, 0x00, 0x7F, 0xFF, 0x00, 0x00, 0x1F, 0xFC, 0x00
};
static const uint8_t Resistance_Icon[]  PROGMEM =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xFC, 0x00, 0x00, 0xF8, 0x0F, 0x00, 0x01, 0xC0, 0x01, 0xC0,
	0x03, 0x00, 0x00, 0xE0, 0x06, 0x00, 0x00, 0x30, 0x0C, 0x00, 0x08, 0x18, 0x18, 0x00, 0x1C, 0x0C,
	0x30, 0x00, 0x3C, 0x0C, 0x30, 0x00, 0x3C, 0x06, 0x20, 0x00, 0x3C, 0x02, 0x60, 0x00, 0x30, 0x03,
	0x40, 0x82, 0x68, 0x33, 0x41, 0xC3, 0x6C, 0x31, 0xC1, 0xC7, 0xDC, 0x71, 0xC1, 0x67, 0xDE, 0x79,
	0xFB, 0x6D, 0xB6, 0xDF, 0xCE, 0x39, 0xF3, 0xC1, 0x4E, 0x3B, 0xE3, 0x81, 0x44, 0x1B, 0x61, 0x83,
	0x64, 0x16, 0x41, 0x03, 0x60, 0x06, 0x00, 0x02, 0x20, 0x0C, 0x00, 0x06, 0x30, 0x0C, 0x00, 0x06,
	0x18, 0x18, 0x00, 0x0C, 0x1C, 0x18, 0x00, 0x18, 0x0C, 0x00, 0x00, 0x30, 0x07, 0x00, 0x00, 0x70,
	0x03, 0x80, 0x01, 0xC0, 0x00, 0xE0, 0x07, 0x80, 0x00, 0x3F, 0xFE, 0x00, 0x00, 0x07, 0xE0, 0x00
};

void receiveEvent(int bytes) {
	String ret[8];
	int j = 0;
	ret[j] = "";
	char c;
	int pageNumber;


	if (Wire.available())
	{
		pageNumber = Wire.read() - '0';
		Wire.read();
	}

	while (Wire.available())
	{
		c = Wire.read();
		//	Serial.print(c);



		if (c == ':')
		{
			j++;
			ret[j] = "";
			continue;
		}
		ret[j] += c;

	}
	//Serial.println("");

	switch (pageNumber)
	{
	case 1:
		OpData.SetCurrent = atof(ret[0].c_str());
		OpData.SetPower = atof(ret[1].c_str());
		OpData.SetResistance = atof(ret[2].c_str());
		OpData.CurrentSensed = atof(ret[3].c_str());
		OpData.VoltageSensed = atof(ret[4].c_str());
		OpData.PowerSensed = OpData.CurrentSensed * OpData.VoltageSensed;
		if (OpData.CurrentSensed > 0)
			OpData.ResistanceSensed = OpData.VoltageSensed / OpData.CurrentSensed;
		else
			OpData.ResistanceSensed = -1;
	case 2:
		OpData.TempratureSensed = atof(ret[0].c_str());
		OpData.LoadState = atof(ret[1].c_str());
		OpData.TimeStamp = atof(ret[2].c_str());
		OpData.Mode = atof(ret[3].c_str());
		OpData.BTState = atof(ret[4].c_str());
		break;
	}
}

void DisplayData(TDataInfo);
void InitDisplay()
{
	tft.fillScreen(ILI9340_WHITE);
	DisplayLayout();
	OpData.CurrentSensed = 0.0;
	OpData.PowerSensed = 0.0;
	OpData.VoltageSensed = 0.0;
	OpData.ResistanceSensed = -1;
	OpData.TempratureSensed = 0.0;
	OpData.SetPower = 1.54;
	OpData.SetResistance = 0.0;
	OpData.Mode = 0;
	DisplayData(OpData);
	ApplyModeLayout(OpData);
}
void setup() {
	Wire.begin(1);
	Wire.onReceive(receiveEvent);
	tft.begin();
	tft.setRotation(1);
	//Serial.begin(9600);
	InitDisplay();
	delay(10);
}
double val = 0;


void loop(void) {

	if (OldMode == -1)OldMode = OpData.Mode;
	else if (OpData.Mode != OldMode)
	{

		ApplyModeLayout(OpData);
		OldMode = OpData.Mode;
		return;
	}
	DisplayData(OpData);
}
bool cleared = 0;
void ApplyModeLayout(TDataInfo exData)
{



	cleared = 0;
	//tft.fillRect(1, 1, 200, 89, ILI9340_BLACK);//Main Display clear
	tft.fillRect(5, 165, 175, 20, ILI9340_BLACK);

	switch (exData.Mode)
	{
	case 1:
		tft.fillRect(226, 165, 90, 19, ILI9340_BLACK);
		DrawNumber(10, 10, "Current", ILI9340_WHITE, ILI9340_BLACK, 3, 12);
		DrawNumber(185, 57, "A", ILI9340_WHITE, ILI9340_BLACK, 3, 2);

		DrawNumber(79, 95, "Power", ILI9340_WHITE, ILI9340_BLACK, 1, 12);
		DrawNumber(153, 95, "Resistance", ILI9340_WHITE, ILI9340_BLACK, 1, 12);
		DrawNumber(141, 114, "W", ILI9340_WHITE, ILI9340_BLACK, 1, 1);
		DrawNumber(215, 114, String(char(233)), ILI9340_WHITE, ILI9340_BLACK, 1, 1);
		tft.fillRect(4, 198, 32, 32, ILI9340_BLACK);
		tft.drawBitmap(4, 198, Current_Icon, 32, 32, 0x34DF);
		break;
	case 2:
		tft.fillRect(226, 105, 90, 19, ILI9340_BLACK);
		DrawNumber(150, 43, " ", ILI9340_WHITE, ILI9340_BLACK, 5, 1);
		DrawNumber(10, 10, "Power", ILI9340_WHITE, ILI9340_BLACK, 3, 12);
		DrawNumber(185, 57, "W", ILI9340_WHITE, ILI9340_BLACK, 3, 2);

		DrawNumber(79, 95, "Current", ILI9340_WHITE, ILI9340_BLACK, 1, 12);
		DrawNumber(153, 95, "Resistance", ILI9340_WHITE, ILI9340_BLACK, 1, 12);
		DrawNumber(141, 114, "A", ILI9340_WHITE, ILI9340_BLACK, 1, 1);
		DrawNumber(215, 114, String(char(233)), ILI9340_WHITE, ILI9340_BLACK, 1, 1);
		tft.fillRect(4, 198, 32, 32, ILI9340_BLACK);
		tft.drawBitmap(4, 198, Power_Icon, 32, 32, 0xCE8C);

		break;
	case 3:
		tft.fillRect(226, 135, 90, 19, ILI9340_BLACK);
		DrawNumber(127, 107, " ", ILI9340_WHITE, ILI9340_BLACK, 2, 1);
		DrawNumber(10, 10, "Resistance", ILI9340_WHITE, ILI9340_BLACK, 3, 12);
		DrawNumber(185, 57, String(char(233)), ILI9340_WHITE, ILI9340_BLACK, 3, 2);

		DrawNumber(79, 95, "Power", ILI9340_WHITE, ILI9340_BLACK, 1, 12);
		DrawNumber(153, 95, "Current", ILI9340_WHITE, ILI9340_BLACK, 1, 12);
		DrawNumber(141, 114, "W", ILI9340_WHITE, ILI9340_BLACK, 1, 1);
		DrawNumber(215, 114, "A", ILI9340_WHITE, ILI9340_BLACK, 1, 1);
		tft.fillRect(4, 198, 32, 32, ILI9340_BLACK);
		tft.drawBitmap(4, 198, Resistance_Icon, 32, 32, 0xF81F);
		break;
	}

}

void DisplayLayout()
{
	tft.fillRect(0, 0, 319, 239, ILI9340_BLACK);
	uint16_t LineColor = ILI9340_WHITE;
	tft.drawRect(0, 0, tft.width(), tft.height(), LineColor);
	tft.drawFastHLine(1, 90, 222, LineColor);
	tft.drawFastVLine(74, 91, 35, LineColor);
	tft.drawFastVLine(148, 91, 35, LineColor);
	tft.drawFastHLine(1, 126, 222, LineColor);
	tft.drawFastHLine(1, 190, tft.width() - 2, LineColor);
	tft.drawFastVLine(223, 1, 189, LineColor);
	tft.fillRect(1, 127, 222, 63, ILI9340_BLACK);
	tft.drawRect(233, 55, 74, 31, LineColor);
	tft.fillRect(233, 42, 30, 13, LineColor);
	tft.drawFastVLine(280, 190, 49, LineColor);
	tft.drawFastVLine(40, 190, 49, LineColor);

	//	tft.setTextColor(LineColor);
	//	tft.setTextSize(3);

	//tft.setCursor(10, 10); tft.println("Current");
	//tft.setCursor(185, 57); tft.println("A");

	tft.setTextSize(1);
	tft.setCursor(5, 95); tft.println("Voltage");
	tft.setCursor(65, 114); tft.println("V");
	tft.setCursor(228, 95); tft.println("Current Set");
	tft.setCursor(228, 125); tft.println("Power Set");
	tft.setCursor(228, 155); tft.println("Resistance Set");
	tft.setCursor(243, 5); tft.println("Temprature");
	tft.setTextColor(ILI9340_BLACK); tft.setCursor(236, 45); tft.println("Load");

	//------------ Load Status -----------
	tft.fillRect(236, 56, 68, 28, ILI9340_BLACK);
	tft.drawBitmap(225, 3, Temprature_Icon, 16, 32, ILI9340_GREEN);
}
String floatToEngineering(double x)
{
	if (x == 0)return "0.00";
	static char *prefix[] = {
		"m", "",
		"k", "M", "G" };

	int exp = 0, sign = 1;
	if (x < 0.0) {
		x = -x;
		sign = -sign;
	}
	while (x >= 1000.0) {
		x /= 1000.0;
		exp += 3;
	}
	while (x < 1.0) {
		x *= 1000.0;
		exp -= 3;
	}
	int small = 0;
	if (sign < 0)
		x = -x;
	if (exp == -3)small = 1;
	int dec = 2;
	if (x >= 100)dec = 1;
	else if (x >= 10)dec = 1;
	return String(x, dec - small) + prefix[(exp / 3) + 1];
}
void DrawNumber(int16_t x, int16_t y, double value, uint8_t decimalPlaces, uint16_t forcolor, uint8_t textsize)
{
	String myString = String(value, decimalPlaces);
	for (int i = 0; i < myString.length(); i++)
		tft.drawChar(x + (6 * textsize * i), y, myString[i], forcolor, ILI9340_BLACK, textsize);
}

void DrawNumber(int16_t x, int16_t y, String value, uint16_t forcolor, uint16_t bgcolor, uint8_t textsize, uint8_t MaxLen)
{

	for (int s = 0; s < MaxLen - value.length(); s++)value += " ";
	for (int i = 0; i < value.length(); i++)
		tft.drawChar(x + (6 * textsize * i), y, value[i], forcolor, bgcolor, textsize);
}
bool blinky = false;
String GetTimeStampString(unsigned long elapsed)
{
	int  h, m, s, ms;
	unsigned long over;
	char TimeStampString[20];

	h = int(elapsed / 36000);
	over = elapsed % 36000;
	m = int(over / 600);
	over = over % 600;
	s = int(over / 10);
	ms = over % 10;
	ms /= 1;
	sprintf(TimeStampString, "%02i:%02i:%02i", h, m, s);
	return String(TimeStampString);
}
void DisplayData(TDataInfo dataEx) {

	uint16_t LineColor = ILI9340_WHITE;
	DrawNumber(3, 107, String(dataEx.VoltageSensed, 2), 0x3D8E, ILI9340_BLACK, 2, 5);
	switch (dataEx.Mode)
	{
	case 1:
		DrawNumber(30, 43, dataEx.CurrentSensed, 3, 0x34DF, 5);
		DrawNumber(80, 107, String(dataEx.PowerSensed, 2), 0xCE8C, ILI9340_BLACK, 2, 5);


		if (dataEx.ResistanceSensed < 0){

			if (!cleared){
				tft.fillRect(153, 104, 60, 20, ILI9340_BLACK);

				cleared = true;
			}
			tft.drawChar(168, 102, 'o', 0xF81F, ILI9340_BLACK, 3);
			tft.drawChar(183, 102, 'o', 0xF81F, ILI9340_BLACK, 3);
		}
		else {
			DrawNumber(155, 107, floatToEngineering(dataEx.ResistanceSensed), 0xF81F, ILI9340_BLACK, 2, 5);
			cleared = false;
		}
		break;
	case 2:
		DrawNumber(80, 107, dataEx.CurrentSensed, 3, 0x34DF, 2);
		DrawNumber(30, 43, String(dataEx.PowerSensed, 2), 0xCE8C, ILI9340_BLACK, 5, 5);
		if (dataEx.ResistanceSensed < 0){

			if (!cleared){
				tft.fillRect(153, 104, 60, 20, ILI9340_BLACK);

				cleared = true;
			}
			tft.drawChar(168, 102, 'o', 0xF81F, ILI9340_BLACK, 3);
			tft.drawChar(183, 102, 'o', 0xF81F, ILI9340_BLACK, 3);
		}
		else {
			DrawNumber(155, 107, floatToEngineering(dataEx.ResistanceSensed), 0xF81F, ILI9340_BLACK, 2, 5);
			cleared = false;
		}
		break;
	case 3:
		DrawNumber(155, 107, dataEx.CurrentSensed, 3, 0x34DF, 2);
		DrawNumber(80, 107, dataEx.PowerSensed, 2, 0xCE8C, 2);


		if (dataEx.ResistanceSensed < 0){

			if (!cleared){
				tft.fillRect(30, 35, 145, 50, ILI9340_BLACK);

				cleared = true;
			}
			tft.drawChar(75, 35, 'o', 0xF81F, ILI9340_BLACK, 6);
			tft.drawChar(104, 35, 'o', 0xF81F, ILI9340_BLACK, 6);
		}
		else {
			DrawNumber(30, 43, floatToEngineering(dataEx.ResistanceSensed), 0xF81F, ILI9340_BLACK, 5, 5);
			cleared = false;
		}
		break;
	}



	tft.setTextSize(2);

	DrawNumber(244, 20, dataEx.TempratureSensed, 1, ILI9340_GREEN, 2);
	tft.drawChar(291, 15, 'o', ILI9340_GREEN, ILI9340_BLACK, 1);
	tft.setTextColor(ILI9340_GREEN); tft.setCursor(289, 20);  tft.println(" C"); //Temprature Sensed Value

	DrawNumber(230, 105, (String(dataEx.SetCurrent, 3) + " A").c_str(), 0x34DF, ILI9340_BLACK, (dataEx.Mode == 1 ? 2 : 1), 7);
	DrawNumber(230, 135, (String(dataEx.SetPower, 2) + " W").c_str(), 0xCE8C, ILI9340_BLACK, (dataEx.Mode == 2 ? 2 : 1), 7);
	DrawNumber(230, 165, (floatToEngineering(dataEx.SetResistance) + " " + char(233)).c_str(), 0xF81F, ILI9340_BLACK, (dataEx.Mode == 3 ? 2 : 1), 7);
	DrawNumber(108, 204, GetTimeStampString(dataEx.TimeStamp), (dataEx.LoadState == 1 ? ILI9340_YELLOW : 0x7BEF), ILI9340_BLACK, 3, 8);
	DisplayIcons();
	switch (dataEx.LoadState)
	{
	case 0:
		DrawNumber(244, 60, "OFF", 0x7BEF, ILI9340_BLACK, 3, 3);
		break;
	case 1:
		DrawNumber(234, 60, " ON", ILI9340_RED, ILI9340_BLACK, 3, 4);
		break;
	case 2:
		if (blinky)
			DrawNumber(244, 60, "HOT", ILI9340_RED, ILI9340_BLACK, 3, 3);
		else
			DrawNumber(244, 60, "   ", ILI9340_RED, ILI9340_BLACK, 3, 3);
		blinky = !blinky;
		break;
	}

	/*tft.setTextColor(ILI9340_RED);
	tft.setCursor(253, 60);
	tft.println("ON");*/
	tft.setTextColor(ILI9340_WHITE);
	tft.setCursor(10, 134);
	tft.setTextSize(2);
	tft.print("Mode:");
	uint16_t ModeColor;
	String ModeText = "";
	tft.setTextSize(1);
	switch (dataEx.Mode)
	{
	case 1://Constant Current
		ModeText = "Current";
		ModeColor = 0x34DF;
		tft.setTextColor(ModeColor);
		tft.setCursor(10, 165);
		tft.println("- Max Current:      Amps.");
		DrawNumber(95, 165, String(min(30.0 / dataEx.VoltageSensed, 2.0), 3), ModeColor, ILI9340_BLACK, 1, 5);

		break;
	case 2://Constant Power
		ModeText = "Power";
		ModeColor = 0xCE8C;
		tft.setTextColor(ModeColor);
		tft.setCursor(10, 165);
		tft.println("- Max Power:     WATTs.");
		DrawNumber(85, 165, String(min(dataEx.VoltageSensed*2.0, 30.0), 1), ModeColor, ILI9340_BLACK, 1, 4);

		break;
	case 3://constant Resistance
		ModeText = "Resistance";
		ModeColor = 0xF81F;
		tft.setTextColor(ModeColor);
		tft.setCursor(10, 165);
		tft.println("- Min Resistance:      " + String(char(233)) + ".");
		DrawNumber(113, 165, floatToEngineering(max((dataEx.VoltageSensed*dataEx.VoltageSensed) / 30.0, (dataEx.VoltageSensed / 2.0))), ModeColor, ILI9340_BLACK, 1, 5);

		break;
	}
	tft.setCursor(10, 175);
	tft.println("- Max Temprature : 95.0" + String(char(247)) + " C.");




	tft.setTextColor(ILI9340_WHITE);
	tft.setCursor(10, 154);

	tft.print("Parameters:");



	DrawNumber(70, 134, ModeText, ModeColor, ILI9340_BLACK, 2, 12);


	//tft.setCursor(10, 160);	tft.println("bluetooth status, current \n opertional mode, warrnings or maybe \n a graph or two. also fan status.");

}

uint8_t oldBT = -1, oldLoad = -1;

void DisplayIcons()
{
	if (oldBT != OpData.BTState)
	{
		oldBT = OpData.BTState;
		tft.drawBitmap(284, 198, Bluetooth_Icon, 32, 32, (OpData.BTState == 1 ? ILI9340_BLUE : 0x7BEF));
	}

	if (oldLoad != OpData.LoadState)
	{
		oldLoad = OpData.LoadState;
		tft.drawBitmap(68, 199, Timer_ICon, 32, 32, (OpData.LoadState == 1 ? ILI9340_YELLOW : 0x7BEF));
	}




}
