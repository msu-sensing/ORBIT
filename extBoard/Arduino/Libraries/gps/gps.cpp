//++
//
//  FACILITY:    Seismic Monitoring
//
//  ABSTRACT:
//		This module contains the main code that handles processing messages and hardware
//	signals from the GPS module. Communication to the GPS module is accomplished using
//	a standard _logserial port along with two hardware interrupts. The main interrupt is from
//	the 1 pulse per second hardware line. This rising edge interrupt is raised every second.
//	It is synchronized with the second update of the GPS UTC Time.
//
//	The second interrupt is for the lock indicator. When the gps module is not tracking
//	satellites this hardware line goes high for 100 msec every 200 msec. Once satellites
//	are being tracked this line goes high once every 15 seconds.
//
//	The adafruit.com Adafruit Ultimate GPS Breakout - 66 channel w/10 Hz updates - Version 3 
//	PRODUCT ID: 746 with the GPS Antenna - External Active Antenna - 3-5V 28dB 5 Meter SMA
//	PRODUCT ID: 960 and SMA to uFL/u.FL/IPX/IPEX RF Adapter Cable PRODUCT ID: 851 
//
//  AUTHORS: Dennis Phillips, Michigan State University, Department of Computer Science
//
//  CREATION DATE:   November, 2012
//
//--
#include "Arduino.h"
#include "gps.h";
#define debug

//
//	GPS hardware interface routines and local storage
//
bool _gpsdebug = false;
int daysInMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};
#define LEAP_YEAR(_year) ((_year%4)==0)

typedef enum GPS_STATES {
  wSync		= 0,  	// waiting for sync
  proc		= 1,		// process the character
  cksum 	= 2,  	// checksum is next
  cksum2	= 3,
  eol		= 4
};
GPS_STATES _gpsParseState = wSync;
 
byte _checksum = 0;
byte _mchecksum = 0;
byte _gpsbuffidx;
char _gpsString[_GPSBUFFSIZE];         // a string to hold incoming gps data

// different commands to set the update rate from once a second (1 Hz) to 10 times a second (10Hz)
#define PMTK_SET_NMEA_UPDATE_1HZ  "$PMTK220,1000*1F"
#define PMTK_SET_NMEA_UPDATE_5HZ  "$PMTK220,200*2C"
#define PMTK_SET_NMEA_UPDATE_10HZ "$PMTK220,100*2F"
//
//	Define communication baud rate commands
//
#define PMTK_SET_BAUD_57600 "$PMTK251,57600*2C"
#define PMTK_SET_BAUD_9600 "$PMTK251,9600*17"

// turn on only the second sentence (GPRMC)
#define PMTK_SET_NMEA_OUTPUT_RMCONLY "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29"
// turn on GPRMC and GGA
#define PMTK_SET_NMEA_OUTPUT_RMCGGA "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
// turn on GPRMC, GGA, and GSV
#define PMTK_SET_NMEA_OUTPUT_RMCGGAGSV "$PMTK314,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0*29"
// turn on ALL THE DATA
#define PMTK_SET_NMEA_OUTPUT_ALLDATA "$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
// turn off output
#define PMTK_SET_NMEA_OUTPUT_OFF "$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"

//++
//
//  NAME: GPS::gpsSerialEvent
//
//  FUNCTIONAL DESCRIPTION:
//		This function is called by the gps serial port handling software when there are bytes
//	available on the serial port. This routine looks for the $ character that begins a
//	GPS message. It then assembles a complete message, validates the message checksum. If
//	the message is not corrupt, calls the GPS method to parse the message.
//
//  FORMAL PARAMETERS:
//		None
//  IMPLICIT INPUTS:
//		_gpsserial - serial port used to communicate with the GPS
//		_gpsParseState - command parsing state
//		_gpsdebug - boolean controlling debug output
//		_logserial - Serial port used to output debugging messages
//		_gpsString - buffer containing assembled message
//		_gpsbuffidx - index into next byte in message buffer.
//
//  IMPLICIT OUTPUTS:
//
//  RETURN VALUES:
//
//  SIDE EFFECTS:
//		characters retrieved from GPS Serial port
//
//--
void GPS::gpsSerialEvent() {
	while (_gpsserial->available()) {
		// get the new byte:
		char inChar = (char)_gpsserial->read();
		//
		//	When ever we see a $ then we should reset our parse
		//	state and begin parsing a new command
		//
		if ((inChar == '$') || (_gpsbuffidx == _GPSBUFFSIZE)) {
			_gpsParseState = wSync;
		};
		
		if ((_gpsParseState == wSync) && (inChar == '$')) {
			_checksum = 0;
			_mchecksum = 0;
			_gpsbuffidx = 0;
			_gpsParseState = proc;
		} else if (_gpsParseState == proc) {
			if (inChar != '*') {
				_checksum = _checksum ^ inChar;
				if (_gpsbuffidx == (_GPSBUFFSIZE - 1)) {
					_gpsString[_gpsbuffidx] = 0;
				} else {
					_gpsString[_gpsbuffidx++] = inChar;
					};
			} else
			{
				_gpsParseState = cksum;
			};
		
		} else if ((_gpsParseState == cksum) || (_gpsParseState == cksum2)) {
			byte digit;
			if (inChar >= 'A') 
			{
				digit = inChar - 'A' + 10;
			} 
			else 
			{
				digit = inChar - '0';
			};
			_mchecksum = (_mchecksum << 4) + digit;
			if (_gpsParseState == cksum) 
			{
				_gpsParseState = cksum2;
			} 
			else 
			{
				_gpsParseState = wSync;
				_checksum = _checksum ^ _mchecksum;
				if (_checksum == 0)
				{
					if (_gpsdebug & _logserial != NULL) {
						_logserial->print("% GPS: ");
						_logserial->println(_gpsString);
					};
					gps.parseCommand(_gpsString, _gpsbuffidx);
					_gpsbuffidx = 0;
				} 
				else 
				{
					if (_gpsdebug && _logserial != NULL) _logserial->println("% CERR");
				};
			};	
		};
	};
}
//++
//
//  NAME:	_GPSISR1pps
//
//  FUNCTIONAL DESCRIPTION:
//		This is the interrupt service routine for the GPS 1 pulse per second signal
//
//  FORMAL PARAMETERS:
//		None
//  IMPLICIT INPUTS:
//		_gpsdebug - debug output flag
//		_logserial - log serial port
//  IMPLICIT OUTPUTS:
//		None
//  RETURN VALUES:
//		None
//  SIDE EFFECTS:
//		calls gps.sync method
//--
void _GPSISR1pps()
{
	gps.sync(true); 
}

//++
//
//  NAME:	_GPSISRlock
//
//  FUNCTIONAL DESCRIPTION:
//		This is the interrupt service routine for the GPS lock hardware signal
//
//  FORMAL PARAMETERS:
//		None
//  IMPLICIT INPUTS:
//		None
//  IMPLICIT OUTPUTS:
//		None
//  RETURN VALUES:
//		None
//  SIDE EFFECTS:
//		calls gps.lock method
//
//--
void _GPSISRlock() {
	gps.lock();
}
//
//	String parsing routines
//
//++
//
//  NAME:	skiptoken
//
//  FUNCTIONAL DESCRIPTION:
//		returns a pointer to the character after the N'th occurance of the
//		delimiter character.
//
//--
char *skiptoken(char *str, int num, char delim) {
	char *tmp;
	byte i;
	
	tmp = str;
	for (i=0; (i<num) && (tmp != NULL); i++) {
		tmp = strchr(tmp, delim);
		if (tmp[0] != NULL)
			tmp = tmp + 1;
	}
		
	return tmp;
};
//++
//
//  NAME:	findsepr
//
//  FUNCTIONAL DESCRIPTION:
//		return pointer to the next separator specified by sepr
//		delim specified the end of the string.
//--

char *findsepr(char *str, char sepr, char delim)
{
	char *tmp;
	tmp = str;
	while (tmp[0] != 0 && tmp[0] != sepr && tmp[0] != delim)
		tmp++;
	return tmp;
}
//++
//
//  NAME:	parsedecimal
//
//  FUNCTIONAL DESCRIPTION:
//		convert a string to integer value. Parse ends when either the end of the
//	string has been reached or a non-digit character.
//
//--
uint32_t parsedecimal(char *str)
{
 uint32_t d = 0;

    while (str[0] != 0)
    {
    if ((str[0] > '9') || (str[0] < '0')) {
      return d;
    };
	d *= 10;
    d += str[0] - '0';
    str++;
    };
  return d;
};
uint32_t parsedecimal(char *str, int width)
{
	uint32_t d = 0;
	int tmp = width;
    while (str[0] != 0 && tmp > 0)
    {
		if ((str[0] > '9') || (str[0] < '0') || tmp == 0) {
		  break;
		};
		d *= 10;
		d += str[0] - '0';
		str++;
		tmp -= 1;
    };
  
  while (tmp > 0) {
	d *= 10;
	tmp -= 1;
  };
  return d;
};
//++
//
//  NAME:	parsetime
//
//  FUNCTIONAL DESCRIPTION:
//		parse a timestring that is in the format hhmmss.nnnn
//
//--
TIME parsetime(char *str)
{
	char *ptr;
	TIME tm;
	uint32_t tmp;
	if (str[0] == ',') {
		tm.hour = 0;
		tm.min = 0;
		tm.sec = 0;
		tm.millisec = 0;
		return tm;
	}
	tmp = parsedecimal(str); 
	tm.hour = tmp / 10000;
	tm.min = (tmp / 100) % 100;
	tm.sec = tmp % 100;
	ptr = findsepr(str, '.', ',');
	if (ptr[0] != '.') {
		tm.millisec = 0;
		return tm;
		};
	str = ptr + 1;		// skip the period
	tm.millisec = parsedecimal(str);
	return tm;
}
//++
//
//  NAME:	parsedate
//
//  FUNCTIONAL DESCRIPTION:
//		parse the date string in ddmmyy format
//--
DATE parsedate(char *str)
{
	char *ptr;
	DATE dt;
	uint32_t tmp;
	tmp = 0;
	if (str[0] != ',')
		tmp = parsedecimal(str);
	dt.day = tmp / 10000;
	dt.month = (tmp / 100) % 100;
	dt.year = tmp % 100;
	return dt;
}
//++
//
//  NAME: parsecoord
//
//  FUNCTIONAL DESCRIPTION:
//		parse a lat/long corrdinate in the ddd.sssss format. resulting value
//		will be an integer in the form dddssss, i.e. scaled by 10000. so if the
//		coordinate is 75.1234 the return value will be 751234.
//
//  FORMAL PARAMETERS:
//		str - pointer to null terminated string
//
//  IMPLICIT INPUTS:
//		none
//  IMPLICIT OUTPUTS:
//		none
//  RETURN VALUES:
//		corridnate value.
//  SIDE EFFECTS:
//		None
//--
uint32_t parsecoord(char *str)
{
	uint32_t ps;
	char *ptr;
	
	ps = 0;
	if (str[0] == ',')
		return ps;
		
	ps = parsedecimal(str);
	if (ps != 0) ps *= 10000;
	ptr = findsepr(str, '.',',');
	if (ptr[0] != '.')
		return ps * 10000;
	str = ptr + 1;
	ps += parsedecimal(str,4);
	return ps;
}
//++
//
//  NAME:	parseelev
//
//  FUNCTIONAL DESCRIPTION:
//		parse an elevation in mm.ff format
//		result is an integer representing the altitude in cm
//--
long parseelev(char *str)
{
	uint32_t ps;
	char *ptr;
	
	ps = 0;
	if (str[0] == ',')
		return ps;
		
	ps = parsedecimal(str);
	if (ps != 0) ps *= 100;
	ptr = findsepr(str, '.',',');
	if (ptr[0] != '.')
		return ps * 100;
	str = ptr + 1;
	ps += parsedecimal(str,2);
	return ps;
}
// Constructors ////////////////////////////////////////////////////////////////

GPS::GPS()
{
	_gpsdebug = false;
	_enablePM = false;
	_gpsserial = NULL;
	_logserial = NULL;
	_gps1ppspin = -1;
	_gpslockpin = -1;
	_gpsenablepin = -1;
	_lockcnt = 0;
	_tracking = false;
	_lasttime.hour = -1;
	_lasttime.min = -1;
	_lasttime.sec = 0;
	_lastdate.day = -1;
	_lastdate.month = -1;
	_lastdate.year = -1;
	_lasttime.millisec = 0;
	_lastsynctime.hour = -1;
	_lastsynctime.min = -1;
	_lastsynctime.sec = 0;
	_lastsynctime.millisec = 0;
#ifdef _GPS_USECORR
	_lastsystime = microsCorr();	// get the current system time
#else
	_lastsystime = micros();
#endif
	_lastsyncdate.day = -1;
	_lastsyncdate.month = -1;
	_lastsyncdate.year = -1;
	_pos.latitude.ind = ' ';
	_pos.latitude.degrees = 0;
	_pos.longitude.ind = ' ';
	_pos.longitude.degrees = 0;
	_pos.alt = 0;
	_validPOS = false;
	_validTM = false;
	_numsat = 0;
	_fixind = 0;
	_clockAdjCnt = 5; 
	_onTime = 300;
	_lpInterval = 1000;
	_maxAcqTime = 300000;
	_maxOffTime = 30000;
  
};
// Private Methods ////////////
byte calcCsum(String *in) {
	byte cs;
	byte c;
	int i;
	//
	//	This function assumes that the input string starts with a $ and
	//	and ends with a * both of which need to be skipped.
	//
	cs = 0;
	for (i=0; i < in->length(); i++) {
		c = in->charAt(i);
		if (c == '*')
			break;
		if (c != '$')
			cs = cs ^ c;
	};
	return cs;
};



void GPS::setFullPower() {
	String outmsg = "$PLSC,200,1,,,,*";
	byte cs;
	String csum;
	cs = calcCsum(&outmsg);
	csum = String(cs, HEX);
	if (csum.length() == 1)
		outmsg += "0";
	outmsg += csum;
	
	_gpsserial->println(outmsg);
};	

// Public Methods //////////////////////////////////////////////////////////////
void GPS::begin(Stream &gpsserial, Stream &logserial, uint32_t gpsenablepin, uint32_t gps1ppspin, uint32_t gpslockpin) {
	begin(false, gpsserial, logserial, gpsenablepin, gps1ppspin, gpslockpin);
};
void GPS::begin(bool enablePM, Stream &gpsserial, Stream &logserial, uint32_t gpsenablepin, uint32_t gps1ppspin, uint32_t gpslockpin) {
 _clockAdjCnt = 5;
 _lockcnt = 6;
 _numsat = 0;
 _tracking = false;
 _enablePM = enablePM;
 _gpsserial = &gpsserial;
 _logserial = &logserial;
 _gpslockpin = gpslockpin;
 _gps1ppspin = gps1ppspin;
 _gpsenablepin = gpsenablepin;
 _fixind = 0;
 _validPOS = false;
_lockState = LOW;
 #ifdef _GPS_USECORR
	_lastlocktime = microsCorr();	// get the current system time
#else
	_lastlocktime = micros();
#endif
//
//	Configure the pin to enable GPS power and turn on the power
//
  pinMode(_gpsenablepin, OUTPUT);
  digitalWrite(_gpsenablepin, HIGH);
  
  pinMode(_gps1ppspin, INPUT);
  pinMode(_gpslockpin, INPUT);
  
  attachInterrupt(_gpslockpin, _GPSISRlock, RISING);
  attachInterrupt(_gps1ppspin, _GPSISR1pps, RISING);
  
  //_gpsserial->begin(9600);
  // turn on RMC (recommended minimum), GGA (fix data), GSV (satellites in view) including altitude
  _gpsserial->println(PMTK_SET_NMEA_OUTPUT_RMCGGAGSV);
  delay(100);
  _gpsserial->println(PMTK_SET_NMEA_OUTPUT_RMCGGAGSV);
  

  // Set the update rate
  _gpsserial->println(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate

  //
  //  Make sure serial communication is working
  //
  _gpsserial->flush();

  // initialize the LED as an output:
  pinMode(_gpsledPin, OUTPUT);
  setFullPower();
 if (_gpsdebug && _logserial != NULL)
	_logserial->println("% GPS Hello");
};

void GPS::end() {
	detachInterrupt(_gpslockpin);
	detachInterrupt(_gps1ppspin);
	//_gpsserial->end();
	digitalWrite(_gpsenablepin, LOW);
	pinMode(_gpsledPin, INPUT);
}

void GPS::fullPower() {
	setFullPower();
};

void GPS::setDebug(boolean flag) {
	_gpsdebug = flag;
};

bool GPS::getDebug() {
	return _gpsdebug;
};

void GPS::powerManagement(unsigned int OnTime, unsigned int LPInterval, unsigned long MaxAcqTime, unsigned long MaxOffTime) {
	_onTime = OnTime;
	_lpInterval = LPInterval;
	_maxAcqTime = MaxAcqTime;
	_maxOffTime = MaxOffTime;
	
	//setPower(_onTime, _lpInterval, _maxAcqTime, _maxOffTime);
	String outmsg = "$PLSC,200,2,";
	byte cs;
	String csum;
	
	outmsg += OnTime;
	outmsg += ",";
	outmsg += LPInterval;
	outmsg += ",";
	outmsg += MaxAcqTime;
	outmsg += ",";
	outmsg += MaxOffTime;
	outmsg += "*";
	cs = calcCsum(&outmsg);
	csum = String(cs,HEX);
	if (csum.length() == 1)
		outmsg += "0";
	outmsg += csum;
	
	_gpsserial->println(outmsg);

}
//
//	GPS::lock is called when the fix interrupt occurs
//
void GPS::lock() {
	unsigned long tm;
	//_logserial->println("GPS Lock interrupt");
#ifdef _GPS_USECORR
	tm = microsCorr();	// get the current system time
#else
	tm = micros();
#endif
	if (_lockState == LOW)
		_lockState = HIGH;
	else
		_lockState = LOW;
	digitalWrite(_gpsledPin, _lockState);
	
	if (tm - _lastlocktime < 5000000)
		_tracking = false;
	else
		_tracking = true;
	
	_lastlocktime = tm;
	
	//_lockcnt += 1;
};
bool GPS::lockstate() {
	return (_tracking && (_numsat >= 4));
};
//
//	GPS::sync is called when the 1pps interrupt occurs
//
void GPS::sync(bool flag) {
	unsigned long tm;
	//_logserial->println("GPS 1pps");
#ifdef _GPS_USECORR
	tm = microsCorr();	// get the current system time
#else
	tm = micros();
#endif
	// if ((_lockcnt >= 6) || (_numsat < 4)) {
		// _tracking = false;
	// }
	// else {
		// _tracking = true;
	// };
	if (_tracking) {
#ifdef _GPS_USECORR
		if (_clockAdjCnt == 0) {
		  fixDrift(tm - _lastsystime);
		  _clockAdjCnt = 5;
		};
		if (_clockAdjCnt >= 0) {
		//  _lastsystime = tm;
		  _clockAdjCnt -= 1;
		};
#endif
		//
		//	If Power management was enabled only output
		//	the message once
		//
		if (_enablePM) {
			powerManagement(_onTime, _lpInterval, _maxAcqTime, _maxOffTime);
			_enablePM = false;
			};
	};
	//_lockcnt = 0;
	_lastppstime = tm;
};
void GPS::printPos(char *msg) {
	if (_logserial != NULL) {
		_logserial->print("% ");
		_logserial->print(msg);
		_logserial->print(" coord LAT=");
		_logserial->print(_pos.latitude.ind);
		_logserial->print(_pos.latitude.degrees);
		_logserial->print(" LON=");
		_logserial->print(_pos.longitude.ind);
		_logserial->print(_pos.longitude.degrees);
		_logserial->print(" ELEV=");
		_logserial->println(_pos.alt);
	};
}
void GPS::printTM(char *msg) {
	if (_logserial != NULL) {
		_logserial->print("% ");
		_logserial->print(msg);
		_logserial->print(" Time= ");
		_logserial->print(_lasttime.hour);
		_logserial->print(":");
		_logserial->print(_lasttime.min);
		_logserial->print(":");
		_logserial->print(_lasttime.sec);
		_logserial->print(" ms=");
		_logserial->println(_lasttime.millisec);
	};
}
void GPS::printDate(char *msg) {
	if (_logserial != NULL) {
		_logserial->print("% ");
		_logserial->print(msg);
		_logserial->print(" Date= ");
		_logserial->print(_lastdate.month);
		_logserial->print("/");
		_logserial->print(_lastdate.day);
		_logserial->print("/");
		_logserial->println(_lastdate.year);
	};
}
//++
//
//  NAME:	parseCommand
//
//  FUNCTIONAL DESCRIPTION:
//		This is the main GPS command string parser.
//--
void GPS::parseCommand(char * message, byte msglen) {
	char *_gpsparseptr;
	bool _gotTime = false;
	TIME tmptime;
	POSITION tmppos;
	_gpsparseptr = &message[0];
	
	if (strncmp(_gpsparseptr, "GPGSV",5) == 0)           // check $GPGSV
	{
		_gpsparseptr = skiptoken(_gpsparseptr,3,',');
		_numsat = parsedecimal(_gpsparseptr);
	 } else if (strncmp(_gpsparseptr, "GPGGA",5) == 0)	// check $GPGGA
	 {
		_gpsparseptr = skiptoken(_gpsparseptr,1,',');
		 tmptime = parsetime(_gpsparseptr);
		 _gpsparseptr = skiptoken(_gpsparseptr,1,',');
		 tmppos.latitude.degrees = parsecoord(_gpsparseptr);
		 _gpsparseptr = skiptoken(_gpsparseptr,1,',');
		 if (_gpsparseptr[0] != ',') {
			 tmppos.latitude.ind = _gpsparseptr[0];
		 };
		 _gpsparseptr = skiptoken(_gpsparseptr,1,',');
		 tmppos.longitude.degrees = parsecoord(_gpsparseptr);
		 _gpsparseptr = skiptoken(_gpsparseptr,1,',');
		 if (_gpsparseptr[0] != ',') {
			 _pos.longitude.ind = _gpsparseptr[0];
		 };
		 _gpsparseptr = skiptoken(_gpsparseptr,1,',');
		 
		 if (_gpsparseptr[0] == '0')		// if 0 then no fix
			return;
		_gpsparseptr = skiptoken(_gpsparseptr,1,',');
		int nsat;
		nsat = parsedecimal(_gpsparseptr);
		if (_numsat == 0)
			_numsat = nsat;
		_gpsparseptr = skiptoken(_gpsparseptr,2,',');
		 tmppos.alt = parseelev(_gpsparseptr);
		
		 _lasttime = tmptime;
		 _pos = tmppos;
		 _validPOS = true;
		 _gotTime = true;
		if (_gpsdebug && _logserial != NULL) { 
			_logserial->println("% GPGGA data valid");
			printPos("GPGGA");
			printTM("GPGGA");
		};

	} else if (strncmp(_gpsparseptr,"GPGLL",5) == 0)	// check $GPGLL
	{
		tmppos.alt = _pos.alt;	// this sentence does not have altitude so use
								// last result.
		_gpsparseptr = skiptoken(_gpsparseptr,1,',');
		tmppos.latitude.degrees = parsecoord(_gpsparseptr);
		_gpsparseptr = skiptoken(_gpsparseptr,1,',');
		if (_gpsparseptr[0] != ',') {
			tmppos.latitude.ind = _gpsparseptr[0];
		};
		_gpsparseptr = skiptoken(_gpsparseptr,1,',');
		tmppos.longitude.degrees = parsecoord(_gpsparseptr);
		_gpsparseptr = skiptoken(_gpsparseptr,1,',');
		if (_gpsparseptr[0] != ',') {
			tmppos.longitude.ind = _gpsparseptr[0];
		};
		_gpsparseptr = skiptoken(_gpsparseptr,1,',');
		tmptime = parsetime(_gpsparseptr);
		_gpsparseptr = skiptoken(_gpsparseptr,1,',');
		if (_gpsparseptr[0] == 'A') {
			_lasttime = tmptime;
			_pos = tmppos;
			_validPOS = true;
			_gotTime = true;
			if (_gpsdebug && _logserial != NULL) { 
				_logserial->println("% GPGLL data valid");
				printPos("GPGLL");
				printTM("GPGLL");
			};
		};
			

		
	} else if (strncmp(_gpsparseptr,"GPRMC",5) == 0) // check $GPRMC 
	{	// GPRMC,005704.075,V,,,,,,,150209,,,N,,000000,M,0.0,M,,000000
		_gpsparseptr = skiptoken(_gpsparseptr,1,',');
		_lasttime = parsetime(_gpsparseptr);
		_gpsparseptr = skiptoken(_gpsparseptr,1,',');	//skip the status
		if (_gpsparseptr[0] == 'V')
			return;
		_gotTime = true;
		_gpsparseptr = skiptoken(_gpsparseptr,1,',');
		_pos.latitude.degrees = parsecoord(_gpsparseptr);
		_gpsparseptr = skiptoken(_gpsparseptr,1,',');
		if (_gpsparseptr[0] != ',') {
			_pos.latitude.ind = _gpsparseptr[0];
		};
		_gpsparseptr = skiptoken(_gpsparseptr,1,',');
		_pos.longitude.degrees = parsecoord(_gpsparseptr);
		_gpsparseptr = skiptoken(_gpsparseptr,1,',');
		if (_gpsparseptr[0] != ',') {
			_pos.longitude.ind = _gpsparseptr[0];
		};
		_validPOS = true;
		_gpsparseptr = skiptoken(_gpsparseptr,3,',');	// skip speed and course
		_lastdate = parsedate(_gpsparseptr);
		 if (_gpsdebug && _logserial != NULL) { 
			_logserial->println("% GPRMC data valid");
			printPos("GPRMC");
			printTM("GPRMC");
			printDate("GPRMC");
		};


	} else if (strncmp(_gpsparseptr,"PLSR,200",8) == 0)	// check $PLSR,200
	{
		unsigned int flg;
		if (_gpsdebug && _logserial != NULL)
			_logserial->println("% PLSR,200");
		_gpsparseptr = skiptoken(_gpsparseptr,2,',');
		flg = parsedecimal(_gpsparseptr);
		if (_gpsdebug && _logserial != NULL) { 
			if (flg == 1)
				_logserial->println("% Valid Power cmd");
			else
				_logserial->println("% Invalid Power cmd");
				
		};
	};
	//
	//	If we got a valid time then update
	//
	if (_gotTime) {
		_lastsynctime = _lasttime;
		_lastsyncdate = _lastdate;
		_lastsystime = _lastppstime;
		_validTM = true;
	};
};
//++
//
//  NAME:	getPosition
//
//  FUNCTIONAL DESCRIPTION:
//		return the current latitude/longitude/altitude
//
//--
POSITION GPS::getPosition() {
	return _pos;
};
//++
//
//  NAME:	getElev
//
//  FUNCTIONAL DESCRIPTION:
//		return current elevation in CM.
//--
long GPS::getElev() {
	return _pos.alt;
}
//++
//
//  NAME:	validPosition
//
//  FUNCTIONAL DESCRIPTION:
//		return boolean indicating if we have received a valid position
//		from the GPS.
//--
bool GPS::validPosition() {
	return _validPOS;
};
//++
//
//  NAME:	validTime
//
//  FUNCTIONAL DESCRIPTION:
//		return boolean indicating if we have received a valid time
//		from the GPS
//--
bool GPS::validTime() {
	return _validTM;
};
//++
//
//  NAME: numSat
//
//  FUNCTIONAL DESCRIPTION:
//		return the number of satellites currently in view of the GPS
//--
byte GPS::numSat() {
	return _numsat;
};
//++
//
//  NAME:	fixInd
//
//  FUNCTIONAL DESCRIPTION:
//		return a boolean indicating if the gps has been able to calculate
//		a valid position fix.
//--
byte GPS::fixInd() {
	return _fixind;
};
//++
//
//  NAME:	now
//
//  FUNCTIONAL DESCRIPTION:
//		return the current time with microsecond resolution
//--

TIME GPS::now() {
	unsigned long tm;
	unsigned long tmp;
	
	TIME tmn;
#ifdef _GPS_USECORR
	tm = microsCorr();	// get the current system time
#else
	tm = micros();
#endif
	tmn = _lastsynctime;
	tmp = tm - _lastsystime;
	tmn.microsec = tmp % 1000;	//get the number of microseconds
	
	tmp = tmp / 1000;		// convert to milliseconds
	tmn.millisec = tmp % 1000;
	tmp = tmp / 1000;		// Get the number of seconds
	//
	// Now handle over flow for when we are not getting the 1pps signal
	//
	tmn.sec += tmp;
	tmp = tmn.sec / 60;
	tmn.sec = tmn.sec % 60;
	
	//
	//	If the minute or hour are not negative then we have gotten
	//	a time message from the GPS.  In this catre handle the overflow
	//	for the minutes and hours.
	//
	if (tmn.min >= 0) {
		tmn.min += tmp;
		tmp = tmn.min / 60;
		tmn.min = tmn.min % 60;
		
		tmn.hour += tmp;
		tmn.hour = tmn.hour % 24;
	};
	return tmn;
};
//++
//
//  NAME:	nowShort
//
//  FUNCTIONAL DESCRIPTION:
//		return the current time with millisecond resolution
//--
TIMESHORT GPS::nowShort() {
	TIME tmn;
	TIMESHORT tms;
	
	tmn = now();
	tms.hour = tmn.hour;
	tms.min = tmn.min;
	tms.sec = tmn.sec;
	tms.millisec = tmn.millisec;
	return tms;
};
//++
//
//  NAME: nowDate
//
//  FUNCTIONAL DESCRIPTION:
//		return the current date. If we have not received a valid
//		date/time from the gps then convert the internal Arduino
//		millisecond clock to the date.
//--
DATE GPS::nowDate() {
	if (_validTM)
		return _lastsyncdate;
	UNIXTIMEMS ms = millis();
	DATETIMESHORT dt = unixTimeMillisToDateTime(ms);
	return dt.date;
};
//++
//
//  NAME:	toMicros
//
//  FUNCTIONAL DESCRIPTION:
//		Convert the TIME structure to an unsigned long with
//		microsecond resolution.
//
//--
unsigned long GPS::toMicros(TIME tm) {
	unsigned long tmp = 0;
	//
	//	This will overflow 32 bits but that is ok
	//
	tmp = tm.hour * 60;
	tmp = (tmp + tm.min) * 60;
	tmp = (tmp + tm.sec) * 1000;
	tmp = (tmp + tm.millisec) * 1000;
	tmp = tmp + tm.microsec;
	
	return tmp;
}
//++
//
//  NAME:	unixTime
//
//  FUNCTIONAL DESCRIPTION:
//		return the current Unix time with second resolution
//
//--
UNIXTIME GPS::unixTime() {
	DATE nowdt = nowDate();
	TIME tmn = now();
	UNIXTIME _ticks;
	uint16_t _days;
	int _hour = tmn.hour;
	int _minute = tmn.min;
	int _second = tmn.sec;
	int _millis = tmn.millisec;
	int _day = nowdt.day;
	int _month = nowdt.month;
	uint16_t _year = nowdt.year;

	_days = _day;
 
	for (int i = 1; i < _month; ++i)
	   _days += daysInMonth[i - 1];
	 
	if (_month > 2 && _year % 4 == 0)
		++_days;
	
	_days += 365 * _year + (_year + 3) / 4 - 1;

	_ticks = ((_days * 24 + _hour) * 60 + _minute) * 60 + _second;
	
	_ticks += SECONDS_FROM_1970_TO_2000;

	return _ticks;
}
//++
//
//  NAME: unixTimeMillis
//
//  FUNCTIONAL DESCRIPTION:
//		return the current time in Unix format with millisecond resolution
//
//--
UNIXTIMEMS GPS::unixTimeMillis() {
	if (_validTM)
	{
		DATE nowdt = nowDate();
		TIME tmn = now();
		UNIXTIMEMS _ticks;
		uint16_t _days;
		int _hour = tmn.hour;
		int _minute = tmn.min;
		int _second = tmn.sec;
		int _millis = tmn.millisec;
		int _day = nowdt.day;
		int _month = nowdt.month;
		uint16_t _year = nowdt.year;
		_days = _day;

		for (int i = 1; i < _month; ++i)
		   _days += daysInMonth[i - 1];
		 
		 if (_month > 2 && _year % 4 == 0)
			++_days;
		
		_days += 365 * _year + (_year + 3) / 4 - 1;
		
		_ticks = ((_days * 24 + _hour) * 60 + _minute) * 60 + _second;
		
		_ticks += SECONDS_FROM_1970_TO_2000;
		_ticks = _ticks * 1000 + _millis;
		
		return _ticks;
	} else
		return millis();
}
//++
//
//  NAME: unixTimeMillisToDateTime(UNIXTIMEMS tm)
//
//  FUNCTIONAL DESCRIPTION:
//		Convert the 64 bit Unix Time with millisecond resolution to a date
//		time structure
//
//--
DATETIMESHORT GPS::unixTimeMillisToDateTime(UNIXTIMEMS tm)
{
	DATETIMESHORT rtm;
	
    UNIXTIMEMS epoch=tm;
	byte year;
	byte month, monthLength;
	unsigned long days;
	rtm.time.millisec = epoch % 1000;
	epoch /= 1000;
	
	rtm.time.sec = epoch%60;
	epoch/=60; // now it is minutes
	rtm.time.min=epoch%60;
	epoch/=60; // now it is hours
	rtm.time.hour=epoch%24;
	epoch/=24; // now it is days
	//rtm.date.day=(epoch+4)%7;

	year=70;  
	days=0;
	while((unsigned)(days += (LEAP_YEAR(year) ? 366 : 365)) <= epoch) {
		year++;
	}
	rtm.date.year=year; // *pyear is returned as years from 1900

	days -= LEAP_YEAR(year) ? 366 : 365;
	epoch -= days; // now it is days in this year, starting at 0
	//*pdayofyear=epoch;  // days since jan 1 this year

	days=0;
	month=0;
	monthLength=0;
	for (month=0; month<12; month++) {
		if (month==1) { // february
		  if (LEAP_YEAR(year)) {
			monthLength=29;
		  } else {
			monthLength=28;
		  }
		} else {
		  monthLength = daysInMonth[month];
		}

		if (epoch>=monthLength) {
		  epoch-=monthLength;
		} else {
			break;
		}
	}
	rtm.date.month=month + 1;  // jan is month 1
	rtm.date.day=epoch+1;  // day of month
	rtm.date.year += 1900;
	return rtm;
}
// Preinstantiate gps Object //////////////////////////////////////////////////////

GPS gps;