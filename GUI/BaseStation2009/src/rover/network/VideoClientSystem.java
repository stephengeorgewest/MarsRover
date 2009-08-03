package rover.network;

import java.awt.Dimension;
import java.net.DatagramPacket;
import java.net.InetAddress;
import java.net.MulticastSocket;
import java.net.SocketTimeoutException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.LinkedList;
import javax.media.Buffer;
import javax.media.Codec;
import javax.media.Format;
import javax.media.format.RGBFormat;
import javax.media.format.VideoFormat;

import net.sourceforge.jffmpeg.codecs.video.mpeg4.divx.DIVXCodec;

import com.omnividea.media.codec.video.JavaDecoder;

import rover.Main;
import rover.portal.MainGUI;
import rover.video.FrameListener;

/*
 * Name: VideoClientSystem
 * Author: Travis L Brown
 * Description: 
 * This is a rover video client that is meant to be used in a static way. It is meant to
 * be the only provider of video streams to rover portals. This eliminates redundant 
 * decoding that would otherwise take place if each portal decoded its own video stream.
 * 
 * The video client system consists of a socket that reads from the video stream and feeds
 * a VideoPacketBuffer for each channel. The channel is decoded and presented to other parts
 * of the gui for use.
 * 
 */
public class VideoClientSystem implements FrameListener{

	public static final int CHANNELS = 4;
	
	public static final int STREAM_STOPPED = 0;
	public static final int STREAM_STARTED = 1;
	public static final int STREAM_PAUSED = 2;
	
	static private int bMask = 0x0000ff;
	static private int gMask = 0x00ff00;
	static private int rMask = 0xff0000;
	
	private static VideoClientSystem instance = null;
	
	private static boolean initialized = false;
	
	private Thread socketThread;
	private boolean threadstop = false;
	
	private MulticastSocket socket;
	private InetAddress mc_address;
	private int mc_port;
	private SocketInfo si;
	
	private int[] currentFrameID;
	private VideoPacketBuffer[] packetBufs;
	private LinkedList<Buffer>[] videoCache;
	private Buffer[] currentBuffers;
	private int[] streamStatus;
	private Codec[] codecs;
	private ArrayList<FrameListener>[] listeners;
	
	public static void init(){
		if(initialized == true){
			return;
		}
		initialized = true;
		
		instance = new VideoClientSystem();
	}
	
	
	public static VideoClientSystem getInstance(){
		if(instance == null) init();
		return instance;
	}
	
	public static void shutdown(){
		if(instance != null) instance.threadstop = true;
	}
	
	protected VideoClientSystem(){
		currentFrameID = new int[CHANNELS];
		packetBufs = new VideoPacketBuffer[CHANNELS];
		videoCache = new LinkedList[CHANNELS];
		listeners = new ArrayList[CHANNELS];
		streamStatus = new int[CHANNELS];
		codecs = new Codec[CHANNELS];
		currentBuffers = new Buffer[CHANNELS];
		
		for(int i = 0;i<CHANNELS;i++){
			listeners[i] = new ArrayList<FrameListener>();
			videoCache[i] = new LinkedList<Buffer>();
			packetBufs[i] = new VideoPacketBuffer();
			packetBufs[i].addFrameListener(this);
			streamStatus[i] = STREAM_STOPPED;
			codecs[i] = null;
			currentBuffers[i] = null;
			currentFrameID[i] = 0;
		}
		
		
		
		socketThread = new Thread(){	@Override public void run() {ThreadStart();}};
		socketThread.setDaemon(true);
		socketThread.start();
	}
	
	//start causes buffers to be cached and decoded, and causes video events to be sent
	public void start(int channel){
		streamStatus[channel] = STREAM_STARTED;
		if(codecs[channel] != null) codecs[channel].reset();
	}
	
	//pause stops video events and keeps track of the current location in the stream but 
	//continues to receive and cache the buffers
	public void pause(int channel){
		streamStatus[channel] = STREAM_PAUSED;
	}
	
	//stop, causes new buffers from the network to be ignored, eventually it would be nice
	//to add the ability to stop the video stream from the source, but I probably won't have
	//time to do that
	public void stop(int channel){
		streamStatus[channel] = STREAM_STOPPED;
		
	}
	
	public int getStatus(int channel){
		return streamStatus[channel];
	}
	
	
	public void registerForVideoEvents(FrameListener fl, int channel){
		System.out.println("before add Listenter list for channel " + channel + " size = " + listeners[channel].size());
		if(channel < CHANNELS) listeners[channel].add(fl);
		System.out.println("after add Listenter list for channel " + channel + " size = " + listeners[channel].size());
	}
	
	public void unregisterForVideoEvents(FrameListener fl, int channel){
		System.out.println("before remove Listenter list for channel " + channel + " size = " + listeners[channel].size());
		if(channel < CHANNELS) listeners[channel].remove(fl);
		System.out.println("after remove Listenter list for channel " + channel + " size = " + listeners[channel].size());
	}
	
	//initializes the socket
	private void initSocket(){

		try{
			mc_address = InetAddress.getByName(Main.props.getProperty("video_address"));
			mc_port = Integer.parseInt(Main.props.getProperty("video_port"));
		
			
			socket = new MulticastSocket(mc_port);
			socket.setSoTimeout(40);
			socket.joinGroup(mc_address);
			socket.setTimeToLive(10);
			
			
			si = new SocketInfo();
			si.setPortal("Video Client System");
			si.setPort(mc_port);
			si.setType("MC Rec");
			si.setAddress(mc_address);
			
			MainGUI.registerSocket(si);
		}catch(Exception e){
			e.printStackTrace();
		}
	}
	
	private void ThreadStart(){
		initSocket();
		
		byte[] buf = new byte[64000];
		while(true){
			try{
				if(threadstop) break;
				
				DatagramPacket pack = new DatagramPacket(buf,buf.length);
				
				try{socket.receive(pack); }
				catch(SocketTimeoutException ste){
					//ste.printStackTrace();
					continue;
				}
				
//				System.out.println("video packet received");
				//byte[] data = Arrays.copyOf(buf, pack.getLength());
				VideoPacket vp = new VideoPacket(buf.clone(), pack.getLength());
				boolean worked = vp.fillData();
				
				
				if(worked && vp.ChannelID < CHANNELS){
					//if(vp.FrameID >= currentFrameID[vp.ChannelID]){
						currentFrameID[vp.ChannelID] = vp.FrameID;
						packetBufs[vp.ChannelID].Process(vp);
						//System.out.println("Packet parsed");
					//}else{
					//	System.out.println("Out of order packet");
					//}	
				}
			}catch(Exception e){
				e.printStackTrace();
			}
			
		}
		
	}
	
	
	public void nextFrameReady(Buffer current_frame, int channel){
		//System.out.println("Frame receieved");
		
		switch(streamStatus[channel]){
		
		case STREAM_STOPPED:
			break;
		case STREAM_PAUSED:
			//cache the frame
			//videoCache[channel].add(current_frame);
			break;
		case STREAM_STARTED:
			//cache the frame
			//videoCache[channel].add(current_frame);
			
			if(listeners[channel].size() != 0){
				//decode the frame
				if(codecs[channel] == null)
					initStream(current_frame, channel);
				if(codecs[channel] == null) break;
				
				byte[] rawdata = (byte[]) current_frame.getData();
				//System.out.println(" header = " + Integer.toHexString(rawdata[0]) + " " + Integer.toHexString(rawdata[1]) + " " + Integer.toHexString(rawdata[2]) + " " + Integer.toHexString(rawdata[3]));
				
				int retcode = codecs[channel].process(current_frame, currentBuffers[channel]);
				if(retcode == javax.media.PlugIn.BUFFER_PROCESSED_OK){
					//System.out.println("Success");
				} else {
					//System.out.println("Failure");
					break;
				}
				
				
				for(FrameListener fl : listeners[channel]){
					fl.nextFrameReady(currentBuffers[channel], channel);
				}
			}
			break;
		}
	}
	
	private void initStream(Buffer input_buffer, int channel){
		Format f = input_buffer.getFormat();
		if(f.getEncoding().compareTo("dx50") == 0){
			//initialize new codec
			VideoFormat vf = (VideoFormat) f;
			int width = vf.getSize().width;
			int height = vf.getSize().height;
			
			RGBFormat out = new RGBFormat(new Dimension(width, height),
	                width*height, int[].class,
	                vf.getFrameRate(),
	                32,
	                rMask, gMask, bMask,
	                1, width,
	                Format.FALSE, // flipped
	                Format.NOT_SPECIFIED// bigEndian?RGBFormat.BIG_ENDIAN:RGBFormat.LITTLE_ENDIAN // endian
	                );
			
			//JavaDecoder decode = new JavaDecoder();
			DIVXCodec decode = new DIVXCodec();
			Format fin = decode.setInputFormat(input_buffer.getFormat());
			Format fout = decode.setOutputFormat(out);
			
			System.out.println(fin);
			System.out.println(fout);
			
			Buffer output_buffer = new Buffer();		
			output_buffer.setData(new int[width*height]);
			output_buffer.setLength(width*height);
			output_buffer.setFormat(fout);
			
			try{decode.open();}
			catch(Exception e){
				e.printStackTrace();
			}
			codecs[channel] = decode;
			currentBuffers[channel] = output_buffer;
			
		} else{
			codecs[channel] = null;
			currentBuffers[channel] = null;
		}
		return;
	}
	
}
