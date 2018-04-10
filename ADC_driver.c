/*
 * ADC_driver.c
 *
 *  Created on: April 2018
 *      Author: Lourens Willekes
 */

#include "ADC_driver.h"


void ADC_init(void)
{
    // initializing adc clk = 3MHz
    MAP_ADC14_enableModule();
    MAP_ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_4,
                ADC_DIVIDER_1, 0);

    // configure gpio 5.5 and 5.4 for analog input
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5,
                GPIO_PIN5 | GPIO_PIN4, GPIO_TERTIARY_MODULE_FUNCTION);

    // configure adc mem 0 for voltage and mem 1 for differential
    MAP_ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM1, false);
    MAP_ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_AVCC_VREFNEG_VSS,
                ADC_INPUT_A0, false);
    MAP_ADC14_configureConversionMemory(ADC_MEM1, ADC_VREFPOS_AVCC_VREFNEG_VSS,
                ADC_INPUT_A1, true);
    MAP_ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);

    // enable conversion
    MAP_ADC14_enableConversion();

}

void ADC_read(uint16_t *mem0, uint16_t *mem1)
{
    // start a conversion
    MAP_ADC14_toggleConversionTrigger();
    // wait until it completes
    while (0 == (ADC14->IFGR0 & ADC_INT1));

    // and get the result
    *mem0 = MAP_ADC14_getResult(ADC_MEM0);
    *mem1 = MAP_ADC14_getResult(ADC_MEM1);
}
