package rover.input;

import net.java.games.input.Component;
import net.java.games.input.Controller;

public interface InputDevice {

	public String getName();
	
	public String getStatus();
	
	public boolean isConnected(); 
	
	public JoystickState getState();
	
	public void AquireDevice();
	
}
