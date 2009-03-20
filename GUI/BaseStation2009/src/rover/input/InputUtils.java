package rover.input;

import net.java.games.input.Component;
import net.java.games.input.Controller;

public class InputUtils {


	public static int analogAxes(Controller c){
		if(c == null) return -1;
		Component[] comps = c.getComponents(); 
		int analog_axes = 0;
		for(Component comp : comps){
			if(comp.isAnalog()) analog_axes++;
		}
		return analog_axes;
	}
	
	public static float scaleByCalibration(float min, float zero, float max, float value){
		float retval;
		if(value > zero){
			retval = (max-zero)*(value-50)/50 + zero;
		}else{
			retval = (zero-min)*(value-50)/50 + zero;
		}
		return retval;
	}
	
}
