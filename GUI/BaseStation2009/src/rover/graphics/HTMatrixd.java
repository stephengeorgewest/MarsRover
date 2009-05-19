package rover.graphics;

public class HTMatrixd {
	public double[][] val;
	
	public HTMatrixd(){
		val = new double[4][4];
		makeIdentity();
	}

	//note, opengl matrices are in column major order
	public double[] get1DArray(){
		double[] retval = new double[16];
		for(int i = 0;i<4;i++)
			for(int j = 0;j<4;j++){
				retval[j*4+i] = val[i][j];
			}
		return retval;
	}
	public HTMatrixd copy(){
		HTMatrixd retval = new HTMatrixd();
		for(int i = 0;i<4;i++)
			for(int j = 0;j<4;j++){
				retval.val[i][j] = val[i][j];
			}
		return retval;
	}
	
	public void orthonormalize(){
		HTVectord xc = new HTVectord(val[0][0], val[1][0], val[2][0], 0);
		HTVectord yc = new HTVectord(val[0][1], val[1][1], val[2][1], 0);
		HTVectord zc = xc.crossp(yc);
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

	
	
	public void premultiply(HTMatrixd m){
		double[][] res = new double[4][4];
		for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                for (int k = 0; k < 4; k++)
                    res[i][j] += m.val[i][k] * val[k][j];
		val = res;
	}
	public void postmultiply(HTMatrixd m){
		double[][] res = new double[4][4];
		for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                for (int k = 0; k < 4; k++)
                    res[i][j] += val[i][k] * m.val[k][j];
		val = res;
	}
	public double[] vecmultiply(double[] vec){
		double[] res = new double[4];
		for (int i = 0; i < 4; i++){
			res[i] = 0;
            for (int j = 0; j < 4; j++)
                res[i] += val[i][j] * vec[j]; 
		}
		return res;
	}
	
	public HTVectord vecmultiply(HTVectord vec){
		HTVectord res = new HTVectord();
		for (int i = 0; i < 4; i++){
			res.val[i] = 0;
            for (int j = 0; j < 4; j++)
                res.val[i] += val[i][j] * vec.val[j]; 
		}
		return res;
	}
	
	public void Rotate(double alpha, double x, double y, double z){
		//System.out.println("applying rotation");
		//System.out.println(this);
		HTMatrixd r = new HTMatrixd();
		r.makeScrewRotation(alpha, x, y, z);
		//System.out.println(r);
		this.postmultiply(r);
		//System.out.println(this);
	}
	public void Translate(double x, double y, double z){
		val[0][3] += x;
		val[1][3] += y;
		val[2][3] += z;
	}
	
	public void Translate(HTVectord vec){
		val[0][3] += vec.val[0];
		val[1][3] += vec.val[1];
		val[2][3] += vec.val[2];
	}
	
	public void unTranslate(HTVectord vec){
		val[0][3] -= vec.val[0];
		val[1][3] -= vec.val[1];
		val[2][3] -= vec.val[2];
	}
	
	public void setP(HTVectord np){
		val[0][3] = np.val[0];
		val[1][3] = np.val[1];
		val[2][3] = np.val[2];
	}
	
	public void Invert(){
		double[][] nval = new double[4][4];
		
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
	
	public HTMatrixd getInverse(){
		HTMatrixd r = this.copy();
		r.Invert();
		return r;
	}
	
	public void makeScrewRotation(double alpha, double x, double y, double z){
		val[0][0] = (double) (x*x*(1-Math.cos(alpha)) + Math.cos(alpha));
		val[0][1] = (double) (y*x*(1-Math.cos(alpha)) - z*Math.sin(alpha));
		val[0][2] = (double) (z*x*(1-Math.cos(alpha)) + y*Math.sin(alpha));
		val[0][3] = 0;
		
		val[1][0] = (double) (x*y*(1-Math.cos(alpha)) + z*Math.sin(alpha));
		val[1][1] = (double) (y*y*(1-Math.cos(alpha)) + Math.cos(alpha));
		val[1][2] = (double) (z*y*(1-Math.cos(alpha)) - x*Math.sin(alpha));
		val[1][3] = 0;
		
		val[2][0] = (double) (x*z*(1-Math.cos(alpha)) - y*Math.sin(alpha));
		val[2][1] = (double) (y*z*(1-Math.cos(alpha)) + x*Math.sin(alpha));
		val[2][2] = (double) (z*z*(1-Math.cos(alpha)) + Math.cos(alpha));
		val[2][3] = 0;
		
		val[3][0] = 0;
		val[3][1] = 0;
		val[3][2] = 0;
		val[3][3] = 1;
	}
	
	public void makeScrewRotation(double alpha, HTVectord k){
		makeScrewRotation(alpha, k.val[0], k.val[1], k.val[2]);
	}
	
	public void makeIdentity(){
		for(int i = 0;i<4;i++)
			for(int j = 0;j<4;j++){
				val[i][j] = 0;
				if(i==j) val[i][j] = 1;
			}
	}

	public HTVectord getP(){
		double[] vals = {val[0][3], val[1][3], val[2][3], val[3][3]};
		return new HTVectord(vals);
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
