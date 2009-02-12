package rover;

import javax.swing.JPanel;
import javax.swing.JSlider;

import java.awt.Frame;
import java.awt.BorderLayout;
import java.awt.GridBagConstraints;

import javax.swing.JDialog;
import java.awt.Dimension;
import java.util.ArrayList;

import javax.swing.JSpinner;
import java.awt.GridLayout;
import java.awt.GridBagLayout;

public class ServoPortal extends Portal {

	public static int CHANNELS = 10;
	private JPanel jContentPane = null;
	
	ArrayList<JSpinner> spinlist = null;  //  @jve:decl-index=0:
	ArrayList<JSlider> slidelist = null;

	int channels = 0;
	
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
		this.setSize(305, channels * 30);
		this.setResizable(false);
		
		this.setContentPane(getJContentPane());
		
		spinlist = new ArrayList<JSpinner>();
		slidelist = new ArrayList<JSlider>();
		
		for(int i = 0;i<channels;i++){
			JSpinner spinner = new JSpinner();
			JSlider slider = new JSlider();
			
			//do some configuration here
			spinner.setMinimumSize(new Dimension(60, 1));
			spinner.setMaximumSize(new Dimension(60, 20));
			spinner.setSize(60, 20);
			
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
	protected void Shutdown() {
		// TODO Auto-generated method stub
		
	}

}  //  @jve:decl-index=0:visual-constraint="10,10"
