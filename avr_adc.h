//======================================================================================
/** \file  avr_adc.h
 *  This file contains a very, very simple A/D converter driver. 
 *
 *  Revisions:
 *    \li  01-15-08 JRR Original (somewhat useful) file
 *    \li  02-20-08 CBG Added functionality to make it not hog the processor
 *
 *  License:
 *    This file released under the Lesser GNU Public License. The program is intended
 *    for educational use only, but its use is not restricted thereto. 
 */
//======================================================================================

#ifndef _AVR_ADC_H_                         // To prevent *.h file from being included
#define _AVR_ADC_H_                         // in a source file more than once


//-------------------------------------------------------------------------------------
/** This class should run the A/D converter on an AVR processor. It should have some
 *  better comments. 
 */

class avr_adc
{
    protected:
        // The ADC class needs a pointer to the serial port used to say hello

    public:
        // The constructor just says hello at the moment, using the serial port which
        // is specified in the pointer given to it
        avr_adc (void);

        // This could be a function to read one channel once, returning the result as
        // an unsigned integer. The parameter is the channel number 
        unsigned int read_once (unsigned char);
        
        //this will start a conversion but not wait until it is ready.
        bool startConversion(unsigned char );
         
         //polls to see if conversion is done
        bool convertDone(void);
         
         //grabs the values from the registers
        unsigned int getValue(void);
};

#endif // _AVR_ADC_H_
