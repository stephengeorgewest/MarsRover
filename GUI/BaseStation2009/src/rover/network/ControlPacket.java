package rover.network;

import java.util.ArrayList;

import rover.utils.BinUtils;

public class ControlPacket extends Packet {
	
	public static byte CONTROL_HEADER = 32;
	
	public byte Channels = -1;
	
	public byte[] Channel_list;
	public float[] Value_list;
	
	public ControlPacket(){
		super();
		Channel_list = new byte[100];
		Value_list = new float[100];
	}
	
	public ControlPacket(byte channels){
		super();
		Channels = channels;
		Channel_list = new byte[Channels];
    	Value_list = new float[Channels];
	}
	
	public ControlPacket(byte[] packet, int bytes) {
		super(packet, bytes);
		Channel_list = new byte[100];
		Value_list = new float[100];
	}

	public ControlPacket(byte[] channels, float[] values){
		Channel_list = channels;
		Channels = (byte)(channels.length);
		Value_list = values;
	}
	
	public void setData(byte[] channels, float[] values){
		Channel_list = channels;
		Channels = (byte)(channels.length);
		Value_list = values;
	}
	
	public boolean FromByteArray(byte[] packet, int bytes)
	{
		if(bytes < 7) return false;
	    try
	    {
	    	if(packet[0] != CONTROL_HEADER) return false;
	        Channels = packet[1];
	        if(Channel_list.length < Channels){
	        	Channel_list = new byte[Channels];
	        	Value_list = new float[Channels];
	        }
	        for(int i = 0;i<Channels;i++){
	        	Channel_list[i] = packet[2+5*i];
	        	Value_list[i] = Float.intBitsToFloat(BinUtils.byteArrayToInt(packet, 2+5*i + 1));
	        }
	        return true;
	    }
	    catch(Exception e){
	    	return false;
	    }
	}
	
	public boolean ToByteArray(){
		if(packet == null || packet.length < 2+5*Channels){
			packet = new byte[2+5*Channels];
		}
		
		bytes = 2+5*Channels;
		packet[0] = CONTROL_HEADER;
		packet[1] = Channels;
		for(int i = 0;i<Channels;i++){
			packet[2+5*i] = Channel_list[i];
			
			int val = Float.floatToIntBits(Value_list[i]);
			BinUtils.intToByteArray(val, packet, 2+5*i + 1);
//			packet[2+5*i + 1] = (byte)((val >> 24) & 0x000000FF);
//			packet[2+5*i + 2] = (byte)((val >> 16) & 0x000000FF);
//			packet[2+5*i + 3] = (byte)((val >> 8) & 0x000000FF);
//			packet[2+5*i + 4] = (byte)(val & 0x000000FF);
			
			//System.out.println(packet[2+5*i + 1] + " " + packet[2+5*i + 2] + " " + packet[2+5*i + 3] + " " + packet[2+5*i + 4]);
			
        }
		return true;
		
	}
	
}
