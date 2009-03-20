package rover.network;

import java.net.InetAddress;

import rover.utils.Stopwatch;

public class SocketInfo {

	public static Class[] classTypes = {String.class, String.class, String.class, Integer.class, InetAddress.class, Double.class, Integer.class};
	public static String[] Names = {"Portal", "Type", "Status", "Port", "Address", "PPS", "Total Packets"};
	public static boolean[] editable = {false, false, false, true, true, false, false};
	
	public Object[] data;
	public int packetCount;
	public int oldpacketCount;
	public Stopwatch sw;
	
	public SocketInfo(){
		data = new Object[classTypes.length];
		packetCount = 0;
		oldpacketCount = 0;
		for(int i = 0;i<classTypes.length;i++){
			if(classTypes[i] == String.class)
				data[i] = "NI";
			else if(classTypes[i] == Double.class)
				data[i] = new Double(0);
		}
		sw = new Stopwatch();
		sw.Start();
	}
	
	public void setPortal(String portal){
		data[0] = portal;
	}
	public void setType(String type){
		data[1] = type;
	}
	public void setStatus(String status){
		data[2] = status;
	}
	public void setPort(Integer port){
		data[3] = port;
	}
	public void setAddress(InetAddress address){
		data[4] = address;
	}
	public void setPPS(double pps){
		data[5] = new Double(pps);
	}
	public void setTotalPackets(Integer totalpackets){
		data[6] = totalpackets;
	}
	public void incPacketCount(int i){
		packetCount += i;
	}
	
	public void recalculatePPS(){
		double deltaT = sw.getElapsedSeconds();
		sw.Reset();
		double deltaP = packetCount - oldpacketCount;
		oldpacketCount = packetCount;
		double pps = deltaP/deltaT;
		data[5] = new Double(pps);
	}
	
}
