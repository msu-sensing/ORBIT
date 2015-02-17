
//$Id: Packetizer.java,v 1.7 2007/08/20 23:50:04 idgay Exp $

/*									tab:4
* "Copyright (c) 2000-2003 The Regents of the University  of California.  
* All rights reserved.
*
* Permission to use, copy, modify, and distribute this software and its
* documentation for any purpose, without fee, and without written agreement is
* hereby granted, provided that the above copyright notice, the following
* two paragraphs and the author appear in all copies of this software.
* 
* IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
* DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
* OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
* CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* 
* THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
* ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
* PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
*
* Copyright (c) 2002-2003 Intel Corporation
* All rights reserved.
*
* This file is distributed under the terms in the attached INTEL-LICENSE     
* file. If you do not find these files, copies can be found by writing to
* Intel Research Berkeley, 2150 Shattuck Avenue, Suite 1300, Berkeley, CA, 
* 94704.  Attention:  Intel License Inquiry.
*/
//package net.tinyos.packet;
package edu.msu.boardinf;

//import net.tinyos.util.*;

import java.util.*;

import android.util.Log;

/**
* The Packetizer class implements the new mote-PC protocol, using a ByteSource
* for low-level I/O
*/
/**
 * We are adapting this code for the Arduino-smartDevices(Android) using USB-host 
 * for communication 
 * @author Mohammad
 *
 */
public class Packetizer {
/*
* Protocol inspired by, but not identical to, RFC 1663. There is
* currently no protocol establishment phase, and a single byte
* ("packet type") to identify the kind/target/etc of each packet.
* 
* The protocol is really, really not aiming for high performance.
* 
* There is however a hook for future extensions: implementations
* are required to answer all unknown packet types with a P_UNKNOWN
* packet.
* 
* To summarize the protocol: 
* - the two sides (A & B) are connected by a (potentially
*   unreliable) byte stream
*
* - the two sides exchange packets framed by 0x7e (SYNC_BYTE) bytes
*
* - each packet has the form 
*     <packet type> <data bytes 1..n> <16-bit crc> 
*   where the crc (see net.tinyos.util.Crc) covers the packet type
*   and bytes 1..n
*
* - bytes can be escaped by preceding them with 0x7d and their
*   value xored with 0x20; 0x7d and 0x7e bytes must be escaped,
*   0x00 - 0x1f and 0x80-0x9f may be optionally escaped
*
* - There are currently 5 packet types: 
*   P_PACKET_NO_ACK: A user-packet, with no ack required
*   P_PACKET_ACK: A user-packet with a prefix byte, ack
*   required. The receiver must send a P_ACK packet with the 
*   prefix byte as its contents.  
*   P_ACK: ack for a previous P_PACKET_ACK packet 
*   P_UNKNOWN: unknown packet type received. On reception of an
*   unknown packet type, the receiver must send a P_UNKNOWN packet,
*   the first byte must be the unknown packet type. 
*
* - Packets that are greater than a (private) MTU are silently
*   dropped.
*/
	final static boolean DEBUG = false;
	
	final static int SYNC_BYTE = 0x7E;
	
	final static int ESCAPE_BYTE = 0x7D;
	
	final static int MTU = 256;
	
	private boolean inSync;
	
	private boolean escaped;
	
	private byte[] receiveBuffer = new byte[MTU];
	
	//Packets are received by a separate thread and placed in a
	//per-packet-type queue. 	
	//	private LinkedList[] received;
	
	/**  Add by Mohammad */ 
	private  int count;
	
	public LinkedList<byte []> receivedPackets; // Alternative to LinkedList[] received
	
	final static int NumOfMsg = 256;			// Total number of assembled decoded messages called packets and stored in the LinkedList (ArrayList)
	
	private final String TAG = "Packetizer"; 
		
	/**
	* Packetizers are built using the makeXXX methods in BuildSource
	*/
	public Packetizer() {
	
		inSync = false;
	//	received = new LinkedList[256];
	//	received = new LinkedList[NumOfMsg];
		
		/**
		 * Added by Mohammad
		 */
		count = 0;
		escaped = false;
		receivedPackets = new LinkedList<byte[]>();
	}


	// Read system-level packet. If inSync is false, we currently don't
	// have sync
	public boolean readFramedPacket(byte [] data, int length) {
		// int count = 0;
		// escaped = false;
	
		for (int i=0; i < length; i++) {
			byte b = data[i];
			if (!inSync) {
				//	message(name + ": resynchronising");
				//	re-synchronise
				if (b == SYNC_BYTE)  {      
					inSync = true;
					count = 0;
					escaped = false;
				} else
					continue;
			}
	
			if (count >= MTU) {
				//	 Packet too long, give up and try to resync
				//   message(name + ": packet too long");
				inSync = false;
				continue;
			}
	
			if (escaped) {
				if (b == SYNC_BYTE) {
					// 	sync byte following escape is an error, resync
					//  message(name + ": unexpected sync byte");
					inSync = false;
					continue;
				}
				b ^= 0x20;
				escaped = false;
			} else 
				if (b == ESCAPE_BYTE) {
					escaped = true;
					continue;
				} else 
					if (b == SYNC_BYTE) {
						if (count < 4) { 
							// too-small frames are ignored
							Log.e(TAG, " ^ too small frame ^");
							count = 0;
							continue;
						}
						byte[] packet = new byte[count - 2];
						System.arraycopy(receiveBuffer, 0, packet, 0, count - 2);
	
						int readCrc = (receiveBuffer[count - 2] & 0xff)
								| (receiveBuffer[count - 1] & 0xff) << 8;
						int computedCrc = Crc.calc(packet, packet.length);
	
						if (readCrc == computedCrc) {
							//	TO-DO: put assembled packet in receive queue add to the received linkedlist  (DONE)
							// 	Added by Mohamamd
							receivedPackets.add(packet);
							Log.e(TAG, " ^ packet added ^");
							count = 0;
							continue;
						} else {
							// message(name + ": bad packet");
							/* We don't lose sync here. If we did, garbage on the line at startup
							 * will cause loss of the first packet.
							 */
							Log.e(TAG, " ^ bad packet ^");
							count = 0;
							continue;
						}
					}
	
				receiveBuffer[count++] = b;
			}//for-loop
				
			// TO-DO: see if there are any packets in the list if yes return true else return false (DONE)
			// Added by Mohammad
			if (!receivedPackets.isEmpty())
				return true;
			else
				return false;
	}// readFramedPacket (decoder)

	/**
	 * This Actually our Encoder
	 * @param packet
	 * @param count
	 * @return the encoded message to the HDLC format
	 */
	// Write a packet bytes 0..'count' in 'packet'
	public byte[] writeFramedPacket( byte[] packet, int count) {
		
		Escaper buffer = new Escaper(count + 6);
	
		for (int i = 0; i < count; i++) {
			buffer.nextByte(packet[i]);
		}
	
		int crc = buffer.crc;
		buffer.nextByte(crc & 0xff);
		buffer.nextByte(crc >> 8);
	
		buffer.terminate();
		
		byte[] realPacket = new byte[buffer.escapePtr];
		System.arraycopy(buffer.escaped, 0, realPacket, 0, buffer.escapePtr);
		
		return realPacket;
	}//writeFramedPacket (Encoder)
	
	// Class to build a framed, escaped and CRCed packet byte stream
	static class Escaper {
		 byte[] escaped;
		
		 int escapePtr;
		
		 int crc;
		
		 // We're building a length-byte packet
		 Escaper(int length) {
			 escaped = new byte[2 * length];
			 escapePtr = 0;
			 crc = 0;
			 escaped[escapePtr++] = SYNC_BYTE;
		 }
	
		 static private boolean needsEscape(int b) {
			 return b == SYNC_BYTE || b == ESCAPE_BYTE;
		 }
	
		 void nextByte(int b) {
			 b = b & 0xff;
			 crc = Crc.calcByte(crc, b);
			 if (needsEscape(b)) {
				 escaped[escapePtr++] = ESCAPE_BYTE;
				 escaped[escapePtr++] = (byte) (b ^ 0x20);
			 } else {
				 escaped[escapePtr++] = (byte) b;
			 }
		 }
		
		 void terminate() {
		   escaped[escapePtr++] = SYNC_BYTE;
		 }
	}//Escaper

	
}//Packetizer Class
