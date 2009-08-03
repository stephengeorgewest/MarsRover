package rover.video;

import javax.media.Buffer;
import javax.media.opengl.GL;
import javax.media.opengl.GLAutoDrawable;
import javax.media.opengl.GLCanvas;
import javax.media.opengl.GLEventListener;

import com.sun.opengl.util.texture.Texture;

public class GLRenderer extends GLCanvas implements GLEventListener{

	Texture t;
	
	public GLRenderer(){
		
		t = new Texture(boundsOp);
		
	}
	
	
	public int process(Buffer buffer) {
		
		byte[] data = (byte[])buffer.getData();
		
		
		
		
		return 0;
	}
	

	@Override
	public void init(GLAutoDrawable drawable) {
		
		
		GL gl = drawable.getGL();
		gl.glLoadIdentity();
		gl.glClearColor(0f, 0f, 0f, 1f);
		
		gl.setSwapInterval(1);
		//gl.glClearColor(0f, 0f, 0f, 0f);
		gl.glClearDepth (1.0f);

		 gl.glMatrixMode(GL.GL_PROJECTION);
		 glu.gluPerspective(45, 1.0, .1, 1.1);
		 
		 //gl.glOrtho(-view_bounds, view_bounds, -view_bounds, view_bounds, 0, 100);
		 //gl.glMatrixMode(GL.GL_MODELVIEW);
		 //glu.gluLookAt(0.0, -80, 0,  0.0, 0.0, 0.0,   0.0, 0.0, 1.0);
		 //glu.gluLookAt(0.0, 0, 80.0,  0.0, 0.0, 0.0,   0.0, 1.0, 0.0);
	}

	@Override
	public void reshape(GLAutoDrawable drawable, int x, int y, int width,
			int height) {
		// TODO Auto-generated method stub
		
	}


	@Override
	public void display(GLAutoDrawable drawable) {
		// TODO Auto-generated method stub
		
	}


	@Override
	public void displayChanged(GLAutoDrawable drawable, boolean modeChanged,
			boolean deviceChanged) {
		// TODO Auto-generated method stub
		
	}
	
}
