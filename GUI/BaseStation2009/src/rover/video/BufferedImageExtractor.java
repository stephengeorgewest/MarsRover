package rover.video;

import java.awt.image.BufferedImage;
import javax.media.Buffer;
import javax.media.format.RGBFormat;


/*
 * Name: BufferedImageExtractor
 * Author: Travis L Brown
 * Description: 
 * Provides a conversion routine to convert data in a RGB formated JMF buffer into 
 * a Java BufferedImage
 * 
 */
public class BufferedImageExtractor {

	public static BufferedImage convert(Buffer b){
		RGBFormat f = (RGBFormat) b.getFormat();
		Bitmap bmp = new Bitmap((byte[])b.getData(), f.getSize().height, f.getSize().width);
		return bmp.getImage();
	}
	
}
