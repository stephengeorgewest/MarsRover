package rover;

public class VideoPacket extends Packet{
	public static int DATA_OFFSET = 10;
	
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
	}
	
	public boolean fillData()
	{
	    try
	    {
	        ChannelID = packet[1];
	
	        PacketNumber = packet[8];
	        TotalPackets = packet[9];
	
	        FrameID = 0;
	        FrameID |= packet[7];
	        FrameID |= (packet[6] << 8);
	
	        Width = 0;
	        Width |= (packet[2] << 8);
	        Width |= packet[3];
	
	        Height = 0;
	        Height |= (packet[4] << 8);
	        Height |= packet[5];
	
	    }
	    catch (Exception e)
	    {
	        return false;
	    }
	    return true;
	}

}
