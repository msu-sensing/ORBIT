package edu.msu.boardinf;

import java.io.FileOutputStream;
import java.io.IOException;
import android.util.Log;

public class ArduinoProtocol {

	private static final String TAG = "ServiceADK_ArduinoProtocol";
	private static FileOutputStream mOutputStream;
	
	private static final int numOfByteForTime = 7;
	private static final int numOfByteFor1DReading = 2;
	private static final int numOfByteFor3DReading = 3 * numOfByteFor1DReading ;
	private static final int numOfByteForReading_total = numOfByteForTime + numOfByteFor3DReading;

	public static String debugMessage = "Debug Message is null \n";
	
	
	
	
	public static class ARG_TYPES {
		public static final byte
			INT16TYPE 			= 0x01,
			INT32TYPE 			= 0x02,
			FLOATTYPE			= 0x03,
			VECTORINT16TYPE 	= 0x11,
			VECTORINT32TYPE 	= 0x12,
			VECTORFLOATTYPE		= 0x13,
			VECTORSAMPLETYPE	= 0x14,
			VECTORPOSITIONTYPE 	= 0x15,
			VECTORDATETIMESHORTTYPE = 0x16,		// does not include milliseconds(TIMESHORT)
			VECTORTIMINGSTYPE 	= 0x17,
			VECTORUSERTYPE = (byte)0xFF; 		// why casting needed?
				 		
	}
 
	
    public static class MessageType{
    	public static final byte
    		NullMessage			  = 0x00,		// NULL Message
    		SET_DEBUG             = 0x01,		//turn on, off debug output
			SET_SAMPLING		  = 0x02,		//turn on, off adc sampling
			SET_PERIOD			  = 0x03,		//set adc sampling period
			GET_STATUS			  = 0x04,		//request status
			REPORT_STATUS		  = 0x04,		//report status
			GET_AVAIL			  = 0x05,		//request number of samples available
			REPORT_AVAIL		  = 0x05,		//Report number of samples available
			GET_SAMPLES			  = 0x06,		//request n samples
			SEND_SAMPLES		  = 0x06,		//send specified number of samples
			SAMPLE_NOTIFICATION	  = 0x07,		//set number of samples before notification
			REPORT_SAMPLES		  = 0x07,		//status message when sample limit reached
			GET_GPS_STATUS		  = 0x08,		//request gps status
			REPORT_GPS_STATUS	  = 0x08,		//report gps status
			GET_GPS_POSITION	  = 0x09,		//request position
			REPORT_GPS_POSITION	  = 0x09,		//report position
			SEND_TASK_LIST		  = 0x0A,		// send a task list to board
			TASK_OPERATION		  = 0x0B,		// perform a task operation
			SET_TASK_ARG		  = 0x0C,		// set a task input argument
			SEND_TASK_RESULT	  = 0x0C,		// send results of task to phone
			ROBOT_CONTROL		  = 0x0D,		// Robot control
			ROBOT_STATUS		  = 0x0D,		// Robot sensor status
			
			LOG_MESG			  = 0x0E,		// log Message
			TEXT_MESG 			  = 0x0F,		//Text Message

			// BOOKMARK(add_feature): Add new message type to enum.
    		SYNC				  = (byte) 0xFF;		// Special synchronize message
	} 
    
    public static class TASK_OPERATIONS {
    	public static final byte 
    			TASK_EXECUTE = 0x01,
    			TASK_DELETE = 0x02,
    			TASK_STOP = 0x03;
	}	
	
	
	/**
	 *  Board-Phone Protocol methods
	 */
    public static byte[] task_operation(int taskListId, int operation) {
    	long params[] = new long[2];
    	params[0] = taskListId;
    	params[1] = operation;
		byte[] MessageBuffer = formatMessage(MessageType.TASK_OPERATION, params);
		return MessageBuffer;
    }
	public static byte[] get_sample_notification(int numOfSamples) {
		long params[] = new long[]{numOfSamples};
		byte[] MessageBuffer = formatMessage(MessageType.SAMPLE_NOTIFICATION, params);
		return MessageBuffer;
	}
	
	public static byte[] getNSamples(int numOfSamples) {

		long params[] = new long[1];
		params[0] = numOfSamples;
		byte[] Messagebuffer = formatMessage(MessageType.GET_SAMPLES, params);
	//	writeByteBuffer(Messagebuffer);
		return Messagebuffer;
		
	} //getNSamples()
	
	public static byte[] getStatus() {
		long params[] = null;
		byte[] Messagebuffer = formatMessage(MessageType.REPORT_STATUS, params);
		return Messagebuffer;
		
	} //getStatue()
	
	public static byte[] setSamplingPeriod(long timePeriod) {
		long params[] = {timePeriod};
		byte[] Messagebuffer = formatMessage(MessageType.SET_PERIOD, params);
		return Messagebuffer;
	
	}//SetSamplingPeriod
	
	
	/**
	 * formatMessage
	 * @param msgType
	 * @param params
	 * @return formattedMassage
	 */
	private static byte[] formatMessage(byte msgType, long params[]){
		short numOfbytes = 0;
		byte[] buffer = null;
		
		switch (msgType) {
		case MessageType.TASK_OPERATION:
			byte[] taskListId = intToByteArray((int) params[0] , 1);
			byte[] operation = intToByteArray((int) params[1] , 1);
			numOfbytes = 4;
			buffer = new byte[numOfbytes];
			buffer[0] = msgType;
			buffer[1] = 0;
			buffer[2] = taskListId[0];
			buffer[3] = operation[0];		
			buffer[1] = computeCheckSum(buffer);
			return buffer;							
		
		case MessageType.SAMPLE_NOTIFICATION :
			byte[] numOfSamples = intToByteArray((int)params[0], 2);
			numOfbytes = 4;
			buffer = new byte[numOfbytes];
			buffer[0] = msgType;
			buffer[1] = 0;
			buffer[2] = numOfSamples[0];
			buffer[3] = numOfSamples[1];		
			buffer[1] = computeCheckSum(buffer);
			return buffer;

		case MessageType.GET_SAMPLES :
			
			byte[] numOfSml = intToByteArray((int) params[0] , 2);

			numOfbytes = (short) (2 + numOfSml.length);
			buffer = new byte[numOfbytes];
			
			if (numOfSml.length <= 252 ) {
				buffer[0] = msgType;
				buffer[1] = 0;  				//We reserve this byte for checksum which will be computed at the end
								
				for (int x = 0; x < numOfSml.length; x++) {
					buffer[2 + x] = numOfSml[x];
				}
			}
			
			buffer[1] = computeCheckSum(buffer);
			return buffer;
		
		case MessageType.REPORT_STATUS :
			numOfbytes = 2;
			buffer = new byte[numOfbytes];
			buffer[0] = msgType;
			buffer[1] = 0;
			//add more bytes here?
			buffer[1] = computeCheckSum(buffer);
			return buffer;
			
		case MessageType.SET_PERIOD :
			numOfbytes = 10;
			buffer = new byte[numOfbytes];
			buffer[0] = msgType;
			buffer[1] = 0;
			//add more bytes here
			buffer[1] = computeCheckSum(buffer);
			return buffer;
		
		default :
			numOfbytes = 2;
			buffer = new byte[numOfbytes];
			buffer[0] = MessageType.NullMessage;
			buffer[1] = 0;
			//add more bytes here
			buffer[1] = computeCheckSum(buffer);
			return buffer;
		}

	}//formatMessage()
	
	/**
	 * decompose Message
	 * @param byte[] recviedMessage
	 * @return decomposed message
	 */
	
	public static Message decomposeMessage(byte [] recviedMessage) {
		Message message = new Message(recviedMessage[0]);
	//	message.msgType = recviedMessage[0];
	//	Message.msgType = MessageType.NullMessage;
		
	//	boolean isCorrupted = ( computeCheckSum(recviedMessage) ==0 );
		byte chksumInMesg = recviedMessage[1];
		recviedMessage[1] = 0;
		byte chksumComputed = computeCheckSum(recviedMessage);
		boolean isCorrupted = (chksumInMesg != chksumComputed);
		
		Log.d(TAG, "message corrupted = " + isCorrupted);
		if (!isCorrupted) {
			
			//* switch over different received message types
			switch (message.msgType) {
			
			case MessageType.LOG_MESG:
				Log.d(TAG,"Got a LOG_MSG");
				LOGGING_ARGS log_message = new LOGGING_ARGS(message);
				int TIMEsize = 7;
				
				Time logTimeObj = byteArrayToTime(recviedMessage , 2 , TIMEsize);
				log_message.logTime = logTimeObj.getUnixTime();
				log_message.length = (int)recviedMessage[9];
				Log.d(TAG,"LOG_MSG time processed");
			
				byte[] logStringBytes = new byte[log_message.length];					

				for (int i=0; i < logStringBytes.length; i++)
					logStringBytes[i] = recviedMessage[10+i];
				
				Log.d(TAG,"LOG_MSG bytes got ready");
				
				log_message.msg = new String(logStringBytes);
				//log_message.msg = byte
	
			//	log_message.msgType = message.msgType;
				Log.d(TAG,"LOG_MSG returend and content is: " + log_message.msg);
				return log_message;
				
			//** SEND_TASK_RESULT scope starts
			case MessageType.SEND_TASK_RESULT :  
				TASK_RESULT_ARGS result_message = new TASK_RESULT_ARGS(message);		

				result_message.task_id = recviedMessage[2];
				result_message.arg_id =  recviedMessage[3];
				result_message.num_msg = recviedMessage[4];		
				result_message.seq = recviedMessage[5];	
				result_message.max_result =  (int) byteArrayToUInt(recviedMessage, 6, 2);
				result_message.type = recviedMessage[8];						
				result_message.num_result = recviedMessage[9];
			
				if (result_message.seq == 1) {
					Log.d(TAG, "got first sequence");		
					result_message.nele = 0;
				}	
				
				Log.d(TAG,"SEND_TASK_RESULT header is processed and seg is " + result_message.seq );

				//* switch over different "Result Types" in SEND_TASK_RESULT
				switch (result_message.type ) {
				
				case ARG_TYPES.VECTORSAMPLETYPE :
					Log.d(TAG, "Result type is VECTORSAMPLETYPE");
					
					int numOfSamples = result_message.num_result;
					
					Log.d(TAG, "VECTORSAMPLETYPE args processed 1");

					if ( numOfSamples > 0) {
						result_message.result = new int[numOfSamples];
						result_message.timeStamps = new long[numOfSamples];
					}
					else {
						result_message.result = null;
						Log.e(TAG, "oooo result is null");
					}
					// Process the VECTORSAMPLETYPE
					Log.d(TAG, "VECTORSAMPLETYPE args processed 2");

					int TIMESHORT_size = 5; 	// # of bytes (see protocol_defs)
					int DATE_size = 3; 			// # of bytes
					int SampleVal_size = 2;		// # of bytes
					int sampleSize = TIMESHORT_size +  DATE_size +  SampleVal_size;
					
					if (result_message.result !=null)
						for(int i=0; i< result_message.num_result ; i++) {	
							
							int offset = 10 + i*sampleSize;
							Log.d(TAG, "VECTORSAMPLETYPE args processed nele so far: " + result_message.nele + "/" + result_message.num_result );
	
			//				Date date = new Date((int)recviedMessage[offset + 2] + 2000 ,(int)recviedMessage[offset + 1],(int)recviedMessage[offset + 0]);
			//				date.setHours(0);
			//				date.setMinutes(0);
			//				date.setSeconds(0);
			//				Log.d(TAG, "VECTORSAMPLETYPE args processed Date : " + date.getTime());
	
							Time tm = byteArrayToTime(recviedMessage , offset + DATE_size , TIMESHORT_size);					
							Log.d(TAG, "VECTORSAMPLETYPE args processed Time: " + tm.getUnixTime());
							
							if (tm !=null) {
							//	result_message.timeStamps[result_message.nele] = tm.getUnixTime() + date.getTime();
								result_message.timeStamps[result_message.nele] = tm.getUnixTime();
								Log.d(TAG, "VECTORSAMPLETYPE args processed timeStamp");
							}
							else
								Log.e(TAG, "wow got null time !");

							result_message.result[result_message.nele] = (int) byteArrayToInt(recviedMessage, offset + DATE_size + TIMESHORT_size, SampleVal_size);
							Log.d(TAG, "VECTORSAMPLETYPE args processed nele");

							result_message.nele++;
						}
					else
						Log.e(TAG, "Shoot result array is Null !!");
					
					//result_message.msgType = message.msgType;
					Log.d(TAG,"VECTORSAMPLETYPE returend with " + result_message.result.length + " elements");
					return result_message;
				
				case ARG_TYPES.VECTORPOSITIONTYPE :
					Log.d(TAG, "Result type is VECTORPOSITIONTYPE");

					REPORT_POSITION_ARGS position_message = new REPORT_POSITION_ARGS(result_message);
					Log.d(TAG, "REPORT_POSITION_ARGS processed");
					
					position_message.latInd = (char) recviedMessage[10];
					Log.d(TAG, "REPORT_POSITION_ARGS processed latInd: " + position_message.latInd);

					position_message.latitude = (int) byteArrayToUInt(recviedMessage, 11, 4);			
					Log.d(TAG, "REPORT_POSITION_ARGS processed latitude: " + position_message.latitude);

					position_message.longInd = (char) recviedMessage[15];
					Log.d(TAG, "REPORT_POSITION_ARGS processed longInd: " + position_message.longInd);

					position_message.longitude = (int) byteArrayToUInt(recviedMessage, 16, 4);															
					Log.d(TAG, "REPORT_POSITION_ARGS processed longitude: " + position_message.longitude);

			//		result_message.msgType = msgType;
					Log.d(TAG,"VECTORPOSITIONTYPE returend");
					return position_message;
				 
				case ARG_TYPES.VECTORDATETIMESHORTTYPE : //for p-phase
					Log.d(TAG, "Result type is VECTORDATETIMESHORTTYPE");

					Pphase pPhase_message = new Pphase(result_message);
					
					int offset = 10;
					int TIMESHORTsize = 5; 			// # of bytes (see protocol_defs)
					int DATEsize = 3; 				// # of bytes
			//		Date date = new Date((int)recviedMessage[offset + 2] + 2000,(int)recviedMessage[offset + 1],(int)recviedMessage[offset + 0]);
			//		date.setHours(0);
			//		date.setMinutes(0);
			//		date.setSeconds(0);
					Time tm = byteArrayToTime(recviedMessage , offset + DATEsize , TIMESHORTsize);
					
					if ( tm != null) {
			//			pPhase_message.eventTime = tm.getUnixTime() + date.getTime();
						pPhase_message.eventTime = tm.getUnixTime();
						Log.d(TAG, "Got pphase and is: " + pPhase_message.eventTime);
					} else 
						Log.e(TAG, "Wow pphase time is null !");
							
			//		pPhase_message.msgType = msgType;
					Log.d(TAG,"VECTORDATETIMESHORTTYPE returend");
					return pPhase_message;
					
				case ARG_TYPES.VECTORTIMINGSTYPE :
					Log.d(TAG, "Result type is VECTORTIMINGSTYPE");
					
					TIMINGS_TYPE timings_message = new TIMINGS_TYPE(result_message);
					
					Log.d(TAG, "VECTORTIMINGSTYPE args processed 1");

					offset = 10;
					TIMEsize = 7;
					int timingMeasureSize = 4; //# of bytes for each timing measurement (protocol defs)
					Time timingTm = byteArrayToTime(recviedMessage, offset, TIMEsize);
					
					Log.d(TAG, "VECTORTIMINGSTYPE args processed 2");

					if (timingTm !=null){
						timings_message.unixTime = timingTm.getUnixTime();
						Log.d(TAG, " got time : " + timingTm.getUnixTime());
					}
					else
						Log.e(TAG, "got a null time !!");
					
					Log.d(TAG, "VECTORTIMINGSTYPE args processed 3");

					int num = (int)recviedMessage[offset + TIMEsize];
					
					if (num < 0) {
						Log.e(TAG, "Wow, num < 0  :" + num);

						num = 1;	// just to prevent exception!!
					}
					timings_message.timings = new int[num];
					
					Log.d(TAG, "VECTORTIMINGSTYPE args processed 4");

					for (int i=0; i<num; i++)
						timings_message.timings[i] = (int) byteArrayToInt(recviedMessage, offset + TIMEsize + 1 + i*timingMeasureSize, timingMeasureSize);
					
			//		timings_message.msgType = msgType;
					Log.d(TAG,"VECTORTIMINGSTYPE returned content is: " + timings_message.timings.toString() );
					return timings_message;
					
				default:
					Log.d(TAG,"I dont know what to do, I ma lost :-(");
					break;
						
				} // end of different TASK_RESULT_ARGS

				Log.d(TAG,"what the hec I'm doing here!");

				message = result_message;
				break;			
				//** SEND_TASK_RESULT scope ends
			default :
					// so what?
				break;
			

			}// end of different received message type
			
		} else {
			// error: message is corrupted
			Log.e(TAG,"oopps!! Mesasge is CORRUPTED !!"); 
		}
		
		return message;
	}//decomposeMessage()
	
	/**
	 *  Utility methods
	 */
	
	private static void writeByteBuffer(byte[] buffer) {
		if (mOutputStream != null) {
			try {
				mOutputStream.write(buffer);
			} catch (IOException e) {
				Log.e(TAG, "write failed", e);
			}
		}
	}
	
	protected static byte computeCheckSum(byte[] msg) {
	    byte cs = 0;
        
        for (int i = 0; i < msg.length; i++)
        	cs =(byte)( cs ^ msg[i]);				//bitwise exclusive OR operation.

        return cs;
	}
	
	private static byte[] intToByteArray(int value , int numOfbytes) {
	    byte [] bytes =  new byte[numOfbytes];
	    
	    for (int i=0 ; i<numOfbytes; i++ ) {
	    	bytes[i] = (byte) ( value >> (8*i)) ;
	    }
	    return bytes;
    }
	
	//Signed
	private static long byteArrayToInt(byte[] byteBuffer, int startOffset,int numOfbytes) {
		long value = 0;
		long t = 0;
		
		if ( (numOfbytes + startOffset ) > byteBuffer.length )
			return -1;
		if (byteBuffer[startOffset + numOfbytes - 1] < 0)
			value = -1;
	    for (int i=numOfbytes - 1; i>=0; i--) {
	    	t = byteBuffer[startOffset + i];
	    	value = (value << 8) | (t & 0x000000ff);	    	
	    }
	    return value;
	}
	
	//UnSigned
	private static long byteArrayToUInt(byte[] byteBuffer, int startOffset,int numOfbytes) {
		long value = 0;
		long t =0;
		if ( (numOfbytes + startOffset ) > byteBuffer.length )
			return -1;
	    for (int i=numOfbytes - 1; i>=0; i--) {
	    	t = byteBuffer[startOffset + i];
	    	t = t & 0x000000ff;
	    	value = (value << 8) | t; 
	    }
	    return value;
	}
	
	private static Time byteArrayToTime(byte[] byteBuffer, int startOffset, int numOfbytes) {
		
		if ( (numOfbytes + startOffset ) > byteBuffer.length )
			return null;
		
		Time t = new Time();
		int[] hms_ms = new int[4];
		
		for(int i=0; i < 3; i++) {
	//		hms_ms[i] = byteBuffer[startOffset + i];
			hms_ms[i] = (int) byteArrayToUInt(byteBuffer, startOffset + i, 1); 
		}
		hms_ms[3] = (int) byteArrayToUInt(byteBuffer, startOffset + 3, numOfbytes - 3); // 3 bytes are used for hr, min and sec so 2 bytes is left for millisec which is correct
		
		t.setTime(hms_ms);
		Log.d(TAG,"byteArrayToTime " + t.getUnixTime());
		return t;
	}

}// end of class
