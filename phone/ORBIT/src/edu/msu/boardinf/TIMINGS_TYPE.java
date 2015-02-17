package edu.msu.boardinf;

public class TIMINGS_TYPE extends TASK_RESULT_ARGS {
	
	public long unixTime = 0;
	public int num;
	public int[] timings;
	
	public TIMINGS_TYPE(TASK_RESULT_ARGS result_message) {
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
	public String toCSV() {
		String temp = "";
		for(int i =0; i<this.timings.length; i++) {
			if(temp.length() > 0)
				temp +=",";
			temp += this.timings[i];
		}
		return temp;
	}
	
}
