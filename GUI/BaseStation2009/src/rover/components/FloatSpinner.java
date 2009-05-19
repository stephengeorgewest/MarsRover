package rover.guistuff;

import javax.swing.AbstractSpinnerModel;
import javax.swing.JSpinner;

public class FloatSpinner extends JSpinner{

	FloatSpinnerModel model;
	
	public FloatSpinner(float value, float min, float max, float increment){
		super();
		model = new FloatSpinnerModel(value, min, max, increment);
		super.setModel(model);
		super.setEditor(createEditor(model));
	}
	
	
	
	class FloatSpinnerModel extends AbstractSpinnerModel{
		
		float current_value;
		float increment;
		float min;
		float max;
		
		public FloatSpinnerModel(float value, float min, float max, float increment){
			this.current_value = value;
			this.min = min;
			this.max = max;
			this.increment = increment;
		}
		
		@Override
		public Object getNextValue() {
			return new Float(Math.min(max, current_value+increment));
		}

		@Override
		public Object getPreviousValue() {
			return new Float(Math.max(min, current_value-increment));
		}

		@Override
		public Object getValue() {
			return new Float(current_value);
		}

		@Override
		public void setValue(Object value) {
			Float f = (Float)value;
			current_value = f.floatValue();
			current_value = Math.min(current_value, max);
			current_value = Math.max(current_value, min);
		}
		
		
	}
	
	
}
