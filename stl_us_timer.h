//*************************************************************************************
/** \file stl_us_timer.h
 *      This file contains a class which runs a task timer whose resolution is one
 *      microsecond. The timer is used to help schedule the execution of tasks' run()
 *      functions. The functions can be run from a main loop by checking periodically 
 *      if a task time has expired, or they can be called directly from the timer 
 *      interrupt service routine in this file, or they can be called from some other 
 *      hardware interrupt service routine; in the last case this file isn't involved. 
 *
 *  Revisions:
 *      \li 08-07-07  JRR  Created this file as daytimer.* with 1 second interrupts
 *      \li 08-08-07  JRR  Added event triggers
 *      \li 12-23-07  JRR  Made more general by allowing faster interrupt rates
 *      \li 01-05-08  JRR  Converted from time-of-day version to microsecond version
 *
 *  License:
 *      This file copyright 2007 by JR Ridgely. It is released under the Lesser GNU
 *      public license, version 2.
 */
//*************************************************************************************

#ifndef _STL_US_TIMER_H_                    // To prevent *.h file from being included
#define _STL_US_TIMER_H_                    // in a source file more than once


//------------------ Macros to be set by user -----------------------------------------

// Exactly one of these macros should be set, corresponding to the speed of the crystal
// oscillator on the AVR processor; this controls the rate of counting of the timer
#undef   SUT_CLOCK_4MHZ
#define  SUT_CLOCK_8MHZ

//--------------- End of stuff the user needs to set ----------------------------------

// The macros in the section after this are for specific processor models
#ifdef __AVR_ATmega8__                      // For the ATmega8 processor
    #ifdef SUT_CLOCK_4MHZ
        #define USEC_PER_COUNT  2
    #endif
    #ifdef SUT_CLOCK_8MHZ
        #define USEC_PER_COUNT  1
    #endif
#endif // __AVR_ATmega8__

#ifdef __AVR_ATmega32__                     // For the ATmega32 processor
    #ifdef SUT_CLOCK_4MHZ
        #define USEC_PER_COUNT  2
    #endif
    #ifdef SUT_CLOCK_8MHZ
        #define USEC_PER_COUNT  1
    #endif
#endif // __AVR_ATMEGA32__

#if defined __AVR_ATmega128__
    #ifdef SUT_CLOCK_4MHZ
        #define USEC_PER_COUNT  2
    #endif
    #ifdef SUT_CLOCK_8MHZ
        #define USEC_PER_COUNT  1
    #endif
#endif // __AVR_ATmega128__

#if defined __AVR_ATmega644__ || defined __AVR_ATmega324P__
    #ifdef SUT_CLOCK_4MHZ
        #define USEC_PER_COUNT  2
    #endif
    #ifdef SUT_CLOCK_8MHZ
        #define USEC_PER_COUNT  1
    #endif
#endif // __AVR_ATmega644__ || __AVR_ATmega324P__


//--------------------------------------------------------------------------------------
/** This union holds a 32-bit time count. The count can be accessed as a single 32-bit
 *  number, as two 16-bit integers placed together, or as an array of 8-bit characters. 
 *  This is helpful because the time measurement we use consists of the number in a 
 *  16-bit hardware counter and a 16-bit overflow count which should be put together 
 *  into one time number. The characters are handy for looking at individual bits. 
 */

typedef union
    {
    long whole;                             // This union contains a time stamp as
    int half[2];                            // one set of data in three forms: a 
    char quarters[4];                       // 32-bit long integer, two 16-bit 
    } time_data_32;                         // integers or four 8-bit characters


//--------------------------------------------------------------------------------------
/** This class holds a time stamp which is used to measure the passage of real time in
 *  the world around an AVR processor. This version of the time stamp implements a 
 *  32-bit time counter that runs at one megahertz. There's a 16-bit number which is
 *  copied directly from a 16-bit hardware counter and another 16-bit number which is 
 *  incremented every time the hardware counter overflows; the combination of the two 
 *  is a 32-bit time measurement. 
 */

class time_stamp
    {
    protected:
        time_data_32 data;                  // Holds the time stamp's data

    public:
        time_stamp (void);                  // Constructor creates empty time stamp

        // This constructor creates a time stamp and initializes all its data
        time_stamp (long);

        // This constructor creates a time stamp with the given seconds and microsec.
        time_stamp (int, long);

        // This method fills the timestamp with the given value
        void set_time (long);

        // This method fills the timestamp with the given seconds and microseconds
        void set_time (int, long);

        // This method reads out all the timestamp's data at once
        void get_time (long&);

        // This overloaded addition operator adds two time stamps together
        void operator += (const time_stamp&);

        // This overloaded subtraction operator finds the time between two time stamps
        void operator -= (const time_stamp&);

        // This overloaded equality operator tests if all time fields are the same
        bool operator == (const time_stamp&);

        // This overloaded operator tests if a time stamp is greater (later) than this
        bool operator >= (const time_stamp&);

        // This method writes the currently held time into a character string
        void to_string (char*, unsigned char = 5);
        
        // This declaration gives permission for objects of class task_timer to access
        // the private and/or protected data belonging to objects of this class
        friend class task_timer;
    };


//--------------------------------------------------------------------------------------
/** This class implements a timer to synchronize the operation of tasks on an AVR. The
 *  timer is implemented as a combination of a 16-bit hardware timer (Timer 1 is the 
 *  usual choice) and a 16-bit overflow counter. The two timers' data is combined to
 *  produce a 32-bit time count which is used to decide when tasks run. WARNING: This
 *  timer does not keep track of the time of day, and it overflows after a little more
 *  than an hour of use. Another timer (see stl_timer.h/cc) works when longer time
 *  periods need to be kept track of, to lower precision. 
 */

class task_timer
    {
    protected:
        time_stamp now_time;                // Holds the current time

    public:
        task_timer (void);                  // Constructor creates an empty timer

        void save_time_stamp (time_stamp&); // Save current time in a timestamp

        time_stamp& get_time_now (void);    // Get the current time

        // This method sets the current time to the time in the given time stamp
        bool set_time (time_stamp&);
    };

#endif  // _STL_US_TIMER_H_
