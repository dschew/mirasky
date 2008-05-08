//======================================================================================
/** \file  task_sensors.h
 *  This file contains the class definition for a main sensor control task. This is basically
 *  the operating system for the data collection "unit" for Mirasky's Para-Ceres. Only one of 
 *  these objects are necessary to operate all of the sensors onboard.
 *
 *  Sensors Available:
 *	1. Linear Actuator	 x2	(1 A/D Channel)
 *	2. 6 Degree of Freedom   x2	(6 A/D Channels)
 *	3. Load Cells		 x2	(1 A/D Channel)
 *	4. Pitot Tube		 x1	(1 A/D Channel)
 *	5. Static		 x1	(1 A/D Channel)
 *
 *  Revisions:
 *    \li  04-15-08 DSC Original (Relatively useless file... in progress/planning stages)
 *    \li  04-17-08 DSC Basic layout format written
 *    \li  04-18-08 DSC General variables defined for channels and task state diagram developed
 *
 *  License:
 *    This file released under the Lesser GNU Public License. The program is intended
 *    for educational use only, but its use is not restricted thereto. 
 */
//======================================================================================
#ifndef _TASK_SENSOR_H_                         // To prevent *.h file from being included
#define _TASK_SENSOR_H_                         // in a source file more than once

//-------------------------------------------------------------------------------------
/** This task class should collect all the data from the devices on the Para-Ceres. Some of the
 *  functions may block the processor, and further testing is required to figure out how critical
 *  the timing requirements may be.
 */

class task_sensors
{
    protected:
        // The sensors task class needs a pointer to the serial port used to say hello 
	// For testing purposes only... anything sent to the serial port will result in blocking
        avr_uart* p_serial;                 // Pointer to a serial port for messages
	avr_adc* p_adc;			    // Pointer to the A/D converter object

    private:
	int dataArray;			// Have to check these two for type... may need 
	long timeArray;			// a different variable type for these

    public:
	// This constructor creates a sensor controller to operate the various sensors
        task_sensors (void);
	// This function runs the sensor task
	char run (char);

	// These functions call the print to serial
	void printLinActA ();
	void printLinActB ();
	void printSixDOFA ();
	void printSixDOFB ();
	void printPitot ();
	void printStatic ();
	void printLoadA ();
	void printLoadB ();
};

#endif // _TASK_SENSORS_H_
