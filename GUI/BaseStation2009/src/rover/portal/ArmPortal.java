package rover.portal;

import java.awt.Component;
import java.awt.Frame;
import java.awt.Dimension;
import javax.swing.JPanel;
import java.awt.GridBagLayout;
import java.awt.Rectangle;
import javax.swing.BorderFactory;
import javax.swing.border.TitledBorder;
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
import rover.components.ArmPanel3D;

public class ArmPortal extends Portal implements ActionListener {

	private JPanel jContentPane = null;
	private JPanel jointPanel = null;
	private JPanel xyzPanel = null;
	private JLabel jLabel = null;
	private JLabel jLabel1 = null;
	private JLabel jLabel2 = null;
	private JLabel jLabel3 = null;
	private JLabel jLabel4 = null;
	private JLabel jLabel5 = null;
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
	private JSpinner toolSpinner = null;
	private JSpinner xSpinner = null;
	private JSpinner ySpinner = null;
	private JSpinner zSpinner = null;
	private JSpinner phiSpinner = null;
	private JSpinner thetaSpinner = null;
	private JPanel modePanel = null;
	private JRadioButton jointRadioButton = null;
	private JRadioButton xyzRadioButton = null;
	private JRadioButton disableRadioButton = null;

	private int mode = 0;
	
	private static final int MODE_DISABLED = 0;
	private static final int MODE_JOINT = 1;
	private static final int MODE_XYZ = 2;
	private JPanel armPanel = null;
	private GLCanvas GLCanvas = null;
	private ArmPanel3D armPanel3D = null;
	
	public ArmPortal(Frame owner) {
		super(owner);
		initialize();
		// TODO Auto-generated constructor stub
	}

	/**
	 * This method initializes this
	 * 
	 */
	private void initialize() {
        this.setSize(new Dimension(656, 296));
        this.setTitle("Arm Control Portal");
        this.setContentPane(getJContentPane());
		
        setMode(MODE_DISABLED);
	}

	@Override
	public void Shutdown() {
		// TODO Auto-generated method stub

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
			jLabel5 = new JLabel();
			jLabel5.setText("Tool");
			jLabel5.setLocation(new Point(6, 122));
			jLabel5.setSize(new Dimension(53, 17));
			jLabel4 = new JLabel();
			jLabel4.setText("Joint 5");
			jLabel4.setSize(new Dimension(50, 17));
			jLabel4.setLocation(new Point(6, 102));
			jLabel3 = new JLabel();
			jLabel3.setText("Joint 4");
			jLabel3.setSize(new Dimension(50, 17));
			jLabel3.setLocation(new Point(6, 82));
			jLabel2 = new JLabel();
			jLabel2.setText("Joint 3");
			jLabel2.setSize(new Dimension(50, 17));
			jLabel2.setLocation(new Point(6, 62));
			jLabel1 = new JLabel();
			jLabel1.setText("Joint 2");
			jLabel1.setSize(new Dimension(50, 17));
			jLabel1.setLocation(new Point(6, 42));
			jLabel = new JLabel();
			jLabel.setText("Joint 1");
			jLabel.setSize(new Dimension(50, 17));
			jLabel.setLocation(new Point(6, 22));
			jointPanel = new JPanel();
			jointPanel.setLayout(null);
			jointPanel.setBounds(new Rectangle(8, 58, 153, 153));
			jointPanel.setBorder(BorderFactory.createTitledBorder(null, "Joint Space", TitledBorder.DEFAULT_JUSTIFICATION, TitledBorder.DEFAULT_POSITION, new Font("Dialog", Font.BOLD, 12), new Color(51, 51, 51)));
			jointPanel.setEnabled(true);
			jointPanel.add(jLabel, null);
			jointPanel.add(jLabel1, null);
			jointPanel.add(jLabel2, null);
			jointPanel.add(jLabel3, null);
			jointPanel.add(jLabel4, null);
			jointPanel.add(jLabel5, null);
			jointPanel.add(getJoint1Spinner(), null);
			jointPanel.add(getJoint2Spinner(), null);
			jointPanel.add(getJoint3Spinner(), null);
			jointPanel.add(getJoint4Spinner(), null);
			jointPanel.add(getJoint5Spinner(), null);
			jointPanel.add(getToolSpinner(), null);
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
			jLabel10 = new JLabel();
			jLabel10.setText("TTheta:");
			jLabel10.setSize(new Dimension(50, 17));
			jLabel10.setLocation(new Point(8, 102));
			jLabel9 = new JLabel();
			jLabel9.setText("TPhi:");
			jLabel9.setSize(new Dimension(40, 17));
			jLabel9.setLocation(new Point(8, 82));
			jLabel8 = new JLabel();
			jLabel8.setText("Z:");
			jLabel8.setSize(new Dimension(40, 17));
			jLabel8.setLocation(new Point(8, 62));
			jLabel7 = new JLabel();
			jLabel7.setText("Y:");
			jLabel7.setSize(new Dimension(40, 17));
			jLabel7.setLocation(new Point(8, 42));
			jLabel6 = new JLabel();
			jLabel6.setText("X:");
			jLabel6.setSize(new Dimension(40, 17));
			jLabel6.setLocation(new Point(8, 22));
			xyzPanel = new JPanel();
			xyzPanel.setLayout(null);
			xyzPanel.setBounds(new Rectangle(167, 59, 162, 153));
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
			joint1Spinner.setLocation(new Point(71, 22));
			joint1Spinner.setSize(new Dimension(60, 17));
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
			joint2Spinner.setLocation(new Point(71, 42));
			joint2Spinner.setSize(new Dimension(60, 17));
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
			joint3Spinner.setLocation(new Point(71, 62));
			joint3Spinner.setSize(new Dimension(60, 17));
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
			joint4Spinner.setLocation(new Point(71, 82));
			joint4Spinner.setSize(new Dimension(60, 17));
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
			joint5Spinner.setLocation(new Point(71, 102));
			joint5Spinner.setSize(new Dimension(60, 17));
		}
		return joint5Spinner;
	}

	/**
	 * This method initializes toolSpinner	
	 * 	
	 * @return javax.swing.JSpinner	
	 */
	private JSpinner getToolSpinner() {
		if (toolSpinner == null) {
			toolSpinner = new JSpinner(new SpinnerNumberModel(0, 0, 100, 1));
			toolSpinner.setLocation(new Point(71, 122));
			toolSpinner.setSize(new Dimension(60, 17));
		}
		return toolSpinner;
	}

	/**
	 * This method initializes xSpinner	
	 * 	
	 * @return javax.swing.JSpinner	
	 */
	private JSpinner getXSpinner() {
		if (xSpinner == null) {
			xSpinner = new JSpinner();
			xSpinner.setSize(new Dimension(59, 17));
			xSpinner.setLocation(new Point(68, 22));
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
			ySpinner.setSize(new Dimension(60, 16));
			ySpinner.setLocation(new Point(68, 42));
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
			zSpinner.setSize(new Dimension(60, 17));
			zSpinner.setLocation(new Point(68, 62));
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
			phiSpinner.setSize(new Dimension(59, 17));
			phiSpinner.setLocation(new Point(68, 82));
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
			thetaSpinner.setSize(new Dimension(58, 17));
			thetaSpinner.setLocation(new Point(68, 102));
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
			armPanel.setBounds(new Rectangle(337, 8, 304, 254));
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
	private ArmPanel3D getArmPanel3D() {
		if (armPanel3D == null) {
			armPanel3D = new ArmPanel3D();
			armPanel3D.setBounds(new Rectangle(8, 17, 288, 229));
		}
		return armPanel3D;
	}
	
}  //  @jve:decl-index=0:visual-constraint="10,10"
