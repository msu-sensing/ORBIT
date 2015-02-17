package edu.msu.boardinf;

public class RobotProtocol extends ArduinoProtocol{
	
	public RobotProtocol() {
		
	}
	
	public byte[] robotRange()
    {
        byte[] b = new byte[4];
        b[0] = 0x0D;
        b[1] = 0x00;
        b[2] = 0x06;
        b[3] = (byte) 0x84;
        b[1] = computeCheckSum(b);
        return b;
    }

	public byte[] robotPan(int angle)
    {
        byte[] b = new byte[5];
        b[0] = 0x0D;
        b[1] = 0x00;
        b[2] = 0x04;
        b[3] = (byte)(angle & 0x00ff);
        b[4] = (byte)(angle >> 8 & 0x00ff);
        b[1] = computeCheckSum(b);
       return b;
    }
    
    public byte[] robotTilt(int angle)
    {
        byte[] b = new byte[5];
        b[0] = 0x0D;
        b[1] = 0x00;
        b[2] = 0x05;
        b[3] = (byte)(angle & 0x00ff);
        b[4] = (byte)(angle >> 8 & 0x00ff);
        b[1] = computeCheckSum(b);
        return b;
    }
    
    public byte[] robotForward(int speed)
    {
        byte[] b = new byte[4];
        b[0] = 0x0D;
        b[1] = 0x00;
        b[2] = 0x01;
        b[3] = (byte)speed;
        b[1] = computeCheckSum(b);
        return b;
    }
    
    public byte[] robotBackward(long speed)
    {
         byte[] b = new byte[4];
        b[0] = 0x0D;
        b[1] = 0x00;
        b[2] = 0x01;
        b[3] = (byte)(-1*speed);
        b[1] = computeCheckSum(b);
        return b;
  }
    public byte[] robotRight(int speed)
    {
        byte[] b = new byte[4];
        b[0] = 0x0D;
        b[1] = 0x00;
        b[2] = 0x02;
        b[3] = (byte)(speed);;
        b[1] = computeCheckSum(b);
        return b;
    }
    
    public byte[] robotLeft(int speed)
    {
        byte[] b = new byte[4];
        b[0] = 0x0D;
        b[1] = 0x00;
        b[2] = 0x02;
        b[3] = (byte)(-1*speed);
        b[1] = computeCheckSum(b);
        return b;
    }
    
    public byte[] robotStop()
    {
        byte[] b = new byte[3];
        b[0] = 0x0D;
        b[1] = 0x00;
        b[2] = (byte) 0xFF;
        b[1] = computeCheckSum(b);
        return b;
    }

    public byte[] sendSync()
    {
        byte[] b = new byte[2];
        b[0] = (byte) 0xFF;
        b[1] = (byte) 0xFF;
        b[1] = computeCheckSum(b);
        return b;
   }
	

}
