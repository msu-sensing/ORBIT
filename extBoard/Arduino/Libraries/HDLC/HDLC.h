#include <inttypes.h>
#include "Arduino.h"

#define MAX_MESSAGES 10
#define SYNC_VAL 0x7E
#define ESCAPE_VAL 0x7D
#define MIN_LENGTH 4
#define HEADER_LEN 3	// data starts in byte 3, byte 0 is sync, bytes 1,2 length

struct Message
{
	int length;
	byte data[];
};
struct message_buffer
{
  Message *buffer[MAX_MESSAGES];
  volatile unsigned int head;
  volatile unsigned int tail;
};

class HDLC
{
  private:
    message_buffer *_mx_ringbuffer;
	int countRX; //number of bytes in the RX cache
	//int _pos;
	bool escaped;
	unsigned int len;
	unsigned int crcTx;
	int MAX_LENGTH;
	void store_message(Message *m);
	Message *allocMessage(void);
	int EncodeByte(int idx, byte *buff, byte val);
	Message *DecodeByte(byte val);
	//bool CorrectCRC();
	uint16_t ComputeCRC(byte val, uint16_t runningCRC);
	
 public:
	byte *byteCacheRX; //RX byte cache (longer than MTU)
	unsigned int crcPKT;
	unsigned int crcRx;
	int errCRC;
	int errShort;
	int errDropped;
    HDLC(int );
	void freeMessage(Message *res);
    int available(void);
	Message *EncodeBytes(byte *data, int len);
	bool DecodeBytes(int len, byte *data);
    Message *get_message(void);
};
