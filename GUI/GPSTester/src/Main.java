import java.io.IOException;
import java.net.DatagramPacket;
import java.net.InetAddress;
import java.net.MulticastSocket;
import java.nio.charset.Charset;
import java.nio.charset.CharsetDecoder;
import java.nio.charset.CharsetEncoder;
import java.nio.charset.spi.CharsetProvider;

import sun.misc.CharacterEncoder;


public class Main {

	public static void main(String[] args) throws IOException{
		
		Charset charset = Charset.forName("US-ASCII");
	    CharsetDecoder decoder = charset.newDecoder();
	    CharsetEncoder encoder = charset.newEncoder();

		
		int port = 4625;
		// Which address
		String group = "224.6.2.5";
		// Which ttl
		int ttl = 20;

		MulticastSocket s = new MulticastSocket();

		byte buf[] = new byte[1000];
		for (int i=0; i<buf.length; i++) buf[i] = (byte)i;
				
		while(true){
			try{
				String NMEAString;
				int bytes;
				
				NMEAString = "$GPGGA,170834,4124.8963,N,08151.6838,W,1,05,1.5,280.2,M,-34.0,M,,,";
				//NMEAString = "Jibberish";
				bytes = getBytes(NMEAString, buf);
		
				DatagramPacket pack = new DatagramPacket(buf, bytes,
						 InetAddress.getByName(group), port);
				System.out.println("Sending "+ NMEAString);
				s.send(pack);
				
				NMEAString = "$GPGXA asdflajds;flasjf";
				bytes = getBytes(NMEAString, buf);
				pack.setData(buf, 0, bytes);
				System.out.println("Sending "+ NMEAString);
				s.send(pack);
				
				

				NMEAString = "$HCHDG,101.1,,,7.1,W*3C";
				bytes = getBytes(NMEAString, buf);
				pack.setData(buf, 0, bytes);
				System.out.println("Sending "+ NMEAString);
				s.send(pack);

				
				
				NMEAString = "$GPRMC,225446,A,4916.45,N,12311.12,W,000.5,054.7,191194,020.3,E";
				bytes = getBytes(NMEAString, buf);
				pack.setData(buf, 0, bytes);
				System.out.println("Sending "+ NMEAString);
				s.send(pack);
				
				Thread.sleep(1000);
			}catch(Exception e){
				e.printStackTrace();
			}
		}
		
		
		// And when we have finished sending data close the socket
		//s.close();
	}
	
	public static int getBytes(String text, byte[] buf){
		for(int i = 0;i<text.length();i++){
			buf[i] = (byte)text.charAt(i);
		}
		return text.length();
	}
	
}
