package edu.msu.boardinf;

import java.io.BufferedWriter;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;


import android.os.ParcelFileDescriptor;
import android.util.Log;

public class ArduinoProtocolTest {
	private static final String TAG = "ArduinoProtocolTest ";
	public static String debugMessage = TAG;
	public static boolean AccessoryIsOpen = false;
	public static boolean msgRecived = true;
	public static boolean msgSent = false;
	
	public static ParcelFileDescriptor mFileDescriptor;
	public static FileInputStream mInputStream;
	public static FileOutputStream mOutputStream;

	public static void TimerMethod() {
		// TODO Auto-generated method stub
		 int i = 0, j = 0;
		 byte[] MessageByte;
		 
		 if(msgRecived){				 
			 MessageByte = ArduinoProtocol.get_sample_notification(5);
			 if (AccessoryIsOpen) { 
				 send(MessageByte);
				 long sentTime = System.nanoTime();
				 msgSent = true;
				 msgRecived = false;
				 debugMessage +="Sending Message " + i++ + " @ " + sentTime*1e-9 + "\n" ;
			 }
		 }			 
	
		 if (!AccessoryIsOpen)
			 debugMessage +="Timer Method: Accessory is Closed \n";
	
		 if(msgSent) {
			 if (AccessoryIsOpen) {
				 MessageByte = receive();
				 long recvdTime = System.nanoTime();
				 msgRecived = true;
				 msgSent = false;
				 debugMessage +="Receiving Message " + j++ + " @ " + recvdTime*1e-9 + " \n" ;
			 }			 
		 }
	}//TimerMethod();
	
	Runnable myCommRunnable = new Runnable() {
		@Override
		public void run() {
			
			 int i = 0, j = 0;
			 byte[] MessageByte;
			 
			 for (int k=0; k < 10; k++){
			 if(true){				 
				 MessageByte = ArduinoProtocol.get_sample_notification(5);
				 if (AccessoryIsOpen) { 
					 send(MessageByte);
					 long sentTime = System.nanoTime();
					 msgSent = true;
					 msgRecived = false;
					 debugMessage +="Sending Message " + i++ + " @ " + sentTime*1e-9 + "\n" ;
				 }
			 }}			 
		
			 if (!AccessoryIsOpen)
				 debugMessage +="Accessory is Closed \n";
		
			 if(msgSent) {
				 if (AccessoryIsOpen) {
					 MessageByte = receive();
					 long recvdTime = System.nanoTime();
					 msgRecived = true;
					 msgSent = false;
					 debugMessage +="Receiving Message " + j++ + " @ " + recvdTime*1e-9 + " \n" ;
				 }			 
			 }
		}
	};//myCommRunnable
	
	public static void send(byte[] MessageBuffer) {	
		writeByteBuffer(MessageBuffer);
		long time = System.currentTimeMillis();
		debugMessage +=" Sent @ time " + time*1e-3 + "\n";
	}
	
	public static byte[] receive() {
		int ret = 0;
		byte[] MessageBuffer = new byte[255];
		try {
			ret = mInputStream.read(MessageBuffer);
			if (ret>0) {
				long time = System.currentTimeMillis();
				debugMessage +="received @ " + time*1e-3 + "\n";
				return MessageBuffer;	
			} 
		} catch (Exception e) {
			debugMessage += e + " read buff err \n";
		}
		return MessageBuffer; 
	}

	private static void writeByteBuffer(byte[] buffer) {
		if (mOutputStream != null) {
			try {
				mOutputStream.write(buffer);
			} catch (IOException e) {
				Log.e(TAG, "write failed", e);
			}
		}
	}
	
	public void sendText(byte command, byte target, String text) {
		int textLength = text.length();
		byte[] buffer = new byte[3 + textLength];
		if (textLength <= 252) {
			buffer[0] = command;
			buffer[1] = target;
			buffer[2] = (byte) textLength;
			byte[] textInBytes = text.getBytes();
			for (int x = 0; x < textLength; x++) {
				buffer[3 + x] = textInBytes[x];
			}
			if (mOutputStream != null) {
				try {
					mOutputStream.write(buffer);
				} catch (IOException e) {
					Log.e(TAG, "write failed", e);
				}
			}
		}
	}//SendText()
	
	public static void synch(byte command, byte target) {
		
		byte[] buffer = new byte[2];
		
		buffer[0] = command;
		buffer[1] = target;
	
		if (mOutputStream != null) {
			try {
				mOutputStream.write(buffer);
			} catch (IOException e) {
				Log.e(TAG, "write failed", e);
			}
		}
	}//synch()
	
	void writeToTheFile(BufferedWriter out) {
	    
    	SimpleDateFormat s = new SimpleDateFormat("ddMMyyyyhhmmss");
    	String format = s.format(new Date());
        try {
            //if (root.canWrite()){
	            out.append("I got woked Up by USB :D !!" + format + "\n");
           //} else {
           // 	Log.d("TAG","file is not writeable");
           // }
        } catch (IOException e) {
        	Log.e("TAG", "Could not write file " + e.getMessage());
        }
    }//writeTotheFile
}
