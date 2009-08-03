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
            
			float x_r = 0,y_r = 0,z_r = 0, rz_r = 0;
			float[] buttons_r = new float[12];
			
            for(Component comp : comps){
            	String ID = comp.getIdentifier().getName();
            	//System.out.println(ID);
            	if(Character.isDigit(ID.charAt(0))){
            		int b;
            		try{b = Integer.parseInt(ID);}
            		catch (Exception e){ continue;}
            		buttons_r[b] = comp.getPollData();
            		//System.out.println(buttons[b]);
            	}
            	else if(ID.compareTo("x") == 0){x_r = comp.getPollData();}
            	else if(ID.compareTo("y") == 0){y_r = comp.getPollData();}
            	else if(ID.compareTo("z") == 0 || ID.compareTo("slider") == 0){z_r = comp.getPollData();}
            	else if(ID.compareTo("rz") == 0){rz_r = comp.getPollData();}
            }
            
            comps = left.getComponents();
            
            float x_l = 0,y_l = 0,z_l = 0, rz_l = 0;
			float[] buttons_l = new float[12];
			
            for(Component comp : comps){
            	String ID = comp.getIdentifier().getName();
            	//System.out.println(ID);
            	if(Character.isDigit(ID.charAt(0))){
            		int b;
            		try{b = Integer.parseInt(ID);}
            		catch (Exception e){ continue;}
            		buttons_l[b] = comp.getPollData();
            		//System.out.println(buttons[b]);
            	}
            	else if(ID.compareTo("x") == 0){x_l = comp.getPollData();}
            	else if(ID.compareTo("y") == 0){y_l = comp.getPollData();}
            	else if(ID.compareTo("z") == 0 || ID.compareTo("slider") == 0){z_l = comp.getPollData();}
            	else if(ID.compareTo("rz") == 0){rz_l = comp.getPollData();}
            }
            
            state.axes[0] = state.axes[2] = -1*y_l;
    		state.axes[1] = state.axes[3] = -1*y_r;
            state.axes[4] = rz_r;
            
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
