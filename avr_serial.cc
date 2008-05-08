//*************************************************************************************
/** \file avr_serial.cc
 *        This file contains functions which allow the use of a serial port on an AVR 
 *        microcontroller.  Compatibility macros are provided to isolate the names of
 *        various registers from the many specific AVR device types.
 *
 *        This code is designed to work for low-performance applications without
 *        requiring the use of interrupts.  Interrupt based receiving code has not
 *        been completed or tested.
 *
 *  Revised:
 *      \li 04-03-06  JRR  For updated version of compiler
 *      \li 06-10-06  JRR  Ported from C++ to C for use with some C-only projects; also
 *                         the serial_avr.h header has been stuffed with defines for
 *                         compatibility among lots of AVR variants
 *      \li 08-11-06  JRR  Some bug fixes
 *      \li 03-02-07  JRR  Ported back to C++. I've had it with the limitations of C.
 *      \li 04-16-07  JO   Added write (unsigned long)
 *      \li 07-19-07  JRR  Changed some character return values to bool, added m324p
 *      \li 12-18-07  JRR  Added write (unsigned long long) and CTS flow control
 */
//*************************************************************************************

#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include "avr_serial.h"


//-------------------------------------------------------------------------------------
/** This method sets up the AVR UART for communications.  It enables the appropriate 
 *  inputs and outputs and sets the baud rate divisor.
 *  @param divisor The baud rate divisor to be used for controlling the rate of
 *      communication.  See the *.h file in which various values of the divisor are 
 *      defined as macros. 
 *  @param a_CTS_mask This is a bitmask for the Clear To Send flow control bit.  If
 *      this bitmask is 0 or left off, CTS flow control will not be used.
 */

avr_uart::avr_uart (unsigned char divisor, unsigned char a_CTS_mask = 0)
    {
    CTS_mask = a_CTS_mask;                  // Save the Clear To Send bitmask

    if (a_CTS_mask)
        {                                   // If the CTS line is being used
        UART_CTS_DDR &= ~CTS_mask;          // Set the CTS pin to be an input
        UART_CTS_PORT &= ~CTS_mask;         // Turn off the pullup on the CTS line
        }

    UART_modeN81 ();                        // Set No parity, 8 data bits, 1 stop bit
    #ifdef UART_DOUBLE_SPEED                // If double-speed macro has been defined,
        UART_STATUS |= 0x02;                // Turn on double-speed operation
    #endif
    UART_set_baud_div (divisor);            // Set the baud rate divisor

    UART_tx_rx_on ();                       // Enable transmitter and receiver of UART

    // If serial interrupts are enabled, turn on serial port interrupt
    #ifdef UART_USE_INTERRUPTS
        UART_CONTROL |= RXCIE;            //    sbi (UCR, RXCIE);    // Old style
    #endif
    }


//-------------------------------------------------------------------------------------
/** This function checks if the serial port transmitter is ready to send data.  It 
 *  tests whether the CTS bit is low (if CTS is in use) and whether the transmitter 
 *  buffer is empty. 
 *  @return True if the serial port is ready to send, and false if not
 */

bool avr_uart::ready_to_send (void)
    {
    // If CTS is being used and it's high, we're not ready to send
    if (CTS_mask && (UART_CTS_PORT & CTS_mask))
        return (false);

    // If transmitter buffer is full, we're not ready either
    if (UART_STATUS & UART_DREG_MT)
        return (false);

    return (true);
    }


//-------------------------------------------------------------------------------------
/** This method sends one character to the serial port.  It waits until the port is
 *  ready, so it can hold up the system for a while.  It times out if it waits too 
 *  long to send the character; you can check the return value to see if the character
 *  was successfully sent, or just cross your fingers and ignore the return value.
 *  Note 1:  It's possible that at slower baud rates and/or higher processor speeds, 
 *  this routine might time out even when the port is working fine.  A solution would
 *  be to change the count variable to an integer and use a larger starting number. 
 *  Note 2:  Fixed!  The count is now an integer and it works at lower baud rates.
 *  @param chout The character to be sent out
 *  @return True if everything was OK and false if there was a timeout
 */

bool avr_uart::putchar (char chout)
    {
    unsigned int count = 0;                 // Timeout counter

    // If the CTS mask is not zero, wait for the CTS pin to be low (ready for data)
    if (CTS_mask)
        {
        for (count = 0; (UART_CTS_PORT & CTS_mask); count++)
            {
            if (count > UART_TX_TOUT)
                return (false);
            }
        }

    // Now wait for the serial port transmitter buffer to be empty     
    for (count = 0; ((UART_STATUS & UART_DREG_MT) == 0); count++)
        {
        if (count > UART_TX_TOUT)
            return (false);
        }

    // The CTS line is 0 and the transmitter buffer is empty, so send the character
    UART_DATA = chout;
    return (true);
    }


//-------------------------------------------------------------------------------------
/** This method writes all the characters in a string until it gets to the '\\0' at 
 *  the end. Warning: This function blocks until it's finished. 
 *  @param str The string to be written 
 */

void avr_uart::puts (char const* str)
    {
    while (*str) putchar (*str++);
    }


//-------------------------------------------------------------------------------------
/** This method gets one character from the serial port, if one is there.  If not, it
 *  waits until there is a character available.  This can sometimes take a long time
 *  (even forever), so use this function carefully.  It's generally safer to use
 *  check_for_char() to ensure that there's data available first. 
 *  @return The character which was found in the serial port receive buffer
 */

char avr_uart::getchar (void)
    {
    //  Wait until there's something in the receiver buffer
    while ((UART_STATUS & UART_RX_CPT) == 0);

    //  Return the character retreived from the buffer
    return (UART_DATA);
    }


//-------------------------------------------------------------------------------------
/** This method gets a character from the serial port, trying the specified number of
 *  times to find a character in the receiver buffer. If a character is found, that
 *  character is returned; if no character is found, a (-1) is returned. Don't use
 *  this method if 0xFF (or -1) is required as a valid character.
 *  @param retries The number of times to look for a character before timing out, in
 *                 an unsigned int which can be from 1 to 65535
 *  @return The character which was received, or (-1) if no character was found
 */

char avr_uart::getch_timeout (unsigned int retries)
    {
    for (unsigned int timeout = 0; timeout < retries; timeout++)
        {
        if (UART_STATUS & UART_RX_CPT)
            return (UART_DATA);             // Aha, a character was found
        }
    return (0xFF);                          // Nothing was found; return error code
    }


//-------------------------------------------------------------------------------------
/** This function checks if there is a character in the serial port's receiver buffer.
 *  It returns 1 if there's a character available, and 0 if not. 
 *  @return True for character available, false for no character available
 */

bool avr_uart::check_for_char (void)
    {
    if (UART_STATUS & UART_RX_CPT)
        return (true);
    else
        return (false);
    }


//-------------------------------------------------------------------------------------
/** This method writes boolean value to the serial port as a character, either "T"
 *  or "F" depending on the value. 
 *  @param value The boolean value to be written
 */

void avr_uart::write (bool value)
    {
    if (value)
        putchar ('T');
    else
        putchar ('F');
    }


//-------------------------------------------------------------------------------------
/** This method writes a character to the serial port as a text string showing the 
 *  8-bit unsigned number in that character in binary form. 
 *  @param num The 8-bit number to be sent out
 */

void avr_uart::write_bin (unsigned char num)
    {
    unsigned char bmask;                            // Bit mask

    for (bmask = 0x80; bmask != 0; bmask >>= 1)
        {
        if (num & bmask) putchar ('1');
        else             putchar ('0');
        }
    }


//-------------------------------------------------------------------------------------
/** This method writes a character to the serial port as a text string showing the 
 *  8-bit unsigned number in that character in hexadecimal form. 
 *  @param num The 8-bit number to be sent out
 */

void avr_uart::write_hex (unsigned char num)
    {
    unsigned char dcnt;                             // Counts digits to be displayed
    unsigned char digit;                            // 4-bit number to be displayed

    for (dcnt = 0; dcnt < 2; dcnt++)
        {
        digit = (num & 0xF0) >> 4;
        if (digit > 9)
            putchar (digit + ('A' - 10));      // Write a character 'A' - 'F'
        else
            putchar (digit + '0');

        num <<= 4;
        }
    }


//-------------------------------------------------------------------------------------
/** This method writes a character to the serial port as a text string showing the 
 *  8-bit unsigned number in that character. 
 *  @param num The 8-bit number to be sent out
 */

void avr_uart::write (unsigned char num)
    {
    char out_str[5];
    utoa ((unsigned int)num, out_str, 10);
    puts (out_str);
    }


//-------------------------------------------------------------------------------------
/** This method writes a character to the serial port as a text string showing the 
 *  8-bit signed number in that character. 
 *  @param num The 8-bit number to be sent out
 */

void avr_uart::write (char num)
    {
    char out_str[5];
    itoa ((int)num, out_str, 10);
    puts (out_str);
    }


//-------------------------------------------------------------------------------------
/** This method writes an integer to the serial port as a text string showing the 
 *  16-bit unsigned number in that character in binary form. 
 *  @param num The 16-bit number to be sent out
 */

void avr_uart::write_bin (unsigned int num)
    {
    unsigned int bmask;                             // Bit mask

    for (bmask = 0x8000; bmask != 0; bmask >>= 1)
        {
        if (num & bmask) putchar ('1');
        else             putchar ('0');
        }
    }


//-------------------------------------------------------------------------------------
/** This method writes an integer to the serial port as a text string showing the 
 *  16-bit unsigned number in that integer in hexadecimal notation. 
 *  @param num The 16-bit number to be sent out
 */

void avr_uart::write_hex (unsigned int num)
    {
    unsigned char dcnt;                             // Counts digits to be displayed
    unsigned char digit;                            // 4-bit number to be displayed

    for (dcnt = 0; dcnt < 4; dcnt++)
        {
        digit = (unsigned char)((num & 0xF000) >> 12);
        if (digit > 9)
            putchar (digit + ('A' - 10));           // Write a character 'A' - 'F'
        else
            putchar (digit + '0');

        num <<= 4;
        }
    }


//-------------------------------------------------------------------------------------
/** This method writes an integer to the serial port as a text string showing the 
 *  16-bit unsigned number in that integer. 
 *  @param num The 16-bit number to be sent out
 */

void avr_uart::write (unsigned int num)
    {
    char out_str[7];
    utoa ((unsigned int)num, out_str, 10);
    puts (out_str);
    }


//-------------------------------------------------------------------------------------
/** This method writes an integer to the serial port as a text string showing the 
 *  16-bit signed number in that integer. 
 *  @param num The 16-bit number to be sent out
 */

void avr_uart::write (int num)
    {
    char out_str[7];
    itoa ((int)num, out_str, 10);
    puts (out_str);
    }


//-------------------------------------------------------------------------------------
/** This method writes a long integer to the serial port as a text string showing the 
 *  32-bit signed number in that long integer. 
 *  @param num The 32-bit number to be sent out
 */

void avr_uart::write (long num)
    {
    char out_str[12];
    ltoa ((long)num, out_str, 10);
    puts (out_str);
    }


//-------------------------------------------------------------------------------------
/** This method writes an unsigned long integer to the serial port as a text string 
 *  showing the 32-bit unsigned number in that long integer. 
 *  @param num The 32-bit number to be sent out
 */

void avr_uart::write (unsigned long num)
    {
    char out_str[12];
    ultoa ((long)num, out_str, 10);
    puts (out_str);
    }


//-------------------------------------------------------------------------------------
/** This method writes a long integer to the serial port as a text string showing the 
 *  32-bit unsigned number in that long integer. 
 *  @param num The 32-bit number to be sent out
 */


void avr_uart::write_hex (unsigned long num)
    {
    unsigned int half;

    half = (unsigned int)(num >> 16);
    write_hex (half);
    half = (unsigned int)num;
    write_hex (half);
    }


//-------------------------------------------------------------------------------------
/** This method writes a 64-bit integer to the serial port as a text string showing the 
 *  number as 16 hexadecimal digits. 
 *  @param num The 64-bit number to be sent out
 */

void avr_uart::write_hex (unsigned long long num)
    {
    union onion64                           // The union allows the same data to be
        {                                   // treated as either one 64-bit number
        unsigned long long num64;           // or eight 8-bit numbers
        unsigned char bytes[8];
        } the_num;
    the_num.num64 = num;                    // Copy the number into the 

    char dcnt = 7;
    do
        write_hex (the_num.bytes[dcnt--]);
    while (dcnt >= 0);
    }
