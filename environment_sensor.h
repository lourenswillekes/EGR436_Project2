/*
 * environment_sensor.h
 *
 *  Created on: Feb 2018
 *      Author: lourw and joe
 */

#ifndef ENVIRONMENT_SENSOR_H_
#define ENVIRONMENT_SENSOR_H_

#include "bme280.h"
#include "I2c.h"
#include "Timer32_driver.h"


// Configures I2C and initializes BME280 in Normal Mode
int8_t BME280_Init(struct bme280_dev *dev);
// Fills data structure with compensated data from sensor
int8_t BME280_Read(struct bme280_dev *dev, struct bme280_data *data);


#endif /* ENVIRONMENT_SENSOR_H_ */
