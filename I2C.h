
#ifndef I2C_H_
#define I2C_H_

/*
 * This method configures UCB1 for I2C protocol. The settings
 * are configured for SMCLK of 12MHz with a divisor of 120.
 * As such a 100kHz baud clock is established.
 */
void I2C_Init(void);
/*
 * This method is used to right data to a register on the specified device.
 */
int8_t I2C_Write_String(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t byteCount);

/*
 * This method is used to read data for a register on the specified device.
 */
int8_t I2C_Read_String(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t byteCount);

#endif
