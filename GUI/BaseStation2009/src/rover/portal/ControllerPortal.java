package rover.portal;

import java.awt.BorderLayout;
import java.awt.Frame;

import javax.swing.JPanel;
import net.java.games.input.*;
import java.awt.Dimension;
import javax.swing.JLabel;
import java.awt.Rectangle;
import javax.swing.JTextField;
import javax.swing.JComboBox;
import javax.swing.JOptionPane;
import javax.swing.SpinnerNumberModel;

import java.awt.GridBagLayout;
import javax.swing.BorderFactory;
import javax.swing.border.TitledBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;


import java.awt.Font;
import java.awt.Color;
import java.awt.GridBagConstraints;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.Random;

import rover.components.RoverPanel2D;
import rover.utils.InputDevice;
import rover.utils.Stopwatch;

import javax.swing.ImageIcon;
import javax.swing.JSpinner;

public class ControllerPortal extends Portal{

	private static final long serialVersionUID = 1L;
	private JPanel jContentPane = null;
	private JLabel jLabel = null;
	private JTextField controllerNameField = null;
	private JComboBox jComboBox = null;
	private JPanel jPanel = null;
	private JPanel jPanel1 = null;
	private RoverPanel2D roverPanel2D = null;
	private JLabel jLabel1 = null;
	private JLabel statusLightLabel = null;
	private JSpinner ppsSpinner = null;
	private JLabel ppsLabel = null;
	private JLabel jLabel2 = null;
	
	private Thread inputThread;
	private boolean inputThreadKill = false;
	private Object inputLock = null;
	private Controller inputdevice;
	private int control_rate;
	
	private ArrayList<InputDevice> inputDevices;
	
	ImageIcon red;
	ImageIcon green;
	ImageIcon yellow;
	
	
	/**
	 * @param owner
	 */
	public ControllerPortal(Frame owner) {
		super(owner);
		
		red = new ImageIcon(getClass().getResource("/redlight.gif"));
		green = new ImageIcon(getClass().getResource("/greenlight.gif"));
		yellow = new ImageIcon(getClass().getResource("/yellowlight.gif"));
		
		control_rate = 20;
		
		initialize();
		
		inputThread = new Thread(){	@Override public void run() {inputThreadStart();}};
		inputThread.setDaemon(true);
		inputLock = new Object();
		inputThread.start();
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
                    	inputDevices = new ArrayList<InputDevice>();
                		jComboBox.removeAllItems();
                		jComboBox.addItem(new InputDevice());
                		ControllerEnvironment ce = ControllerEnvironment.getDefaultEnvironment();
                		
                		System.out.println("Searching for controllers");
                		
                		Controller input;
                		// retrieve the available controllers
                		Controller[] controllers = ce.getControllers();
                		for(int i = 0;i<controllers.length;i++){
                			input = controllers[i];
                			if(analogAxes(input) > 3){
                				InputDevice dev = new InputDevice(input);
                				inputDevices.add(dev);
                			}
                		}

                		if(inputDevices.size() > 0){
                			this.statusLightLabel.setIcon(yellow);
                			inputdevice = inputDevices.get(0).controller;
                		}
                		else{
                			this.statusLightLabel.setIcon(red);
                		}
                        
                        //if(inputdevice != null) inputStatusLabel.Text = inputdevice.Status;
                        sleep_time = 200; //we don't want to be trying to discover new devices at full frequency
                    }
                    else if (!inputdevice.poll()) //try to reconnect
                    {
                    	System.out.println("Killing invald controller");
                    	inputdevice = null;
//                        inputdevice.AquireDevice();
//                        inputStatusLight.Image = global::GroundStation.Properties.Resources.redlight;
//                        inputStatusLabel.Text = inputdevice.Status;

                    }else
                    {
                        int target_period = 1000 / control_rate;
                        int display_rate = Math.max(control_rate/disp_rate, 1);
                        //read from the device and send off a udp control packet
                        
                        //read status data from device

                        //System.out.println("Reading Controller Data");
                        
                        Component[] comps = inputdevice.getComponents();
                        
                        float[] axes = new float[4];
                        
                        int i = 0;
                        for(Component comp : comps){
                			if(comp.isAnalog()){
                				axes[i] = comp.getPollData();
                				i++;
                				if(i >= axes.length) break;
                			}
                		}
                		
                        
//                        axes[0] = generator.nextFloat();
//                        axes[1] = generator.nextFloat();
//                        axes[2] = generator.nextFloat();
//                        axes[3] = 0;
                        
                        //System.out.println("a0= " + axes[0] + " a1= " + axes[1] + " a2= " + axes[2] + " a3= " + axes[3]);
                        
                        roverPanel2D.setHead_Angle(-90*axes[2]);
                        
                        float[] throttles = new float[4];
                        
                        throttles[0] = throttles[2] = -100*axes[0]*(axes[1]+1)/2f;
                        throttles[1] = throttles[3] = 100*axes[0]*(axes[1]-1)/2f;
                        
                        roverPanel2D.setThrottles(throttles);
                        
                        roverPanel2D.repaint();
                        
                        //InputState s = inputdevice.State;
                        //Console.WriteLine(s.roll + " " + s.pitch + " " + s.yaw_rate + " " + s.thrust_level);

                        float max_rate = 1;
                        //TODO Send UDP control packet

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
                            
                            this.ppsLabel.setText(df.format(pps));
                            this.statusLightLabel.setIcon(green);
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
	
	private int analogAxes(Controller c){
		Component[] comps = c.getComponents(); 
		int analog_axes = 0;
		for(Component comp : comps){
			if(comp.isAnalog()) analog_axes++;
		}
		return analog_axes;
	}
	
	/**
	 * This method initializes this
	 * 
	 * @return void
	 */
	private void initialize() {
		this.setSize(783, 305);
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
			jLabel2 = new JLabel();
			jLabel2.setBounds(new Rectangle(214, 45, 61, 14));
			jLabel2.setText("Desired:");
			ppsLabel = new JLabel();
			ppsLabel.setBounds(new Rectangle(142, 45, 61, 18));
			ppsLabel.setText("0");
			statusLightLabel = new JLabel();
			statusLightLabel.setBounds(new Rectangle(375, 10, 43, 44));
			statusLightLabel.setIcon(new ImageIcon(getClass().getResource("/redlight.gif")));
			statusLightLabel.setText("");
			jLabel1 = new JLabel();
			jLabel1.setBounds(new Rectangle(12, 45, 115, 19));
			jLabel1.setText("Actual Packets/s");
			jLabel = new JLabel();
			jLabel.setBounds(new Rectangle(13, 14, 114, 20));
			jLabel.setText("Controller Name:");
			jContentPane = new JPanel();
			jContentPane.setLayout(null);
			jContentPane.add(jLabel, null);
			jContentPane.add(getControllerNameField(), null);
			jContentPane.add(getJComboBox(), null);
			jContentPane.add(getJPanel(), null);
			jContentPane.add(jLabel1, null);
			jContentPane.add(statusLightLabel, null);
			jContentPane.add(getPpsSpinner(), null);
			jContentPane.add(ppsLabel, null);
			jContentPane.add(jLabel2, null);
		}
		return jContentPane;
	}

	@Override
	public void Shutdown() {
		// TODO Auto-generated method stub
			inputThreadKill = true;
	}

	/**
	 * This method initializes controllerNameField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	private JTextField getControllerNameField() {
		if (controllerNameField == null) {
			controllerNameField = new JTextField();
			controllerNameField.setBounds(new Rectangle(141, 10, 216, 27));
			controllerNameField.setEditable(false);
		}
		return controllerNameField;
	}

	/**
	 * This method initializes jComboBox	
	 * 	
	 * @return javax.swing.JComboBox	
	 */
	private JComboBox getJComboBox() {
		if (jComboBox == null) {
			jComboBox = new JComboBox();
			jComboBox.setBounds(new Rectangle(140, 207, 221, 34));
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
			GridBagConstraints gridBagConstraints = new GridBagConstraints();
			gridBagConstraints.gridx = 0;
			gridBagConstraints.gridy = 0;
			jPanel = new JPanel();
			jPanel.setLayout(null);
			jPanel.setBounds(new Rectangle(515, 14, 252, 242));
			jPanel.setBorder(BorderFactory.createTitledBorder(null, "Rover View", TitledBorder.DEFAULT_JUSTIFICATION, TitledBorder.DEFAULT_POSITION, new Font("Dialog", Font.BOLD, 12), new Color(51, 51, 51)));
			jPanel.add(getRoverPanel2D(), null);
		}
		return jPanel;
	}

	/**
	 * This method initializes jPanel1	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getJPanel1() {
		if (jPanel1 == null) {
			jPanel1 = new JPanel();
			jPanel1.setLayout(new GridBagLayout());
		}
		return jPanel1;
	}

	/**
	 * This method initializes roverPanel2D	
	 * 	
	 * @return rover.components.RoverPanel2D	
	 */
	private RoverPanel2D getRoverPanel2D() {
		if (roverPanel2D == null) {
			roverPanel2D = new RoverPanel2D();
			roverPanel2D.setBounds(new Rectangle(11, 21, 226, 208));
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
			ppsSpinner.setBounds(new Rectangle(282, 44, 75, 18));
			ppsSpinner.addChangeListener(new ChangeListener(){
				public void stateChanged(ChangeEvent e)
				{
					control_rate = (Integer)ppsSpinner.getValue();
				}
				
			});

		}
		return ppsSpinner;
	}

}  //  @jve:decl-index=0:visual-constraint="10,10"
