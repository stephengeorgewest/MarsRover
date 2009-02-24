package rover;

import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.util.Properties;

import javax.swing.UIManager;

import rover.portal.MainGUI;

public class Main {

	public static Properties props;
	
	public static void main(String[] args){
		try {
			
			System.setProperty("sun.java2d.noddraw", "true");
			
			ClassLoader cl = Main.class.getClassLoader();
			InputStream file = cl.getResourceAsStream("config.properties");

			props = new Properties();
	        props.load(file);

			UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
			MainGUI BaseStation = new MainGUI();
			BaseStation.setVisible(true);
			
		} catch(Exception e){
			e.printStackTrace();
		}
		
		
	}
	
}
