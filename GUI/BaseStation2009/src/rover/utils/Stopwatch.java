package rover.utils;

public class Stopwatch {

	long start;
	long stop;
	
	long accumulated;
	
	boolean running;
	
	public Stopwatch(){
		accumulated = 0;
		start = 0;
		stop = 0;
		running = false;
	}
	
	public void Start(){
		start = System.nanoTime();
		running = true;
	}
	
	public void Stop(){
		stop = System.nanoTime();
		running = false;
		accumulated += (stop-start);
	}
	
	public void Reset(){
		accumulated = 0;
	}
	
	public long getElapsedNanos(){
		if(running)	return accumulated + (System.nanoTime()-start);
		else return accumulated;
	}
	
	public double getElapsedMicros(){
		return getElapsedNanos()/1000.0;
	}
	
	public double getElapsedMillis(){
		return getElapsedNanos()/1000000.0;
	}
	
	public double getElapsedSeconds(){
		return getElapsedNanos()/1000000000.0;
	}
	

	
	
	
}
