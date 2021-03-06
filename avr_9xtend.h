//*************************************************************************************
/** \file avr_9xtend.h
 *      This file implements a class which extends the AVR serial port object to 
 *      operate a MaxStream 9XStream(tm) radio modem connected to the serial port.
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
 *      \li 12-18-07  JRR  Merged lots of stufADCf with avr_serial for efficiency
 */
//*************************************************************************************

#ifndef _AVR_9XTEND_H_                    // To prevent *.h file from being included
#define _AVR_9XTEND_H_                    // in a source file more than once

/** This class communicates with the 9XTend radio. It is a descendent of the uart
 *  class from avr_serial.*. It operates the serial port with a baud rate which is 
 *  set in avr_9xtend.h.
 */

class avr_9xtend : public avr_uart
{
    protected:
        unsigned char sleep_mask;           // Bitmask for the sleep bit
        volatile unsigned int timeout;      // Counter for software timeouts
        bool setup_error;                   // Flag for problems setting up radio

    public:
        // The constructor creates the radio modem object and assigns port pins
        avr_9xtend (unsigned char, unsigned char, unsigned char);

//         void sleep (void);                  // Put the radio into sleep mode
//         void wake_up (void);                // Awaken the radio modem

        /** This method returns the number of timeout errors which have occurred. */
        unsigned char timeouts (void) { return timeout; }
};

#endif  // _AVR_9XTEND_H_
