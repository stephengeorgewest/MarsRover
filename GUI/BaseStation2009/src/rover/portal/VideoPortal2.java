package rover.portal;

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.DisplayMode;
import java.awt.Frame;
import java.awt.Graphics;
import java.awt.GraphicsDevice;
import java.awt.GraphicsEnvironment;

import javax.imageio.ImageIO;
import javax.swing.JPanel;
import javax.swing.JFrame;
import java.awt.Dimension;
import javax.swing.BoxLayout;
import java.awt.GridBagLayout;
import java.awt.Rectangle;
import javax.swing.JLabel;
import java.awt.Point;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.im.InputContext;
import java.awt.image.BufferedImage;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.net.DatagramPacket;
import java.net.InetAddress;
import java.net.MulticastSocket;
import java.net.URL;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.Date;
import javax.swing.JButton;

import rover.Main;
import rover.guistuff.ImagePanel;
import rover.network.MulticastListener;
import rover.network.Packet;
import rover.network.PacketHandler;
import rover.network.SocketInfo;
import rover.network.VideoClientSystem;
import rover.network.VideoPacket;
import rover.network.VideoPacketBuffer;
import rover.utils.Stopwatch;
import rover.video.Avi_Chunk;
import rover.video.FrameListener;
import rover.video.Java2DRenderer;

import javax.media.Buffer;
import javax.media.Codec;
import javax.media.Controls;
import javax.media.Format;
import javax.media.Manager;
import javax.media.MediaLocator;
import javax.media.Player;
import javax.media.PlugIn;
import javax.media.Time;
import javax.media.Track;
import javax.media.bean.playerbean.MediaPlayer;
import javax.media.format.RGBFormat;
import javax.media.format.VideoFormat;

//import com.omnividea.media.parser.video.Parser;
//import com.omnividea.media.protocol.file.DataSource;
//import com.omnividea.media.renderer.video.Java2DRenderer;
import javax.swing.ImageIcon;
import java.awt.Color;
import javax.swing.JRadioButton;
import javax.swing.JToggleButton;
import javax.swing.JComboBox;

import net.sourceforge.jffmpeg.codecs.video.mpeg4.divx.DIVXCodec;

public class VideoPortal2 extends Portal implements FrameListener, KeyListener {

	private static final long serialVersionUID = 1L;
	private static DecimalFormat df = new DecimalFormat("##.00");
	private JPanel jContentPane = null;
	private JPanel displayPanel = null;
	private JLabel resLabel = null;
	private JLabel fpsLabel = null;
	private Component video = null;
	BufferedImage cur_image = null;  //  @jve:decl-index=0:
	Stopwatch fps_watch;
	
	static private int bMask = 0x0000ff;
	static private int gMask = 0x00ff00;
	static private int rMask = 0xff0000;
	
	Thread videoThread;
	
	Stopwatch sw = new Stopwatch();  //  @jve:decl-index=0:
	int frameCount = 0;
	
	int current_channel = 0;
	int width_old;
	int height_old;
	int x_old;
	int y_old;

	boolean fsmode = false;
	//InetAddress mcgroup = null;
	//int mcport = 0;
	
	VideoPacket test1, test2, test3;  //  @jve:decl-index=0:
	//private MediaPlayer mediaPlayer = null;
	private Java2DRenderer java2DRenderer = null;
	private JToggleButton playToggleButton = null;
	private JToggleButton pauseToggleButton = null;
	private JToggleButton stopToggleButton = null;
	private JComboBox channelComboBox = null;
	private JButton jButton = null;
	private JButton fsButton = null;
	/**
	 * This is the default constructor
	 */
	public VideoPortal2(Frame owner) {
		super(owner);
		initialize();
		VideoClientSystem.init();
		this.setFocusable(true);
		this.addKeyListener(this);
	}
	
	@Override
	public String toString(){
		return this.getTitle();
	}
	
	public static class VideoPortalInit extends PortalInit {
		@Override
		public Portal createPortal() {
			return new VideoPortal2(MainGUI.instance);
		}
		
		@Override
		public String toString(){
			return "Video Portal";
		}
	}
	
	
	
	/**
	 * This method initializes this
	 * 
	 * @return void
	 */
	private void initialize() {
		this.setSize(502, 393);
		this.setContentPane(getJContentPane());
		this.setTitle("Video");
		switchChannels(current_channel);	
	}

//	private void playTestVid(){
//		try{
//			File f = new File("./test.avi");
//			MediaLocator ml = new MediaLocator(f.toURL());
//			DataSource ds = new DataSource();
//			ds.setLocator(ml);
//			Parser p = new Parser();
//			p.setSource(ds);
//			System.out.println("DataSource set");
//			Time dur = p.getDuration();
//			System.out.println(dur.getSeconds());
//			Track[] tracks = p.getTracks();
//			
//			long length = 1000000;
//			byte[] outdata = new byte[(int)length];
//			//ByteBuffer outdata = ByteBuffer.allocateDirect((int)length);
//			long bytes = 0;
//			//p.getNextFrame(outdata, bytes, length);
//			//System.out.println("bytes= " + bytes);
//			System.out.println("# of Tracks = " + tracks.length);
//			//VideoFormat in = new VideoFormat("dx50", new Dimension(704, 480), 30000, outdata.getClass(), 20);
//			Format in = tracks[0].getFormat();
//			System.out.println(in.getEncoding());
//			
//			
////			RGBFormat out = new RGBFormat(new Dimension(704, 480), 1500000, ByteBuffer.class, 20f, 32, 50,50,50, 1, 704*3, Format.FALSE, RGBFormat.LITTLE_ENDIAN);
//			RGBFormat out = new RGBFormat(new Dimension(640, 480), 1500000, ByteBuffer.class, 20f, 24, rMask, gMask, bMask, 1, 640*3, Format.FALSE, RGBFormat.LITTLE_ENDIAN);
//			
//			NativeDecoder decode = new NativeDecoder();
//			decode.setInputFormat(in);
//			decode.setOutputFormat(out);
//			
//			Buffer a = new Buffer();
//			Buffer b = new Buffer();
//			
//			b.setFormat(out);
//			b.setLength(640*480*3);
//			int count = 0;
//			//System.out.println("nextoffset = " + nextoffset);
//			//in.getSize().setSize(nextoffset, 0);
//			//in.getSize().setSize(30,50);
//			
//			while(true){
//				
//				tracks[0].readFrame(a);
//				System.out.println("Frame read");
//				//a.setFormat(out);
//				int retcode = decode.process(a, b);
//				/*
//				switch (retcode){
//				case javax.media.PlugIn.BUFFER_PROCESSED_FAILED:
//					System.out.println("Buffer processed failed");
//					break;
//				case javax.media.PlugIn.BUFFER_PROCESSED_OK:
//					System.out.println("Buffer processed ok");
//					break;
//				case javax.media.PlugIn.INPUT_BUFFER_NOT_CONSUMED:
//					System.out.println("Input buffer not consumed");
//					break;
//				case javax.media.PlugIn.OUTPUT_BUFFER_NOT_FILLED:
//					System.out.println("output buffer not filled");
//					break;
//				case javax.media.PlugIn.PLUGIN_TERMINATED:
//					System.out.println("plugin terminated");
//					break;
//				}*/
//				//System.out.println("Done");
//				
//				//System.out.println("class = " + b.getData().getClass());
//				//ByteBuffer data = (ByteBuffer)b.getData();
//				//System.out.println("data.capacity = " + data.capacity());
//				/*
//				int[] data = (int[])b.getData();
//				System.out.println("data.length = " + data.length);
//				//System.out.println("b.length = " + b.getLength());
//				System.out.println("b.sequencenumber" + b.getSequenceNumber());
//				System.out.println("b.offset" + b.getOffset());
//				System.out.println("b.duration" + b.getDuration());
//				*/
//				getJava2DRenderer().process(b);
//			}
//		}catch (Exception e){
//			e.printStackTrace();
//		}
//		
//	}
	
	private void toggleFullscreen(){
		if(fsmode == true){
			exitFullscreen();
		} else {
			gotoFullscreen();
		}
	}
	
	private void gotoFullscreen(){
		if(fsmode == true) return;
		GraphicsEnvironment ge = GraphicsEnvironment.getLocalGraphicsEnvironment();
		GraphicsDevice gd = ge.getDefaultScreenDevice();
		DisplayMode[] dms = gd.getDisplayModes();
//		for(DisplayMode mode : dms){
//			System.out.println(mode.getWidth() + " " + mode.getHeight() + " " + mode.getRefreshRate() + " hz");
//		}
		
		DisplayMode mode = gd.getDisplayMode();
		
		
		System.out.println(ge);
		System.out.println(gd);
		
//		int width = 640;
//		int height = 480;
		int width = mode.getWidth();
		int height = mode.getHeight();
		
		width_old = this.getSize().width;
		height_old = this.getSize().height;
		x_old = this.getLocation().x;
		y_old = this.getLocation().y;
		
		
		
		this.removeNotify();
		this.setUndecorated(true);
		this.addNotify();
		
		this.setLocation(0,0);
		this.setSize(width, height);
		java2DRenderer.setSize(width, height);

		this.enableInputMethods(true);
		this.setFocusable(true);
		this.setEnabled(true);
		this.setFocusableWindowState(true);
		this.setVisible(true);
		
		InputContext ic = this.getInputContext();
		System.out.println("ic = " + ic);
		
		
		
		fsmode = true;
		
	}
	
	private void exitFullscreen(){
		if(fsmode == false) return;
		
		this.removeNotify();
		this.setUndecorated(false);
		this.addNotify();

		
		this.setLocation(x_old, y_old);
		this.setSize(width_old, height_old);
		
		fsmode = false;
		
	}
	
	private void switchChannels(int new_channel){
		VideoClientSystem.getInstance().unregisterForVideoEvents(this, current_channel);
		VideoClientSystem.getInstance().registerForVideoEvents(this, new_channel);
		current_channel = new_channel;
		setToggleButtons();
	}
	
	private void setToggleButtons(){
		int status = VideoClientSystem.getInstance().getStatus(current_channel);
		
		playToggleButton.setSelected(false);
		stopToggleButton.setSelected(false);
		pauseToggleButton.setSelected(false);
		
		switch(status){
		case VideoClientSystem.STREAM_STOPPED:
			stopToggleButton.setSelected(true);
			break;
		case VideoClientSystem.STREAM_PAUSED:
			pauseToggleButton.setSelected(true);
			break;
		case VideoClientSystem.STREAM_STARTED:
			playToggleButton.setSelected(true);
			break;
		}
		
	}
	
	private void playStream(){
		int length = 0;
		ByteBuffer bbuf = null;
		FileInputStream fis;
		byte[] rawdata = null;
		try{
			File file = new File("./stream2.avi");
			fis = new FileInputStream(file);
			FileChannel chan = fis.getChannel();
			length = (int)chan.size();
			rawdata = new byte[length];
			System.out.println("File size = " + chan.size());
			bbuf = chan.map(FileChannel.MapMode.READ_ONLY, 0, chan.size());
			bbuf.get(rawdata);
			chan.close();
			fis.close();
		
		}catch(Exception e){
			e.printStackTrace();
		}
		
		
		
		int index = 0;
	
		ArrayList<Avi_Chunk> chunks = new ArrayList<Avi_Chunk>();

		
		
		for(int i = 0;i<rawdata.length;){
			Avi_Chunk c = new Avi_Chunk();
			c.address = i;
			c.length = 0;
			c.length |= (rawdata[i] & 0x000000FF);
			c.length |= (rawdata[i + 1] & 0x000000FF) << 8;
			c.length |= (rawdata[i + 2] & 0x000000FF) << 16;
			c.length |= (rawdata[i + 3] & 0x000000FF) << 24;
				//if(chunk_length %2 ==1) chunk_length++;
			chunks.add(c);
				System.out.println("chunk of length "+ c.length 
						+ " found at " + Integer.toHexString(i) + " next = " + Integer.toHexString(i+4+c.length)
						+ " header = " + Integer.toHexString(rawdata[i+4]) + " " + Integer.toHexString(rawdata[i+5]) + " " + Integer.toHexString(rawdata[i+6]) + " " + Integer.toHexString(rawdata[i+7]));
						//(char)rawdata[i+8] + " " + (char)rawdata[i+9] + " " + (char)rawdata[i+10] + " " + (char)rawdata[i+11]);
			i+=c.length + 4;
		}
		
		System.out.println("# of chunks = " + chunks.size());
		
		VideoFormat in = new VideoFormat("dx50", new Dimension(704, 480), 30000, rawdata.getClass(), 20);

		//RGBFormat out = new RGBFormat(new Dimension(640, 480), 640*480*3, byte[].class, 20f, 24, rMask, gMask, bMask);
		RGBFormat out = new RGBFormat(new Dimension(704, 480),
                704*480, int[].class,
                20,
                32,
                rMask, gMask, bMask,
                1, 704,
                Format.FALSE, // flipped
                Format.NOT_SPECIFIED// bigEndian?RGBFormat.BIG_ENDIAN:RGBFormat.LITTLE_ENDIAN // endian
                );
		
		//JavaDecoder decode = new JavaDecoder();
		DIVXCodec decode = new DIVXCodec();
		
		Format fin = decode.setInputFormat(in);
		Format fout = decode.setOutputFormat(out);
		try{decode.open();}
		catch(Exception e){
			e.printStackTrace();
		}
		
		System.out.println(fin);
		System.out.println(fout);
		
		Buffer a = new Buffer();
		a.setFormat(in);
		a.setData(rawdata);
		
		Buffer b = new Buffer();		
		b.setData(new int[704*480]);
		b.setLength(704*480);
		b.setFormat(fout);
		
		
		
		
		System.out.println("Start Decoding");
		
		int count = 0;
		index = 0;
		int i = 0;
		for(i = 0;i<chunks.size();i++){
			
			//System.out.println("nextoffset = " + nextoffset);
			//in.getSize().setSize(nextoffset, 0);
			//in.getSize().setSize(30,50);
			
			
			Avi_Chunk c = chunks.get(i);
			index = c.address;
			System.out.print("index = " + Integer.toHexString(index) + " ");
			
			a.setLength(c.length);
			a.setOffset(c.address+4);
			a.setSequenceNumber(i);
			//a.setDuration(1l);
			int retcode = decode.process(a, b);
			
			if(retcode == javax.media.PlugIn.BUFFER_PROCESSED_OK){
				System.out.println("Success");
				//break;
			} else {
				System.out.println("Failure");
			}
			
			/*
			switch (retcode){
			case javax.media.PlugIn.BUFFER_PROCESSED_FAILED:
				System.out.println("Buffer processed failed");
				break;
			case javax.media.PlugIn.BUFFER_PROCESSED_OK:
				System.out.println("Buffer processed ok");
				break;
			case javax.media.PlugIn.INPUT_BUFFER_NOT_CONSUMED:
				System.out.println("Input buffer not consumed");
				break;
			case javax.media.PlugIn.OUTPUT_BUFFER_NOT_FILLED:
				System.out.println("output buffer not filled");
				break;
			case javax.media.PlugIn.PLUGIN_TERMINATED:
				System.out.println("plugin terminated");
				break;
			}*/
			//System.out.println("Done");
			
			//System.out.println("class = " + b.getData().getClass());
			//ByteBuffer data = (ByteBuffer)b.getData();
			//System.out.println("data.capacity = " + data.capacity());
			
			int[] data = (int[])b.getData();
			//System.out.println("data.length = " + data.length);
			//System.out.println("b.length = " + b.getLength());
			//System.out.println("b.sequencenumber" + b.getSequenceNumber());
			//System.out.println("b.offset" + b.getOffset());
			//System.out.println("b.duration" + b.getDuration());
			
			
			getJava2DRenderer().process(b);
			try{Thread.sleep(50);}
			catch(Exception e){e.printStackTrace();}
			/*
			count++;
			index += nextoffset + 4;
			if(index >= length) break;
			nextoffset = 0;
//			nextoffset |= (bbuf.get(index) & 0x000000FF);
//			nextoffset |= (bbuf.get(index + 1) & 0x000000FF) << 8;
//			nextoffset |= (bbuf.get(index + 2) & 0x000000FF) << 16;
//			nextoffset |= (bbuf.get(index + 3) & 0x000000FF) << 24;
			nextoffset |= (rawdata[index - 4] & 0x000000FF);
			nextoffset |= (rawdata[index - 3] & 0x000000FF) << 8;
			nextoffset |= (rawdata[index - 2] & 0x000000FF) << 16;
			nextoffset |= (rawdata[index - 1] & 0x000000FF) << 24;
			if(nextoffset %2 ==1) nextoffset++;
			System.out.println(Integer.toHexString(index) + " " + Integer.toHexString(nextoffset) + ", Following bytes = " + Integer.toHexString(rawdata[index+4]) + " " + Integer.toHexString(rawdata[index+5]) + " " + Integer.toHexString(rawdata[index+6]) + " " + Integer.toHexString(rawdata[index+7]));
			*/
			//index+=1;
			//System.out.println("Index = " + index);
			if(index >= length) break;
		}
		decode.reset();
		decode.close();
		System.out.println("Decode complete #frames= " + i);
	}
	
	
//	private void playStream2(){
//		int length = 0;
//		ByteBuffer bbuf = null;
//		FileInputStream fis;
//		byte[] rawdata = null;
//		try{
//			File file = new File("./test.avi");
//			fis = new FileInputStream(file);
//			FileChannel chan = fis.getChannel();
//			length = (int)chan.size();
//			rawdata = new byte[length];
//			System.out.println("File size = " + chan.size());
//			bbuf = chan.map(FileChannel.MapMode.READ_ONLY, 0, chan.size());
//			bbuf.get(rawdata);
//			chan.close();
//			fis.close();
//		
//		}catch(Exception e){
//			e.printStackTrace();
//		}
//		
//		
//		
//		int index = 0;
//		int nextoffset = 0;
//		
////		for(int i = 1;i<rawdata.length;i++){
//			//System.out.println(rawdata[i-1] + " " + rawdata[i]);
////			if(rawdata[i] == (byte)0x00 
////					&& rawdata[i] == (byte)0x00
////					&& rawdata[i+2] == (byte)0x01
////					&& rawdata[i+3] == (byte)0xb0){
////				index = i;
////				System.out.println("First index found at " + index);
////				break;
////			if(rawdata[i] == (byte)0x30 
////					&& rawdata[i] == (byte)0x30
////					&& rawdata[i+2] == (byte)0x64
////					&& rawdata[i+3] == (byte)0x63){
////				index = i;
////				System.out.println("First 00dc index found at " + index);
////				break;
////			}
////		}
//	
//		ArrayList<Avi_Chunk> chunks = new ArrayList<Avi_Chunk>();
//
//		for(int i = 1;i<rawdata.length;i++){
//			//System.out.println(rawdata[i-1] + " " + rawdata[i]);
////			if(rawdata[i-1] == (byte)0x01 && (rawdata[i] == (byte)0xb6 || rawdata[i] == (byte)0xb0)){
////				//System.out.println("index found at " + i);
////				chunks++;
//			
//			if(rawdata[i] == (byte)0x30 
//					&& rawdata[i] == (byte)0x30
//					&& rawdata[i+2] == (byte)0x64
//					&& rawdata[i+3] == (byte)0x63){
//				int chunk_length = 0;
//				chunk_length |= (rawdata[i + 4] & 0x000000FF);
//				chunk_length |= (rawdata[i + 5] & 0x000000FF) << 8;
//				chunk_length |= (rawdata[i + 6] & 0x000000FF) << 16;
//				chunk_length |= (rawdata[i + 7] & 0x000000FF) << 24;
//				if(chunk_length %2 ==1) chunk_length++;
//				Avi_Chunk c = new Avi_Chunk();
//				c.address = i;
//				c.length = chunk_length;
//				chunks.add(c);
//				System.out.println("00dc of length "+ chunk_length 
//						+ " found at " + Integer.toHexString(i) + " next = " + Integer.toHexString(i+8+chunk_length)
//						+ " header = " + Integer.toHexString(rawdata[i+8]) + " " + Integer.toHexString(rawdata[i+9]) + " " + Integer.toHexString(rawdata[i+10]) + " " + Integer.toHexString(rawdata[i+11]));
//						//(char)rawdata[i+8] + " " + (char)rawdata[i+9] + " " + (char)rawdata[i+10] + " " + (char)rawdata[i+11]);
//				if(rawdata[i+8] == (byte)0x69 
//						&& rawdata[i+9] == (byte)0x64
//						&& rawdata[i+10] == (byte)0x78
//						&& rawdata[i+11] == (byte)0x31) break;
//			}
//		}
//		
//		System.out.println("# of chunks = " + chunks.size());
//		
////		nextoffset |= (bbuf.get(0) & 0x000000FF);
////		nextoffset |= (bbuf.get(1) & 0x000000FF) << 8;
////		nextoffset |= (bbuf.get(2) & 0x000000FF) << 16;
////		nextoffset |= (bbuf.get(3) & 0x000000FF) << 24;
////		nextoffset |= (rawdata[index-4] & 0x000000FF);
////		nextoffset |= (rawdata[index-3] & 0x000000FF) << 8;
////		nextoffset |= (rawdata[index-2] & 0x000000FF) << 16;
////		nextoffset |= (rawdata[index-1] & 0x000000FF) << 24;
////		if(nextoffset %2 ==1) nextoffset++;
////		System.out.println(nextoffset + ", Following bytes = " + Integer.toHexString(rawdata[index+4]) + " " + Integer.toHexString(rawdata[index+5]) + " " + Integer.toHexString(rawdata[index+6]) + " " + Integer.toHexString(rawdata[index+7]));
//		
//		VideoFormat in = new VideoFormat("dx50", new Dimension(640, 480), 3998, rawdata.getClass(), 20);
//
//		//RGBFormat out = new RGBFormat(new Dimension(640, 480), 640*480*3, byte[].class, 20f, 24, rMask, gMask, bMask);
//		RGBFormat out = new RGBFormat(new Dimension(640, 480),
//                640*480, int[].class,
//                20,
//                32,
//                rMask, gMask, bMask,
//                1, 640,
//                Format.FALSE, // flipped
//                Format.NOT_SPECIFIED// bigEndian?RGBFormat.BIG_ENDIAN:RGBFormat.LITTLE_ENDIAN // endian
//                );
//		
//		JavaDecoder decode = new JavaDecoder();
//		Format fin = decode.setInputFormat(in);
//		Format fout = decode.setOutputFormat(out);
//		try{decode.open();}
//		catch(Exception e){
//			e.printStackTrace();
//		}
//
//		System.out.println(fin);
//		System.out.println(fout);
//		
//		Buffer a = new Buffer();
//		a.setFormat(in);
//		a.setData(rawdata);
//		
//		Buffer b = new Buffer();		
//		b.setData(new int[640*480]);
//		b.setLength(640*480);
//		b.setFormat(fout);
//		
//		
//		
//		
//		int count = 0;
//		index = 0;
//		int i = 0;
//		for(i = 0;i<chunks.size();i++){
//			
//			//System.out.println("nextoffset = " + nextoffset);
//			//in.getSize().setSize(nextoffset, 0);
//			//in.getSize().setSize(30,50);
//			
//			
//			Avi_Chunk c = chunks.get(i);
//			index = c.address;
//			System.out.print("index = " + Integer.toHexString(index) + " ");
//			
//			a.setLength(c.length);
//			a.setOffset(c.address+8);
//			a.setSequenceNumber(i);
//			//a.setDuration(1l);
//			int retcode = decode.process(a, b);
//			
//			if(retcode == javax.media.PlugIn.BUFFER_PROCESSED_OK){
//				System.out.println("Success");
//				//break;
//			} else {
//				System.out.println("Failure");
//			}
//			
//			/*
//			switch (retcode){
//			case javax.media.PlugIn.BUFFER_PROCESSED_FAILED:
//				System.out.println("Buffer processed failed");
//				break;
//			case javax.media.PlugIn.BUFFER_PROCESSED_OK:
//				System.out.println("Buffer processed ok");
//				break;
//			case javax.media.PlugIn.INPUT_BUFFER_NOT_CONSUMED:
//				System.out.println("Input buffer not consumed");
//				break;
//			case javax.media.PlugIn.OUTPUT_BUFFER_NOT_FILLED:
//				System.out.println("output buffer not filled");
//				break;
//			case javax.media.PlugIn.PLUGIN_TERMINATED:
//				System.out.println("plugin terminated");
//				break;
//			}*/
//			//System.out.println("Done");
//			
//			//System.out.println("class = " + b.getData().getClass());
//			//ByteBuffer data = (ByteBuffer)b.getData();
//			//System.out.println("data.capacity = " + data.capacity());
//			
//			int[] data = (int[])b.getData();
//			//System.out.println("data.length = " + data.length);
//			//System.out.println("b.length = " + b.getLength());
//			//System.out.println("b.sequencenumber" + b.getSequenceNumber());
//			//System.out.println("b.offset" + b.getOffset());
//			//System.out.println("b.duration" + b.getDuration());
//			
//			
//			getJava2DRenderer().process(b);
//			try{Thread.sleep(50);}
//			catch(Exception e){e.printStackTrace();}
//			/*
//			count++;
//			index += nextoffset + 4;
//			if(index >= length) break;
//			nextoffset = 0;
////			nextoffset |= (bbuf.get(index) & 0x000000FF);
////			nextoffset |= (bbuf.get(index + 1) & 0x000000FF) << 8;
////			nextoffset |= (bbuf.get(index + 2) & 0x000000FF) << 16;
////			nextoffset |= (bbuf.get(index + 3) & 0x000000FF) << 24;
//			nextoffset |= (rawdata[index - 4] & 0x000000FF);
//			nextoffset |= (rawdata[index - 3] & 0x000000FF) << 8;
//			nextoffset |= (rawdata[index - 2] & 0x000000FF) << 16;
//			nextoffset |= (rawdata[index - 1] & 0x000000FF) << 24;
//			if(nextoffset %2 ==1) nextoffset++;
//			System.out.println(Integer.toHexString(index) + " " + Integer.toHexString(nextoffset) + ", Following bytes = " + Integer.toHexString(rawdata[index+4]) + " " + Integer.toHexString(rawdata[index+5]) + " " + Integer.toHexString(rawdata[index+6]) + " " + Integer.toHexString(rawdata[index+7]));
//			*/
//			//index+=1;
//			//System.out.println("Index = " + index);
//			if(index >= length) break;
//		}
//		decode.reset();
//		decode.close();
//		System.out.println("Decode complete #frames= " + i);
//		
//	}
//	
	
	/**
	 * This method initializes jContentPane
	 * 
	 * @return javax.swing.JPanel
	 */
	private JPanel getJContentPane() {
		if (jContentPane == null) {
			jContentPane = new JPanel();
			jContentPane.setLayout(new BoxLayout(getJContentPane(), BoxLayout.Y_AXIS));
			//jContentPane.add(getMediaPlayer(), null);
			//getMediaPlayer();
			//jContentPane.add(getVideo());
			jContentPane.add(getDisplayPanel(), null);
			jContentPane.add(getJava2DRenderer(), null);
		}
		return jContentPane;
	}

	@Override
	public void Shutdown() {
		VideoClientSystem.shutdown();
	}

	/**
	 * This method initializes displayPanel	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getDisplayPanel() {
		if (displayPanel == null) {
			fpsLabel = new JLabel();
			fpsLabel.setText("0 fps");
			fpsLabel.setLocation(new Point(2, 20));
			fpsLabel.setSize(new Dimension(70, 20));
			resLabel = new JLabel();
			resLabel.setText("NA");
			resLabel.setLocation(new Point(2, 0));
			resLabel.setSize(new Dimension(69, 20));
			displayPanel = new JPanel();
			displayPanel.setPreferredSize(new Dimension(300, 40));
			displayPanel.setMaximumSize(new Dimension(2000, 60));
			displayPanel.setLayout(null);
			displayPanel.add(resLabel, null);
			displayPanel.add(fpsLabel, null);
			displayPanel.add(getPlayToggleButton(), null);
			displayPanel.add(getPauseToggleButton(), null);
			displayPanel.add(getStopToggleButton(), null);
			displayPanel.add(getChannelComboBox(), null);
			displayPanel.add(getJButton(), null);
			displayPanel.add(getFsButton(), null);
		}
		return displayPanel;
	}
//
//	/**
//	 * This method initializes mediaPlayer	
//	 * 	
//	 * @return javax.media.bean.playerbean.MediaPlayer	
//	 */
//	private MediaPlayer getMediaPlayer() {
//		if (mediaPlayer == null) {
//			File f = new File("./test.avi");
//			//File f = new File("./test1.wmv");
//			//File f = new File("./test.m4v");
//			//File f = new File("./Sample.mov");
//			try{
//				
////				mediaPlayer = (MediaPlayer)Manager.createPlayer(f.toURL());
//				mediaPlayer = new MediaPlayer();
//				mediaPlayer.setMediaLocation("file://" + f.getAbsolutePath());
//				
//				//decode.process(arg0, arg1);
////				VideoCodec vc;
////				BasicCodec bc;
////				Codec c;
////				PlugIn p;
////				Controls ctrls;
////				
////				c = decode;
////				//format.
////				//format.
////				TestDataSource s = new TestDataSource();
////				
////				mediaPlayer = new MediaPlayer();
////				mediaPlayer.setDataSource(s);
////				//mediaPlayer = Manager.createProcessor(c);
////				//mediaPlayer = new Player();
////				// get the components for the video and the playback controls
////				video = mediaPlayer.getVisualComponent();
////				mediaPlayer.start(); // start playing the media clip
//			}catch(Exception e){
//				e.printStackTrace();
//			}
//			
//		}
//		return mediaPlayer;
//	}

	/**
	 * This method initializes java2DRenderer	
	 * 	
	 * @return com.omnividea.media.renderer.video.Java2DRenderer	
	 */
	private Java2DRenderer getJava2DRenderer() {
		if (java2DRenderer == null) {
			java2DRenderer = new Java2DRenderer();
		}
		return java2DRenderer;
	}

	/**
	 * This method initializes playToggleButton	
	 * 	
	 * @return javax.swing.JToggleButton	
	 */
	private JToggleButton getPlayToggleButton() {
		if (playToggleButton == null) {
			playToggleButton = new JToggleButton();
			playToggleButton.setIcon(new ImageIcon(getClass().getResource("/play.png")));
			playToggleButton.setSize(new Dimension(60, 40));
			playToggleButton.setLocation(new Point(396, 1));
			playToggleButton.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					VideoClientSystem.getInstance().start(current_channel);
					setToggleButtons();
				}
			});
		}
		return playToggleButton;
	}

	/**
	 * This method initializes pauseToggleButton	
	 * 	
	 * @return javax.swing.JToggleButton	
	 */
	private JToggleButton getPauseToggleButton() {
		if (pauseToggleButton == null) {
			pauseToggleButton = new JToggleButton();
			pauseToggleButton.setPreferredSize(new Dimension(60, 40));
			pauseToggleButton.setLocation(new Point(337, 0));
			pauseToggleButton.setSize(new Dimension(60, 40));
			pauseToggleButton.setIcon(new ImageIcon(getClass().getResource("/pause.png")));
			pauseToggleButton.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					VideoClientSystem.getInstance().pause(current_channel);
					setToggleButtons();
				}
			});
		}
		return pauseToggleButton;
	}

	/**
	 * This method initializes stopToggleButton	
	 * 	
	 * @return javax.swing.JToggleButton	
	 */
	private JToggleButton getStopToggleButton() {
		if (stopToggleButton == null) {
			stopToggleButton = new JToggleButton();
			stopToggleButton.setIcon(new ImageIcon(getClass().getResource("/stop.png")));
			stopToggleButton.setSize(new Dimension(60, 40));
			stopToggleButton.setLocation(new Point(277, 0));
			stopToggleButton.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					VideoClientSystem.getInstance().stop(current_channel);
					setToggleButtons();
				}
			});
		}
		return stopToggleButton;
	}
	
	/**
	 * This method initializes channelComboBox	
	 * 	
	 * @return javax.swing.JComboBox	
	 */
	private JComboBox getChannelComboBox() {
		if (channelComboBox == null) {
			String[] strings = { "Ch. 1", "Ch. 2", "Ch. 3", "Ch. 4"};
			channelComboBox = new JComboBox(strings);
			//channelComboBox = new JComboBox();
			channelComboBox.setLocation(new Point(137, 10));
			channelComboBox.setSize(new Dimension(64, 20));
			channelComboBox.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					switchChannels(channelComboBox.getSelectedIndex());
				}
			});
		}
		return channelComboBox;
	}

	@Override
	public void nextFrameReady(Buffer current_frame, int channel) {
		//System.out.println("It's working !!!");
		System.out.println("video packet received channel = " + channel);
		getJava2DRenderer().process(current_frame);
		frameCount++;
		if(frameCount % 20 == 0){
			sw.Stop();
			fpsLabel.setText(df.format(20000/sw.getElapsedMillis()) + " fps");
			VideoFormat vf = (VideoFormat) current_frame.getFormat();
			resLabel.setText(vf.getSize().width + "x" + vf.getSize().height);
			
			sw.Reset();
			sw.Start();
		}
	}

	/**
	 * This method initializes jButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getJButton() {
		if (jButton == null) {
			jButton = new JButton();
			jButton.setBounds(new Rectangle(214, 8, 61, 24));
			jButton.setText("test");
			jButton.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					playStream();
				}
			});
		}
		return jButton;
	}

	/**
	 * This method initializes fsButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getFsButton() {
		if (fsButton == null) {
			fsButton = new JButton();
			fsButton.setBounds(new Rectangle(76, 3, 48, 30));
			fsButton.setText("FS");
			fsButton.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					toggleFullscreen();
				}
			});
		}
		return fsButton;
	}

	@Override
	public void keyPressed(KeyEvent arg0) {
		System.out.println("keypressed = " + arg0.getKeyChar());
//		if(arg0.getKeyChar() == 'q'){
//			exitFullscreen();
//		}
		toggleFullscreen();
	}

	@Override
	public void keyReleased(KeyEvent arg0) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void keyTyped(KeyEvent arg0) {
		// TODO Auto-generated method stub
		
	}
	
}  //  @jve:decl-index=0:visual-constraint="10,10"
