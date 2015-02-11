#ifndef _SAMPLEBUFFER_H
#define _SAMPLEBUFFER_H

#include <sample.h>

#define SAMPLE_BUFFER_SIZE 200

struct sample_ring_buffer
{
  volatile unsigned int count;
  volatile unsigned int max_samples;
  volatile unsigned int head;
  volatile unsigned int tail;
  volatile uint8_t full : 1;
  volatile uint8_t wrap : 1;
  volatile uint8_t bank;
  SAMPLE *samples;
};

bool isSampleFull(sample_ring_buffer *buffer);
bool reset_samples(sample_ring_buffer *buffer);
bool store_sample(SAMPLE *sample, sample_ring_buffer *buffer);
bool read_sample(SAMPLE *dest, sample_ring_buffer *buffer);
unsigned int num_samples(sample_ring_buffer *buffer);
bool allocate_samples(sample_ring_buffer *buffer, uint16_t numSamples, bool wrap, uint8_t memoryBank);
#endif