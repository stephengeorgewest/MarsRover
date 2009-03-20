package rover.utils;

import java.text.DecimalFormat;

public class GpsData{
	
	private static DecimalFormat df = new DecimalFormat("####.####");
	 
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
    static class GPGGA
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
    static class GPRMC
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
    static class HCHDG
    {
    	public static int HEADING = 1;
    	public static int DEVIATION = 2;
    	public static int DEVIATION2 = 3;
    	public static int VARIATION = 4;
    	public static int E_W = 5;
    }

	
	public double Longitude;
    public double Latitude;
    public double Elevation;
    public double Speed;
    public double Heading;
    public double Compass_Heading;
    public byte NumberOfSatellites;
    public boolean LockAcquired;
    public byte Strikes;
    public boolean goodData;
    public char lathemi;
    public char lonhemi;
	
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
        Strikes = 5;
        goodData = false;
        lathemi = ' ';
        lonhemi = ' ';

    }

    @Override
    public String toString()
    {
        return "GpsData";
    }
    
    public boolean MergeData(String gpsString){
    	return GpsData.MergeGpsString(gpsString, this);
    }
    

    public double getRealLat()
    {
        if (Latitude == Double.NaN) return 0;
        String lat = df.format(Latitude);
        int ilat = lat.indexOf('.');
        double latitude = 0;
        double minutes = 0;
        try
        {
            latitude = Double.parseDouble(lat.substring(0, ilat - 2));
            minutes = Double.parseDouble(lat.substring(ilat - 2, lat.length() - (ilat - 2)));
        }
        catch (Exception e) 
        {
            return 0;
        }
        latitude += minutes / 60.0;
        if (lathemi == 'W') latitude *= -1;

        return latitude;
    }
    public double getRealLong()
    {
        if (Longitude == Double.NaN) return Longitude;
        String lon = df.format(Longitude);
        int ilon = lon.indexOf('.');

        double longitude = 0;
        double minutes = 0;

        try
        {
            longitude = Double.parseDouble(lon.substring(0, ilon - 2));
            minutes = Double.parseDouble(lon.substring(ilon - 2, lon.length() - (ilon - 2)));
        }
        catch (Exception e)
        {
            return 0;
        }
        longitude += minutes / 60.0;
        if (lonhemi == 'W') longitude *= -1;

        return longitude;
    }


    public String getFormatedPos()
    {
        if (Latitude == Double.NaN || Longitude == Double.NaN) return "Invalid";
        String lat = df.format(Latitude);
        String lon = df.format(Longitude);

        int ilat = lat.indexOf('.');
        int ilon = lon.indexOf('.');

        if (ilat == -1 || ilon == -1) return "Invalid";

        

        //Console.WriteLine(lat + " index of . is " + ilat);

        String latitude = "";
        String longitude = "";
        try
        {
            latitude = lat.substring(0, ilat - 2) + " " + lat.substring(ilat - 2, lat.length() - (ilat - 2)) + "' " + lathemi;
            longitude = lon.substring(0, ilat - 2) + " " + lon.substring(ilat - 2, lon.length() - (ilon - 2)) + "' " + lonhemi;

        }
        catch (Exception e) 
        {
            return "Error";
        }
        return latitude + ", " + longitude;
    }
    
    
 
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
    
}
