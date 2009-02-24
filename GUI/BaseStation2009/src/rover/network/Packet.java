package rover.network;

public  class Packet
{
    public int bytes;
    public byte[] packet;

    public Packet(){
    	bytes = -1;
    	packet = null;
    }
    
    public Packet(byte[] packet, int bytes)
    {
        this.bytes = bytes;
        this.packet = packet;
    }

}