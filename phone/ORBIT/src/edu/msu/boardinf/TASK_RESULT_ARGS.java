package edu.msu.boardinf;

public class TASK_RESULT_ARGS extends Message {
	public int
		 task_id = 0,
		 arg_id = 0,
		 num_msg = 0,					// total number of messages needed to send results
		 seq = 0,						// number of this message
		 max_result = 0,				// total number of elements in this result
		 type = 0,						// result type
		 num_result = 0,				// number of elements of type in result for this message
		 nele = 0;						//number of elements received so far (in a segmented big message)
	
	public int[] result = null; 		//Actual sample values
	public long[] timeStamps = null;
	
	public TASK_RESULT_ARGS(Message msg) {
		super(msg.msgType);
	}

}



