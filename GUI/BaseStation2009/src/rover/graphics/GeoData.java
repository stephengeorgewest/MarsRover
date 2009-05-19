package rover.graphics;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileReader;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.FloatBuffer;
import java.nio.channels.FileChannel;

public class GeoData {

	private static final double m_nm = 1852f;
	private static final double nm_degree = 60;
	private static final double earth_radius = 6378137;
	//private static final double earth_radius = .766*6378137;
	private static final double m_degree_latitude = 111132;
	public static final double adj_latitude = 40;
	
	
	public static double meters2Latitude(double m){
		return m/(m_degree_latitude);
	}
	
	public static double meters2Longitude(double m, double latitude){
		//System.out.println("m = " + m + " lat = " + latitude);
		double radius = Math.cos(latitude*Math.PI/180)*earth_radius;
		double m_degree_longitude = radius*(Math.PI/180);
		//System.out.println("radius = " + radius);
		//System.out.println("m_degree_longitude = " + m_degree_longitude);
		return m/m_degree_longitude;
	}
	
	public static double latitude2meters(double latitude){
		return latitude*m_degree_latitude;
	}
	
	public static double longitude2meters(double longitude, double latitude){
		//System.out.println("longitude = " + longitude + " lat = " + latitude);
		double radius = Math.cos(latitude*Math.PI/180)*earth_radius;
		double m_degree_longitude = radius*(Math.PI/180);
		//System.out.println("radius = " + radius);
		//System.out.println("m_degree_longitude = " + m_degree_longitude);
//		System.out.println("longigtude = " + longitude);
//		System.out.println("lon in meters = " + m_degree_longitude*longitude);
		return m_degree_longitude*longitude;
	}
	
	
	double westb;
	double eastb;
	double northb;
	double southb;
	
	double cell_size;
	int ncols;
	int nrows;
	
	int total;
	
	//float[] data;
	ByteBuffer bbuf;
	FloatBuffer fbuf;
	
	public GeoData(){
		cell_size = 0.0008983111749910168882500898311175f; //approximately 10 meters, this will be set when geo data is loaded
		ncols = 0;
		nrows = 0;
		westb = 0;
		eastb = 0;
		northb = 0;
		southb = 0;
		//data = new float[1];
		//data[0] = 0;
		fbuf = null;
		bbuf = null;
		total = 0;
	}
	
	public int load(String filename){
		
		BufferedReader metain;
		try{
			metain = new BufferedReader(new FileReader(filename));
			String line = null;
			while(true){
				line = metain.readLine();
				if(line == null) break;
				if(line.startsWith("ncols")){
					ncols = Integer.parseInt(line.substring(5).trim());
				} else if(line.startsWith("nrows")){
					nrows = Integer.parseInt(line.substring(5).trim());
				} else if(line.startsWith("xllcorner")){
					westb = Double.parseDouble(line.substring(9).trim());
				} else if(line.startsWith("yllcorner")){
					southb = Double.parseDouble(line.substring(9).trim());
				} else if(line.startsWith("cellsize")){
					cell_size = Double.parseDouble(line.substring(8).trim());
				}
			} 
		}catch(Exception e){
			e.printStackTrace();
		}
		
		if(nrows != 0 && ncols != 0 && westb != 0 && southb != 0 && cell_size != 0){
			eastb = ncols*cell_size+westb;
			northb = nrows*cell_size+southb;
			
			System.out.println("Loading usgs data");
			System.out.println("(" + nrows + "," + ncols + ")");
			System.out.println("westb = " + westb);
			System.out.println("eastb = " + eastb);
			System.out.println("southb = " + southb);
			System.out.println("northb = " + northb);
			System.out.println("cell_size = " + cell_size);

			
		} else
			return -1;
		
		//data = new float[nrows*ncols];
		
		String dataname = filename.substring(0, filename.lastIndexOf('.'));
		dataname+=".flt";
		
		System.out.println("Opening data file name=" + dataname);
		
		FileInputStream fis;
		try{
			File file = new File(dataname);
			fis = new FileInputStream(file);
			FileChannel chan = fis.getChannel();
			System.out.println("File size = " + chan.size());
			bbuf = chan.map(FileChannel.MapMode.READ_ONLY, 0, chan.size());
			bbuf.order(java.nio.ByteOrder.LITTLE_ENDIAN);
			fbuf = bbuf.asFloatBuffer();
			total = ncols*nrows;
			chan.close();
			fis.close();
		
		}catch(Exception e){
			e.printStackTrace();
			return -1;
		}
		System.out.println("Done Reading Data");
		
		return 0;
	}
	
	public double getGridSize(){
		return cell_size*nm_degree*m_nm;
	}
	
	public int pointsBetweenLon(double lonmin, double lonmax, double lat){
		//if the geodata hasn't been loaded yet return 0
		if(total == 0) return 0;
		//region completely outside geodata return 0
		if(lonmax < westb) return 0;
		if(lonmin > eastb) return 0;
		if(lat > northb || lat < southb) return 0;
		
		//region overlaps with geodata return geodata resolution
		return (int)((lonmax-lonmin)/cell_size);
	}
	
	public int pointsBetweenLat(double latmin, double latmax, double lon){
		//if the geodata hasn't been loaded yet return 0
		if(total == 0) return 0;
		//region completely outside geodata return 0
		if(latmin > northb) return 0;
		if(latmax < southb) return 0;
		if(lon > eastb || lon < westb) return 0;
		
		//region overlaps with geodata return geodata resolution
		return (int)((latmax-latmin)/cell_size);
	}
	
	public float get(int x, int y){
		int index = y*ncols + x;
		if(y < 0 || y >= nrows || x < 0 || x >= ncols){
			return 0;
		}
//		System.out.println("returning " + fbuf.get(index));
		return fbuf.get(index);
	}
	
	public float get(double lat, double lon){
		
		int x = (int)(ncols*(lon-westb)/(eastb-westb));
		int y = (int)(nrows*(lat-northb)/(southb-northb));
		//System.out.println("x = " + x);
		//System.out.println("y = " + y);
		
		x = Math.min(Math.max(x, 0), ncols);
		y = Math.min(Math.max(y, 0), nrows);
		return get(x, y);
	}
	
	public int getxindex(double lon){
		int x = (int)(nrows*(lon-westb)/(eastb-westb));
		return Math.min(Math.max(x, 0), ncols);
	}
	
	public int getyindex(double lat){
		int y = (int)(nrows*(lat-northb)/(southb-northb));
		return Math.min(Math.max(y, 0), nrows);
	}
}
