package rover.graphics;

public class HTVectord {
	public double[] val;
	
	
	public HTVectord(){
		val = new double[4];
		val[0] = val[1] = val[2] = val[3] = 0;
	}
	
	public HTVectord(double[] initialvals){
		val = initialvals;
	}
	
	public HTVectord(double x, double y, double z, double one){
		val = new double[4];
		val[0] = x;
		val[1] = y;
		val[2] = z;
		val[3] = one;
	}
	
	public HTVectord copy(){
		double[] vals = {val[0], val[1], val[2], val[3]};
		return new HTVectord(vals);
	}
	
	public HTVectord crossp(HTVectord vec){
		
		HTVectord ret = new HTVectord();
		
		ret.val[0] = val[1]*vec.val[2] - val[2]*vec.val[1];
		ret.val[1] = -(val[0]*vec.val[2] - val[2]*vec.val[0]);
		ret.val[2] = val[0]*vec.val[1] - val[1]*vec.val[0];
		
		return ret;
	}
	
	public void set(double x, double y, double z, double one){
		val[0] = x;
		val[1] = y;
		val[2] = z;
		val[3] = one;
	}
	
	public void add(HTVectord vec){
		
		val[0] += vec.val[0];
		val[1] += vec.val[1];
		val[2] += vec.val[2];
	}

	public void sub(HTVectord vec){
	
		val[0] -= vec.val[0];
		val[1] -= vec.val[1];
		val[2] -= vec.val[2];
	}
		
	public void mult(double m){
		val[0] *= m;
		val[1] *= m;
		val[2] *= m;
		val[3] *= m;
	}
	
	public HTVectord getUnitVector(){
		HTVectord ret = copy();
		ret.mult(1/getMagnitude());
		return ret;
	}
	
	public void normalize(){
		this.mult(1/getMagnitude());
	}
	
	public double getMagnitude(){
		return (double)(Math.sqrt(val[0]*val[0] + val[1]*val[1] + val[2]*val[2] + val[3]*val[3]));
	}
	
	public String toString(){
		return "{" + val[0] + " " + val[1] + " " + val[2] + " " + val[3] + "}";
	}
}
