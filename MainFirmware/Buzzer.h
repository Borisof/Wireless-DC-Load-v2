#ifndef Buzzer_h
#define Buzzer_h

#define ToneNum 7
bool ToneToggle = false;
int ToggleCount = ToneNum;

class TBuzzer
{
private:
	int Pin;
	
public:
	bool BeepOnce;
	TBuzzer(int _Pin)
	{
		Pin = _Pin;
		BeepOnce = false;
	}
	void TempWarningTone(bool *Toned)
	{
		if (ToggleCount >= ToneNum && !*Toned)
		{
			ToggleCount = 0;
			ToneToggle = true;
		}

		if (ToggleCount < ToneNum && !*Toned)
		{
			if (ToneToggle)tone(Pin, 2000);
			else tone(Pin, 1800);
			ToneToggle = !ToneToggle;
			ToggleCount++;
			if (ToggleCount == ToneNum)
			{
				*Toned = true;
				noTone(Pin);
			}
		}
	}
	void Beep()
	{
		if (BeepOnce)
		{
			tone(Pin, 1900, 50);
			BeepOnce = false;
			delay(50);
		}
	}
	void Beep2()
	{
		if (BeepOnce)
		{
			tone(Pin, 2000, 50);
			delay(50);
			tone(Pin, 1500, 50);
			delay(50);
			BeepOnce = false;
		}
	}
};



#endif
