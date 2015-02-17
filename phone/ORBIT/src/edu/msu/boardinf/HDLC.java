package edu.msu.boardinf;

public class HDLC {
	
	private final int MAX_MESSAGES 	= 10 ;
	private final byte SYNC_VAL 	= 0x7E;
	private final byte ESCAPE_VAL  	= 0x7D;
	private final byte MIN_LENGTH 	= 4;
	
	
	private class Message {
		byte[] data;
		int lenght;
	}

	private class Message_buffer{
		Message[] buffer = new  Message[MAX_MESSAGES];
		volatile int head;		//unsigned	
		volatile int tail;		//unsigned
	}
	
	private
	    Message_buffer[] mx_ringbuffer;
		byte[] byteCacheRX; 	//RX byte cache (longer than MTU)
		int countRX; 			//number of bytes in the RX cache
		boolean escaped;
		boolean synced;
		int crcTx;				//unsigned
		int MAX_LENGTH;
		
		void store_message(Message m) {
		}
		
		Message allocMessage() {
			return null;
		}
		
		int EncodeByte(int idx, byte[] buff, byte val) {
			return 0;
		}
		Message DecodeByte(byte val) {
			return null;
		}
		boolean CorrectCRC() {
			return (Boolean) null;
		}
		int ComputeCRC(byte val, int runningCRC) {
			return 0;
		}
		
	 public
		int errCRC;
		int errShort;
		int errDropped;
	    HDLC(int i) {
		}
		void freeMessage(Message res) {
		}
	    int available() {
			return 0;
		}
		Message EncodeBytes(byte []data, int len) {
			return null;
		}
		boolean DecodeBytes(int len, byte[] data) {
			return false;
		}
	    Message get_message() {
			return null;
		}
}
