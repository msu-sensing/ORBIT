//++
//
//  FACILITY:    Seismic Monitoring
//
//  ABSTRACT:
//		This module implements the STALTA P-Phase detection algorithm.
//
//  AUTHORS: Dennis Phillips, Michigan State University, Department of Computer Science
//
//  CREATION DATE:   May 15, 2014
//
//  MODIFICATION HISTORY:
//  $Id$
//
//--
#include "Arduino.h"
#include "STALTA.h";

PPhase::PPhase()
{
	Samples = NULL;
	nSamples = 0;
}
void PPhase::begin(int nsamples)
{
	end();
	Samples = (SAMPLE *)calloc(nsamples, sizeof(SAMPLE));
	nSamples =  nsamples;
}

void PPhase::end()
{
	if (Samples != NULL)
		free(Samples);
	
	nSamples = 0;
	Samples = NULL;
}

STALTA::STALTA()
{
	_maxRatio = 0;

	_samples = NULL;
	//_ratio = new double[n];
	_ltareadings = NULL;
	_stareadings = NULL;
	_nLTA = 0;
	_nSTA = 0;
	_nCount = 0;
	_state = notinit;
	_lastState = notinit;

}

void STALTA::begin(float sampleRate, float ltaDuration, float staDuration, float highThreshold, float lowThreshold, float minDuration, float skip)
{
	end();	// free the memory if it has been allocated
	int n;
	_maxRatio = 0;
	_nLTAlength = (int)(sampleRate * ltaDuration + 0.5);
	_nSTAlength = (int)(sampleRate * staDuration + 0.5);
	_nSkiplength = (int)(sampleRate * skip + 0.5);
	_nDurationlength = (int)(sampleRate * minDuration + 0.5);
	n = _nLTAlength + _nSkiplength + _nDurationlength;
	_maxSamples = n;
	_samples = (SAMPLE *)calloc(n,sizeof(SAMPLE));
	//_ratio = new double[n];
	_ltareadings = (float *)calloc(_nLTAlength+1, sizeof(float));
	_stareadings = (float *)calloc(_nSTAlength+1, sizeof(float));
	_nLTA = 0;
	_nSTA = 0;
	_nCount = 0;
	_hThreshold = highThreshold;
	_lThreshold = lowThreshold;
	_state = accumSamples;
	
}

void STALTA::end()
{
	if (_samples != NULL)
		free(_samples);
	_samples = NULL;
	if (_ltareadings != NULL)
		free(_ltareadings);
	_ltareadings = NULL;
	if (_stareadings != NULL)
		free(_stareadings);
	_maxSamples = 0;
	_state = notinit;
	_nCount = 0;
	_nLTA = 0;
	_nSTA = 0;
	_maxRatio = 0;
	_lastPPhase.Samples = NULL;
	_lastPPhase.nSamples = 0;
	
}

uint8_t STALTA::detect(SAMPLE sample)
{
	bool flgFound = false;
	uint8_t rtnStatus;
	float mean;
	float ratio;
	_samples[_nCount] = sample;
	_nCount += 1;
	rtnStatus = noPhase;
	
	#ifdef _STALTA_DEBUG
	if (_state != _lastState)
	{
		Serial.print("Entering state ");
		Serial.print(_state);
		Serial.print(" ncount = ");
		Serial.println(_nCount);
		_lastState = _state;
	};
	#endif
	switch (_state)
	{
		case accumSamples:
			if (_nCount == _nLTAlength)
			{
				_ltasum = 0.0;
				_stasum = 0.0;
				_sum = 0.0;
				_nSTA = 0;
				_nLTA = 0;
				_maxRatio = 0.0;
				//
				//  Compute the sum for use with mean removal
				//
				for (int i = 0; i < _nLTAlength; i++)
					_sum += _samples[i].reading[0];

				mean = _sum / _nLTAlength;
				//
				//  Compute the lta sum
				//
				for (int i = 0; i < _nLTAlength; i++)
				{   //_ratio[i] = 1.0;
					_ltasum += Abs(_samples[i].reading[0] - mean);
					_ltareadings[i] = Abs(_samples[i].reading[0] - mean);
				};
				//
				//  Compute the sta sum
				//
				for (int i = 0; i < _nSTAlength; i++)
				{
					_stasum += Abs(_samples[_nCount - 1 - i].reading[0] - mean);
					_stareadings[i] = Abs(_samples[_nCount - 1 - i].reading[0] - mean);
				}
				_state = lookPhase;

			};
			break;

		case lookPhase:
			//mean = _sum / _nLTAlength;
			_ltasum -= _ltareadings[_nLTA];
			_stasum -= _stareadings[_nSTA];
			//
			// Update the mean
			//
			_sum -= _samples[_nCount - _nLTAlength - 1].reading[0];
			_sum += _samples[_nCount - 1].reading[0];

			mean = _sum / _nLTAlength;
			_ltasum += Abs(_samples[_nCount - 1].reading[0] - mean);
			_ltareadings[_nLTA] = Abs(_samples[_nCount - 1].reading[0] - mean);
			_nLTA = (_nLTA + 1) % _nLTAlength;
			_stasum += Abs(_samples[_nCount - 1].reading[0] - mean);
			_stareadings[_nSTA] = Abs(_samples[_nCount - 1].reading[0] - mean);
			_nSTA = (_nSTA + 1) % _nSTAlength;

			//_ratio[_nCount - 1] = (_stasum / _nSTAlength) / (_ltasum / _nLTAlength);
			ratio = (_stasum / _nSTAlength) / (_ltasum / _nLTAlength);
			if (ratio >= _hThreshold)
			{
				_maxRatio = ratio;
				_pOffset = _nCount;
				_state = pStartFound;
				rtnStatus = PhaseStart;
	#ifdef _STALTA_DEBUG
				Serial.print("PPhase Found at ");
				Serial.print(_nCount);
				Serial.print(" ratio = ");
				Serial.println(ratio);
	#endif
			} else
			{
				//
				//  Shift the samples
				//
				for (int i = 1; i < _nCount; i++)
				{
					_samples[i-1] = _samples[i];
					//_ratio[i-1] = _ratio[i];
				};
				_nCount -= 1;
			}

			break;

		case pStartFound:
			//
			//  See if the ratio goes below the low threshold
			//
			//mean = _sum / _nLTAlength;
			//_ltasum -= Math.Abs(_samples[_nCount - _nLTAlength - 1].reading - mean);
		   _stasum -= _stareadings[_nSTA];
			//
			// Update the mean
			//
			//_sum -= _samples[_nCount - _nLTAlength - 1].reading;
			// _sum += _samples[_nCount - 1].reading;

			mean = _sum / _nLTAlength;
			//_ltasum += Math.Abs(_samples[_nCount - 1].reading - mean);
			_stasum += Abs(_samples[_nCount - 1].reading[0] - mean);
			_stareadings[_nSTA] = Abs(_samples[_nCount - 1].reading[0] - mean);
			_nSTA = (_nSTA + 1) % _nSTAlength;
			  
			//_ratio[_nCount - 1] = (_stasum / _nSTAlength) / (_ltasum / _nLTAlength);
			ratio = (_stasum / _nSTAlength) / (_ltasum / _nLTAlength);
			
			if (_maxRatio < ratio)
				_maxRatio = ratio;
			//
			//  if we have filled our arrays then extend the arrays
			//
			//Array.Resize(ref _samples, _nCount + _nSkiplength);
			//Array.Resize(ref _ratio, _nCount + _nSkiplength);

			if (ratio < _lThreshold)
			{
				//  If the signal duration is less than the needed duration length then
				//  this isn't a valid pPhase
				//
				if (_nCount - _pOffset < _nDurationlength)
				{
					//
					//  start looking for a pPhase after this pulse
					//
					_nCount = 0;
					_state = accumSamples;
					rtnStatus = PhaseTooShort;
					//Array.Resize(ref _samples, _nLTAlength + _nDurationlength + _nSkiplength);
					//Array.Resize(ref _ratio, _nLTAlength + _nDurationlength + _nSkiplength);
				}
				else
				{
					//
					//  We found the end
					//
					_state = skippingSamples;
					rtnStatus = PhaseEnd;
					// if (_samples.Length < _nCount + _nSkiplength)
					// {
						// Array.Resize(ref _samples, _nCount + _nSkiplength);
						// Array.Resize(ref _ratio, _nCount + _nSkiplength);
					// }
				};
			} else if ((_nCount == (_maxSamples - 1)) || ((_nCount - _pOffset) >= _nDurationlength))
			{
				//
				//	Signal is longer than the minimum length
				//
				_state = skippingSamples;
			
			};
			break;

		case skippingSamples:
			mean = _sum / _nLTAlength;
			//_ltasum -= Math.Abs(_samples[_nCount - _nLTAlength - 1].reading - mean);
			//_stasum -= Math.Abs(_samples[_nCount - _nSTAlength - 1].reading - mean);
			_stasum -= _stareadings[_nSTA];

			//
			// Update the mean
			//
		   // _sum -= _samples[_nCount - _nLTAlength - 1].reading;
		   // _sum += _samples[_nCount - 1].reading;

		   // mean = _sum / _nLTAlength;
			//_ltasum += Math.Abs(_samples[_nCount - 1].reading - mean);
			_stasum += Abs(_samples[_nCount - 1].reading[0] - mean);
			_stareadings[_nSTA] = Abs(_samples[_nCount - 1].reading[0] - mean);
			_nSTA = (_nSTA + 1) % _nSTAlength;
			   
			ratio = (_stasum / _nSTAlength) / (_ltasum / _nLTAlength);
			if (_maxRatio < ratio)
				_maxRatio = ratio;

			if (_nCount == _maxSamples)
			{
				//_lastPPhase = new PPhase();
				//_lastPPhase.Ratio = ratio;
				_lastPPhase.Samples = _samples;
				_lastPPhase.phaseOffset = _pOffset;
				_lastPPhase.maxRatio = _maxRatio;
				_lastPPhase.phaseTime = _samples[_pOffset].tm;
				_lastPPhase.nSamples = _nCount;
	#ifdef _STALTA_DEBUG
				Serial.print("Saving last pPhase ");
				Serial.print(_nCount);
				Serial.print(" at time ");
				char buffer[100];
				sprintf(buffer, "%0ld", _lastPPhase.phaseTime/1000000L); 
				Serial.print(buffer);  
				sprintf(buffer, "%0ld", _lastPPhase.phaseTime%1000000L); 
				Serial.print(buffer); 

				//Serial.print(_lastPPhase.phaseTime/1000);
				//Serial.print(_lastPPhase.phaseTime % 1000);
				Serial.print(" max ratio = ");
				Serial.println(_maxRatio);
	#endif
				//flgFound = true;
				rtnStatus = PhaseFound;
				_nCount = 0;
				_state = accumSamples;
			};
			break;
	};

	//return flgFound;
	return rtnStatus;
}
float STALTA::getLTA()
{
	return (_ltasum / _nLTAlength);
}
float STALTA::getSTA()
{
	return (_stasum / _nSTAlength);
}
PPhase * STALTA::getLast()
{
	return &_lastPPhase;
}


