package rover.portal;

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.DisplayMode;
import java.awt.Frame;
import java.awt.Graphics;
import java.awt.GraphicsDevice;
import java.awt.GraphicsEnvironment;

import javax.imageio.ImageIO;
import javax.swing.JPanel;
import javax.swing.JFrame;
import java.awt.Dimension;
import javax.swing.BoxLayout;
import java.awt.GridBagLayout;
import java.awt.Rectangle;
import javax.swing.JLabel;
import java.awt.Point;
import java.awt.image.BufferedImage;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.net.DatagramPacket;
import java.net.InetAddress;
import java.net.MulticastSocket;
import java.net.URL;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;
import java.util.ArrayList;
import java.util.Date;
import javax.swing.JButton;

import rover.Main;
import rover.guistuff.ImagePanel;
import rover.network.MulticastListener;
import rover.network.Packet;
import rover.network.PacketHandler;
import rover.network.SocketInfo;
import rover.network.VideoClientSystem;
import rover.network.VideoPacket;
import rover.network.VideoPacketBuffer;
import rover.utils.Stopwatch;
import rover.video.Avi_Chunk;
import rover.video.FrameListener;

import javax.media.Buffer;
import javax.media.Codec;
import javax.media.Controls;
import javax.media.Format;
import javax.media.Manager;
import javax.media.MediaLocator;
import javax.media.Player;
import javax.media.PlugIn;
import javax.media.Time;
import javax.media.Track;
import javax.media.bean.playerbean.MediaPlayer;
import javax.media.format.RGBFormat;
import javax.media.format.VideoFormat;
import javax.media.opengl.GL;
import javax.media.opengl.GLAutoDrawable;
import javax.media.opengl.GLCanvas;
import javax.media.opengl.GLCapabilities;
import javax.media.opengl.GLEventListener;
import javax.media.opengl.glu.GLU;

import com.ibm.media.codec.video.VideoCodec;
import com.omnividea.media.codec.video.JavaDecoder;
import com.omnividea.media.codec.video.NativeDecoder;
//import rover.video.JavaDecoder;
import com.sun.media.BasicCodec;
import com.sun.opengl.util.GLUT;
import com.omnividea.media.parser.video.Parser;
import com.omnividea.media.protocol.file.DataSource;
import com.omnividea.media.renderer.video.Java2DRenderer;
import javax.swing.ImageIcon;
import java.awt.Color;
import javax.swing.JRadioButton;
import javax.swing.JToggleButton;
import javax.swing.JComboBox;

import net.sourceforge.jffmpeg.codecs.video.mpeg4.divx.DIVXCodec;

public class GlassesPortal extends Portal implements GLEventListener, FrameListener {

	private static final long serialVersionUID = 1L;
	private JPanel jContentPane = null;  
	BufferedImage cur_image = null;  
	Stopwatch fps_watch;
	Thread dispThread;
	
	boolean white = true;
	
	private GLCapabilities caps;
	private GLU glu;  //  @jve:decl-index=0:
	private GLUT glut;
	//private GlassesPortal This = this;
	Stopwatch sw = new Stopwatch();

	private Java2DRenderer java2DRenderer = null;
	private GLCanvas GLCanvas = null;
	/**
	 * This is the default constructor
	 */
	public GlassesPortal(Frame owner) {
		super(owner);
		initialize();
		VideoClientSystem.init();
		dispThread = new Thread(){	@Override public void run() {vgamux();}};
		dispThread.setDaemon(true);
		dispThread.start();
		
	}
	
	private void vgamux(){
		
		while(true){
			
			GLCanvas.repaint();
			
		}
	}
	
	
	@Override
	public String toString(){
		return this.getTitle();
	}
	
	public static class GlassesPortalInit extends PortalInit {
		@Override
		public Portal createPortal() {
			return new GlassesPortal(MainGUI.instance);
		}
		
		@Override
		public String toString(){
			return "Glasses Portal";
		}
	}
	
	
	
	/**
	 * This method initializes this
	 * 
	 * @return void
	 */
	private void initialize() {


		this.setContentPane(getJContentPane());
		this.setTitle("Video");		
		
		GraphicsEnvironment ge = GraphicsEnvironment.getLocalGraphicsEnvironment();
		GraphicsDevice gd = ge.getDefaultScreenDevice();
		DisplayMode[] dms = gd.getDisplayModes();
		for(DisplayMode mode : dms){
			System.out.println(mode.getWidth() + " " + mode.getHeight() + " " + mode.getRefreshRate() + " hz");
			
		}
		
		DisplayMode mode = gd.getDisplayMode();
		
		
		System.out.println(ge);
		System.out.println(gd);
		
//		int width = 640;
//		int height = 480;
		int width = mode.getWidth();
		int height = mode.getHeight();
		
		this.setUndecorated(true);
		this.setSize(width, height);
		GLCanvas.setSize(width, height);  
	}


	
	/**
	 * This method initializes jContentPane
	 * 
	 * @return javax.swing.JPanel
	 */
	private JPanel getJContentPane() {
		if (jContentPane == null) {
			jContentPane = new JPanel();
			//jContentPane.add(getJava2DRenderer(), null);
			jContentPane.setLayout(null);
			jContentPane.add(getGLCanvas(), null);
		}
		return jContentPane;
	}

	@Override
	public void Shutdown() {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void nextFrameReady(Buffer current_frame, int channel) {
		//System.out.println("It's working !!!");
		//getJava2DRenderer().process(current_frame);
	}

	/**
	 * This method initializes GLCanvas	
	 * 	
	 * @return javax.media.opengl.GLCanvas	
	 */
	private GLCanvas getGLCanvas() {
		if (GLCanvas == null) {
			caps = new GLCapabilities();
		    caps.setDoubleBuffered(false);
		    System.out.println(caps.toString());
		    
		    glu = new GLU();
		    glut = new GLUT();
			GLCanvas = new GLCanvas();
			GLCanvas.addGLEventListener(this);
		}
		return GLCanvas;
	}

	@Override
	public void display(GLAutoDrawable drawable) {
		GL gl = drawable.getGL();
		gl.glPushMatrix();
		
		gl.glClear(GL.GL_COLOR_BUFFER_BIT | GL.GL_DEPTH_BUFFER_BIT);
		
		if(white){
			gl.glColor3ub((byte)255, (byte)255, (byte)255);
			white = false;
		} else{
			gl.glColor3ub((byte)0, (byte)0, (byte)0);
			white = true;
		}
		
		gl.glPolygonMode(GL.GL_FRONT_AND_BACK, GL.GL_FILL);
		
		gl.glBegin(GL.GL_QUADS);
			gl.glVertex3f(-1f, 1f, -1f);
			gl.glVertex3f(1f, 1f, -1f);
			
			gl.glVertex3f(1f, -1f, -1f);
			
			gl.glVertex3f(-1, -1f, -1f);
		gl.glEnd();
		
		gl.glPopMatrix();
		
		gl.glFlush();
		gl.glFinish();
		
		sw.Stop();
		System.out.println("fps = " + 1000.0/(sw.getElapsedMillis()));
		sw.Reset();
		sw.Start();
		
	
	}

	@Override
	public void displayChanged(GLAutoDrawable drawable, boolean modeChanged,
			boolean deviceChanged) {
		// TODO Auto-generated method stub
		
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
	
}  //  @jve:decl-index=0:visual-constraint="10,10"
