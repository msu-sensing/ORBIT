#include "LOG.h"

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "PString.h"

LOG::LOG() {
	_logLevel = LOGLEVEL_WARNING;
}
void LOG::begin(Stream *serial) {
	_serial = serial;
	_sd = NULL;
}
void LOG::begin( ) {
	_serial = NULL;
	_sd = NULL;
}
void LOG::begin(File *sd) {
	_sd = sd;
	_serial = NULL;
}
void LOG::begin(File *sd, Stream *serial) {
	_sd = sd;
	_serial = serial;
}
void LOG::setSerial(Stream *serial) {
	_serial = serial;
}
void LOG::clearSerial() {
	_serial = NULL;
}
void LOG::setSD(File *sd) {
	_sd = sd;
}
void LOG::clearSD() {
	_sd = NULL;
}
void LOG::setLevel(uint8_t level) {
	_logLevel = level;
}

uint8_t LOG::getLevel() {
	return _logLevel;
}

bool LOG::isDebug() {
	return (_logLevel <= LOGLEVEL_DEBUG);
}

bool LOG::isVerbose() {
	return (_logLevel <= LOGLEVEL_VERBOSE);
}
bool LOG::isInfo() {
	return (_logLevel <= LOGLEVEL_INFO);
}
bool LOG::isError() {
	return (_logLevel <= LOGLEVEL_ERROR);
}	
void LOG::verbose(char *str) {
	if (_logLevel <= LOGLEVEL_VERBOSE)
		logit(str, LOGLEVEL_VERBOSE);
}

void LOG::debug(char *str) {
	if (_logLevel <= LOGLEVEL_DEBUG)
		logit(str, LOGLEVEL_DEBUG);
}

void LOG::info(char *str) {
	if (_logLevel <= LOGLEVEL_INFO)
		logit(str, LOGLEVEL_INFO);
}

void LOG::warning(char *str) {
	if (_logLevel <= LOGLEVEL_WARNING)
		logit(str, LOGLEVEL_WARNING);
}

void LOG::error(char *str) {
	if (_logLevel <= LOGLEVEL_ERROR)
		logit(str, LOGLEVEL_ERROR);
}

void LOG::fatal(char *str) {
	if (_logLevel <= LOGLEVEL_FATAL)
		logit(str, LOGLEVEL_FATAL);
}
void LOG::logit(char *sstr, uint8_t level) {
	char buf[133];
	PString str(buf, sizeof(buf));
	str.begin();
	switch (level)
	{
		case LOGLEVEL_VERBOSE:
			str.print("%V%-");
			break;
			
		case LOGLEVEL_DEBUG:
			str.print("%D%-");
			break;
		
		case LOGLEVEL_INFO:
			str.print("%I%-");
			break;
		
		case LOGLEVEL_WARNING:
			str.print("%W%-");
			break;
		
		case LOGLEVEL_ERROR:
			str.print("%E%-");
			break;
		
		case LOGLEVEL_FATAL:
			str.print("%F%-");
			break;
		
		default:
			str.print("%U%-");
			break;
	};
	str.print(sstr);
	if (_serial != NULL)
		_serial->println(buf);

		#ifdef _USE_SD
	if (_sd != NULL)
	{
		_sd->println(buf);
		_sd->flush();
	}
#endif
}
