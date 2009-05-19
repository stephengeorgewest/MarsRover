package rover.portal;

import java.awt.BorderLayout;
import java.awt.Frame;
import java.awt.Graphics;

import javax.imageio.ImageIO;
import javax.swing.JPanel;
import javax.swing.JFrame;
import javax.swing.SpinnerNumberModel;

import java.awt.Dimension;
import javax.swing.BoxLayout;
import java.awt.GridBagLayout;
import java.awt.Rectangle;
import javax.swing.JLabel;
import java.awt.Point;
import java.awt.image.BufferedImage;
import java.io.DataInputStream;
import java.io.File;
import java.io.InputStream;
import java.net.DatagramPacket;
import java.net.InetAddress;
import java.net.MulticastSocket;
import java.net.SocketTimeoutException;
import java.util.Date;
import javax.swing.JButton;

import rover.Main;
import rover.guistuff.ImagePanel;
import rover.network.ControlPacket;
import rover.network.MulticastListener;
import rover.network.Packet;
import rover.network.PacketHandler;
import rover.network.SocketInfo;
import rover.network.VideoPacket;
import rover.network.VideoPacketBuffer;
import rover.utils.GPSClient;
import rover.utils.GpsData;
import rover.utils.Stopwatch;
import rover.utils.GPSClient.GPSHandler;

import javax.swing.JSpinner;
import javax.swing.JTextField;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

public class SurveyPortal extends Portal implements ChangeListener, GPSHandler {

	private static final long serialVersionUID = 1L;
	private JPanel jContentPane = null;
	private ImagePanel videoPanel = null;
	
	//video stream stuff
	BufferedImage cur_image = null;  //  @jve:decl-index=0:	
	Stopwatch fps_watch;
	VideoPacketBuffer buffer = null;
	//Thread netThread = null;
	MulticastListener ml = null;
	Thread videoThread;
	VideoPacket test1, test2, test3;
	
	//gps stuff
	private GPSClient gpsclient = null;
	private GpsData gpsdata = null;
	
	//servo control stuff
	SocketInfo si;
	MulticastSocket socket;  //  @jve:decl-index=0:
	private InetAddress mc_address;  //  @jve:decl-index=0:
	private int mc_port;
	Thread servoThread;
	Object threadLock;
	boolean servoThreadKill = false;
	int send_period = 500;
	float gimbr = 50;
	float gimbe = 50;
	float zoom = 50;
	ControlPacket cp;
	int rotation_chan;
	int elevation_chan;
	int zoom_chan;
	
	float[] gimbr_cal = {0,50,100};
	float[] gimbe_cal = {0,50,100};
	float[] zoom_cal = {40,50,60};
	
	
	
	private JLabel jLabel2 = null;
	private JLabel jLabel3 = null;
	private JSpinner phiSpinner = null;
	private JSpinner phiSpinner1 = null;
	private JLabel jLabel = null;
	private JLabel jLabel1 = null;
	private JLabel jLabel4 = null;
	private JButton outButton = null;
	private JButton inButton = null;
	private JLabel jLabel5 = null;
	private JLabel fpsLabel = null;
	private JLabel heightLabel = null;
	private JLabel widthLabel = null;
	private JLabel jLabel6 = null;
	private JLabel jLabel7 = null;
	private JLabel jLabel8 = null;
	private JLabel jLabel9 = null;
	private JTextField headingField = null;
	private JTextField camAzField = null;
	private JTextField camElField = null;
	private JTextField mousexField = null;
	private JTextField mouseyField = null;
	private JLabel gimbr_chan = null;
	private JLabel gimbe_chan = null;
	private JLabel zoomLabel = null;
	private JLabel jLabel10 = null;
	private JTextField elevationField = null;
	/**
	 * This is the default constructor
	 */
	public SurveyPortal(Frame owner) {
		super(owner);
		
		initSocket();
		
		cp = new ControlPacket((byte)3);
		rotation_chan = Integer.parseInt(Main.props.getProperty("gimbr_chan"));
		elevation_chan = Integer.parseInt(Main.props.getProperty("gimbe_chan"));
		zoom_chan = Integer.parseInt(Main.props.getProperty("zoom_chan"));
		
		cp.Channel_list[0] = (byte)rotation_chan;
		cp.Channel_list[1] = (byte)elevation_chan;
		cp.Channel_list[2] = (byte)zoom_chan;
		
		gpsdata = new GpsData();
		try{
			InetAddress gps_address = InetAddress.getByName(Main.props.getProperty("gps_address"));
			int gps_port = Integer.parseInt(Main.props.getProperty("gps_port"));
			gpsclient = new GPSClient(gps_address, gps_port);
			gpsclient.addGPSListener(this);
		} catch (Exception e){
			e.printStackTrace();			
		}
		
		
		servoThread = new Thread(){	@Override public void run() {servoThreadStart();}};
		servoThread.setDaemon(true);
		threadLock = new Object();
		servoThread.start();
		
		initialize();
		
		gimbr_chan.setText("c" + rotation_chan);
		gimbe_chan.setText("c" + elevation_chan);
		zoomLabel.setText("c" + zoom_chan);
		
		fps_watch = new Stopwatch();
		try {
		    cur_image = ImageIO.read(this.getClass().getClassLoader().getResourceAsStream("No_Image.GIF"));
		} catch (Exception e) {
			e.printStackTrace();
		}
		SetFrame(cur_image);
		
		
		
		//TODO Add configuration stuff here
		/*try{
			mcgroup = InetAddress.getByName("224.6.2.5");
			mcport = 4625;
		}catch(Exception e){
			e.printStackTrace();
		}*/
		
		try{
			InetAddress mcaddress = InetAddress.getByName(Main.props.getProperty("video1_address"));
			int port = Integer.parseInt(Main.props.getProperty("video1_port"));
			ml = new MulticastListener(mcaddress, port);
			
			SocketInfo si = new SocketInfo();
			si.setPortal("Site Survey Portal");
			si.setPort(port);
			si.setType("MC Rec");
			si.setAddress(mcaddress);
			
			MainGUI.registerSocket(si);
			
			
		}
		catch(Exception e){
			e.printStackTrace();
			return;
		}
		
		ml.addPacketListener(new PacketHandler(){
			@Override
			public void PacketReceived(Packet p) {
				VideoPacket vp = new VideoPacket(p);
				buffer.Process(vp);

			}

		});
		buffer = new VideoPacketBuffer();
		
		
//		ThreadStart t = new ThreadStart(this);
//		netThread = new Thread(t);
//		netThread.setDaemon(true);
//		netThread.start();
	
		
		
		
	}
	
	private void servoThreadStart(){
		
		while(!servoThreadKill){
			try{
				sendGimbalUpdate();
				int sleep_period = send_period;
				if(sleep_period > 0)
					Thread.sleep(sleep_period);
			}catch(Exception e){
				e.printStackTrace();
			}
		}
		
	}
	
	private void sendGimbalUpdate(){
		synchronized(threadLock) {
			cp.Value_list[0] = gimbr;
			cp.Value_list[1] = gimbe;
			cp.Value_list[2] = zoom;
			
			cp.ToByteArray();
			DatagramPacket pack = new DatagramPacket(cp.packet, cp.bytes,
					 mc_address, mc_port);
			System.out.println("Refreshing gimbal values");
			try{socket.send(pack);}
			catch(Exception e){
				e.printStackTrace();
			}
		}
	}
	
	private void initSocket(){

		try{
			mc_address = InetAddress.getByName(Main.props.getProperty("control_address"));
			mc_port = Integer.parseInt(Main.props.getProperty("control_port"));
		
			
			socket = new MulticastSocket(mc_port);
			socket.setSoTimeout(40);
			socket.joinGroup(mc_address);
			socket.setTimeToLive(10);
			
			
			si = new SocketInfo();
			si.setPortal("Servo Portal");
			si.setPort(mc_port);
			si.setType("MC Send/MC Rec");
			si.setAddress(mc_address);
			
			MainGUI.registerSocket(si);
		}catch(Exception e){
			e.printStackTrace();
		}
	}
	
	@Override
	public String toString(){
		return this.getTitle();
	}
	
	public static class SurveyPortalInit extends PortalInit {
		@Override
		public Portal createPortal() {
			return new SurveyPortal(MainGUI.instance);
		}
		
		@Override
		public String toString(){
			return "Site Survey Portal";
		}
	}
	
	
/*	class ThreadStart implements Runnable{
		VideoPortal parent;
		MulticastSocket s = null;
		
		public ThreadStart(VideoPortal p){
			parent = p;
		}
		
		public void run(){
			
			while(true){
				try{
					if(s == null){
						s = new MulticastSocket(parent.mcport);
						s.joinGroup(parent.mcgroup);
						
					}else{
						byte[] buf = new byte[65000];
						DatagramPacket recv = new DatagramPacket(buf, buf.length);
//						System.out.println("Listening for packets");
						s.receive(recv);
//						System.out.println("Packet Received");
						VideoPacket vp = new VideoPacket(recv.getData(), recv.getLength());
						parent.buffer.Process(vp);
						if(parent.cur_image != buffer.current_image) parent.SetFrame(buffer.current_image);
					}
				}catch (Exception e){
					e.printStackTrace();
				}
				
				
			}
		}
		
	}*/
	
	public void SetFrame(BufferedImage b){

		fps_watch.Stop();
		double fps = 1/fps_watch.getElapsedSeconds();

		fpsLabel.setText(fps + " fps");
		
		fps_watch.Reset();
		fps_watch.Start();
		
		if(b != null){
			cur_image = b;
			heightLabel.setText(b.getHeight() + " pixels");
			widthLabel.setText(b.getWidth() + " pixels");
			videoPanel.setImage(cur_image);
			videoPanel.repaint();
		}
	}
	
	
	
	/**
	 * This method initializes this
	 * 
	 * @return void
	 */
	private void initialize() {
		this.setSize(621, 385);
		this.setContentPane(getJContentPane());
		this.setTitle("Site Survey");
	}

	/**
	 * This method initializes jContentPane
	 * 
	 * @return javax.swing.JPanel
	 */
	private JPanel getJContentPane() {
		if (jContentPane == null) {
			jLabel10 = new JLabel();
			jLabel10.setBounds(new Rectangle(15, 133, 96, 21));
			jLabel10.setText("Rover Elevation");
			zoomLabel = new JLabel();
			zoomLabel.setBounds(new Rectangle(224, 79, 42, 13));
			zoomLabel.setText("");
			gimbe_chan = new JLabel();
			gimbe_chan.setBounds(new Rectangle(223, 45, 41, 16));
			gimbe_chan.setText("");
			gimbr_chan = new JLabel();
			gimbr_chan.setBounds(new Rectangle(224, 20, 41, 17));
			gimbr_chan.setText("");
			jLabel9 = new JLabel();
			jLabel9.setBounds(new Rectangle(14, 248, 68, 18));
			jLabel9.setText("Mouse Y");
			jLabel8 = new JLabel();
			jLabel8.setBounds(new Rectangle(14, 221, 67, 18));
			jLabel8.setText("Mouse X");
			jLabel7 = new JLabel();
			jLabel7.setBounds(new Rectangle(14, 301, 55, 19));
			jLabel7.setText("Width");
			jLabel6 = new JLabel();
			jLabel6.setBounds(new Rectangle(15, 271, 49, 23));
			jLabel6.setText("Height");
			widthLabel = new JLabel();
			widthLabel.setBounds(new Rectangle(128, 298, 47, 21));
			widthLabel.setText("0");
			fpsLabel = new JLabel();
			fpsLabel.setBounds(new Rectangle(15, 326, 218, 20));
			fpsLabel.setText("0 fps");
			jLabel5 = new JLabel();
			jLabel5.setBounds(new Rectangle(13, 191, 103, 21));
			jLabel5.setText("Camera Elevation");
			jLabel4 = new JLabel();
			jLabel4.setBounds(new Rectangle(13, 76, 97, 21));
			jLabel4.setText("Camera Zoom");
			jLabel1 = new JLabel();
			jLabel1.setBounds(new Rectangle(13, 161, 97, 22));
			jLabel1.setText("Camera Azimuth");
			jLabel = new JLabel();
			jLabel.setBounds(new Rectangle(14, 105, 97, 20));
			jLabel.setText("Rover Heading");
			jLabel3 = new JLabel();
			jLabel3.setBounds(new Rectangle(14, 45, 96, 21));
			jLabel3.setText("Gimbal Elevation");
			jLabel2 = new JLabel();
			jLabel2.setBounds(new Rectangle(14, 16, 74, 22));
			jLabel2.setText("Gimbal Phi");
			jContentPane = new JPanel();
			jContentPane.setLayout(null);
			jContentPane.add(getVideoPanel(), null);
			jContentPane.add(jLabel2, null);
			jContentPane.add(jLabel3, null);
			jContentPane.add(getPhiSpinner(), null);
			jContentPane.add(getPhiSpinner1(), null);
			jContentPane.add(jLabel, null);
			jContentPane.add(jLabel1, null);
			jContentPane.add(jLabel4, null);
			jContentPane.add(getOutButton(), null);
			jContentPane.add(getInButton(), null);
			jContentPane.add(jLabel5, null);
			jContentPane.add(fpsLabel, null);
			jContentPane.add(getHeightLabel(), null);
			jContentPane.add(widthLabel, null);
			jContentPane.add(jLabel6, null);
			jContentPane.add(jLabel7, null);
			jContentPane.add(jLabel8, null);
			jContentPane.add(jLabel9, null);
			jContentPane.add(getHeadingField(), null);
			jContentPane.add(getCamAzField(), null);
			jContentPane.add(getCamElField(), null);
			jContentPane.add(getMousexField(), null);
			jContentPane.add(getMouseyField(), null);
			jContentPane.add(gimbr_chan, null);
			jContentPane.add(gimbe_chan, null);
			jContentPane.add(zoomLabel, null);
			jContentPane.add(jLabel10, null);
			jContentPane.add(getElevationField(), null);
		}
		return jContentPane;
	}

	@Override
	public void Shutdown() {
		// TODO Auto-generated method stub
		
	}

	/**
	 * This method initializes videoPanel	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getVideoPanel() {
		if (videoPanel == null) {
			videoPanel = new ImagePanel();
			videoPanel.setLayout(null);
			videoPanel.setBounds(new Rectangle(272, 11, 325, 300));
		}
		return videoPanel;
	}

	public void ThreadVideoTest(){
		videoThread = new Thread(){	
			@Override public void run() { 
				while(true){
					try{
						videoTest();
						sleep(30);
					}catch(Exception e){
						e.printStackTrace();
					}
					
				}
			}
		};
		videoThread.setDaemon(true);
		videoThread.start();
	}
	
	

	public void videoTest(){

		if(test1 == null){
			try{
				//System.out.println("Testing Video System");
				InputStream s = this.getClass().getClassLoader().getResourceAsStream("test.yuv");
				DataInputStream ds = new DataInputStream(s);
				
	            int psize = 65000;
	            int byte_count = 0;
	            byte[] p1 = new byte[psize];
	            byte[] p2 = new byte[psize];
	            byte[] p3 = new byte[psize];
	
	            int bytes1 = 10;
	            int bytes2 = 10;
	            int bytes3 = 10;
	
	
	            short width  = 352;
	            short height = 288;
	
	            p1[0] = 0; //PacketID
	            p1[1] = 0;                      //ChannelID
	            p1[2] = (byte)(width >> 8);     //Image Width MSB
	            p1[3] = (byte)(width & 0x00FF); //Image Width LSB
	            p1[4] = (byte)(height >> 8);    //Image Height MSB
	            p1[5] = (byte)(height & 0x00FF);//Image Height LSB
	            p1[6] = 0;
	            p1[7] = 5;
	            p1[8] = 1;
	            p1[9] = 3;
	
	            for (int i = 10; i < psize; i++)
	            {
	                ds.read(p1, i, 1);
	                //Console.Write(" " + p1[i]);
	                bytes1++;
	                byte_count++;
	            }
	
	            p2[0] = 0; //PacketID
	            p2[1] = 0;                      //ChannelID
	            p2[2] = (byte)(width >> 8);     //Image Width MSB
	            p2[3] = (byte)(width & 0x00FF); //Image Width LSB
	            p2[4] = (byte)(height >> 8);    //Image Height MSB
	            p2[5] = (byte)(height & 0x00FF);//Image Height LSB
	            p2[6] = 0;
	            p2[7] = 5;
	            p2[8] = 2;
	            p2[9] = 3;
	
	            for (int i = 10; i < psize; i++)
	            {
	                ds.read(p2, i, 1);
	                bytes2++;
	                byte_count++;
	            }
	
	            p3[0] = 0; //PacketID
	            p3[1] = 0;                      //ChannelID
	            p3[2] = (byte)(width >> 8);     //Image Width MSB
	            p3[3] = (byte)(width & 0x00FF); //Image Width LSB
	            p3[4] = (byte)(height >> 8);    //Image Height MSB
	            p3[5] = (byte)(height & 0x00FF);//Image Height LSB
	            p3[6] = 0;
	            p3[7] = 5;
	            p3[8] = 3;
	            p3[9] = 3;
	
	            for (int i = 10; i < psize; i++)
	            {
	                int read = ds.read(p3, i, 1);
	                if (read == 0)
	                {
	                    //Console.WriteLine("read = " + read);
	                    break;
	                }
	                bytes3++;
	                byte_count++;
	                //Console.WriteLine(byte_count);
	            }
	            test1 = new VideoPacket(p1, p1.length);
	            test2 = new VideoPacket(p2, p2.length);
	            test3 = new VideoPacket(p3, p3.length);
	            
		            ds.close();
	
			}catch(Exception ex){
				ex.printStackTrace();
			}
            
		}
            
        buffer.Process(test1);
        buffer.Process(test2);
        buffer.Process(test3);
        
        SetFrame(buffer.current_image);
		
	}

	/**
	 * This method initializes phiSpinner	
	 * 	
	 * @return javax.swing.JSpinner	
	 */
	private JSpinner getPhiSpinner() {
		if (phiSpinner == null) {
			phiSpinner = new JSpinner(new SpinnerNumberModel(50, 0, 100, .01));
			phiSpinner.setBounds(new Rectangle(127, 17, 87, 23));
			phiSpinner.addChangeListener(this);
		}
		return phiSpinner;
	}

	/**
	 * This method initializes phiSpinner1	
	 * 	
	 * @return javax.swing.JSpinner	
	 */
	private JSpinner getPhiSpinner1() {
		if (phiSpinner1 == null) {
			phiSpinner1 = new JSpinner(new SpinnerNumberModel(50, 0, 100, .01));
			phiSpinner1.setBounds(new Rectangle(128, 44, 86, 21));
			phiSpinner1.addChangeListener(this);
		}
		return phiSpinner1;
	}

	/**
	 * This method initializes outButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getOutButton() {
		if (outButton == null) {
			outButton = new JButton();
			outButton.setBounds(new Rectangle(125, 74, 42, 25));
			outButton.setText("-");
			outButton.addMouseListener(new java.awt.event.MouseAdapter() {   
				public void mouseReleased(java.awt.event.MouseEvent e) {    
					zoom = zoom_cal[1];
					sendGimbalUpdate();
				}
				public void mousePressed(java.awt.event.MouseEvent e) {
					zoom = zoom_cal[0];
					sendGimbalUpdate();
				}
			});
		}
		return outButton;
	}

	/**
	 * This method initializes inButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getInButton() {
		if (inButton == null) {
			inButton = new JButton();
			inButton.setBounds(new Rectangle(175, 75, 44, 24));
			inButton.setText("+");
			inButton.addMouseListener(new java.awt.event.MouseAdapter() {   
				public void mouseReleased(java.awt.event.MouseEvent e) {    
					zoom = zoom_cal[1];
					sendGimbalUpdate();
				}
				public void mousePressed(java.awt.event.MouseEvent e) {
					zoom = zoom_cal[2];
					sendGimbalUpdate();
				}
			});
			
		}
		return inButton;
	}

	/**
	 * This method initializes heightLabel	
	 * 	
	 * @return javax.swing.JLabel	
	 */
	private JLabel getHeightLabel() {
		if (heightLabel == null) {
			heightLabel = new JLabel();
			heightLabel.setText("0");
			heightLabel.setBounds(new Rectangle(130, 271, 50, 20));
		}
		return heightLabel;
	}

	/**
	 * This method initializes headingField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	private JTextField getHeadingField() {
		if (headingField == null) {
			headingField = new JTextField();
			headingField.setLocation(new Point(130, 107));
			headingField.setSize(new Dimension(90, 20));
		}
		return headingField;
	}

	/**
	 * This method initializes camAzField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	private JTextField getCamAzField() {
		if (camAzField == null) {
			camAzField = new JTextField();
			camAzField.setLocation(new Point(129, 162));
			camAzField.setSize(new Dimension(90, 20));
		}
		return camAzField;
	}

	/**
	 * This method initializes camElField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	private JTextField getCamElField() {
		if (camElField == null) {
			camElField = new JTextField();
			camElField.setLocation(new Point(129, 192));
			camElField.setSize(new Dimension(90, 20));
		}
		return camElField;
	}

	/**
	 * This method initializes mousexField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	private JTextField getMousexField() {
		if (mousexField == null) {
			mousexField = new JTextField();
			mousexField.setLocation(new Point(129, 222));
			mousexField.setSize(new Dimension(90, 20));
		}
		return mousexField;
	}

	/**
	 * This method initializes mouseyField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	private JTextField getMouseyField() {
		if (mouseyField == null) {
			mouseyField = new JTextField();
			mouseyField.setLocation(new Point(129, 249));
			mouseyField.setSize(new Dimension(90, 20));
		}
		return mouseyField;
	}

	@Override
	public void stateChanged(ChangeEvent arg0) {
		Number phi = (Number)(phiSpinner.getValue());
		gimbr = phi.floatValue();
		
		Number theta = (Number)(phiSpinner1.getValue());
		gimbe = theta.floatValue();
		
		sendGimbalUpdate();
	}

	@Override
	public void GPSDataReceived(String data) {
		GpsData.MergeGpsString(data, gpsdata);
		headingField.setText(Double.toString(gpsdata.Heading));
		elevationField.setText(Double.toString(gpsdata.Elevation));
	}

	/**
	 * This method initializes elevationField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	private JTextField getElevationField() {
		if (elevationField == null) {
			elevationField = new JTextField();
			elevationField.setLocation(new Point(131, 134));
			elevationField.setSize(new Dimension(90, 20));
		}
		return elevationField;
	}

}  //  @jve:decl-index=0:visual-constraint="10,10"
