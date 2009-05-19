package rover.graphics;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.net.URL;
import java.net.URLConnection;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Queue;

import rover.guistuff.RoverSimulationPanel3D;

public class TexTileDB extends Thread{

	public static HashMap<String, TexTile> Tiles;
	static HashMap<String, Boolean> notavailable;
	static String folder;
	public static boolean FETCH_TILES = false;
	public static int TILE_THROTTLE = 0;
	
	private static Queue<LoadRequest> requestQueue = new LinkedList<LoadRequest>();
	private static ArrayList<TileLoadListener> listenerlist = new ArrayList<TileLoadListener>();
	private static TexTileDB instance;
	
	public boolean sleeping;
	
	public static void addRequest(LoadRequest lr){
		synchronized (requestQueue){
			requestQueue.add(lr);
		}
		
		if(instance == null){
			instance = new TexTileDB();
			instance.setDaemon(true);
			instance.start();
		}else{
			if(instance.sleeping) 
				instance.interrupt();
		}
	}
	
	
	
	public void run(){
		sleeping = false;
		while(true){
			int size = 0;
			synchronized (requestQueue){
				size = requestQueue.size();
			}
			while(size != 0){
				LoadRequest lr;
				synchronized (requestQueue){
					lr = requestQueue.poll();
					size = requestQueue.size();
				}
				TexTile t = Tiles.get(lr.address);
				if(t.preloadTex())
					TileLoaded();
			}
			try{
				sleeping = true;
				Thread.sleep(10000);
			}catch(Exception e){
				//e.printStackTrace();
			}finally{
				sleeping = false;
			}
			
		}
		
		
	}
	
	public static TexTile get(double lat, double lon, int LOD){
		if(LOD < 1) return null;
		String address = GetQuadtreeAddress(lat, lon);
		
		return get(address, LOD);
	}
	
	public static TexTile get(String address){
		return get(address, address.length());
	}
	
	public static TexTile get(String full_address, int LOD){
		
		//System.out.println("get called on " + full_address + " LOD = " + LOD);
		
		if(full_address == null) return null;
		if(LOD < 1 || LOD > full_address.length()) return null;
		//LOD = Math.min(address.length(), LOD);
		String address = full_address.substring(0, LOD);
		
		//find deepest currently renderable tile, dt
		TexTile dt = Tiles.get(address);
		while(dt == null || !(dt.loaded||dt.preloaded)){
			//System.out.println("Checking depth = " + address);
			address = address.substring(0, address.length()-1);
			if(address.length() == 0) return null;
			dt = Tiles.get(address);
		}
		
		//System.out.println("Deepest node = " + address);

		//If we can't return the requested tile let's 
		//descend into this part of the tree so that next time
		//we'll be a little closer to being able to render the tile
		if(!dt.isLeaf && address.length() != LOD) { //preload child tiles;
			TexTile t = CreateTileFromAddress(full_address.substring(0, address.length()+1));
			if(t.fileExists()){ //if we alread have the file in the cache preload it
				LoadRequest lr = new LoadRequest();
				lr.address = t.address;
				addRequest(lr);
			//}else if(!notavailable.containsKey(t.address)){
			}else if(!dt.parent.isLeaf){ //otherwise put in a request to have it downloaded
				FetchRequest fr = new FetchRequest();
				fr.address = t.address;
				//System.out.println("a: DL request for " + fr.address);
				AsyncTileFetcher.addRequest(fr);
			}
		}
		
		
		//if we have found the requested tile
		if(address.length() == LOD){//if dt.LOD == LOD return dt
			//if it's preloaded but not loaded, go ahead and load it
			if(!dt.loaded && dt.preloaded) dt.loadTex();
			
			//if it's not preloaded but the file exists in the cache, put in a preload request
//			if(!dt.preloaded && dt.fileExists()){
//				LoadRequest lr = new LoadRequest();
//				lr.address = address;
//				addRequest(lr);
//			//}else if(!notavailable.containsKey(t.address)){
//			//if the file doesn't exist in the cache put in a download request
//			}else if(! dt.fileExists() && dt.parent != null && !dt.parent.isLeaf){
//				FetchRequest fr = new FetchRequest();
//				fr.address = dt.address;
//				System.out.println("b: DL request for " + fr.address);
//				AsyncTileFetcher.addRequest(fr);
//			}
		} //else if(address.length() == LOD-1) dt = Tiles.get(address);
		else dt = null;
		
		return dt;
	}
	
	public static void FetchRequestComplete(String address, boolean worked){
		System.out.println("Request Complete " + address);
		//if(!worked) notavailable.put(address, true);
		TexTile t = Tiles.get(address);
		if(!worked){
			t.parent.isLeaf = true;
		}
		else {
			if(t.preloadTex())
				TileLoaded();
		}
	}
	
	public static TexTile get(double lat, double lon){
		return get(lat, lon, 20);
	}
	

	
	public static void Initialize(String folder_path){
		Tiles = new HashMap<String, TexTile>();
		notavailable = new HashMap<String, Boolean>();
		folder = folder_path;
		
		//create the root tile
		CreateTileFromAddress("t");
		
		
		System.out.println("TexTile DB Initialized at " + folder);
	}
	
	
	
	public static double MercatorToNormal(double y)
	{
		y = -y * Math.PI / 180; // convert to radians
		y = Math.sin(y);
		y = (1+y)/(1-y);
		y = 0.5 * Math.log(y);
		y *= 1.0 / (2 * Math.PI); // scale factor from radians to normalized
		y += 0.5; // and make y range from 0 - 1
		return y;
	}
	 
	public static double NormalToMercator(double y)
	{
		y -= 0.5;
		y *= 2 * Math.PI;
		y = Math.exp(y * 2);
		y = (y-1)/(y+1);
		y = Math.asin(y);
		y = y * -180/Math.PI;
		return y;
	}
	 
	public static TexTile CreateTileFromAddress(String address)
	{
		if(Tiles.containsKey(address)) return Tiles.get(address);
		// get normalized coordinate first
		double x = 0.0;
		double y = 0.0;
		double scale = 1.0;
		for(int i = 1;i<address.length();i++)
		{
			scale *= 0.5;
			char c = address.charAt(i); // look at next character
			if (c == 'r' || c == 's')
			{
				x += scale;
			}
			if (c == 't' || c == 's')
//			if (c == 'q' || c == 'r')
			{
				y += scale;
			}
		}
		
		TexTile t = new TexTile(null);
		t.lonmin = (x - 0.5) * 360;
		t.lon = (x + scale * 0.5 - 0.5) * 360;
		t.lonmax = (x + scale - 0.5) * 360;
		t.latmin = NormalToMercator(y + scale);
		t.lat = NormalToMercator(y + scale * 0.5);
		t.latmax = NormalToMercator(y);
//		t.latmin = (y-.5)*180;
//		t.lat = (y + .5 * scale - .5)*180;
//		t.latmax = (y + scale - 0.5) * 180;
		t.address = address;
		
		if(address.length() == 1){ //if this is the world map load the tile right away
			fetchTile(address);
			t.preloadTex();
		}else{
			String paddress = address.substring(0, address.length()-1);
			TexTile parent = Tiles.get(paddress);
			//if(parent == null) parent = CreateTileFromAddress(paddress);
			t.parent = parent; 
		}
		
		Tiles.put(address, t);

		return t;
	}
	 
	public static String GetQuadtreeAddress(double lat, double lon)
	{
		if(lat > 90 || lat < -90) return null;
		if(lon > 180 || lon < -180) return null;
		//System.out.println("creating address for lat = " + lat + " lon = " + lon);
		
		// now convert to normalized square coordinates
		// use standard equations to map into mercator projection
		double x = (180.0 + lon) / 360.0;
		double y = MercatorToNormal(lat);
		String quad = "t";// google addresses start with t
		char[] lookup = {'q', 'r', 't', 's'}; // tl tr bl br
		for (int digits = 0; digits < 23; digits++)
		{
			// make sure we only look at fractional part
			x -= Math.floor(x);
			y -= Math.floor(y);
			
			quad = quad + lookup[(x>=0.5?1:0) + (y>=0.5?2:0)];
			// now descend into that square
			x *= 2;
			y *= 2;
		}
		//System.out.println("Returning " + quad);
		return quad;
	}

	
	public static boolean fetchTile(String addrs){
		
		
		File file = new File(getFileName(addrs));
		if(file.exists()){
			//System.out.println(file.getAbsolutePath() + "File exists");
			return true;
		}
		
		
		String tile_address = "http://kh.google.com/kh?v=3&t=" + addrs;
		try
	    {
			System.out.println("fetching url = " + tile_address);
			URL url = new URL(tile_address);
			URLConnection urlConnection = url.openConnection();
			urlConnection.connect();
			InputStream input = url.openStream();
			file.createNewFile();
	        FileOutputStream fos = new FileOutputStream(file);
	        int read = input.read();
	        while(read != -1){
	        	fos.write(read);
	        	read = input.read();
	        }
	        fos.close();
	        input.close();
	        return true;

	    } catch(FileNotFoundException fnfe){
	    	System.out.println("Does Not Exist");
	    	//fnfe.printStackTrace();
	    }catch ( Exception e ){
	    	System.out.println("filename = " + file.getAbsolutePath());
	    	System.out.println("url = " + tile_address);
	    	e.printStackTrace();
	    }
		return false;
	}
	
//	public static void fetchAllTiles(double lat, double lon){
//		
//		//System.out.println("Checking for tiles for " + lat + " " + lon);
//		String addrs = GetQuadtreeAddress(lat, lon);
//		for(int i = 1;i<addrs.length();i++){
//			String path = addrs.substring(0, addrs.length()-i);
//			//System.out.println("getting " + path);
//			//System.out.println(path + " available? " + !notavailable.containsKey(path));
//			if(notavailable.containsKey(path)){
//				continue;
//			}
//			boolean available = fetchTile(path);
//			if(!available){
//				//System.out.println("adding " + path +" to notavilable list");
//				notavailable.put(path, true);
//			}
//		}
//
//	}



	public static String getFileName(String address) {
		return folder + "\\" + address + ".jpg";
	}
	
	public static void recomputeTiles(){
		Collection<TexTile> tiles = Tiles.values();
		for(TexTile t : tiles){
			t.recompute = true;
		}
	}



	public static void addTileLoadListener(TileLoadListener l) {
		listenerlist.add(l);
	}
	private static void TileLoaded(){
		for(TileLoadListener l : listenerlist){
			l.TileLoaded();
		}
	}
	
}