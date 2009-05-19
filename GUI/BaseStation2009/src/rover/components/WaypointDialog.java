package rover.guistuff;

import javax.swing.JDialog;
import javax.swing.JPanel;
import java.awt.GridBagLayout;
import java.awt.Dimension;
import javax.swing.JLabel;
import java.awt.Rectangle;
import javax.swing.JRadioButton;
import javax.swing.JSpinner;
import javax.swing.JTextField;
import javax.swing.SpinnerNumberModel;
import javax.swing.JButton;

import rover.portal.MapPortal;
import rover.utils.GpsData;
import rover.utils.Waypoint;
import java.awt.Point;

public class WaypointDialog extends JDialog{

	public static int waypoint_count = 1;
	
	private JPanel jPanel = null;
	private JSpinner jSpinner = null;
	private JLabel jLabel1 = null;
	private JTextField latitudeTextField = null;
	private JTextField longitudeTextField = null;
	private JLabel jLabel = null;
	private JLabel jLabel2 = null;
	private JLabel jLabel3 = null;
	private JButton averageButton = null;
	private JButton cancelButton = null;
	private JButton addButton = null;

	public double lat = 0;
	public double lon = 0;
	public double elevation = 0;
	
	private MapPortal portal = null;
	private JLabel jLabel4 = null;
	private JTextField nameTextField = null;
	
//	/**
//	 * This method initializes 
//	 * 
//	 */
//	public WaypointDialog() {
//		super();
//		initialize();
//	}
	
	/**
	 * This method initializes 
	 * 
	 */
	public WaypointDialog(MapPortal portal) {
		super(portal);
		this.portal = portal;
		initialize();
	}

	public void showDialog(){
		setVisible(true);
		nameTextField.setText("waypoint" + waypoint_count);
		latitudeTextField.setText(GpsData.formatLat(lat));
		longitudeTextField.setText(GpsData.formatLon(lon));
	}
	
	/**
	 * This method initializes this
	 * 
	 */
	private void initialize() {
        this.setSize(new Dimension(275, 221));
        this.setTitle("Add Waypoint");
        this.setContentPane(getJPanel());
			
	}

	/**
	 * This method initializes jPanel	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getJPanel() {
		if (jPanel == null) {
			jLabel4 = new JLabel();
			jLabel4.setBounds(new Rectangle(10, 10, 75, 16));
			jLabel4.setText("Name");
			jLabel3 = new JLabel();
			jLabel3.setBounds(new Rectangle(187, 124, 73, 13));
			jLabel3.setText("readings");
			jLabel2 = new JLabel();
			jLabel2.setBounds(new Rectangle(12, 73, 73, 16));
			jLabel2.setText("Longitude");
			jLabel = new JLabel();
			jLabel.setBounds(new Rectangle(11, 42, 74, 14));
			jLabel.setText("Latitude");
			jLabel1 = new JLabel();
			jLabel1.setBounds(new Rectangle(134, 100, 98, 18));
			jLabel1.setText("Average");
			jPanel = new JPanel();
			jPanel.setLayout(null);
			jPanel.add(getJSpinner(), null);
			jPanel.add(jLabel1, null);
			jPanel.add(getLatitudeTextField(), null);
			jPanel.add(getLongitudeTextField(), null);
			jPanel.add(jLabel, null);
			jPanel.add(jLabel2, null);
			jPanel.add(jLabel3, null);
			jPanel.add(getAverageButton(), null);
			jPanel.add(getCancelButton(), null);
			jPanel.add(getAddButton(), null);
			jPanel.add(jLabel4, null);
			jPanel.add(getNameTextField(), null);
		}
		return jPanel;
	}

	/**
	 * This method initializes jSpinner	
	 * 	
	 * @return javax.swing.JSpinner	
	 */
	private JSpinner getJSpinner() {
		if (jSpinner == null) {
			jSpinner = new JSpinner(new SpinnerNumberModel(4, 0, 15, 1));
			jSpinner.setBounds(new Rectangle(137, 121, 40, 19));
		}
		return jSpinner;
	}

	/**
	 * This method initializes latitudeTextField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	private JTextField getLatitudeTextField() {
		if (latitudeTextField == null) {
			latitudeTextField = new JTextField();
			latitudeTextField.setSize(new Dimension(160, 20));
			latitudeTextField.setLocation(new Point(90, 40));
		}
		return latitudeTextField;
	}

	/**
	 * This method initializes longitudeTextField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	private JTextField getLongitudeTextField() {
		if (longitudeTextField == null) {
			longitudeTextField = new JTextField();
			longitudeTextField.setSize(new Dimension(160, 20));
			longitudeTextField.setLocation(new Point(90, 70));
		}
		return longitudeTextField;
	}

	/**
	 * This method initializes averageButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getAverageButton() {
		if (averageButton == null) {
			averageButton = new JButton();
			averageButton.setBounds(new Rectangle(17, 106, 101, 28));
			averageButton.setText("Average GPS");
		}
		return averageButton;
	}

	/**
	 * This method initializes cancelButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getCancelButton() {
		if (cancelButton == null) {
			cancelButton = new JButton();
			cancelButton.setBounds(new Rectangle(134, 154, 105, 30));
			cancelButton.setText("Close");
			cancelButton.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					setVisible(false);
				}
			});
		}
		return cancelButton;
	}

	/**
	 * This method initializes addButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getAddButton() {
		if (addButton == null) {
			addButton = new JButton();
			addButton.setBounds(new Rectangle(13, 154, 113, 30));
			addButton.setText("Add Waypoint");
			addButton.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					try{
						lat = Double.parseDouble(latitudeTextField.getText());

					}catch (Exception ex){
						latitudeTextField.setText(GpsData.formatLat(lat));
						return;
					}
					try{
						lon = Double.parseDouble(longitudeTextField.getText());

					}catch (Exception ex){
						longitudeTextField.setText(GpsData.formatLon(lon));
						return;
					}
					String name = nameTextField.getText();
					Waypoint wp = new Waypoint(name, lat, lon, 0);
					portal.addWaypoint(wp);
					waypoint_count++;
					nameTextField.setText("waypoint" + waypoint_count);
					//setVisible(false);
				}
			});
		}
		return addButton;
	}

	/**
	 * This method initializes nameTextField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	private JTextField getNameTextField() {
		if (nameTextField == null) {
			nameTextField = new JTextField();
			nameTextField.setSize(new Dimension(160, 20));
			nameTextField.setLocation(new Point(90, 10));
		}
		return nameTextField;
	}

	
	
	
}  //  @jve:decl-index=0:visual-constraint="10,10"
