/*
 * delays.c
 *
 *  Created on: Feb 10, 2018
 *      Author: joe
 */

#include "delays.h"
/* Standard Includes */
#include <stdint.h>

// Subroutine to wait 1 msec (assumes 48 MHz clock)
// Inputs: n  number of 1 msec to wait
// Outputs: None
// Notes: implemented in ST7735.c as count of assembly instructions executed
void OneMsDelay(uint32_t n){
    return;
}

// Subroutine to wait 10 msec
// Inputs: n  number of 10 msec to wait
// Outputs: None
// Notes: calls Delay1ms repeatedly
void TenMsDelay(uint32_t n){
    OneMsDelay(n*10);
}

