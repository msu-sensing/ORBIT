#include "samplebuffer.h"

#ifndef __arm__
#include "xmem.h"
#endif
//
//	definitions to protect code from interrupts
//
#ifndef __arm__
static inline void sregCleanup(uint8_t *sregp) __attribute__((always_inline));
void sregCleanup(uint8_t *sregp) { SREG = *sregp; };
#define intDisable()      ({ uint8_t sreg = SREG; cli(); sreg; })
#define intRestore(sreg)  SREG = sreg
#define begin_atomic      { \
uint8_t sregSave __attribute__((__cleanup__(sregCleanup))) = intDisable();
#define end_atomic        }
#else
#define begin_atomic { \
noInterrupts();
#define end_atomic interrupts(); }
#endif

//
//	Implementation
//
 // inline void copy_sample(SAMPLE *src, SAMPLE *dest)
// {
	// dest->tm = src->tm;
	// dest->readings[0] = src->readings[0];
	// dest->readings[1] = src->readings[1];
	// dest->readings[2] = src->readings[2];
// }
bool reset_samples(sample_ring_buffer *buffer) {
	buffer->count = 0;
	buffer->head = 0;
	buffer->tail = 0;
	buffer->full = false;

}
unsigned int num_samples(sample_ring_buffer *buffer) {
	return buffer->count;
};

bool allocate_samples(sample_ring_buffer *buffer, uint16_t numSamples, bool wrap, uint8_t memoryBank=0)
{
	uint8_t oldBank;
	buffer->count = 0;
	buffer->head = 0;
	buffer->tail = 0;
	buffer->max_samples = numSamples;
	buffer->full = false;
	buffer->wrap = wrap;
	buffer->bank = memoryBank;
#ifndef __arm__
	oldBank = xmem::setMemoryBank(memoryBank,true);
#endif
	buffer->samples = (SAMPLE *)malloc(sizeof(SAMPLE) * numSamples);
#ifndef __arm__
	xmem::setMemoryBank(oldBank, true);
#endif
}
bool isSampleFull(sample_ring_buffer *buffer)
{
	bool flg;
	begin_atomic;
	flg = buffer->full;
	end_atomic;
	return flg;
}
bool store_sample(SAMPLE *sample, sample_ring_buffer *buffer)
{
  int i;
#ifndef __arm__
  uint8_t prevBank = xmem::currentBank;
#endif
  bool flgError; 
  // if we should be storing the sample into the location
  // just before the tail (meaning that the head would advance to the
  // current location of the tail), we're about to overflow the buffer
  // and so we don't write the character or advance the head.
  
  begin_atomic;
  if (!buffer->full || buffer->wrap) {
 #ifndef __arm__
	if (buffer->bank != xmem::currentBank)
		xmem::setMemoryBank(buffer->bank, true);
#endif
 	//copy_sample(sample, &buffer->samples[buffer->head]);
	buffer->samples[buffer->head] = sample[0];
    buffer->head = (unsigned int)(buffer->head + 1) % buffer->max_samples;
	if (buffer->full)
		buffer->tail = buffer->head;
	else
		buffer->count += 1;
	if (buffer->head == buffer->tail)
		buffer->full = true;
#ifndef __arm__
	if (prevBank != xmem::currentBank)
		xmem::setMemoryBank(prevBank, true);
#endif
	flgError = true;
  } else
	flgError= false;
	end_atomic;
	return flgError;
}

bool read_sample(SAMPLE *dest, sample_ring_buffer *buffer)
{
	boolean flgError;
	
	begin_atomic;
	if (buffer->head == buffer->tail && !buffer->full)
	{
		flgError = false;
	} else
	{  
#ifndef __arm__
		uint8_t prevBank = xmem::currentBank;
		if (buffer->bank != xmem::currentBank)
			xmem::setMemoryBank(buffer->bank, true);
#endif
		//copy_sample(&buffer->samples[buffer->tail], dest);
		dest[0] = buffer->samples[buffer->tail];
		buffer->tail = (buffer->tail + 1) % buffer->max_samples;
		buffer->count -= 1;
		if (buffer->full)
			buffer->full = false;
#ifndef __arm__
		if (prevBank != xmem::currentBank)
			xmem::setMemoryBank(prevBank, true);
#endif
		flgError = true;
	};
	end_atomic;
	return flgError;
}
