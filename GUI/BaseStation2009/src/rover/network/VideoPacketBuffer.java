package rover.network;
import java.awt.Dimension;
import java.util.ArrayList;
import javax.media.Buffer;
import javax.media.Format;
import javax.media.format.VideoFormat;
import rover.video.FrameListener;

/*
 * Name: Video Packet Buffer 
 * Author: Travis L Brown
 * Description: 
 * Converts a stream of video packets into a stream of frames. It's basic function is to 
 * wait until all packets for a given frame have been received, knit these packets together
 * to form 1 complete frame and then raise an event to notify others that a new frame is available.
 * 
 * If a packet from a newer frame is received before all the packets from the previous frame have 
 * come in, the packet buffer will drop that frame and start assembling the next one.
 * 
 * This class is really more useful for uncompressed or lightly compressed video formats 
 * such as RGB or YUV where fitting a whole frame on one packet is impossible. It is 
 * recommended that frames be transfered on a single packet if possible. 
 * 
 * The theoretical max size for a UDP packet is 64 KBytes although many systems don't support 
 * packets that large.
 * 
 * 
 */
public class VideoPacketBuffer
{
    int currentPacketCount;
    int currentFrameID;
    VideoPacket[] buffer;

    public Buffer current_frame; 
    Format current_format;
    
    ArrayList<FrameListener> listeners = null;
    
    
    public VideoPacketBuffer()
    {
        currentPacketCount = -1;
        currentFrameID = -1;
        current_frame = null;
    }

    private void resetState(){
        currentFrameID = -1;
        for (int i = 0; i < currentPacketCount; i++)
        {
            buffer[i] = null;
        }
    }

    public void addFrameListener(FrameListener fl){
    	if(listeners == null){
    		listeners = new ArrayList<FrameListener>();
    	}
    	listeners.add(fl);
    }
    
    /*This is the basic function that is called each time a new packet is received, 
     * it contains the main buffering logic of the Video Packet Buffer 
     */
    public void Process(VideoPacket p)
    {
        p.fillData();
        
        //if we've changed to a different number of packets/frame reset the state
        if(currentPacketCount != p.TotalPackets){
            //change the capacity of the buffer and update the format
            buffer = new VideoPacket[p.TotalPackets];
            currentPacketCount = p.TotalPackets;
            current_format = new VideoFormat(new String(p.fourcc), new Dimension(p.Width, p.Height), p.Width*p.Height, (new byte[0]).getClass(), p.fps);
            
            resetState();
        } else if (p.FrameID != currentFrameID){
        	//we've received an out of order packet
            resetState();
        }

        currentFrameID = p.FrameID;

        buffer[p.PacketNumber-1] = p;

        //System.out.println("Received " + p.PacketNumber + "/" + currentPacketCount);

        boolean allPacketsReceived = true;
        for (int i = 0; i < currentPacketCount; i++)
        {
            if(buffer[i] == null) allPacketsReceived = false;
        }

        if (allPacketsReceived){
        	MergePackets();
        	nextFrameReady();
        	resetState();
        }
        

    }

    private void MergePackets(){
    	
    	Buffer nextFrame = new Buffer();
    	nextFrame.setFormat(current_format);
    	nextFrame.setLength(buffer[0].FrameSize);
    	nextFrame.setSequenceNumber(buffer[0].FrameID);
    	
    	//System.out.println(nextFrame.getLength() + " " + buffer[0].packet.length);
    	byte[] data = new byte[buffer[0].FrameSize];

        int data_index = 0;
        for (int i = 0; i < buffer[0].TotalPackets; i++)
        {
//        	System.out.println(buffer[i].toString());
//        	System.out.println("packetlength = " + buffer[i].bytes + " payload = " + buffer[i].PacketPayload);
//        	System.out.println("total frame data = " + buffer[i].FrameSize);
//            System.out.println("merging packet " + i + " " + buffer[i].PacketPayload + " bytes");
            int count = buffer[i].PacketPayload;
//            System.out.println("data_index = " + data_index + " count = " + count);
//            System.out.println(buffer[i].packet[VideoPacket.HEADER_LENGTH] + " " 
//            					+ buffer[i].packet[VideoPacket.HEADER_LENGTH+1] + " "
//            					+ buffer[i].packet[VideoPacket.HEADER_LENGTH+2] + " "
//            					+ buffer[i].packet[VideoPacket.HEADER_LENGTH+3]);
            System.arraycopy(buffer[i].packet, VideoPacket.HEADER_LENGTH, data, data_index, count);
            //System.out.println(" header = " + Integer.toHexString(data[0]) + " " + Integer.toHexString(data[1]) + " " + Integer.toHexString(data[2]) + " " + Integer.toHexString(data[3]));
            data_index += count;
        }
        nextFrame.setData(data);
        current_frame = nextFrame;
    }

    private void nextFrameReady(){
    	if(listeners != null)
	    	for(FrameListener fl : listeners){
	    		fl.nextFrameReady(current_frame, buffer[0].ChannelID);
	    	}
    }
    
}