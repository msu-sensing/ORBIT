package edu.msu.boardinf;

import edu.msu.boardinf.ArduinoProtocol.MessageType;

public class Message {
	//public int numOfreading = 0;
	
	//public Data3D [] reading3D = null;
	public byte msgType = MessageType.NullMessage ;
	//public boolean debug = false;
	//public boolean sampling = false;
	//public long samplingPeriod = 0;
	
	public Message(byte type) {
		this.msgType = type;
	}
}


