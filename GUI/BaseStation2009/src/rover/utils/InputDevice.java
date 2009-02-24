package rover.utils;

import net.java.games.input.Controller;

public class InputDevice {

	public Controller controller;
	
	public InputDevice(Controller c){
		controller = c;
	}
	
	public InputDevice(){
		controller = null;
	}
	
	@Override
	public String toString(){
		if(controller != null) return controller.getName();
		else return "Scanning for Controllers";
	}
	
	
}
