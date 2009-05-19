package rover.utils;

public class RoverArmKinematics {

	public static class invkinResult{
		//number of valid solutions
		public int num_sol;
		public float[][] solutions;
		
	}
	
	public static float[] fkin(float[] joint_angles){
		//TODO compute the forward kinematics
		return joint_angles;
	}
	
	public static invkinResult inv_kin(float[] xyzpt, float[] min, float[] max){
		invkinResult r = new invkinResult();
		if(xyzpt[0] > 2) r.num_sol = 0;
		else r.num_sol = 1;
		r.solutions = new float[1][5];
		r.solutions[0][0] = xyzpt[0];
		r.solutions[0][1] = xyzpt[1];
		r.solutions[0][2] = xyzpt[2];
		r.solutions[0][3] = xyzpt[3];
		r.solutions[0][4] = xyzpt[4];
		
		return r;
	}
	
	
}
