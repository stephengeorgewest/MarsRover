package rover.graphics;

public class HTMatrixf {
	public float[][] val;
	
	public HTMatrixf(){
		val = new float[4][4];
		makeIdentity();
	}

	//note, opengl matrices are in column major order
	public float[] get1DArray(){
		float[] retval = new float[16];
		for(int i = 0;i<4;i++)
			for(int j = 0;j<4;j++){
				retval[j*4+i] = val[i][j];
			}
		return retval;
	}
	public HTMatrixf copy(){
		HTMatrixf retval = new HTMatrixf();
		for(int i = 0;i<4;i++)
			for(int j = 0;j<4;j++){
				retval.val[i][j] = val[i][j];
			}
		return retval;
	}
	
	public void orthonormalize(){
		HTVectorf xc = new HTVectorf(val[0][0], val[1][0], val[2][0], 0);
		HTVectorf yc = new HTVectorf(val[0][1], val[1][1], val[2][1], 0);
		HTVectorf zc = xc.crossp(yc);
		zc.normalize();
		yc = zc.crossp(xc);
		xc.normalize();
		yc.normalize();
		zc.normalize();
		
		for(int i = 0;i<3;i++){
			val[i][0] = xc.val[i];
			val[i][1] = yc.val[i];
			val[i][2] = zc.val[i];
		}
		
	}

	
	
	public void premultiply(HTMatrixf m){
		float[][] res = new float[4][4];
		for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                for (int k = 0; k < 4; k++)
                    res[i][j] += m.val[i][k] * val[k][j];
		val = res;
	}
	public void postmultiply(HTMatrixf m){
		float[][] res = new float[4][4];
		for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                for (int k = 0; k < 4; k++)
                    res[i][j] += val[i][k] * m.val[k][j];
		val = res;
	}
	public float[] vecmultiply(float[] vec){
		float[] res = new float[4];
		for (int i = 0; i < 4; i++){
			res[i] = 0;
            for (int j = 0; j < 4; j++)
                res[i] += val[i][j] * vec[j]; 
		}
		return res;
	}
	
	public HTVectorf vecmultiply(HTVectorf vec){
		HTVectorf res = new HTVectorf();
		for (int i = 0; i < 4; i++){
			res.val[i] = 0;
            for (int j = 0; j < 4; j++)
                res.val[i] += val[i][j] * vec.val[j]; 
		}
		return res;
	}
	
	public void Rotate(float alpha, float x, float y, float z){
		//System.out.println("applying rotation");
		//System.out.println(this);
		HTMatrixf r = new HTMatrixf();
		r.makeScrewRotation(alpha, x, y, z);
		//System.out.println(r);
		this.postmultiply(r);
		//System.out.println(this);
	}
	public void Translate(float x, float y, float z){
		val[0][3] += x;
		val[1][3] += y;
		val[2][3] += z;
	}
	
	public void Translate(HTVectorf vec){
		val[0][3] += vec.val[0];
		val[1][3] += vec.val[1];
		val[2][3] += vec.val[2];
	}
	
	public void unTranslate(HTVectorf vec){
		val[0][3] -= vec.val[0];
		val[1][3] -= vec.val[1];
		val[2][3] -= vec.val[2];
	}
	
	public void setP(HTVectorf np){
		val[0][3] = np.val[0];
		val[1][3] = np.val[1];
		val[2][3] = np.val[2];
	}
	
	public void Invert(){
		float[][] nval = new float[4][4];
		
		for(int i = 0;i<3;i++)
			for(int j = 0;j<3;j++){
				nval[j][i] = val[i][j];
			}
		
		nval[0][3] = -(nval[0][0]*val[0][3] + nval[0][1]*val[1][3] + nval[0][2]*val[2][3]);
		nval[1][3] = -(nval[1][0]*val[0][3] + nval[1][1]*val[1][3] + nval[1][2]*val[2][3]);
		nval[2][3] = -(nval[2][0]*val[0][3] + nval[2][1]*val[1][3] + nval[2][2]*val[2][3]);
		
		nval[3][3] = 1;
		
		val = nval;
	}
	
	public HTMatrixf getInverse(){
		HTMatrixf r = this.copy();
		r.Invert();
		return r;
	}
	
	public void makeScrewRotation(float alpha, float x, float y, float z){
		val[0][0] = (float) (x*x*(1-Math.cos(alpha)) + Math.cos(alpha));
		val[0][1] = (float) (y*x*(1-Math.cos(alpha)) - z*Math.sin(alpha));
		val[0][2] = (float) (z*x*(1-Math.cos(alpha)) + y*Math.sin(alpha));
		val[0][3] = 0;
		
		val[1][0] = (float) (x*y*(1-Math.cos(alpha)) + z*Math.sin(alpha));
		val[1][1] = (float) (y*y*(1-Math.cos(alpha)) + Math.cos(alpha));
		val[1][2] = (float) (z*y*(1-Math.cos(alpha)) - x*Math.sin(alpha));
		val[1][3] = 0;
		
		val[2][0] = (float) (x*z*(1-Math.cos(alpha)) - y*Math.sin(alpha));
		val[2][1] = (float) (y*z*(1-Math.cos(alpha)) + x*Math.sin(alpha));
		val[2][2] = (float) (z*z*(1-Math.cos(alpha)) + Math.cos(alpha));
		val[2][3] = 0;
		
		val[3][0] = 0;
		val[3][1] = 0;
		val[3][2] = 0;
		val[3][3] = 1;
	}
	
	public void makeScrewRotation(float alpha, HTVectorf k){
		makeScrewRotation(alpha, k.val[0], k.val[1], k.val[2]);
	}
	
	public void makeIdentity(){
		for(int i = 0;i<4;i++)
			for(int j = 0;j<4;j++){
				val[i][j] = 0;
				if(i==j) val[i][j] = 1;
			}
	}

	public HTVectorf getP(){
		float[] vals = {val[0][3], val[1][3], val[2][3], val[3][3]};
		return new HTVectorf(vals);
	}
	
	public String toString(){
		StringBuilder sb = new StringBuilder();
		for(int i = 0;i<4;i++){
			for(int j = 0;j<4;j++){
				sb.append(val[i][j] + " ");
			}
			sb.append("\n");
		}
		return sb.toString();
	}
}
