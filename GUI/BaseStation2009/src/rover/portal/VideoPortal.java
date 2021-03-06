package rover.portal;

import java.awt.BorderLayout;
import java.awt.Frame;
import java.awt.Graphics;

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
import java.io.InputStream;
import java.net.DatagramPacket;
import java.net.InetAddress;
import java.net.MulticastSocket;
import java.util.Date;
import javax.swing.JButton;

import rover.Main;
import rover.guistuff.ImagePanel;
import rover.network.MulticastListener;
import rover.network.Packet;
import rover.network.PacketHandler;
import rover.network.SocketInfo;
import rover.network.VideoPacket;
import rover.network.VideoPacketBuffer;
import rover.utils.Stopwatch;
import rover.video.BufferedImageExtractor;

public class VideoPortal extends Portal {

	private static final long serialVersionUID = 1L;
	private JPanel jContentPane = null;
	private ImagePanel videoPanel = null;
	private JPanel displayPanel = null;
	private JLabel jLabel = null;
	private JLabel jLabel1 = null;
	private JLabel widthLabel = null;
	private JLabel heightLabel = null;
	private JLabel fpsLabel = null;
	private JButton jButton = null;

	BufferedImage cur_image = null;  //  @jve:decl-index=0:
	
	Stopwatch fps_watch;
	
	
	VideoPacketBuffer buffer = null;
	//Thread netThread = null;
	MulticastListener ml = null;
	
	Thread videoThread;
	
	//InetAddress mcgroup = null;
	//int mcport = 0;
	
	VideoPacket test1, test2, test3;
	
	
	/**
	 * This is the default constructor
	 */
	public VideoPortal(Frame owner) {
		super(owner);
		initialize();
		
		fps_watch = new Stopwatch();
		try {
		    cur_image = ImageIO.read(this.getClass().getClassLoader().getResourceAsStream("No_Image.GIF"));
		} catch (Exception e) {
			e.printStackTrace();
		}
		SetFrame(cur_image);
		
		
		
		//TODO Add configuration stuff here
		/*try{
			mcgroup = InetAddress.getByName("224.6.2.5");
			mcport = 4625;
		}catch(Exception e){
			e.printStackTrace();
		}*/
		
		try{
			InetAddress mcaddress = InetAddress.getByName(Main.props.getProperty("video1_address"));
			int port = Integer.parseInt(Main.props.getProperty("video1_port"));
			ml = new MulticastListener(mcaddress, port);
			
			SocketInfo si = new SocketInfo();
			si.setPortal("Video Portal");
			si.setPort(port);
			si.setType("MC Rec");
			si.setAddress(mcaddress);
			
			MainGUI.registerSocket(si);
			
			
		}
		catch(Exception e){
			e.printStackTrace();
			return;
		}
		
		ml.addPacketListener(new PacketHandler(){
			@Override
			public void PacketReceived(Packet p) {
				VideoPacket vp = new VideoPacket(p);
				buffer.Process(vp);

			}

		});
		buffer = new VideoPacketBuffer();
		
		
//		ThreadStart t = new ThreadStart(this);
//		netThread = new Thread(t);
//		netThread.setDaemon(true);
//		netThread.start();
	
		
		
		
	}
	
	@Override
	public String toString(){
		return this.getTitle();
	}
	
	public static class VideoPortalInit extends PortalInit {
		@Override
		public Portal createPortal() {
			return new VideoPortal(MainGUI.instance);
		}
		
		@Override
		public String toString(){
			return "Video Portal";
		}
	}
	
	
/*	class ThreadStart implements Runnable{
		VideoPortal parent;
		MulticastSocket s = null;
		
		public ThreadStart(VideoPortal p){
			parent = p;
		}
		
		public void run(){
			
			while(true){
				try{
					if(s == null){
						s = new MulticastSocket(parent.mcport);
						s.joinGroup(parent.mcgroup);
						
					}else{
						byte[] buf = new byte[65000];
						DatagramPacket recv = new DatagramPacket(buf, buf.length);
//						System.out.println("Listening for packets");
						s.receive(recv);
//						System.out.println("Packet Received");
						VideoPacket vp = new VideoPacket(recv.getData(), recv.getLength());
						parent.buffer.Process(vp);
						if(parent.cur_image != buffer.current_image) parent.SetFrame(buffer.current_image);
					}
				}catch (Exception e){
					e.printStackTrace();
				}
				
				
			}
		}
		
	}*/
	
	public void SetFrame(BufferedImage b){

		fps_watch.Stop();
		double fps = 1/fps_watch.getElapsedSeconds();

		fpsLabel.setText(fps + " fps");
		
		fps_watch.Reset();
		fps_watch.Start();
		
		if(b != null){
			cur_image = b;
			heightLabel.setText(b.getHeight() + " pixels");
			widthLabel.setText(b.getWidth() + " pixels");
			videoPanel.setImage(cur_image);
			videoPanel.repaint();
		}
	}
	
	
	
	/**
	 * This method initializes this
	 * 
	 * @return void
	 */
	private void initialize() {
		this.setSize(350, 300);
		this.setContentPane(getJContentPane());
		this.setTitle("Video");
	}

	/**
	 * This method initializes jContentPane
	 * 
	 * @return javax.swing.JPanel
	 */
	private JPanel getJContentPane() {
		if (jContentPane == null) {
			jContentPane = new JPanel();
			jContentPane.setLayout(new BoxLayout(getJContentPane(), BoxLayout.Y_AXIS));
			jContentPane.add(getVideoPanel(), null);
			jContentPane.add(getDisplayPanel(), null);
		}
		return jContentPane;
	}

	@Override
	public void Shutdown() {
		// TODO Auto-generated method stub
		
	}

	/**
	 * This method initializes videoPanel	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getVideoPanel() {
		if (videoPanel == null) {
			videoPanel = new ImagePanel();
			videoPanel.setLayout(null);
		}
		return videoPanel;
	}

	/**
	 * This method initializes displayPanel	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getDisplayPanel() {
		if (displayPanel == null) {
			fpsLabel = new JLabel();
			fpsLabel.setBounds(new Rectangle(185, 23, 72, 17));
			fpsLabel.setText("0 fps");
			heightLabel = new JLabel();
			heightLabel.setBounds(new Rectangle(72, 38, 62, 21));
			heightLabel.setText("NA");
			widthLabel = new JLabel();
			widthLabel.setText("NA");
			widthLabel.setLocation(new Point(72, 11));
			widthLabel.setSize(new Dimension(62, 24));
			jLabel1 = new JLabel();
			jLabel1.setBounds(new Rectangle(5, 35, 50, 23));
			jLabel1.setText("Height:");
			jLabel = new JLabel();
			jLabel.setBounds(new Rectangle(6, 8, 46, 23));
			jLabel.setText("Width:");
			displayPanel = new JPanel();
			displayPanel.setPreferredSize(new Dimension(300, 60));
			displayPanel.setMaximumSize(new Dimension(2000, 60));
			displayPanel.setLayout(null);
			displayPanel.add(jLabel, null);
			displayPanel.add(jLabel1, null);
			displayPanel.add(widthLabel, null);
			displayPanel.add(heightLabel, null);
			displayPanel.add(fpsLabel, null);
			displayPanel.add(getJButton(), null);
		}
		return displayPanel;
	}

	public void ThreadVideoTest(){
		videoThread = new Thread(){	
			@Override public void run() { 
				while(true){
					try{
						videoTest();
						sleep(30);
					}catch(Exception e){
						e.printStackTrace();
					}
					
				}
			}
		};
		videoThread.setDaemon(true);
		videoThread.start();
	}
	
	

	public void videoTest(){

		if(test1 == null){
			try{
				//System.out.println("Testing Video System");
				InputStream s = this.getClass().getClassLoader().getResourceAsStream("test.yuv");
				DataInputStream ds = new DataInputStream(s);
				
	            int psize = 65000;
	            int byte_count = 0;
	            byte[] p1 = new byte[psize];
	            byte[] p2 = new byte[psize];
	            byte[] p3 = new byte[psize];
	
	            int bytes1 = 10;
	            int bytes2 = 10;
	            int bytes3 = 10;
	
	
	            short width  = 352;
	            short height = 288;
	
	            p1[0] = 0; //PacketID
	            p1[1] = 0;                      //ChannelID
	            p1[2] = (byte)(width >> 8);     //Image Width MSB
	            p1[3] = (byte)(width & 0x00FF); //Image Width LSB
	            p1[4] = (byte)(height >> 8);    //Image Height MSB
	            p1[5] = (byte)(height & 0x00FF);//Image Height LSB
	            p1[6] = 0;
	            p1[7] = 5;
	            p1[8] = 1;
	            p1[9] = 3;
	
	            for (int i = 10; i < psize; i++)
	            {
	                ds.read(p1, i, 1);
	                //Console.Write(" " + p1[i]);
	                bytes1++;
	                byte_count++;
	            }
	
	            p2[0] = 0; //PacketID
	            p2[1] = 0;                      //ChannelID
	            p2[2] = (byte)(width >> 8);     //Image Width MSB
	            p2[3] = (byte)(width & 0x00FF); //Image Width LSB
	            p2[4] = (byte)(height >> 8);    //Image Height MSB
	            p2[5] = (byte)(height & 0x00FF);//Image Height LSB
	            p2[6] = 0;
	            p2[7] = 5;
	            p2[8] = 2;
	            p2[9] = 3;
	
	            for (int i = 10; i < psize; i++)
	            {
	                ds.read(p2, i, 1);
	                bytes2++;
	                byte_count++;
	            }
	
	            p3[0] = 0; //PacketID
	            p3[1] = 0;                      //ChannelID
	            p3[2] = (byte)(width >> 8);     //Image Width MSB
	            p3[3] = (byte)(width & 0x00FF); //Image Width LSB
	            p3[4] = (byte)(height >> 8);    //Image Height MSB
	            p3[5] = (byte)(height & 0x00FF);//Image Height LSB
	            p3[6] = 0;
	            p3[7] = 5;
	            p3[8] = 3;
	            p3[9] = 3;
	
	            for (int i = 10; i < psize; i++)
	            {
	                int read = ds.read(p3, i, 1);
	                if (read == 0)
	                {
	                    //Console.WriteLine("read = " + read);
	                    break;
	                }
	                bytes3++;
	                byte_count++;
	                //Console.WriteLine(byte_count);
	            }
	            test1 = new VideoPacket(p1, p1.length);
	            test2 = new VideoPacket(p2, p2.length);
	            test3 = new VideoPacket(p3, p3.length);
	            
		            ds.close();
	
			}catch(Exception ex){
				ex.printStackTrace();
			}
            
		}
            
        buffer.Process(test1);
        buffer.Process(test2);
        buffer.Process(test3);
        
        SetFrame(BufferedImageExtractor.convert(buffer.current_frame));
		
	}

	/**
	 * This method initializes jButton	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getJButton() {
		if (jButton == null) {
			jButton = new JButton();
			jButton.setBounds(new Rectangle(273, 11, 53, 33));
			jButton.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					//videoTest();
					ThreadVideoTest();
				}
			});
		}
		return jButton;
	}

}  //  @jve:decl-index=0:visual-constraint="10,10"
