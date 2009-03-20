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
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.JLabel;
import javax.swing.table.AbstractTableModel;

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
	
	private PortalInit[] pinit = {new ControllerPortal.ControllerPortalInit(),
			new ServoPortal.ServoPortalInit(), 
			new VideoPortal.VideoPortalInit(), 
			new MapPortal.MapPortalInit(),
			new ArmPortal.ArmPortalInit()};
	
	
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
		initialize();
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
		this.setSize(456, 241);
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
			jScrollPane.setBounds(new Rectangle(6, 29, 431, 126));
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
			jLabel2.setBounds(new Rectangle(246, 12, 134, 19));
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
			jList.setBounds(new Rectangle(18, 35, 166, 114));
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
			jList1.setBounds(new Rectangle(247, 36, 161, 117));
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
			addButton.setLocation(new Point(188, 59));
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
			removeButton.setLocation(new Point(190, 94));
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

}  //  @jve:decl-index=0:visual-constraint="10,10"
