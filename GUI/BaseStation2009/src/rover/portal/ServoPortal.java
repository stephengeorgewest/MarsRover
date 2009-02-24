package rover.portal;

import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.SpinnerNumberModel;

import java.awt.Frame;
import java.awt.BorderLayout;
import java.awt.GridBagConstraints;

import javax.swing.JDialog;
import java.awt.Dimension;
import java.util.ArrayList;

import javax.swing.JSpinner;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;


import java.awt.GridLayout;
import java.awt.GridBagLayout;

public class ServoPortal extends Portal implements ChangeListener{

	public static int CHANNELS = 32;
	private JPanel jContentPane = null;
	
	ArrayList<JSpinner> spinlist = null;  //  @jve:decl-index=0:
	ArrayList<JSlider> slidelist = null;

	int channels = 0;
	boolean disable_events = true;
	
	/**
	 * @param owner
	 */
	public ServoPortal(Frame owner) {
		super(owner);
		initialize(CHANNELS);
	}

	/**
	 * This method initializes this
	 * 
	 * @return void
	 */
	private void initialize(int channels) {
		this.channels = channels;
		this.setSize(305, channels * 22);
		this.setResizable(false);
		
		this.setContentPane(getJContentPane());
		
		spinlist = new ArrayList<JSpinner>();
		slidelist = new ArrayList<JSlider>();
		
		for(int i = 0;i<channels;i++){
			JSpinner spinner = new JSpinner(new SpinnerNumberModel(50, 0, 100, .01));
			JSlider slider = new JSlider();
			
			//do some configuration here
			spinner.setMinimumSize(new Dimension(60, 1));
			spinner.setMaximumSize(new Dimension(60, 20));
			spinner.setSize(60, 20);
			
			slider.setMaximum(10000);
			slider.setMinimum(0);
			slider.setMajorTickSpacing(100);
			slider.setMinorTickSpacing(1);
			slider.setValue(5000);
			
			slider.addChangeListener(this);
			spinner.addChangeListener(this);
			
			
			GridBagConstraints c = new GridBagConstraints();
			c.fill = GridBagConstraints.HORIZONTAL;
			c.gridx = 0;
			c.gridy = i;
			c.weightx = 1;
		
			jContentPane.add(slider, c);

			c.fill = GridBagConstraints.HORIZONTAL;
			c.gridx = 1;
			c.gridy = i;
			c.weightx = 1;
			jContentPane.add(spinner, c);
			
			spinlist.add(spinner);
			slidelist.add(slider);
			
		}
		disable_events = false;
		
	}

	
	@Override
	public void stateChanged(ChangeEvent arg0) {
		if(disable_events) return;
		disable_events = true;
		int i;
		double val;
		if(arg0.getSource().getClass() == JSpinner.class){
			i = spinlist.indexOf(arg0.getSource());
			Double d = (Double)spinlist.get(i).getValue();
			val = d.doubleValue();
			d *= 100;
			slidelist.get(i).setValue(d.intValue());
		}else{
			i = slidelist.indexOf(arg0.getSource());
			val = slidelist.get(i).getValue();
			val /= 100;
			spinlist.get(i).setValue(val);
		}
		//send some servo control packets on channel i
		System.out.println("Servo " + i + " changing to " + val);
		
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
	public void Shutdown() {
		// TODO Auto-generated method stub
		
	}

}  //  @jve:decl-index=0:visual-constraint="10,10"