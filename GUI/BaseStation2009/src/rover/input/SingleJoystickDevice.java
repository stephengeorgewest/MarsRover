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

			float x = 0,y = 0,z = 0, rz = 0;
			float[] buttons = new float[12];
			Component[] comps = controller.getComponents();
            int i = 0;
            for(Component comp : comps){
            	String ID = comp.getIdentifier().getName();
            	//System.out.println(ID);
            	if(Character.isDigit(ID.charAt(0))){
            		int b;
            		try{b = Integer.parseInt(ID);}
            		catch (Exception e){ continue;}
            		buttons[b] = comp.getPollData();
            		//System.out.println(buttons[b]);
            	}
            	else if(ID.compareTo("x") == 0){
            		x = comp.getPollData();
            	}
            	else if(ID.compareTo("y") == 0){
            		y = comp.getPollData();
            	}
            	else if(ID.compareTo("z") == 0 || ID.compareTo("slider") == 0){
            		z = comp.getPollData();
            	}
            	else if(ID.compareTo("rz") == 0){
            		rz = comp.getPollData();
            	}
            	//System.out.println(comp.getName());
            	//System.out.println();
            	//if(comp.getName().compareTo("))
//    			if(comp.isAnalog()){
//    				axes[i] = comp.getPollData();
//    				i++;
//    				if(i >= axes.length) break;
//    			}
    		}
            
//            state.axes[0] = state.axes[2] = -1*axes[0]*(axes[1]+1)/2f;
//    		state.axes[1] = state.axes[3] = axes[0]*(axes[1]-1)/2f;
//            state.axes[4] = axes[2];
            state.axes[0] = state.axes[2] = -1*y*(x+1)/2f;
    		state.axes[1] = state.axes[3] = y*(x-1)/2f;
            state.axes[4] = z;
            
            
            if(buttons[0] == 1){ //boost
            	state.axes[0] = Math.max(-1, Math.min(1, state.axes[0] * 2));
            	state.axes[1] = Math.max(-1, Math.min(1, state.axes[1] * 2));
            	state.axes[2] = Math.max(-1, Math.min(1, state.axes[2] * 2));
            	state.axes[3] = Math.max(-1, Math.min(1, state.axes[3] * 2));
            } else if(buttons[1] == 1){ //in place rotation
            	state.axes[0] = state.axes[2] = rz;
        		state.axes[1] = state.axes[3] = -1*rz;
            }
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
