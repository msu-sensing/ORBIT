package edu.msu.boardinf;

public class Time {
	
	private int
			hour,
			min,
			sec,
			millisec;
		
	public Time() {
		hour = min = sec = millisec = 0;
	}
	public Time(int[] hms_ms) {
		setTime(hms_ms);
	}
	public void setTime(int[] timeIntArry) {
			int i=0;
			hour =  timeIntArry[i]; 
			min =  timeIntArry[++i];
			sec =  timeIntArry[++i];
			millisec =  timeIntArry[++i];		
	}
	
	public long getUnixTime() {
		long unixTime = ((((this.hour*60) + this.min ) * 60 + this.sec ) * 1000 + this.millisec );
		return unixTime;
	}
	
	public int getTimeInSec() {
		return hour* 3600 + min * 60 + sec + millisec/1000;
	}
	public int[] getTimeDetails() {		
		return new int[]{hour, min, sec, millisec};
	}
	
	public void setTime(Time t) {
		setTime(t.getTimeDetails());		
	}
		
}
