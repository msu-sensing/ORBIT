/*
 *	Define the STALTA Interface
 *
 */
#ifndef STALTA_h
#define STALTA_h

#include <time.h>
#include <stddef.h>
#include <sample.h>


class PPhase
{
	public:
		int phaseOffset;
		UNIXTIMEMS phaseTime;
		double maxRatio;
		int nSamples;
		SAMPLE * Samples;
	//
	//	Methods
	//
		PPhase();
		void begin(int);
		void end();
};

typedef enum States {
	notinit,
	accumSamples,
	lookPhase,
	pStartFound,
	skippingSamples
};

typedef enum ReturnStatus {
	noPhase = 0,
	PhaseFound = 1,
	PhaseStart = 2,
	PhaseTooShort = 3,
	PhaseEnd = 4
};
class STALTA
{
	private:
        int _nSkiplength;
        int _nDurationlength;
        int _nLTA;
        int _nSTA;
        int _nCount;
        float _maxRatio;

        float _hThreshold;
        float _lThreshold;
       float _sum;
        int _pOffset;
		
        SAMPLE * _samples;
        //double * _ratio;
        float * _ltareadings;
        float * _stareadings;
        States _state;
		States _lastState;
		PPhase _lastPPhase;
		
	public:
	    int _nLTAlength;
        int _nSTAlength;
        float _ltasum;
        float _stasum;
 		int _maxSamples;
		STALTA();
		void begin(float sampleRate, float ltaDuration, float staDuration, float highThreshold, float lowThreshold, float minDuration, float skip);
		void end();
		uint8_t detect(SAMPLE sample);
		PPhase * getLast();
		float getLTA();
		float getSTA();
};
 
#endif
