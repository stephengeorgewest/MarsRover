package rover.guistuff;


import java.awt.BorderLayout;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.event.MouseWheelEvent;
import java.awt.event.MouseWheelListener;
import java.io.File;
import java.nio.FloatBuffer;

import javax.media.opengl.GL;
import javax.media.opengl.GLAutoDrawable;
import javax.media.opengl.GLCanvas;
import javax.media.opengl.GLCapabilities;
import javax.media.opengl.GLEventListener;
import javax.media.opengl.glu.GLU;
import javax.media.opengl.glu.GLUquadric;
import com.sun.opengl.util.*;
import com.sun.opengl.util.texture.Texture;
import com.sun.opengl.util.texture.TextureCoords;
import com.sun.opengl.util.texture.TextureIO;

import javax.swing.JPanel;

import rover.graphics.HTMatrixf;
import rover.graphics.HTVectorf;


public class ArmSimulationPanel3D  extends JPanel implements GLEventListener{

	private static final long serialVersionUID = 1L;

	private GLCapabilities caps;
	private GLU glu;  //  @jve:decl-index=0:
	private GLUT glut;
	private Texture ground;
	private GLUquadric quadratic;
	
	private FloatBuffer mat1;
	
	private float[] joints;
	//private float view_bounds = 30;
	//private float[] view_xyz;
	//private float[] view_cntr;

	private HTMatrixf viewMatrix;
	
	//private GLJPanel glJPanel = null;
	private GLCanvas glJPanel = null;

	private float PI = 3.1415926535f;
	/**
	 * This is the default constructor
	 */
	public ArmSimulationPanel3D() {
		super();
		joints = new float[5];
		for(int i = 0;i<5;i++)
			joints[i] = 0;
		
		viewMatrix = new HTMatrixf();
		viewMatrix.Rotate(PI/4, -1, 0, 0);
		viewMatrix.Translate(0,-20,-50);
		
		initialize();
		
		MouseEventListener listener = new MouseEventListener(this);
	    glJPanel.addMouseListener(listener);
	    glJPanel.addMouseMotionListener(listener);
	    glJPanel.addMouseWheelListener(listener);

	}

	public static Texture load(String fileName){
		Texture text = null;
		try{
			text = TextureIO.newTexture(new File(fileName), false);
			text.setTexParameteri(GL.GL_TEXTURE_MAG_FILTER, GL.GL_NEAREST);
			text.setTexParameteri(GL.GL_TEXTURE_MIN_FILTER, GL.GL_NEAREST);
		}catch(Exception e){
			System.out.println(e.getMessage());
			System.out.println("Error loading texture " + fileName);
		}
		return text;
	}
	
	public class MouseEventListener implements MouseListener, MouseMotionListener, MouseWheelListener{
		ArmSimulationPanel3D panel;	
		float sx;
		float sy;
		boolean dragging;
		int button = 0;
		
		MouseEventListener(ArmSimulationPanel3D panel){
			this.panel = panel;
			dragging = false;
		}
		@Override
		public void mouseClicked(MouseEvent e) {}
		@Override
		public void mouseEntered(MouseEvent e) {}
		@Override
		public void mouseExited(MouseEvent e) {}
		@Override
		public void mousePressed(MouseEvent e) {
			dragging = true;
			button = e.getButton();
			sx = e.getX();
			sy = e.getY();
		}
		@Override
		public void mouseReleased(MouseEvent e) {
			dragging = false;
		}

		@Override
		public void mouseDragged(MouseEvent arg0) {
			float dx = arg0.getX()-sx;
			float dy = arg0.getY()-sy;
			sx = arg0.getX();
			sy = arg0.getY();
			
			float[] svec = {dx, -dy, 0, 0};
			HTVectorf s = new HTVectorf(svec);
			if(button == MouseEvent.BUTTON1){
				s.mult(-viewMatrix.getP().val[2]/1000);
				viewMatrix.Translate(s);
				panel.repaint();
				
			} else if (button == MouseEvent.BUTTON3 || button == MouseEvent.BUTTON2){

				// TODO Auto-generated method stub
//				System.out.println("Mouse Dragged");

				HTVectorf z = viewMatrix.getP();
				z.mult(-1);
				
				HTVectorf k = z.crossp(s);
				
				
				System.out.println(viewMatrix);
//				System.out.println(s);
//				System.out.println(z);
//				System.out.println(k);
				
				
				k.normalize();
				
				HTMatrixf inv = viewMatrix.getInverse();
				k = inv.vecmultiply(k);
				
//				System.out.println(" k normalized " + k);
				float alpha = (s.getMagnitude()/z.getMagnitude());
//				System.out.println("Alpha " + alpha);
				panel.viewMatrix.Rotate(alpha, k.val[0], k.val[1], k.val[2]);
				System.out.println(viewMatrix);
				
				panel.repaint();
			}
		}

		@Override
		public void mouseMoved(MouseEvent arg0) {
			// TODO Auto-generated method stub
		}

		@Override
		public void mouseWheelMoved(MouseWheelEvent arg0) {
			// TODO Auto-generated method stub
			float f = arg0.getWheelRotation();
			panel.viewMatrix.Translate(0,0,f*10);
			//System.out.println(panel.viewMatrix.toString());
			panel.repaint();
		}
		
		
	
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
	}

	public void setJoints(float[] joints){
		this.joints = joints;
//		for(int i = 0;i<5;i++)
//			System.out.println("Joint " + i + " = " + joints[i]);
		repaint();
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
		    glut = new GLUT();
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
		gl.glPushMatrix();
		//glu.gluLookAt(view_xyz[0], view_xyz[1], view_xyz[2],  view_cntr[0], view_cntr[1], view_cntr[2],   0.0, 1.0, 0.0);
		
		gl.glMultMatrixf(viewMatrix.get1DArray(), 0);
		gl.glColor3ub((byte)255, (byte)255, (byte)255);
	    gl.glClear(GL.GL_COLOR_BUFFER_BIT | GL.GL_DEPTH_BUFFER_BIT);
	    
	    gl.glPushMatrix();
			drawGround(gl);
		gl.glPopMatrix();
	    
	    gl.glPushMatrix();
			drawRover(gl);
		gl.glPopMatrix();
		
		gl.glPushMatrix();
			drawArm(gl);
		gl.glPopMatrix();
		
		
		
		
		
		gl.glPopMatrix();
		
		gl.glFlush();
	}
	
	private void drawGround(GL gl){
		
		TextureCoords tc = ground.getImageTexCoords();
		 
		gl.glPushMatrix();
		gl.glTranslatef( 0 , 0 , 0 ); // move position of the texture to this coord
		
		
		int halfsize = 200;
		ground.enable();
		ground.bind();
		gl.glBegin(GL.GL_QUADS);
		for(int i = -4;i<5;i++){
			for(int j = -4;j<5;j++){
				gl.glTexCoord2f(tc.left(), tc.bottom()); gl.glVertex3f( i*halfsize, j*halfsize, 0);
				gl.glTexCoord2f(tc.right(), tc.bottom()); gl.glVertex3f( (i+1)*halfsize, j*halfsize, 0);
				gl.glTexCoord2f(tc.right(), tc.top()); gl.glVertex3f( (i+1)*halfsize, (j+1)*halfsize, 0);
				gl.glTexCoord2f(tc.left(), tc.top()); gl.glVertex3f( i*halfsize, (j+1)*halfsize, 0);
			}
		}
		gl.glEnd();
		ground.disable();
		gl.glPopMatrix();

		
	}
	
	private void drawRover(GL gl){
		//later we can add the ability to load the rover drawing from vrml
		//for now we'll just hardcode it

		gl.glColor4ub((byte)255, (byte)255, (byte)255, (byte)255);
		
		gl.glPolygonMode(GL.GL_FRONT_AND_BACK, GL.GL_FILL);
		
		gl.glBegin(GL.GL_QUADS);
			//bottom
			gl.glVertex3f(-12.0f, 18.0f, 8f);
			gl.glVertex3f(12.0f, 18.0f, 8f);
			gl.glVertex3f(12.0f, -18.0f, 8f);
			gl.glVertex3f(-12.0f, -18.0f, 8f);
			
			//top
			gl.glVertex3f(-12.0f, 18.0f, 18f);
			gl.glVertex3f(12.0f, 18.0f, 18f);
			gl.glVertex3f(12.0f, -18.0f, 18f);
			gl.glVertex3f(-12.0f, -18.0f, 18f);
			
			//front
			gl.glVertex3f(-12.0f, 18.0f, 8f);
			gl.glVertex3f(12.0f, 18.0f, 8f);
			gl.glVertex3f(12.0f, 18.0f, 18f);
			gl.glVertex3f(-12.0f, 18.0f, 18f);
			
			//back
			gl.glVertex3f(-12.0f, -18.0f, 8f);
			gl.glVertex3f(12.0f, -18.0f, 8f);
			gl.glVertex3f(12.0f, -18.0f, 18f);
			gl.glVertex3f(-12.0f, -18.0f, 18f);
			
			//right
			gl.glVertex3f(12.0f, -18.0f, 8f);
			gl.glVertex3f(12.0f, 18.0f, 8f);
			gl.glVertex3f(12.0f, 18.0f, 18f);
			gl.glVertex3f(12.0f, -18.0f, 18f);
			
			//left
			gl.glVertex3f(-12.0f, -18.0f, 8f);
			gl.glVertex3f(-12.0f, 18.0f, 8f);
			gl.glVertex3f(-12.0f, 18.0f, 18f);
			gl.glVertex3f(-12.0f, -18.0f, 18f);
			
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
		gl.glTranslatef(0, 15, 19);
//		gl.glRotatef(Head_angle, 0, 0, 1);
		gl.glBegin(GL.GL_QUADS);
			//bottom
			gl.glVertex3f(-3.0f, 1.50f, 0f);
			gl.glVertex3f(3.0f, 1.50f, 0f);
			gl.glVertex3f(3.0f, 0f, 0f);
			gl.glVertex3f(-3.0f, 0f, 0f);
			
			//top
			gl.glVertex3f(-3.0f, 1.5f, 1.5f);
			gl.glVertex3f(3.0f, 1.5f, 1.5f);
			gl.glVertex3f(3.0f, 0f, 1.5f);
			gl.glVertex3f(-3.0f, 0f, 1.5f);
			
			//front
			gl.glVertex3f(-3.0f, 1.5f, 0f);
			gl.glVertex3f(3.0f, 1.5f, 0f);
			gl.glVertex3f(3.0f, 1.5f, 1.5f);
			gl.glVertex3f(-3.0f, 1.5f, 1.5f);
			
			//back
			gl.glVertex3f(-3.0f, 0f, 0f);
			gl.glVertex3f(3.0f, 0f, 0f);
			gl.glVertex3f(3.0f, 0f, 1.5f);
			gl.glVertex3f(-3.0f, 0f, 1.5f);
			
			//right
			gl.glVertex3f(3.0f, 0f, 0f);
			gl.glVertex3f(3.0f, 1.5f, 0f);
			gl.glVertex3f(3.0f, 1.5f, 1.5f);
			gl.glVertex3f(3.0f, 0f, 1.5f);
			
			//left
			gl.glVertex3f(-3.0f, 0f, 0f);
			gl.glVertex3f(-3.0f, 1.5f, 0f);
			gl.glVertex3f(-3.0f, 1.5f, 1.5f);
			gl.glVertex3f(-3.0f, 0f, 1.5f);
			
		gl.glEnd();
		gl.glColor3ub((byte)0, (byte)0, (byte)0);
		gl.glPushMatrix();
			gl.glTranslatef(1.5f, 1.5f, .75f);
			gl.glRotatef(-90, 1, 0, 0);
			glu.gluCylinder(quadratic, .25f,.25f,.5f,5,1);
		gl.glPopMatrix();
			gl.glTranslatef(-1.5f, 1.5f, .75f);
			gl.glRotatef(-90, 1, 0, 0);
			glu.gluCylinder(quadratic, .25f,.25f,.5f,5,1);
		gl.glPopMatrix();
		
		
	}
	
	private void drawArm(GL gl){

		gl.glTranslatef(10, 16, 18);
		
		gl.glRotatef(180*joints[0]/PI, 0, 0, 1);
		
		
		
		gl.glPolygonMode(GL.GL_FRONT_AND_BACK, GL.GL_FILL);

		//base
		gl.glColor3ub((byte)75, (byte)75, (byte)75);
		glut.glutSolidCylinder(2, 5, 4, 1);

		//upper arm
		gl.glColor3ub((byte)150, (byte)150, (byte)150);
		gl.glTranslated(0, 0, 5);
		gl.glRotatef(-90+180*joints[1]/PI, 1, 0, 0);
		glut.glutSolidCylinder(1, 15, 4, 1);
		
		//lower arm segment
		gl.glColor3ub((byte)100, (byte)100, (byte)100);
		gl.glTranslated(0,0,15);
		gl.glRotatef(180*joints[2]/PI, 1, 0, 0);	
		glut.glutSolidCylinder(.5, 5, 4, 1);
		
		//wrist rotation
		gl.glColor3ub((byte)150, (byte)150, (byte)150);
		gl.glTranslated(0,0,5);
		gl.glRotatef(180*joints[3]/PI, 0, 0, 1);	
		glut.glutSolidCylinder(.5, 10, 4, 1);
		
		//wrist extension
		gl.glColor3ub((byte)100, (byte)100, (byte)100);
		gl.glTranslated(0,0,10);
		gl.glRotatef(180*joints[4]/PI, 1, 0, 0);	
		glut.glutSolidCylinder(.375, 5, 4, 1);
		
	}
	
	
	
	@Override
	public void displayChanged(GLAutoDrawable arg0, boolean arg1, boolean arg2) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void init(GLAutoDrawable drawable) {
		System.out.println("Init Called");
		GL gl = drawable.getGL();
		glu.gluPerspective(50.0, 1.0, .5, 10000.0);
		gl.glEnable(GL.GL_DEPTH_TEST);
		gl.glEnable(GL.GL_TEXTURE_2D);
		gl.glClearColor(168/255f, 211/255f, 255/255f, 1);
		gl.glClearDepth (1.0f);

		try{
			String path = getClass().getResource("/sand2.gif").toURI().getPath();
			//System.out.println(path);
			ground = load(path);
		}catch(Exception e){
			e.printStackTrace();
		}
		
		gl.glEnable(GL.GL_LIGHTING);
		gl.glShadeModel(GL.GL_SMOOTH);
		
		float[] ambient_light = { 0f, 0f, 0f };
		FloatBuffer ambient = FloatBuffer.wrap(ambient_light);
		gl.glLightModelfv(GL.GL_LIGHT_MODEL_AMBIENT, ambient);
		
		float[] ambient_light_model = { 1f, 1f, 1f };
		FloatBuffer ambient_model = FloatBuffer.wrap(ambient_light_model);
		gl.glLightfv(GL.GL_LIGHT0, GL.GL_AMBIENT, ambient_model);
		
		float[] specular_light = { .5f, .5f, .5f };
		FloatBuffer specular = FloatBuffer.wrap(specular_light);
		gl.glLightfv(GL.GL_LIGHT0, GL.GL_SPECULAR, specular);
		
		float[] diffuse_light= { 0f, 0f, 0f };
		FloatBuffer diffuse = FloatBuffer.wrap(diffuse_light);
		gl.glLightfv(GL.GL_LIGHT0, GL.GL_DIFFUSE, diffuse);
		
		float[] position_light = { 100f, 100f, 1000f };
		FloatBuffer position = FloatBuffer.wrap(position_light);
		gl.glLightfv(GL.GL_LIGHT0, GL.GL_POSITION, position);

		gl.glEnable(GL.GL_LIGHT0);
		float[] material = { 1.0f, 1f, 1f, 1f };
		mat1 = FloatBuffer.wrap(material);
		gl.glMaterialfv(GL.GL_FRONT, GL.GL_AMBIENT_AND_DIFFUSE, mat1);
		gl.glEnable(GL.GL_COLOR_MATERIAL);
		gl.glColorMaterial(GL.GL_FRONT, GL.GL_AMBIENT_AND_DIFFUSE);

		
	}

	@Override
	public void reshape(GLAutoDrawable drawable, int x, int y, int w, int h){

	}

}
