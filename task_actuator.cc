//======================================================================================
/** \file  task_actuator.cc
 *
 *  This file contains the code necessary to operate the linear actuators. It reads the
 *  RC PWM signal from the controller and then converts the duty cycle into a usable
 *  position for the actuator. It uses interrupts to read the time between the rising
 *  edge of the signal and the falling edge of the signal.
 *
 *	\li  04-11-08  DSC  And the world was created...
 *	\li  04-15-08  DSC  Setting up interrupts to read the PWM signal
 *	\li  04-18-08  DSC  Creating task state structure
 *	\li  04-22-08  DSC  Rearranging states
 *	
 *  License:
 *    This file released under the Lesser GNU Public License. The program is intended
 *    for educational use only, but its use is not restricted thereto. 
 */
//======================================================================================

#include <stdlib.h>                         // Include standard library header files
#include <avr/io.h>
#include <avr/interrupt.h>

#include "avr_serial.h"
#include "stl_debug.h"
#include "stl_us_timer.h"
#include "stl_task.h"
#include "task_actuator.h"

// State definitions
#define UPDATE_STICK_POSITION = 0
#define STICK_UPDATE_DELAY = 1

// Flag shows that pin state (high or low)
bool pinStatus = false;
// Flag shows that an interrupt has triggered
bool pinStatusFlag = false;

//should go in h file
time_stamp risingedge_time, fallingedge_time, pwm_width;

ISR(INT4_vect)
{
    if((PINE & 0b00010000) == 0b00010000)
    {
	// When the pin goes high (rising edge), set the flag to say interrupt has occurred
	pinStatusFlag = true;
	// Set pin state to be high
	pinStatus = true;
    }	
    else
    {
        // Otherwise 
	pinstatusflag = true;
	    pinstatus = false;
    }
}

//-------------------------------------------------------------------------------------
/** This constructor sets up an A/D converter. How does it do so?
 *  @param p_serial_port A pointer to the serial port which writes debugging info. 
 */
	
task_actuator::task_actuator (avr_uart* p_serial_port, task_timer* the_timer, time_stamp* the_timestamp) : stl_task(*the_timer, the_timestamp)
{
	debug_port = p_serial_port;
	timer = the_timer;

	// Prepare interrupts (Port E pin 4,5 free for external interrupts)
	EICRB |= 0b00000001;	// This code enables interrupts on pin E4
	EIMSK |= 0b00010000;
	*debug_port << "Setting up";
	pwm_width_value = 0;
	stickupdate_dummy = 0;
	stick_position = 0;
}

//-------------------------------------------------------------------------------------
/** This method takes one A/D reading from the given channel, and returns the value
 *  \param  channel The A/D channel which is being read must be from 0 to 7
 *  \return The result of the A/D conversion, or 0xFFFF if there was a timeout
 */

char task_actuator::run (char state)
{
	switch(state){
		case(UPDATE_STICK_POSITION):
			if(time_to_update_stick_position_value){
				if(pinstatusflag){
					if(pinstatus){
						risingedge_time = timer.get_time_now();
						pinstatusflag = false;
					}
					else{
						fallingedge_time = timer.get_time_now();
						pwm_width = fallingedge_time - risingedge_time;
						pwm_width.get_time(&pwm_width_value);
						*debug_port << pwm_width_value << "        \r";
						stick_position = 0; // CONVERT PWM WIDTH TO STICK POSITION HERE
						pinstatusflag = false;
						time_to_update_stick_position_value = false;
					}
				}
			}
			return STICK_UPDATE_DELAY;
		case(STICK_UPDATE_DELAY):
			if(stickupdate_dummy++ > 1000){
				time_to_update_stick_position_value = true;
			}
			return UPDATE_STICK_POSITION;
			//case(ACTUATOR_STUFF):
			//deal with actuator stuff
			//}
}

/*
   int actuator::findDutyCycle ()
   {

   }
   */

