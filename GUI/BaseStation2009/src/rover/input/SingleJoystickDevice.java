package rover.input;

import net.java.games.input.Component;
import net.java.games.input.Controller;
import net.java.games.input.ControllerEnvironment;

public class SingleJoystickDevice implements InputDevice {

	Controller controller;
	
	JoystickState state;
	
	public SingleJoystickDevice(){
		state = new JoystickState(5, 0);
	}
	
	@Override
	public void AquireDevice() {
		// TODO Auto-generated method stub
		
		ControllerEnvironment ce = ControllerEnvironment.getDefaultEnvironment();
		
		System.out.println("Searching for joystick");
		
		Controller input = null;
		// retrieve the available controllers
		Controller[] controllers = ce.getControllers();
		for(int i = controllers.length-1;i>-1;i--){
			int analog_axes = InputUtils.analogAxes(controllers[i]);
			if(analog_axes >= 3 && analog_axes > InputUtils.analogAxes(input)){
				input = controllers[i];
			}
		}
		controller = input;
	}

	@Override
	public String getName() {
		return "Single Joystick Drive Controller";
	}

	@Override
	public JoystickState getState() {
		if(controller != null){
			controller.poll();

			float[] axes = new float[3];
			Component[] comps = controller.getComponents();
            int i = 0;
            for(Component comp : comps){
    			if(comp.isAnalog()){
    				axes[i] = comp.getPollData();
    				i++;
    				if(i >= axes.length) break;
    			}
    		}
            
            state.axes[0] = state.axes[2] = -1*axes[0]*(axes[1]+1)/2f;
    		state.axes[1] = state.axes[3] = axes[0]*(axes[1]-1)/2f;
            state.axes[4] = axes[2];
		}
		
		
		return state;
	}

	@Override
	public String getStatus() {
		if(controller != null){
			return controller.getName();
		}
		else
			return "Joystick not Found";
	}

	@Override
	public boolean isConnected() {
		if(controller != null && controller.poll())
			return true;	
		else return false;
	}

}
