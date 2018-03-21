/*
 * UART_driver.h
 *
 *  Created on: March 2018
 *      Author: lourw
 */

#ifndef UART_DRIVER_H_
#define UART_DRIVER_H_

#include "driverlib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void UARTA0_init(void);
void UARTA2_init(void);

void UART_transmitString(uint32_t moduleInstance, const char *transmitData);

#endif /* UART_DRIVER_H_ */
