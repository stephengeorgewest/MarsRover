package rover;

public  class Packet
{
    public int bytes;
    public byte[] packet;

    public Packet(byte[] packet, int bytes)
    {
        this.bytes = bytes;
        this.packet = packet;
    }

}