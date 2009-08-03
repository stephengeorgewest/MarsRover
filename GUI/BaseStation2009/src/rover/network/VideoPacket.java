package rover.network;
import rover.utils.BinUtils;

/*
 * Name: VideoPacket
 * Author: Travis L Brown
 * Description: 
 * Represents a packet designed to carry video. Provides packet header generation
 * and parsing capability. The packet header carries all the format information so 
 * the raw data can be placed directly in the packet starting at HEADER_LENGTH. 
 * 
 * This packet format also supports packet buffering, see VideoPacketBuffer
 * 
 */
public class VideoPacket extends Packet{
	public static int HEADER_LENGTH = 32;
	
	public static byte VIDEO_HEADER_MAGIC = 12;
	public byte ChannelID = -1;          //The channel this packet belongs to
	public int FrameID = -1;             //The frame this packet belongs to
	public byte PacketNumber = -1;       //The index of the current packet eg packet n of 5
	public byte TotalPackets = -1;       //The total number of packets required to make this frame
	public int PacketPayload = -1;      //The size of the payload of this packet in bytes
	public int FrameSize = -1;          //The size of the total frame payload in bytes
	public char[] fourcc = new char[4]; //The video format, expected to conform to well known standard id
	public int Width = -1;               //The uncompressed width of the video
	public int Height = -1;              //The uncompressed height of the video
	public byte fps = -1;                //The intended fps of the video stream
	public byte pad1 = -1;               //Extra space for later use, alligns packet on 32bit boundary
	public byte pad2 = -1;               //Extra space for later use, alligns packet on 32bit boundary
	public byte pad3 = -1;               //Extra space for later use, alligns packet on 32bit boundary
	
	
	public VideoPacket(byte[] packet, int bytes)
	{
		super(packet, bytes);
	    FromByteArray(packet, bytes);
	}
	
	public VideoPacket(Packet p){
		super(p.packet, p.bytes);
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
		if(bytes < HEADER_LENGTH) return false;
	    try
	    {
	    	if(packet[0] != VIDEO_HEADER_MAGIC) return false;
	    	ChannelID = packet[1];
	        FrameID = BinUtils.byteArrayToInt(packet, 2);
	        PacketNumber = packet[6];
	        TotalPackets = packet[7];
	        PacketPayload = BinUtils.byteArrayToInt(packet, 8);
	        FrameSize = BinUtils.byteArrayToInt(packet, 12);
	        fourcc[0] = (char)packet[16];
	        fourcc[1] = (char)packet[17];
	        fourcc[2] = (char)packet[18];
	        fourcc[3] = (char)packet[19];  
	        Width = BinUtils.byteArrayToInt(packet, 20);
	        Height = BinUtils.byteArrayToInt(packet, 24);
	        fps = packet[28];
	        pad1 = packet[29];
	        pad2 = packet[30];
	        pad3 = packet[31];
	    }
	    catch (Exception e)
	    {
	        return false;
	    }
	    return true;
	}

	public boolean ToByteArray(){
		if(bytes < HEADER_LENGTH) return false;
	    try
	    {
	    	packet[0] = VIDEO_HEADER_MAGIC;
	    	packet[1] = ChannelID;
	        BinUtils.intToByteArray(FrameID, packet, 2);
	        packet[6] = PacketNumber;
	        packet[7] = TotalPackets;
	        BinUtils.intToByteArray(PacketPayload, packet, 8);
	        BinUtils.intToByteArray(FrameSize, packet, 12);
	        packet[16] = (byte)fourcc[0];
	        packet[17] = (byte)fourcc[1];
	        packet[18] = (byte)fourcc[2];
	        packet[19] = (byte)fourcc[3]; 
	        BinUtils.intToByteArray(Width, packet, 20);
	        BinUtils.intToByteArray(Height, packet, 24);
	        packet[28] = fps;
	        packet[29] = pad1;
	        packet[30] = pad2;
	        packet[31] = pad3;
	    }
	    catch (Exception e)
	    {
	        return false;
	    }
	    return true;
	}
	
	public String toString(){
		StringBuffer sb = new StringBuffer();
		sb.append("ChannelID = " + ChannelID);
		sb.append(", FrameID = " + FrameID);
		sb.append(", PacketNumber = " + PacketNumber);
		sb.append(", TotalPackets = " + TotalPackets);
		sb.append(", PacketPayload = " + PacketPayload);
		sb.append(", FrameSize = " + FrameSize);
		sb.append(", fourcc = " + new String(fourcc));
		sb.append(", " + Width + " " + Height);
		return sb.toString();
	}
	
}

