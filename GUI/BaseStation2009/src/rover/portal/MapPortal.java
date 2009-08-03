package rover.portal;

import java.awt.Frame;

import javax.swing.ImageIcon;
import javax.swing.JPanel;
import javax.swing.JFrame;

import rover.Main;
import rover.network.SocketInfo;
import rover.utils.*;

import java.awt.Dimension;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.PrintStream;
import java.net.InetAddress;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.Iterator;
import java.awt.Rectangle;
import java.awt.GridBagLayout;
import javax.swing.BorderFactory;
import javax.swing.border.TitledBorder;
import javax.swing.filechooser.FileFilter;

import java.awt.Font;
import java.awt.Color;
import javax.swing.JLabel;
import javax.swing.JTextField;
import java.awt.Point;

import rover.graphics.TexTileDB;
import rover.guistuff.EarthSimPanel3D;
import rover.guistuff.WaypointDialog;
import rover.guistuff.WaypointPropertiesDialog;
import rover.guistuff.EarthSimPanel3D.CameraMoveListener;

import javax.swing.ButtonGroup;
import javax.swing.DefaultListModel;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JDialog;
import javax.swing.JFileChooser;
import javax.swing.JList;
import javax.swing.SpinnerNumberModel;
import javax.swing.SwingConstants;
import java.awt.GridBagConstraints;
import java.awt.Insets;
import java.awt.event.ActionEvent;

import javax.swing.JScrollPane;
import javax.swing.JRadioButton;
import javax.swing.JSpinner;

public class MapPortal extends Portal implements GPSClient.GPSHandler, CameraMoveListener {

	private static final long serialVersionUID = 1L;
	

	private static DecimalFormat df = new DecimalFormat("####.0000");
	ImageIcon red;
	ImageIcon green;
	ImageIcon yellow;
	WaypointDialog wd;
	ArrayList<Waypoint> waypoints;
	DefaultListModel model;
	ButtonGroup disp_group;
	ButtonGroup datum_group;
	MapPortal This = this;
	
	public GPSClient gpsclient = null;
	private JPanel jContentPane = null;
	private GpsData gpsdata = null;
	private GpsData camdata = null;
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
	private JPanel navPanel = null;
	private JButton topviewButton = null;
	public EarthSimPanel3D roverSimulationPanel3D = null;
	private JCheckBox textureCheckBox = null;
	private JCheckBox linesCheckBox = null;
	private JButton jButton = null;
	private JButton loadButton = null;
	private JFileChooser topoLoadFileChooser = null;  //  @jve:decl-index=0:visual-constraint="27,793"
	private JFileChooser saveFileChooser = null;   //  @jve:decl-index=0:visual-constraint="150,793"
	private JList waypointList = null;
	private JButton addWaypointButton = null;
	private JButton goButton = null;
	private JButton deleteButton = null;
	private JPanel optionsPanel = null;
	private JLabel jLabel6 = null;
	private JLabel jLabel7 = null;
	private JTextField coordTextField = null;
	private JTextField elevationTextField = null;
	private JLabel jLabel10 = null;
	private JScrollPane jScrollPane = null;
	private JCheckBox jCheckBox = null;
	private JRadioButton dmsRadioButton = null;
	private JRadioButton decimalRadioButton = null;
	private JLabel jLabel11 = null;
	private JCheckBox trackCheckBox = null;
	private JButton incLatButton = null;
	private JCheckBox downloadCheckBox = null;
	private JRadioButton dmRadioButton = null;
	private JCheckBox attitudeCheckBox = null;
	private JButton incLonButton = null;
	private JButton decLatButton = null;
	private JButton decLonButton = null;
	private JButton decElevationButton = null;
	private JButton incElevationButton = null;
	private JLabel jLabel12 = null;
	private JRadioButton wgs84RadioButton = null;
	private JRadioButton nad27RadioButton = null;
	private JButton propertiesButton = null;
	private JCheckBox vectorCheckBox = null;
	private JButton savePathButton = null;
	private JRadioButton utmRadioButton = null;
	/**
	 * This is the default constructor
	 */
	public MapPortal(Frame owner) {
		super(owner);
		
		red = new ImageIcon(getClass().getResource("/redlight.gif"));
		green = new ImageIcon(getClass().getResource("/greenlight.gif"));
		yellow = new ImageIcon(getClass().getResource("/yellowlight.gif"));
		waypoints = new ArrayList<Waypoint>();
		
		camdata = new GpsData();
		gpsdata = new GpsData();
		//test data
		
		gpsdata.Latitude = Double.parseDouble(Main.props.getProperty("starting_lat"));
		gpsdata.Longitude = Double.parseDouble(Main.props.getProperty("starting_lon"));
		gpsdata.Heading = 0;
		//gpsdata.Latitude = 38.40646766;
		//gpsdata.Longitude = -110.79159296;
		
		//gpsdata.Latitude = 40.24130317;
		//gpsdata.Longitude = -111.54451365;
		
		wd = new WaypointDialog(this);
		wd.lat = gpsdata.Latitude;
		wd.lon = gpsdata.Longitude;
		
		
		initialize();
		
		roverSimulationPanel3D.addCameraMoveListener(this);
		//roverSimulationPanel3D.setCenter(gpsdata.Latitude, gpsdata.Longitude);
		roverSimulationPanel3D.setRoverPos(gpsdata);
		roverSimulationPanel3D.SetInitialView();
		
		try{
			InetAddress mc_address = InetAddress.getByName(Main.props.getProperty("gps_address"));
			int port = Integer.parseInt(Main.props.getProperty("gps_port"));
			gpsclient = new GPSClient(mc_address, port);
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
	

	@Override
//	public void CameraMoved(double lat, double lon, double elevation) {
//		//System.out.println("Camera Moved");
//		latTextField.setText(GpsData.formatLat(lat));
//		lonTextField.setText(GpsData.formatLon(lon));
//		elevationTextField.setText(df.format(elevation) + " m");
//	}
	public void CameraMoved(GpsData cdata) {
		camdata = cdata;
		//System.out.println("Camera Moved");
		coordTextField.setText(GpsData.formatCoordinates(cdata.Latitude, cdata.Longitude));
		elevationTextField.setText(df.format(cdata.Elevation) + " m");
	}

	
	public void radioActionPerformed(ActionEvent arg0) {
		Object sender = arg0.getSource();
		if(sender == dmsRadioButton){
			GpsData.format_mode = GpsData.MODE_DMS;
		}else if(sender == decimalRadioButton){
			GpsData.format_mode = GpsData.MODE_DD;
		}else if(sender == utmRadioButton){
			GpsData.format_mode = GpsData.MODE_UTM;
		}else if(sender == dmRadioButton){
			GpsData.format_mode = GpsData.MODE_DDM;
		}else if(sender == wgs84RadioButton){
			GpsData.datum_mode = GpsData.WGS84;
		}else if(sender == nad27RadioButton){
			GpsData.datum_mode = GpsData.NAD27;
		}
		waypointList.repaint();
		//CameraMoved(roverSimulationPanel3D.clat, roverSimulationPanel3D.clon, roverSimulationPanel3D.celevation);
		CameraMoved(camdata);
	}
	
	/**
	 * This method initializes this
	 * 
	 * @return void
	 */
	private void initialize() {
		this.setSize(981, 696);
		this.setTitle("3D Map Portal");
		this.setContentPane(getJContentPane());
	}

	/**
	 * This method initializes jContentPane
	 * 
	 * @return javax.swing.JPanel
	 */
	private JPanel getJContentPane() {
		if (jContentPane == null) {
			GridBagConstraints gridBagConstraints3 = new GridBagConstraints();
			gridBagConstraints3.insets = new Insets(0,0,0,0);
			gridBagConstraints3.gridy = 2;
			gridBagConstraints3.gridx = 0;
			GridBagConstraints gridBagConstraints2 = new GridBagConstraints();
			gridBagConstraints2.insets = new Insets(0,0,0,0);
			gridBagConstraints2.gridx = 1;
			gridBagConstraints2.gridy = 0;
			gridBagConstraints2.weightx = 1.0D;
			gridBagConstraints2.weighty = 1.0D;
			gridBagConstraints2.fill = GridBagConstraints.BOTH;
			gridBagConstraints2.gridheight = 3;
			GridBagConstraints gridBagConstraints1 = new GridBagConstraints();
			gridBagConstraints1.insets = new Insets(0,0,0,0);
			gridBagConstraints1.gridy = 1;
			gridBagConstraints1.gridx = 0;
			GridBagConstraints gridBagConstraints = new GridBagConstraints();
			gridBagConstraints.insets = new Insets(0,0,0,0);
			gridBagConstraints.gridy = 0;
			gridBagConstraints.gridx = 0;
			jContentPane = new JPanel();
			jContentPane.setLayout(new GridBagLayout());
			jContentPane.add(getGpsPanel(), gridBagConstraints);
			jContentPane.add(getNavPanel(), gridBagConstraints1);
			jContentPane.add(getRoverSimulationPanel3D(), gridBagConstraints2);
			jContentPane.add(getOptionsPanel(), gridBagConstraints3);
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
			groundSpeedLabel.setBounds(new Rectangle(133, 120, 76, 20));
			groundSpeedLabel.setText("Unknown");
			jLabel5 = new JLabel();
			jLabel5.setBounds(new Rectangle(11, 123, 104, 16));
			jLabel5.setText("Ground Speed:");
			headingLabel = new JLabel();
			headingLabel.setBounds(new Rectangle(134, 101, 73, 19));
			headingLabel.setText("Unknown");
			elevationLabel = new JLabel();
			elevationLabel.setBounds(new Rectangle(134, 82, 76, 20));
			elevationLabel.setText("Unknown");
			jLabel4 = new JLabel();
			jLabel4.setBounds(new Rectangle(11, 102, 65, 21));
			jLabel4.setText("Heading:");
			jLabel3 = new JLabel();
			jLabel3.setText("Elevation (m)");
			jLabel3.setLocation(new Point(11, 85));
			jLabel3.setSize(new Dimension(80, 18));
			jLabel2 = new JLabel();
			jLabel2.setText("Coordinates:");
			jLabel2.setLocation(new Point(11, 65));
			jLabel2.setSize(new Dimension(79, 18));
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
			gpsPanel.setBorder(BorderFactory.createTitledBorder(null, "GPS System", TitledBorder.DEFAULT_JUSTIFICATION, TitledBorder.DEFAULT_POSITION, new Font("Dialog", Font.BOLD, 12), new Color(51, 51, 51)));
			gpsPanel.setPreferredSize(new Dimension(280, 145));
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
			latLongField.setLocation(new Point(91, 63));
		}
		return latLongField;
	}

	
	
	@Override
	public void GPSDataReceived(String data) {
		//System.out.println("GPS update received");
		if(gpsdata.MergeData(data)){
			//System.out.println("GPS update applied");
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
		latLongField.setText(GpsData.getFormatedPos(gpsdata));
		groundSpeedLabel.setText(""+gpsdata.Speed);
		
		gpsdata.time_stamp = System.currentTimeMillis();
		roverSimulationPanel3D.setRoverPos(gpsdata.clone());
		roverSimulationPanel3D.repaint();
		
	}

	/**
	 * This method initializes navPanel	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getNavPanel() {
		if (navPanel == null) {
			jLabel10 = new JLabel();
			jLabel10.setBounds(new Rectangle(8, 76, 81, 14));
			jLabel10.setText("Elevation (m)");
			jLabel7 = new JLabel();
			jLabel7.setBounds(new Rectangle(11, 18, 115, 18));
			jLabel7.setText("Camera Position");
			jLabel6 = new JLabel();
			jLabel6.setBounds(new Rectangle(6, 96, 86, 16));
			jLabel6.setText("Waypoints");
			navPanel = new JPanel();
			navPanel.setLayout(null);
			navPanel.setBorder(BorderFactory.createTitledBorder(null, "Navigation", TitledBorder.DEFAULT_JUSTIFICATION, TitledBorder.DEFAULT_POSITION, new Font("Dialog", Font.BOLD, 12), new Color(51, 51, 51)));
			navPanel.setPreferredSize(new Dimension(280, 230));
			navPanel.add(getTopviewButton(), null);
			navPanel.add(getJButton(), null);
			navPanel.add(jLabel6, null);
			navPanel.add(jLabel7, null);
			navPanel.add(getLatTextField(), null);
			navPanel.add(getElevationTextField(), null);
			navPanel.add(jLabel10, null);
			navPanel.add(getJScrollPane(), null);
			navPanel.add(getIncLatButton(), null);
			navPanel.add(getAddWaypointButton(), null);
			navPanel.add(getGoButton(), null);
			navPanel.add(getDeleteButton(), null);
			navPanel.add(getIncLonButton(), null);
			navPanel.add(getDecLatButton(), null);
			navPanel.add(getDecLonButton(), null);
			navPanel.add(getDecElevationButton(), null);
			navPanel.add(getIncElevationButton(), null);
			navPanel.add(getPropertiesButton(), null);
		}
		return navPanel;
	}

	/**
	 * This method initializes topviewButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getTopviewButton() {
		if (topviewButton == null) {
			topviewButton = new JButton();
			topviewButton.setLocation(new Point(232, 53));
			topviewButton.setText("");
			topviewButton.setFont(new Font("Dialog", Font.BOLD, 8));
			topviewButton.setIcon(new ImageIcon(getClass().getResource("/map_icon.png")));
			topviewButton.setToolTipText("Map View");
			topviewButton.setSize(new Dimension(35, 35));
			topviewButton.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					roverSimulationPanel3D.SetMapView();
				}
			});
		}
		return topviewButton;
	}

	/**
	 * This method initializes roverSimulationPanel3D	
	 * 	
	 * @return rover.components.RoverSimulationPanel3D	
	 */
	private EarthSimPanel3D getRoverSimulationPanel3D() {
		if (roverSimulationPanel3D == null) {
			roverSimulationPanel3D = new EarthSimPanel3D();
			roverSimulationPanel3D.setPreferredSize(new Dimension(100, 100));
		}
		return roverSimulationPanel3D;
	}

	/**
	 * This method initializes textureCheckBox	
	 * 	
	 * @return javax.swing.JCheckBox	
	 */
	private JCheckBox getTextureCheckBox() {
		if (textureCheckBox == null) {
			textureCheckBox = new JCheckBox();
			textureCheckBox.setSelected(true);
			textureCheckBox.setSize(new Dimension(150, 20));
			textureCheckBox.setLocation(new Point(5, 15));
			textureCheckBox.setText("Show Ground Texture");
			textureCheckBox.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					roverSimulationPanel3D.setDraw_textures(textureCheckBox.isSelected());
				}
			});
		}
		return textureCheckBox;
	}

	/**
	 * This method initializes linesCheckBox	
	 * 	
	 * @return javax.swing.JCheckBox	
	 */
	private JCheckBox getLinesCheckBox() {
		if (linesCheckBox == null) {
			linesCheckBox = new JCheckBox();
			linesCheckBox.setSelected(true);
			linesCheckBox.setSize(new Dimension(150, 20));
			linesCheckBox.setLocation(new Point(5, 35));
			linesCheckBox.setText("Show Ground Lines");
			linesCheckBox.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					roverSimulationPanel3D.setDraw_lines(linesCheckBox.isSelected());
				}
			});
		}
		return linesCheckBox;
	}

	/**
	 * This method initializes jButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getJButton() {
		if (jButton == null) {
			jButton = new JButton();
			jButton.setFont(new Font("Dialog", Font.BOLD, 8));
			jButton.setIcon(new ImageIcon(getClass().getResource("/rover_icon.png")));
			jButton.setToolTipText("Zoom to Rover");
			jButton.setLocation(new Point(232, 14));
			jButton.setSize(new Dimension(35, 35));
			jButton.setText("");
			jButton.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					roverSimulationPanel3D.SetRoverView();
				}
			});
		}
		return jButton;
	}

	/**
	 * This method initializes loadButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getLoadButton() {
		if (loadButton == null) {
			loadButton = new JButton();
			loadButton.setIcon(new ImageIcon(getClass().getResource("/open.png")));
			loadButton.setHorizontalTextPosition(SwingConstants.RIGHT);
			loadButton.setMargin(new Insets(0,0,0,0));
			loadButton.setBounds(new Rectangle(170, 152, 92, 25));
			loadButton.setText("Topology");
			loadButton.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					File curdir = new File ("./geocache");
					getTopoLoadFileChooser().setFileFilter(new ExtFilter(".hdr"));
					getTopoLoadFileChooser().setCurrentDirectory(curdir);
					getTopoLoadFileChooser().showOpenDialog(getJContentPane());
					File f = getTopoLoadFileChooser().getSelectedFile();
					if(f != null) roverSimulationPanel3D.loadTopoData(f.getAbsolutePath());
				}
			});
		}
		return loadButton;
	}

	/**
	 * This method initializes jFileChooser	
	 * 	
	 * @return javax.swing.JFileChooser	
	 */
	private JFileChooser getTopoLoadFileChooser() {
		if (topoLoadFileChooser == null) {
			topoLoadFileChooser = new JFileChooser();
			topoLoadFileChooser.setSize(new Dimension(501, 263));
		}
		return topoLoadFileChooser;
	}
	
	/**
	 * This method initializes saveFileChooser	
	 * 	
	 * @return javax.swing.JFileChooser	
	 */
	private JFileChooser getSaveFileChooser() {
		if (saveFileChooser == null) {
			saveFileChooser = new JFileChooser();
			saveFileChooser.setSize(new Dimension(501, 263));
		}
		return saveFileChooser;
	}

	
	/**
	 * This method initializes waypointList	
	 * 	
	 * @return javax.swing.JList	
	 */
	private JList getWaypointList() {
		if (waypointList == null) {
			model = new DefaultListModel();
			waypointList = new JList(model);
		}
		return waypointList;
	}

	/**
	 * This method initializes addWaypointButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getAddWaypointButton() {
		if (addWaypointButton == null) {
			addWaypointButton = new JButton();
			addWaypointButton.setIcon(new ImageIcon(getClass().getResource("/plus.png")));
			addWaypointButton.setBounds(new Rectangle(231, 117, 25, 25));
			addWaypointButton.setToolTipText("Add a waypoint");
			addWaypointButton.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					wd.lat = gpsdata.Latitude;
					wd.lon = gpsdata.Longitude;
					wd.elevation = gpsdata.Elevation;
					wd.showDialog();
				}
			});
		}
		return addWaypointButton;
	}

	/**
	 * This method initializes goButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getGoButton() {
		if (goButton == null) {
			goButton = new JButton();
			goButton.setIcon(new ImageIcon(getClass().getResource("/goto.png")));
			goButton.setBounds(new Rectangle(232, 146, 25, 25));
			goButton.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					Object ob = waypointList.getSelectedValue();
					if(ob == null) return;
					Waypoint wp = (Waypoint) ob;
					roverSimulationPanel3D.moveCameraTo(wp.lat, wp.lon);
				}
			});
		}
		return goButton;
	}

	/**
	 * This method initializes deleteButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getDeleteButton() {
		if (deleteButton == null) {
			deleteButton = new JButton();
			deleteButton.setIcon(new ImageIcon(getClass().getResource("/X.png")));
			deleteButton.setBounds(new Rectangle(232, 199, 25, 25));
			deleteButton.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					Object ob = waypointList.getSelectedValue();
					if(ob == null) return;
					Waypoint wp = (Waypoint) ob;
					removeWaypoint(wp);
				}
			});
		}
		return deleteButton;
	}

	/**
	 * This method initializes optionsPanel	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getOptionsPanel() {
		if (optionsPanel == null) {
			jLabel12 = new JLabel();
			jLabel12.setBounds(new Rectangle(171, 100, 84, 17));
			jLabel12.setText("Datum");
			jLabel11 = new JLabel();
			jLabel11.setBounds(new Rectangle(169, 11, 85, 15));
			jLabel11.setText("Display Format");
			optionsPanel = new JPanel();
			optionsPanel.setLayout(null);
			optionsPanel.setBorder(BorderFactory.createTitledBorder(null, "Options", TitledBorder.DEFAULT_JUSTIFICATION, TitledBorder.DEFAULT_POSITION, new Font("Dialog", Font.BOLD, 12), new Color(51, 51, 51)));
			optionsPanel.setPreferredSize(new Dimension(280, 210));
			optionsPanel.add(getTextureCheckBox(), null);
			optionsPanel.add(getLinesCheckBox(), null);
			optionsPanel.add(getLoadButton(), null);
			optionsPanel.add(getJCheckBox(), null);
			optionsPanel.add(getDmsRadioButton(), null);
			optionsPanel.add(getDecimalRadioButton(), null);
			optionsPanel.add(jLabel11, null);
			optionsPanel.add(getTrackCheckBox(), null);
			optionsPanel.add(getDownloadCheckBox(), null);
			optionsPanel.add(getDmRadioButton(), null);
			optionsPanel.add(getAttitudeCheckBox(), null);
			optionsPanel.add(jLabel12, null);
			optionsPanel.add(getWgs84RadioButton(), null);
			optionsPanel.add(getNad27RadioButton(), null);
			optionsPanel.add(getVectorCheckBox(), null);
			optionsPanel.add(getSavePathButton(), null);
			optionsPanel.add(getUtmRadioButton(), null);
			
			disp_group = new ButtonGroup();
		    disp_group.add(getDmsRadioButton());
		    disp_group.add(getDecimalRadioButton());
		    disp_group.add(getDmRadioButton());
		    disp_group.add(getUtmRadioButton());
			
		    datum_group = new ButtonGroup();
		    datum_group.add(getWgs84RadioButton());
		    datum_group.add(getNad27RadioButton());
		    
		}
		return optionsPanel;
	}

	/**
	 * This method initializes latTextField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	private JTextField getLatTextField() {
		if (coordTextField == null) {
			coordTextField = new JTextField();
			coordTextField.setBounds(new Rectangle(13, 37, 167, 18));
		}
		return coordTextField;
	}

	/**
	 * This method initializes elevationTextField	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	private JTextField getElevationTextField() {
		if (elevationTextField == null) {
			elevationTextField = new JTextField();
			elevationTextField.setBounds(new Rectangle(99, 77, 90, 19));
		}
		return elevationTextField;
	}

	public void addWaypoint(Waypoint wp) {
		this.roverSimulationPanel3D.addWaypoint(wp);
		model.addElement(wp);
		waypointList.repaint();
	}

	public void removeWaypoint(Waypoint wp){
		this.roverSimulationPanel3D.removeWaypoint(wp);
		model.removeElement(wp);
		waypointList.repaint();
	}
	
	/**
	 * This method initializes jScrollPane	
	 * 	
	 * @return javax.swing.JScrollPane	
	 */
	private JScrollPane getJScrollPane() {
		if (jScrollPane == null) {
			jScrollPane = new JScrollPane();
			jScrollPane.setBounds(new Rectangle(6, 117, 221, 106));
			jScrollPane.setViewportView(getWaypointList());
		}
		return jScrollPane;
	}

	/**
	 * This method initializes jCheckBox	
	 * 	
	 * @return javax.swing.JCheckBox	
	 */
	private JCheckBox getJCheckBox() {
		if (jCheckBox == null) {
			jCheckBox = new JCheckBox();
			jCheckBox.setSelected(true);
			jCheckBox.setSize(new Dimension(128, 20));
			jCheckBox.setLocation(new Point(5, 55));
			jCheckBox.setText("Show Compass");
			jCheckBox.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					roverSimulationPanel3D.setDrawCompass(jCheckBox.isSelected());
				}
			});
		}
		return jCheckBox;
	}

	/**
	 * This method initializes dmsRadioButton	
	 * 	
	 * @return javax.swing.JRadioButton	
	 */
	private JRadioButton getDmsRadioButton() {
		if (dmsRadioButton == null) {
			dmsRadioButton = new JRadioButton();
			dmsRadioButton.setText("0 0' 0.00\"");
			dmsRadioButton.setPreferredSize(new Dimension(60, 20));
			dmsRadioButton.setLocation(new Point(180, 30));
			dmsRadioButton.setSize(new Dimension(80, 15));
			if(GpsData.format_mode == GpsData.MODE_DMS) dmsRadioButton.setSelected(true);
			dmsRadioButton.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					radioActionPerformed(e);
				}
			});
		}
		return dmsRadioButton;
	}

	/**
	 * This method initializes decimalRadioButton	
	 * 	
	 * @return javax.swing.JRadioButton	
	 */
	private JRadioButton getDecimalRadioButton() {
		if (decimalRadioButton == null) {
			decimalRadioButton = new JRadioButton();
			if(GpsData.format_mode == GpsData.MODE_DD) decimalRadioButton.setSelected(true);
			decimalRadioButton.setLocation(new Point(180, 45));
			decimalRadioButton.setSize(new Dimension(80, 15));
			decimalRadioButton.setText("0.00000");
			decimalRadioButton.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					radioActionPerformed(e);
				}
			});
		}
		return decimalRadioButton;
	}

	/**
	 * This method initializes trackCheckBox	
	 * 	
	 * @return javax.swing.JCheckBox	
	 */
	private JCheckBox getTrackCheckBox() {
		if (trackCheckBox == null) {
			trackCheckBox = new JCheckBox();
			trackCheckBox.setText("Show Rover Path");
			trackCheckBox.setSize(new Dimension(150, 20));
			trackCheckBox.setSelected(true);
			trackCheckBox.setLocation(new Point(5, 95));
			trackCheckBox.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					roverSimulationPanel3D.setDrawPath(trackCheckBox.isSelected());
				}
			});
		}
		return trackCheckBox;
	}

	/**
	 * This method initializes incLatButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getIncLatButton() {
		if (incLatButton == null) {
			incLatButton = new JButton();
			incLatButton.setPreferredSize(new Dimension(20, 20));
			incLatButton.setMargin(new Insets(0,0,0,0));
			incLatButton.setLocation(new Point(199, 19));
			incLatButton.setSize(new Dimension(15, 15));
			incLatButton.setText("+");
			incLatButton.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					roverSimulationPanel3D.incCLat();
				}
			});
		}
		return incLatButton;
	}

	/**
	 * This method initializes downloadCheckBox	
	 * 	
	 * @return javax.swing.JCheckBox	
	 */
	private JCheckBox getDownloadCheckBox() {
		if (downloadCheckBox == null) {
			downloadCheckBox = new JCheckBox();
			downloadCheckBox.setText("Download Image Data");
			downloadCheckBox.setLocation(new Point(5, 140));
			downloadCheckBox.setSize(new Dimension(150, 20));
			downloadCheckBox.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					TexTileDB.FETCH_TILES = downloadCheckBox.isSelected();
					//TexTileDB.TILE_THROTTLE++;
				}
			});
		}
		return downloadCheckBox;
	}

	/**
	 * This method initializes dmRadioButton	
	 * 	
	 * @return javax.swing.JRadioButton	
	 */
	private JRadioButton getDmRadioButton() {
		if (dmRadioButton == null) {
			dmRadioButton = new JRadioButton();
			if(GpsData.format_mode == GpsData.MODE_DDM) dmRadioButton.setSelected(true);
			dmRadioButton.setLocation(new Point(180, 60));
			dmRadioButton.setSize(new Dimension(80, 15));
			dmRadioButton.setText("0 0.0000'");
			dmRadioButton.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					radioActionPerformed(e);
				}
			});
		}
		return dmRadioButton;
	}

	/**
	 * This method initializes attitudeCheckBox	
	 * 	
	 * @return javax.swing.JCheckBox	
	 */
	private JCheckBox getAttitudeCheckBox() {
		if (attitudeCheckBox == null) {
			attitudeCheckBox = new JCheckBox();
			attitudeCheckBox.setText("Show Attitude");
			attitudeCheckBox.setSize(new Dimension(150, 20));
			attitudeCheckBox.setSelected(true);
			attitudeCheckBox.setLocation(new Point(5, 75));
			attitudeCheckBox.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					roverSimulationPanel3D.setDrawAScale(attitudeCheckBox.isSelected());
				}
			});
		}
		return attitudeCheckBox;
	}

	/**
	 * This method initializes incLonButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getIncLonButton() {
		if (incLonButton == null) {
			incLonButton = new JButton();
			incLonButton.setText("+");
			incLonButton.setLocation(new Point(213, 33));
			incLonButton.setSize(new Dimension(15, 15));
			incLonButton.setMargin(new Insets(0, 0, 0, 0));
			incLonButton.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					roverSimulationPanel3D.incCLon();
				}
			});
		}
		return incLonButton;
	}

	/**
	 * This method initializes decLatButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getDecLatButton() {
		if (decLatButton == null) {
			decLatButton = new JButton();
			decLatButton.setMargin(new Insets(0, 0, 0, 0));
			decLatButton.setText("-");
			decLatButton.setLocation(new Point(198, 46));
			decLatButton.setSize(new Dimension(15, 15));
			decLatButton.setPreferredSize(new Dimension(20, 20));
			decLatButton.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					roverSimulationPanel3D.decCLat();
				}
			});
		}
		return decLatButton;
	}

	/**
	 * This method initializes decLonButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getDecLonButton() {
		if (decLonButton == null) {
			decLonButton = new JButton();
			decLonButton.setMargin(new Insets(0, 0, 0, 0));
			decLonButton.setText("-");
			decLonButton.setLocation(new Point(183, 33));
			decLonButton.setSize(new Dimension(15, 15));
			decLonButton.setPreferredSize(new Dimension(20, 20));
			decLonButton.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					roverSimulationPanel3D.decCLon();
				}
			});
		}
		return decLonButton;
	}

	/**
	 * This method initializes decElevationButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getDecElevationButton() {
		if (decElevationButton == null) {
			decElevationButton = new JButton();
			decElevationButton.setMargin(new Insets(0, 0, 0, 0));
			decElevationButton.setText("-");
			decElevationButton.setLocation(new Point(191, 78));
			decElevationButton.setSize(new Dimension(15, 15));
			decElevationButton.setPreferredSize(new Dimension(20, 20));
			decElevationButton.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					roverSimulationPanel3D.decCElevation();
				}
			});
		}
		return decElevationButton;
	}

	/**
	 * This method initializes incElevationButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getIncElevationButton() {
		if (incElevationButton == null) {
			incElevationButton = new JButton();
			incElevationButton.setText("+");
			incElevationButton.setLocation(new Point(209, 78));
			incElevationButton.setSize(new Dimension(15, 15));
			incElevationButton.setMargin(new Insets(0, 0, 0, 0));
			incElevationButton.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					roverSimulationPanel3D.incCElevation();
				}
			});
		}
		return incElevationButton;
	}

	/**
	 * This method initializes wgs84RadioButton	
	 * 	
	 * @return javax.swing.JRadioButton	
	 */
	private JRadioButton getWgs84RadioButton() {
		if (wgs84RadioButton == null) {
			wgs84RadioButton = new JRadioButton();
			wgs84RadioButton.setBounds(new Rectangle(178, 117, 77, 17));
			wgs84RadioButton.setSelected(true);
			wgs84RadioButton.setText("WGS 84");
			wgs84RadioButton.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					radioActionPerformed(e);
				}
			});
		}
		return wgs84RadioButton;
	}

	/**
	 * This method initializes nad27RadioButton	
	 * 	
	 * @return javax.swing.JRadioButton	
	 */
	private JRadioButton getNad27RadioButton() {
		if (nad27RadioButton == null) {
			nad27RadioButton = new JRadioButton();
			nad27RadioButton.setBounds(new Rectangle(178, 133, 79, 20));
			nad27RadioButton.setText("NAD 27");
			nad27RadioButton.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					radioActionPerformed(e);
				}
			});
		}
		return nad27RadioButton;
	}

	/**
	 * This method initializes propertiesButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getPropertiesButton() {
		if (propertiesButton == null) {
			propertiesButton = new JButton();
			propertiesButton.setIcon(new ImageIcon(getClass().getResource("/dialog.png")));
			propertiesButton.setLocation(new Point(232, 172));
			propertiesButton.setSize(new Dimension(25, 25));
			propertiesButton.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					Object ob = waypointList.getSelectedValue();
					if(ob == null) return;
					Waypoint wp = (Waypoint) ob;
					WaypointPropertiesDialog wpd = new WaypointPropertiesDialog(This, wp);
					wpd.showDialog();
				}
			});
		}
		return propertiesButton;
	}

	/**
	 * This method initializes vectorCheckBox	
	 * 	
	 * @return javax.swing.JCheckBox	
	 */
	private JCheckBox getVectorCheckBox() {
		if (vectorCheckBox == null) {
			vectorCheckBox = new JCheckBox();
			vectorCheckBox.setBounds(new Rectangle(5, 117, 142, 15));
			vectorCheckBox.setSelected(true);
			vectorCheckBox.setText("Show Rover Vector");
		}
		return vectorCheckBox;
	}

	/**
	 * This method initializes savePathButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getSavePathButton() {
		if (savePathButton == null) {
			savePathButton = new JButton();
			savePathButton.setBounds(new Rectangle(171, 181, 92, 23));
			savePathButton.setText("Save Path");
			savePathButton.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					File curdir = new File ("./Data");
					System.out.println(curdir.getAbsolutePath());
					getSaveFileChooser().setCurrentDirectory(curdir);
					getSaveFileChooser().showSaveDialog(getJContentPane());
					
					File saveFile = getSaveFileChooser().getSelectedFile();
					SavePath(saveFile);
				}
			});
		}
		return savePathButton;
	}
	
	public void SavePath(File f){
		synchronized(roverSimulationPanel3D.RoverPath){
		try{
			f.createNewFile();
			BufferedOutputStream bis = new BufferedOutputStream(new FileOutputStream(f));
			PrintStream p = new PrintStream(bis);
			Iterator<GpsData> itr = roverSimulationPanel3D.RoverPath.iterator();
			while(itr.hasNext()){
				GpsData d = itr.next();
				p.println(d.time_stamp + " " + d.Latitude + " " + d.Longitude + " " + d.Elevation + " " + d.Heading);
			}
			p.close();
		}catch(Exception e){
			e.printStackTrace();
		}
		}
	}

	/**
	 * This method initializes utmRadioButton	
	 * 	
	 * @return javax.swing.JRadioButton	
	 */
	private JRadioButton getUtmRadioButton() {
		if (utmRadioButton == null) {
			utmRadioButton = new JRadioButton();
			utmRadioButton.setBounds(new Rectangle(180, 74, 75, 16));
			utmRadioButton.setText("UTM");
			utmRadioButton.setSelected(true);
			utmRadioButton.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					radioActionPerformed(e);
				}
			});

		}
		return utmRadioButton;
	}
	


}
