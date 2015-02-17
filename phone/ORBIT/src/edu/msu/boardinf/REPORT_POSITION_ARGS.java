package edu.msu.boardinf;

public class REPORT_POSITION_ARGS extends TASK_RESULT_ARGS {
	public char latInd;
	public int latitude;
	public char longInd;
	public int longitude;
	
	public REPORT_POSITION_ARGS(TASK_RESULT_ARGS result_message) {
	//	this.msgType = result_message.msgType;
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
		return this.latInd + "," + this.latitude + "," + this.longInd + "," + this.longitude;
	}

}

