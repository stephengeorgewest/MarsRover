package rover.portal;


import java.awt.Frame;

import javax.swing.JPanel;
import net.java.games.input.*;
import java.awt.Dimension;
import javax.swing.JLabel;
import java.awt.Rectangle;
import javax.swing.JTextField;
import javax.swing.JComboBox;

import javax.swing.SpinnerNumberModel;


import javax.swing.BorderFactory;
import javax.swing.border.TitledBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.table.AbstractTableModel;
import javax.swing.table.TableColumn;


import java.awt.Font;
import java.awt.Color;
import java.awt.GridBagConstraints;
import java.net.DatagramPacket;
import java.net.InetAddress;
import java.net.MulticastSocket;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.Random;

import rover.Main;
import rover.components.RoverPanel2D;
import rover.input.*;
import rover.network.ControlPacket;
import rover.network.SocketInfo;
import rover.utils.Stopwatch;

import javax.swing.ImageIcon;
import javax.swing.JSpinner;
import java.awt.Point;
import javax.swing.JTable;
import javax.swing.JScrollPane;
import javax.swing.JButton;

public class ControllerPortal extends Portal{

	private static final long serialVersionUID = 1L;
	private JPanel jContentPane = null;
	private JLabel jLabel = null;
	private JTextField controllerStatusField = null;
	private JComboBox jComboBox = null;
	private JPanel jPanel = null;
	private RoverPanel2D roverPanel2D = null;
	private JLabel jLabel1 = null;
	private JLabel statusLightLabel = null;
	private JSpinner ppsSpinner = null;
	private JLabel ppsLabel = null;
	private JLabel jLabel2 = null;
	
	private Thread inputThread;
	private boolean inputThreadKill = false;
	private Object inputLock = null;
	private InputDevice inputdevice;
	private int control_rate;

	
	ImageIcon red;
	ImageIcon green;
	ImageIcon yellow;
	private JLabel jLabel3 = null;
	private JPanel jPanel2 = null;
	private JPanel jPanel3 = null;
	private JTable jTable = null;
	private JScrollPane jScrollPane = null;
	private JLabel jLabel4 = null;
	private JLabel jLabel5 = null;
	private JLabel jLabel6 = null;
	private JLabel jLabel7 = null;
	private JLabel jLabel8 = null;
	private JLabel jLabel9 = null;
	private JLabel jLabel10 = null;
	private JLabel jLabel11 = null;
	private JLabel LFLabel = null;
	private JLabel RFLabel = null;
	private JLabel LRLabel = null;
	private JLabel RRLabel = null;
	private JLabel GLabel = null;
	private JButton applyButton = null;
	
	private CalibrationModel calib = null;
	
	private byte[] channels;
	private float[][] calibration;
	
	SocketInfo si = null;
	MulticastSocket socket;  //  @jve:decl-index=0:
	private InetAddress mc_address;  //  @jve:decl-index=0:
	private int mc_port;
	private int s_port;
	
	
	class CalibrationModel extends AbstractTableModel {
	    /**
		 * 
		 */
		private static final long serialVersionUID = 1L;
		private String[] columnNames = {"Name", "Channel", "Min", "Zero", "Max"};
	    private Object[][] data = {{"LF", new Integer(0), new Double(0), new Double(50), new Double(50)},
	    		{"RF", new Integer(1), new Double(0), new Double(50), new Double(100)},
	    		{"LR", new Integer(2), new Double(0), new Double(50), new Double(100)},
	    		{"RR", new Integer(3), new Double(0), new Double(50), new Double(100)},
	    		{"Gimbal", new Integer(4), new Double(0), new Double(50), new Double(100)},
	    	   };


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

	public static class ControllerPortalInit extends PortalInit {
		@Override
		public Portal createPortal() {
			return new ControllerPortal(MainGUI.instance);
		}
		
		@Override
		public String toString(){
			return "Controller Portal";
		}
	}
	
	/**
	 * @param owner
	 */
	public ControllerPortal(Frame owner) {
		super(owner);
		initSocket();
		
		red = new ImageIcon(getClass().getResource("/redlight.gif"));
		green = new ImageIcon(getClass().getResource("/greenlight.gif"));
		yellow = new ImageIcon(getClass().getResource("/yellowlight.gif"));

		control_rate = 20;
		
		channels = new byte[] {0,1,2,3,10};
		calibration = new float[][]{{0,50,100},{0,50,100}, {0,50,100}, {0,50,100}, {0,50,100}};
		
		calib = new CalibrationModel();
		WriteCalibrationToTable();
		
		initialize();
		

	    jComboBox.addItem("Disable Driving");
	    jComboBox.addItem("Single Joystick Driving");
	    jComboBox.addItem("Double Joystick Driving");
	    jComboBox.setSelectedIndex(0);;
	    inputdevice = null;
		
		inputThread = new Thread(){	@Override public void run() {inputThreadStart();}};
		inputThread.setDaemon(true);
		inputLock = new Object();
		inputThread.start();
	}

	private void initSocket(){

		try{
			mc_address = InetAddress.getByName(Main.props.getProperty("control_address"));
			mc_port = Integer.parseInt(Main.props.getProperty("control_port"));
		
			socket = new MulticastSocket();
			socket.joinGroup(mc_address);
			s_port = socket.getLocalPort();
			
			si = new SocketInfo();
			si.setPortal("Controller Portal");
			si.setPort(s_port);
			si.setType("MC Send");
			si.setAddress(mc_address);
			
			MainGUI.registerSocket(si);
		}catch(Exception e){
			e.printStackTrace();
		}
	}
	
	private void sendControlPacket(byte[] channels, float[] values){
		try{
			ControlPacket cp = new ControlPacket(channels, values);
		
			cp.ToByteArray();
			DatagramPacket pack = new DatagramPacket(cp.packet, cp.bytes,
					 mc_address, mc_port);
			socket.send(pack);
		}catch(Exception e){
			e.printStackTrace();
		}
	}
	
	public void ReadCalibrationFromTable(){
		for(int i = 0;i<5;i++){
			Integer chan = (Integer) calib.getValueAt(i, 1);
			byte ch = chan.byteValue();
			channels[i] = ch;
		}
		
		for(int i = 0;i<5;i++){
			calibration[i][0] = (Float) calib.getValueAt(i, 2);
			calibration[i][1] = (Float) calib.getValueAt(i, 3);
			calibration[i][2] = (Float) calib.getValueAt(i, 4);
		}
		
	}
	
	public void WriteCalibrationToTable(){
		for(int i = 0;i<5;i++){
			calib.setValueAt(new Integer(channels[i]),i, 1);
		}
		
		for(int i = 0;i<5;i++){
			calib.setValueAt(new Float(calibration[i][0]), i, 2);
			calib.setValueAt(new Float(calibration[i][1]), i, 3);
			calib.setValueAt(new Float(calibration[i][2]), i, 4);
		}
	}
	
	private void inputThreadStart()
    {
        int disp_count = 0;
        int disp_rate = 5;
        Stopwatch input_sw = new Stopwatch();
        Stopwatch sw = new Stopwatch();
        sw.Start();
        
        DecimalFormat df = new DecimalFormat("###.##");
        
        Random generator = new Random( 19580427 );
        
        while (!inputThreadKill)
        {
            int sleep_time = 500;

                try
                {
                    synchronized (inputLock){
                    
                	 if (inputdevice == null) //device must have been switched
                     {
                		 System.out.println("Aquiring Device");
                        //initialize device based on which device is selected
                        switch (jComboBox.getSelectedIndex())
                        {
                            case 0: //no input device
                                inputdevice = new NoInput();
                                break;
                            case 1: //single 4 axis joystick
                                inputdevice = new SingleJoystickDevice();
                                inputdevice.AquireDevice();
                                break;
                            case 2: //double joystick
                                inputdevice = new DoubleJoystickDevice();
                                inputdevice.AquireDevice();
                                break;
                        }
                        this.statusLightLabel.setIcon(red);
                        if(inputdevice != null) controllerStatusField.setText(inputdevice.getStatus());
                        sleep_time = 200; //we don't want to be trying to discover new devices at full frequency
                    }
                    else if (!inputdevice.isConnected()) //try to reconnect
                    {
                        inputdevice.AquireDevice();
                        this.statusLightLabel.setIcon(red);
                        controllerStatusField.setText(inputdevice.getStatus());
                    }else
                    {
                        int target_period = 1000 / control_rate;
                        int display_rate = Math.max(control_rate/disp_rate, 1);
                        
                        if(statusLightLabel.getIcon() != green) statusLightLabel.setIcon(green);
                        
                        //read from the device and send off a udp control packet
                        
 
                        //read status data from device
                        JoystickState state = inputdevice.getState();
                        
                        float head_angle = -90*state.axes[4];
                        roverPanel2D.setHead_Angle(head_angle);
                        roverPanel2D.setThrottles(state.axes);
                        
                        roverPanel2D.repaint();
                        
                        float[] values = new float[5];
                        float LF, RF, LR, RR, Head;
                        
                        LF = InputUtils.scaleByCalibration(calibration[0][0], calibration[0][1], calibration[0][2], 50+50*state.axes[0]);
                        RF = InputUtils.scaleByCalibration(calibration[1][0], calibration[1][1], calibration[1][2], 50+50*state.axes[1]);
                        LR = InputUtils.scaleByCalibration(calibration[2][0], calibration[2][1], calibration[2][2], 50+50*state.axes[2]);
                        RR = InputUtils.scaleByCalibration(calibration[3][0], calibration[3][1], calibration[3][2], 50+50*state.axes[3]);
                        Head  = InputUtils.scaleByCalibration(calibration[3][0], calibration[3][1], calibration[3][2], 50+50*state.axes[4]);
                        
                        this.LFLabel.setText(df.format(LF));
                        this.RFLabel.setText(df.format(RF));
                        this.LRLabel.setText(df.format(LR));
                        this.RRLabel.setText(df.format(RR));
                        this.GLabel.setText(df.format(head_angle));
                        
                        //InputState s = inputdevice.State;
                        //Console.WriteLine(s.roll + " " + s.pitch + " " + s.yaw_rate + " " + s.thrust_level);

                        float max_rate = 1;
                        //TODO Send UDP control packet
                        values[0] = LF;
                        values[1] = RF;
                        values[2] = LR;
                        values[3] = RR;
                        values[4] = Head;
                        
                        sendControlPacket(channels, values);
                        
                        sw.Stop();
                        sleep_time = Math.max((int)(target_period-sw.getElapsedMillis()), 0);
                        sw.Reset();

                        disp_count++;
                        //Console.WriteLine(disp_count);
                        if (disp_count % display_rate == 0)
                        {
                            disp_count = 0;
                            //pps calculator
                            input_sw.Stop();
                            double sec = input_sw.getElapsedSeconds();
                            double pps = (display_rate) / (sec);
                            
                            this.controllerStatusField.setText(inputdevice.getStatus());
                            this.ppsLabel.setText(df.format(pps));
                            if(pps < .5*control_rate) this.statusLightLabel.setIcon(yellow);
                            else this.statusLightLabel.setIcon(green);
                            //System.out.println("Updating pps");
                            
                            input_sw.Reset();
                            input_sw.Start();
                        }



                    }
                    }
                    
                    Thread.sleep(sleep_time);
                    sw.Start();
                    
                }
                catch (Exception e)
                {
                	e.printStackTrace();
                }
            
            
        }

    }
	
	@Override
	public void Shutdown() {
		// TODO Auto-generated method stub
			inputThreadKill = true;
			if(si != null) MainGUI.instance.unregisterSocket(si);
	}
	
	@Override
	public String toString(){
		return this.getTitle();
	}
	/**
	 * This method initializes this
	 * 
	 * @return void
	 */
	private void initialize() {
		this.setSize(733, 319);
		this.setContentPane(getJContentPane());
		this.setTitle("Controller Portal");
	}

	/**
	 * This method initializes jContentPane
	 * 
	 * @return javax.swing.JPanel
	 */
	private JPanel getJContentPane() {
		if (jContentPane == null) {
			jLabel3 = new JLabel();
			jLabel3.setText("Select Control Style:");
			jLabel3.setBounds(new Rectangle(10, 26, 119, 24));
			jLabel2 = new JLabel();
			jLabel2.setText("Desired:");
			jLabel2.setBounds(new Rectangle(212, 88, 61, 14));
			ppsLabel = new JLabel();
			ppsLabel.setText("0");
			ppsLabel.setBounds(new Rectangle(140, 88, 61, 18));
			statusLightLabel = new JLabel();
			statusLightLabel.setIcon(new ImageIcon(getClass().getResource("/redlight.gif")));
			statusLightLabel.setBounds(new Rectangle(370, 29, 43, 44));
			statusLightLabel.setText("");
			jLabel1 = new JLabel();
			jLabel1.setText("Actual Packets/s");
			jLabel1.setBounds(new Rectangle(10, 88, 115, 19));
			jLabel = new JLabel();
			jLabel.setText("Controller Status:");
			jLabel.setBounds(new Rectangle(11, 57, 114, 20));
			jContentPane = new JPanel();
			jContentPane.setLayout(null);
			jContentPane.add(getJPanel(), null);
			jContentPane.add(getJPanel2(), null);
			jContentPane.add(getJPanel3(), null);
		}
		return jContentPane;
	}

	/**
	 * This method initializes controllerStatusField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	private JTextField getControllerStatusField() {
		if (controllerStatusField == null) {
			controllerStatusField = new JTextField();
			controllerStatusField.setEditable(false);
			controllerStatusField.setBounds(new Rectangle(139, 53, 216, 27));
		}
		return controllerStatusField;
	}

	/**
	 * This method initializes jComboBox	
	 * 	
	 * @return javax.swing.JComboBox	
	 */
	private JComboBox getJComboBox() {
		if (jComboBox == null) {
			jComboBox = new JComboBox();
			jComboBox.setBounds(new Rectangle(139, 27, 217, 21));
			jComboBox.addItemListener(new java.awt.event.ItemListener() {
				public void itemStateChanged(java.awt.event.ItemEvent e) {
					inputdevice = null;
				}
			});
		}
		return jComboBox;
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
			jPanel.setBounds(new Rectangle(435, 0, 291, 287));
			jPanel.setBorder(BorderFactory.createTitledBorder(null, "Rover View", TitledBorder.DEFAULT_JUSTIFICATION, TitledBorder.DEFAULT_POSITION, new Font("Dialog", Font.BOLD, 12), new Color(51, 51, 51)));
			jPanel.add(getRoverPanel2D(), null);
		}
		return jPanel;
	}

	/**
	 * This method initializes roverPanel2D	
	 * 	
	 * @return rover.components.RoverPanel2D	
	 */
	private RoverPanel2D getRoverPanel2D() {
		if (roverPanel2D == null) {
			roverPanel2D = new RoverPanel2D();
			roverPanel2D.setBounds(new Rectangle(6, 18, 279, 259));
			roverPanel2D.setPreferredSize(new Dimension(250, 250));
		}
		return roverPanel2D;
	}

	/**
	 * This method initializes ppsSpinner	
	 * 	
	 * @return javax.swing.JSpinner	
	 */
	private JSpinner getPpsSpinner() {
		if (ppsSpinner == null) {
			ppsSpinner = new JSpinner(new SpinnerNumberModel(control_rate, 1, 200, 1));
			ppsSpinner.setBounds(new Rectangle(280, 87, 75, 18));
			ppsSpinner.addChangeListener(new ChangeListener(){
				public void stateChanged(ChangeEvent e)
				{
					control_rate = (Integer)ppsSpinner.getValue();
				}
				
			});

		}
		return ppsSpinner;
	}


	/**
	 * This method initializes jPanel2	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getJPanel2() {
		if (jPanel2 == null) {
			GLabel = new JLabel();
			GLabel.setText("0");
			GLabel.setSize(new Dimension(60, 15));
			GLabel.setLocation(new Point(330, 110));
			RRLabel = new JLabel();
			RRLabel.setText("0");
			RRLabel.setLocation(new Point(330, 95));
			RRLabel.setSize(new Dimension(60, 15));
			LRLabel = new JLabel();
			LRLabel.setText("0");
			LRLabel.setLocation(new Point(330, 80));
			LRLabel.setSize(new Dimension(60, 15));
			RFLabel = new JLabel();
			RFLabel.setText("0");
			RFLabel.setLocation(new Point(330, 65));
			RFLabel.setSize(new Dimension(60, 15));
			LFLabel = new JLabel();
			LFLabel.setText("0");
			LFLabel.setLocation(new Point(330, 50));
			LFLabel.setSize(new Dimension(60, 15));
			jLabel11 = new JLabel();
			jLabel11.setBounds(new Rectangle(330, 50, 61, 16));
			jLabel11.setText("JLabel");
			jLabel10 = new JLabel();
			jLabel10.setText("LF:");
			jLabel10.setSize(new Dimension(60, 14));
			jLabel10.setLocation(new Point(250, 50));
			jLabel9 = new JLabel();
			jLabel9.setPreferredSize(new Dimension(20, 14));
			jLabel9.setLocation(new Point(250, 110));
			jLabel9.setSize(new Dimension(60, 14));
			jLabel9.setText("Gimbal:");
			jLabel8 = new JLabel();
			jLabel8.setPreferredSize(new Dimension(20, 14));
			jLabel8.setLocation(new Point(250, 95));
			jLabel8.setSize(new Dimension(60, 14));
			jLabel8.setText("RR:");
			jLabel7 = new JLabel();
			jLabel7.setText("LR:");
			jLabel7.setSize(new Dimension(60, 14));
			jLabel7.setLocation(new Point(250, 80));
			jLabel6 = new JLabel();
			jLabel6.setText("RF:");
			jLabel6.setSize(new Dimension(60, 14));
			jLabel6.setLocation(new Point(250, 65));
			jLabel5 = new JLabel();
			jLabel5.setBounds(new Rectangle(255, 20, 95, 16));
			jLabel5.setText("Output Values");
			jLabel4 = new JLabel();
			jLabel4.setBounds(new Rectangle(15, 20, 103, 18));
			jLabel4.setText("Calibration Table");
			jPanel2 = new JPanel();
			jPanel2.setLayout(null);
			jPanel2.setBorder(BorderFactory.createTitledBorder(null, "Control Tuning/Options", TitledBorder.DEFAULT_JUSTIFICATION, TitledBorder.DEFAULT_POSITION, new Font("Dialog", Font.BOLD, 12), new Color(51, 51, 51)));
			jPanel2.setLocation(new Point(0, 125));
			jPanel2.setSize(new Dimension(436, 162));

			jPanel2.add(getJScrollPane(), null);
			jPanel2.add(jLabel4, null);
			jPanel2.add(jLabel5, null);
			jPanel2.add(jLabel6, null);
			jPanel2.add(jLabel7, null);
			jPanel2.add(jLabel8, null);
			jPanel2.add(jLabel9, null);
			jPanel2.add(jLabel10, null);
			jPanel2.add(LFLabel, null);
			jPanel2.add(RFLabel, null);
			jPanel2.add(LRLabel, null);
			jPanel2.add(RRLabel, null);
			jPanel2.add(GLabel, null);
			jPanel2.add(getApplyButton(), null);
			
		}
		return jPanel2;
	}


	/**
	 * This method initializes jPanel3	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getJPanel3() {
		if (jPanel3 == null) {
			jPanel3 = new JPanel();
			jPanel3.setLayout(null);
			jPanel3.setBorder(BorderFactory.createTitledBorder(null, "Controller Config", TitledBorder.DEFAULT_JUSTIFICATION, TitledBorder.DEFAULT_POSITION, new Font("Dialog", Font.BOLD, 12), new Color(51, 51, 51)));
			jPanel3.setSize(new Dimension(435, 125));
			jPanel3.setLocation(new Point(0, 0));
			jPanel3.add(jLabel3, null);
			jPanel3.add(getJComboBox(), null);
			jPanel3.add(jLabel, null);
			jPanel3.add(getControllerStatusField(), null);
			jPanel3.add(statusLightLabel, null);
			jPanel3.add(jLabel1, null);
			jPanel3.add(ppsLabel, null);
			jPanel3.add(jLabel2, null);
			jPanel3.add(getPpsSpinner(), null);
		}
		return jPanel3;
	}


	/**
	 * This method initializes jTable	
	 * 	
	 * @return javax.swing.JTable	
	 */
	private JTable getJTable() {
		if (jTable == null) {

		    jTable = new JTable(calib);
		    
		    jTable.getColumnModel().getColumn(0).setPreferredWidth(90);
		    jTable.getColumnModel().getColumn(1).setPreferredWidth(100);
		    jTable.getColumnModel().getColumn(2).setPreferredWidth(60);
		    jTable.getColumnModel().getColumn(3).setPreferredWidth(60);
		    jTable.getColumnModel().getColumn(4).setPreferredWidth(60);

		    
			jTable.setBounds(new Rectangle(19, 25, 200, 82));
		}
		return jTable;
	}


	/**
	 * This method initializes jScrollPane	
	 * 	
	 * @return javax.swing.JScrollPane	
	 */
	private JScrollPane getJScrollPane() {
		if (jScrollPane == null) {
			jScrollPane = new JScrollPane();
			jScrollPane.setBounds(new Rectangle(13, 41, 233, 113));
			jScrollPane.setViewportView(getJTable());
		}
		return jScrollPane;
	}


	/**
	 * This method initializes applyButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getApplyButton() {
		if (applyButton == null) {
			applyButton = new JButton();
			applyButton.setBounds(new Rectangle(252, 129, 137, 22));
			applyButton.setText("Apply Calibration");
			applyButton.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					ReadCalibrationFromTable();
				}
			});
		}
		return applyButton;
	}


}  //  @jve:decl-index=0:visual-constraint="10,10"
