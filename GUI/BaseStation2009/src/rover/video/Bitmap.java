package rover.video;

import java.awt.Color;
import java.awt.Image;
import java.awt.Transparency;
import java.awt.color.ColorSpace;
import java.awt.image.BufferedImage;
import java.awt.image.ColorModel;
import java.awt.image.ComponentColorModel;
import java.awt.image.DataBuffer;
import java.awt.image.DataBufferByte;
import java.awt.image.Raster;
import java.awt.image.SampleModel;
import java.awt.image.WritableRaster;

public class Bitmap {

	byte[] rgb = null;
	int width = -1;
	int height = -1;
	
	public Bitmap(int width, int height){
		this.width = width;
		this.height = height;
		rgb = new byte[width*height*3];
	}
	
	public Bitmap(byte[] rgb, int width, int height){
		this.width = width;
		this.height = height;
		this.rgb = rgb;
	}
	
	public BufferedImage getImage(){

        ColorSpace cs = ColorSpace.getInstance(ColorSpace.CS_sRGB);
        int[] nBits = {8, 8, 8};
        ColorModel cm = new ComponentColorModel(cs, nBits, false, false, Transparency.OPAQUE, DataBuffer.TYPE_BYTE);
        DataBufferByte db = new DataBufferByte(rgb, 3*width*height);
        
        SampleModel sm = cm.createCompatibleSampleModel(width, height);
        WritableRaster raster = Raster.createWritableRaster(sm, db, null);
        
        //WritableRaster raster = Raster.createInterleavedRaster(db, width, height, 3*width, 3, new int[]{2,1,0}, null);
        BufferedImage bm = new BufferedImage(cm, raster, false, null);
       
//		BufferedImage bm = new BufferedImage(width, height, BufferedImage.TYPE_3BYTE_BGR);
//		for(int i = 0;i<height;i++){
//			for(int j = 0;j<width;j++){
//				int color = 0;
//				int b = rgb[3*(width*i+j)+0];
//				int g = (rgb[3*(width*i+j)+1] << 8) & 0xFF00;
//				int r = (rgb[3*(width*i+j)+2] << 16) & 0xFF0000;
//				//color |= 0xF0 << 24;
//				bm.setRGB(j, i, r+g+b);
//			}
//		}
		
		
		return bm;
	}
	
	
}
