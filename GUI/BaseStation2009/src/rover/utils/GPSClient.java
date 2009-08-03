package rover.utils;

import java.net.DatagramPacket;
import java.net.InetAddress;
import java.net.MulticastSocket;
import java.util.ArrayList;

import rover.network.Packet;
import rover.network.PacketHandler;

public class GPSClient implements Runnable{
    //public delegate void gpsDataHandler(String data);

    //public event gpsDataHandler GPSDataReceived;

    private Thread thread;
    private MulticastSocket socket;

    private InetAddress multiCastGroup;
    private int mcport;
    
    private ArrayList<GPSHandler> gpsHandlerList;
    
    //private System.Text.ASCIIEncoding encoding;
    //EndPoint localEP;
    //EndPoint rEP;

    public interface GPSHandler{
    	
    	public void GPSDataReceived(String data);
    }
    
   
    public GPSClient(InetAddress multiCastGroup, int mcport)
    {
        this.multiCastGroup = multiCastGroup;
        this.mcport = mcport;
        
        gpsHandlerList = new ArrayList<GPSHandler>();
        try{
    		 socket = new MulticastSocket(mcport);
    	     socket.setReuseAddress(true);
    	     socket.joinGroup(multiCastGroup);
    	     
//    	     socket.leaveGroup(multiCastGroup);
//    	     socket.close();

    	}
    	catch(Exception e){
    		e.printStackTrace();
    	}
        
        Thread thread = new Thread(this);
        thread.setDaemon(true);
        thread.start();
    }


	@Override
	public void run() {
		while(true)
        {
            try{
            	if(socket == null){
            		 socket = new MulticastSocket(mcport);
            	     socket.setReuseAddress(true);
            	     socket.joinGroup(multiCastGroup);
            	}
	            byte[] buf = new byte[1024];
				DatagramPacket recv = new DatagramPacket(buf, buf.length);
				//System.out.println("GPS Client Listening for Packets");
				socket.receive(recv);
	
				recv.getLength();
				//System.out.println("GPS Packet received");
	            String str = new String(buf);
	            if (str.length() < 5) continue;
	            GPSDataReceived(str);
            } catch (Exception e){
            	
            }
        }

    }

	public void addGPSListener(GPSHandler ph){
		gpsHandlerList.add(ph);
	}
	
	public void removeGPSHandler(GPSHandler ph){
		gpsHandlerList.remove(ph);
	}

	private void GPSDataReceived(String data){
		for(GPSHandler ph : gpsHandlerList){
			ph.GPSDataReceived(data);
		}
	}

}
