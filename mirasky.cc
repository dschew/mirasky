 //======================================================================================
/** \file mirasky.cc
 *      This file contains a program to run the Mirasky Para-Ceres Aircraft.
 *	Current tasks:
 *	    1. Operate linear actuators with a closed control loop
 *		a) Read input from RC controller (RC PWM signal)
 *		b) Take PWM signal and find the duty cycle
 *		c) Convert into a digital value which represents distance to move
 *		d) Operate motor to move that distance
 *		e) Read A/D converter value for actual position
 *		f) Adjust position accordingly
 *	    2. Read two 6 DOF sensors (6 A/D converter channels each)
 *	    3. Read three accelerometers (1 A/D converter channel each)
 *	    4. Read three gyros (1 A/D converter channel each)
 *
 *  Revisions
 *    \li  02-26-08  LTD  Original file
 *    \li  03-01-08  LTD  Writing code
 *    \li  04-01-08  DSC  Mirasky is born
 *    \li  04-05-08  DSC  Structure created
 *    \li  04-08-08  DSC  Basic classes included
 */
//======================================================================================
                                            // System headers included with < >
#include <stdlib.h>                         // Standard C library
#include <avr/io.h>                         // Input-output ports, special registers
#include <avr/interrupt.h>                  // Interrupt handling functions
#include <stdint.h>

                                            // User written headers included with " "
#include "avr_adc.h"
#include "avr_serial.h"                     // Serial port header
#include "stl_us_timer.h"                   // Microsecond-resolution timer
#include "stl_debug.h"                      // Handy debugging macros
#include "stl_task.h"                       // Base class for all task classes
#include "avr_adc.h"			    // ADC header

#define  BAUD_DIV        52                 // For Mega128 with 8MHz crystal

/** The main function is the "entry point" of every C program, the one which runs first
 *  (after standard setup code has finished). For mechatronics programs, main() runs an
 *  infinite loop and never exits. 
 */

int main ()
{
    char time_string[16];                   // Character buffer holds printable time

    // Create a radio modem to act as the serial port object. Output will be printed to 
    // this port, which should be hooked up to a dumb terminal program like minicom
    avr_9xtend the_radio (BAUD_DIV);

    // Print a greeting message. This is almost always a good thing because it lets 
    // the user know that the program is actually running
    the_radio.puts ("\r\n\nInitiating Mirasky Routine. Hello!\r\n");

    // Create a really basic no-frills analog to digital converter interface object
    avr_adc my_adc (&the_serial_port);

    // Create a microsecond-resolution timer
    task_timer the_timer;

    // Create the sensor controller object
    sensor_controller my_sensor_control ();

    // Create tasks to control robot's movements
    task_sensors sensor_task (&interval_time, &the_serial_port, &my_sensor_control);
    task_find search_task (&interval_time, &the_serial_port, &sen_control);
    task_avoid avo_task (&interval_time, &my_motor_control, &the_serial_port, &sensor_task);
    task_wander move_task (&interval_time, &my_motor_control, &the_serial_port);

    // Turn on interrupt processing so the timer can work
    sei ();

    // Run the main scheduling loop, in which the tasks are continuously scheduled.
    // This program currently uses very simple "round robin" scheduling in which the
    // tasks are simply called in order. More sophisticated scheduling strategies
    // will be used in other more sophisticated programs
    while (true)
    {
	sensor_task.schedule (the_timer.get_time_now ());
	search_task.schedule (the_timer.get_time_now ());
        avo_task.schedule (the_timer.get_time_now ());
        move_task.schedule (the_timer.get_time_now ());
    }

    return (0);
}
