/*
 * UART_driver.c
 *
 *  Created on: March 2018
 *      Author: lourw
 */

#include "UART_driver.h"

// --- UART Configuration Parameter ---
const eUSCI_UART_Config uartConfig =
{
        // Configured for 115200 Baud Rate off of 12 MHz clock
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,                 // SMCLK Clock Source
        6,                                              // BRDIV
        8,                                              // UCxBRF
        17,                                             // UCxBRS
        EUSCI_A_UART_NO_PARITY,                         // No Parity
        EUSCI_A_UART_LSB_FIRST,                         // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,                      // One stop bit
        EUSCI_A_UART_MODE,                              // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION   // BRDIV > 16
};

void UARTA0_init(void)
{
    // Selecting P1.2 and P1.3 in UART mode.
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
                    GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
    MAP_UART_initModule(EUSCI_A0_BASE, &uartConfig);
    MAP_UART_enableModule(EUSCI_A0_BASE);

    MAP_UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    MAP_Interrupt_enableInterrupt(INT_EUSCIA0);
}

void UARTA2_init(void)
{
    // Selecting P3.2 and P3.3 in UART mode.
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3,
                    GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
    MAP_UART_initModule(EUSCI_A2_BASE, &uartConfig);
    MAP_UART_enableModule(EUSCI_A2_BASE);

    MAP_UART_enableInterrupt(EUSCI_A2_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    MAP_Interrupt_enableInterrupt(INT_EUSCIA2);
}

void UART_transmitString(uint32_t moduleInstance, const char *transmitData)
{
    int i;

    for (i = 0; i < strlen(transmitData); i++)
    {
        MAP_UART_transmitData(moduleInstance, transmitData[i]);
    }
}
