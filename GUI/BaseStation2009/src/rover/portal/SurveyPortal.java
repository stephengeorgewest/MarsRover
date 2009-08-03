package rover.portal;

import java.awt.BorderLayout;
import java.awt.Frame;
import java.awt.Graphics;

import javax.imageio.ImageIO;
import javax.media.Buffer;
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
import rover.network.CameraPacket;
import rover.network.ControlPacket;
import rover.network.MulticastListener;
import rover.network.Packet;
import rover.network.PacketHandler;
import rover.network.SocketInfo;
import rover.network.VideoClientSystem;
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

import rover.video.FrameListener;
import rover.video.Java2DRenderer;
import java.awt.GridBagConstraints;
import java.awt.Insets;

public class SurveyPortal extends Portal implements ChangeListener, GPSHandler, FrameListener {

	private static final long serialVersionUID = 1L;
	private JPanel jContentPane = null;
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
	
	private InetAddress z_address;  //  @jve:decl-index=0:
	private int z_port;
	
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
	private JLabel jLabel5 = null;
	private JLabel jLabel8 = null;
	private JLabel jLabel9 = null;
	private JTextField headingField = null;
	private JTextField camAzField = null;
	private JTextField camElField = null;
	private JTextField mousexField = null;
	private JTextField mouseyField = null;
	private JLabel gimbr_chan = null;
	private JLabel gimbe_chan = null;
	private JLabel jLabel10 = null;
	private JTextField elevationField = null;
	private JSpinner zoomSpinner = null;
	private Java2DRenderer java2DRenderer = null;
	private JPanel jPanel = null;
	/**
	 * This is the default constructor
	 */
	public SurveyPortal(Frame owner) {
		super(owner);
		
		initSocket();
		
		VideoClientSystem.init();
		VideoClientSystem.getInstance().registerForVideoEvents(this, 2);
		VideoClientSystem.getInstance().start(2);
		
		cp = new ControlPacket((byte)2);
		rotation_chan = Integer.parseInt(Main.props.getProperty("gimbr_chan"));
		elevation_chan = Integer.parseInt(Main.props.getProperty("gimbe_chan"));
		//zoom_chan = Integer.parseInt(Main.props.getProperty("zoom_chan"));
		
		cp.Channel_list[0] = (byte)rotation_chan;
		cp.Channel_list[1] = (byte)elevation_chan;
		//cp.Channel_list[2] = (byte)zoom_chan;
		
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
		
		fps_watch = new Stopwatch();
		try {
		    cur_image = ImageIO.read(this.getClass().getClassLoader().getResourceAsStream("No_Image.GIF"));
		} catch (Exception e) {
			e.printStackTrace();
		}
		
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
			//cp.Value_list[2] = zoom;
			
			cp.ToByteArray();
			DatagramPacket pack = new DatagramPacket(cp.packet, cp.bytes,
					 mc_address, mc_port);
			//System.out.println("Refreshing gimbal values");
			try{socket.send(pack);}
			catch(Exception e){
				e.printStackTrace();
			}
		}
	}
	
	private void sendZoomUpdate(){
		synchronized(threadLock) {
			CameraPacket campak = new CameraPacket(zoom);
			campak.ToByteArray();
			DatagramPacket pack = new DatagramPacket(campak.packet, campak.bytes,
					 z_address, z_port);
			System.out.println("Sending zoom value");
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
		
			z_address = InetAddress.getByName(Main.props.getProperty("zoom_address"));
			z_port = Integer.parseInt(Main.props.getProperty("zoom_port"));
		
			
			
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
	
//	public void SetFrame(BufferedImage b){
//
//		fps_watch.Stop();
//		double fps = 1/fps_watch.getElapsedSeconds();
//
//		fpsLabel.setText(fps + " fps");
//		
//		fps_watch.Reset();
//		fps_watch.Start();
//		
//		if(b != null){
//			cur_image = b;
//			heightLabel.setText(b.getHeight() + " pixels");
//			widthLabel.setText(b.getWidth() + " pixels");
//			videoPanel.setImage(cur_image);
//			videoPanel.repaint();
//		}
//	}
	
	
	
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
			GridBagConstraints gridBagConstraints1 = new GridBagConstraints();
			gridBagConstraints1.insets = new Insets(0, 0, 0, 1);
			gridBagConstraints1.gridy = 0;
			gridBagConstraints1.ipadx = 300;
			gridBagConstraints1.ipady = 346;
			gridBagConstraints1.weightx = 1.0D;
			gridBagConstraints1.weighty = 1.0D;
			gridBagConstraints1.gridx = 1;
			gridBagConstraints1.fill = GridBagConstraints.BOTH;
			GridBagConstraints gridBagConstraints = new GridBagConstraints();
			gridBagConstraints.gridx = 0;
			gridBagConstraints.ipadx = 280;
			gridBagConstraints.ipady = 300;
			gridBagConstraints.gridy = 0;
			jLabel10 = new JLabel();
			jLabel10.setText("Rover Elevation");
			jLabel10.setBounds(new Rectangle(17, 135, 96, 21));
			gimbe_chan = new JLabel();
			gimbe_chan.setText("");
			gimbe_chan.setBounds(new Rectangle(225, 47, 41, 16));
			gimbr_chan = new JLabel();
			gimbr_chan.setText("");
			gimbr_chan.setBounds(new Rectangle(226, 22, 41, 17));
			jLabel9 = new JLabel();
			jLabel9.setText("Mouse Y");
			jLabel9.setBounds(new Rectangle(16, 250, 68, 18));
			jLabel8 = new JLabel();
			jLabel8.setText("Mouse X");
			jLabel8.setBounds(new Rectangle(16, 223, 67, 18));
			jLabel5 = new JLabel();
			jLabel5.setText("Camera Elevation");
			jLabel5.setBounds(new Rectangle(15, 193, 103, 21));
			jLabel4 = new JLabel();
			jLabel4.setText("Camera Zoom");
			jLabel4.setBounds(new Rectangle(15, 78, 97, 21));
			jLabel1 = new JLabel();
			jLabel1.setText("Camera Azimuth");
			jLabel1.setBounds(new Rectangle(15, 163, 97, 22));
			jLabel = new JLabel();
			jLabel.setText("Rover Heading");
			jLabel.setBounds(new Rectangle(16, 107, 97, 20));
			jLabel3 = new JLabel();
			jLabel3.setText("Gimbal Elevation");
			jLabel3.setBounds(new Rectangle(16, 47, 96, 21));
			jLabel2 = new JLabel();
			jLabel2.setText("Gimbal Phi");
			jLabel2.setBounds(new Rectangle(16, 18, 74, 22));
			jContentPane = new JPanel();
			jContentPane.setLayout(new GridBagLayout());
			jContentPane.add(getJPanel(), gridBagConstraints);
			jContentPane.add(getJava2DRenderer(), gridBagConstraints1);
		}
		return jContentPane;
	}

	@Override
	public void Shutdown() {
		// TODO Auto-generated method stub
		
	}

	/**
	 * This method initializes phiSpinner	
	 * 	
	 * @return javax.swing.JSpinner	
	 */
	private JSpinner getPhiSpinner() {
		if (phiSpinner == null) {
			phiSpinner = new JSpinner(new SpinnerNumberModel(50, 0, 100, .01));
			phiSpinner.setBounds(new Rectangle(129, 19, 87, 23));
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
			phiSpinner1.setBounds(new Rectangle(130, 46, 86, 21));
			phiSpinner1.addChangeListener(this);
		}
		return phiSpinner1;
	}

	/**
	 * This method initializes headingField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	private JTextField getHeadingField() {
		if (headingField == null) {
			headingField = new JTextField();
			headingField.setBounds(new Rectangle(132, 109, 90, 20));
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
			camAzField.setBounds(new Rectangle(131, 164, 90, 20));
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
			camElField.setBounds(new Rectangle(131, 194, 90, 20));
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
			mousexField.setBounds(new Rectangle(131, 224, 90, 20));
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
			mouseyField.setBounds(new Rectangle(131, 251, 90, 20));
		}
		return mouseyField;
	}

	@Override
	public void stateChanged(ChangeEvent arg0) {
		if(arg0.getSource() == zoomSpinner){
			Number z = (Number)(zoomSpinner.getValue());
			zoom = z.floatValue();
			sendZoomUpdate();
		}else{
			Number phi = (Number)(phiSpinner.getValue());
			gimbr = phi.floatValue();
			
			Number theta = (Number)(phiSpinner1.getValue());
			gimbe = theta.floatValue();
			
			sendGimbalUpdate();
		}
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
			elevationField.setBounds(new Rectangle(133, 136, 90, 20));
		}
		return elevationField;
	}

	/**
	 * This method initializes zoomSpinner	
	 * 	
	 * @return javax.swing.JSpinner	
	 */
	private JSpinner getZoomSpinner() {
		if (zoomSpinner == null) {
			zoomSpinner = new JSpinner(new SpinnerNumberModel(1, 1, 34, .5));
			zoomSpinner.setBounds(new Rectangle(130, 76, 86, 21));
			zoomSpinner.addChangeListener(this);
		}
		return zoomSpinner;
	}

	/**
	 * This method initializes java2DRenderer	
	 * 	
	 * @return rover.video.Java2DRenderer	
	 */
	private Java2DRenderer getJava2DRenderer() {
		if (java2DRenderer == null) {
			java2DRenderer = new Java2DRenderer();
		}
		return java2DRenderer;
	}

	/**
	 * This method initializes jPanel	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getJPanel() {
		if (jPanel == null) {
			jPanel = new JPanel();
			jPanel.setLayout(null);
			jPanel.add(jLabel2, null);
			jPanel.add(jLabel3, null);
			jPanel.add(getPhiSpinner(), null);
			jPanel.add(getPhiSpinner1(), null);
			jPanel.add(jLabel, null);
			jPanel.add(jLabel1, null);
			jPanel.add(jLabel4, null);
			jPanel.add(jLabel5, null);
			jPanel.add(jLabel8, null);
			jPanel.add(jLabel9, null);
			jPanel.add(getHeadingField(), null);
			jPanel.add(getCamAzField(), null);
			jPanel.add(getCamElField(), null);
			jPanel.add(getMousexField(), null);
			jPanel.add(getMouseyField(), null);
			jPanel.add(gimbr_chan, null);
			jPanel.add(gimbe_chan, null);
			jPanel.add(jLabel10, null);
			jPanel.add(getElevationField(), null);
			jPanel.add(getZoomSpinner(), null);
		}
		return jPanel;
	}

	@Override
	public void nextFrameReady(Buffer current_frame, int channel) {
		java2DRenderer.process(current_frame);
		
	}

}  //  @jve:decl-index=0:visual-constraint="10,10"
