package rover.network;

import rover.utils.BinUtils;

public class ArmPacket{
	
	public static final byte ARM_PACKET_HEADER = 42;
	public static final byte ARM_JOINTS = 5;
	public static final byte TYPE_ONOFF = 1;
	public static final byte TYPE_GAINS = 2;
	public static final byte TYPE_CONTROL = 3;
	public static final byte TYPE_POSITION = 4;
		
	public Packet getArmPacket(byte[] packet, int bytes){
		if(packet == null || bytes == 0) return null;
		Packet p = null;
		switch(packet[2]){
		
		case TYPE_ONOFF:
			p = new ArmOnOffPacket(packet, bytes);
			break;
		case TYPE_GAINS:
			p = new ArmGainsPacket(packet, bytes);
			break;
			
		case TYPE_CONTROL:
			p = new ArmControlPacket(packet, bytes);
			break;
			
		case TYPE_POSITION:
			p = new ArmPositionPacket(packet, bytes);
			break;
		}
		return p;
	}

	

	//[ARM_PACKET_HEADER][TYPE_ONOFF][on]
	public static class ArmOnOffPacket extends Packet{
		public boolean on;

		public ArmOnOffPacket(byte[] buf, int bytes){
			super(buf, bytes);
		}
		
		public ArmOnOffPacket(){
			on = false;
		}
		
		public boolean FromByteArray(){
			if(packet == null || bytes != 3) return false;
			if(packet[0] != ARM_PACKET_HEADER || packet[1] != TYPE_ONOFF) return false;
			
			//parse packet
			if(packet[2] == 0) on = false;
			else on = true;

			return true;
		}
		
		public void ToByteArray(){
			if(packet == null || packet.length != 3){
				packet = new byte[3];
				bytes = 3;
			}
			packet[0] = ARM_PACKET_HEADER;
			packet[1] = TYPE_ONOFF;
			packet[2] = (byte)((on) ? 1 : 0);
			
		}
		
	}
	
	//[ARM_PACKET_HEADER][TYPE_GAINS][P0, P1, P2, P3][I0, I1, I2, I3][D0, D1, D2, D3][P0, P1, P2, P3][I0, I1, I2 ... for all joints
	public static class ArmGainsPacket extends Packet{
		
		private static final int length = 2+3*4*ARM_JOINTS;
		
		public float[] P;
		public float[] I;
		public float[] D;

		public ArmGainsPacket(byte[] buf, int bytes){
			super(buf, bytes);
			P = new float[ARM_JOINTS];
			I = new float[ARM_JOINTS];
			D = new float[ARM_JOINTS];
		}
		
		public boolean FromByteArray(){
			if(packet == null || bytes != length) return false;
			if(packet[0] != ARM_PACKET_HEADER || packet[1] != TYPE_GAINS) return false;
			
			//parse packet
			for(int i = 0;i<ARM_JOINTS;i++){
				P[i] = Float.intBitsToFloat(BinUtils.byteArrayToInt(packet, 2+3*4*i));
				I[i] = Float.intBitsToFloat(BinUtils.byteArrayToInt(packet, 2+3*4*i + 4));
				D[i] = Float.intBitsToFloat(BinUtils.byteArrayToInt(packet, 2+3*4*i + 8));       
			}

			return true;
		}
		
		public void ToByteArray(){
			
			if(packet == null || packet.length != length){
				packet = new byte[length];
				bytes = length;
			}
			packet[0] = ARM_PACKET_HEADER;
			packet[1] = TYPE_GAINS;
			for(int j = 0;j<ARM_JOINTS;j++){
				int p = Float.floatToIntBits(P[j]);
				BinUtils.intToByteArray(p, packet, 2+3*4*j);       
				int i = Float.floatToIntBits(I[j]);
				BinUtils.intToByteArray(i, packet, 2+3*4*j + 4);
				int d = Float.floatToIntBits(D[j]);
				BinUtils.intToByteArray(d, packet, 2+3*4*j + 8);
			}
			
		}
		
	}

	//[ARM_PACKET_HEADER][TYPE_CONTROL][Number of Channels][[chan][val0, val1, val2, val3][chan][val0....]
	public static class ArmControlPacket extends Packet{
		
		static final int header = 3;
		
		public byte Channels = -1;
		public byte[] Channel_list;
		public float[] Value_list;
		
		public ArmControlPacket(byte[] packet, int bytes){
			super(packet, bytes);
		}
		
		public ArmControlPacket(byte channels){
			super(null, 0);
			Channels = channels;
			Channel_list = new byte[Channels];
        	Value_list = new float[Channels];
		}
		
		public boolean FromByteArray(byte[] packet, int bytes)
		{
			if(packet == null || bytes < header + 5) return false;
			if(packet[0] != ARM_PACKET_HEADER || packet[1] != TYPE_CONTROL) return false;
		    try
		    {
		        Channels = packet[2];
		        if(Channel_list.length < Channels){
		        	Channel_list = new byte[Channels];
		        	Value_list = new float[Channels];
		        }
		        for(int i = 0;i<Channels;i++){
		        	Channel_list[i] = packet[header+5*i];
		        	Value_list[i] = Float.intBitsToFloat(BinUtils.byteArrayToInt(packet, header+5*i + 1));
		        }
		        return true;
		    }
		    catch(Exception e){
		    	return false;
		    }
		}
		
		public boolean ToByteArray(){
			if(packet == null || packet.length < header+5*Channels){
				packet = new byte[header+5*Channels];
			}
			
			bytes = header+5*Channels;
			packet[0] = ARM_PACKET_HEADER;
			packet[1] = TYPE_CONTROL;
			packet[1] = Channels;
			for(int i = 0;i<Channels;i++){
				packet[header+5*i] = Channel_list[i];
				
				int val = Float.floatToIntBits(Value_list[i]);
				BinUtils.intToByteArray(val, packet, header+5*i + 1);

	        }
			return true;
			
		}
	}

	//[ARM_PACKET_HEADER][TYPE_POSITION][P0, P1, P2, P3][P0, P1, P2, P3].... for all channels
	public static class ArmPositionPacket extends Packet{

		private static final int length = 2+4*ARM_JOINTS;
		
		public float[] positions;

		public ArmPositionPacket(byte[] packet, int bytes) {
			super(packet, bytes);
		}
		
		public boolean FromByteArray(){
			if(packet == null || bytes != length) return false;
			if(packet[0] != ARM_PACKET_HEADER || packet[1] != TYPE_POSITION) return false;
			
			//parse packet
			for(int i = 0;i<ARM_JOINTS;i++){
				positions[i] = Float.intBitsToFloat(BinUtils.byteArrayToInt(packet, 2+4*i));
			}

			return true;
		}
		
		public void ToByteArray(){
			
			if(packet == null || packet.length != length){
				packet = new byte[length];
				bytes = length;
			}
			packet[0] = ARM_PACKET_HEADER;
			packet[1] = TYPE_POSITION;
			for(int j = 0;j<ARM_JOINTS;j++){
				int p = Float.floatToIntBits(positions[j]);
				BinUtils.intToByteArray(p, packet, 2+4*j);       
			}
			
		}
		
		
	}
	
	
}
