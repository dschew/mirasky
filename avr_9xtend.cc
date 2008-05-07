//**************************************************************************************
/** \file avr_9xtend.cc
 *      This file implements a class which extends the AVR serial port object to 
 *      operate a MaxStream 9XTend(tm) radio modem connected to the serial port.
 *      The Clear To Send line is used for flow control (it's necessary for 
 *      reliability); the sleep line can optionally be used to save power. 
 *
 *  Revised:
 *      \li 04-03-06  JRR  For updated version of compiler
 *      \li 06-10-06  JRR  Ported from C++ to C for use with some C-only projects; also
 *                         the serial_avr.h header has been stuffed with defines for
 *                         compatibility among lots of AVR variants
 *      \li 08-11-06  JRR  Some bug fixes
 *      \li 03-02-07  JRR  Ported back to C++. I've had it with the limitations of C.
 *      \li 04-16-07  JO   Added write (unsigned long)
 *      \li 12-16-07  JRR  Changed puts() to (char const*) to shut up a GCC warning
 *      \li 12-18-07  JRR  Merged lots of stuff with avr_serial for efficiency
 *	\li 05-05-08  DSC  Modifying radio modem class to operate XTend module
 */
//**************************************************************************************

#include <avr/io.h>
#include <stdlib.h>
#include "avr_serial.h"
#include "avr_9xtend.h"


//--------------------------------------------------------------------------------------
/** This constructor creates a radio modem object by calling the serial port 
 *  constructor setting up the input and output pins for CTS and sleep mode, and
 *  sending the correct setup codes to the radio modem. Due to the need for dumb delay
 *  loops to satisfy the radio's timing requirements, this constructor takes several
 *  seconds to execute. 
 *  @param a_divisor A baud rate divisor to give to the UART constructor
 *  @param cts_bitmask Bitmask for the Clear To Send flow control bit
 *  @param sleep_bitmask Bitmask for the sleep control line (default 0, sleep unused)
 */

avr_9xtend::avr_9xtend (unsigned char a_divisor, unsigned char cts_bitmask,
    unsigned char sleep_bitmask = 0) 
    : avr_uart (a_divisor, cts_bitmask)
{
    sleep_mask = sleep_bitmask;             // Save the bitmask for the sleep bit

    setup_error = false;

// Set bitmask for Clear To Send flow control bit


    // If sleep line is used, set up the sleep pin as an output and awaken radio
//     A9XS_SLEEP_DDR |= sleep_mask;
//     A9XS_SLEEP_PRT &= ~sleep_mask;

    // Put the radio modem into command mode and tell it to use sleep mode 1, the mode
    // in which pin 2 controls whether it's asleep or awake. Dumb delay loops are used
    // to ensure that the "guard times" for the radio are satisfied
//     for (volatile long dumbdelay = 0L; dumbdelay < 100000L; dumbdelay++);
//     puts ("+++");
//     if (getchar () != 'O')  setup_error = true;
//     if (getchar () != 'K')  setup_error = true;
//     if (getchar () != '\r') setup_error = true;
//     puts ("ATSM1,WR,CN\r\n");
//     if (getchar () != 'O')  setup_error = true;
//     if (getchar () != 'K')  setup_error = true;
//     if (getchar () != '\r') setup_error = true;

    // Even though nobody's likely to see this, we can at least try to complain
    if (setup_error) puts ("Error setting up radio modem\r\n");
//  else             puts ("Radio modem setup OK\r\n");
}


//--------------------------------------------------------------------------------------
/** This method causes the radio to enter sleep mode. The sleep mode used is "pin 
 *  sleep", in which the radio consumes about 25 uA of current and wakes up only when
 *  its sleep pin (pin 2) is set to 0. If the sleep pin isn't being used, this method
 *  does nothing. 
 */

// void avr_9xtend::sleep (void)
// {
//     A9XS_SLEEP_PRT |= sleep_mask;
// }


//--------------------------------------------------------------------------------------
/** This method awakens the radio from sleep mode by de-asserting the sleep pin. If 
 *  the sleep pin isn't being used, this method does nothing. 
 */

// void avr_9xtend::wake_up (void)
// {
//     A9XS_SLEEP_PRT &= ~sleep_mask;
// }
