/*
 * RTC_driver.c
 *
 *  Created on: Mar 20, 2018
 *      Author: lourw
 */

#include "RTC_driver.h"


void RTC_init(void){

    /* Configuring pins for peripheral/crystal usage */
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ,
            GPIO_PIN0 | GPIO_PIN1, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Starting LFXT in non-bypass mode without a timeout. */
    CS_startLFXT(false);

    /* Specify an interrupt to assert every minute */
    MAP_RTC_C_setCalendarEvent(RTC_C_CALENDAREVENT_MINUTECHANGE);

    /* Enable interrupt for RTC Ready Status, which asserts when the RTC
     * Calendar registers are ready to read.
     * Also, enable interrupts for the Calendar alarm and Calendar event. */
    MAP_RTC_C_clearInterruptFlag(
            RTC_C_CLOCK_READ_READY_INTERRUPT | RTC_C_TIME_EVENT_INTERRUPT);
    MAP_RTC_C_enableInterrupt(
            RTC_C_CLOCK_READ_READY_INTERRUPT | RTC_C_TIME_EVENT_INTERRUPT);

    MAP_Interrupt_enableInterrupt(INT_RTC_C);
}

void RTC_setFromCalendar(RTC_C_Calendar *date)
{
    MAP_RTC_C_initCalendar(date, RTC_C_FORMAT_BINARY);
    MAP_RTC_C_startClock();
}

RTC_C_Calendar RTC_read(void)
{
    RTC_C_Calendar time;

    time = MAP_RTC_C_getCalendarTime();

    return time;
}
