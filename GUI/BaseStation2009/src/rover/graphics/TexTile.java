package rover.graphics;

import java.awt.Color;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.net.DatagramPacket;
import java.nio.ByteBuffer;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;
import java.nio.ShortBuffer;

import javax.imageio.ImageIO;
import javax.media.opengl.GL;
import javax.media.opengl.glu.GLU;

import com.sun.opengl.util.BufferUtil;
import com.sun.opengl.util.texture.Texture;
import com.sun.opengl.util.texture.TextureCoords;
import com.sun.opengl.util.texture.TextureIO;

public class TexTile {

	public double latmin;
	public double latmax;
	public double lonmin;
	public double lonmax;
	public double lat;
	public double lon;
	
	
	public String address; 
	public Texture texture;
	public BufferedImage teximage;
	
	public boolean recompute;
	public boolean loaded = false;
	public boolean preloaded = false;
	
	FloatBuffer geoBuffer;
	FloatBuffer texBuffer;
	IntBuffer[] indicies;
	
	int xres;
	int yres;
	
	TexTile parent;
	public boolean isLeaf = false;
	
	public int garbage_counter;
	
	public static boolean drawTexture = true;
	public static boolean drawLines = false;
	public static Color tileColor = new Color(182,114,12);
	//public static Color tileColor = new Color(243,172,75);
	
	
	
	public TexTile(TexTile parent){
		this.parent = parent;
		latmin = 0;
		latmax = 0;
		lonmin = 0;
		lonmax = 0;
		texture = null;
		teximage = null;
		address = null;
		recompute = true;
		garbage_counter = TexTileDB.GARBAGE_RENDER_COUNT;
	}

	public boolean fileExists(){
		String filename = TexTileDB.getFileName(address);
		File f = new File(filename);
		return f.exists();
	}
	
	public boolean preloadTex(){
		if(teximage != null) return true;
		String filename = TexTileDB.getFileName(address);
		System.out.println("preloading image, " + address);
		try{
			teximage = ImageIO.read(new File(filename));
			teximage.getWidth();
		}catch (Exception e){
			e.printStackTrace();
		}
		if(teximage == null) return false;
		else{
			garbage_counter = TexTileDB.GARBAGE_RENDER_COUNT;
			preloaded = true;
			return true;
		}
	}
	
	public boolean loadTex(){
		if(address == null || address.length() < 1){
			loaded = false;
			return false;
		}
		
		if(teximage == null){
			String filename = TexTileDB.getFileName(address);
			File f = new File(filename);
			if(f.exists()){
				System.out.println("loading tile, address=" + address);
				texture = load(f);
			}
		} else { //image data has been preloaded
			System.out.println("Texturizing preloaded image, address=" + address);
			texture = TextureIO.newTexture(teximage, false);
			teximage = null;
		}
		if(texture == null){
			return false;
		}
		else{
			loaded = true;
			recompute = true;
			garbage_counter = TexTileDB.GARBAGE_RENDER_COUNT;
			return true;
		}
	}
	
	public static Texture load(File f){
		Texture text = null;
		try{
			text = TextureIO.newTexture(f, false);

			text.setTexParameteri(GL.GL_TEXTURE_MAG_FILTER, GL.GL_NEAREST);
			text.setTexParameteri(GL.GL_TEXTURE_MIN_FILTER, GL.GL_NEAREST);
			
		}catch(Exception e){
			System.out.println("Error loading texture " + f.getAbsolutePath());
			e.printStackTrace();
		}
		return text;
	}
	
	public void Render(GL gl, GLU glu, GeoData geo) {
		
		garbage_counter = TexTileDB.GARBAGE_RENDER_COUNT;
		
		//System.out.println("Render( " + address  + ") ");
		//System.out.println("lat " + latmin + " to " + latmax);
		//System.out.println("lon " + lonmin + " to " + lonmax);
		
		if(texture != null){
			//System.out.println("Binding Texture");
			texture.enable();
			texture.bind();
		} else {
			return;
		}
		
		
		
		if(geoBuffer == null || recompute == true){
			//System.out.println("Initiating Buffers");
			int pblat = geo.pointsBetweenLat(latmin, latmax, lon);
			int pblon = geo.pointsBetweenLon(lonmin, lonmax, lat);
			//yres = Math.max(2, Math.min(pblat, 256));
			//xres = Math.max(2, Math.min(pblon, 256));
			
			if(pblat > 2)
				yres = 25;
			else
				yres = 2;
			if(pblon > 2)
				xres = 25;
			else
				xres = 2;
			
			//System.out.println("xres = " + xres + " yres = " + yres);
			
			geoBuffer = BufferUtil.newFloatBuffer(3*(xres)*(yres));
			texBuffer = BufferUtil.newFloatBuffer(2*(xres)*(yres));
            indicies = new IntBuffer[yres];
			
            
            for(int y = 0;y<yres-1;y++){
            	indicies[y] = BufferUtil.newIntBuffer((2*xres));
            	int index = 0;
    			for(int x = 0;x<xres;x++){
    				indicies[y].put(index++, y*xres + x);
    				indicies[y].put(index++, (y+1)*xres + x);
    			}
            }
//            for(int i = 0;i<index;i++){
//            	System.out.println(indicies.get(i));
//            }
            
            TextureCoords tc = texture.getImageTexCoords();
            int index = 0;
            for(float y = 0;y<yres;y++){
            	double texy =y/(yres-1);
            	double vlat = latmin + (yres-y-1)*(latmax-latmin)/(yres-1);
            	double ry = GeoData.latitude2meters(vlat-lat);
    			for(float x = 0;x<xres;x++){
    				double texx = x/(xres-1);
    				double vlon = lonmin + x*(lonmax-lonmin)/(xres-1);
    				double rx = GeoData.longitude2meters(vlon-lon, GeoData.adj_latitude);
    				double h = geo.get(vlat, vlon);
    				geoBuffer.put(3*index,(float)rx);
    				geoBuffer.put(3*index+1,(float)ry);
    				geoBuffer.put(3*index+2,(float)h);
    				
    				//System.out.println("Adding vertex " + rx + " " + ry + " " + h);
    				
    				texBuffer.put(2*index, (float)texx);
    				texBuffer.put(2*index+1, (float)texy);
    				//System.out.println("texture vertex " + texx + " " + texy);
    				index++;
    			}
            }
            recompute = false;
		}

		//gl.glTranslated(GeoData.latitude2meters(lat), GeoData.longitude2meters(lon, 0), 0);
		
		
		gl.glEnableClientState(GL.GL_VERTEX_ARRAY);
		gl.glVertexPointer(3, GL.GL_FLOAT, 0, geoBuffer);
		
		if(drawTexture){
			gl.glColor4ub((byte)255, (byte)255, (byte)255, (byte)255);
			gl.glEnableClientState(GL.GL_TEXTURE_COORD_ARRAY);
			gl.glEnable(GL.GL_TEXTURE_2D);
			gl.glTexCoordPointer(2, GL.GL_FLOAT, 0, texBuffer);
		}else{
			gl.glColor4ub((byte)tileColor.getRed(), (byte)tileColor.getGreen(), (byte)tileColor.getBlue(), (byte)255);
			gl.glDisableClientState(GL.GL_TEXTURE_COORD_ARRAY);
			gl.glDisable(GL.GL_TEXTURE_2D);
		}
		
		gl.glPolygonMode(GL.GL_FRONT_AND_BACK, GL.GL_FILL);
		for(int i = 0;i<yres-1;i++)
			gl.glDrawElements(GL.GL_QUAD_STRIP, indicies[i].capacity(), GL.GL_UNSIGNED_INT, indicies[i]);
		
		//System.out.println("Rendering");
		
		
		
	
		if(drawLines){
			gl.glPolygonMode(GL.GL_FRONT_AND_BACK, GL.GL_LINE);
			gl.glDisableClientState(GL.GL_TEXTURE_COORD_ARRAY);
			gl.glDisable(GL.GL_TEXTURE_2D);
			gl.glLineWidth(2);
			
			gl.glColor4ub((byte)0, (byte)255, (byte)0, (byte) 255);
			for(int i = 0;i<yres-1;i++)
				gl.glDrawElements(GL.GL_QUAD_STRIP, indicies[i].capacity(), GL.GL_UNSIGNED_INT, indicies[i]);
			
		}
		
//		gl.glPolygonMode(GL.GL_FRONT_AND_BACK, GL.GL_LINE);
//		gl.glLineWidth(2);
//		gl.glColor3ub((byte)0, (byte)255, (byte)0);
//		gl.glDisableClientState(GL.GL_TEXTURE_COORD_ARRAY);
//		gl.glDrawElements(GL.GL_QUAD_STRIP, indicies.capacity(), GL.GL_UNSIGNED_INT, indicies);
//		
		if(texture != null) texture.disable();
	}
	
	
}
