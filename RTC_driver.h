/*
 * RTC_driver.h
 *
 *  Created on: Mar 2018
 *      Author: lourw
 */

#ifndef RTC_DRIVER_H_
#define RTC_DRIVER_H_

#include "driverlib.h"


void RTC_init(void);
void RTC_setFromCalendar(RTC_C_Calendar *date);
RTC_C_Calendar RTC_read(void);


#endif /* RTC_DRIVER_H_ */
