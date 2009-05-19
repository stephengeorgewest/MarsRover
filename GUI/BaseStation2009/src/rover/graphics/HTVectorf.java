package rover.graphics;

public class HTVectorf {
	public float[] val;
	
	
	public HTVectorf(){
		val = new float[4];
		val[0] = val[1] = val[2] = val[3] = 0;
	}
	
	public HTVectorf(float[] initialvals){
		val = initialvals;
	}
	
	public HTVectorf(float x, float y, float z, float one){
		val = new float[4];
		val[0] = x;
		val[1] = y;
		val[2] = z;
		val[3] = one;
	}
	
	public HTVectorf copy(){
		float[] vals = {val[0], val[1], val[2], val[3]};
		return new HTVectorf(vals);
	}
	
	public HTVectorf crossp(HTVectorf vec){
		
		HTVectorf ret = new HTVectorf();
		
		ret.val[0] = val[1]*vec.val[2] - val[2]*vec.val[1];
		ret.val[1] = -(val[0]*vec.val[2] - val[2]*vec.val[0]);
		ret.val[2] = val[0]*vec.val[1] - val[1]*vec.val[0];
		
		return ret;
	}
	
	public void set(float x, float y, float z, float one){
		val[0] = x;
		val[1] = y;
		val[2] = z;
		val[3] = one;
	}
	
	public void add(HTVectorf vec){
		
		val[0] += vec.val[0];
		val[1] += vec.val[1];
		val[2] += vec.val[2];
	}

	public void sub(HTVectorf vec){
	
		val[0] -= vec.val[0];
		val[1] -= vec.val[1];
		val[2] -= vec.val[2];
	}
		
	public void mult(float m){
		val[0] *= m;
		val[1] *= m;
		val[2] *= m;
		val[3] *= m;
	}
	
	public HTVectorf getUnitVector(){
		HTVectorf ret = copy();
		ret.mult(1/getMagnitude());
		return ret;
	}
	
	public void normalize(){
		this.mult(1/getMagnitude());
	}
	
	public float getMagnitude(){
		return (float)(Math.sqrt(val[0]*val[0] + val[1]*val[1] + val[2]*val[2] + val[3]*val[3]));
	}
	
	public String toString(){
		return "{" + val[0] + " " + val[1] + " " + val[2] + " " + val[3] + "}";
	}
}
