#ifndef TDataInfo_h
#define TDataInfo_h
class TDataInfo
{
public:

	double 
		CurrentSensed,
		VoltageSensed,
		PowerSensed,
		ResistanceSensed,
		TempratureSensed,
		SetCurrent,
		SetPower,
		SetResistance;
	uint8_t LoadState,Mode,BTState;
	unsigned long TimeStamp;
};
#endif
