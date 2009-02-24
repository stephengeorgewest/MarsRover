package rover.portal;

import java.awt.BorderLayout;
import java.awt.Rectangle;

import javax.swing.JPanel;
import javax.swing.JFrame;


import java.awt.Dimension;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Properties;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.JLabel;

public class MainGUI extends JFrame {

	private static final long serialVersionUID = 1L;
	private JPanel jContentPane = null;

	public Properties configFile = null;
	
	ArrayList<Portal> PortalList = null;
	private JScrollPane jScrollPane = null;
	private JTable jTable = null;
	private JLabel jLabel = null;
	

	/**
	 * This is the default constructor
	 * @throws IOException 
	 */
	public MainGUI() throws IOException {
		super();
		initialize();
		this.setDefaultCloseOperation(DISPOSE_ON_CLOSE);
		configFile = new Properties();
		configFile.load(this.getClass().getClassLoader().getResourceAsStream("config.properties"));

		//TODO: Add xml file parser that repopulates the Portal list from a layout file
		
		PortalList = new ArrayList<Portal>();
		
		//for now we'll hardcode the layout
		PortalList.add(new ServoPortal(this));
		PortalList.get(0).setLocation(10, 500);
		PortalList.add(new VideoPortal(this));
		PortalList.get(1).setLocation(500, 10);
		PortalList.add(new MapPortal(this));
		PortalList.get(2).setLocation(500,500);
		PortalList.add(new ControllerPortal(this));
		PortalList.get(3).setLocation(500, 300);
		
		//PortalList.add(new )
		for(int i = 0;i<PortalList.size();i++){
			PortalList.get(i).setVisible(true);
		}

		
		
		
	}

	/**
	 * This method initializes this
	 * 
	 * @return void
	 */
	private void initialize() {
		this.setSize(383, 289);
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
			jLabel = new JLabel();
			jLabel.setBounds(new Rectangle(15, 8, 99, 18));
			jLabel.setText("Network Sockets");
			jContentPane = new JPanel();
			jContentPane.setLayout(null);
			jContentPane.add(getJScrollPane(), null);
			jContentPane.add(jLabel, null);
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
			jScrollPane.setBounds(new Rectangle(16, 43, 310, 182));
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
			jTable = new JTable();
		}
		return jTable;
	}

}  //  @jve:decl-index=0:visual-constraint="10,10"
