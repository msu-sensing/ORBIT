package edu.msu.boardinf;


public class LOGGING_ARGS extends Message {
	public int length = -1;
	
	//public char[] msg = null;
	public String msg = "nothing!";
	public	long logTime;			//UnixTime (without date!!)
	
	public LOGGING_ARGS(Message msg) {
		super(msg.msgType);
	}
}
