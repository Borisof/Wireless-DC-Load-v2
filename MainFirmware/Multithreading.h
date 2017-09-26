#ifndef Multithreading_h
#define Multithreading_h
typedef void(*TFunPtr)();
class TThread
{
private:
	unsigned int CycleTime;
	unsigned long CycleLast;
	TFunPtr Function;

	boolean CycleCheck(unsigned long *lastMillis, unsigned int cycle)
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

public:
	TThread(TFunPtr function, unsigned int cycleTime)
	{
		Function = function;
		CycleTime = cycleTime;
	}
	void RunThread()
	{
		if (CycleCheck(&CycleLast, CycleTime))
		{
			Function();
		}
	}

};
#endif

