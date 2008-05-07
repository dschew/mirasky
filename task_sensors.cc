//======================================================================================
/** \file  task_sensor_control.h
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

// Channel definitions for the linear actuators
const unsigned char linAct_1    =  ;
const unsigned char linAct_2    =  ;

// Channel definitions for various measurement devices
const unsigned char loadCell_1  =  ;
const unsigned char loadCell_2  =  ;
const unsigned char pitot	=  ;
const unsigned char staticM     =  ;

// Channels for 6 DOF on chassis
const unsigned char sixDOF_A1   =  ;               
const unsigned char sixDOF_A2   =  ;               
const unsigned char sixDOF_A3   =  ;               
const unsigned char sixDOF_A4   =  ;               
const unsigned char sixDOF_A5   =  ;               
const unsigned char sixDOF_A6   =  ;               

// Channels for 6 DOF on parachute
const unsigned char sixDOF_B1   =  ;               
const unsigned char sixDOF_B2   =  ;               
const unsigned char sixDOF_B3   =  ;               
const unsigned char sixDOF_B4   =  ;               
const unsigned char sixDOF_B5   =  ;               
const unsigned char sixDOF_B6   =  ;               

//-------------------------------------------------------------------------------------
/** This constructor creates a sensor control task. The sensor control operates the various
 *  sensors on the Para-Ceres and collects the data and stores it until it is ready to be
 *  broadcast on the radio modem.
 *	Linear Actuator:
 *	Load Cell: 
 *	Pitot Tube:
 *	Static: 
 *	6 Degree of Freedom:
 *
 *  @param t_stamp   	     A timestamp which contains the time between runs of this task
 *  @param p_ser     	     A pointer to a serial port for sending messages if required
 */

task_sensors::task_sensors (time_stamp* t_stamp, avr_uart* p_ser, avr_adc p_)
    : stl_task (*t_stamp, p_ser)
{
    // Save pointers to motors, and sensors
    p_serial = p_ser;

    // Say hello
    p_serial->puts ("Sensor control task constructor\r\n");
}

//-------------------------------------------------------------------------------------
/** This is the function which runs when it is called by the task scheduler. It runs through
 *  each sensor collecting data and storing it in arrays before sending it through the radio
 *  to the ground station.
 *  @param state The state of the task when this run method begins running
 *  @return The state to which the task will transition, or STL_NO_TRANSITION if no
 *      transition is called for at this time
 */

char task_sensors::run (char state)
{
    switch (state)
    {
        // In State 0, we do nothing for now 
        case (INIT):
	    return (WAIT);
	    break;

	// In State 1, we wait for the right count before polling the sensors for data
	case (WAIT):
	    // If the right time has been reached, start moving through the devices for data
	    if (timeUP)
	        return (LINACT_1);
	    break;

	case (LINACT_1):
	    p_adc -> startConversion(linAct_1);

	    if (p_adc -> convertDone())
	    {
		dataArray[Actuator1] = p_adc -> getValue();
		return (LINACT_2):
	    }

	    break;

	case (LINACT_2):
	    p_adc -> startConversion(linAct_2);

	    if (p_adc -> convertDone())
	    {
		dataArray[Actuator2] = p_adc -> getValue();
		return (SIXDOF_1):
	    }

	    return (SIXDOF_1);
	    break;

       // In State 1, we check all the A/D channels for data on the first 6 DOF

        case (SIX_DOF_1):
	    p_adc -> startConversion(sixDOF_A1);

	    if (p_adc -> convertDone())
	    {
		dataArray[DOF_A1] = p_adc -> getValue();
	        p_adc -> startConversion(sixDOF_A2);

		if (p_adc -> convertDone())
	        {
		    dataArray[DOF_A2] = p_adc -> getValue();
		    p_adc -> startConversion(sixDOF_A3);

	    	    if (p_adc -> convertDone())
	    {
		dataArray[Actuator2] = p_adc -> getValue();
		return (SIXDOF_1):
	    }

	        }

	    }

	    return (SIXDOF_2);
            break;

        // In State 2, we check all the A/D channels for data on the second 6 DOF
        case (SIXDOF_2):
	    return (PITOT);
            break;
	
	case (PITOT):
	    return (STATICM);
	    break;

	case (STATICM):
	    return (LOADCELL_1)
	    break;

	case (LOADCELL_1):
	    return (LOADCELL_2);
	    break;

	case (LOADCELL_2):
	    return (WAIT);
	    break;

        // If the state isn't a known state, call Houston; we have a problem
        default:
            STL_DEBUG_PUTS ("WARNING: Avoid task in state ");
            STL_DEBUG_WRITE (state);
            STL_DEBUG_PUTS ("\r\n");
            return (ALL_ON);
    };

    // If we get here, no transition is called for
    return (STL_NO_TRANSITION);
}
