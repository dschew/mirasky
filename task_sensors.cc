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
 *    \li  04-19-08 DSC Minor adjustments to task state diagram (Allowing six DOF to block)
 *
 *  License:
 *    This file released under the Lesser GNU Public License. The program is intended
 *    for educational use only, but its use is not restricted thereto. 
 */
//======================================================================================

// State name definitions
#define  INIT		=  0
#define	 WAIT		=  1
#define  ACT_A		=  2
#define  ACT_B	        =  3
#define  SIX_DOF_A	=  4
#define  SIX_DOF_B	=  5
#define  PITOTTUBE	=  6
#define  STATICM	=  7
#define  LOAD_A		=  8
#define  LOAD_B		=  9

// Channel definitions for the linear actuators
const unsigned char linAct_1    =  ;
const unsigned char linAct_2    =  ;

// Channel definitions for various measurement devices
const unsigned char loadCell_1  =  ;
const unsigned char loadCell_2  =  ;
const unsigned char pitot_1	=  ;
const unsigned char static_1    =  ;

// Channels for 6 DOF on chassis
const unsigned char sixDOF_1    =  ;                             

// Channels for 6 DOF on parachute
const unsigned char sixDOF_2    =  ;               

// Array Slot Labels
const int linActA 		= 0;		// Linear actuator #1
const int linActB		= 1;		// Linear actuator #2
const int sixDOFA		= 2;		// Start of the first six DOF slots
const int sixDOFB		= 7;		// Start of the second six DOF slots
const int pitot_A		= 13;		// Pitot tube
const int static_A		= 14;		// Static measurement device
const int loadCellA		= 15;		// Load cell #1
const int loadCellB		= 16;		// Load cell #2

//-------------------------------------------------------------------------------------
/** This constructor creates a sensor control task. The sensor control operates the various
 *  sensors on the Para-Ceres and collects the data and stores it until it is ready to be
 *  broadcast on the radio modem.
 *	Linear Actuator: These control the turning lines for the Para-Ceres
 *			 Data is collected to close the control loop
 *	Load Cell: These collect data on the force generated from thrust and drag (?)
 *	Pitot Tube: This collects data on the airflow across the vehicle
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
	        return (ACT_A);
	    break;

	// In State 2, we get data for the first linear actuator and wait for it to be done
	// before transitioning to the next device
	case (ACT_A):
	    p_adc->startConversion(linAct_1);

	    if (p_adc->convertDone())
	    {
		dataArray[Actuator1] = p_adc->getValue();
		return (ACT_B):
	    }

	    break;

	// In State 3, we get data for the second linear actuator and wait for it to be done
	// before transitioning to the next device
	case (ACT_B):
	    p_adc -> startConversion(linAct_2);

	    if (p_adc -> convertDone())
	    {
		dataArray[Actuator2] = p_adc -> getValue();
		return (SIX_DOF_A):
	    }

	    break;

       // In State 4, we check all the A/D channels for data on the first 6 DOF

        case (SIX_DOF_A):
	    for (int i = 0; i < 6; i++)
	    {
	   	 dataArray[sixDOF_A + i] =  p_adc->readonce(sixDOF_1 + i);
	    }

	    return (SIX_DOF_B);
            break;

        // In State 2, we check all the A/D channels for data on the second 6 DOF
        case (SIX_DOF_B):
	    for (int i = 0; i < 6; i++)
	    {
		dataArray[sixDOF_B + i] = p_adc->readonce(sixDOF_1 + i);
	    }

	    return (PITOTTUBE);
            break;
	
	case (PITOTTUBE):
	    p_adc->startConversion(pitot_1);

	    if (p_adc->convertDone())
	    {
		dataArray[PitotA] = p_adc->getValue();
	    	return (STATICM);
	    }

	    break;

	case (STATICM):
	    p_adc->startConversion(static_1)
	    return (LOAD_A)
	    break;

	case (LOAD_1):
	    return (LOAD_B);
	    break;

	case (LOAD_B):
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
