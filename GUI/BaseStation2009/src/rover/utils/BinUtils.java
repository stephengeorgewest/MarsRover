package rover.utils;


/*
 * Name: BinUtils
 * Author: Travis L Brown
 * Description: 
 * Provides convenience functions for dealing with byte arrays
 * 
 */
public class BinUtils {

	
	//converts a 4 byte section of an array to an integer
	//Assumes BigEndian byte order 
	public static int byteArrayToInt(byte[] b, int offset) {
		if(b == null) return Integer.MIN_VALUE;
        int value = 0;
        for (int i = 0; i < 4; i++) {
            int shift = (4 - 1 - i) * 8;
            value += (b[i + offset] & 0x000000FF) << shift;
        }
        return value;
    }
	
	//converts an integer to 4 bytes and places them in the given array at the specified offset
	//Assumes BigEndian byte order
	public static void intToByteArray(int val, byte[] b, int offset){
		if(b == null || offset + 4 > b.length) return;
		b[offset] = (byte)((val >> 24) & 0x000000FF);
		b[offset + 1] = (byte)((val >> 16) & 0x000000FF);
		b[offset + 2] = (byte)((val >> 8) & 0x000000FF);
		b[offset + 3] = (byte)(val & 0x000000FF);
	}

	
}
