package rover.guistuff;

import javax.swing.JDialog;
import javax.swing.JPanel;
import java.awt.GridBagLayout;
import java.awt.Dimension;
import java.awt.Insets;

import javax.swing.JLabel;
import java.awt.Rectangle;
import javax.swing.JRadioButton;
import javax.swing.JSpinner;
import javax.swing.JTextField;
import javax.swing.SpinnerNumberModel;
import javax.swing.JButton;

import rover.graphics.GeoData;
import rover.portal.MapPortal;
import rover.utils.GpsData;
import rover.utils.Waypoint;
import rover.utils.GPSClient.GPSHandler;

import java.awt.Point;

public class WaypointPropertiesDialog extends JDialog implements GPSHandler{

	public static int waypoint_count = 1;
	
	private JPanel jPanel = null;
	private JTextField coordTextField = null;
	private JLabel jLabel = null;
	private MapPortal portal = null;
	private Waypoint wp = null;
	private GpsData rp = null;
	
	private JLabel jLabel1 = null;
	private JTextField distTextField = null;
	private JLabel jLabel2 = null;
	private JTextField headingTextField = null;
	/**
	 * This method initializes 
	 * 
	 */
	public WaypointPropertiesDialog(MapPortal portal, Waypoint wp) {
		super(portal);
		this.portal = portal;
		this.wp = wp;
		initialize();
		rp = new GpsData();
		portal.gpsclient.addGPSListener(this);
	}

	public void showDialog(){
		setVisible(true);
		coordTextField.setText(GpsData.formatCoordinates(wp.lat, wp.lon));
	}
	
	/**
	 * This method initializes this
	 * 
	 */
	private void initialize() {
        this.setSize(new Dimension(270, 119));
        this.setTitle(wp.name);
        this.setContentPane(getJPanel());
			
	}

	/**
	 * This method initializes jPanel	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getJPanel() {
		if (jPanel == null) {
			jLabel2 = new JLabel();
			jLabel2.setBounds(new Rectangle(7, 61, 116, 17));
			jLabel2.setText("Heading from Rover");
			jLabel1 = new JLabel();
			jLabel1.setBounds(new Rectangle(9, 39, 91, 14));
			jLabel1.setText("Dist from Rover");
			jLabel = new JLabel();
			jLabel.setBounds(new Rectangle(8, 11, 74, 14));
			jLabel.setText("Coordinates");
			jPanel = new JPanel();
			jPanel.setLayout(null);
			jPanel.add(getLatitudeTextField(), null);
			jPanel.add(jLabel, null);
			jPanel.add(jLabel1, null);
			jPanel.add(getDistTextField(), null);
			jPanel.add(jLabel2, null);
			jPanel.add(getHeadingTextField(), null);
		}
		return jPanel;
	}

	/**
	 * This method initializes latitudeTextField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	private JTextField getLatitudeTextField() {
		if (coordTextField == null) {
			coordTextField = new JTextField();
			coordTextField.setSize(new Dimension(160, 20));
			coordTextField.setEditable(false);
			coordTextField.setLocation(new Point(90, 10));
		}
		return coordTextField;
	}

	/**
	 * This method initializes distTextField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	private JTextField getDistTextField() {
		if (distTextField == null) {
			distTextField = new JTextField();
			distTextField.setBounds(new Rectangle(125, 36, 124, 17));
			distTextField.setEditable(false);
		}
		return distTextField;
	}

	/**
	 * This method initializes headingTextField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	private JTextField getHeadingTextField() {
		if (headingTextField == null) {
			headingTextField = new JTextField();
			headingTextField.setBounds(new Rectangle(132, 60, 116, 18));
			headingTextField.setEditable(false);
		}
		return headingTextField;
	}

	@Override
	public void GPSDataReceived(String data) {
		// TODO Auto-generated method stub
		rp.MergeData(data);
		
		double wx = GeoData.longitude2meters(wp.lon, GeoData.adj_latitude);
		double wy = GeoData.latitude2meters(wp.lat);
		
		double rx = GeoData.longitude2meters(rp.Longitude, GeoData.adj_latitude);
		double ry = GeoData.latitude2meters(rp.Latitude);
		
		double dist = Math.sqrt((wx-rx)*(wx-rx) + (wy-ry)*(wy-ry));
		//System.out.println("" + (wy-ry) + " " + (wx-rx) );
		double heading = 180*(Math.PI/2-Math.atan2((wy-ry), (wx-rx)))/Math.PI;
		
		distTextField.setText(dist + " m");
		headingTextField.setText(heading + " degrees");
	}

	
	
	
}  //  @jve:decl-index=0:visual-constraint="45,10"
