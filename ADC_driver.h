/*
 * ADC_driver.h
 *
 *  Created on: April 2018
 *      Author: Lourens Willekes
 */

#ifndef ADC_DRIVER_H_
#define ADC_DRIVER_H_

#include "driverlib.h"


void ADC_init(void);
void ADC_read(uint16_t *mem0, uint16_t *mem1);


#endif /* ADC_DRIVER_H_ */
