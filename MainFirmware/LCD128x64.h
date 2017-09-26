
#ifndef LCD128x64_h
#define LCD128x64_h
bool Toggle = true;
class TLCD
{
private:
	U8GLIB_ST7920_128X64_1X *u8g;
	char * GetTimeStampString(unsigned long elapsed)
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
		return TimeStampString;
	}

public:
	
	TLCD(uint8_t SCK, uint8_t MOSI, uint8_t CS){ u8g = new U8GLIB_ST7920_128X64_1X(SCK, MOSI, CS); }// SPI Com: SCK = en = 4, MOSI = rw = 5, CS = di = 6
	void DrawTimerIcon(void) {
		#define TimerIcon_width 16
		#define TimerIcon_height 11	
		//0x1c, 0x3e, 0x49, 0x49, 0x7b, 0x41, 0x49, 0x3e
		static unsigned char TimerIcon_bits[] U8G_PROGMEM = {
			0xE0, 0x00, 0xFC, 0x07,
			0x42, 0x08,	0x02, 0x08,
			0x82, 0x08, 0x46, 0x0C,
			0x46, 0x0C, 0x82, 0x08,
			0x02, 0x09, 0x42, 0x08,
			0xFC, 0x07
		 };
		u8g->drawXBMP(1, 51, TimerIcon_width, TimerIcon_height, TimerIcon_bits);
	}
	void DrawLoadTitle(void) {
		#define LoadTitle_width 17
		#define LoadTitle_height 7
		static unsigned char LoadTitle_bits[] U8G_PROGMEM = {
			0xff, 0xff, 0x01, 0xfd, 0x7f, 0x01, 0xbd, 0x33, 0x01, 0x5d, 0x55, 0x01,
			0x5d, 0x55, 0x01, 0xb1, 0x33, 0x01, 0xff, 0xff, 0x01 };
		u8g->drawXBMP(92, 15, LoadTitle_width, LoadTitle_height, LoadTitle_bits);
	}
	void drawTempIcon(void) {
		#define TempIconWidth  5
		#define  TempIconHeight  10
		static unsigned char TempIcon[] U8G_PROGMEM = {
			0x0e, 0x1a, 0x0e, 0x1a, 0x0a, 0x1a, 0x0a, 0x11, 0x11, 0x0e };  
		u8g->drawXBMP(85, 2, TempIconWidth, TempIconHeight, TempIcon);
	}
	void drawBtIcon(void){
		#define BTIconWidth  8
		#define  BTIconHeight  13
		//0x10, 0x34, 0x58, 0x30, 0x58, 0x34, 0x10 };
		static unsigned char BTIcon_bits[] U8G_PROGMEM = {
			0xFF,0xF7, 0xE7, 0xD7, 0xB5, 0xD3, 0xE7, 0xD3, 0xB5, 0xD7, 0xE7, 0xF7, 0xFF};
		u8g->drawXBMP(74, 50, BTIconWidth, BTIconHeight, BTIcon_bits);
	}
	void Clear()
	{
		u8g->firstPage();
		while (u8g->nextPage());
	}
	void Test(char *str)
	{
		u8g->firstPage();
		do
		{
			u8g->setFont(u8g_font_5x8r);
			u8g->drawStr(8, 24, str);
		}
		while (u8g->nextPage());
	}
	void RunLCD(double VoltageVal, double CurrentVal, double PowerVal, double CurrentSetVal, double TempVal, short int LoadState, unsigned long  elapsed, short int BT_State)
	{
		char buf[7], Current[7], Voltage[7], Power[7], CurrentSet[7],Temp[7], Time[7];
		dtostrf(CurrentVal, 5, 3, Current);
		dtostrf(CurrentSetVal, 5, 3, CurrentSet);
		if (VoltageVal < 10)dtostrf(VoltageVal, 6, 2, Voltage);
		else dtostrf(VoltageVal, 5, 2, Voltage);
		if (PowerVal < 10)dtostrf(PowerVal, 6, 2, Power);
		else dtostrf(PowerVal, 5, 2, Power);
		if (TempVal < 100)dtostrf(TempVal, 3, 1, Temp);
		else dtostrf(TempVal, 3, 0, Temp);
		
		u8g->firstPage();
		do {
			
			//----------------------------- Borders ------------------------------
			u8g->drawFrame(0, 0, 128, 64);	//border
			u8g->drawLine(1, 29, 83, 29);	//Current Sense Box
			u8g->drawLine(1, 49, 83, 49);	//Middle Vertical Line
			u8g->drawLine(41, 30, 41, 48);	//Voltage and Current Separator
			u8g->drawLine(83, 0, 83, 62);	//Time stamp box
			u8g->drawFrame(92, 22, 27, 16); //Current state box
			//------------------------ Titles and Units --------------------------
			u8g->setFont(u8g_font_5x8r);
			u8g->drawStr(4, 8, "Current");	//Current Title
			u8g->drawStr(3, 37, "Voltage");	//Voltage Title
			u8g->drawStr(44, 37, "Power");	//Power Title
			u8g->drawStr(119, 59, "A");		//Current Set Unit
			u8g->drawStr(75, 47, "W");		//Power Unit
			u8g->drawStr(33, 47, "V");		//Voltage Unit
			u8g->drawStr(70, 27, "A");		//current Sense Unit
			
			//------------------------ Current Sense -----------------------------
			u8g->setFont(u8g_font_fub17n);
			u8g->drawStr(9, 27, Current);
			//------------------- Voltage Sense and Power ------------------------
			u8g->setFont(u8g_font_helvB08r);
			u8g->drawStr(4, 47, Voltage);
			u8g->drawStr(46, 47, Power);
			//---------------------------- Timer ---------------------------------
			u8g->drawStr(16, 61, GetTimeStampString(elapsed));
			DrawTimerIcon();
			//------------------------ Load Status -------------------------------
			DrawLoadTitle();
			if (LoadState == 1) u8g->drawStr(98, 34, "ON");
			else if (LoadState == 0) u8g->drawStr(96, 34, "OFF");
			else if (Toggle)u8g->drawStr(95, 34, "HOT");		
			//--------------------------- Temprature -----------------------------
			if (LoadState<2 || Toggle) drawTempIcon(); //Blink when hot
			int UnitPoz = 115;
			if (TempVal >= 100 || TempVal<10)UnitPoz = 109;			
			u8g->setFont(u8g_font_6x10);//remove this to save memory (6%)
			u8g->drawStr(92, 11, Temp);
			char TempUnit[2] = { 176, '\0' };
			u8g->drawStr(UnitPoz, 9, TempUnit);
			u8g->drawStr(UnitPoz + 5, 11, "C");
			u8g->drawStr(87, 49, "Set");
			u8g->drawStr(88, 59, CurrentSet);	
			//--------------------------- Bluetooth Icon -------------------------
			if (BT_State)
			{
				drawBtIcon();
				u8g->drawLine(72, 50, 72, 62);
				u8g->drawLine(73, 50, 73, 62);
				u8g->drawLine(82, 50, 82, 62);
			}
				
		} while (u8g->nextPage());
		Toggle = !Toggle;
	}
};
#endif

