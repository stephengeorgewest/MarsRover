package rover.network;

import java.net.DatagramPacket;
import java.net.InetAddress;
import java.net.MulticastSocket;
import java.net.Socket;
import java.util.ArrayList;


public class MulticastListener extends Thread{

	MulticastSocket s;
	private InetAddress mc_address;
	private int mc_port;
	
	private ArrayList<PacketHandler> packetHandlerList; 
	
	public MulticastListener(InetAddress address, int port){
		this.mc_address = address;
		this.mc_port = port;
		packetHandlerList = new ArrayList<PacketHandler>();
		this.setDaemon(true);
	}
	
	public void addPacketListener(PacketHandler ph){
		packetHandlerList.add(ph);
	}
	
	public void removePacketHandler(PacketHandler ph){
		packetHandlerList.remove(ph);
	}

	private void packetReceived(Packet p){
		for(PacketHandler ph : packetHandlerList){
			ph.PacketReceived(p);
		}
	}
	
	public void run(){
		
		while(true){
			try{
				if(s == null){
					s = new MulticastSocket(mc_port);
					s.joinGroup(mc_address);
					
				}else{
					byte[] buf = new byte[65000];
					DatagramPacket recv = new DatagramPacket(buf, buf.length);
//					System.out.println("Listening for packets");
					s.receive(recv);
//					System.out.println("Packet Received");
					Packet p = new Packet(recv.getData(), recv.getLength());
					packetReceived(p);
					
				}
			}catch (Exception e){
				e.printStackTrace();
			}
			
			
		}
	}
	
	
}
