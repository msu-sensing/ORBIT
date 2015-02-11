/*
 *	LOG - class to support logging at various levels
 */
#ifndef _LOG_H
#define _LOG_H

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "stdint.h"

#ifdef _USE_SD
#include "SdFat.h"
#else
#define File void
#endif

#define LOGLEVEL_VERBOSE	0
#define LOGLEVEL_DEBUG		1
#define LOGLEVEL_INFO		2
#define LOGLEVEL_WARNING	3
#define LOGLEVEL_ERROR		4
#define LOGLEVEL_FATAL		5
#define LOGLEVEL_SILENT		6

class LOG {
	private:
		uint8_t _logLevel;
		Stream* _serial;
		File* _sd;
		void logit(char *str, uint8_t level);
		
	public:
		LOG();
		void begin( );
		void begin(Stream *serial);
		void begin(File *sd);
		void begin(File *sd, Stream *serial);
		void setSerial(Stream *serial);
		void clearSerial();
		void setSD(File *sd);
		void clearSD();

		void setLevel(uint8_t level);
		uint8_t getLevel();
		void verbose(char *str);
		void debug(char *str);
		void info(char *str);
		void warning(char *str);
		void error(char *str);
		void fatal(char *str);
		bool isDebug();
		bool isVerbose();
		bool isInfo();
		bool isError();

};

#endif