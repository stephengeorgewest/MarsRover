package rover;

import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.URL;
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
	
	public static void writeConfig(){
		ClassLoader cl = Main.class.getClassLoader();
		URL address = cl.getResource("config.properties");
		try{
			String path = address.toURI().getPath();
			System.out.println("Saving config at " + path);
			OutputStream file = new FileOutputStream(path);
			props.store(file, "Program Generated Config File");	
		} catch (Exception e){
			e.printStackTrace();
		}


	}
	
}
