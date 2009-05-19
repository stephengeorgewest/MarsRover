package rover.portal;

import java.awt.BorderLayout;
import java.awt.Rectangle;

import javax.swing.JPanel;
import javax.swing.JFrame;

import rover.utils.*;
import java.awt.Dimension;
import java.io.IOException;
import java.net.InetAddress;
import java.util.ArrayList;
import java.util.Properties;

import javax.swing.ImageIcon;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.JLabel;
import javax.swing.table.AbstractTableModel;

import rover.guistuff.PortalModel;
import rover.guistuff.SocketModel;
import rover.network.SocketInfo;
import javax.swing.BorderFactory;
import javax.swing.border.EtchedBorder;
import javax.swing.JMenuBar;
import javax.swing.JMenu;
import javax.swing.JTabbedPane;
import java.awt.GridBagLayout;
import java.awt.GridBagConstraints;
import javax.swing.JComboBox;
import javax.swing.JList;
import java.awt.GridLayout;
import javax.swing.JButton;
import java.awt.Point;
import javax.swing.ListSelectionModel;

public class MainGUI extends JFrame {

	private static final long serialVersionUID = 1L;
	
	public static MainGUI instance = null;
	
	
	private JPanel jContentPane = null;

	public Properties configFile = null;
	
	public ArrayList<Portal> PortalList = null;
	private JScrollPane jScrollPane = null;
	private JTable jTable = null;
	private JLabel jLabel = null;
	
	ImageIcon red;
	ImageIcon green;
	ImageIcon yellow;
	
	InetAddress bsr;  //  @jve:decl-index=0:
	InetAddress rsr;  //  @jve:decl-index=0:
	InetAddress arm;
	InetAddress main;  //  @jve:decl-index=0:
	
	
	private PortalInit[] pinit = {new ControllerPortal.ControllerPortalInit(),
			new ServoPortal.ServoPortalInit(), 
			new VideoPortal.VideoPortalInit(), 
			new MapPortal.MapPortalInit(),
			new ArmPortal.ArmPortalInit(),
			new SurveyPortal.SurveyPortalInit()};
	
	
	/**
	 * This is the default constructor
	 * @throws IOException 
	 */
	public MainGUI() throws IOException {
		super();
		if(instance != null){
			System.out.println("Only one instance of the main gui is allowed");
			return;
		}
		instance = this;
		
		red = new ImageIcon(getClass().getResource("/redlight.gif"));
		green = new ImageIcon(getClass().getResource("/greenlight.gif"));
		yellow = new ImageIcon(getClass().getResource("/yellowlight.gif"));
		
		
		this.setDefaultCloseOperation(DISPOSE_ON_CLOSE);
		configFile = new Properties();
		configFile.load(this.getClass().getClassLoader().getResourceAsStream("config.properties"));
		
		
		
		//TODO: Add xml file parser that repopulates the Portal list from a layout file
		
		PortalList = new ArrayList<Portal>();
		
		//for now we'll hardcode the layout
//		PortalList.add(new ServoPortal(this));
//		PortalList.get(0).setLocation(10, 500);
//		PortalList.add(new VideoPortal(this));
//		PortalList.get(1).setLocation(500, 10);
//		PortalList.add(new MapPortal(this));
//		PortalList.get(2).setLocation(500,500);
//		PortalList.add(new ControllerPortal(this));
//		PortalList.get(3).setLocation(500, 300);
//		
//		//PortalList.add(new )
//		for(int i = 0;i<PortalList.size();i++){
//			PortalList.get(i).setVisible(true);
//		}

		initialize();
		updateIPs();
	}
	
	public void addNewPortal(Portal p){
		PortalList.add(p);
		p.setVisible(true);
	}
	
	static SocketModel model = new SocketModel();

	private JMenuBar jJMenuBar = null;

	private JMenu fileMenu = null;

	private JMenu viewMenu = null;

	private JTabbedPane jTabbedPane = null;

	private JPanel portalPanel = null;

	private JPanel socketPanel = null;

	private JList jList = null;

	private JList jList1 = null;

	private JLabel jLabel1 = null;

	private JLabel jLabel2 = null;

	private JButton addButton = null;

	private JButton removeButton = null;

	private JPanel tracePanel = null;

	private JLabel jLabel3 = null;

	private JLabel jLabel4 = null;

	private JLabel jLabel5 = null;

	private JLabel jLabel6 = null;

	private JLabel armicon = null;

	private JLabel mainicon = null;

	private JLabel rsricon = null;

	private JLabel bsricon = null;

	private JButton pingButton = null;

	private JLabel mainip = null;

	private JLabel armip = null;

	private JLabel rsrip = null;

	private JLabel bsrip = null;

	public static void registerSocket(SocketInfo si){
		System.out.println("New socket Registered");
		model.addSocket(si);
	}
	
	public static void unregisterSocket(SocketInfo si){
		System.out.println("socket UnRegistered");
		model.removeSocket(si);
	}
	
	public static void removeSocket(SocketInfo si){
		model.removeSocket(si);
	}
	
	
	/**
	 * This method initializes this
	 * 
	 * @return void
	 */
	private void initialize() {
		this.setSize(486, 244);
		this.setJMenuBar(getJJMenuBar());
		this.setMaximumSize(new Dimension(500, 500));
		this.setContentPane(getJContentPane());
		this.setTitle("Main Portal");
		this.addWindowListener(new java.awt.event.WindowListener() {
			public void windowClosing(java.awt.event.WindowEvent e) {
				for(Portal p : PortalList){
					p.Shutdown();
				}
			}
			public void windowOpened(java.awt.event.WindowEvent e) {
			}
			public void windowClosed(java.awt.event.WindowEvent e) {
			}
			public void windowIconified(java.awt.event.WindowEvent e) {
			}
			public void windowDeiconified(java.awt.event.WindowEvent e) {
			}
			public void windowActivated(java.awt.event.WindowEvent e) {
			}
			public void windowDeactivated(java.awt.event.WindowEvent e) {
			}
		});
	}

	/**
	 * This method initializes jContentPane
	 * 
	 * @return javax.swing.JPanel
	 */
	private JPanel getJContentPane() {
		if (jContentPane == null) {
			GridLayout gridLayout = new GridLayout();
			gridLayout.setRows(1);
			jLabel = new JLabel();
			jLabel.setText("Network Sockets");
			jLabel.setBounds(new Rectangle(4, 6, 99, 18));
			jContentPane = new JPanel();
			jContentPane.setLayout(gridLayout);
			jContentPane.add(getJTabbedPane(), null);
		}
		return jContentPane;
	}

	/**
	 * This method initializes jScrollPane	
	 * 	
	 * @return javax.swing.JScrollPane	
	 */
	private JScrollPane getJScrollPane() {
		if (jScrollPane == null) {
			jScrollPane = new JScrollPane();
			jScrollPane.setBorder(BorderFactory.createEtchedBorder(EtchedBorder.LOWERED));
			jScrollPane.setBounds(new Rectangle(6, 29, 458, 129));
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
			jTable = new JTable(model);
			jTable.getColumnModel().getColumn(0).setPreferredWidth(100);
			jTable.getColumnModel().getColumn(1).setPreferredWidth(50);
			jTable.getColumnModel().getColumn(2).setPreferredWidth(50);
			jTable.getColumnModel().getColumn(3).setPreferredWidth(25);
			jTable.getColumnModel().getColumn(4).setPreferredWidth(100);
			jTable.getColumnModel().getColumn(5).setPreferredWidth(25);
			jTable.getColumnModel().getColumn(6).setPreferredWidth(50);
			jTable.setShowGrid(true);
		}
		return jTable;
	}
	/**
	 * This method initializes jJMenuBar	
	 * 	
	 * @return javax.swing.JMenuBar	
	 */
	private JMenuBar getJJMenuBar() {
		if (jJMenuBar == null) {
			jJMenuBar = new JMenuBar();
			jJMenuBar.add(getFileMenu());
			jJMenuBar.add(getViewMenu());
		}
		return jJMenuBar;
	}
	/**
	 * This method initializes fileMenu	
	 * 	
	 * @return javax.swing.JMenu	
	 */
	private JMenu getFileMenu() {
		if (fileMenu == null) {
			fileMenu = new JMenu();
			fileMenu.setText("File");
		}
		return fileMenu;
	}
	/**
	 * This method initializes viewMenu	
	 * 	
	 * @return javax.swing.JMenu	
	 */
	private JMenu getViewMenu() {
		if (viewMenu == null) {
			viewMenu = new JMenu();
			viewMenu.setText("View");
		}
		return viewMenu;
	}
	/**
	 * This method initializes jTabbedPane	
	 * 	
	 * @return javax.swing.JTabbedPane	
	 */
	private JTabbedPane getJTabbedPane() {
		if (jTabbedPane == null) {
			jTabbedPane = new JTabbedPane();
			//jTabbedPane.addTab("Portal Manager", component)
			jTabbedPane.addTab("Portal Manager", null, getPortalPanel(), null);
			jTabbedPane.addTab("Socket Manager", null, getSocketPanel(), null);
			jTabbedPane.addTab("Network Trace", null, getTracePanel(), null);
		}
		return jTabbedPane;
	}
	/**
	 * This method initializes portalPanel	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getPortalPanel() {
		if (portalPanel == null) {
			jLabel2 = new JLabel();
			jLabel2.setBounds(new Rectangle(278, 13, 134, 19));
			jLabel2.setText("Currently Active Portals");
			jLabel1 = new JLabel();
			jLabel1.setBounds(new Rectangle(27, 10, 101, 21));
			jLabel1.setText("Available Portals");
			portalPanel = new JPanel();
			portalPanel.setLayout(null);
			portalPanel.add(getJList(), null);
			portalPanel.add(getJList1(), null);
			portalPanel.add(jLabel1, null);
			portalPanel.add(jLabel2, null);
			portalPanel.add(getAddButton(), null);
			portalPanel.add(getRemoveButton(), null);
		}
		return portalPanel;
	}
	/**
	 * This method initializes socketPanel	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getSocketPanel() {
		if (socketPanel == null) {
			socketPanel = new JPanel();
			socketPanel.setLayout(null);
			socketPanel.add(getJScrollPane(), null);
			socketPanel.add(jLabel, null);
		}
		return socketPanel;
	}
	/**
	 * This method initializes jList	
	 * 	
	 * @return javax.swing.JList	
	 */
	private JList getJList() {
		if (jList == null) {
			jList = new JList(pinit);
			jList.setBounds(new Rectangle(18, 35, 166, 123));
			jList.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
		}
		return jList;
	}
	/**
	 * This method initializes jList1	
	 * 	
	 * @return javax.swing.JList	
	 */
	private JList getJList1() {
		if (jList1 == null) {
			jList1 = new JList(new PortalModel());
			jList1.setBounds(new Rectangle(278, 35, 161, 123));
			jList1.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
			jList1.addMouseListener(new java.awt.event.MouseAdapter() {
				public void mouseClicked(java.awt.event.MouseEvent e) {
					if(e.getClickCount() == 2){
						Portal p = (Portal)(jList1.getSelectedValue());
						p.setVisible(true);
					}
				}
			});
		}
		return jList1;
	}
	/**
	 * This method initializes addButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getAddButton() {
		if (addButton == null) {
			addButton = new JButton();
			addButton.setText(">>");
			addButton.setSize(new Dimension(50, 20));
			addButton.setLocation(new Point(199, 59));
			addButton.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					PortalInit pi = (PortalInit)(getJList().getSelectedValue());
					MainGUI.instance.addNewPortal(pi.createPortal());
					PortalModel pm = (PortalModel)(jList1.getModel());
					pm.setPortalList(MainGUI.instance.PortalList);
				}
			});
		}
		return addButton;
	}
	/**
	 * This method initializes removeButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getRemoveButton() {
		if (removeButton == null) {
			removeButton = new JButton();
			removeButton.setText("<<");
			removeButton.setSize(new Dimension(50, 20));
			removeButton.setLocation(new Point(200, 92));
			removeButton.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					Object ob = jList1.getSelectedValue();
					if(ob == null) return;
					Portal p = (Portal)(ob);
					p.Shutdown();
					p.dispose();
					MainGUI.instance.PortalList.remove(p);
					PortalModel pm = (PortalModel)(jList1.getModel());
					pm.setPortalList(MainGUI.instance.PortalList);
				}
			});
		}
		return removeButton;
	}

	/**
	 * This method initializes tracePanel	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getTracePanel() {
		if (tracePanel == null) {
			bsrip = new JLabel();
			bsrip.setBounds(new Rectangle(28, 35, 93, 20));
			bsrip.setText("192.168.88.100");
			rsrip = new JLabel();
			rsrip.setBounds(new Rectangle(35, 103, 92, 23));
			rsrip.setText("192.168.88.101");
			armip = new JLabel();
			armip.setBounds(new Rectangle(270, 36, 94, 25));
			armip.setText("192.168.88.50");
			mainip = new JLabel();
			mainip.setBounds(new Rectangle(271, 106, 92, 22));
			mainip.setText("192.168.88.44");
			bsricon = new JLabel();
			bsricon.setText("");
			bsricon.setSize(new Dimension(50, 50));
			bsricon.setLocation(new Point(145, 14));
			bsricon.setIcon(red);
			rsricon = new JLabel();
			rsricon.setText("");
			rsricon.setSize(new Dimension(50, 50));
			rsricon.setLocation(new Point(144, 77));
			rsricon.setIcon(red);
			mainicon = new JLabel();
			mainicon.setText("");
			mainicon.setSize(new Dimension(50, 50));
			mainicon.setLocation(new Point(375, 74));
			mainicon.setIcon(red);
			armicon = new JLabel();
			armicon.setText("");
			armicon.setSize(new Dimension(50, 50));
			armicon.setLocation(new Point(373, 9));
			armicon.setIcon(red);
			jLabel6 = new JLabel();
			jLabel6.setBounds(new Rectangle(276, 83, 87, 19));
			jLabel6.setText("Main Board");
			jLabel5 = new JLabel();
			jLabel5.setBounds(new Rectangle(275, 11, 83, 25));
			jLabel5.setText("ARM9 Board");
			jLabel4 = new JLabel();
			jLabel4.setBounds(new Rectangle(60, 79, 32, 21));
			jLabel4.setText("RSR");
			jLabel3 = new JLabel();
			jLabel3.setBounds(new Rectangle(57, 10, 35, 19));
			jLabel3.setText("BSR");
			tracePanel = new JPanel();
			tracePanel.setLayout(null);
			tracePanel.add(jLabel3, null);
			tracePanel.add(jLabel4, null);
			tracePanel.add(jLabel5, null);
			tracePanel.add(jLabel6, null);
			tracePanel.add(armicon, null);
			tracePanel.add(mainicon, null);
			tracePanel.add(rsricon, null);
			tracePanel.add(bsricon, null);
			tracePanel.add(getPingButton(), null);
			tracePanel.add(mainip, null);
			tracePanel.add(armip, null);
			tracePanel.add(rsrip, null);
			tracePanel.add(bsrip, null);
		}
		return tracePanel;
	}

	/**
	 * This method initializes pingButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getPingButton() {
		if (pingButton == null) {
			pingButton = new JButton();
			pingButton.setBounds(new Rectangle(17, 133, 122, 29));
			pingButton.setText("Ping All");
			pingButton.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					try{
						int timeOut = 2000;
						boolean bsr_available = bsr.isReachable(timeOut);
						if(bsr_available) bsricon.setIcon(green);
						else bsricon.setIcon(red);
						boolean rsr_available = rsr.isReachable(timeOut);
						if(rsr_available) rsricon.setIcon(green);
						else rsricon.setIcon(red);
						boolean arm_available = arm.isReachable(timeOut);
						if(arm_available) armicon.setIcon(green);
						else armicon.setIcon(red);
						boolean main_available = main.isReachable(timeOut);
						if(main_available) mainicon.setIcon(green);
						else mainicon.setIcon(red);
						
					} catch (Exception ex){
						ex.printStackTrace();
					}
				}
			});
		}
		return pingButton;
	}
	
	private void updateIPs(){
		try{
			bsr = InetAddress.getByName(configFile.getProperty("bsr_ip"));
			rsr = InetAddress.getByName(configFile.getProperty("rsr_ip"));
			arm = InetAddress.getByName(configFile.getProperty("arm_ip"));
			main = InetAddress.getByName(configFile.getProperty("main_ip"));
		} catch (Exception e){
			e.printStackTrace();
		}
		
		if(rsr != null) rsrip.setText(rsr.getHostAddress());
		else rsrip.setText("null");
		if(bsr != null) bsrip.setText(bsr.getHostAddress());
		else bsrip.setText("null");
		if(arm != null) armip.setText(arm.getHostAddress());
		else armip.setText("null");
		if(main != null) mainip.setText(main.getHostAddress());
		else mainip.setText("null");
	}

}  //  @jve:decl-index=0:visual-constraint="10,10"
