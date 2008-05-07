//======================================================================================
/** \file  task_sensor.h
 *  This file contains the class definition for a main sensor control. This is basically
 *  the data collection "unit" for Mirasky's Para-Ceres. Only one of these objects are
 *  necessary to operate all of the sensors onboard.
 *
 *  Sensors Available:
 *	1. 6 Degree of Freedom   x2	(6 A/D Channels)
 *	2. Load Cells		 x2	(1 A/D Channel)
 *	3. Pitot Tube		 x1	(1 A/D Channel)
 *	4. Static		 x1	(1 A/D Channel)
 *	5. Linear Actuators	 x2	(1 A/D Channel)
 *
 *  Revisions:
 *    \li  04-15-08 DSC Original (Relatively useless file... in progress/planning stages)
 *    \li  04-17-08 DSC Basic layout format written
 *
 *  License:
 *    This file released under the Lesser GNU Public License. The program is intended
 *    for educational use only, but its use is not restricted thereto. 
 */
//======================================================================================

#ifndef _TASK_SENSOR_H_                         // To prevent *.h file from being included
#define _TASK_SENSOR_H_                         // in a source file more than once

//-------------------------------------------------------------------------------------
/** This class should run the A/D converter on an AVR processor. It should have some
 *  better comments. 
 */

class task_sensors
{
    protected:
        // The ADC class needs a pointer to the serial port used to say hello (for testing)
        avr_uart* p_serial;                 // Pointer to a serial port for messages
	avr_adc* p_adc;			    // Pointer to the A/D converter object

    private:
    public:
	// This constructor creates a sensor controller to operate the various sensors
        sensors (void);
        char run (char);
};

#endif // _TASK_SENSORS_H_
