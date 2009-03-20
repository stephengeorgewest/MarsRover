package rover.input;

import net.java.games.input.Component;
import net.java.games.input.Controller;
import net.java.games.input.ControllerEnvironment;

public class DoubleJoystickDevice implements InputDevice {

	Controller right, left;
	
	JoystickState state;
	
	public DoubleJoystickDevice(){
		state = new JoystickState(5, 0);
	}
	
	@Override
	public void AquireDevice() {
		// TODO Auto-generated method stub
		
		ControllerEnvironment ce = ControllerEnvironment.getDefaultEnvironment();
		
		System.out.println("Searching for right");
		Controller input = null;
		// retrieve the available controllers
		Controller[] controllers = ce.getControllers();
		for(int i = controllers.length-1;i>-1;i--){
			int analog_axes = InputUtils.analogAxes(controllers[i]);
			if(analog_axes >= 3 && analog_axes > InputUtils.analogAxes(input)){
				input = controllers[i];
			}
		}
		right = input;
		if(right == null){
			left = null;
			return;
		}
		
		System.out.println("Searching for left");
		input = null;
		for(int i = controllers.length-1;i>-1;i--){
			if(controllers[i] == right) continue;
			int analog_axes = InputUtils.analogAxes(controllers[i]);
			if(analog_axes >= 3 && analog_axes > InputUtils.analogAxes(input)){
				input = controllers[i];
			}
		}
		left = input;
	}

	@Override
	public String getName() {
		return "Double Joystick Drive Controller";
	}

	@Override
	public JoystickState getState() {
		if(right != null && left != null){
			right.poll();
			left.poll();

			Component[] comps = right.getComponents();
            
            int i = 0;
            for(Component comp : comps){
    			if(comp.isAnalog() && i == 0){
    				state.axes[1] = -comp.getPollData();
    				state.axes[3] = -comp.getPollData();
    				i++;
    			}else if(comp.isAnalog() && i == 1){
    				state.axes[4] = comp.getPollData();
    				break;
    			}
    		}
            comps = left.getComponents();
            for(Component comp : comps){
    			if(comp.isAnalog()){
    				state.axes[0] = -comp.getPollData();
    				state.axes[2] = -comp.getPollData();
    				break;
    			}
    		}
		}
		return state;
	}

	@Override
	public String getStatus() {
		String status = "";
		
		if(left != null){
			status += left.getName();
		} else status += "Not Found";
		
		status += " : ";
		
		if(right != null){
			status += right.getName();
		}else status += "Not Found";
		
		return status;
	}

	@Override
	public boolean isConnected() {
		if(left != null && left.poll() && right != null && right.poll())
			return true;	
		else return false;
	}


}
