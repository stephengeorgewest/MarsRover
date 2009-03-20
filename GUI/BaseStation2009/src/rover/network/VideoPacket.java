package rover.network;


public class VideoPacket extends Packet{
	public static int DATA_OFFSET = 10;
	
	public static int VIDEO_HEADER = 12;
	
	public int Height;
	public int Width;
	public int FrameID;
	public int ChannelID;
	public int PacketNumber;
	public int TotalPackets;
	
	
	public VideoPacket(byte[] packet, int bytes)
	{
		super(packet, bytes);
	    this.Height = -1;
	    this.Width = -1;
	    this.FrameID = -1;
	    this.ChannelID = -1;
	    this.PacketNumber = -1;
	    this.TotalPackets = -1;
	    FromByteArray(packet, bytes);
	}
	
	public VideoPacket(Packet p){
		super(p.packet, p.bytes);
		this.Height = -1;
	    this.Width = -1;
	    this.FrameID = -1;
	    this.ChannelID = -1;
	    this.PacketNumber = -1;
	    this.TotalPackets = -1;
	}
	
	public void setPacket(byte[] packet, int bytes){
		this.packet = packet;
		this.bytes = bytes;
	}
	
	public boolean fillData(){
		return FromByteArray(packet, bytes);
	}
	
	public boolean FromByteArray(byte[] packet, int bytes)
	{
		if(bytes < 9) return false;
	    try
	    {
	    	if(packet[0] != VIDEO_HEADER) return false;
	        ChannelID = packet[1];
	
	        Width = 0;
	        Width |= (packet[2] << 8);
	        Width |= packet[3];
	
	        Height = 0;
	        Height |= (packet[4] << 8);
	        Height |= packet[5];
	
	        FrameID = 0;
	        FrameID |= packet[7];
	        FrameID |= (packet[6] << 8);
	        
	        PacketNumber = packet[8];
	        TotalPackets = packet[9];
	
	    }
	    catch (Exception e)
	    {
	        return false;
	    }
	    return true;
	}

}
