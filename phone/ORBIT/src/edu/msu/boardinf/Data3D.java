package edu.msu.boardinf;

public class Data3D {
	
	private Time time;
	private int[] values; 
	
	public Data3D() {
		values = new int[]{0, 0,0};
		time = new Time();
	}
	public Data3D(int[] vals, Time t) {
		time = new Time(t.getTimeDetails());
		
		values = new int[3];
		values [0] = vals[0];
		values [1] = vals[1];
		values [2] = vals[2];
	}
	public void setValues(int[] vals){
		values [0] = vals[0];
		values [1] = vals[1];
		values [2] = vals[2];
	}
	public void setTime(Time t) {
		time.setTime(t);
	}
	public int getValue(int axisIndex) {
		return values[axisIndex];
	}

}
