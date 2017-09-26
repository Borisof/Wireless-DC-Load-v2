/********************************************************************************************************/
/*																										*/
/*									Humber College - Technical Project									*/
/*																										*/
/*		Author:		Sam Farah																			*/
/*		Student ID: *********																			*/
/*		Project:	DC Electronic Load Firmware															*/
/*		Version:	2.0																					*/
/*		Description:																					*/
/*				The firmware for the ATMega microcontroller on board, it controls						*/
/*				the analog circuit, and handles reading inputs and sensors, updating LCD display		*/
/*				also handles bluetooth communications													*/
/*																										*/
/********************************************************************************************************/
//-------------------------- Headers ----------------------------
#include <Wire.h>
#include <RotaryEncoder.h>
#include <MCP3208.h>
#include <DAC_MCP49xx.h>
#include <SPI.h>
#include "Buzzer.h"
//#include "U8glib.h"
//#include "LCD128x64.h"
#include "Multithreading.h"
#include <SoftwareSerial.h>
//---------------------------------------------------------------
//---------------------------- Pins -----------------------------
//			Name			Arduino Pin				Pin on ATMega
//		-----------			-----------				-------------
#define StatusLED				0			//			2
#define ZeroCurrent				1			//			3
#define Rotary_A				2			//			4 
#define Rotary_B				3			//			5 
//#define LCD_SCK				4			//			6 
#define Mode_SW					5			//			11			
//#define LCD_MOSI				5			//			11
//#define LCD_CS				6			//			12
#define LoadSwitch				7			//			13
#define BuzzerPin				8			//			14
#define ADC_CS					9			//			15
#define DAC_CS					10			//			16
#define MOSI					11			//			17
#define MISO					12			//			18
#define SCK						13			//			19
#define BT_TX					A0			//			23
#define BT_RX					A1			//			24
#define BT_EN					A2			//			25
#define BT_SW					A3			//			26
//#define DisplayBL				A4			//			27

//---------------------------------------------------------------
//------------------------- Settings ----------------------------
#define MaxTemp					95
#define CoolingWindow			5
#define MaxCurrent				2
#define MaxPower				30.0
#define VRef					4.101
//---------------------------------------------------------------
//----------------------- Prototypes ----------------------------
void ChangeMode();
void ReadSwitches();
void ReadEncoder();
//---------------------------------------------------------------
//------------------------- Threads -----------------------------
void AcquireData();
void UpdateLCD();
void SendToTFT();
void BuzzerWarning();
void BlueToothTX();
void BlueToothRX();
void ErrorMode();

TThread
AcruireDataThread(AcquireData, 100U),
//UpdateLCDThread(UpdateLCD, 500U),
SendToTFTThread(SendToTFT, 300U),
BuzzerWarningThread(BuzzerWarning, 200U),
BluetoothTXThreat(BlueToothTX, 500U),
BluetoothRXThreat(Bluetooth_RX, 250U);
//---------------------------------------------------------------
//------------------------- Objects -----------------------------
//TLCD LCD(LCD_SCK, LCD_MOSI, LCD_CS);
TBuzzer Buzzer(BuzzerPin);
MCP3208 ADC1(ADC_CS);
DAC_MCP49xx DAC1(DAC_MCP49xx::MCP4921, DAC_CS);
RotaryEncoder Encoder(Rotary_A, Rotary_B, 5, 6, 3000);
SoftwareSerial Bluetooth(BT_TX, BT_RX);
//---------------------------------------------------------------
//---------------------- Public Variables -----------------------
char BluetoothData; // the data given from Computer
unsigned long start = 0, finished, elapsed;		// variables used on more than 1 function need to be declared here
int64_t temp = 0;
bool Toned = false;
double val = 0;
short resMultiplier = 1;
double CurrentVal = 0;
double VoltageVal = 0;
double PowerVal = 0;
double CurrentSetVal = 0;
double PowerSetVal = 0;
double ResistanceSetVal = 100.0;
double TempVal = 0, CTemp, PTemp, RTemp = -1;
short int LoadState;
uint8_t OperationMode = 1;
bool Cooling = false;
uint8_t RemoteOn = LOW, RemoteReset = HIGH, DisplaySate = HIGH;
uint8_t ToggleLED = HIGH, OldLoatState, RemoteOldLoatState;

short int BT_State = LOW;			// the current state of the Bluetooth output pin
short int BT_SW_State;				// the current reading from the Bluetooth switch pin
short int lastBT_SW_State = LOW;	// the previous reading from the Bluetooth switch pin
long lastBTDebounceTime = 0;		// the last time the Bluetooth output pin was toggled

short int Disp_State = HIGH;        // the current state of the display output pin
short int Disp_SW_State;            // the current reading from the display switch pin
short int lastDisp_SW_State = LOW;  // the previous reading from the display switch pin
long lastDispDebounceTime = 0;		// the last time the display output pin was toggled
long debounceDelay = 50;			// the debounce time; increase if the output flickers
unsigned long LastBeep = 0;			// hold the time of the last beep.
//---------------------------------------------------------------
boolean cycleCheck(unsigned long *lastMillis, unsigned int cycle)
{
	unsigned long currentMillis = millis();
	if (currentMillis - *lastMillis >= cycle)
	{
		*lastMillis = currentMillis;
		return true;
	}
	else
		return false;
}
//---------------------------------------------------------------
void setup(void) {
	//------- Pin Configuration --------//
	pinMode(LoadSwitch, INPUT);
	pinMode(BuzzerPin, OUTPUT);
	pinMode(StatusLED, OUTPUT);
	pinMode(ZeroCurrent, INPUT);
	//pinMode(DisplayBL, OUTPUT);
	//digitalWrite(DisplayBL, HIGH);
	pinMode(BT_SW, INPUT);
	pinMode(Mode_SW, INPUT_PULLUP);

	pinMode(BT_EN, OUTPUT);
	digitalWrite(BT_EN, LOW);

	ADC1.begin();
	DAC1.setPortWrite(true);
	DAC1.output(0);	//Make sure current is set to zero upon power up
	//InitTimersSafe(); //initialize all timers except for 0, to save time keeping functions
	//Serial.begin(9600);
	OldLoatState = !digitalRead(LoadSwitch);
	Bluetooth.begin(9600);//set up Bluetooth baudrate
	Wire.begin();
}
//------------------------------------------------------------------------------------------------------------------------------------------
void loop(void)
{
	//the threads are roundrobined to allow multithreading (as if it is executing multiple threads at once)
	ReadSwitches();						// Read and debounce switches on the front panel
	ReadEncoder();						// Read Knob-Encoder 
	AcruireDataThread.RunThread();		// Reads all the sensor values every 100 milli second
	//	UpdateLCDThread.RunThread();		// update the display every 500 milli second
	SendToTFTThread.RunThread();
	BuzzerWarningThread.RunThread();	// Activate over heating warning if needed
	if (BT_State == HIGH)				// only run Bluetooth threads if it was enabled
	{
		BluetoothTXThreat.RunThread();	// Send values to the PC every 500 milli second
		BluetoothRXThreat.RunThread();	// Recieve values from the PC every 250 millisecond
	}
}
//------------------------------------------------------------------------------------------------------------------------------------------
void ReadSwitches()
{
	//Read and debounce Bluetooth switch
	int readingBTSW = digitalRead(BT_SW);					//Read Switch Pin
	if (readingBTSW != lastBT_SW_State)lastBTDebounceTime = millis();//if the swtich has changed state, record time stamp
	if ((millis() - lastBTDebounceTime) > debounceDelay) {	//if it is within the acceptable time delay
		if (readingBTSW != BT_SW_State) {					//if it was it read is opposite to the current BT switch State
			BT_SW_State = readingBTSW;						//update BT state (toggle it)
			if (BT_SW_State == HIGH)						//if the switch is pushed down and all the other conditions have been met
			{
				BT_State = !BT_State;						//Toggle the Bt State
				digitalWrite(BT_EN, BT_State);				// apply changes on the output pin
				Buzzer.BeepOnce = true;						//produce one beep tone
				Buzzer.Beep();
			}
		}
	}
	lastBT_SW_State = readingBTSW;							//save the last switch state

	//Read and debounce Display switch
	int readingDispSW = digitalRead(Mode_SW);
	if (readingDispSW != lastDisp_SW_State)lastDispDebounceTime = millis();
	if ((millis() - lastDispDebounceTime) > debounceDelay) {
		if (readingDispSW != Disp_SW_State) {
			Disp_SW_State = readingDispSW;
			if (Disp_SW_State == HIGH)
			{
				//DisplaySate = !DisplaySate;
				ChangeMode();
			}
		}
	}
	lastDisp_SW_State = readingDispSW;
}
//------------------------------------------------------------------------------------------------------------------------------------------
int old_mul = resMultiplier;
void ReadEncoder()
{
	int enc = Encoder.readEncoder();
	switch (OperationMode)
	{
	case 1://current
		if (enc != 0) {
			temp += (enc);
			val = int(temp / 4) / 1000.0;
			temp = max(temp, 0);
			val = max(val, 0);
			CTemp = temp;
		}
		temp = min(temp, min(MaxCurrent * 4000, (MaxPower / VoltageVal) * 4000));	// limits the current to either max current or
		val = min(val, min(MaxCurrent, MaxPower / VoltageVal));						// max allowable current before it reaches max power 
		break;
	case 2://power

		if (enc != 0) {
			temp += (enc);
			val = int(temp / 4) / 100.0;
			temp = max(temp, 0);
			val = max(val, 0);
			PTemp = temp;
		}
		temp = min(temp, MaxPower * 400);	// limits the current to either max current or
		val = min(val, MaxPower);						// max allowable current before it reaches max power 

		break;
	case 3://resistance
		if (enc != 0) {
			temp += (enc);
			val = int(temp / 4) / (resMultiplier * 10.0);
			temp = min(temp, 40000);
			val = min(val, 1000);
			RTemp = temp;
		}


		temp = max(temp, max((VoltageVal*VoltageVal) / MaxPower, (VoltageVal / MaxCurrent)) * resMultiplier * 40);	// limits the current to either max current or
		val = max(val, max((VoltageVal*VoltageVal) / MaxPower, (VoltageVal / MaxCurrent)));						// max allowable current before it reaches max power 


		if (val < 10)resMultiplier = 10;
		else resMultiplier = 1;
		if (old_mul != resMultiplier)
		{
			old_mul = resMultiplier;
			if (resMultiplier == 10) temp *= 10;
			else temp /= 10;

		}
		break;
	}


}
//------------------------------------------------------------------------------------------------------------------------------------------
void AcquireData()
{
	if (digitalRead(ZeroCurrent) == LOW || RemoteReset == LOW)	//reset current set either remotely or locally
	{
		if (OperationMode == 3){
			resMultiplier = 1;
			old_mul = 1;
			temp = 4000;
			val = 100;

		}
		else
		{

			temp = 0;
			val = 0;
		}
		RemoteReset = HIGH;
		if (LastBeep <= millis() - 500)
		{
			Buzzer.BeepOnce = true;
			Buzzer.Beep2();
			LastBeep = millis();
		}
	}

	LoadState = 0;
	if (TempVal >= MaxTemp || Cooling) //stop current load if temp reaches a certain point
	{
		Cooling = true;
		LoadState = 2;
	}

	if (Cooling && TempVal < MaxTemp - CoolingWindow)Cooling = false; //once cooling is done, continue operation

	if ((OldLoatState != digitalRead(LoadSwitch) && (RemoteOn == LOW)) || (OldLoatState != RemoteOn && digitalRead(LoadSwitch) == LOW))
	{
		Buzzer.BeepOnce = true;
		Buzzer.Beep();
		OldLoatState = !OldLoatState;
	}

	if ((digitalRead(LoadSwitch) == HIGH || RemoteOn == HIGH) && !Cooling) {

		switch (OperationMode)
		{
		case 1:
			//Serial.println(temp);
			DAC1.output(map(temp, 0, MaxCurrent * 4000, 0, 4095));
			LoadState = 1;
			Toned = false;
			break;
		case 2:
			//			int RealCurrent = ;

			DAC1.output(map(min((temp*10.0 / VoltageVal), MaxCurrent * 4000), 0, MaxCurrent * 4000, 0, 4095));
			LoadState = 1;
			Toned = false;
			break;
		case 3:

			DAC1.output(map(min((VoltageVal * 1600 / (temp / (resMultiplier *100.0))), MaxCurrent * 4000), 0, MaxCurrent * 4000, 0, 4095));
			LoadState = 1;
			Toned = false;
			break;
		}
	}
	else
	{
		DAC1.output(0);
		start = (millis() / 100) - 5;
	}

	if (RemoteOn && digitalRead(LoadSwitch))RemoteOn = LOW; //if load was on remotely, flipping the switch on will override the remote state

	switch (OperationMode)
	{
	case 1:
		CurrentSetVal = val;
		break;
	case 2:
		PowerSetVal = val;
		break;
	case 3:
		ResistanceSetVal = val;
		break;
	}


	CurrentVal = (map(ADC1.analogRead(0), 0, 2124, 0, 2000) / 1000.0);
	if (CurrentVal > 0.004)CurrentVal += (0.002*CurrentVal*CurrentVal) - (0.007*CurrentVal) + 0.0042; //error compensation
	VoltageVal = map(ADC1.analogRead(1), 0, 4101, 0, 27000) / 1000.0;
	PowerVal = VoltageVal*CurrentVal;
	TempVal = ((ADC1.analogRead(2) * VRef * 100) / 4096.0) + 4.6;
}
//------------------------------------------------------------------------------------------------------------------------------------------

void SendToTFT()
{
	String Info;
	finished = millis() / 100; // saves stop time to calculate the elapsed time

	Info = "1#" + String(CurrentSetVal, 3) + ":" + String(PowerSetVal, 2) + ":" + String(ResistanceSetVal, 3) + ":" + String(CurrentVal, 3) + ":" + String(VoltageVal, 2);
	Wire.beginTransmission(1); // transmit to device #9
	Wire.write(Info.c_str());              // sends
	Wire.endTransmission();    // stop transmittingx 

	Info = "2#" + String(TempVal, 1) + ":" + String(LoadState) + ":" + String(finished - start) + ":" + String(OperationMode) + ":" + String(BT_State);
	Wire.beginTransmission(1); // transmit to device #9
	Wire.write(Info.c_str());              // sends
	Wire.endTransmission();    // stop transmitting

}
//void UpdateLCD()
//{
//	finished = millis() / 100; // saves stop time to calculate the elapsed time
//	if (DisplaySate == HIGH) //only update display if it was turned on
//		LCD.RunLCD(VoltageVal, CurrentVal, PowerVal, CurrentSetVal, TempVal, LoadState, finished - start, BT_State);
//}
//------------------------------------------------------------------------------------------------------------------------------------------
void BuzzerWarning()
{
	if (LoadState == 2)
	{
		Buzzer.TempWarningTone(&Toned);
		digitalWrite(StatusLED, ToggleLED);
		ToggleLED = !ToggleLED;
	}
	else
	{
		digitalWrite(StatusLED, (digitalRead(LoadSwitch) || RemoteOn));
		ToggleLED = HIGH;
	}
}
//------------------------------------------------------------------------------------------------------------------------------------------
void BlueToothTX()
{
	//----- Sending Info To PC ---------
	Bluetooth.print(CurrentSetVal, 3);
	Bluetooth.print(":");
	Bluetooth.print(CurrentVal, 3);
	Bluetooth.print(":");
	Bluetooth.print(VoltageVal, 2);
	Bluetooth.print(":");
	Bluetooth.print(PowerVal, 2);
	Bluetooth.print(":");
	Bluetooth.print(TempVal, 1);
	Bluetooth.print(":");
	Bluetooth.print(LoadState);
	Bluetooth.print(":");
	Bluetooth.print(finished - start);
	Bluetooth.print(":");
	Bluetooth.println(!DisplaySate);
}
//------------------------------------------------------------------------------------------------------------------------------------------
void ChangeMode()
{
	if (digitalRead(LoadSwitch) == HIGH)
	{
		for (int i = 0; i < 5; i++)
		{
			if (i < 3)
			{
				Buzzer.BeepOnce = true;
				Buzzer.Beep();
			}
			digitalWrite(StatusLED, ToggleLED);
			ToggleLED = !ToggleLED;
			delay(100);
			
		}

		return;
	}

	Buzzer.BeepOnce = true;
	Buzzer.Beep();
	OperationMode++;
	if (OperationMode > 3)OperationMode = 1;
	switch (OperationMode)
	{
	case 1:
		temp = CTemp;
		val = int(temp / 4) / 1000.0;
		break;
	case 2:
		temp = PTemp;
		val = int(temp / 4) / 100.0;
		break;
	case 3:

		temp = (RTemp == -1 ? 4000 : RTemp);
		val = int(temp / 4) / (resMultiplier * 10.0);
		break;
	}

}
//------------------------------------------------------------------------------------------------------------------------------------------
void Bluetooth_RX()
{
	char BTBuffer[10] = { 0 };
	float ReadValue;
	if (Bluetooth.available())
	{
		int i = 0;
		while (Bluetooth.available())
		{
			BluetoothData = Bluetooth.read();
			BTBuffer[i++] = BluetoothData;
		}
		Bluetooth.flush();
		if (BTBuffer[i - 1] == '\0' && BTBuffer[0] == '*' && BTBuffer[1] == '~')
			switch (BTBuffer[2])
		{
			case 'L':RemoteOn = LOW; break;									// Turn load off
			case 'H':RemoteOn = HIGH; break;								// Turn load on
			case 'R':RemoteReset = LOW; break;								// Zero current set
			case 'S':DisplaySate = !DisplaySate; Bluetooth_RX(); break;	// Turn display off/on		
			case 'p':														// Set current value
				ReadValue = String(BTBuffer).substring(3).toFloat();
				if (ReadValue >= 0.0 && ReadValue <= 2.0)
				{
					val = ReadValue;
					temp = val * 4000;
				}
				break;
		}
	}
}
//------------------------------------------------------------------------------------------------------------------------------------------