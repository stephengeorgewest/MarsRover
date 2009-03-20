package rover.portal;

import javax.swing.JCheckBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.SpinnerNumberModel;

import java.awt.Frame;
import java.awt.BorderLayout;
import java.awt.GridBagConstraints;

import javax.swing.JDialog;
import java.awt.Dimension;
import java.net.DatagramPacket;
import java.net.InetAddress;
import java.net.MulticastSocket;
import java.net.SocketTimeoutException;
import java.util.ArrayList;

import javax.swing.JSpinner;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import rover.Main;
import rover.input.InputDevice;
import rover.network.ControlPacket;
import rover.network.SocketInfo;
import rover.utils.Stopwatch;


import java.awt.GridLayout;
import java.awt.GridBagLayout;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;

public class ServoPortal extends Portal implements ChangeListener, ItemListener{

	public static int CHANNELS = 20;
	private JPanel jContentPane = null;
	
	ArrayList<JSpinner> spinlist = null;  //  @jve:decl-index=0:
	ArrayList<JSlider> slidelist = null;  //  @jve:decl-index=0:
	ArrayList<JCheckBox> boxlist = null;  //  @jve:decl-index=0:
	float[] values = null;
	
	int channels = 0;
	boolean disable_events = true;
	SocketInfo si = null;

	MulticastSocket socket;  //  @jve:decl-index=0:
	private InetAddress mc_address;  //  @jve:decl-index=0:
	private int mc_port;
	
	Thread servoThread;
	Object threadLock;
	
	private boolean servoThreadKill = false;
	private int send_period = 2000;
	
	/**
	 * @param owner
	 */
	public ServoPortal(Frame owner) {
		super(owner);
		initialize(CHANNELS);
		
		servoThread = new Thread(){	@Override public void run() {servoThreadStart();}};
		servoThread.setDaemon(true);
		threadLock = new Object();
		servoThread.start();
		
	}

	private void servoThreadStart(){
		int[] activeChannels = new int[channels];
		ControlPacket cp = new ControlPacket();
		Stopwatch sw = new Stopwatch();
		while(!servoThreadKill){
			try{
				int j = 0;
				for(int i = 0;i<channels;i++){
					if(boxlist.get(i).isSelected()){
						activeChannels[j] = i;
						j++;
					}
				}
				if(j != 0){
					synchronized(threadLock) {
						float[] val_list = new float[j];
						byte[] channel_list = new byte[j];
						for(int i = 0;i<j;i++){
							channel_list[i] = (byte)activeChannels[i];
							val_list[i] = getSpinVal(channel_list[i]);
							System.out.println("Channel " + channel_list[i] + " is active");
						}			
						cp.setData(channel_list, val_list);
					
						cp.ToByteArray();
						DatagramPacket pack = new DatagramPacket(cp.packet, cp.bytes,
								 mc_address, mc_port);
						System.out.println("Refreshing " + j + " active channels");
						socket.send(pack);
					}
				}
				sw.Reset();
				sw.Start();
				int count = 0;
				while(true){
					byte[] buf = new byte[channels*5+2];
					DatagramPacket pack = new DatagramPacket(buf,buf.length);
					try{socket.receive(pack); }
					catch(SocketTimeoutException ste){
						break;
					}
					
					cp.FromByteArray(pack.getData(), pack.getLength());
					readControlPacket(cp);
					count++;
					//System.out.println("Packet Received");
				}
				
				if(count > 0) updateGUI();
				sw.Stop();
				int sleep_period = send_period-(int)(sw.getElapsedMillis());
				if(sleep_period > 0)
					Thread.sleep(sleep_period);
			}catch(Exception e){
				e.printStackTrace();
			}
		}
		
	}
	
	private void readControlPacket(ControlPacket cp){
		for(int i = 0;i<cp.Channels;i++){
			float val = cp.Value_list[i];
			int chan = cp.Channel_list[i];
			if(boxlist.get(chan).isSelected()) continue;
			//System.out.println("Chan " +chan+ "being set to " + val);
			values[chan] = val;
		}
	}
	
	private void updateGUI(){
		disable_events = true;
		for(int i = 0;i<channels;i++){
			if(boxlist.get(i).isSelected()) continue;
			slidelist.get(i).setValue((int)(100*values[i]));
			spinlist.get(i).setValue(new Double(values[i]));
		}
		disable_events = false;
	}
	
	@Override
	public String toString(){
		return this.getTitle();
	}
	
	public static class ServoPortalInit extends PortalInit {
		@Override
		public Portal createPortal() {
			return new ServoPortal(MainGUI.instance);
		}
		
		@Override
		public String toString(){
			return "Servo Portal";
		}
	}
	
	@Override
	public void Shutdown() {
		servoThreadKill = true;
		if(si != null) MainGUI.instance.unregisterSocket(si);
		
	}

	/**
	 * This method initializes this
	 * 
	 * @return void
	 */
	private void initialize(int channels) {
		this.channels = channels;
		this.setSize(305, channels * 30);
		//this.setResizable(false);
		this.setTitle("Servo Portal");
		
		this.setContentPane(getJContentPane());
		
		values = new float[channels];
		spinlist = new ArrayList<JSpinner>();
		slidelist = new ArrayList<JSlider>();
		boxlist = new ArrayList<JCheckBox>();
		
		for(int i = 0;i<channels;i++){
			values[i] = 50f;
		}
		
		for(int i = 0;i<channels;i++){
			JSpinner spinner = new JSpinner(new SpinnerNumberModel(50, 0, 100, .01));
			JSlider slider = new JSlider();
			JCheckBox box = new JCheckBox();
			
			int line_height = 20;
			
			box.setText("Ch. "+i);
			
			//do some configuration here
			spinner.setMinimumSize(new Dimension(60, line_height));
			spinner.setMaximumSize(new Dimension(60, line_height));
			spinner.setSize(60, line_height);
			
			slider.setMaximum(10000);
			slider.setMinimum(0);
			slider.setMajorTickSpacing(100);
			slider.setMinorTickSpacing(1);
			slider.setValue(5000);
			slider.setMinimumSize(new Dimension(600, line_height));
			slider.setMaximumSize(new Dimension(600, line_height));
			slider.setSize(600, line_height);
		
			
			slider.setEnabled(false);
			spinner.setEnabled(false);
			
			slider.addChangeListener(this);
			spinner.addChangeListener(this);
			box.addItemListener(this);
			
			
			GridBagConstraints c = new GridBagConstraints();

			c.fill = GridBagConstraints.HORIZONTAL;
			c.gridx = 2;
			c.gridy = i;
			c.weightx = 1;
		
			jContentPane.add(slider, c);

			c.fill = GridBagConstraints.NONE;
			c.gridx = 1;
			c.gridy = i;
			c.weightx = 0;
			jContentPane.add(spinner, c);
			
			c.fill = GridBagConstraints.NONE;
			c.gridx = 0;
			c.gridy = i;
			c.weightx = 0;
			jContentPane.add(box, c);
			
			spinlist.add(spinner);
			slidelist.add(slider);
			boxlist.add(box);
		}
		updateGUI();
		disable_events = false;
		initSocket();
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
	
	private float getSpinVal(int i){
		float val;
		Object o = spinlist.get(i).getValue();
		if(o.getClass() == Double.class){
			Double d = (Double)o;
			val = d.floatValue();
		}else{
			Float f = (Float)o;
			val = f.floatValue();
		}
		return val;
	}
	
	@Override
	public void stateChanged(ChangeEvent arg0) {
		if(disable_events) return;
		disable_events = true;
		int i = 0;
		float val;
		if(arg0.getSource().getClass() == JSpinner.class){
			i = spinlist.indexOf(arg0.getSource());
			val = getSpinVal(i);
			slidelist.get(i).setValue((int)(100*val));
			//System.out.println("Servo " + i + " changing to " + val);
		}else{
			i = slidelist.indexOf(arg0.getSource());
			val = slidelist.get(i).getValue();
			val /= 100;
			spinlist.get(i).setValue(new Double(val));
			//System.out.println("Servo " + i + " changing to " + val);
		}
		values[i] = val;
		//send some servo control packets on channel i
		synchronized(threadLock) {
			try{
				float[] val_list = new float[1];
				byte[] channel_list = new byte[1];
				
				channel_list[0] = (byte)i;
				val_list[0] = (float) val;
				
		
				ControlPacket cp = new ControlPacket(channel_list, val_list);
			
				cp.ToByteArray();
				DatagramPacket pack = new DatagramPacket(cp.packet, cp.bytes,
						 mc_address, mc_port);
				System.out.println("Sending packet " + i + " set to " + val);
				socket.send(pack);
			} catch (Exception e){
				e.printStackTrace();
			}
		}
		
		//TODO send network packet
		
		disable_events = false;
	}

	
	/**
	 * This method initializes jContentPane
	 * 
	 * @return javax.swing.JPanel
	 */
	private JPanel getJContentPane() {
		if (jContentPane == null) {
			jContentPane = new JPanel();
			jContentPane.setLayout(new GridBagLayout());
		}
		return jContentPane;
	}

	@Override
	public void itemStateChanged(ItemEvent arg0) {
		
		int i = boxlist.indexOf(arg0.getSource());
		if(boxlist.get(i).isSelected()) {
			System.out.println("Enabling slider/spinner" + i);
			slidelist.get(i).setEnabled(true);
			spinlist.get(i).setEnabled(true);
		}else{
			System.out.println("Disabling slider/spinner "+i);
			slidelist.get(i).setEnabled(false);
			spinlist.get(i).setEnabled(false);
		}

	}

}  //  @jve:decl-index=0:visual-constraint="10,10"
