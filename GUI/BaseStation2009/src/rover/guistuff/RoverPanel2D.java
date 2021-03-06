package rover.guistuff;

import java.awt.GridBagLayout;
import javax.swing.JPanel;
import javax.media.opengl.*;
import javax.media.opengl.glu.GLU;
import javax.media.opengl.glu.GLUquadric;

import java.awt.Graphics;
import java.awt.GridBagConstraints;
import java.awt.BorderLayout;
import javax.media.opengl.GLCanvas;

public class RoverPanel2D extends JPanel implements GLEventListener{

	private static final long serialVersionUID = 1L;

	private GLCapabilities caps;
	private GLU glu;  //  @jve:decl-index=0:
	
	private GLUquadric quadratic;
	
	private float Head_angle;
	private float[] Throttles;
	private float view_bounds = 30;

	//private GLJPanel glJPanel = null;
	private GLCanvas glJPanel = null;

	private float PI = 3.1415926535f;
	/**
	 * This is the default constructor
	 */
	public RoverPanel2D() {
		super();
		Throttles = new float[4];
		initialize();

	}

	/**
	 * This method initializes this
	 * 
	 * @return void
	 */
	private void initialize() {
		this.setSize(300, 200);
		this.setLayout(new BorderLayout());
		this.add(getglJPanel());
		Throttles[0] = .50f;
		Throttles[1] = -.20f;
		Throttles[2] = .90f;
		Throttles[3] = -1.00f;
		Head_angle = 45;
	}

	public void setThrottles(float[] throttles){
		if(throttles.length > 3){
			Throttles = throttles;
		}
	}
	
	public void setHead_Angle(float angle){
		Head_angle = angle;
	}
	
	@Override
	public void repaint(){
		if(glJPanel != null) glJPanel.repaint();
	}
	
	/**
	 * This method initializes glJPanel	
	 * 	
	 * @return javax.media.opengl.glJPanel	
	 */
	private GLCanvas getglJPanel() {
	//private GLJPanel getglJPanel() {
		if (glJPanel == null) {
			caps = new GLCapabilities();
		    caps.setDoubleBuffered(false);
		    System.out.println(caps.toString());

		    glu = new GLU();
		    quadratic = glu.gluNewQuadric();
		    //glJPanel = new GLJPanel(caps);
		    glJPanel = new GLCanvas();
		    glJPanel.addGLEventListener(this);
		}
		return glJPanel;
	}

	@Override
	public void display(GLAutoDrawable drawable) {
		//System.out.println("Display method called");
		GL gl = drawable.getGL();

	    gl.glClear(GL.GL_COLOR_BUFFER_BIT | GL.GL_DEPTH_BUFFER_BIT);
	    	    
		gl.glPushMatrix();
			drawPowerBars(gl);
		gl.glPopMatrix();
	    
	    gl.glPushMatrix();
			drawRover(gl);
		gl.glPopMatrix();
		
		gl.glFlush();
	}

	private void drawPowerBars(GL gl){

		gl.glColor3ub((byte)0, (byte)0, (byte)0);
		//draw power_bars
		gl.glBegin(GL.GL_LINES);
			gl.glVertex3f(-view_bounds, 11, 0f);
			gl.glVertex3f(view_bounds, 11, 0f);
			
			gl.glVertex3f(-view_bounds, -11, 0f);
			gl.glVertex3f(view_bounds, -11, 0f);
		gl.glEnd();
		
		gl.glPolygonMode(GL.GL_FRONT_AND_BACK, GL.GL_FILL);
		
		float maxsize = 10;
		float width = 4;
		
		int i = -1;
		int j = -1;
		
		for(int n = 0;n<4;n++){
			float height = maxsize*Throttles[n];
			if(height > 0)
				gl.glColor3ub((byte)116, (byte)255, (byte)13);
			else
				gl.glColor3ub((byte)255, (byte)13, (byte)13);
			
			gl.glPushMatrix();
			gl.glTranslatef(i*view_bounds, j*11f, 0);
			gl.glBegin(GL.GL_QUADS);
				gl.glVertex3f(0f, height, 0);
				gl.glVertex3f(-i*width, height, 0);
				
				gl.glVertex3f(-i*width, 0, 0);
				gl.glVertex3f(0, 0, 0);
			gl.glEnd();
			gl.glPopMatrix();
			
			if(i == 1){
				j=1;
				i=-1;
			}else
				i += 2;
		}
		
		
					
		
	}
	
	private void drawRover(GL gl){
		//later we can add the ability to load the rover drawing from vrml
		//for now we'll just hardcode it

		gl.glColor3ub((byte)255, (byte)255, (byte)255);
		
		gl.glPolygonMode(GL.GL_FRONT_AND_BACK, GL.GL_FILL);
		
		gl.glBegin(GL.GL_QUADS);
			gl.glVertex3f(-12.0f, 18.0f, 8f);
			gl.glVertex3f(12.0f, 18.0f, 8f);
			
			gl.glVertex3f(12.0f, -18.0f, 8f);
			
			gl.glVertex3f(-12.0f, -18.0f, 8f);
		gl.glEnd();
		
		gl.glColor3ub((byte)100, (byte)100, (byte)100);
		//draw tires
		for(int i = -1;i<3;i+=2)
			for(int j = -1;j<3;j+=2){
			
				gl.glPushMatrix();
					gl.glTranslatef(i*19, j*11, 5.5f);
					gl.glRotatef(i*90, 0, 1, 0);
					
					glu.gluCylinder(quadratic, 5.5f,5.5f,4.0f,10,3);
				gl.glPopMatrix();
			}
		
		//draw tire lines
		gl.glPolygonMode(GL.GL_FRONT_AND_BACK, GL.GL_LINE);
		gl.glColor3ub((byte)0, (byte)0, (byte)0);
		
		for(int i = -1;i<3;i+=2)
			for(int j = -1;j<3;j+=2){
			
				gl.glPushMatrix();
					gl.glTranslatef(i*19, j*11, 5.5f);
					gl.glRotatef(i*90, 0, 1, 0);
					
					gl.glBegin(GL.GL_LINES);
						gl.glVertex3f(0.0f, -2.0f, 0f);
						gl.glVertex3f(0.0f, -5.0f, -7f);
						
						gl.glVertex3f(0.0f, 2.0f, 0f);
						gl.glVertex3f(0.0f, 5.0f, -7f);
					gl.glEnd();		
					
					glu.gluCylinder(quadratic, 5.5f,5.5f,4.0f,10,3);
				gl.glPopMatrix();
			}
		
		gl.glColor3ub((byte)0, (byte)0, (byte)0);
		
		//draw rover body
		gl.glBegin(GL.GL_LINE_STRIP);
			gl.glVertex3f(-12.0f, 18.0f, 8f);
			gl.glVertex3f(12.0f, 18.0f, 8f);
			
			gl.glVertex3f(12.0f, -18.0f, 8f);
			
			gl.glVertex3f(-12.0f, -18.0f, 8f);
			
			gl.glVertex3f(-12.0f, 18.0f, 8f);
			
			gl.glVertex3f(-12.0f, 18.0f, 18f);
			gl.glVertex3f(12.0f, 18.0f, 18f);
			
			gl.glVertex3f(12.0f, -18.0f, 18f);
			
			gl.glVertex3f(-12.0f, -18.0f, 18f);
			
			gl.glVertex3f(-12.0f, 18.0f, 18f);
			
		gl.glEnd();
		

		gl.glPolygonMode(GL.GL_FRONT_AND_BACK, GL.GL_FILL);
		
		gl.glPushMatrix();

		//draw head
		gl.glColor3ub((byte)119, (byte)187, (byte)255);
		gl.glTranslatef(0, 15, 18);
		gl.glRotatef(Head_angle, 0, 0, 1);
		gl.glBegin(GL.GL_QUADS);
			gl.glVertex3f(-3f, -1.5f, 8f);
			gl.glVertex3f(3f, -1.5f, 8f);
			
			gl.glVertex3f(3.0f, 1.5f, 8f);
			gl.glVertex3f(-3.0f, 1.5f, 8f);
			
		gl.glEnd();
		
		gl.glPopMatrix();
		
		
	}
	
	
	
	@Override
	public void displayChanged(GLAutoDrawable arg0, boolean arg1, boolean arg2) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void init(GLAutoDrawable drawable) {
		System.out.println("Init Called");
		GL gl = drawable.getGL();
		
		
//		gl.glMatrixMode(GL.GL_PROJECTION);
//        gl.glLoadIdentity();
//        glu.gluOrtho2D(-1.0f, 1.0f, -1.0f, 1.0f); // drawing square
//        gl.glMatrixMode(GL.GL_MODELVIEW);
//        gl.glLoadIdentity();
//	    
       
		float rb = this.getBackground().getRed()/255.0f;
		float gb = this.getBackground().getGreen()/255.0f;
		float bb = this.getBackground().getBlue()/255.0f;
		float ab = this.getBackground().getAlpha()/255.0f;
		
		gl.glClearColor(rb, gb, bb, ab);
		
		//gl.glClearColor(0f, 0f, 0f, 0f);
		gl.glClearDepth (1.0f);

		 gl.glMatrixMode(GL.GL_PROJECTION);
		 glu.gluPerspective(50.0, 1.0, 0.0, 100.0);
		 //gl.glOrtho(-view_bounds, view_bounds, -view_bounds, view_bounds, 0, 100);
		 gl.glMatrixMode(GL.GL_MODELVIEW);
		 //glu.gluLookAt(0.0, -80, 0,  0.0, 0.0, 0.0,   0.0, 0.0, 1.0);
		 glu.gluLookAt(0.0, 0, 80.0,  0.0, 0.0, 0.0,   0.0, 1.0, 0.0);

	}

	@Override
	public void reshape(GLAutoDrawable drawable, int x, int y, int w, int h){

	}

}
