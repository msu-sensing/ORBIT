/*
 *	Define the GPS Interface
 *
 */
#ifndef GPS_h
#define GPS_h

//
//	If the GPS should use the corrected Arduino time define the following
//	this requires a modified Arduino kernel
//
//#define _GPS_USECORR

#include "Arduino.h"
#include "Time.h"

const int _gpsledPin	= 13;		// the pin that the LED is attached to
#define SECONDS_FROM_1970_TO_2000 946684800
#define SECONDS_PER_HOUR 3600



#define _GPSBUFFSIZE 256

struct COORD {
	char ind;			//N, S, E, W indicator
	uint32_t degrees;	//in [D]DDmmffff format (D=degrees, m= minutes, f=fraction of a minute)
};
struct POSITION {
	COORD latitude;
	COORD longitude;
	int32_t alt;		//altitude in cm
	};
	
class GPS 
{
	private:
		TIME _lasttime;
		DATE _lastdate;
		TIME _lastsynctime;
		DATE _lastsyncdate;
		unsigned long _lastppstime;
		unsigned long _lastsystime;
		unsigned long _lastlocktime;
		POSITION _pos;
		uint8_t _numsat;
		byte _fixind;
		uint8_t _clockAdjCnt;
		unsigned int _onTime;
		unsigned int _lpInterval;
		unsigned long _maxAcqTime;
		unsigned long _maxOffTime;
		byte _lockcnt;
		bool _tracking;
		bool _enablePM;
		uint32_t _gps1ppspin;
		uint32_t _gpslockpin;
		uint32_t _gpsenablepin;
		bool _validPOS;
		bool _validTM;
		void printPos(char *msg);
		void printTM(char *msg);
		void printDate(char *msg);
		Stream * _gpsserial;
		Stream * _logserial;
		int _lockState;
	public:
		GPS();
		void begin(Stream &gpsserial, Stream &logserial, uint32_t gpsenablepin, uint32_t gps1ppspin, uint32_t gpslockpin);
		void begin(bool, Stream &gpsserial, Stream &logserial, uint32_t gpsenablepin, uint32_t gps1ppspin, uint32_t gpslockpin);
		void end();
		void parseCommand(char *buffer, byte len);
		void lock();
		bool lockstate();
		void sync(bool);
		void fullPower();
		void powerManagement(unsigned int, unsigned int, unsigned long, unsigned long);
		void setFullPower();
		//void setPower(unsigned int OnTime, unsigned int LPInterval, unsigned long MaxAcqTime, unsigned long MaxOffTime)
		void setDebug(boolean);
		bool getDebug();
		bool validPosition();
		bool validTime();
		POSITION getPosition();
		long getElev();
		byte numSat();
		byte fixInd();
		TIME now();
		TIMESHORT nowShort();
		DATE nowDate();
		unsigned long toMicros(TIME tm);
		void gpsSerialEvent();
		UNIXTIME unixTime();
		UNIXTIMEMS unixTimeMillis();
		DATETIMESHORT unixTimeMillisToDateTime(UNIXTIMEMS tm);
};

extern GPS gps;

#endif