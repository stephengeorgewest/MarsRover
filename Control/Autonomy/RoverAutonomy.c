#include "RoverNetwork.h"
#include "openssc.h"
#include <math.h>
#define FL_CHAN 0
#define FR_CHAN 1
#define RL_CHAN 2
#define RR_CHAN 3

#define tlat 38.40637510
#define tlon -110.79154666
#define PI 3.141592654
#define heading_gain .5

#define m_nm 1852
#define nm_degree 60
#define earth_radius 6378137
#define m_degree_latitude 111132
#define adj_latitude 40
	

struct RoverNetwork RN_SERVO;
struct RoverNetwork RN_GPS;

void parseNMEAGPRMC(char* string, double* lat_lon);

double meters2Latitude(double m);
double meters2Longitude(double m, double latitude);
double latitude2meters(double latitude);
double longitude2meters(double longitude, double latitude);

float right = 80;
float left = 80;

int main( int argc, char** argv)
{
	init_multicast(&RN_SERVO, ROVER_GROUP_SERVO, ROVER_PORT_SERVO);
	init_multicast(&RN_GPS, ROVER_GROUP_GPS, ROVER_PORT_GPS);
	
	setup_non_blocking(&RN_GPS);
	char command[1024];

	while (1)
	{
		char message[MSGBUFSIZE];
		int nbytes= recieve_message(&RN_GPS, message);
		//printf("\nRecieved Message %d bytes long:%x\n", nbytes, message);
		if(nbytes<0)
			usleep(500);

		if(message[0]==ROVER_MAGIC_GPS
				&& message[1] == 'G'
				&& message[2] == 'P'
				&& message[3] == 'R'
				&& message[4] == 'M'
				&& message[5] == 'C')
		{
			int garbage = 0;
			char valid;
			//sscanf(message+7, "%i, %c",garbage, valid); 
			printf("valid = %c\n", valid);
			printf("%s\n", message);
			double lat_lon[3];
			parseNMEAGPRMC(message, lat_lon);
			printf("lat=%lf, lon=%lf, heading=%lf\n", lat_lon[0], lat_lon[1], lat_lon[2]);

			double diff_lat = tlat-lat_lon[0];
			double diff_lon = tlon-lat_lon[1];
			double dy = latitude2meters(diff_lat);
			double dx = longitude2meters(diff_lon, lat_lon[0]);
			
			double target_heading = 90-180*atan2(dy,dx)/PI;
			double heading_correction = (lat_lon[2]-target_heading);
			if(heading_correction > 180) heading_correction-=360;
			if(heading_correction < -180) heading_correction+=360;
			printf("target heading=%lf, heading_correction=%lf\n", target_heading, heading_correction);
//			if(heading_correction > 20) heading_correction = 20;
//			if(heading_correction < -20) heading_correction = -20;
			heading_correction *= heading_gain;
			left = 80 + heading_correction;
			right = 80 - heading_correction;
			if(right < 50) right = 50;
			if(right > 80) right = 80;
			if(left < 50) left = 50;
			if(left > 80) left = 80;
			printf("right=%lf, left = %lf\n", left, right);
		}
		command[0] = ROVER_MAGIC_SERVO;
		command[1] = 4;
		int i = 0;
		
		for(i = 0;i<4;i++){
			command[i*5 + 2] = (char)i;
			uint32_t val;
			if(!(i%2))
				memcpy(&val, &right, 4);
			else
				memcpy(&val,&left,4);
			uint32_t val2 = htonl(val);
			memcpy(command+(i*5+2+1), &val2, 4);
		}
	//	printf("Sending message\n");
		int sent = send_message(&RN_SERVO, command, 22);
	//	printf("sent = %d\n", sent);


			
	}
}


void parseNMEAGPRMC(char* string, double* lat_lon_heading){

//	int garbage = 0;
	double lat = 0;
	double lon = 0;
	double garbage = 0;
	char lat_hemi = 'N';
	char lon_hemi = 'W';
	double heading = 0;
//	sscanf(string+16, "%d", &garbage);
	sscanf(string+16, "%lf,%c,%lf,%c,%lf,%lf",&lat,&lat_hemi,&lon,&lon_hemi, &garbage,&heading); 
//	printf("garbage=%d\n", garbage);

	int lati = (int)(lat/100);
	lat = lati + (lat - 100*lati)/60;
	int loni = (int)(lon/100);
	lon = loni + (lon - 100*loni)/60;

	if (lon_hemi == 'W') lon *= -1;
        if (lat_hemi == 'S') lat *= -1;

	lat_lon_heading[0] = lat;
	lat_lon_heading[1] = lon;
	lat_lon_heading[2] = heading;
}

double meters2Latitude(double meters){
	double lat = meters / m_degree_latitude;
	return lat;
}

double meters2Longitude(double m, double latitude){
	//System.out.println("m = " + m + " lat = " + latitude);
	double radius = cos(latitude*PI/180)*earth_radius;
	double m_degree_longitude = radius*(PI/180);
	//System.out.println("radius = " + radius);
	//System.out.println("m_degree_longitude = " + m_degree_longitude);
	return m/m_degree_longitude;
}

double latitude2meters(double latitude){
	return latitude*m_degree_latitude;
}
	
double longitude2meters(double longitude, double latitude){
	//System.out.println("longitude = " + longitude + " lat = " + latitude);
	double radius = cos(latitude*PI/180)*earth_radius;
	double m_degree_longitude = radius*(PI/180);
	//System.out.println("radius = " + radius);
	//System.out.println("m_degree_longitude = " + m_degree_longitude);
//	System.out.println("longigtude = " + longitude);
//	System.out.println("lon in meters = " + m_degree_longitude*longitude);
	return m_degree_longitude*longitude;
}
