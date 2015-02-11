//++
//
//  FACILITY:    Seismic Monitoring
//
//  ABSTRACT:
//		This module contains the main code that handles processing messages from the usb host.
//  It handles the main packing and unpacking of messages into and out buffers.
//
//  AUTHORS: Dennis Phillips, Michigan State University, Department of Computer Science
//
//  CREATION DATE:   June 3, 2013
//
//--
#include "Arduino.h"
#include "HDLC.h";
#define debug


// Constructors ////////////////////////////////////////////////////////////////

message_buffer mx_ringbuffer;

HDLC::HDLC(int mtuSize) {
	byteCacheRX = (byte *)malloc(mtuSize * 2);
	MAX_LENGTH = mtuSize;
	countRX = 0;
	errCRC = 0;
	_mx_ringbuffer = &mx_ringbuffer;
	for (int i = 0; i < MAX_MESSAGES; i++)
		mx_ringbuffer.buffer[i] = NULL;
	mx_ringbuffer.head = 0;
	mx_ringbuffer.tail = 0;
	
	escaped = false;
    //synced = false;

}
 
void HDLC::store_message(Message *m)
{
  int i = (unsigned int)(_mx_ringbuffer->head + 1) % MAX_MESSAGES;

  // if we should be storing the received character into the location
  // just before the tail (meaning that the head would advance to the
  // current location of the tail), we're about to overflow the buffer
  // and so we don't write the character or advance the head.
  if (i != _mx_ringbuffer->tail) {
	_mx_ringbuffer->buffer[_mx_ringbuffer->head] = m;
	_mx_ringbuffer->head = i;
  } else
	errDropped += 1;
}
Message *HDLC::get_message()
{
  // if the head isn't ahead of the tail, we don't have any characters
  if (_mx_ringbuffer->head == _mx_ringbuffer->tail) {
    return NULL;
  } else {
    Message *m = _mx_ringbuffer->buffer[_mx_ringbuffer->tail];
	_mx_ringbuffer->buffer[_mx_ringbuffer->tail] = NULL;
    _mx_ringbuffer->tail = (unsigned int)(_mx_ringbuffer->tail + 1) % MAX_MESSAGES;
    return m;
  }

}
Message *HDLC::allocMessage(void)
{
	Message *m;
	
	m = (Message *)malloc(sizeof(Message) + MAX_LENGTH * 2);
	return m;
}
void HDLC::freeMessage(Message *res)
{
	free(res);
}
int HDLC::available(void)
{
  return (unsigned int)(MAX_MESSAGES + _mx_ringbuffer->head - _mx_ringbuffer->tail) % MAX_MESSAGES;
}

Message *HDLC::EncodeBytes(byte *data, int len)
{
	//Message res = new Message();
	Message *res;//NEEDF TO DEFINE MAX MESS SIZE
	uint8_t msbLen = (len >> 8) & 0xff;
	uint8_t lsbLen = len & 0xff;

	res = allocMessage();
	//write initial stuff
	crcTx = 0;
	int idx = 1;
	res->data[0] = SYNC_VAL;
    //idx = EncodeByte(idx, res->data, SYNC_VAL);
	idx = EncodeByte(idx, res->data, msbLen);
	idx = EncodeByte(idx, res->data, lsbLen);

	for (int i = 0; i < len; i++)
	{
		idx = EncodeByte(idx, res->data, data[i]);
		crcTx = ComputeCRC(data[i], crcTx);
	}
	//write CRC
	uint16_t crc = crcTx;
	idx = EncodeByte(idx, res->data, (byte)((crc >> 8) & 0xff));
	idx = EncodeByte(idx, res->data, (byte)(crc & 0xff));
	//res->data[idx] = SYNC_VAL;
	//idx++;
	res->length = idx;		// final length of the encoded message
	return res;
}

int HDLC::EncodeByte(int idx, byte *buff, byte val)
{
	//src->send.crc = crc_byte(src->send.crc, b);
	if (val == SYNC_VAL || val == ESCAPE_VAL)
	{
		buff[idx] = ESCAPE_VAL;
		buff[idx + 1] = (byte)(val ^ 0x20);
		return (idx + 2);
	}
	else
	{
		buff[idx] = val;
		return (idx + 1);
	}

}

//if there are one or more packets, true
//if there is no packets, false
//Packets can be retrieved using getPacket

bool HDLC::DecodeBytes(int len, byte *data)
{

	//Message[] temp = new Message[128];//no more than 128 messages in a raw data buffer

	Message *m;
	for (int i = 0; i < len && available() < MAX_MESSAGES; i++)
	{
		m = DecodeByte(data[i]);
		if (m != NULL)
		{
			//add to list of messages to be returned
			store_message(m);
		};
	};
	if (available() == 0)
	{
		return false;
	};

	return true;
}

Message *HDLC::DecodeByte(byte val)
{
	if (countRX > 0 && val == SYNC_VAL)
	{
		// new packet started before previous packeted completed -- discard previous packet and start over
		countRX = 0;
		len = 0;
		//countRX = 0;
		//synced = true;
		escaped = false;
	};
	if (countRX > 0 && val == ESCAPE_VAL)
	{
		escaped = true;
		return NULL;
	};
	if (escaped == true) {
		val = 0x20 ^ val;
		escaped = false;
	};

	switch (countRX)
	{
		case 0:
			if (val == SYNC_VAL) {
				countRX++;
				crcPKT = 0;
				crcRx = 0;
			}

			break;
		case 1:
			// length msb
			len = val;
			countRX++;

			break;
		case 2:
			// length lsb
			len = len << 8 | val;
			countRX++;

			break;
		default:
			// starts at fourth byte
			// When we are here we know that we are past the start byte and length
			//	So compute the crc on everything except the crc bytes
			if (countRX - HEADER_LEN < len)
			{
				crcRx = ComputeCRC(val, crcRx);
			};

			if (countRX > MAX_LENGTH) {
				// exceed max size.  should never occur
				return NULL;
			};
			byteCacheRX[countRX - HEADER_LEN]  = val;
			countRX++;
			if (countRX >= (len + HEADER_LEN + 2 ))
			{
				// we have received the last byte of the crc
				// validate the crc
				
				crcPKT = (byteCacheRX[countRX - HEADER_LEN - 2] << 8) | (byteCacheRX[countRX - HEADER_LEN - 1]);
				if (crcPKT == crcRx)
				{
					Message *m;
					m = allocMessage();
					m->length = len;	// number of bytes minus crc.

					for (int i = 0; i < len; i++)
					{
						m->data[i] = byteCacheRX[i];
					}
					escaped = false;
					countRX = 0;
					return m;
				
				} else
				{
					// bad crc
					//
					len = 0;
					countRX = 0;
					escaped = false;
					errCRC += 1;
					//if failed ; try again
					return NULL;

				};
			};
			break;
	};
	return NULL;
}

uint16_t HDLC::ComputeCRC(byte val, uint16_t runningCRC)
{
	runningCRC = (uint16_t)(runningCRC ^ (val << 8));
	for (int i = 0; i < 8; i++)
	{
		if ((runningCRC & 0x8000) != 0)
		{
			runningCRC = (runningCRC << 1) ^ 0x1021;
		}
		else
		{
			runningCRC = (runningCRC << 1);
		}
	}
	return (runningCRC & 0xffff);
}
		