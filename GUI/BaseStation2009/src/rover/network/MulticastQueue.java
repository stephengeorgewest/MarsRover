package rover.network;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.InetAddress;
import java.net.MulticastSocket;
import java.util.LinkedList;


public class MulticastQueue extends Thread{

	MulticastSocket socket;
	private InetAddress mc_address;
	private int mc_port;

	private LinkedList<Packet> queue;
	private Object queue_lock;
	
	public MulticastQueue(InetAddress address, int port) throws IOException{
		this.mc_address = address;
		this.mc_port = port;
		this.setDaemon(true);
		queue_lock = new Object();
		queue = new LinkedList<Packet>();
		socket = new MulticastSocket();
	}
	
	public void Enqueue(Packet p){
		synchronized(queue_lock){
			queue.add(p);
		}
	}
	
	public void run(){
		
		while (true)
        {
			try{
	            int count = 0;
	            synchronized (queue_lock)
	            {
	                count = queue.size();
	            }
	
	            if (count > 0)
	            {
	                //Console.WriteLine("Sending " + count + " packets");
	            	
	            	synchronized( queue_lock){
	                while (queue.size() > 0)
	                {
	                    //send the packet
	                    Packet p = queue.poll();
	                    DatagramPacket dp = new DatagramPacket(p.packet, p.bytes, mc_address, mc_port);
	                    socket.send(dp);
	                }}
	            }
	            else
	            {
	                Thread.sleep(5);
	            }
			}
        	catch(Exception e){
        		e.printStackTrace();
        	}
        }
	}
	
}
