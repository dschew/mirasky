//*************************************************************************************
/** \file avr_serial.h
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
 */
//*************************************************************************************

#ifndef _AVR_SERIAL_H_                      // To prevent *.h file from being included
#define _AVR_SERIAL_H_                      // in a source file more than once


//-------------------------------------------------------------------------------------
// This section contains macros to define the various I/O ports and bits on those 
// ports which are used to operate the serial interface.  The problem being addressed 
// is that in various different AVR processors, there are various different names for 
// the I/O ports connected to the serial interface; on some newer versions of the 
// chip there are ports which don't even exist on the older ones.  What a pain. :oP

#ifdef __AVR_AT90S2313__                // For the AT90S2313 processor
    #define UART_DATA       UDR         // UART data register

    #define UART_STATUS     USR         // UART status register
    #define UART_RX_CPT     0x80        // Receive complete bit
    #define UART_TX_CPT     0x40        // Transmission complete bit
    #define UART_DREG_MT    0x20        // UART data register empty bit
    #define UART_FRAME_ERR  0x10        // Framing error bit
    #define UART_OVRRN_ERR  0x08        // Overrun error bit

    #define UART_CONTROL    UCR         // UART control register
    #define UART_RCV_IE     0x80        // Receive complete interrupt enable
    #define UART_TXC_IE     0x40        // Transmit complete interrupt enable
    #define UART_DRMT_IE    0x20        // Data register empty interrupt enable
    #define UART_RX_EN      0x10        // UART receiver enable bit
    #define UART_TX_EN      0x08        // UART transmitter enable bit

    #define UART_BAUD_HI    (ERROR)     // No baud rate high for AT90S2313
    #define UART_BAUD_LOW   UBRR        // Low (only) byte of baud divisor

    // Macro sets mode to async, 8 bit, no parity, 1 stop bit
    #define UART_modeN81()  UCR = 0x18
    
    // Macro to set baud rate divisor
    #define UART_set_baud_div(x) UBRR = (x)
    
    // Macro to turn transmitter only on (no interrupts)
    #define UART_tx_only_on()  UCR = 0x08
    
    // Macro to turn receiver only on (no interrupts)
    #define UART_rx_only_on()  UCR = 0x10
    
    // Macro to turn transmitter and receiver both on (no interrupts)
    #define UART_tx_rx_on()  UCR = 0x18
    
    // Macro to turn transmitter and receiver both off (no interrupts)
    #define UART_tx_rx_off()  UCR = 0x00
#endif

#ifdef __AVR_ATmega8__                 // For the ATMega8 processor
    #define UART_DATA       UDR         // USART data register

    #define UART_STATUS     UCSRA       // USART status register
    #define UART_RX_CPT     0x80        // Receive complete bit
    #define UART_TX_CPT     0x40        // Transmission complete bit
    #define UART_DREG_MT    0x20        // UART data register empty bit
    #define UART_FRAME_ERR  0x10        // Framing error bit
    #define UART_OVRRN_ERR  0x08        // Overrun error bit
    #define UART_PAR_ERR    0x04        // Parity error bit
    #define UART_2_SPEED    0x02        // Double-speed bit
    #define UART_MULPROC    0x01        // Multi-processor comm. mode bit

    #define UART_CONTROL    UCSRB       // UART control register
    #define UART_RCV_IE     0x80        // Receive complete interrupt enable
    #define UART_TXC_IE     0x40        // Transmit complete interrupt enable
    #define UART_DRMT_IE    0x20        // Data register empty interrupt enable
    #define UART_RX_EN      0x10        // UART receiver enable bit
    #define UART_TX_EN      0x08        // UART transmitter enable bit

    #define UART_BAUD_HI    UBRRH       // High byte of baud rate divisor
    #define UART_BAUD_LOW   UBRRL       // Low (only) byte of baud divisor

    // Macro sets mode to async, 8 bit, no parity, 1 stop bit
    #define UART_modeN81()  UBRRH = 0x00; UCSRC = 0x86

    // Macro to set baud rate divisor
    #define UART_set_baud_div(x)  UBRRL = (x)
    
    // Macro to turn transmitter only on (no interrupts)
    #define UART_tx_only_on()  UCSRB = 0x08
    
    // Macro to turn receiver only on (no interrupts)
    #define UART_rx_only_on()  UCSRB = 0x10
    
    // Macro to turn transmitter and receiver both on (no interrupts)
    #define UART_tx_rx_on()  UCSRB = 0x18
    
    // Macro to turn transmitter and receiver both off (no interrupts)
    #define UART_tx_rx_off()  UCSRB = 0x00
#endif

#ifdef __AVR_ATmega8535__               // For the old ATMega8535 processor
    #define UART_DATA       UDR         // USART data register

    #define UART_STATUS     UCSRA       // USART status register
    #define UART_RX_CPT     0x80        // Receive complete bit
    #define UART_TX_CPT     0x40        // Transmission complete bit
    #define UART_DREG_MT    0x20        // UART data register empty bit
    #define UART_FRAME_ERR  0x10        // Framing error bit
    #define UART_OVRRN_ERR  0x08        // Overrun error bit
    #define UART_PAR_ERR    0x04        // Parity error bit
    #define UART_2_SPEED    0x02        // Double-speed bit
    #define UART_MULPROC    0x01        // Multi-processor comm. mode bit

    #define UART_CONTROL    UCSRB       // UART control register
    #define UART_RCV_IE     0x80        // Receive complete interrupt enable
    #define UART_TXC_IE     0x40        // Transmit complete interrupt enable
    #define UART_DRMT_IE    0x20        // Data register empty interrupt enable
    #define UART_RX_EN      0x10        // UART receiver enable bit
    #define UART_TX_EN      0x08        // UART transmitter enable bit

    #define UART_BAUD_HI    UBRRH       // High byte of baud rate divisor
    #define UART_BAUD_LOW   UBRRL       // Low (only) byte of baud divisor

    // Macro sets mode to async, 8 bit, no parity, 1 stop bit
    #define UART_modeN81()  UBRRH = 0x00; UCSRC = 0x86

    // Macro to set baud rate divisor
    #define UART_set_baud_div(x)  UBRRL = (x)
    
    // Macro to turn transmitter only on (no interrupts)
    #define UART_tx_only_on()  UCSRB = 0x08
    
    // Macro to turn receiver only on (no interrupts)
    #define UART_rx_only_on()  UCSRB = 0x10
    
    // Macro to turn transmitter and receiver both on (no interrupts)
    #define UART_tx_rx_on()  UCSRB = 0x18
    
    // Macro to turn transmitter and receiver both off (no interrupts)
    #define UART_tx_rx_off()  UCSRB = 0x00
#endif // __AVR_ATmega8535__

#ifdef __AVR_ATmega32__                 // For the ATMega32 processor
    #define UART_DATA       UDR         // USART data register

    #define UART_STATUS     UCSRA       // USART status register
    #define UART_RX_CPT     0x80        // Receive complete bit
    #define UART_TX_CPT     0x40        // Transmission complete bit
    #define UART_DREG_MT    0x20        // UART data register empty bit
    #define UART_FRAME_ERR  0x10        // Framing error bit
    #define UART_OVRRN_ERR  0x08        // Overrun error bit
    #define UART_PAR_ERR    0x04        // Parity error bit
    #define UART_2_SPEED    0x02        // Double-speed bit
    #define UART_MULPROC    0x01        // Multi-processor comm. mode bit

    #define UART_CONTROL    UCSRB       // UART control register
    #define UART_RCV_IE     0x80        // Receive complete interrupt enable
    #define UART_TXC_IE     0x40        // Transmit complete interrupt enable
    #define UART_DRMT_IE    0x20        // Data register empty interrupt enable
    #define UART_RX_EN      0x10        // UART receiver enable bit
    #define UART_TX_EN      0x08        // UART transmitter enable bit

    #define UART_BAUD_HI    UBRRH       // High byte of baud rate divisor
    #define UART_BAUD_LOW   UBRRL       // Low (only) byte of baud divisor

    // Macro sets mode to async, 8 bit, no parity, 1 stop bit
    #define UART_modeN81()  UBRRH = 0x00; UCSRC = 0x86

    // Macro to set baud rate divisor
    #define UART_set_baud_div(x)  UBRRL = (x)

    // Macro to turn transmitter only on (no interrupts)
    #define UART_tx_only_on()  UCSRB = 0x08

    // Macro to turn receiver only on (no interrupts)
    #define UART_rx_only_on()  UCSRB = 0x10

    // Macro to turn transmitter and receiver both on (no interrupts)
    #define UART_tx_rx_on()  UCSRB = 0x18

    // Macro to turn transmitter and receiver both off (no interrupts)
    #define UART_tx_rx_off()  UCSRB = 0x00
#endif // __AVR_ATmega32__

#if (defined __AVR_ATmega644__ || defined __AVR_ATmega324P__)
    #define UART_DATA       UDR0        // USART data register

    #define UART_STATUS     UCSR0A      // USART status register
    #define UART_RX_CPT     0x80        // Receive complete bit
    #define UART_TX_CPT     0x40        // Transmission complete bit
    #define UART_DREG_MT    0x20        // UART data register empty bit
    #define UART_FRAME_ERR  0x10        // Framing error bit
    #define UART_OVRRN_ERR  0x08        // Overrun error bit
    #define UART_PAR_ERR    0x04        // Parity error bit
    #define UART_2_SPEED    0x02        // Double-speed bit
    #define UART_MULPROC    0x01        // Multi-processor comm. mode bit

    #define UART_CONTROL    UCSR0B      // UART control register
    #define UART_RCV_IE     0x80        // Receive complete interrupt enable
    #define UART_TXC_IE     0x40        // Transmit complete interrupt enable
    #define UART_DRMT_IE    0x20        // Data register empty interrupt enable
    #define UART_RX_EN      0x10        // UART receiver enable bit
    #define UART_TX_EN      0x08        // UART transmitter enable bit

    #define UART_BAUD_HI    UBRR0H      // High byte of baud rate divisor
    #define UART_BAUD_LOW   UBRR0L      // Low (only) byte of baud divisor

    // Macro sets mode to async, 8 bit, no parity, 1 stop bit
    #define UART_modeN81()  UBRR0H = 0x00; UCSR0C = 0x86

    // Macro to set baud rate divisor
    #define UART_set_baud_div(x)  UBRR0L = (x)
    
    // Macro to turn transmitter only on (no interrupts)
    #define UART_tx_only_on()  UCSR0B = 0x08
    
    // Macro to turn receiver only on (no interrupts)
    #define UART_rx_only_on()  UCSR0B = 0x10
    
    // Macro to turn transmitter and receiver both on (no interrupts)
    #define UART_tx_rx_on()  UCSR0B = 0x18
    
    // Macro to turn transmitter and receiver both off (no interrupts)
    #define UART_tx_rx_off()  UCSR0B = 0x00
#endif

/** The number of tries to wait for the transmitter buffer to become empty */
#define UART_TX_TOUT        20000

/** The input port (PORTA, PORTB, etc.) used for the CTS pin(s), if they are used.
 *  This must match the data direction register in UART_CTS_DDR */
#define UART_CTS_PORT       PIND

/** The data direction register (DDRA, DDRB, etc.) for the CTS pin(s). This must match
 *  the I/O port specified in UART_CTS_PORT */
#define UART_CTS_DDR        DDRD


//-------------------------------------------------------------------------------------
/** This class controls a UART (Universal Asynchronous Receiver Transmitter), a common 
 *  serial interface. It talks to old-style RS232 serial ports (through a voltage
 *  converter chip such as a MAX232) or through a USB to serial converter such as a
 *  FT232RL chip. The UART is also sometimes used to communicate directly with other
 *  microcontrollers, sensors, or wireless modems. 
 *
 *  This class has originally been written for AVR processors which have only one
 *  UART, but it should be extendable for use with processors which have dual UARTs. 
 */

class avr_uart
    {
    // Private data and methods are accessible only from within this class and 
    // cannot be accessed from outside -- even from descendents of this class
    private:

    // Protected data and methods are accessible from this class and its descendents
    // only
    protected:
        unsigned char CTS_mask;             // Bitmask for the CTS flow control bit

        bool transmitter_empty (void);      // Check if transmitter buffer is empty

    // Public methods can be called from anywhere in the program where there is a 
    // pointer or reference to an object of this class
    public:
        // The constructor sets up the UART, saving its location, CTS bit, etc.
        avr_uart (unsigned char, unsigned char);
        bool ready_to_send (void);          // Check if the port is ready to transmit
        bool putchar (char);                // Write one character to serial port
        void puts (char const*);            // Write a string constant to serial port
        bool check_for_char (void);         // Check if a character is in the buffer
        char getchar (void);                // Get a character; wait if none is ready
        char getch_timeout (unsigned int);  // Get a character unless we time out

        void write (bool);                  //
        void write_bin (unsigned char);     // 
        void write_hex (unsigned char);     // These functions write various sizes of
        void write (unsigned char);         // integers to the serial port as text
        void write (char num);              // strings. They're used whenever you 
        void write_bin (unsigned int);      // need to show the numerical values of
        void write_hex (unsigned int);      // numbers rather than the ASCII 
        void write (unsigned int);          // characters which the numbers represent
        void write (int);                   // 
        void write_hex (unsigned long);
        void write (unsigned long);
        void write (long);
        void write_hex (unsigned long long);
    };

#endif  // _AVR_SERIAL_H_
