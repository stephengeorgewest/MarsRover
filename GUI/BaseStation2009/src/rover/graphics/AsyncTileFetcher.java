package rover.graphics;

import java.util.LinkedList;
import java.util.Queue;

public class AsyncTileFetcher extends Thread{

	private static Queue<FetchRequest> requestQueue = new LinkedList<FetchRequest>();
	
	private static AsyncTileFetcher instance;
	
	public static void addRequest(FetchRequest fr){
		synchronized (requestQueue){
			requestQueue.add(fr);
		}
		
		if(instance == null){
			instance = new AsyncTileFetcher();
			instance.setDaemon(true);
			instance.start();
		}else{
			if(instance.sleeping) 
				instance.interrupt();
		}
	}
	
	public boolean sleeping;
	
	public void run(){
		sleeping = false;
		while(true){
			int size = 0;
			synchronized (requestQueue){
				size = requestQueue.size();
			}
			while(size != 0){
				FetchRequest fr;
				synchronized (requestQueue){
					fr = requestQueue.poll();
					size = requestQueue.size();
				}
				boolean fetchable = true;
				if(fr.address.length() > 1){
					String paddress = fr.address.substring(0, fr.address.length()-1);
					TexTile t = TexTileDB.Tiles.get(paddress);
					fetchable = !t.isLeaf;
				}
				
				if(TexTileDB.FETCH_TILES){
					boolean worked = TexTileDB.fetchTile(fr.address);
					TexTileDB.FetchRequestComplete(fr.address, worked);
				}
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
	
}
