package rover.input;

public class JoystickState {

	public float[] axes;
	public boolean[] buttons;
	
	public JoystickState(int axes, int buttons){
		this.axes = new float[axes];
		this.buttons = new boolean[buttons];
	}
	
	
}
