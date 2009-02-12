/*
	FILE:
		ADIS16354A.c

	AUTHOR(s):
		Caleb Chamberlain

	DESCRIPTION:
		Implements functions for interfacing with the Analog Devices ADIS16354A IMU.


	DEPENDENCIES:
		Communication with the IMU is done over the Gumstix SPI port.  This code therefore
		includes SPI.h, and must be built with SPI.c.

	REVISION LOG:
		Sept. 15, 2008		- 	Original Revision (Caleb Chamberlain)
*/

/********************************************** INCLUDES *********************************************************/
#include "ADIS16354A.h"
#include "SPI.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>					// for 'open(.)'
#include <sys/types.h>			// for 'open(.)'
#include <sys/stat.h>				// for 'open(.)'
#include "network.h"

// Global file-descriptor used to MAP memory registers used to conrol SPI (and hence talk to the IMU)
int __ADIS_fd;
// Global flag indicating whether the IMU has been initialized
char __ADIS_initialized;

/********************************************** FUNCTION DEFINITIONS *********************************************/

/* ---------------------------------------------------------------------------------------
	FUNCTION:
		int ADIS_init( void );

	DESCRIPTION:
		Configures the SPI port to talk to the Analog Devices IMU.  This should be called before
		attempting to do anything with the IMU.

	RETURN VALUE:
		Returns 0 (ADIS_SUCCESS) if successfull
 ----------------------------------------------------------------------------------------- */
int ADIS_init( )
{
	// First create a file-descriptor for memory-management
	__ADIS_fd = open("/dev/mem", O_RDWR | O_SYNC);

	if( SPIinit( __ADIS_fd, ADIS_CLOCK_RATE ) ) {
		__ADIS_initialized = 1;
		return ADIS_SUCCESS;
	}

	return ADIS_ERROR_INIT;
}


/* ---------------------------------------------------------------------------------------
	FUNCTION:
		int ADIS_read( int channel, float* result );

	DESCRIPTION:
		Reads the specified channel on the Analog Devices IMU, places the data in 'result'
		'channel' must be one of:
			ADIS_SUPPLY
			ADIS_X_GYRO
			ADIS_Y_GYRO
			ADIS_Z_GYRO
		 	ADIS_X_ACCEL
		 	ADIS_Y_ACCEL
			ADIS_Z_ACCEL
			ADIS_X_TEMP
			ADIS_Y_TEMP
			ADIS_Z_TEMP
			ADIS_AUX_ADC

		Each individual call to ADIS_read results in two bus transfers over SPI (one to write the command, one to read the data)
		For better data throughput, use ADIS_block_read(.), which takes advantage of the full-duplex SPI bus

		Power supply voltage is returned in V
		Gyro rates are returned in degrees/second
		Accel measurements are in gravities
		Temperature = (result + 25 degrees Celsius)
		Aux. ADC is in Volts

	RETURN VALUE:
		Returns 0 (ADIS_SUCCESS) if successfull
  --------------------------------------------------------------------------------------- */
int ADIS_read( int channel, float* result )
{
	int rx;
	int error;

	// Make sure that the SPI port has been initialized
	if( !__ADIS_initialized ) {
		return ADIS_ERROR_NOT_INITIALIZED;
	}

	// Make sure that the specified channel is valid
	if( (channel != ADIS_SUPPLY) && (channel != ADIS_X_GYRO) && (channel != ADIS_Y_GYRO) && (channel != ADIS_Z_GYRO) && (channel != ADIS_X_ACCEL) && (channel != ADIS_Y_ACCEL) && (channel != ADIS_Z_ACCEL) && (channel != ADIS_X_TEMP) && (channel != ADIS_Y_TEMP) && (channel != ADIS_Z_TEMP) && (channel != ADIS_AUX_ADC) )
	{
		*result = 0;
		return ADIS_ERROR_BAD_CHANNEL;
	}

	// Get data
	ADIS_read_register( channel, &rx );

	error = ADIS_error( rx );
	if( error ) {
		*result = 0;
		return error;
	}

	// Compute actual float value of data
	*result = ADIS_convert( channel, rx );

	return ADIS_SUCCESS;
}



/* ---------------------------------------------------------------------------------------
	FUNCTION:
		int ADIS_block_read( int channels, ADIS_data* data );

	DESCRIPTION:
		Reads the specified channels on the Analog Devices IMU, places the data in the 'data'
		'channels' must be one of:
			ADIS_ALL						- 		 Read all channels (temperatures, supply voltage, aux. ADC, All rate gyros, All accels)
			ADIS_GYROS					-			 Read all gyro rates
			ADIS_ACCELS					-			 Read all accel rates
			ADIS_TEMPERATURE		-			 Read temperatures on each rate gyro
			ADIS_GYROS_ACCELS		-			 Read gyros and accels

		ADIS_block_read(.) takes advantage of the full-duplex SPI bus, which makes it more efficient than ADIS_read(.)

	RETURN VALUE:
		Returns 0 (ADIS_SUCCESS) if successfull
  --------------------------------------------------------------------------------------- */
int ADIS_block_read( int channels, ADIS_data* data )
{
	int channel[11];
	int count;				// Number of samples in channel[.]
	int rx;						// Stores raw data
	int index;				// index in channel[.]
	int error;				// used to store result of function call to ADIS_error(.)  Checks raw data for errors.
	float formatted_data;		// Stores formatted data

	// Make sure that the SPI port has been initialized
	if( !__ADIS_initialized ) {
		return ADIS_ERROR_NOT_INITIALIZED;
	}

	// Set contents of 'channel' based on the specified desired measurements.
	// Later, code will use the contents of the array to sample the IMU
	if( channels == ADIS_ALL )
	{
		channel[0] = ADIS_SUPPLY;
		channel[1] = ADIS_X_GYRO;
		channel[2] = ADIS_Y_GYRO;
		channel[3] = ADIS_Z_GYRO;
		channel[4] = ADIS_X_ACCEL;
		channel[5] = ADIS_Y_ACCEL;
		channel[6] = ADIS_Z_ACCEL;
		channel[7] = ADIS_X_TEMP;
		channel[8] = ADIS_Y_TEMP;
		channel[9] = ADIS_Z_TEMP;
		channel[10] = ADIS_AUX_ADC;

		count = 11;
	}
	else if( channels == ADIS_GYROS )
	{
		channel[0] = ADIS_X_GYRO;
		channel[1] = ADIS_Y_GYRO;
		channel[2] = ADIS_Z_GYRO;

		count = 3;
	}
	else if( channels == ADIS_ACCELS )
	{
		channel[0] = ADIS_X_ACCEL;
		channel[1] = ADIS_Y_ACCEL;
		channel[2] = ADIS_Z_ACCEL;

		count = 3;
	}
	else if( channels == ADIS_GYROS_ACCELS )
	{
		channel[0] = ADIS_X_GYRO;
		channel[1] = ADIS_Y_GYRO;
		channel[2] = ADIS_Z_GYRO;
		channel[3] = ADIS_X_ACCEL;
		channel[4] = ADIS_Y_ACCEL;
		channel[5] = ADIS_Z_ACCEL;

		count = 6;
	}
	else if( channels == ADIS_TEMPERATURE )
	{
		channel[0] = ADIS_X_TEMP;
		channel[1] = ADIS_Y_TEMP;
		channel[2] = ADIS_Z_TEMP;

		count = 3;
	}
	else
	{
		return ADIS_ERROR_BAD_CHANNEL;
	}

	// Get data from IMU
	SPI_TxRx( __ADIS_fd, channel[0] );
	for( index = 1; index < count; index++ )
	{
		rx = SPI_TxRx( __ADIS_fd, channel[index] );

		error = ADIS_error( rx );
		if( error ) {
			return error;
		}

		formatted_data = ADIS_convert( channel[index-1], rx );

		// Save data to data structure
		ADIS_write_struct( data, channel[index-1], formatted_data );
	}

	// Get last data value
	rx = SPI_TxRx( __ADIS_fd, channel[index - 1] );

	error = ADIS_error( rx );
	if( error ) {
		return error;
	}

	formatted_data = ADIS_convert( channel[index-1], rx );

	ADIS_write_struct( data, channel[index-1], formatted_data );

	return ADIS_SUCCESS;
}


/* ---------------------------------------------------------------------------------------
	FUNCTION:
		int ADIS_reset( void );

	DESCRIPTION:
		Performs a software reset of the IMU; this is equivalent to pulling the RST pin low on the
		actual device

	RETURN VALUE:
		Returns 0 (ADIS_SUCCESS) if successfull
  --------------------------------------------------------------------------------------- */
int ADIS_reset( )
{
	if( !__ADIS_initialized ) {
		return ADIS_ERROR_NOT_INITIALIZED;
	}

	return ADIS_write( ADIS_COMMAND | ADIS_SOFTWARE_RESET );
}



/* ---------------------------------------------------------------------------------------
	FUNCTION:
		int ADIS_set_filter( int config, int update_flash );

	DESCRIPTION:
		The ADIS16354A IMU implements an on-board Bartlett window FIR low-pass filter.  This function
		allows the user to specify the number of taps used in filter.

		Only the lowest three bits of 'config' are used (the decimal value can thus range between 0 and 6);
		2^'config' taps will be used in the filter.  There can be at most 64 taps in the filter.

		See the IMU datasheet for details about the frequency response of the filter for each setting.

		If update_flash = ADIS_UPDATE, then the range will remain as set by this function call
		even after the IMU restarts.

		If update_flash = ADIS_NO_UPDATE, then the change will be lost at restart.

	RETURN VALUE:
		Returns 0 (ADIS_SUCCESS) if successfull
  --------------------------------------------------------------------------------------- */
int ADIS_set_filter( int config, int update_flash )
{
	if( !__ADIS_initialized ) {
		return ADIS_ERROR_NOT_INITIALIZED;
	}

	// Get rid of any unused data.
	config &= 0x07;

	// Write new filter configuration
	ADIS_write( ADIS_AVG | config );

	if( update_flash == ADIS_UPDATE )
	{
		return ADIS_update_flash( );
	}

	return ADIS_SUCCESS;
}



/* ---------------------------------------------------------------------------------------
	FUNCTION:
		int ADIS_set_range( int config, int update_flash );

	DESCRIPTION:
		Sets the dynamic range of the rate gyros on the IMU.  'config' must be one of:
			ADIS_RANGE_300
			ADIS_RANGE_150
			ADIS_RANGE_75

		If update_flash = ADIS_UPDATE, then the range will remain as set by this function call
		even after the IMU restarts.

		If update_flash = ADIS_NO_UPDATE, then the change will be lost at restart.

	RETURN VALUE:
		Returns 0 (ADIS_SUCCESS) if successfull
  --------------------------------------------------------------------------------------- */
int ADIS_set_range( int config, int update_flash )
{
	if( !__ADIS_initialized ) {
		return ADIS_ERROR_NOT_INITIALIZED;
	}

	if( (config != ADIS_RANGE_300) && (config != ADIS_RANGE_150) && (config != ADIS_RANGE_75) )
	{
		return ADIS_ERROR_BAD_DATA;
	}

	// Write new range data
	ADIS_write( ADIS_SENS | config );

	if( update_flash == ADIS_UPDATE )
	{
		return ADIS_update_flash( );
	}

	return ADIS_SUCCESS;
}



/* ---------------------------------------------------------------------------------------
	FUNCTION:
		int ADIS_zero_gyros( void );

	DESCRIPTION:
		Initiates IMU self-calibration of rate gyros.  The ADIS16354A automatically takes measurements of the three gyro axes and
		determines the mean.  The gyro biases are then set appropriately so that the readings are zero-mean.  Biases are saved into flash
		on the IMU, so that if the power is cycled, bias values are maintained.

		THIS OPERATION TAKES ROUGHLY 30 SECONDS.  During this time, the IMU should remain stationary.  This function will block for the entire
		30 second period.

		This function should not need to be	called very often.  Most of the DC offset you might observe in the gyro readings is a result of
		temperature variations.  Software that uses the IMU data should take temperature into account and compensate.

	RETURN VALUE:
		Returns 0 (ADIS_SUCCESS) if successfull
  --------------------------------------------------------------------------------------- */
int ADIS_zero_gyros(  )
{
	int index;

	// Make sure that the SPI port has been initialized
	if( !__ADIS_initialized ) {
		return ADIS_ERROR_NOT_INITIALIZED;
	}

	ADIS_write( ADIS_SPI_WRITE | ADIS_COMMAND | ADIS_PRECISION_AUTONULL );

	// Wait for about 30 seconds before returning
	for( index = 0; index < 60; index++ )
	{
		usleep( 500000 );		// 500 ms
	}

	return ADIS_SUCCESS;
}

/* ---------------------------------------------------------------------------------------
	FUNCTION:
		int ADIS_zero_accels( int options );

	DESCRIPTION:
		When this function is called, it is assumed that the z-axis is pointing perfectly in the direction of the gravity
		vector, and that the x and y axes should see no acceleration.  Thus, the accel biases are set to match what is being read.
		(ie. after calling ADIS_zero_accels, the bias registers will be set so that acceleration along the x and y axes will be zero,
		and acceleration along the z axis will be -1).

		If 'options' == ADIS_UPDATE, then the new bias values will be written to flash, so that on restart the bias values will remain.
		If 'options' == ADIS_NO_UPDATE, then the new bias values will be lost when the IMU is restarted.  Note that the flash has a limited
		lifetime; the user should avoid performing flash updates too regularly.

		This function could be useful to call this function prior to quadrotor flight to compensate for misalignment of the IMU with respect to
		the quadrotor airframe.

	RETURN VALUE:
		Returns 0 (ADIS_SUCCESS) if successfull
  --------------------------------------------------------------------------------------- */
int ADIS_zero_accels( int options )
{
	int index;
	ADIS_data sensors;
	float avg_x, avg_y, avg_z;
	int bias_x, bias_y, bias_z;		// Values to be written to the IMU to offset incorrect accel biases.
	int old_bias_x, old_bias_y, old_bias_z;		// Biases on the IMU at time of this function call.

	// Make sure that the SPI port has been initialized
	if( !__ADIS_initialized ) {
		return ADIS_ERROR_NOT_INITIALIZED;
	}

	// Get an average accel measurement on each axis
	avg_x = 0;
	avg_y = 0;
	avg_z = 0;
	for( index = 0; index < ADIS_ACCEL_AVG; index++ )
	{
		ADIS_block_read( ADIS_ACCELS, &sensors );

		avg_x += sensors.x_accel;
		avg_y += sensors.y_accel;
		avg_z += sensors.z_accel;

		// Wait 1 ms to ensure that the next block read gets up-to-date data
		usleep( 1000 );
	}

	avg_x = avg_x/ADIS_ACCEL_AVG;
	avg_y = avg_y/ADIS_ACCEL_AVG;
	avg_z = avg_z/ADIS_ACCEL_AVG;

	// We want avg_x and avg_y to be 0, and avg_z to be -1.
	// Given the actual averages, we compute the bias offsets needed
	// to bring them to the desired range, and write the new biases to the IMU
	bias_x = (int)(-avg_x/ADIS_ACCEL_SCALE);
	bias_y = (int)(-avg_y/ADIS_ACCEL_SCALE);
	bias_z = (int)(-(1+avg_z)/ADIS_ACCEL_SCALE);

	// Get current biases (since they will have affected the averages)
	ADIS_read_register( ADIS_X_ACCEL_OFF0, &old_bias_x );
	ADIS_read_register( ADIS_Y_ACCEL_OFF0, &old_bias_y );
	ADIS_read_register( ADIS_Z_ACCEL_OFF0, &old_bias_z );

	// Do sign-extension for negative values
	if( old_bias_x & 0x8000 ) {
		old_bias_x |= 0xFFFF8000;
	}
	if( old_bias_y & 0x8000 ) {
		old_bias_y |= 0xFFFF8000;
	}
	if( old_bias_z & 0x8000 ) {
		old_bias_z |= 0xFFFF8000;
	}

	bias_x += old_bias_x;
	bias_y += old_bias_y;
	bias_z += old_bias_z;

	// Send bias commands for x-axis
	// (Start with lower order bits, then send higher-order bits)
	ADIS_write( ADIS_SPI_WRITE | ADIS_X_ACCEL_OFF0 | (0x0FF & bias_x ) );
	ADIS_write( ADIS_SPI_WRITE | ADIS_X_ACCEL_OFF1 | (0x0FF & (bias_x >> 8)) );

	// Send bias commands for y-axis
	ADIS_write( ADIS_SPI_WRITE | ADIS_Y_ACCEL_OFF0 | (0x0FF & bias_y ) );
	ADIS_write( ADIS_SPI_WRITE | ADIS_Y_ACCEL_OFF1 | (0x0FF & (bias_y >> 8)) );

	// Send bias commands for z-axis
	ADIS_write( ADIS_SPI_WRITE | ADIS_Z_ACCEL_OFF0 | (0x0FF & bias_z ) );
	ADIS_write( ADIS_SPI_WRITE | ADIS_Z_ACCEL_OFF1 | (0x0FF & (bias_z >> 8)) );

	// If the ADIS_UPDATE flag is set in 'options', then save new bias values to flash
	if( options & ADIS_UPDATE )
	{
		return ADIS_update_flash( );
	}


	return ADIS_SUCCESS;
}



/* ---------------------------------------------------------------------------------------
	FUNCTION:
		int ADIS_update_flash( void );

	DESCRIPTION:
		Updates IMU flash with current register values.  Takes ~100 ms

	RETURN VALUE:
		Returns 0 (ADIS_SUCCESS) if success
		Returns ADIS_ERROR_NOT_INITIALIZED if the file descriptor was never opened.
  --------------------------------------------------------------------------------------- */
int ADIS_update_flash( void )
{
	int status_reg;

	ADIS_write( ADIS_SPI_WRITE | ADIS_COMMAND | ADIS_FLASH_UPDATE );

	// Wait ~100 ms for flash write to complete
	usleep( 103000 );

	// Check the status register and make sure the flash update was succesfull
	ADIS_read_register( ADIS_STATUS, &status_reg );

	if( status_reg & ADIS_CNTRL_UPDATE_FAILED ) {
		return ADIS_ERROR_FLASH_UPDATE;
	}

	return ADIS_SUCCESS;
}



/* ---------------------------------------------------------------------------------------
	FUNCTION:
		int ADIS_close( void );

	DESCRIPTION:
		Closes the file descriptor that was used to map SPI memory registers.

	RETURN VALUE:
		Returns 0 (ADIS_SUCCESS) if success
		Returns ADIS_ERROR_NOT_INITIALIZED if the file descriptor was never opened.
  --------------------------------------------------------------------------------------- */
int ADIS_close( )
{
	if( __ADIS_initialized )
	{
		close( __ADIS_fd );
		return ADIS_SUCCESS;
	}

	return ADIS_ERROR_NOT_INITIALIZED;
}


/* ---------------------------------------------------------------------------------------
	FUNCTION:
		int ADIS_read_register( int address, int* data );

	DESCRIPTION:
		Reads the contents of the specified register and saves the contents to the memory location
		pointed to by 'data'.  This function assumes that the register address is already shifted
		into the position expected by the IMU (i.e. register 0x00IE would be 0xIE00 on the SPI bus).

		Register address definitions in ADIS16354A.h use pre-shifted register values.
		(ie. ADIS_read_register( ADIS_STATUS, &result ) would store the STATUS register contents in
		'result')

	RETURN VALUE:
		ADIS_SUCCESS if succesfull.
  --------------------------------------------------------------------------------------- */
int ADIS_read_register( int address, int* data )
{

	// Make sure that the SPI port has been initialized
	if( !__ADIS_initialized ) {
		return ADIS_ERROR_NOT_INITIALIZED;
	}

	SPI_TxRx( __ADIS_fd, (ADIS_SPI_READ | address ) );
	*data = SPI_TxRx( __ADIS_fd, 0x0000 );			// Data on second bus request is irrelevant

	return ADIS_SUCCESS;
}

/* ---------------------------------------------------------------------------------------
	FUNCTION:
		int ADIS_self_test( void );

	DESCRIPTION:
		Causes the IMU to perform a self-test of all sensors.  The self-test operation takes
		~ 35 ms

	RETURN VALUE:
		ADIS_SUCCESS if succesfull.

		If one or more of the MEMS sensors failed, the bits of the return value will specify
		which channel(s) failed. ie
			if( returnval & ADIS_ERROR_X_GYRO ) // the x-axis gyro failed
			if( returnval & ADIS_ERROR_Y_GYRO ) // the y-axis gyro failed
		The possible failure flags are:
			ADIS_ERROR_X_GYRO
			ADIS_ERROR_Y_GYRO
			ADIS_ERROR_Z_GYRO
			ADIS_ERROR_X_ACCEL
			ADIS_ERROR_Y_ACCEL
			ADIS_ERROR_Z_ACCEL

  --------------------------------------------------------------------------------------- */
int ADIS_self_test( )
{
	int status_reg;
	int error;

	// Make sure that the SPI port has been initialized
	if( !__ADIS_initialized ) {
		return ADIS_ERROR_NOT_INITIALIZED;
	}

	// Start self-test
	ADIS_write( ADIS_MSC_CTRL1 | ADIS_ENABLE_SELF_TEST );

	// Wait for completion (40 ms to make sure self-test finishes)
	usleep( 40000 );

	// Read status register
	ADIS_read_register( ADIS_STATUS, &status_reg );

	if( (status_reg & ADIS_SELF_DIAG_ERROR) == 0 )
	{
			return ADIS_SUCCESS;
	}

	// There was an error if code reaches this point; determine which sensor(s) failed and return.
	error = 0;
	// Check accels
	error = (status_reg & ADIS_ZACCEL_DIAG_ERROR) | (status_reg & ADIS_YACCEL_DIAG_ERROR) | (status_reg & ADIS_XACCEL_DIAG_ERROR);
	// Check gyros
	error |= ( (status_reg & ADIS_ZGYRO_DIAG_ERROR) | (status_reg & ADIS_YGYRO_DIAG_ERROR) | (status_reg & ADIS_XGYRO_DIAG_ERROR) );

	return error;
}


/* ---------------------------------------------------------------------------------------
	FUNCTION:
		int ADIS_write( int command );

	DESCRIPTION:
		Writes the data in 'command' to the SPI bus.

	RETURN VALUE:
		ADIS_SUCCESS if succesfull
  --------------------------------------------------------------------------------------- */
int ADIS_write( int command )
{
	if( !__ADIS_initialized ) {
		return ADIS_ERROR_NOT_INITIALIZED;
	}

	SPI_TxRx( __ADIS_fd, ADIS_SPI_WRITE | command );

	return ADIS_SUCCESS;
}



/********************************************* INTERNAL FUNCTIONS *************************************************/
// These functions need not be called by the end user of the IMU libraries



/* ---------------------------------------------------------------------------------------
	FUNCTION:
		float ADIS_convert( int channel, int rx );

	DESCRIPTION:
		Takes the integer data received from the IMU and converts to a float in the proper units.
		Works for all channels (temperature, accel, gyro, power supply, and aux. ADC)
		'channel' must be one of:
			ADIS_SUPPLY
			ADIS_X_GYRO
			ADIS_Y_GYRO
			ADIS_Z_GYRO
		 	ADIS_X_ACCEL
		 	ADIS_Y_ACCEL
			ADIS_Z_ACCEL
			ADIS_X_TEMP
			ADIS_Y_TEMP
			ADIS_Z_TEMP
			ADIS_AUX_ADC

	RETURN VALUE:
		Returns the formatted value

		Power supply voltage is returned in V
		Gyro rates are returned in degrees/second
		Accel measurements are in gravities
		Temperature in degrees Celsius
		Aux. ADC is in Volts
  --------------------------------------------------------------------------------------- */
float ADIS_convert( int channel, int rx )
{

	// Clear 'new data' bit and error bit
	rx &= ~0xC000;

	// If the measured channel is a gyro or accelerometer, check for negative data and sign-extend if necessary
	// AUX_ADC and SUPPLY_OUT are both unsigned numbers, so this code doesn't apply.  The temperature channels are signed,
	// but are only 12-bits (rather than 14) so the sign extending code will be a little different.
	if( (channel < ADIS_X_TEMP) && (channel > ADIS_SUPPLY) )
	{
		if( rx & 0x2000 )
		{
			rx |= 0xFFFFE000;
		}
	}
	// Do sign extension for temperature channels
	else if( (channel < ADIS_AUX_ADC) && (channel > ADIS_Z_ACCEL) )
	{
		// Clear bits 13 and 14 (these are "don't care bits," want to make sure they are not set, as it could influence the value of the int).
		rx &= ~0xF000;

		if( rx & 0x0800 )
		{
			rx |= 0xFFFFF800;
		}
	}

	// Conversion for gyro channels
	if( (channel < ADIS_X_ACCEL) && (channel > ADIS_SUPPLY) )
	{
		return ADIS_GYRO_SCALE*(float)rx;
	}
	// Conversion for accel channels
	else if( (channel < ADIS_X_TEMP) && (channel > ADIS_Z_GYRO) )
	{
		return ADIS_ACCEL_SCALE*(float)rx;
	}
	// Conversion for temperature channels
	else if( (channel < ADIS_AUX_ADC) && (channel > ADIS_Z_ACCEL) )
	{
		return (ADIS_TEMP_SCALE*(float)rx + 25);
	}
	// Conversion for Aux ADC channel
	else if( channel == ADIS_AUX_ADC )
	{
		// Truncate to 12-bits
		rx &= ~0xF000;
		return ADIS_AUX_SCALE*(float)rx;
	}
	// Conversion for power supply channel
	else
	{
		// Truncate to 12-bits
		rx &= ~0xF000;
		return ADIS_SUPPLY_SCALE*(float)rx;
	}

}



/* ---------------------------------------------------------------------------------------
	FUNCTION:
		int ADIS_error( int rx );

	DESCRIPTION:
		Takes the raw data returned by the IMU and checks for errors (only used when reading sensor output register)

	RETURN VALUE:
		Returns 0 (ADIS_SUCCESS) if data is OK.
		Otherwise, returns one of the several error codes listed in ADIS16354A.h
  --------------------------------------------------------------------------------------- */
int ADIS_error( int rx )
{
	int status_reg;

	// See if data was new
	if( !(rx & 0x8000) ) {
		return ADIS_ERROR_NOT_READY;
	}

	// Check for IMU error
	if( rx & 0x4000 )
	{
		// Poll status register to determine what the error is
		ADIS_read_register( ADIS_STATUS, &status_reg );

		// Check for SPI bus error
		if( status_reg & ADIS_SPI_FAILURE )
		{
			return ADIS_ERROR_SPI_BUS;
		}

		// Check for bad sensor reading
		if( status_reg & ADIS_SENSOR_OVERRANGE )
		{
			return ADIS_ERROR_SENSOR;
		}

		// Make sure power supply is in correct range
		if( (status_reg & ADIS_SUPPLY_OVERRANGE) || (status_reg & ADIS_SUPPLY_UNDERRANGE) )
		{
			return ADIS_ERROR_SUPPLY;
		}

		// Some other error...
		return ADIS_ERROR_IMU;
	}

	return ADIS_SUCCESS;
}

/* ---------------------------------------------------------------------------------------
	FUNCTION:
		void ADIS_write_struct( ADIS_data* data, int channel, float formatted_data );

	DESCRIPTION:
		Takes as arguments a pointer to an IMU data structure, a 'channel', and a float to write to the structure.
		This function sets the variable in the structure specified by 'channel' to the value in 'formatted_data'

	RETURN VALUE:
		None

  ---------------------------------------------------------------------------------------*/
void ADIS_write_struct( ADIS_data* data, int channel, float formatted_data )
{

	// Based on the channel, write data to the proper location in the ADIS_data structure
	// Note that the data just received is the result of the last bus write (hence the 'count - 1' array location)
	switch( channel )
	{
		case ADIS_X_GYRO:
			data->x_gyro = formatted_data;
		break;

		case ADIS_Y_GYRO:
			data->y_gyro = formatted_data;
		break;

		case ADIS_Z_GYRO:
			data->z_gyro = formatted_data;
		break;

		case ADIS_X_ACCEL:
			data->x_accel = formatted_data;
		break;

		case ADIS_Y_ACCEL:
			data->y_accel = formatted_data;
		break;
			case ADIS_Z_ACCEL:
			data->z_accel = formatted_data;
		break;
			case ADIS_X_TEMP:
			data->x_temperature = formatted_data;
		break;
			case ADIS_Y_TEMP:
			data->y_temperature = formatted_data;
		break;

		case ADIS_Z_TEMP:
			data->z_temperature = formatted_data;
		break;

		case ADIS_SUPPLY:
			data->supply = formatted_data;
		break;

		case ADIS_AUX_ADC:
			data->aux_adc = formatted_data;
		break;
	}
}

