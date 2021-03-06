package rover.utils;


public class Waypoint {

	public String name;
	public double lat, lon, z;
	public byte[] color = {(byte)255,0,0};
	
	public Waypoint(String name, double lat, double lon, double z){
		this.name = name;
		this.lon = lon;
		this.lat = lat;
		this.z = z;
		
	}
	
	public Waypoint(GpsData data) {
		lat = data.Latitude;
		lon = data.Longitude;
		z = data.Elevation;
	}

	public String toString(){
		return name + " " + GpsData.formatCoordinates(lat, lon);
	}
	
}
