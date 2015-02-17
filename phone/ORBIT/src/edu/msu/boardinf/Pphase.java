package edu.msu.boardinf;

public class Pphase extends TASK_RESULT_ARGS {
	
	public long eventTime = 0;		//event date and time in millisecond in Unixtime format
	
	public Pphase(TASK_RESULT_ARGS result_message) {
		super(result_message);
		this.arg_id = result_message.arg_id;
		this.task_id = result_message.task_id;
		this.num_msg = result_message.num_msg;
		this.seq = result_message.seq;
		this.max_result = result_message.max_result;
		this.type = result_message.type; 			// result type 
		this.num_result = result_message.num_result; 						
		this.nele = result_message.nele;
		 	
	}
}