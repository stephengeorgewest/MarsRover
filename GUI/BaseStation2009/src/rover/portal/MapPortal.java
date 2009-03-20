package rover.portal;

import java.awt.BorderLayout;
import java.awt.Frame;

import javax.swing.ImageIcon;
import javax.swing.JPanel;
import javax.swing.JFrame;

import rover.Main;
import rover.network.SocketInfo;
import rover.utils.*;

import java.awt.Dimension;
import java.net.InetAddress;
import java.awt.Rectangle;
import java.awt.GridBagLayout;
import javax.swing.BorderFactory;
import javax.swing.border.TitledBorder;
import java.awt.Font;
import java.awt.Color;
import javax.swing.JLabel;
import javax.swing.JTextField;
import java.awt.Point;

public class MapPortal extends Portal implements GPSClient.GPSHandler {

	private static final long serialVersionUID = 1L;
	private JPanel jContentPane = null;

	ImageIcon red;
	ImageIcon green;
	ImageIcon yellow;
	
	private GPSClient gpsclient = null;
	private GpsData gpsdata = null;
	private JPanel gpsPanel = null;
	private JLabel jLabel = null;
	private JLabel fixStatusLabel = null;
	private JLabel jLabel1 = null;
	private JLabel sataliteCountLabel = null;
	private JLabel jLabel2 = null;
	private JTextField latLongField = null;
	private JLabel jLabel3 = null;
	private JLabel jLabel4 = null;
	private JLabel elevationLabel = null;
	private JLabel headingLabel = null;
	private JLabel jLabel5 = null;
	private JLabel groundSpeedLabel = null;
	private JLabel gpsIconLabel = null;
	/**
	 * This is the default constructor
	 */
	public MapPortal(Frame owner) {
		super(owner);
		
		red = new ImageIcon(getClass().getResource("/redlight.gif"));
		green = new ImageIcon(getClass().getResource("/greenlight.gif"));
		yellow = new ImageIcon(getClass().getResource("/yellowlight.gif"));
		
		initialize();
		try{
			InetAddress mc_address = InetAddress.getByName(Main.props.getProperty("gps_address"));
			int port = Integer.parseInt(Main.props.getProperty("gps_port"));
			gpsclient = new GPSClient(mc_address, port);
			gpsdata = new GpsData();
			gpsclient.addGPSListener(this);
			
			SocketInfo si = new SocketInfo();
			si.setPortal("Map Portal");
			si.setPort(port);
			si.setType("MC Rec");
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
	
	public static class MapPortalInit extends PortalInit {
		@Override
		public Portal createPortal() {
			return new MapPortal(MainGUI.instance);
		}
		
		@Override
		public String toString(){
			return "Map Portal";
		}
	}
	
	/**
	 * This method initializes this
	 * 
	 * @return void
	 */
	private void initialize() {
		this.setSize(406, 420);
		this.setContentPane(getJContentPane());
		this.setTitle("3D Map Portal");
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
			jContentPane.add(getGpsPanel(), null);
		}
		return jContentPane;
	}

	@Override
	public void Shutdown() {
		// TODO Auto-generated method stub
		
	}

	/**
	 * This method initializes gpsPanel	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getGpsPanel() {
		if (gpsPanel == null) {
			gpsIconLabel = new JLabel();
			gpsIconLabel.setBounds(new Rectangle(208, 13, 47, 46));
			gpsIconLabel.setIcon(red);
			groundSpeedLabel = new JLabel();
			groundSpeedLabel.setBounds(new Rectangle(136, 141, 76, 20));
			groundSpeedLabel.setText("Unknown");
			jLabel5 = new JLabel();
			jLabel5.setBounds(new Rectangle(10, 143, 104, 16));
			jLabel5.setText("Ground Speed:");
			headingLabel = new JLabel();
			headingLabel.setBounds(new Rectangle(136, 119, 73, 19));
			headingLabel.setText("Unknown");
			elevationLabel = new JLabel();
			elevationLabel.setBounds(new Rectangle(136, 94, 76, 20));
			elevationLabel.setText("Unknown");
			jLabel4 = new JLabel();
			jLabel4.setBounds(new Rectangle(10, 118, 65, 21));
			jLabel4.setText("Heading:");
			jLabel3 = new JLabel();
			jLabel3.setText("Elevation (ft)");
			jLabel3.setLocation(new Point(10, 94));
			jLabel3.setSize(new Dimension(80, 18));
			jLabel2 = new JLabel();
			jLabel2.setText("Lat/Long:");
			jLabel2.setLocation(new Point(10, 74));
			jLabel2.setSize(new Dimension(60, 18));
			sataliteCountLabel = new JLabel();
			sataliteCountLabel.setBounds(new Rectangle(100, 39, 40, 22));
			sataliteCountLabel.setText("0");
			jLabel1 = new JLabel();
			jLabel1.setBounds(new Rectangle(9, 39, 71, 21));
			jLabel1.setText("Satalites:");
			fixStatusLabel = new JLabel();
			fixStatusLabel.setBounds(new Rectangle(99, 20, 71, 17));
			fixStatusLabel.setText("Unknown");
			jLabel = new JLabel();
			jLabel.setText("Fix Status:");
			jLabel.setLocation(new Point(10, 20));
			jLabel.setSize(new Dimension(70, 18));
			gpsPanel = new JPanel();
			gpsPanel.setLayout(null);
			gpsPanel.setBounds(new Rectangle(6, 6, 283, 174));
			gpsPanel.setBorder(BorderFactory.createTitledBorder(null, "GPS System", TitledBorder.DEFAULT_JUSTIFICATION, TitledBorder.DEFAULT_POSITION, new Font("Dialog", Font.BOLD, 12), new Color(51, 51, 51)));
			gpsPanel.add(jLabel, null);
			gpsPanel.add(fixStatusLabel, null);
			gpsPanel.add(jLabel1, null);
			gpsPanel.add(sataliteCountLabel, null);
			gpsPanel.add(jLabel2, null);
			gpsPanel.add(getLatLongField(), null);
			gpsPanel.add(jLabel3, null);
			gpsPanel.add(jLabel4, null);
			gpsPanel.add(elevationLabel, null);
			gpsPanel.add(headingLabel, null);
			gpsPanel.add(jLabel5, null);
			gpsPanel.add(groundSpeedLabel, null);
			gpsPanel.add(gpsIconLabel, null);
		}
		return gpsPanel;
	}

	/**
	 * This method initializes latLongField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	private JTextField getLatLongField() {
		if (latLongField == null) {
			latLongField = new JTextField();
			latLongField.setEditable(false);
			latLongField.setSize(new Dimension(179, 20));
			latLongField.setLocation(new Point(90, 72));
		}
		return latLongField;
	}

	@Override
	public void GPSDataReceived(String data) {
		if(gpsdata.MergeData(data)){
			System.out.println("GPS update");
			refreshGPS();
		}
	}
	
	private void refreshGPS(){
		headingLabel.setText("" + gpsdata.Heading);
		elevationLabel.setText("" + gpsdata.Elevation);
		if(gpsdata.LockAcquired){
			fixStatusLabel.setText("Lock Aquired");
			if(gpsdata.NumberOfSatellites > 4){
				gpsIconLabel.setIcon(green);
			}else
				gpsIconLabel.setIcon(yellow);
		}
		else{
			fixStatusLabel.setText("No Lock");
			gpsIconLabel.setIcon(red);
		}
		sataliteCountLabel.setText("" + gpsdata.NumberOfSatellites);
		latLongField.setText(gpsdata.getFormatedPos());
		groundSpeedLabel.setText(""+gpsdata.Speed);
		
	}

}
