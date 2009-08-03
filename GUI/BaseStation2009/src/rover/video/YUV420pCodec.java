package rover.video;
import java.awt.Dimension;
import javax.media.Buffer;
import javax.media.Codec;
import javax.media.Format;
import javax.media.ResourceUnavailableException;
import javax.media.format.RGBFormat;


/*
 * Name: YUV420pCodec
 * Author: Travis L Brown
 * Description: 
 * 
 * Provides a JMF compliant codec for decoding YUV420p (aka YV12) This class has never
 * been tested and is included in the project only because I had already implemented the 
 * basic YUV to RGB conversion for another project and YUV420 is a popular format for 
 * webcams which we may need to use someday.
 * 
 * 
 */

public class YUV420pCodec implements Codec{

	RGBFormat output_format = null;
	
	Format[] supported_output_formats;
	
	public YUV420pCodec(){
		supported_output_formats = new Format[1];
		byte[] barray = new byte[1];
		supported_output_formats[0] = new RGBFormat(new Dimension(1,1), 0, barray.getClass(), 0xFF<<24, 0xFF<<16, 0xFF, 0, 0);
	}
	
	@Override
	public Format[] getSupportedInputFormats() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Format[] getSupportedOutputFormats(Format input) {
		return supported_output_formats;
	}

	@Override
	public int process(Buffer input, Buffer output) {
		
		byte[] yuv = (byte[]) input.getData();
		byte[] rgb = (byte[]) output.getData();
		
		if(!output.getFormat().matches(output_format)) 
			return javax.media.Codec.BUFFER_PROCESSED_FAILED;
		
		int width = output_format.getSize().width;
		int height = output_format.getSize().height;
		if(yuv.length < 3*width*height/2)
			return javax.media.Codec.BUFFER_PROCESSED_FAILED;
		if(rgb.length < 3*width*height)
			output.setData(new byte[3*width*height]);
		
		try{ convert_YUV420P_RGB(yuv, rgb, height, width);}
		catch(Exception e){
			e.printStackTrace();
			return javax.media.Codec.BUFFER_PROCESSED_FAILED;
		}
		return javax.media.Codec.BUFFER_PROCESSED_OK;
	}

	@Override
	public Format setInputFormat(Format format) {
		if(format.getEncoding() == "I420")
			return format;
		else return null;
	}

	@Override
	public Format setOutputFormat(Format format) {
		if(format.getClass() != RGBFormat.class){
			return null;
		} else{
			RGBFormat f = (RGBFormat)(format);
			byte[] type = new byte[0];
			if(f.getDataType() != type.getClass()){
				return null;
			}
			output_format = f;
			return output_format;
		}
	}

	@Override
	public void close() {
		// TODO Auto-generated method stub
		
	}

	@Override
	public String getName() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void open() throws ResourceUnavailableException {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void reset() {
		// TODO Auto-generated method stub
		
	}

	@Override
	public Object getControl(String controlType) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Object[] getControls() {
		// TODO Auto-generated method stub
		return null;
	}

	
	void convert_YUV420P_RGB(byte[] yuv, byte[] rgb, int rows, int cols)
    {
        int rvScale = 91881;
        int guScale = -22553;
        int gvScale = -46801;
        int buScale = 116129;
        int yScale = 65536;

        
        int Y_index = 0;
        int U_index = rows*cols;
        int V_index = U_index + rows * cols/4;
        int RGB_index = 0;

        for (int r = 0; r <= rows - 2; r += 2)
        {
            for (int c = 0; c <= cols - 2; c += 2)
            {
                //gather yuv information for 4 pixel group
                int y00, y01, y10, y11, u, v;
                y00 = yuv[Y_index] & 0xFF;
                y01 = yuv[Y_index+1] & 0xFF;
                y10 = yuv[Y_index + cols] & 0xFF;
                y11 = yuv[Y_index + cols + 1] & 0xFF;
                u = (yuv[U_index] & 0xFF) - 128;
                v = (yuv[V_index] & 0xFF) - 128;

//                if(c == 20 && r == 40){
//                	System.out.println("y, u, v" + y00 + " " + y01 + " " + y10 + " " + y11 + " " + u + " " + v);
//                }
                
                //calculate rgb base components
                int R, G, B, x;
                G = guScale * u + gvScale * v;
                R = rvScale * v;
                B = buScale * u;
                /*
                rgb[RGB_index] = (byte) y00;
                rgb[RGB_index + 1] = (byte)y00;
                rgb[RGB_index + 2] = (byte)y00;
                rgb[RGB_index + 3] = (byte)y01;
                rgb[RGB_index + 4] = (byte)y01;
                rgb[RGB_index + 5] = (byte)y01;
                rgb[RGB_index + cols * 3] = (byte)y10;
                rgb[RGB_index + 1 + cols * 3] = (byte)y10;
                rgb[RGB_index + 2 + cols * 3] = (byte)y10;
                rgb[RGB_index + 3 + cols * 3] = (byte)y11;
                rgb[RGB_index + 4 + cols * 3] = (byte)y11;
                rgb[RGB_index + 5 + cols * 3] = (byte)y11;
                */

                
                //top left pixel
                x = R + y00 * yScale;
//                if(c == 20) System.out.print(x + " ");
                rgb[RGB_index] = (byte)((x) > 0xffffff ? 0xff : ((x) <= 0xffff ? 0 : ((x) >> 16)));
                x = G + y00 * yScale;
//                if(c == 20) System.out.print(x + " ");
                rgb[RGB_index + 1] = (byte)((x) > 0xffffff ? 0xff : ((x) <= 0xffff ? 0 : ((x) >> 16)));
                x = B + y00 * yScale;
//                if(c == 20) System.out.print(x + " ");
                rgb[RGB_index + 2] = (byte)((x) > 0xffffff ? 0xff : ((x) <= 0xffff ? 0 : ((x) >> 16)));

//                if(c == 20) System.out.println("{" + (rgb[RGB_index] & 0xFF) + " " + (rgb[RGB_index+1] & 0xFF)+ " " + (rgb[RGB_index+2] & 0xFF) + "}");
                
                //top right pixel
                x = R + y01 * yScale;
                rgb[RGB_index + 3] = (byte)((x) > 0xffffff ? 0xff : ((x) <= 0xffff ? 0 : ((x) >> 16)));
                x = G + y01 * yScale;
                rgb[RGB_index + 4] = (byte)((x) > 0xffffff ? 0xff : ((x) <= 0xffff ? 0 : ((x) >> 16)));
                x = B + y01 * yScale;
                rgb[RGB_index + 5] = (byte)((x) > 0xffffff ? 0xff : ((x) <= 0xffff ? 0 : ((x) >> 16)));

                //bottom left pixel
                x = R + y10 * yScale;
                rgb[RGB_index + cols*3] = (byte)((x) > 0xffffff ? 0xff : ((x) <= 0xffff ? 0 : ((x) >> 16)));
                x = G + y10 * yScale;
                rgb[RGB_index + 1 + cols*3] = (byte)((x) > 0xffffff ? 0xff : ((x) <= 0xffff ? 0 : ((x) >> 16)));
                x = B + y10 * yScale;
                rgb[RGB_index + 2 + cols*3] = (byte)((x) > 0xffffff ? 0xff : ((x) <= 0xffff ? 0 : ((x) >> 16)));

                //bottom right pixel
                x = R + y11 * yScale;
                rgb[RGB_index + 3 + cols*3] = (byte)((x) > 0xffffff ? 0xff : ((x) <= 0xffff ? 0 : ((x) >> 16)));
                x = G + y11 * yScale;
                rgb[RGB_index + 4 + cols*3] = (byte)((x) > 0xffffff ? 0xff : ((x) <= 0xffff ? 0 : ((x) >> 16)));
                x = B + y11 * yScale;
                rgb[RGB_index + 5 + cols*3] = (byte)((x) > 0xffffff ? 0xff : ((x) <= 0xffff ? 0 : ((x) >> 16)));
                
                

                Y_index += 2;
                U_index++;
                V_index++;

                RGB_index += 6;

            }
            Y_index += cols;
            RGB_index += 3 * cols;
        }

    }
	
}
