package rover.network;
import rover.utils.BinUtils;


/*
 * Name: ControlPacket
 * Author: Travis L Brown
 * Description: 
 * Represents a packet designed to servo control information. Provides packet header 
 * generation and parsing capability.
 * 
 */
public class CameraPacket extends Packet {
	
	public static byte CAMERA_HEADER = 76;
	
	public float zoom = 1;
	
	public CameraPacket(){
		super();
	}
	
	public CameraPacket(float zoom){
		this.zoom = zoom;
	}
	
	public void setZoom(float zoom){
		this.zoom = zoom;
	}
	public boolean FromByteArray(byte[] packet, int bytes)
	{
		if(bytes < 5) return false;
	    try
	    {
	    	if(packet[0] != CAMERA_HEADER) return false;
	        zoom = Float.intBitsToFloat(BinUtils.byteArrayToInt(packet, 1));
	        return true;
	    }
	    catch(Exception e){
	    	return false;
	    }
	}
	
	public boolean ToByteArray(){
		if(packet == null || packet.length < 5){
			packet = new byte[5];
		}
		bytes = 5;
		packet[0] = CAMERA_HEADER;
		int zval = Float.floatToIntBits(zoom);
		BinUtils.intToByteArray(zval, packet, 1);
		
		System.out.println("zval = " + zval);
		
		for(int i = 0;i<4;i++){
			System.out.println(packet[i+1]);
		}
		
		return true;
	}
	
}
