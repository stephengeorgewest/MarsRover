/*
	FILE:
		ADIS16354A.c
	
	AUTHOR(s):
		Caleb Chamberlain
	
	DESCRIPTION:
		Function prototypes, preprocessor definitions, and datatype definitions for ADIS16354A IMU from Analog Devices.

	DEPENDENCIES:
		Communication with the IMU is performed over the PXA270 SPI port.  This code therefore
		includes SPI.h, and must be built with SPI.c.
			
	REVISION LOG:
		Sept. 15, 2008		- 	Original Revision (Caleb Chamberlain)
*/

#ifndef __ADIS16354A
#define __ADIS16354A

/********************************************** PREPROCESSOR DEFINITIONS *****************************************/
// These refer to the actual addresses used to reference the various sensors on the IMU.
// These are also passed as arguments to functions that read the sensor data off the SPI bus.
#define		ADIS_SUPPLY							0x0200		// IMU power supply reading
#define 	ADIS_X_GYRO							0x0400
#define 	ADIS_Y_GYRO							0x0600
#define		ADIS_Z_GYRO							0x0800
#define 	ADIS_X_ACCEL						0x0A00
#define 	ADIS_Y_ACCEL						0x0C00
#define		ADIS_Z_ACCEL						0x0E00
#define		ADIS_X_TEMP							0x1000
#define		ADIS_Y_TEMP							0x1200
#define		ADIS_Z_TEMP							0x1400
#define		ADIS_AUX_ADC						0x1600		// ADC reading from IMU (from aux. ADC input)

// Bias offset registers for accelerometers.  ...OFF0 refers to lowest-order byte
// ...OFF1 refers to highest-order byte
#define		ADIS_X_ACCEL_OFF0				0x2000
#define		ADIS_X_ACCEL_OFF1				0x2100
#define		ADIS_Y_ACCEL_OFF0				0x2200
#define		ADIS_Y_ACCEL_OFF1				0x2300
#define		ADIS_Z_ACCEL_OFF0				0x2400
#define		ADIS_Z_ACCEL_OFF1				0x2500

// Bais offset registers for gyros.  When reading the offset registers, either the lower order or the higher order
// address will work. In either case, the entire 16-bit bias register will be read.
#define		ADIS_X_GYRO_OFF0				0x1A00
#define		ADIS_X_GYRO_OFF1				0x1B00
#define		ADIS_Y_GYRO_OFF0				0x1C00
#define		ADIS_Y_GYRO_OFF1				0x1D00
#define		ADIS_Z_GYRO_OFF0				0x1E00
#define		ADIS_Z_GYRO_OFF1				0x1F00

// Flag for write command over SPI bus (to IMU)
#define		ADIS_SPI_WRITE					0x8000
#define		ADIS_SPI_READ						0x0000

// Defines the configuration used on the IMU.  Only one of the following should be defined.
#define 	ADIS_GYRO_300				// 300/s range
//#define	ADIS_GYRO_150				// 150/s range

// Defines the accelerometer dynamic range used on the IMU.  Only one of the following should be defined
#define	ADIS_ACCEL_10G				// 10 g accelerometer dynamic range	(ADIS16355A)
//#define	ADIS_ACCEL_3G				// 3 g accelerometer dynamic range (ADIS16354A)

// Constants for conversion from IMU measurements to float values
#ifdef		ADIS_GYRO_300
	#define		ADIS_GYRO_SCALE					.07326
#endif
#ifdef		ADIS_GYRO_150
	// TODO - Set scale factors for IMU with max. rate at 150/s
#endif

#ifdef		ADIS_ACCEL_3G
	#define		ADIS_ACCEL_SCALE				.0004672
#endif
#ifdef 		ADIS_ACCEL_10G
	#define		ADIS_ACCEL_SCALE				.002522
#endif

#define		ADIS_TEMP_SCALE					.1453
#define		ADIS_SUPPLY_SCALE				.0018315
#define		ADIS_AUX_SCALE					.0006105


// Bit masks for COMMAND register on IMU
#define		ADIS_COMMAND							0x3E00			// Address of command register
#define 	ADIS_SOFTWARE_RESET				0x0080			// Software reset bit
#define		ADIS_PRECISION_AUTONULL		0x0010			// Precision autonull command
#define		ADIS_FLASH_UPDATE					0x0008			// Update flash ("saves" current register settings)
#define		ADIS_FACTORY_DEFAULTS			0x0002			// Restore factory defaults
#define		ADIS_AUTONULL							0x0001			// autonull command

// Bit masks for STATUS register on IMU
#define		ADIS_STATUS								0x3C00			// Address of status register
#define 	ADIS_ZACCEL_DIAG_ERROR		0x8000			// Z-axis accelerometer self-diagnostic error flag
#define		ADIS_YACCEL_DIAG_ERROR		0x4000			// Y-axis ""
#define		ADIS_XACCEL_DIAG_ERROR		0x2000			// X-axis ""
#define		ADIS_ZGYRO_DIAG_ERROR			0x1000			// Z-axis gyro self-diagnostic error flag.
#define		ADIS_YGYRO_DIAG_ERROR			0x0800			// Y-axis ""
#define		ADIS_XGYRO_DIAG_ERROR			0x0400			// X-axis ""
#define		ADIS_ALARM2								0x0200			// Alarm 2 status
#define		ADIS_ALARM1								0x0100			// ALarm 1 status
#define		ADIS_SELF_DIAG_ERROR			0x0020			// Self-diagnostic error flag
#define		ADIS_SENSOR_OVERRANGE			0x0010			// Sensor overrange flag
#define		ADIS_SPI_FAILURE					0x0008			// SPI communications failure
#define		ADIS_CNTRL_UPDATE_FAILED	0x0004			// Control register update failed
#define		ADIS_SUPPLY_OVERRANGE			0x0002			// Power supply above 5.25 V
#define		ADIS_SUPPLY_UNDERRANGE		0x0001			// Power supply below 4.75 V

// Bit masks for MSC_CTRL register on IMU (high-order bits)
#define		ADIS_MSC_CTRL1						0x3400			// Address of MSC_CTRL register (higher-order bits)
#define		ADIS_ENABLE_SELF_TEST			0x0004

// Bit masks for MSC_CTRL register on IMU (low-order bits)
#define		ADIS_MSC_CTRL0						0x3500			// Address of MSC_CTRL register (lower-order bits)
#define		ADIS_ACCEL_BIAS_COMP			0x0080			// Set to enable linar acceleration bias compensation for gyroscopes
#define		ADIS_ACCEL_ORIGIN					0x0040			// Set to enable linear accelerometer origin alignment
#define		ADIS_DATA_READY_ENABLE		0x0004			// Set to enable data-ready indicator on IMU output pin
#define		ADIS_DATA_READY_POLARITY	0x0002			// 1 = active high, 0 = active low
#define		ADIS_DATA_READY_LINE			0x0001			// 1 = DIO2, 0 = DIO1

// Definitions governing which sensors are measured using ADIS_block_read( . )
// Designed to be passed as arguments to the function
#define		ADIS_ALL								0x01		// Read all channels (temperatures, supply voltage, aux. ADC, All rate gyros, All accels)
#define		ADIS_GYROS							0x02		// Read all gyro rates
#define		ADIS_ACCELS							0x04		// Read all accel rates
#define		ADIS_TEMPERATURE				0x08		// Read temperatures on each rate gyro
#define		ADIS_GYROS_ACCELS				0x10		// Read gyros and accels

// Addresses for SENS and AVG registers
#define 	ADIS_SENS								0x3900
#define		ADIS_AVG								0x3800

// Definitions to set dynamic range of sensor (using ADIS_set_range(.) )
#define		ADIS_RANGE_300					0x04
#define		ADIS_RANGE_150					0x02
#define		ADIS_RANGE_75						0x01

// In function calls that update IMU registers, these constants are passed as operators to determine whether the IMU flash is updated
// (if flash is updated, register values will be reloaded when the IMU restarts)
#define 	ADIS_UPDATE							0x01		
#define		ADIS_NO_UPDATE					0x00		 

// Return codes
#define		ADIS_SUCCESS						0x00
#define		ADIS_ERROR_NOT_READY		0x01		// Retrieved sample was already read - new data not yet available
#define		ADIS_ERROR_SUPPLY				0x02		// Power supply for IMU too low.
#define		ADIS_ERROR_BAD_CHANNEL	0x03		// Specified channel was invalid
#define		ADIS_ERROR_INIT					0x04		// Failed to initialize SPI bus
#define		ADIS_ERROR_IMU					0x05		// IMU reported an error - could be power supply, could be bad IMU - see datasheet
#define		ADIS_ERROR_NOT_INITIALIZED	0x06	// SPI port has not been initialized
#define		ADIS_ERROR_FLASH_UPDATE	0x07		// IMU failed to update flash
#define		ADIS_ERROR_SENSOR				0x08		// One of the sensors returned a value outside the correct range
#define		ADIS_ERROR_SPI_BUS			0x09		// There was an SPI bus-error

#define		ADIS_ERROR_X_GYRO				0x10		// When returned by ADIS_self_test(.), specifies that the x-gyro failed mechanical self-test
#define		ADIS_ERROR_Y_GYRO				0x20
#define		ADIS_ERROR_Z_GYRO				0x40
#define		ADIS_ERROR_X_ACCEL			0x80
#define		ADIS_ERROR_Y_ACCEL			0x100
#define		ADIS_ERROR_Z_ACCEL			0x200

#define		ADIS_ERROR_BAD_DATA			0x11

// Definition for SPI bus clock rate
#define		ADIS_CLOCK_RATE					590000 //30000  // ~550 KHz

// Number of samples to use as an average accel value when "zeroing" sensors (in ADIS_zero_accels() )
#define		ADIS_ACCEL_AVG					100

/********************************************** STRUCTURES ********************************************************/

// ADIS_data is used to store data read in blocks from the IMU, using the ADIS_block_read function
// 'x_gyro' is the roll rate, 'phi_dot'
// 'y_gyro' is the pitch rate, 'theta_dot'
// 'z_gyro' is the roll rate, 'psi_dot'
typedef struct __ADIS_data {
	float supply;
	union {
		float x_gyro;
		float phi_dot;
	};
	
	union {
		float y_gyro;
		float theta_dot;	
	};
	
	union {
		float z_gyro;
		float psi_dot;
	};
	
	float x_accel;
	float y_accel;
	float z_accel;
	float x_temperature;
	float y_temperature;	
	float z_temperature;
	float aux_adc;	
} ADIS_data;


/********************************************** FUNCTION DECLARATIONS *********************************************/
// See definitions for more descriptive comments

// Initialization functions
int ADIS_init( void );
int ADIS_close( void );
int ADIS_reset( void );
int ADIS_update_flash( void );

// Read/write functions
int ADIS_write( int command );
int ADIS_read( int channel, float* result );
int ADIS_block_read( int channels, ADIS_data* data );
int ADIS_read_register( int address, int* data );

// Sensor calibration/self-test
int ADIS_zero_gyros( void );
int ADIS_zero_accels( int options );
int ADIS_self_test( void );
int ADIS_set_filter( int config, int update_flash );
int ADIS_set_range( int config, int update_flash );

// Internal functions
float ADIS_convert( int channel, int rx );
int ADIS_error( int rx );
void ADIS_write_struct( ADIS_data* data, int channel, float formatted_data );

#endif

