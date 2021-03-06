/*
 * adc_uart.c
 *
 *  Created on: Oct 9, 2017
 *      Author: Keifer Bowen
 *
 *  This code has been very slightly modified from lab 3, but has
 *  mostly remained the same. This code configures our UART for
 *  transmission of 115200 bps. It also allows us to define
 *  how we send bytes and n-bytes of data to a terminal.
 *
 */

#include "msp.h"
#include <stdint.h>
//#include "circbuf.h"
#include <uart.h>

// make some macros for configuring our BAUD rate
#define CR (0x80)
#define br0 (0x01)
#define br1 (0x00)

// this function allows us to configure our BAUD rate to 115,200 bps.
void adc_UART_Configure() {
    UCA0CTLW0 |= UCSWRST;  //begin initialization process for BAUD Rate 115200
    P1SEL0 |= BIT2 | BIT3;  // configure pins 1.2 and 1.3 to primary output mode, UART
    P1SEL1 &= ~(BIT2 | BIT3);
    UCA0CTLW0 |= EUSCI_A_CTLW0_SSEL__SMCLK;
    UCA0CTLW0 &= ~EUSCI_A_CTLW0_PEN;   // parity disabled
    UCA0CTLW0 &= ~EUSCI_A_CTLW0_MODE0; // set to uart mode
    UCA0CTLW0 &= ~EUSCI_A_CTLW0_MODE1;
    UCA0CTLW0 &= ~EUSCI_A_CTLW0_MSB;  //lsb first
    UCA0CTLW0 &= ~EUSCI_A_CTLW0_SEVENBIT; //8 bit data character length
    UCA0CTLW0 &= ~EUSCI_A_CTLW0_SPB;  // one stop bit one start bit is default
    UCA0MCTLW = 0xB5A1;
    UCA0BR0 = br0;     // set baud rate
    UCA0BR1 = br1;
    UCA0CTLW0 &= ~UCSWRST; // initialize eUSCI
    // configure interrupts for RX and TX
    EUSCI_A0->IE |= EUSCI_A_IE_RXIE;
    // enable the stuff that makes stuff happen when other stuff happens
    __NVIC_EnableIRQ(EUSCIA0_IRQn);
}

// This function allows us to send n chars in a string when
// provided an array of chars and its associated length
void UART_send_n(uint8_t * data_array, uint32_t length) {
    //needs to send data if the transmission buffer is empty
    uint8_t i = 0;
    for(i = 0 ; i < length ; i++) {
        if(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG) {
            adc_UART_send_byte(data_array[i]);
        }
        else
            // otherwise dont increment past the char we didn't transmit
            i--;
    }
}
// This function sends a single byte of data out of the UART channel
void UART_send_byte(uint8_t data) {
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
    //send data if flag is ready
    EUSCI_A0 -> TXBUF = data;
}
