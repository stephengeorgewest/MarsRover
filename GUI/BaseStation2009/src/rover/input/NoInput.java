package rover.input;

public class NoInput implements InputDevice {

	@Override
	public void AquireDevice() {
		// TODO Auto-generated method stub
		
	}

	@Override
	public String getName() {
		// TODO Auto-generated method stub
		return "Null Input Device";
	}

	@Override
	public JoystickState getState() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public String getStatus() {
		// TODO Auto-generated method stub
		return "Input Disabled";
	}

	@Override
	public boolean isConnected() {
		// TODO Auto-generated method stub
		return false;
	}

}
