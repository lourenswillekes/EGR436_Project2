// I2C.c
// Runs on MSP432
// Provide a function that initializes, sends, and receives
// the eUSCI1B module interfaced with A BME280 pressure sensor
//
// based on Mazidi et.al.
// see http://www.microdigitaled.com/ARM/MSP432_ARM/Code/Ver1/red/Chapter9
//
// modified by RWB 2/9/2018

#include <stdint.h>
#include "msp.h"
#include "I2C.h"

/* configure UCB1 as I2C */
void I2C_Init(void) {

    UCB1CTLW0 = 0x0001;  // hold the eUSCI module in reset mode
    // configure UCB1CTLW0 for:
    // bit15      UCA10 = 0; own address is 7-bit address
    // bit14      UCSLA10 = 0; address slave with 7-bit address
    // bit13      UCMM = 0; single master environment
    // bit12      reserved
    // bit11      UCMST = 1; master mode
    // bits10-9   UCMODEx = 3; I2C mode
    // bit8       UCSYNC = 1; synchronous mode
    // bits7-6    UCSSELx = 2; eUSCI clock SMCLK
    // bit5       UCTXACK = X; transmit ACK condition in slave mode
    // bit4       UCTR = X; transmitter/receiver
    // bit3       UCTXNACK = X; transmit negative acknowledge in slave mode
    // bit2       UCTXSTP = X; transmit stop condition in master mode
    // bit1       UCTXSTT = X; transmit start condition in master mode
    // bit0       UCSWRST = 1; reset enabled
    UCB1CTLW0 = 0x0F81;

    // set the baud rate for the eUSCI which gets its clock from SMCLK
    // Clock_Init48MHz() from ClockSystem.c sets SMCLK = HFXTCLK/4 = 12 MHz
    // if the SMCLK is set to 12 MHz, divide by 120 for 100 kHz baud clock
    UCB1BRW = 120;
    P6SEL0 |= 0x30;
    P6SEL1 &= ~0x30;                   // configure P6.4 and P6.5 as primary module function
    UCB1CTLW0 &= ~0x0001;              // enable eUSCI module
    UCB1IE = 0x0000;                   // disable interrupts
}

/*
 *  subroutine to write bytes to BME280 registers
 */

int8_t I2C_Write_String(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t byteCount)
{
    if (byteCount <= 0)
        return -1;              /* no write was performed */

    EUSCI_B1->I2CSA = dev_addr;      /* setup slave address */
    EUSCI_B1->CTLW0 |= 0x0010;       /* enable transmitter */
    EUSCI_B1->CTLW0 |= 0x0002;       /* generate START and send slave address */
    while((EUSCI_B1->CTLW0 & 2));    /* wait until slave address is sent */
    EUSCI_B1->TXBUF = reg_addr;       /* send memory address to slave */

    /* send data one byte at a time */
    do {
        while(!(EUSCI_B1->IFG & 2));     /* wait till it's ready to transmit */
        EUSCI_B1->TXBUF = *reg_data++;   /* send data to slave */
        byteCount--;
     } while (byteCount > 0);

    while(!(EUSCI_B1->IFG & 2));      /* wait till last transmit is done */
    EUSCI_B1->CTLW0 |= 0x0004;        /* send STOP */
    while(EUSCI_B1->CTLW0 & 4) ;      /* wait until STOP is sent */

    return 0;                   /* no error */
}

/*
 *  subroutine to read bytes from BME280 registers
 */

int8_t I2C_Read_String(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t byteCount)
{
    if (byteCount <= 0)
        return -1;              /* no read was performed */

    EUSCI_B1->I2CSA = dev_addr;     /* setup slave address */
    EUSCI_B1->CTLW0 |= 0x0010;      /* enable transmitter */
    EUSCI_B1->CTLW0 |= 0x0002;      /* generate START and send slave address */
    while((EUSCI_B1->CTLW0 & 2));   /* wait until slave address is sent */
    EUSCI_B1->TXBUF = reg_addr;     /* send memory address to slave */
    while(!(EUSCI_B1->IFG & 2));    /* wait till last transmit is done */
    EUSCI_B1->CTLW0 &= ~0x0010;     /* enable receiver */
    EUSCI_B1->CTLW0 |= 0x0002;      /* generate RESTART and send slave address */
    while(EUSCI_B1->CTLW0 & 2);     /* wait till RESTART is finished */

    /* receive data one byte at a time */
    do {
        if (byteCount == 1)     /* when only one byte of data is left */
            EUSCI_B1->CTLW0 |= 0x0004; /* setup to send STOP after the last byte is received */

        while(!(EUSCI_B1->IFG & 1));    /* wait till data is received */
        *reg_data++ = EUSCI_B1->RXBUF;  /* read the received data */
        byteCount--;
    } while (byteCount);

    while(EUSCI_B1->CTLW0 & 4) ;      /* wait until STOP is sent */

    return 0;                   /* no error */
}
