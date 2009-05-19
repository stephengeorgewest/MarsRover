package rover.guistuff;


import java.awt.BorderLayout;
import java.awt.Font;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.event.MouseWheelEvent;
import java.awt.event.MouseWheelListener;
import java.io.File;
import java.nio.FloatBuffer;
import java.util.ArrayList;

import javax.media.opengl.GL;
import javax.media.opengl.GLAutoDrawable;
import javax.media.opengl.GLCanvas;
import javax.media.opengl.GLCapabilities;
import javax.media.opengl.GLEventListener;
import javax.media.opengl.glu.GLU;
import javax.media.opengl.glu.GLUquadric;

import com.sun.opengl.impl.windows.WGL;
import com.sun.opengl.util.*;
import com.sun.opengl.util.j2d.TextRenderer;
import com.sun.opengl.util.texture.Texture;
import com.sun.opengl.util.texture.TextureCoords;
import com.sun.opengl.util.texture.TextureIO;

import javax.swing.JPanel;

import rover.graphics.GeoData;
import rover.graphics.HTMatrixd;
import rover.graphics.HTMatrixf;
import rover.graphics.HTVectord;
import rover.graphics.HTVectorf;
import rover.graphics.TexTile;
import rover.graphics.TexTileDB;
import rover.graphics.TileLoadListener;
import rover.utils.Waypoint;

public class RoverSimulationPanel3D  extends JPanel implements GLEventListener, TileLoadListener{

	private static final long serialVersionUID = 1L;
	private static final double m_nm = 1852;
	private static final double in_nm = 72913.3857524;
	private static final double in_m = 39.3700787;
	
	
	private GLCapabilities caps;
	private GLU glu;  //  @jve:decl-index=0:
	private GLUT glut;
	private Texture ground;
	private Texture rover_icon;
	private Texture waypoint_icon;
	private GLUquadric quadratic;
	
	private FloatBuffer mat1;
	
	private float[] joints;
	private double roverlat;
	private double roverlon;
	private float roverphi;
	private HTMatrixd viewMatrix;
	
	
	float fovy = 90;
	float aspect = 1;
	float fovx;
	
	double phi = 0;
	double theta = 0;
	HTVectord camera_pos;
	
	private GeoData geo;
	
	boolean draw_lines = true;
	boolean draw_textures = true;
	boolean draw_compass = true;
	boolean draw_ascale = true;
	boolean fetch_images = true;
	
	private ArrayList<Waypoint> waypoints;
	//Waypoint home;
	
	double clat;
	double clon;
	
	//private GLJPanel glJPanel = null;
	private GLCanvas glJPanel = null;

	private double PI = 3.1415926535f;
	
	private TextRenderer text_rend;
	
	private ArrayList<CameraMoveListener> cameraListeners;
	
	public class MouseEventListener implements MouseListener, MouseMotionListener, MouseWheelListener{
		RoverSimulationPanel3D panel;	
		double sx;
		double sy;
		boolean dragging;
		int button = 0;
		
		MouseEventListener(RoverSimulationPanel3D panel){
			this.panel = panel;
			dragging = false;
		}
		
		@Override
		public void mouseClicked(MouseEvent e) {
			// TODO Auto-generated method stub
		}
		@Override
		public void mouseEntered(MouseEvent e) {
			// TODO Auto-generated method stub
			
		}
		@Override
		public void mouseExited(MouseEvent e) {
			// TODO Auto-generated method stub
			
		}
		@Override
		public void mousePressed(MouseEvent e) {
			// TODO Auto-generated method stub
			dragging = true;
			button = e.getButton();
			sx = e.getX();
			sy = e.getY();
		}
		@Override
		public void mouseReleased(MouseEvent e) {
			// TODO Auto-generated method stub
			dragging = false;
		}

		@Override
		public void mouseDragged(MouseEvent arg0) {
			double dx = arg0.getX()-sx;
			double dy = arg0.getY()-sy;
			sx = arg0.getX();
			sy = arg0.getY();
			
			double[] sdata = {dx, dy, 0, 0};
			sdata[0] /= panel.getWidth();
			sdata[1] /= panel.getHeight();
			HTVectord s = new HTVectord(sdata);
			//System.out.println(s);
			if(button == MouseEvent.BUTTON3){
				phi += (s.val[0]*fovx*Math.PI/180);
				theta += (s.val[1]*fovy*Math.PI/180);
				
				
				recomputeViewMatrix();
			} else if (button == MouseEvent.BUTTON1){
				s.val[1] *= -1;
				s = viewMatrix.getInverse().vecmultiply(s);
				double lat = y2lat(camera_pos.val[1]);
				double lon = x2lon(camera_pos.val[0], GeoData.adj_latitude);
				s.mult(camera_pos.val[2]-geo.get(lat, lon ));
				camera_pos.sub(s);
				//viewMatrix.Translate(s);
				recomputeViewMatrix();
			}
			
		}

		@Override
		public void mouseMoved(MouseEvent arg0) {
			// TODO Auto-generated method stub
		}

		@Override
		public void mouseWheelMoved(MouseWheelEvent arg0) {
			// TODO Auto-generated method stub
			double f = arg0.getWheelRotation();
			double lat = y2lat(camera_pos.val[1]);
			double lon = x2lon(camera_pos.val[0], GeoData.adj_latitude);
			double z = f*(camera_pos.val[2]-geo.get(lat, lon))/5;
			HTVectord tvec = new HTVectord(0, 0, z, 0);
			
			
			//HTMatrixd m = viewMatrix.getInverse();
			//m.val[2][3] = Math.max(geo.get(y2lat(camera_pos.val[1]), x2lon(camera_pos.val[0]))+.1f, m.val[2][3]);
			//viewMatrix = m.getInverse();
			tvec = viewMatrix.getInverse().vecmultiply(tvec);
			camera_pos.sub(tvec);
			camera_pos.val[2] = Math.max(camera_pos.val[2], 0.01);
			
			//panel.viewMatrix.Translate(0,0,f*(camera_pos.val[2]-geo.get(y2lat(camera_pos.val[1]), x2lon(camera_pos.val[0])))/5);
			//HTMatrixd m = viewMatrix.getInverse();
			//m.val[2][3] = Math.max(geo.get(y2lat(camera_pos.val[1]), x2lon(camera_pos.val[0]))+.1f, m.val[2][3]);
			//viewMatrix = m.getInverse();
			
			//System.out.println(panel.viewMatrix.toString());
			//panel.repaint();
			//camera_pos = m.getP();
			recomputeViewMatrix();
		}

	}
	
	public interface CameraMoveListener{
		public void CameraMoved(double lat, double lon, double elevation);
	}
	
	/**
	 * This is the default constructor
	 */
	public RoverSimulationPanel3D() {
		super();
		camera_pos = new HTVectord();
		viewMatrix = new HTMatrixd();
		double lat = 40.2569f;
		double lon = -111.641415f;
		geo = new GeoData();
		//home = new Waypoint("Home", lat, lon, 0);
		waypoints = new ArrayList<Waypoint>();
		joints = new float[5];
		
		
		roverlat = lat;
		roverlon = lon;
		roverphi = 0;
		
		for(int i = 0;i<5;i++)
			joints[i] = 0;
		
		File d = new File("imcache");
		TexTileDB.Initialize(d.getAbsolutePath());
		TexTileDB.addTileLoadListener(this);
		initialize();
		
	
		cameraListeners = new ArrayList<CameraMoveListener>();
		
		MouseEventListener listener = new MouseEventListener(this);
	    glJPanel.addMouseListener(listener);
	    glJPanel.addMouseMotionListener(listener);
	    glJPanel.addMouseWheelListener(listener);

	}

	public void addCameraMoveListener(CameraMoveListener cml){
		cameraListeners.add(cml);
	}
	
	private void cameraMoved(){
		double clat = y2lat(camera_pos.val[1]);
		double clon = x2lon(camera_pos.val[0], GeoData.adj_latitude);
		//TexTileDB.fetchAllTiles(clat, clon);
		for(CameraMoveListener l : cameraListeners){
			l.CameraMoved(clat, clon, camera_pos.val[2]);
		}
	}
	
	public double getCLat(){
		return y2lat(camera_pos.val[1]);
	}
	
	public double getCLon(){
		double clat = y2lat(camera_pos.val[1]);
		double clon = x2lon(camera_pos.val[0], GeoData.adj_latitude);
		return clon;
		//return x2lon(camera_pos.val[0]);
	}
	
	public double getCElevation(){
		return camera_pos.val[2];
	}
	
	public void incCLat(){
		camera_pos.val[1] *= camera_pos.val[2];
		recomputeViewMatrix();
	}
	
	public void decCLat(){
		camera_pos.val[1] /= camera_pos.val[2];
		recomputeViewMatrix();
	}
	
	public void loadTopoData(String filename){
		geo.load(filename);
		TexTileDB.recomputeTiles();
		
		SetRoverView();
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
	
	
	private double lat2y(double latitude){
		return GeoData.latitude2meters(latitude);
	}
	private double lon2x(double longitude, double latitude){
		return GeoData.longitude2meters(longitude, latitude);
	}
	
	private double x2lon(double x, double lat){
		return GeoData.meters2Longitude(x, lat);
	}
	
	private double y2lat(double y){
		return GeoData.meters2Latitude(y);
	}
	
	public void addWaypoint(Waypoint wp) {
		waypoints.add(wp);
		repaint();
	}
	
	public void removeWaypoint(Waypoint wp) {
		waypoints.remove(wp);
		repaint();
	}
	
	public void moveCameraTo(double clat, double clon){
		double x = lon2x(clon, GeoData.adj_latitude);
		double y = lat2y(clat);
		camera_pos.val[0] = x;
		camera_pos.val[1] = y;
		recomputeViewMatrix();
	}
	
//	public void setCenter(double lat, double lon){
//		this.lat = lat;
//		this.lon = lon;
//	}
	
	public void setRoverPos(double lat, double lon){
		roverlat = lat;
		roverlon = lon;
		//roverz = e;
	}
	
	public void setRoverHeading(double heading){
		roverphi = (float)heading;
	}
	
	/**
	 * This method initializes this
	 * 
	 * @return void
	 */
	private void initialize() {
		this.setSize(300, 200);
		
		aspect = this.getWidth();
		aspect /= this.getHeight();
		this.fovx = fovy*aspect;
		
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
	
	private void recomputeViewMatrix(){
//		System.out.println("phi=" + phi + " theta=" + theta);
//		System.out.println(camera_pos);
		viewMatrix.makeIdentity();
		viewMatrix.Rotate(phi, 0, 0, 1);
		viewMatrix.Rotate(theta, 1, 0, 0);
		viewMatrix.Translate(0,0,camera_pos.val[2]);
//		if(camera_pos.val[2] < 10000){ 
//			viewMatrix.Translate(0,0,camera_pos.val[2]);
//		} else {
//			viewMatrix.Translate(0,0,camera_pos.val[2]/1000);
//		}
		viewMatrix.Invert();
		//System.out.println(viewMatrix.toString());
		repaint();
		cameraMoved();
	}
	
	public void SetRoverView(){
		
		double rx = lon2x(roverlon, GeoData.adj_latitude);
		double ry = lat2y(roverlat);
		double rz = geo.get(roverlat, roverlon);
		
		viewMatrix.makeIdentity();
		phi = 0;
		theta = PI/4;
		camera_pos.set(rx, ry-.4f, rz+1, 0);
		recomputeViewMatrix();
//		viewMatrix = new HTMatrix();
//		viewMatrix.Rotate(PI/4, -1, 0, 0);
//		viewMatrix.Translate(0,-20,-50);
//		repaint();
	}
	
	public void SetInitialView(){
		
		double rx = lon2x(roverlon, GeoData.adj_latitude);
		double ry = lat2y(roverlat);
		double rz = geo.get(roverlat, roverlon);
		
		viewMatrix.makeIdentity();
		phi = 0;
		theta = 0;
		camera_pos.set(rx, ry, rz+500000, 0);
		recomputeViewMatrix();
	}

	public void SetMapView(){
		double rx = lon2x(roverlon, GeoData.adj_latitude);
		double ry = lat2y(roverlat);
		double rz = geo.get(roverlat, roverlon);
		
		viewMatrix.makeIdentity();
		phi = 0;
		theta = 0;
		camera_pos.set(rx, ry, rz+500, 0);
		recomputeViewMatrix();
//		viewMatrix = new HTMatrix();
//		viewMatrix.Rotate(PI/4, -1, 0, 0);
//		viewMatrix.Translate(0,-20,-2000);
//		repaint();
	}
	
	
	public boolean isDraw_lines() {
		return draw_lines;
	}

	public void setDraw_lines(boolean draw_lines) {
		this.draw_lines = draw_lines;
		repaint();
	}

	public boolean isDraw_textures() {
		return draw_textures;
	}

	public void setDraw_textures(boolean draw_textures) {
		this.draw_textures = draw_textures;
		repaint();
	}
	
	public void setDrawCompass(boolean draw_compass){
		this.draw_compass = draw_compass;
		repaint();
	}
	
	public void setDrawAScale(boolean draw){
		this.draw_ascale = draw;
		repaint();
	}

//	public double getLat() {
//		return lat;
//	}
//
//	public void setLat(double lat) {
//		this.lat = lat;
//	}
//
//	public double getLon() {
//		return lon;
//	}
//
//	public void setLon(double lon) {
//		this.lon = lon;
//	}
	

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
		
		
		
		
		if(camera_pos.val[2] >= 10000){ 
			gl.glScaled(1/1000.0, 1/1000.0, 1/1000.0);
		}
		
		gl.glMultMatrixd(viewMatrix.get1DArray(), 0);
		
	    gl.glClear(GL.GL_COLOR_BUFFER_BIT | GL.GL_DEPTH_BUFFER_BIT);
	    
	    drawTerrain(gl);
	    gl.glPolygonMode(GL.GL_FRONT_AND_BACK, GL.GL_FILL);
	  
	    double rx = lon2x(roverlon, GeoData.adj_latitude);
		double ry = lat2y(roverlat);
		double rz = geo.get(roverlat, roverlon);
		
		HTVectord rvec = new HTVectord(rx, ry, rz, 0);
		
		rvec.sub(camera_pos);
		//System.out.println("rvec = " + rvec.toString());
		double dist = rvec.getMagnitude();
		//System.out.println("dist to rover = "+ dist);
		if(dist < 100){ //if we're closer than 100 meters draw the rover model
			gl.glPushMatrix();
				//System.out.println("rover heading = " + roverphi);
				gl.glTranslated(rx-camera_pos.val[0], ry-camera_pos.val[1], rz);
				gl.glRotated(roverphi, 0,0,-1);
				
		    	drawRover(gl);
				drawArm(gl);
			gl.glPopMatrix();
		} else {
			drawIconAt(gl, rvec, rover_icon, .1, .1);
		}
		
		
		gl.glPushMatrix();
			//drawWaypoint(gl, home);
			for(Waypoint w : waypoints){
				drawWaypoint(gl, w);
			}
		gl.glPopMatrix();

		gl.glPopMatrix();
		
		//draw on screen controls
		if(draw_compass) drawCompass(gl);
		if(draw_ascale) drawAttitudeScale(gl);
				
		gl.glFlush();
	}
	
	private void drawIconAt(GL gl, HTVectord pos, Texture t, double size_x, double size_y){
		gl.glColor4ub((byte)255, (byte)255, (byte)255, (byte)255);
		HTVectord rpos = viewMatrix.vecmultiply(pos);
		rpos.mult(1/Math.abs(rpos.val[2]));
//		rpos.val[0] /= -rpos.val[2];
//		rpos.val[1] /= -rpos.val[2];
		gl.glPushMatrix();
			gl.glMultMatrixd(viewMatrix.getInverse().get1DArray(), 0);
			
			if(camera_pos.val[2] >= 10000){ 
				gl.glScaled(1000.0, 1000.0, 1000.0);
			}
			rpos.val[0] = Math.max(Math.min(1*aspect, rpos.val[0]), -1*aspect);
			rpos.val[1] = Math.max(Math.min(1, rpos.val[1]), -1);
			gl.glTranslated(rpos.val[0], rpos.val[1], rpos.val[2]);
			//System.out.println("drawing icon at " + rpos.val[0] + " " + rpos.val[1]);
			t.enable();
			t.bind();
			
			TextureCoords tc = t.getImageTexCoords();
			gl.glBegin(GL.GL_QUADS);

				gl.glTexCoord2f(tc.left(),tc.bottom()); gl.glVertex3d( -size_x/2, -size_y/2, 0);
				gl.glTexCoord2f(tc.right(), tc.bottom()); gl.glVertex3d( size_x/2, -size_y/2, 0);
				gl.glTexCoord2f(tc.right(), tc.top()); gl.glVertex3d( size_x/2, size_y/2, 0);
				gl.glTexCoord2f(tc.left(), tc.top()); gl.glVertex3d( -size_x/2, size_y/2, 0);
			gl.glEnd();
			t.disable();
			
		gl.glPopMatrix();
	}
	
	private void drawCompass(GL gl){
		gl.glPushMatrix();
		gl.glLineWidth(2);
		gl.glColor4ub((byte)0, (byte)255, (byte)0, (byte)255);
		gl.glBegin(GL.GL_LINES);
		{
			for(float angle = -180;angle<180;angle++){
				float x = (float) Math.tan(phi+(angle)*Math.PI/180);
				float length = .025f;
				if(angle % 90 == 0) length = .1f;
				else if(angle % 45 == 0) length = .08f;
				else if(angle % 10 == 0) length = .06f;
				else if(angle % 5 == 0) length = .04f;

				gl.glVertex3f(x, 1.0f, -1f);
				gl.glVertex3f(x, 1-length, -1f);
			}
		}
		gl.glEnd();
		
		//gl.glRotated(90, -1, 0, 0);
		text_rend.begin3DRendering();
		text_rend.setColor(0,1,0,1);
		float x = (float) Math.tan(phi+(0)*Math.PI/180);
		float y = .8f;
		float z = (float) Math.round(-Math.cos(phi+(0)*Math.PI/180));
		text_rend.draw3D("N", x-.04f, y, z, .01f);
		x = (float) Math.tan(phi+(-90)*Math.PI/180);
		z = (float) Math.round(-Math.cos(phi+(-90)*Math.PI/180));
		text_rend.draw3D("W", x-.04f, y, z, .01f);
		x = (float) Math.tan(phi+(180)*Math.PI/180);
		z = (float) Math.round(-Math.cos(phi+(180)*Math.PI/180));
		text_rend.draw3D("S", x-.04f, y, z, .01f);
		x = (float) Math.tan(phi+(90)*Math.PI/180);
		z = (float) Math.round(-Math.cos(phi+(90)*Math.PI/180));
		text_rend.draw3D("E", x-.04f, y, z, .01f);
		
		x = (float) Math.tan(phi+(-135)*Math.PI/180);
		z = (float) Math.round(-Math.cos(phi+(-135)*Math.PI/180));
		text_rend.draw3D("Sw", x-.04f, y, z, .005f);
		x = (float) Math.tan(phi+(-45)*Math.PI/180);
		z = (float) Math.round(-Math.cos(phi+(-45)*Math.PI/180));
		text_rend.draw3D("Nw", x-.04f, y, z, .005f);
		x = (float) Math.tan(phi+(135)*Math.PI/180);
		z = (float) Math.round(-Math.cos(phi+(135)*Math.PI/180));
		text_rend.draw3D("Se", x-.04f, y, z, .005f);
		x = (float) Math.tan(phi+(45)*Math.PI/180);
		z = (float) Math.round(-Math.cos(phi+(45)*Math.PI/180));
		text_rend.draw3D("Ne", x-.04f, y, z, .005f);
		
		text_rend.end3DRendering();
		
		gl.glColor4ub((byte)255, (byte)0, (byte)0, (byte)255);
		gl.glBegin(GL.GL_LINES);
		{
			gl.glVertex3d(0, 1.0f, -1f);
			gl.glVertex3d(0, -.1, -1f);
			
		}
		gl.glEnd();
		
		
		gl.glPopMatrix();
	}
	
	private void drawAttitudeScale(GL gl){
		gl.glPushMatrix();
		gl.glLineWidth(2);
		gl.glColor4ub((byte)0, (byte)255, (byte)0, (byte)255);
		gl.glBegin(GL.GL_LINES);
		{
			for(float angle = -180;angle<180;angle++){
				float y = (float) -Math.tan(theta+(angle)*Math.PI/180);
				float length = .025f;
				if(angle % 90 == 0) length = .1f;
				else if(angle % 45 == 0) length = .08f;
				else if(angle % 10 == 0) length = .06f;
				else if(angle % 5 == 0) length = .04f;

				gl.glVertex3d(-1.0*aspect, y, -1f);
				gl.glVertex3d(-1*aspect+length, y, -1f);
			}
		}
		gl.glEnd();
		
		//gl.glRotated(90, -1, 0, 0);
		text_rend.begin3DRendering();
		text_rend.setColor(0,1,0,1);
		//render theta = 0
		
		float x, y, z;
		x = -.8f;
		for(int angle = 0;angle < 360; angle+=45){
			//render theta lable
			y = (float) -Math.tan(theta+(-angle)*Math.PI/180);
			z = (float) Math.round(-Math.cos(theta+(-angle)*Math.PI/180));
			text_rend.draw3D("" + (angle-90), x, y, z, .005f);
			
		}

		text_rend.end3DRendering();
		
		gl.glColor4ub((byte)255, (byte)0, (byte)0, (byte)255);
		gl.glBegin(GL.GL_LINES);
		{
			gl.glVertex3d(-1*aspect, 0, -1f);
			gl.glVertex3d(.1, 0, -1f);
			
		}
		gl.glEnd();
		
		
		gl.glPopMatrix();
	}
	
	private void drawTerrain(GL gl){
	    double clat = y2lat(camera_pos.val[1]);
		double clon = x2lon(camera_pos.val[0], GeoData.adj_latitude);
    	double height = camera_pos.val[2]-geo.get(clat, clon);
    	
    	int xp_up, xp_down, yp_up, yp_down;
    	int xp_prev, yp_prev;
    	
    	TexTile.drawLines = draw_lines;
    	TexTile.drawTexture = draw_textures;
    	
    	int targetLOD = 25 - (int) (Math.log(height-500)/Math.log(2));
    	targetLOD = Math.max(1, targetLOD);
    	TexTile t = TexTileDB.get(clat, clon, targetLOD);
    	while(t == null && targetLOD > 0){
    		//System.out.println(targetLOD);
    		targetLOD--;
    		t = TexTileDB.get(clat, clon, targetLOD);
    	}
    	if(t != null){
			double latinc = t.latmax-t.latmin;
			double loninc = t.lonmax-t.lonmin;
			
			//------------
			//|  Q  |  R |
			//------------
			//|  T  |  S |
			//------------
			
//			System.out.println("latinc = " + latinc);
//			System.out.println("loninc = " + loninc);
//			System.out.println("IntialLOD " + targetLOD + " address ends with " + t.address.charAt(t.address.length()-1));
			
			xp_up = 1;
	    	xp_down = 1;
			yp_up = 1;
			yp_down = 1;
			
			if(t.address.endsWith("q")){
				xp_down++;
				yp_up++;
			} else if(t.address.endsWith("r")){
				xp_up++;
				yp_up++;
			} else if(t.address.endsWith("t")){
				xp_down++;
				yp_down++;
			} else {
				xp_up++;
				yp_down++;
			}
			
			if(t.address.endsWith("q")){
				xp_prev = -1;
				yp_prev = 1;
			} else if(t.address.endsWith("r")){
				xp_prev = 1;
				yp_prev = 1;
			} else if(t.address.endsWith("t")){
				xp_prev = -1;
				yp_prev = -1;
			} else {
				xp_prev = 1;
				yp_prev = -1;
			}
			
			System.out.println("Highest res rendering " + t.address);
			
			//draw highest resolution areas
	       	for(int i = -xp_down;i<=xp_up;i++){
				for(int j = -yp_down;j<=yp_up;j++){
					gl.glPushMatrix();
					TexTile tile = TexTileDB.get(j*latinc+clat, i*loninc+clon, targetLOD);
					if(tile != null){
						gl.glTranslated(lon2x(tile.lon, GeoData.adj_latitude)-camera_pos.val[0], lat2y(tile.lat)-camera_pos.val[1], 0);
						tile.Render(gl, glu, geo);
					}else{
	//					tile = TexTileDB.get(i*latinc+roverlat, i*loninc+roverlon, targetLOD-1);
	//					gl.glTranslated(lon2x(tile.lon, GeoData.adj_latitude)-camera_pos.val[0], lat2y(tile.lat)-camera_pos.val[1], 0);
	//					tile.Render(gl, glu, geo);
					}
					gl.glPopMatrix();
				}
	       	}
	       	//draw all other levels of detail
	       	while(targetLOD > 2){
		       	t = null;
		       	//find next available tile
		       	while(t == null && targetLOD > 2){
		       		targetLOD--;
		       		t = TexTileDB.get(clat, clon, targetLOD);
		    	}
		       	
		       	if(t == null) break;
		       	
		       	//System.out.println("LOD " + targetLOD + " address ends with " + t.address.charAt(t.address.length()-1));
		       	
		       	xp_up = 1;
		    	xp_down = 1;
				yp_up = 1;
				yp_down = 1;
				
				if(t.address.endsWith("q")){
					xp_down++;
					yp_up++;
				} else if(t.address.endsWith("r")){
					xp_up++;
					yp_up++;
				} else if(t.address.endsWith("t")){
					xp_down++;
					yp_down++;
				} else {
					xp_up++;
					yp_down++;
				}
		       	
				latinc = t.latmax-t.latmin;
				loninc = t.lonmax-t.lonmin;
				
				//System.out.println("Rendering latinc = " + latinc + " loninc = " + loninc);
				
				for(int i = -xp_down;i<=xp_up;i++){
					for(int j = -yp_down;j<=yp_up;j++){
						//if(i == 0 && j == 0) continue;
						if(i <= Math.max(0, xp_prev) && i >= Math.min(0, xp_prev) && 
								j <= Math.max(0, yp_prev) && j >= Math.min(0, yp_prev)){
							//System.out.println("i = " + i + " j = " + j + " xp_prev = " + xp_prev + " yp_prev = " + yp_prev);
							
							continue;
						}
							
						gl.glPushMatrix();
						TexTile tile = TexTileDB.get(j*latinc+clat, i*loninc+clon, targetLOD);
						if(tile != null){
							gl.glTranslated(lon2x(tile.lon, GeoData.adj_latitude)-camera_pos.val[0], lat2y(tile.lat)-camera_pos.val[1], 0);
							tile.Render(gl, glu, geo);
						}else{
							//System.out.println("Null Tile at " + (j*latinc+clat) + " " + (i*loninc+clon));
		//					tile = TexTileDB.get(i*latinc+roverlat, i*loninc+roverlon, targetLOD-1);
		//					gl.glTranslated(lon2x(tile.lon, GeoData.adj_latitude)-camera_pos.val[0], lat2y(tile.lat)-camera_pos.val[1], 0);
		//					tile.Render(gl, glu, geo);
						}
						gl.glPopMatrix();
					}
		       	}
				
				if(t.address.endsWith("q")){
					xp_prev = -1;
					yp_prev = 1;
				} else if(t.address.endsWith("r")){
					xp_prev = 1;
					yp_prev = 1;
				} else if(t.address.endsWith("t")){
					xp_prev = -1;
					yp_prev = -1;
				} else {
					xp_prev = 1;
					yp_prev = -1;
				}
				
				
				
	       	}
    	}
		
	}
	
	//This function draws generic ground tiles, it was used in previous iterations but is no long used
	private void drawGround(GL gl){
		
		TextureCoords tc = ground.getImageTexCoords();
		gl.glColor4ub((byte)255, (byte)255, (byte)255, (byte)255);
		gl.glPushMatrix();
		gl.glTranslated( 0 , 0 , 0 ); // move position of the texture to this coord
		
		int x = geo.getxindex(roverlon);
		int y = geo.getyindex(roverlat);
		int xpanels = 100;
		int ypanels = 100;

		float halfsize = (float)(geo.getGridSize()/2);
		
		if(draw_textures){
			ground.enable();
			ground.bind();
			gl.glBegin(GL.GL_QUADS);
			for(int i = -xpanels/2;i<xpanels/2;i++){
				for(int j = -ypanels/2;j<ypanels/2;j++){
	//				System.out.println(geo.get(x+i, y+j));
					gl.glTexCoord2f(tc.left(), tc.bottom()); gl.glVertex3f( i*halfsize, j*halfsize, geo.get(x+i, y+j));
					gl.glTexCoord2f(tc.right(), tc.bottom()); gl.glVertex3f( (i+1)*halfsize, j*halfsize, geo.get(x+i+1, y+j));
					gl.glTexCoord2f(tc.right(), tc.top()); gl.glVertex3f( (i+1)*halfsize, (j+1)*halfsize, geo.get(x+i+1, y+j+1));
					gl.glTexCoord2f(tc.left(), tc.top()); gl.glVertex3f( i*halfsize, (j+1)*halfsize, geo.get(x+i, y+j+1));
	
				}
			}
			gl.glEnd();
		} else {
			gl.glColor4ub((byte)243, (byte)172, (byte)75, (byte)255);
			gl.glBegin(GL.GL_QUADS);
			for(int i = -xpanels/2;i<xpanels/2;i++){
				for(int j = -ypanels/2;j<ypanels/2;j++){
	//				System.out.println(geo.get(x+i, y+j));
					gl.glVertex3f( i*halfsize, j*halfsize, geo.get(x+i, y+j));
					gl.glVertex3f( (i+1)*halfsize, j*halfsize, geo.get(x+i+1, y+j));
					gl.glVertex3f( (i+1)*halfsize, (j+1)*halfsize, geo.get(x+i+1, y+j+1));
					gl.glVertex3f( i*halfsize, (j+1)*halfsize, geo.get(x+i, y+j+1));
	
				}
			}
			gl.glEnd();
		}
		
		
		if(draw_lines){
			gl.glPolygonMode(GL.GL_FRONT_AND_BACK, GL.GL_LINE);
			gl.glLineWidth(2);

			gl.glColor3ub((byte)0, (byte)255, (byte)0);
			gl.glBegin(GL.GL_QUADS);
			for(int i = -xpanels/2;i<xpanels/2;i++){
				for(int j = -ypanels/2;j<ypanels/2;j++){
//					System.out.println(geo.get(x+i, y+j));
					gl.glVertex3f( i*halfsize, j*halfsize, geo.get(x+i, y+j));
					gl.glVertex3f( (i+1)*halfsize, j*halfsize, geo.get(x+i+1, y+j));
					gl.glVertex3f( (i+1)*halfsize, (j+1)*halfsize, geo.get(x+i+1, y+j+1));
					gl.glVertex3f( i*halfsize, (j+1)*halfsize, geo.get(x+i, y+j+1));
				}
			}
			gl.glEnd();
			gl.glLineWidth(1);
		}
		
		ground.disable();
		gl.glPopMatrix();

		
	}
	
	private void drawRover(GL gl){
		//later we can add the ability to load the rover drawing from vrml
		//for now we'll just hardcode it
		
		gl.glColor4ub((byte)255, (byte)255, (byte)255, (byte)255);
		gl.glScaled(1/in_m, 1/in_m, 1/in_m);
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
					gl.glTranslated(i*19, j*11, 5.5f);
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
					gl.glTranslated(i*19, j*11, 5.5f);
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
		gl.glTranslated(0, 15, 19);
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
			gl.glTranslated(1.5f, 1.5f, .75f);
			gl.glRotatef(-90, 1, 0, 0);
			glu.gluCylinder(quadratic, .25f,.25f,.5f,5,1);
		gl.glPopMatrix();
			gl.glTranslated(-1.5f, 1.5f, .75f);
			gl.glRotatef(-90, 1, 0, 0);
			glu.gluCylinder(quadratic, .25f,.25f,.5f,5,1);
		gl.glPopMatrix();
		
		
	}
	
	private void drawArm(GL gl){

		gl.glTranslated(10, 16, 18);
		
		gl.glRotated(180*joints[0]/PI, 0, 0, 1);
		
		gl.glPolygonMode(GL.GL_FRONT_AND_BACK, GL.GL_FILL);

		//base
		gl.glColor3ub((byte)75, (byte)75, (byte)75);
		glut.glutSolidCylinder(2, 5, 4, 1);

		//upper arm
		gl.glColor3ub((byte)150, (byte)150, (byte)150);
		gl.glTranslated(0, 0, 5);
		gl.glRotated(-90+180*joints[1]/PI, 1, 0, 0);
		glut.glutSolidCylinder(1, 15, 4, 1);
		
		//lower arm segment
		gl.glColor3ub((byte)100, (byte)100, (byte)100);
		gl.glTranslated(0,0,15);
		gl.glRotated(180*joints[2]/PI, 1, 0, 0);	
		glut.glutSolidCylinder(.5, 5, 4, 1);
		
		//wrist rotation
		gl.glColor3ub((byte)150, (byte)150, (byte)150);
		gl.glTranslated(0,0,5);
		gl.glRotated(180*joints[3]/PI, 0, 0, 1);	
		glut.glutSolidCylinder(.5, 10, 4, 1);
		
		//wrist extension
		gl.glColor3ub((byte)100, (byte)100, (byte)100);
		gl.glTranslated(0,0,10);
		gl.glRotated(180*joints[4]/PI, 1, 0, 0);	
		glut.glutSolidCylinder(.375, 5, 4, 1);
		
	}
	
	private void drawWaypoint(GL gl, Waypoint way){
		
		
		
		double wx = lon2x(way.lon, GeoData.adj_latitude);
		double wy = lat2y(way.lat);
		double wz = geo.get(way.lat, way.lon);
		
		HTVectord wvec = new HTVectord(wx, wy, wz, 0);
		
		wvec.sub(camera_pos);
		double dist = wvec.getMagnitude();
		if(dist < 100){ //if we're closer than 100 meters draw the rover model
			gl.glPushMatrix();
	
				//System.out.println("wx = " + wx + " wy = " + wy);
				gl.glTranslated(wx-camera_pos.val[0], wy-camera_pos.val[1], way.z);
				//gl.glTranslated(wx, wy, way.z);
				
				gl.glScaled(1/in_m, 1/in_m, 1/in_m);
				
				gl.glPolygonMode(GL.GL_FRONT_AND_BACK, GL.GL_FILL);
		
				//base
				gl.glColor3ub(way.color[0], way.color[1], way.color[2]);
				glut.glutSolidCylinder(5, 200, 8, 1);
		
				gl.glTranslated(0, 0, 200);
				
				gl.glBegin(GL.GL_TRIANGLES);
					//bottom
					gl.glVertex3f(0f, 0f, 0f);
					gl.glVertex3f(60f, 0f, -30f);
					gl.glVertex3f(0f, 0f, -60f);
				gl.glEnd();
			
			gl.glPopMatrix();
		} else {
			drawIconAt(gl, wvec, waypoint_icon, .1, .1);
		}
	}

	@Override
	public void init(GLAutoDrawable drawable) {
		System.out.println("Init Called");
		GL gl = drawable.getGL();
		glu.gluPerspective(fovy, aspect, .5, 1000000.0);
		gl.glEnable(GL.GL_DEPTH_TEST);
		gl.glEnable(GL.GL_TEXTURE_2D);
		//gl.glEnable (GL.GL_LINE_SMOOTH);
		//gl.glEnable (GL.GL_POINT_SMOOTH);
		//gl.glEnable (GL.GL_BLEND);
		
		//gl.glHint(GL.GL_LINE_SMOOTH_HINT, GL.GL_DONT_CARE);

		
		//gl.glClearColor(168/255f, 211/255f, 255/255f, 1);
		gl.glClearColor(0f, 0f, 0f, 1);
		gl.glClearDepth (1.0f);

		try{
			String path = getClass().getResource("/rover_icon.bmp").toURI().getPath();
			//System.out.println(path);
			rover_icon = load(path);
		}catch(Exception e){
			e.printStackTrace();
		}
		
		try{
			String path = getClass().getResource("/flag_icon.png").toURI().getPath();
			//System.out.println(path);
			waypoint_icon = load(path);
		}catch(Exception e){
			e.printStackTrace();
		}
		
		
//		TexTileDB.initialize();
//		TexTile t = TexTileDB.get(lat, lon);
//		ground = t.texture;
		
//		gl.glEnable(GL.GL_LIGHTING);
//		gl.glShadeModel(GL.GL_SMOOTH);
//		
//		float[] ambient_light = { 0f, 0f, 0f };
//		FloatBuffer ambient = FloatBuffer.wrap(ambient_light);
//		gl.glLightModelfv(GL.GL_LIGHT_MODEL_AMBIENT, ambient);
//		
//		float[] ambient_light_model = { 1f, 1f, 1f };
//		FloatBuffer ambient_model = FloatBuffer.wrap(ambient_light_model);
//		gl.glLightfv(GL.GL_LIGHT0, GL.GL_AMBIENT, ambient_model);
//		
//		float[] specular_light = { .5f, .5f, .5f };
//		FloatBuffer specular = FloatBuffer.wrap(specular_light);
//		gl.glLightfv(GL.GL_LIGHT0, GL.GL_SPECULAR, specular);
//		
//		float[] diffuse_light= { 0f, 0f, 0f };
//		FloatBuffer diffuse = FloatBuffer.wrap(diffuse_light);
//		gl.glLightfv(GL.GL_LIGHT0, GL.GL_DIFFUSE, diffuse);
//		
//		float[] position_light = { 100f, 100f, 1000f };
//		FloatBuffer position = FloatBuffer.wrap(position_light);
//		gl.glLightfv(GL.GL_LIGHT0, GL.GL_POSITION, position);
//
//		gl.glEnable(GL.GL_LIGHT0);
//		float[] material = { 1.0f, 1f, 1f, 1f };
//		mat1 = FloatBuffer.wrap(material);
//		gl.glMaterialfv(GL.GL_FRONT, GL.GL_AMBIENT_AND_DIFFUSE, mat1);
//		gl.glEnable(GL.GL_COLOR_MATERIAL);
//		gl.glColorMaterial(GL.GL_FRONT, GL.GL_AMBIENT_AND_DIFFUSE);
		
		text_rend = new TextRenderer(new Font("SansSerif", Font.BOLD, 10));


	}

	@Override
	public void reshape(GLAutoDrawable drawable, int x, int y, int w, int h){
		//System.out.println("x = " + x + " y = " + y + " w = " + w + " h = " + h);
		GL gl = drawable.getGL();
		aspect = w;
		aspect /= h;
		fovx = aspect*fovy;
		gl.glLoadIdentity();
		glu.gluPerspective(fovy, aspect, .5, 1000000.0);
		repaint();
	}

	@Override
	public void displayChanged(GLAutoDrawable drawable, boolean modeChanged,
			boolean deviceChanged) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void TileLoaded() {
		repaint();
	}

}
