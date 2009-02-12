package rover;

import java.awt.BorderLayout;
import java.awt.Rectangle;

import javax.swing.JPanel;
import javax.swing.JFrame;
import java.awt.Dimension;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Properties;

public class MainGUI extends JFrame {

	private static final long serialVersionUID = 1L;
	private JPanel jContentPane = null;

	public Properties configFile = null;
	
	ArrayList<Portal> PortalList = null;
	

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
		this.setSize(390, 155);
		this.setMaximumSize(new Dimension(500, 500));
		this.setContentPane(getJContentPane());
		this.setTitle("Main Control Portal");
	}

	/**
	 * This method initializes jContentPane
	 * 
	 * @return javax.swing.JPanel
	 */
	private JPanel getJContentPane() {
		if (jContentPane == null) {
			jContentPane = new JPanel();
			jContentPane.setLayout(new BorderLayout());
		}
		return jContentPane;
	}

}  //  @jve:decl-index=0:visual-constraint="10,10"
