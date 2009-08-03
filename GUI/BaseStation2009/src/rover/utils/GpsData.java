package rover.utils;

import java.io.File;
import java.text.DecimalFormat;
import org.geotools.referencing.operation.transform.NADCONTransform;

public class GpsData{

	public double Longitude;
    public double Latitude;
    public double Elevation;
    public double Speed;
    public double Heading;
    public double Compass_Heading;
    public byte NumberOfSatellites;
    public boolean LockAcquired;
    public boolean goodData;
    public char lathemi;
    public char lonhemi;
    public long time_stamp;
	
    public GpsData()
    {
        Longitude = Double.NaN;
        Latitude = Double.NaN;
        Elevation = Double.NaN;
        Speed = Double.NaN;
        Heading = Double.NaN;
        Compass_Heading = Double.NaN;
        NumberOfSatellites = 0;
        LockAcquired = false;
        goodData = false;
        lathemi = ' ';
        lonhemi = ' ';
        time_stamp = -1;
    }
    
    @Override
    public GpsData clone(){
    	GpsData data = new GpsData();
    	data.Latitude = Latitude;
    	data.Longitude = Longitude;
    	data.Elevation = Elevation;
    	data.Heading = Heading;
    	data.Compass_Heading = Compass_Heading;
    	data.goodData = goodData;
    	data.lathemi = lathemi;
    	data.lonhemi = lonhemi;
    	data.LockAcquired = LockAcquired;
    	data.NumberOfSatellites = NumberOfSatellites;
    	data.Speed = Speed;
    	data.time_stamp = time_stamp;
    	return data;
    }

    @Override
    public String toString()
    {
        return "GpsData";
    }
    
    public boolean MergeData(String gpsString){
    	return GpsData.MergeGpsString(gpsString, this);
    }
    

//    public double getRealLat()
//    {
//        if (Latitude == Double.NaN) return 0;
//        String lat = df.format(Latitude);
//        int ilat = lat.indexOf('.');
//        double latitude = 0;
//        double minutes = 0;
//        try
//        {
//            latitude = Double.parseDouble(lat.substring(0, ilat - 2));
//            minutes = Double.parseDouble(lat.substring(ilat - 2, lat.length() - (ilat - 2)));
//        }
//        catch (Exception e) 
//        {
//            return 0;
//        }
//        latitude += minutes / 60.0;
//        if (lathemi == 'W') latitude *= -1;
//
//        return latitude;
//    }
//    public double getRealLong()
//    {
//        if (Longitude == Double.NaN) return Longitude;
//        String lon = df.format(Longitude);
//        int ilon = lon.indexOf('.');
//
//        double longitude = 0;
//        double minutes = 0;
//
//        try
//        {
//            longitude = Double.parseDouble(lon.substring(0, ilon - 2));
//            minutes = Double.parseDouble(lon.substring(ilon - 2, lon.length() - (ilon - 2)));
//        }
//        catch (Exception e)
//        {
//            return 0;
//        }
//        longitude += minutes / 60.0;
//        if (lonhemi == 'W') longitude *= -1;
//
//        return longitude;
//    }



    public static String getFormatedPos(GpsData data)
    {
        if (data.Latitude == Double.NaN || data.Longitude == Double.NaN) return "Invalid";
        return getFormatedPosWGS84(data.Latitude, data.Longitude);
    }
    
    public static String getFormatedPosWGS84(double lat, double lon){
    	if(format_mode == MODE_DMS){
    		int lat_deg = (int)(lat);
    		int lat_min = (int)(60*(lat-lat_deg));
    		float lat_sec = (float)(3600*(lat-lat_deg-lat_min/60.0));
    		
    		int lon_deg = (int)(lon);
    		int lon_min = (int)(60*(lon-lon_deg));
    		float lon_sec = (float)(3600*(lon-lon_deg-lon_min/60.0));
    		
    		return lat_deg + " " + lat_min + "' " + sdf.format(lat_sec) + "\" " 
    				+ lon_deg + " " + lon_min + "' " + sdf.format(lon_sec) + "\""; 
    		
    	} else if(format_mode == MODE_DD){
    		return ldf.format(lat) + " " + ldf.format(lon);
    	} else if(format_mode == MODE_DDM){
    		int lat_deg = (int)(lat);
    		double lat_min = 60*(lat-lat_deg);

    		int lon_deg = (int)(lon);
    		double lon_min = 60*(lon-lon_deg);

    		return lat_deg + " " + df.format(lat_min) + "' " 
    				+ lon_deg + " " + df.format(lon_min) + "' "; 
    		
    	} else if(format_mode == MODE_UTM){
    		return cc.latLon2UTM(lat, lon);
    	} else  {
    		return "Unknown format mode ";
    	}
    	
    	
    	
    }
    
    public static double distBetween(GpsData d1, GpsData d2){
    	
    	return 0;
    }

    private static void initNADCON(){
    	nct = null;
    	try{
    		File las = new File ("./geocache/GRIDS/conus.las");
    		File los = new File ("./geocache/GRIDS/conus.los");
    		nct = new NADCONTransform(las.getAbsolutePath(), los.getAbsolutePath());
    		//nct.
    	}catch (Exception e){
    		e.printStackTrace();
    	}
    }
    
//    public static String formatLatLon(GpsData data){
//    	return formatLat(data) + " " + formatLon(data);
//    }
    
    public static String formatCoordinates(double lat, double lon){
    	if(datum_mode == GpsData.NAD27){ //conversion required
    		if(nct == null) initNADCON();
    		try{
				double[] src = new double[2];
				src[0] = lon;
				src[1] = lat;
				nct.inverseTransform(src, 0, src, 0, 1);
				lon = src[0];
				lat = src[1];
    		}catch (Exception e){
    			e.printStackTrace();
    		}
    	}
    	
    	return getFormatedPosWGS84(lat, lon);
    }
    

	public static GpsData parseCoordinates(String text) {
		GpsData d = new GpsData();
		String[] parts = text.split(" ");
		if(format_mode == MODE_DMS){
			if(parts.length == 6){

				int lat_deg = Integer.parseInt(parts[0]);
	    		int lat_min = Integer.parseInt(parts[1].substring(0, parts[1].length()-1));
	    		float lat_sec = Float.parseFloat(parts[2].substring(0, parts[1].length()-2));
	    		
	    		int lon_deg = Integer.parseInt(parts[3]);
	    		int lon_min = Integer.parseInt(parts[4].substring(0, parts[4].length()-1));
	    		float lon_sec = Float.parseFloat(parts[5].substring(0, parts[5].length()-2));
	    		
	    		d.Latitude = lat_deg+lat_min/60.0 + lat_sec/3600.0;
	    		d.Longitude = lon_deg+lon_min/60.0 + lon_sec/3600.0;
			}
    	} else if(format_mode == MODE_DD){
    		d.Latitude = Double.parseDouble(parts[0]);
    		d.Longitude = Double.parseDouble(parts[1]);
    	} else if(format_mode == MODE_DDM){
    		int lat_deg = Integer.parseInt(parts[0]);
    		float lat_min = Float.parseFloat(parts[1].substring(0, parts[1].length()-1));
    		
    		int lon_deg = Integer.parseInt(parts[2]);
    		float lon_min = Float.parseFloat(parts[3].substring(0, parts[3].length()-1));
    		
    		d.Latitude = lat_deg+lat_min/60.0;
    		d.Longitude = lon_deg+lon_min/60.0;
    	} else if(format_mode == MODE_UTM){
    		double[] latlon = cc.utm2LatLon(text);
    		d.Latitude = latlon[0];
    		d.Longitude = latlon[1];
    	} else  {
    	}
		
		if(datum_mode == GpsData.NAD27){ //conversion required
    		if(nct == null) initNADCON();
    		try{
				double[] src = new double[2];
				src[0] = d.Longitude;
				src[1] = d.Latitude;
				nct.transform(src, 0, src, 0, 1);
				d.Longitude = src[0];
				d.Latitude = src[1];
    		}catch (Exception e){
    			e.printStackTrace();
    		}
    	}
		
		return d;
	}

	
    
    
//    public static String formatCoordinates(GpsData data){
//    	
//    	
//    }
//    
//    public static String formatLat(GpsData data){
//    	double lat = data.Latitude;
//    	
//    	if(datum_mode == GpsData.NAD27){ //conversion required
//    		if(nct == null) initNADCON();
//    		try{
//				double[] src = new double[2];
//				src[0] = data.Longitude;
//				src[1] = data.Latitude;
//				nct.inverseTransform(src, 0, src, 0, 1);
//				lat = src[1];
//    		}catch (Exception e){
//    			e.printStackTrace();
//    		}
//    	}
//    	
//    	return formatLat(lat);
//    }
//    
//    public static String formatLon(GpsData data){
//    	double lon = data.Longitude;
//    	
//    	if(datum_mode == GpsData.NAD27){ //conversion required
//    		if(nct == null) initNADCON();
//    		try{
//				double[] src = new double[2];
//				src[0] = data.Longitude;
//				src[1] = data.Latitude;
//				nct.inverseTransform(src, 0, src, 0, 1);
//				lon = src[0];
//    		}catch (Exception e){
//    			e.printStackTrace();
//    		}
//    	}
//    	
//    	return formatLon(lon);
//    }
    
//    public static String formatLat(double lat){
//    	if(format_mode == MODE_DMS){
//    		int lat_deg = (int)(lat);
//    		int lat_min = (int)(60*(lat-lat_deg));
//    		float lat_sec = (float)(3600*(lat-lat_deg-lat_min/60.0));
//
//    		return lat_deg + " " + lat_min + "' " + sdf.format(lat_sec) + "\" "; 
//    		
//    	} else if(format_mode == MODE_DECIMAL){
//    		return ldf.format(lat);
//    	} else if(format_mode == MODE_DM){
//    		int lat_deg = (int)(lat);
//    		double lat_min = 60*(lat-lat_deg);
//
//    		return lat_deg + " " + df.format(lat_min) + "' "; 
//    		
//    	} else  {
//    		return "Unknown format mode ";
//    	}
//    }
//    
//    public static String formatLon(double lon){
//    	if(format_mode == MODE_DMS){
//    		int lon_deg = (int)(lon);
//    		int lon_min = (int)(60*(lon-lon_deg));
//    		float lon_sec = (float)(3600*(lon-lon_deg-lon_min/60.0));
//
//    		return lon_deg + " " + lon_min + "' " + sdf.format(lon_sec) + "\""; 
//    		
//    	} else if(format_mode == MODE_DECIMAL){
//    		return ldf.format(lon);
//    	} else if(format_mode == MODE_DM){
//    		int lon_deg = (int)(lon);
//    		double lon_min = 60*(lon-lon_deg);
//
//    		return lon_deg + " " + df.format(lon_min) + "' "; 
//    		
//    	} else  {
//    		return "Unknown format mode";
//    	}
//    }
    
 
    public static boolean MergeGpsString(String GPSstr, GpsData data){
        String[] words = GPSstr.split(",");
        if(words[0].compareTo("$GPRMC") == 0){
            // A "Recommended Minimum" sentence was found!
            return MergeGPRMC(words, data);
        }else if (words[0].compareTo("$GPGGA") == 0){
            // A "Satellites in View" sentence was received
            return MergeGPGGA(words, data);
        }else if (words[0].compareTo("$GPGSA") == 0){ 
            // A 
            return MergeGPGSA(words, data);
        }else if (words[0].compareTo("$HCHDG") == 0){
        	return MergeHCHDG(words, data);
            //return MergeHCHDG(words, data);
        } else {
            // Indicate that the sentence was not recognized
            return false;
        }


    }

    private static boolean MergeGPGGA(String[] values, GpsData data){
        try{	        
            int fixQuality = Integer.parseInt(values[(int)GPGGA.FIX_QUALITY]);
            int numsats = Integer.parseInt(values[(int)GPGGA.NUM_SATS]);
            double latitude = Double.parseDouble(values[(int)GPGGA.LATITUDE]);
            char lathemi = values[(int)GPGGA.LAT_HEMI].charAt(0);
            double longitude = Double.parseDouble(values[(int)GPGGA.LONGITUDE]);
            char lonhemi = values[(int)GPGGA.LON_HEMI].charAt(0);
            String elstring = values[(int)GPGGA.ELEVATION];
            elstring = elstring == "" ? "0.0" : elstring;
            double elevation = Double.parseDouble(elstring);

            int lat = (int)(latitude/100);
            latitude = lat + (latitude - 100*lat)/60;
            int lon = (int)(longitude/100);
            longitude = lon + (longitude - 100*lon)/60;
            
            if (lonhemi == 'W') longitude *= -1;
            if (lathemi == 'S') latitude *= -1;
            
            data.LockAcquired = (fixQuality == 1 || fixQuality == 2) ? true : false;
            data.Elevation = elevation;
            data.Latitude = latitude;
            data.lathemi = lathemi;
            data.Longitude = longitude;
            data.lonhemi = lonhemi;
            data.NumberOfSatellites = (byte)(numsats);
            data.goodData = true;

            return true;
        }
        catch (Exception e)
        {
            return false;
        }
    }

    private static boolean MergeGPGSA(String[] values, GpsData data){
        try{	        
            //TODO: Implement this
            return true;
        }
        catch (Exception e)
        {
            return false;
        }
    }

    private static boolean MergeGPRMC(String[] values, GpsData data){
        try{	        
            double heading = Double.parseDouble(values[(int)GPRMC.HEADING]);
            double speed = Double.parseDouble(values[(int)GPRMC.GROUND_SPEED]);
            double latitude = Double.parseDouble(values[(int)GPRMC.LATITUDE]);
            char lathemi = values[(int)GPRMC.LAT_HEMI].charAt(0);
            double longitude = Double.parseDouble(values[(int)GPRMC.LONGITUDE]);
            char lonhemi = values[(int)GPRMC.LON_HEMI].charAt(0);

            data.Latitude = latitude;
            data.lathemi = lathemi;
            data.Longitude = longitude;
            data.lonhemi = lonhemi;
            data.Heading = heading;
            data.Speed = speed;

            return true;
        }
        catch (Exception e)
        {
            return false;
        }
    }

    private static boolean MergeHCHDG(String[] values, GpsData data)
    {
        try
        {
            double heading = Double.parseDouble(values[(int)HCHDG.HEADING]);
            double variation = Double.parseDouble(values[(int)HCHDG.VARIATION]);
            char e_w = values[(int)HCHDG.E_W].charAt(0);

            if (e_w == 'E') data.Compass_Heading = heading - variation;
            else data.Compass_Heading = heading + variation;

            data.Heading = heading;
            
            return true;
        }
        catch (Exception e)
        {
            return false;
        }

    }
    
    
    private static DecimalFormat sdf = new DecimalFormat("####.00");
	private static DecimalFormat df = new DecimalFormat("####.0000");
	private static DecimalFormat ldf = new DecimalFormat("####.00000000");
	private static NADCONTransform nct = null;
	private static CoordinateConversion cc = new CoordinateConversion();
	public static final int MODE_DMS = 0;
	public static final int MODE_DD = 1;
	public static final int MODE_DDM = 2;
	public static final int MODE_UTM = 3;
	
	public static final int WGS84 = 0;
	public static final int NAD27 = 1;
	
	public static int format_mode = MODE_DD;
	public static int datum_mode = WGS84;
	
    /*  GPGGA
     *  1    = UTC time of fix
     *  2    = Latitude of fix
     *  3    = N or S of longitude
     *  4    = Longitude of fix
     *  5    = E or W of longitude
     *  6    = Fix Quality (0 = bad, 1 = GPS, 2 = DGPS)
     *  7    = Number of Satalites
     *  8    = Horizontal Dilution of Precision (HDOP)
     *  9    = Altitude above mean sea level
     *  10   = Units of Altitude
     *  11   = Height of geoid above WGS84 ellipsoid
     *  12   = Units of Height above geoid
     *  13   = Time since last DGPS update
     *  14   = DGPS reference station id
     *  15   = CheckSum
     */
    public static class GPGGA
    {
        public static int UTC_TIME = 1;
        public static int LATITUDE = 2;
        public static int LAT_HEMI = 3;
        public static int LONGITUDE = 4;
        public static int LON_HEMI = 5;
        public static int FIX_QUALITY = 6;
        public static int NUM_SATS = 7;
        public static int H_DILUTION = 8;
        public static int ELEVATION = 9;
        public static int ELEV_UNITS = 10;
        public static int G_HEIGHT = 11;
        public static int GH_UNITS = 12;
        public static int DGPS_AGE = 13;
        public static int CHECKSUM = 14;
    };

    /*  GPRMC
     *  1    = UTC time of fix
     *  2    = Data status (A=Valid position, V=navigation receiver warning)
     *  3    = Latitude of fix
     *  4    = N or S of longitude
     *  5    = Longitude of fix
     *  6    = E or W of longitude
     *  7    = Speed over ground in knots
     *  8    = Track made good in degrees True
     *  9    = UTC date of fix
     *  10   = Magnetic variation degrees (Easterly var. subtracts from true course)
     *  11   = E or W of magnetic variation 
     */
    public static class GPRMC
    {
    	public static int UTC_TIME = 1;
    	public static int STATUS = 2;
    	public static int LATITUDE = 3;
    	public static int LAT_HEMI = 4;
    	public static int LONGITUDE = 5;
    	public static int LON_HEMI = 6;
    	public static int GROUND_SPEED = 7;
    	public static int HEADING = 8;
    	public static int UTCDATE = 9;
    	public static int MAGNETIC = 10;
    	public static int CHECKSUM = 11;
    };

    /* HCHDG
     * 1 = Heading
     * 2 = Deviation
     * 3 = Variation
     * 4 = E/W magnetic variation
     */
    public static class HCHDG
    {
    	public static int HEADING = 1;
    	public static int DEVIATION = 2;
    	public static int DEVIATION2 = 3;
    	public static int VARIATION = 4;
    	public static int E_W = 5;
    }

}
