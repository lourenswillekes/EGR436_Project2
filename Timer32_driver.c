/*
 * Timer32_driver.c
 *
 *  Created on: Mar 20, 2018
 *      Author: lourw
 */

#include "Timer32_driver.h"


void Timer32_init(void)
{
    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_16,
                           TIMER32_32BIT, TIMER32_FREE_RUN_MODE);
}

/* ***CALCULATED FOR 12 MHz CLK*** */
void Timer32_waitms(uint32_t ms)
{
    // 750 clock cycles per ms
    int count = ms * 750;

    // set the count and start the timer
    MAP_Timer32_setCount(TIMER32_0_BASE, count);
    MAP_Timer32_startTimer(TIMER32_0_BASE, true);

    // wait until counter hits 0
    while (0 < MAP_Timer32_getValue(TIMER32_0_BASE));
}
