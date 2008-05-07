//======================================================================================
/** \file  avr_adc.cc
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

#include <stdlib.h>                         // Include standard library header files
#include <avr/io.h>
#include "avr_adc.h"                        // Include header for the A/D class

#define ADC_PRESCALE     0x04               // Default prescaler setting
#define ADC_RETRIES      10000              // Retries before giving up on conversion


//-------------------------------------------------------------------------------------
/** This constructor sets up an A/D converter. How does it do so?
 *  @param p_serial_port A pointer to the serial port which writes debugging info. 
 */
	
avr_adc::avr_adc (void)
{
	ADMUX = 0;                    		// Set reference to AVCC pin voltage
	ADCSRA = (1 << ADEN) | ADC_PRESCALE;    // Set clock prescaler and turn A/D on
}
// end constructor

//-------------------------------------------------------------------------------------
/** This method takes one A/D reading from the given channel, and returns the value
 *  \param  channel The A/D channel which is being read must be from 0 to 7
 *  \return The result of the A/D conversion, or 0xFFFF if there was a timeout
 */

unsigned int avr_adc::read_once (unsigned char channel)
{
    unsigned int counter;                   // Counter waits for timeouts

    // Set the channel
    ADMUX &= 0b11111000;                    // Clear the channel bits
    ADMUX |= (channel & 0x07);              // Put channel bits in the register

    // Start a conversion
    ADCSRA |= (1 << ADSC);

    // Wait until a timeout, or the conversion is finished
    for (counter = 0; ADCSRA & (1 << ADSC); counter++)
    {
        if (counter > ADC_RETRIES)
            return (0xFFFF);
    }
	unsigned int tempL = ADCL;		//grabs low byte first
	unsigned int tempH = ADCH;
	
	return (tempH*0xFF+tempL);
}
    //read_once end
    
//starts the conversion but does not wait
bool avr_adc::startConversion(unsigned char channel)
{
	 // Set the channel
	ADMUX &= 0b11111000;                    // Clear the channel bits
	ADMUX |= (channel & 0x07);              // Put channel bits in the register
	
	 // Start a conversion
	ADCSRA |= (1 << ADSC);
	
	return true;
}
    //end startConversion
    
//simply polls to see if the conversion is done
bool avr_adc::convertDone(void)
{
	bool status = false;			//assume it isnt ready
	
	if(ADCSRA & (1 << ADSC))
	{
	    status = true;			//changes it to true since it was ready
	}
	return status;
}
    //end convert done
    
    //gets the value from the registers
unsigned int avr_adc::getValue(void)
{
	unsigned int tempL = ADCL;		//grabs low byte first
	unsigned int tempH = ADCH;
	
	return (tempH*0xFF+tempL);		//turns the two registers into 1
}
    //gets the value from the registers
