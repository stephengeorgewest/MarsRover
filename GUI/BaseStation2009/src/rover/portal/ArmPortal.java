package rover.portal;

import java.awt.Component;
import java.awt.Frame;
import java.awt.Dimension;
import javax.swing.JPanel;
import java.awt.GridBagLayout;
import java.awt.Rectangle;
import javax.swing.BorderFactory;
import javax.swing.border.TitledBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.table.AbstractTableModel;

import java.awt.Font;
import java.awt.Color;
import javax.swing.JLabel;
import java.awt.Point;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.ButtonGroup;
import javax.swing.JSpinner;
import javax.swing.JRadioButton;
import javax.swing.SpinnerNumberModel;
import javax.media.opengl.GLCanvas;
import java.awt.GridBagConstraints;
import java.net.DatagramPacket;
import java.net.InetAddress;
import java.net.MulticastSocket;
import java.net.SocketTimeoutException;
import java.text.DecimalFormat;

import rover.Main;
import rover.guistuff.ArmSimulationPanel3D;
import rover.guistuff.FloatSpinner;
import rover.guistuff.EarthSimPanel3D;
import rover.network.ArmPacket;
import rover.network.ControlPacket;
import rover.network.SocketInfo;
import rover.network.ArmPacket.ArmControlPacket;
import rover.network.ArmPacket.ArmOnOffPacket;
import rover.network.ArmPacket.ArmPositionPacket;
import rover.utils.RoverArmKinematics;
import rover.utils.Stopwatch;

import javax.swing.JDialog;
import javax.swing.JOptionPane;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.JButton;
import javax.swing.JTextField;
import javax.swing.JSlider;

public class ArmPortal extends Portal implements ActionListener, ChangeListener {

	private static final int MODE_DISABLED = 0;
	private static final int MODE_JOINT = 1;
	private static final int MODE_XYZ = 2;
	
	private static DecimalFormat df = new DecimalFormat("###.###");
	
	private int mode = 0;
	private ConfigurationModel model;
	private boolean threadKill = false;
	
	//Thread for listening for position and servo output updates
	Thread armListenThread;
	//Thread for periodically sending onoff packets and control packets
	Thread refreshThread;
	
	
	//Socket bound to the arm port
	MulticastSocket armSocket;  //  @jve:decl-index=0:
	SocketInfo asi;	
	private InetAddress arm_address;
	private int arm_port;
	Object armSocketLock;  //  @jve:decl-index=0:
	
	//Socket bound to servo port
	MulticastSocket servoSocket;
	SocketInfo lsi;	
	private InetAddress servo_address;
	private int servo_port;
	Object servoSocketLock;
	
	private boolean events_disabled = true;
	
	float[] joints;
	float[] mins;
	float[] maxs;
	float[] xyzpt;
	float[] motors;
	
	byte[] joint_channels;
	
	float toolValue;
	byte tool_channel;
	
	
	private JPanel jContentPane = null;
	private JPanel jointPanel = null;
	private JPanel xyzPanel = null;
	private JLabel jLabel = null;
	private JLabel jLabel1 = null;
	private JLabel jLabel2 = null;
	private JLabel jLabel3 = null;
	private JLabel jLabel4 = null;
	private JLabel jLabel6 = null;
	private JLabel jLabel7 = null;
	private JLabel jLabel8 = null;
	private JLabel jLabel9 = null;
	private JLabel jLabel10 = null;
	private JSpinner joint1Spinner = null;
	private JSpinner joint2Spinner = null;
	private JSpinner joint3Spinner = null;
	private JSpinner joint4Spinner = null;
	private JSpinner joint5Spinner = null;
	private JSpinner xSpinner = null;
	private JSpinner ySpinner = null;
	private JSpinner zSpinner = null;
	private JSpinner phiSpinner = null;
	private JSpinner thetaSpinner = null;
	private JPanel modePanel = null;
	private JRadioButton jointRadioButton = null;
	private JRadioButton xyzRadioButton = null;
	private JRadioButton disableRadioButton = null;
	private JPanel armPanel = null;
	private ArmSimulationPanel3D armPanel3D = null;
	private JPanel gainsPanel = null;
	private JScrollPane jScrollPane = null;
	private JTable jTable = null;
	private JButton sendGainsButton = null;
	private JButton loadButton = null;
	private JButton saveGainsButton = null;
	private JTextField j1TextField = null;
	private JTextField j2TextField = null;
	private JTextField j3TextField = null;
	private JTextField j4TextField = null;
	private JTextField j5TextField = null;
	private JPanel toolPanel = null;
	private JLabel jLabel11 = null;
	private JLabel jLabel12 = null;
	private JLabel jLabel13 = null;
	private JTextField j11TextField = null;
	private JTextField j22TextField = null;
	private JTextField j33TextField = null;
	private JTextField j44TextField = null;
	private JTextField j55TextField = null;
	private JSlider toolSlider = null;
	private JTextField jxTextField = null;
	private JTextField jyTextField = null;
	private JTextField jzTextField = null;
	private JTextField jphiTextField = null;
	private JTextField jthetaTextField = null;
	private JLabel jLabel14 = null;
	private JLabel jLabel15 = null;
	private JTextField toolTextField = null;
	public ArmPortal(Frame owner) {
		super(owner);
		model = new ConfigurationModel(5);
		initialize();
		
		joints = new float[5];
		xyzpt = new float[5];
		motors = new float[5];
		mins = new float[5];
		maxs = new float[5];
		
		for(int i = 0;i<5;i++){
			joints[i] = xyzpt[i] = motors[i] = 0;
		}
		
		joint_channels = new byte[5];
		joint_channels[0] = Byte.parseByte(Main.props.getProperty("j1_chan"));
		joint_channels[1] = Byte.parseByte(Main.props.getProperty("j2_chan"));
		joint_channels[2] = Byte.parseByte(Main.props.getProperty("j3_chan"));
		joint_channels[3] = Byte.parseByte(Main.props.getProperty("j4_chan"));
		joint_channels[4] = Byte.parseByte(Main.props.getProperty("j5_chan"));
		tool_channel = Byte.parseByte(Main.props.getProperty("tool_chan"));
		
		for(int i = 0;i<5;i++){
			model.data[i][1] = mins[i];
			model.data[i][2] = maxs[i];
			model.data[i][3] = joint_channels[i];
		}
		
		try{
			arm_address = InetAddress.getByName(Main.props.getProperty("arm_address"));
			arm_port = Integer.parseInt(Main.props.getProperty("arm_port"));
			
			servo_address = InetAddress.getByName(Main.props.getProperty("control_address"));
			servo_port = Integer.parseInt(Main.props.getProperty("control_port"));
		}catch (Exception e){
			e.printStackTrace();
			return;
		}

		servoSocketInit();
		armSocketInit();
		
		refreshThread = new Thread() { @Override public void run() {RefreshThreadStart();}};
		refreshThread.setDaemon(true);
		refreshThread.start();
		
		armListenThread = new Thread(){	@Override public void run() {ListenThreadStart();}};
		armListenThread.setDaemon(true);
		armListenThread.start();
	}

	/**
	 * This method initializes this
	 * 
	 */
	private void initialize() {
        this.setSize(new Dimension(951, 610));
        this.setTitle("Arm Control Portal");
        this.setContentPane(getJContentPane());
		
        setMode(MODE_DISABLED);
        events_disabled = false;
	}

	@Override
	public void Shutdown() {
		threadKill = true;
	}

	@Override
	public String toString(){
		return this.getTitle();
	}
	
	public static class ArmPortalInit extends PortalInit {
		@Override
		public Portal createPortal() {
			return new ArmPortal(MainGUI.instance);
		}
		
		@Override
		public String toString(){
			return "Arm Control Portal";
		}
	}
	
	private void armSocketInit() {
		try{

			armSocket = new MulticastSocket(arm_port);
			armSocket.setSoTimeout(40);
			armSocket.joinGroup(arm_address);
			armSocket.setTimeToLive(10);
			
			armSocketLock = new Object();
			
			asi = new SocketInfo();
			asi.setPortal("Arm Portal");
			asi.setPort(arm_port);
			asi.setType("MC Send/Rec");
			asi.setAddress(arm_address);
			
			MainGUI.registerSocket(asi);
		}catch(Exception e){
			e.printStackTrace();
		}
	}

	private void servoSocketInit() {
		try{

			servoSocket = new MulticastSocket(servo_port);
			servoSocket.joinGroup(servo_address);
			
			servoSocketLock = new Object();
			
			lsi = new SocketInfo();
			lsi.setPortal("Arm Portal");
			lsi.setPort(servo_port);
			lsi.setType("MC Rec");
			lsi.setAddress(servo_address);
			
			MainGUI.registerSocket(lsi);
		}catch(Exception e){
			e.printStackTrace();
		}
	}
	
	private void ListenThreadStart(){
		Stopwatch sw = new Stopwatch();
		ArmPacket.ArmPositionPacket app = new ArmPositionPacket(null, 0);
		ControlPacket cp = new ControlPacket();
		while(!threadKill){
			try{
				sw.Reset();
				sw.Start();
				int count = 0;
				//receive as many arm packets as you can in the time alloted
				synchronized(armSocketLock){
				while(sw.getElapsedMillis() < 50){
					byte[] buf = new byte[1000];
					DatagramPacket pack = new DatagramPacket(buf,buf.length);
					try{armSocket.receive(pack); }
					catch(SocketTimeoutException ste){
						break;
					}
					app.bytes = pack.getLength();
					app.packet = pack.getData();
					app.fillData();
					
					count++;
				}
				}
				//receive as many servo control packets as you can within the alloted time
				while(sw.getElapsedMillis() < 100){
					byte[] buf = new byte[1000];
					DatagramPacket pack = new DatagramPacket(buf,buf.length);
					try{servoSocket.receive(pack); }
					catch(SocketTimeoutException ste){
						break;
					}
					cp.bytes = pack.getLength();
					cp.packet = pack.getData();
					cp.FromByteArray(cp.packet, cp.bytes);
					
					
					
					count++;
					
				}
				
				if(count > 0){
					System.out.println(count + " Packet Received");
					PositionPacketReceived(app);
				}
				sw.Stop();
				Thread.sleep(Math.max(0, 100-(int)sw.getElapsedMillis()));
			}catch(Exception e){
				e.printStackTrace();
			}
		}
		
	}
	
	
	
	private void RefreshThreadStart(){
		ArmPacket.ArmOnOffPacket aofp = new ArmOnOffPacket();
		ArmPacket.ArmControlPacket acp = new ArmControlPacket((byte)5);
		while(!threadKill){
			try{
				synchronized(armSocketLock) {
					//send on off packet
					aofp.on = (mode > 0);
					aofp.ToByteArray();
					DatagramPacket pack = new DatagramPacket(aofp.packet, aofp.bytes,
							 arm_address, arm_port);
					System.out.println("Sending out ON=" + aofp.on);
					armSocket.send(pack);
					
					if(aofp.on){
						//send control packet
						acp.Channel_list = joint_channels;
						acp.Value_list = joints;
						acp.ToByteArray();
						
						pack = new DatagramPacket(acp.packet, acp.bytes,
								 arm_address, arm_port);
						System.out.println("Sending out joint command");
						armSocket.send(pack);
					}
				}
				Thread.sleep(500);
			}catch(Exception e){
				e.printStackTrace();
			}
		}
		
	}
	
	private void sendOnOff(){
		if(armSocketLock == null) return;
		ArmPacket.ArmOnOffPacket aofp = new ArmOnOffPacket();
		try{
			synchronized(armSocketLock) {
				//send on off packet
				aofp.on = (mode > 0);
				aofp.ToByteArray();
				DatagramPacket pack = new DatagramPacket(aofp.packet, aofp.bytes,
						 arm_address, arm_port);
				System.out.println("Sending out ON=" + aofp.on);
				armSocket.send(pack);
			}
		}catch(Exception e){
			e.printStackTrace();
		}
	}
	
	private void sendJointCommand(){
		if(armSocketLock == null || mode == 0) return;
		ArmPacket.ArmControlPacket acp = new ArmControlPacket((byte)5);
		try{
			synchronized(armSocketLock) {
				//send control packet
				acp.Channel_list = joint_channels;
				acp.Value_list = joints;
				acp.ToByteArray();
				
				DatagramPacket pack = new DatagramPacket(acp.packet, acp.bytes,
						 arm_address, arm_port);
				System.out.println("Sending out joint command");
				for(int i = 0;i<acp.Channels;i++){
					System.out.println("Joint " + acp.Channel_list[i] + " = " + acp.Value_list[i]);
				}
				armSocket.send(pack);
			}
		}catch(Exception e){
			e.printStackTrace();
		}
		
	}
	
	private void sendToolUpdate(){
		if(servoSocketLock == null || mode == 0) return;
		ControlPacket cp = new ControlPacket((byte)1);
		try{
			synchronized(servoSocketLock) {
				//send control packet
				cp.Channel_list[0] = tool_channel;
				cp.Value_list[0] = toolValue;
				cp.ToByteArray();
				
				DatagramPacket pack = new DatagramPacket(cp.packet, cp.bytes,
						 servo_address, servo_port);
				System.out.println("Sending out tool command");
				servoSocket.send(pack);
			}
		}catch(Exception e){
			e.printStackTrace();
		}
	}
	



	private void setEnabled(JPanel p, boolean enabled){
		for(Component c : p.getComponents()){
			c.setEnabled(enabled);
		}
	}
	
	private void setMode(int mode){
		this.mode = mode;
		switch(mode){
		
			case MODE_DISABLED:
				setEnabled(jointPanel, false);
				setEnabled(xyzPanel, false);
			break;
			case MODE_JOINT:
				setEnabled(jointPanel, true);
				setEnabled(xyzPanel, false);
			break;
			case MODE_XYZ:
				setEnabled(jointPanel, false);
				setEnabled(xyzPanel, true);
			break;
			default:
				return;
		}
		sendOnOff();
	}
	
	class ConfigurationModel extends AbstractTableModel {
	    /**
		 * 
		 */
		private static final long serialVersionUID = 1L;
		private String[] columnNames = {"Joint", "Lower Limit", "Upper Limit", "Channel", "P", "I", "D"};
	    private Object[][] data;
	    
	    public ConfigurationModel(int joints){
	    	data = new Object[joints][7];
	    	for(int i = 0;i<joints;i++){
	    		data[i][0] = new Integer(i+1);
	    		data[i][1] = new Float(-Math.PI);
	    		data[i][2] = new Float(Math.PI);
	    		data[i][3] = new Integer(-1);
	    		data[i][4] = new Float(1);
	    		data[i][5] = new Float(0);
	    		data[i][6] = new Float(0);
	    	}
	    }
	    
	    public int getColumnCount() { return columnNames.length;}
	    public int getRowCount() { return data.length;}
	    public String getColumnName(int col) {return columnNames[col];}

	    public Object getValueAt(int row, int col) {
	        return data[row][col];
	    }
	    public Class getColumnClass(int c) {
	        return getValueAt(0, c).getClass();
	    }
	    public boolean isCellEditable(int row, int col) {
	        if (col < 1) {
	            return false;
	        } else {
	            return true;
	        }
	    }
	    public void setValueAt(Object value, int row, int col) {
	        data[row][col] = value;
	        fireTableCellUpdated(row, col);
	    }

	}
	
	
	
	private void PositionPacketReceived(ArmPositionPacket app) {
		
		boolean worked = app.fillData();
		if(worked){
			getJ1TextField().setText(df.format(app.positions[0]));
			getJ2TextField().setText(df.format(app.positions[1]));
			getJ3TextField().setText(df.format(app.positions[2]));
			getJ4TextField().setText(df.format(app.positions[3]));
			getJ5TextField().setText(df.format(app.positions[4]));
		}
	}

	private void updateKinematics(){
		//System.out.println("Updating Kinematics");
		switch(mode){
			case MODE_DISABLED:
				return;
			case MODE_XYZ:
				invkin();
				break;
			case MODE_JOINT:
				fkin();
				break;
			default:
		}
		this.armPanel3D.setJoints(joints);
		sendJointCommand();
		
	}
	private float spinToFloat(JSpinner spinner){
		float val = 0;
		Object o = spinner.getValue();
		if(o.getClass() == Float.class){
			Float f = (Float)o;
			val = f.floatValue();
		} else if(o.getClass() == Double.class){
			Double d = (Double)o;
			val = d.floatValue();
		}else if(o.getClass() == Integer.class){
			Integer i = (Integer)o;
			val = i.floatValue();
		}
		return val;
	}
	
	private void fkin(){
		joints[0] = spinToFloat(joint1Spinner);
		joints[1] = spinToFloat(joint2Spinner);
		joints[2] = spinToFloat(joint3Spinner);
		joints[3] = spinToFloat(joint4Spinner);
		joints[4] = spinToFloat(joint5Spinner);
		
		xyzpt = RoverArmKinematics.fkin(joints);
		
		xSpinner.setValue(xyzpt[0]);
		ySpinner.setValue(xyzpt[1]);
		zSpinner.setValue(xyzpt[2]);
		phiSpinner.setValue(xyzpt[3]);
		thetaSpinner.setValue(xyzpt[4]);
	}
	
	private void invkin(){
		xyzpt[0] = spinToFloat(xSpinner);
		xyzpt[1] = spinToFloat(ySpinner);
		xyzpt[2] = spinToFloat(zSpinner);
		xyzpt[3] = spinToFloat(phiSpinner);
		xyzpt[4] = spinToFloat(thetaSpinner);
		
		RoverArmKinematics.invkinResult r = 
			RoverArmKinematics.inv_kin(xyzpt, mins, maxs);
		if(r.num_sol < 1){
			JOptionPane.showMessageDialog(this, "Inverse Kinematics Error (joint solution out of joint limits)");
			fkin();
			return;
		}
		joints = r.solutions[0];
		
		joint1Spinner.setValue(joints[0]);
		joint2Spinner.setValue(joints[1]);
		joint3Spinner.setValue(joints[2]);
		joint4Spinner.setValue(joints[3]);
		joint5Spinner.setValue(joints[4]);
	}
	
	
	/**
	 * This method initializes jContentPane	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getJContentPane() {
		if (jContentPane == null) {
			jContentPane = new JPanel();
			jContentPane.setLayout(null);
			jContentPane.add(getJointPanel(), null);
			jContentPane.add(getXyzPanel(), null);
			jContentPane.add(getModePanel(), null);
			jContentPane.add(getArmPanel(), null);
			jContentPane.add(getGainsPanel(), null);
			jContentPane.add(getToolPanel(), null);
		}
		return jContentPane;
	}

	/**
	 * This method initializes jointPanel	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getJointPanel() {
		if (jointPanel == null) {
			jLabel13 = new JLabel();
			jLabel13.setText("Motor Output");
			jLabel13.setLocation(new Point(230, 21));
			jLabel13.setSize(new Dimension(80, 17));
			jLabel12 = new JLabel();
			jLabel12.setText("Actual");
			jLabel12.setLocation(new Point(150, 21));
			jLabel12.setSize(new Dimension(69, 15));
			jLabel11 = new JLabel();
			jLabel11.setText("Desired");
			jLabel11.setLocation(new Point(80, 21));
			jLabel11.setSize(new Dimension(57, 13));
			jLabel4 = new JLabel();
			jLabel4.setText("Joint 5");
			jLabel4.setSize(new Dimension(50, 17));
			jLabel4.setLocation(new Point(16, 123));
			jLabel3 = new JLabel();
			jLabel3.setText("Joint 4");
			jLabel3.setSize(new Dimension(50, 17));
			jLabel3.setLocation(new Point(16, 103));
			jLabel2 = new JLabel();
			jLabel2.setText("Joint 3");
			jLabel2.setSize(new Dimension(50, 17));
			jLabel2.setLocation(new Point(16, 83));
			jLabel1 = new JLabel();
			jLabel1.setText("Joint 2");
			jLabel1.setSize(new Dimension(50, 17));
			jLabel1.setLocation(new Point(16, 63));
			jLabel = new JLabel();
			jLabel.setText("Joint 1");
			jLabel.setSize(new Dimension(50, 17));
			jLabel.setLocation(new Point(16, 43));
			jointPanel = new JPanel();
			jointPanel.setLayout(null);
			jointPanel.setBounds(new Rectangle(8, 58, 323, 153));
			jointPanel.setBorder(BorderFactory.createTitledBorder(null, "Joint Space", TitledBorder.DEFAULT_JUSTIFICATION, TitledBorder.DEFAULT_POSITION, new Font("Dialog", Font.BOLD, 12), new Color(51, 51, 51)));
			jointPanel.setEnabled(true);
			jointPanel.add(jLabel, null);
			jointPanel.add(jLabel1, null);
			jointPanel.add(jLabel2, null);
			jointPanel.add(jLabel3, null);
			jointPanel.add(jLabel4, null);
			jointPanel.add(getJoint1Spinner(), null);
			jointPanel.add(getJoint2Spinner(), null);
			jointPanel.add(getJoint3Spinner(), null);
			jointPanel.add(getJoint4Spinner(), null);
			jointPanel.add(getJoint5Spinner(), null);
			jointPanel.add(getJ1TextField(), null);
			jointPanel.add(getJ2TextField(), null);
			jointPanel.add(getJ3TextField(), null);
			jointPanel.add(getJ4TextField(), null);
			jointPanel.add(getJ5TextField(), null);
			jointPanel.add(jLabel11, null);
			jointPanel.add(jLabel12, null);
			jointPanel.add(jLabel13, null);
			jointPanel.add(getJ11TextField(), null);
			jointPanel.add(getJ22TextField(), null);
			jointPanel.add(getJ33TextField(), null);
			jointPanel.add(getJ44TextField(), null);
			jointPanel.add(getJ55TextField(), null);
		}
		return jointPanel;
	}

	/**
	 * This method initializes xyzPanel	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getXyzPanel() {
		if (xyzPanel == null) {
			jLabel15 = new JLabel();
			jLabel15.setBounds(new Rectangle(173, 19, 96, 18));
			jLabel15.setText("Actual");
			jLabel14 = new JLabel();
			jLabel14.setBounds(new Rectangle(70, 20, 76, 17));
			jLabel14.setText("Desired");
			jLabel10 = new JLabel();
			jLabel10.setText("TTheta:");
			jLabel10.setSize(new Dimension(50, 17));
			jLabel10.setLocation(new Point(8, 122));
			jLabel9 = new JLabel();
			jLabel9.setText("TPhi:");
			jLabel9.setSize(new Dimension(40, 17));
			jLabel9.setLocation(new Point(8, 102));
			jLabel8 = new JLabel();
			jLabel8.setText("Z:");
			jLabel8.setSize(new Dimension(40, 17));
			jLabel8.setLocation(new Point(8, 82));
			jLabel7 = new JLabel();
			jLabel7.setText("Y:");
			jLabel7.setSize(new Dimension(40, 17));
			jLabel7.setLocation(new Point(8, 62));
			jLabel6 = new JLabel();
			jLabel6.setText("X:");
			jLabel6.setSize(new Dimension(40, 17));
			jLabel6.setLocation(new Point(8, 42));
			xyzPanel = new JPanel();
			xyzPanel.setLayout(null);
			xyzPanel.setBounds(new Rectangle(7, 214, 323, 147));
			xyzPanel.setBorder(BorderFactory.createTitledBorder(null, "Cartesian Space", TitledBorder.DEFAULT_JUSTIFICATION, TitledBorder.DEFAULT_POSITION, new Font("Dialog", Font.BOLD, 12), new Color(51, 51, 51)));
			xyzPanel.setEnabled(true);
			xyzPanel.add(jLabel6, null);
			xyzPanel.add(jLabel7, null);
			xyzPanel.add(jLabel8, null);
			xyzPanel.add(jLabel9, null);
			xyzPanel.add(jLabel10, null);
			xyzPanel.add(getXSpinner(), null);
			xyzPanel.add(getYSpinner(), null);
			xyzPanel.add(getZSpinner(), null);
			xyzPanel.add(getPhiSpinner(), null);
			xyzPanel.add(getThetaSpinner(), null);
			xyzPanel.add(getJxTextField(), null);
			xyzPanel.add(getJyTextField(), null);
			xyzPanel.add(getJzTextField(), null);
			xyzPanel.add(getJphiTextField(), null);
			xyzPanel.add(getJthetaTextField(), null);
			xyzPanel.add(jLabel14, null);
			xyzPanel.add(jLabel15, null);
		}
		return xyzPanel;
	}

	/**
	 * This method initializes joint1Spinner	
	 * 	
	 * @return javax.swing.JSpinner	
	 */
	private JSpinner getJoint1Spinner() {
		if (joint1Spinner == null) {
			joint1Spinner = new JSpinner(new SpinnerNumberModel(0, -Math.PI/2, Math.PI/2, .01));
			//joint1Spinner = new FloatSpinner(0,(float)(-Math.PI/2), (float)(Math.PI/2), .01f);
			joint1Spinner.setLocation(new Point(81, 43));
			joint1Spinner.setSize(new Dimension(60, 17));
			joint1Spinner.addChangeListener(this);
		}
		return joint1Spinner;
	}

	/**
	 * This method initializes joint2Spinner	
	 * 	
	 * @return javax.swing.JSpinner	
	 */
	private JSpinner getJoint2Spinner() {
		if (joint2Spinner == null) {
			joint2Spinner = new JSpinner(new SpinnerNumberModel(0, -Math.PI/2, Math.PI/2, .01));
			joint2Spinner.setLocation(new Point(81, 63));
			joint2Spinner.setSize(new Dimension(60, 17));
			joint2Spinner.addChangeListener(this);
		}
		return joint2Spinner;
	}

	/**
	 * This method initializes joint3Spinner	
	 * 	
	 * @return javax.swing.JSpinner	
	 */
	private JSpinner getJoint3Spinner() {
		if (joint3Spinner == null) {
			joint3Spinner = new JSpinner(new SpinnerNumberModel(0, -Math.PI/2, Math.PI/2, .01));
			joint3Spinner.setLocation(new Point(81, 83));
			joint3Spinner.setSize(new Dimension(60, 17));
			joint3Spinner.addChangeListener(this);
		}
		return joint3Spinner;
	}

	/**
	 * This method initializes joint4Spinner	
	 * 	
	 * @return javax.swing.JSpinner	
	 */
	private JSpinner getJoint4Spinner() {
		if (joint4Spinner == null) {
			joint4Spinner = new JSpinner(new SpinnerNumberModel(0, -Math.PI/2, Math.PI/2, .01));
			joint4Spinner.setLocation(new Point(81, 103));
			joint4Spinner.setSize(new Dimension(60, 17));
			joint4Spinner.addChangeListener(this);
		}
		return joint4Spinner;
	}

	/**
	 * This method initializes joint5Spinner	
	 * 	
	 * @return javax.swing.JSpinner	
	 */
	private JSpinner getJoint5Spinner() {
		if (joint5Spinner == null) {
			joint5Spinner = new JSpinner(new SpinnerNumberModel(0, -Math.PI/2, Math.PI/2, .01));
			joint5Spinner.setLocation(new Point(81, 123));
			joint5Spinner.setSize(new Dimension(60, 17));
			joint5Spinner.addChangeListener(this);
		}
		return joint5Spinner;
	}

	/**
	 * This method initializes xSpinner	
	 * 	
	 * @return javax.swing.JSpinner	
	 */
	private JSpinner getXSpinner() {
		if (xSpinner == null) {
			xSpinner = new JSpinner();
			xSpinner.setSize(new Dimension(80, 17));
			xSpinner.setLocation(new Point(68, 42));
			xSpinner.addChangeListener(this);
		}
		return xSpinner;
	}

	/**
	 * This method initializes ySpinner	
	 * 	
	 * @return javax.swing.JSpinner	
	 */
	private JSpinner getYSpinner() {
		if (ySpinner == null) {
			ySpinner = new JSpinner();
			ySpinner.setSize(new Dimension(80, 17));
			ySpinner.setLocation(new Point(68, 62));
			ySpinner.addChangeListener(this);
		}
		return ySpinner;
	}

	/**
	 * This method initializes zSpinner	
	 * 	
	 * @return javax.swing.JSpinner	
	 */
	private JSpinner getZSpinner() {
		if (zSpinner == null) {
			zSpinner = new JSpinner();
			zSpinner.setSize(new Dimension(80, 17));
			zSpinner.setLocation(new Point(68, 82));
			zSpinner.addChangeListener(this);
		}
		return zSpinner;
	}

	/**
	 * This method initializes phiSpinner	
	 * 	
	 * @return javax.swing.JSpinner	
	 */
	private JSpinner getPhiSpinner() {
		if (phiSpinner == null) {
			phiSpinner = new JSpinner();
			phiSpinner.setSize(new Dimension(80, 17));
			phiSpinner.setLocation(new Point(68, 102));
			phiSpinner.addChangeListener(this);
		}
		return phiSpinner;
	}

	/**
	 * This method initializes thetaSpinner	
	 * 	
	 * @return javax.swing.JSpinner	
	 */
	private JSpinner getThetaSpinner() {
		if (thetaSpinner == null) {
			thetaSpinner = new JSpinner();
			thetaSpinner.setSize(new Dimension(80, 17));
			thetaSpinner.setLocation(new Point(68, 122));
			thetaSpinner.addChangeListener(this);
		}
		return thetaSpinner;
	}

	/**
	 * This method initializes modePanel	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getModePanel() {
		if (modePanel == null) {
			modePanel = new JPanel();
			modePanel.setLayout(null);
			modePanel.setBounds(new Rectangle(9, 9, 322, 44));
			modePanel.setBorder(BorderFactory.createTitledBorder(null, "Mode", TitledBorder.DEFAULT_JUSTIFICATION, TitledBorder.DEFAULT_POSITION, new Font("Dialog", Font.BOLD, 12), new Color(51, 51, 51)));
			modePanel.add(getJointRadioButton(), null);
			modePanel.add(getXyzRadioButton(), null);
			modePanel.add(getDisableRadioButton(), null);
			
			ButtonGroup group = new ButtonGroup();
		    group.add(getJointRadioButton());
		    group.add(getXyzRadioButton());
		    group.add(getDisableRadioButton());
			
		}
		return modePanel;
	}

	/**
	 * This method initializes jointRadioButton	
	 * 	
	 * @return javax.swing.JRadioButton	
	 */
	private JRadioButton getJointRadioButton() {
		if (jointRadioButton == null) {
			jointRadioButton = new JRadioButton();
			jointRadioButton.setBounds(new Rectangle(87, 12, 102, 25));
			jointRadioButton.setText("Joint Control");
			jointRadioButton.addActionListener(this);
		}
		return jointRadioButton;
	}

	/**
	 * This method initializes xyzRadioButton	
	 * 	
	 * @return javax.swing.JRadioButton	
	 */
	private JRadioButton getXyzRadioButton() {
		if (xyzRadioButton == null) {
			xyzRadioButton = new JRadioButton();
			xyzRadioButton.setBounds(new Rectangle(187, 13, 130, 22));
			xyzRadioButton.setText("Cartesian Control");
			xyzRadioButton.addActionListener(this);
		}
		return xyzRadioButton;
	}

	/**
	 * This method initializes disableRadioButton	
	 * 	
	 * @return javax.swing.JRadioButton	
	 */
	private JRadioButton getDisableRadioButton() {
		if (disableRadioButton == null) {
			disableRadioButton = new JRadioButton();
			disableRadioButton.setBounds(new Rectangle(9, 15, 79, 21));
			disableRadioButton.setSelected(true);
			disableRadioButton.setText("Disabled");
			disableRadioButton.addActionListener(this);
		}
		return disableRadioButton;
	}

	@Override
	public void actionPerformed(ActionEvent arg0) {
		Object sender = arg0.getSource();
		if(sender == disableRadioButton){
			setMode(MODE_DISABLED);
		}else if(sender == xyzRadioButton){
			setMode(MODE_XYZ);
		}else if(sender == jointRadioButton){
			setMode(MODE_JOINT);
		}else{
			
		}
		
	}

	/**
	 * This method initializes armPanel	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getArmPanel() {
		if (armPanel == null) {
			armPanel = new JPanel();
			armPanel.setLayout(null);
			armPanel.setBounds(new Rectangle(425, 9, 512, 568));
			armPanel.setBorder(BorderFactory.createTitledBorder(null, "Arm Simulation", TitledBorder.DEFAULT_JUSTIFICATION, TitledBorder.DEFAULT_POSITION, new Font("Dialog", Font.BOLD, 12), new Color(51, 51, 51)));
			armPanel.add(getArmPanel3D(), null);
		}
		return armPanel;
	}

	/**
	 * This method initializes armPanel3D	
	 * 	
	 * @return rover.components.ArmPanel3D	
	 */
	private ArmSimulationPanel3D getArmPanel3D() {
		if (armPanel3D == null) {
			armPanel3D = new ArmSimulationPanel3D();
			armPanel3D.setBounds(new Rectangle(8, 17, 496, 541));
		}
		return armPanel3D;
	}

	/**
	 * This method initializes gainsPanel	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getGainsPanel() {
		if (gainsPanel == null) {
			gainsPanel = new JPanel();
			gainsPanel.setLayout(null);
			gainsPanel.setBounds(new Rectangle(7, 365, 412, 213));
			gainsPanel.setBorder(BorderFactory.createTitledBorder(null, "Joint Configuration", TitledBorder.DEFAULT_JUSTIFICATION, TitledBorder.DEFAULT_POSITION, new Font("Dialog", Font.BOLD, 12), new Color(51, 51, 51)));
			gainsPanel.add(getJScrollPane(), null);
			gainsPanel.add(getSendGainsButton(), null);
			gainsPanel.add(getLoadButton(), null);
			gainsPanel.add(getSaveGainsButton(), null);
		}
		return gainsPanel;
	}

	/**
	 * This method initializes jScrollPane	
	 * 	
	 * @return javax.swing.JScrollPane	
	 */
	private JScrollPane getJScrollPane() {
		if (jScrollPane == null) {
			jScrollPane = new JScrollPane();
			jScrollPane.setBounds(new Rectangle(11, 22, 389, 147));
			jScrollPane.setViewportView(getJTable());
		}
		return jScrollPane;
	}

	/**
	 * This method initializes jTable	
	 * 	
	 * @return javax.swing.JTable	
	 */
	private JTable getJTable() {
		if (jTable == null) {
			/*jTable = new JTable();*/
			jTable = new JTable(model);
			jTable.getColumnModel().getColumn(0).setPreferredWidth(20);
		    jTable.getColumnModel().getColumn(1).setPreferredWidth(40);
		    jTable.getColumnModel().getColumn(2).setPreferredWidth(40);
		    jTable.getColumnModel().getColumn(3).setPreferredWidth(30);
		    jTable.getColumnModel().getColumn(4).setPreferredWidth(20);
		    jTable.getColumnModel().getColumn(5).setPreferredWidth(20);
		    jTable.getColumnModel().getColumn(6).setPreferredWidth(20);
		}
		return jTable;
	}

	/**
	 * This method initializes sendGainsButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getSendGainsButton() {
		if (sendGainsButton == null) {
			sendGainsButton = new JButton();
			sendGainsButton.setBounds(new Rectangle(12, 178, 68, 28));
			sendGainsButton.setText("Send");
			sendGainsButton.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					System.out.println("actionPerformed()"); // TODO Auto-generated Event stub actionPerformed()
				}
			});
		}
		return sendGainsButton;
	}

	/**
	 * This method initializes loadButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getLoadButton() {
		if (loadButton == null) {
			loadButton = new JButton();
			loadButton.setBounds(new Rectangle(86, 179, 66, 28));
			loadButton.setText("Load");
		}
		return loadButton;
	}

	/**
	 * This method initializes saveGainsButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getSaveGainsButton() {
		if (saveGainsButton == null) {
			saveGainsButton = new JButton();
			saveGainsButton.setBounds(new Rectangle(157, 177, 73, 29));
			saveGainsButton.setText("Save");
		}
		return saveGainsButton;
	}

	/**
	 * This method initializes j1TextField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	private JTextField getJ1TextField() {
		if (j1TextField == null) {
			j1TextField = new JTextField();
			j1TextField.setSize(new Dimension(70, 17));
			j1TextField.setEditable(false);
			j1TextField.setLocation(new Point(150, 43));
		}
		return j1TextField;
	}

	/**
	 * This method initializes j2TextField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	private JTextField getJ2TextField() {
		if (j2TextField == null) {
			j2TextField = new JTextField();
			j2TextField.setSize(new Dimension(70, 17));
			j2TextField.setEditable(false);
			j2TextField.setLocation(new Point(150, 63));
		}
		return j2TextField;
	}

	/**
	 * This method initializes j3TextField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	private JTextField getJ3TextField() {
		if (j3TextField == null) {
			j3TextField = new JTextField();
			j3TextField.setLocation(new Point(150, 83));
			j3TextField.setEditable(false);
			j3TextField.setSize(new Dimension(70, 17));
		}
		return j3TextField;
	}

	/**
	 * This method initializes j4TextField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	private JTextField getJ4TextField() {
		if (j4TextField == null) {
			j4TextField = new JTextField();
			j4TextField.setLocation(new Point(150, 103));
			j4TextField.setEditable(false);
			j4TextField.setSize(new Dimension(70, 17));
		}
		return j4TextField;
	}

	/**
	 * This method initializes j5TextField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	private JTextField getJ5TextField() {
		if (j5TextField == null) {
			j5TextField = new JTextField();
			j5TextField.setLocation(new Point(150, 123));
			j5TextField.setEditable(false);
			j5TextField.setSize(new Dimension(70, 17));
		}
		return j5TextField;
	}

	/**
	 * This method initializes toolPanel	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getToolPanel() {
		if (toolPanel == null) {
			toolPanel = new JPanel();
			toolPanel.setLayout(null);
			toolPanel.setBounds(new Rectangle(336, 9, 81, 352));
			toolPanel.setBorder(BorderFactory.createTitledBorder(null, "Tool Control", TitledBorder.DEFAULT_JUSTIFICATION, TitledBorder.DEFAULT_POSITION, new Font("Dialog", Font.BOLD, 12), new Color(51, 51, 51)));
			toolPanel.add(getToolSlider(), null);
			toolPanel.add(getToolTextField(), null);
		}
		return toolPanel;
	}

	/**
	 * This method initializes j11TextField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	private JTextField getJ11TextField() {
		if (j11TextField == null) {
			j11TextField = new JTextField();
			j11TextField.setLocation(new Point(230, 43));
			j11TextField.setEditable(false);
			j11TextField.setSize(new Dimension(70, 17));
		}
		return j11TextField;
	}

	/**
	 * This method initializes j22TextField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	private JTextField getJ22TextField() {
		if (j22TextField == null) {
			j22TextField = new JTextField();
			j22TextField.setLocation(new Point(230, 63));
			j22TextField.setEditable(false);
			j22TextField.setSize(new Dimension(70, 17));
		}
		return j22TextField;
	}

	/**
	 * This method initializes j33TextField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	private JTextField getJ33TextField() {
		if (j33TextField == null) {
			j33TextField = new JTextField();
			j33TextField.setLocation(new Point(230, 83));
			j33TextField.setEditable(false);
			j33TextField.setSize(new Dimension(70, 17));
		}
		return j33TextField;
	}

	/**
	 * This method initializes j44TextField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	private JTextField getJ44TextField() {
		if (j44TextField == null) {
			j44TextField = new JTextField();
			j44TextField.setLocation(new Point(230, 103));
			j44TextField.setEditable(false);
			j44TextField.setSize(new Dimension(70, 17));
		}
		return j44TextField;
	}

	/**
	 * This method initializes j55TextField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	private JTextField getJ55TextField() {
		if (j55TextField == null) {
			j55TextField = new JTextField();
			j55TextField.setLocation(new Point(230, 123));
			j55TextField.setEditable(false);
			j55TextField.setSize(new Dimension(70, 17));
		}
		return j55TextField;
	}

	/**
	 * This method initializes toolSlider	
	 * 	
	 * @return javax.swing.JSlider	
	 */
	private JSlider getToolSlider() {
		if (toolSlider == null) {
			toolSlider = new JSlider();
			toolSlider.setBounds(new Rectangle(12, 41, 46, 300));
			toolSlider.setMinimum(0);
			toolSlider.setMaximum(10000);
			toolSlider.setValue(5000);
			toolSlider.setMinorTickSpacing(100);
			toolSlider.setMajorTickSpacing(1000);
			toolSlider.setPaintTicks(true);
			toolSlider.setPaintLabels(false);
			toolSlider.setSnapToTicks(true);
			toolSlider.setOrientation(JSlider.VERTICAL);
			toolSlider.addChangeListener(new javax.swing.event.ChangeListener() {
				public void stateChanged(javax.swing.event.ChangeEvent e) {
					Float f = new Float(toolSlider.getValue());
					f/=100;
					toolTextField.setText(f.toString());
					toolValue = f;
					sendToolUpdate();
				}
			});
		}
		return toolSlider;
	}

	/**
	 * This method initializes jxTextField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	private JTextField getJxTextField() {
		if (jxTextField == null) {
			jxTextField = new JTextField();
			jxTextField.setSize(new Dimension(100, 17));
			jxTextField.setEditable(false);
			jxTextField.setLocation(new Point(173, 43));
		}
		return jxTextField;
	}

	/**
	 * This method initializes jyTextField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	private JTextField getJyTextField() {
		if (jyTextField == null) {
			jyTextField = new JTextField();
			jyTextField.setSize(new Dimension(100, 17));
			jyTextField.setEditable(false);
			jyTextField.setLocation(new Point(173, 63));
		}
		return jyTextField;
	}

	/**
	 * This method initializes jzTextField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	private JTextField getJzTextField() {
		if (jzTextField == null) {
			jzTextField = new JTextField();
			jzTextField.setSize(new Dimension(100, 17));
			jzTextField.setEditable(false);
			jzTextField.setLocation(new Point(173, 83));
		}
		return jzTextField;
	}

	/**
	 * This method initializes jphiTextField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	private JTextField getJphiTextField() {
		if (jphiTextField == null) {
			jphiTextField = new JTextField();
			jphiTextField.setSize(new Dimension(100, 17));
			jphiTextField.setEditable(false);
			jphiTextField.setLocation(new Point(173, 103));
		}
		return jphiTextField;
	}

	/**
	 * This method initializes jthetaTextField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	private JTextField getJthetaTextField() {
		if (jthetaTextField == null) {
			jthetaTextField = new JTextField();
			jthetaTextField.setSize(new Dimension(100, 17));
			jthetaTextField.setEditable(false);
			jthetaTextField.setLocation(new Point(173, 123));
		}
		return jthetaTextField;
	}

	/**
	 * This method initializes toolTextField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	private JTextField getToolTextField() {
		if (toolTextField == null) {
			toolTextField = new JTextField();
			toolTextField.setBounds(new Rectangle(9, 22, 61, 19));
			toolTextField.setText("50.0");
			toolTextField.setEditable(false);
		}
		return toolTextField;
	}

	@Override
	public void stateChanged(ChangeEvent arg0) {
		
		if(events_disabled) return;
		events_disabled = true;
		updateKinematics();
		events_disabled = false;
	}
	
}  //  @jve:decl-index=0:visual-constraint="10,10"
