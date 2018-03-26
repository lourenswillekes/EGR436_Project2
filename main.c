/*
 * -------------------------------------------
 *    MSP432 DriverLib - v3_21_00_05 
 * -------------------------------------------
 *
 * --COPYRIGHT--,BSD,BSD
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/******************************************************************************
 * MSP432 Empty Project
 *
 * Description: An empty project that uses DriverLib
 *
 *                MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST               |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 * Author: 
*******************************************************************************/

#include "driverlib.h"

#include <stdint.h>
#include <stdbool.h>

#include "RTC_driver.h"
#include "Timer32_driver.h"
#include "UART_driver.h"

#include "LCD.h"

#include "environment_sensor.h"     //BME280 library

//Determines Wi-Fi Credentials
#define J 0
#define L 1
#define USER J

/*Mode Selection*/
#define SOFT_AP_MODE            1
#define STATION_MODE            2
#define STATION_PLUS_SOFT_MODE  3
#define ESP_MODE    SOFT_AP_MODE


#define BUFFER_LENGTH 512

#if ESP_MODE == SOFT_AP_MODE
const char *AT_MODE = "AT+CWMODE=2\r\n";
#elif ESP_MODE == STATION_PLUS_SOFT_MODE
const char *AT_MODE = "AT+CWMODE=3\r\n";
#else
const char *AT_MODE = "AT+CWMODE=1\r\n";
#endif


//setup server
//channel 1
//encoding 0
//max connections 4
//ssid hidden 0 (broadcast)
const char *AT_CWSAP_CUR = "AT+CWSAP_CUR=\"JL_Server\",\"passWord\",1,0,4,0\r\n";

//enable multiple connections
const char *AT_CIPMUX="AT+CIPMUX=1\r\n";

//Create TCP Server
const char *AT_CIPSERVER="AT+CIPSERVER=1,80\r\n";

//Get Local IP Address
const char *AT_CIFSR = "AT+CIFSR\r\n";

#if USER == J
const char *AT_WIFI = "AT+CWJAP=\"Samsung Galaxy S7 9448\",\"clke5086\"\r\n";
#elif USER==L
const char *AT_WIFI = "AT+CWJAP=\"Verizon-SM-G935V-B089\",\"brgf962^\"\r\n";
#endif

const char *AT_NIST = "AT+CIPSTART=\"TCP\",\"time.nist.gov\",13\r\n";

volatile char buffer[BUFFER_LENGTH];
volatile uint8_t idx = 0;
volatile uint8_t response_complete = 0;

volatile int second_count = 0;

int visual_indication = 0;

//structures defined in LCD module
extern volatile display_cell BME_Senosr;

//BME280 variables
int res;
int read_sensor = 0;
struct bme280_dev dev;
struct bme280_data compensated_data;

int main(void)
{
    int j;
    char *res = NULL;
    int err = 0;

    int julian, year, month, day, hour, minute, second;
    RTC_C_Calendar currentTime =
    {
     // sec, min, hour, day of week, day of month, month, year
         00,
         50,
         20,
         05,
         02,
         03,
         2018
    };
    char time_and_date[32];


    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();

    // clk = 12 MHz
    MAP_CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);
    MAP_CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    // initialize LCD
    LCD_init();

    // initialize rtc
    RTC_init();

    // initialize timer32
    Timer32_init();

    /*
     * This method established the settings for the
     * BME280 BOSCH sensor. Within this method, the
     * I2C and timer32 modules are initialized for
     * use in the method.
     */
    BME280_Init(&dev);

    // initialize uart
    UARTA0_init();
    UARTA2_init();

    // enable interrupts
    MAP_Interrupt_enableMaster();

    /* Wi-Fi Module Pin setup */
    //WIFI EN
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN1);

    //WIFI RST (Pulled Up)
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN3);

    //Enable Module
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN1);



    // set mode
    UART_transmitString(EUSCI_A2_BASE, AT_MODE);
    Timer32_waitms(2000);
    res = strstr(buffer, "OK");
    if (NULL != res)
    {
        UART_transmitString(EUSCI_A0_BASE, "01  ACK'd\r\n");
    } else
    {
        err = 1;
        UART_transmitString(EUSCI_A0_BASE, "01  NOT ACK'd\r\n");
    }


    if (!err) {
    // connect wifi
    idx = 0;
    UART_transmitString(EUSCI_A2_BASE, AT_WIFI);
    Timer32_waitms(6000);
    res = strstr(buffer, "OK");
    if (NULL != res)
    {
        UART_transmitString(EUSCI_A0_BASE, "02  ACK'd\r\n");
    } else
    {
        err = 2;
        UART_transmitString(EUSCI_A0_BASE, "02  NOT ACK'd\r\n");
    }
    }

#if ESP_MODE == STATION_PLUS_SOFT_MODE
    if (!err) {
    // request time
    idx = 0;
    UART_transmitString(EUSCI_A2_BASE, AT_NIST);
    Timer32_waitms(2000);
    res = strstr(buffer, "OK");
    if (NULL != res)
    {
        UART_transmitString(EUSCI_A0_BASE, "03  ACK'd\r\n");
    } else
    {
        err = 3;
        UART_transmitString(EUSCI_A0_BASE, "03  NOT ACK'd\r\n");
    }
    }


    if (!err) {
    // get time and date
    res = strstr(buffer, "IPD,51:");
    if (NULL != res)
    {
        UART_transmitString(EUSCI_A0_BASE, "04  ACK'd\r\n");
        res += 8; // move to start of time and date
        sscanf(res, "%d %d-%d-%d %d:%d:%d", &julian, &year, &month, &day, &hour, &minute, &second);
        hour = (hour + 20) % 24;
    } else
    {
        err = 4;
        UART_transmitString(EUSCI_A0_BASE, "04  NOT ACK'd\r\n");
    }
    }


    if (!err) {
    // set time and date
    currentTime.year = year;
    currentTime.month = month;
    currentTime.dayOfmonth = day;
    currentTime.hours = hour;
    currentTime.minutes = minute;
    currentTime.seconds = second;
    RTC_setFromCalendar(&currentTime);
    //update LCD
    Timer32_waitms(100);
    create_data_display();
    Timer32_waitms(100);
    updateTimeandDate();
    }
#elif ESP_MODE == SOFT_AP_MODE   //server mode
    err = 0;
    if (!err) {
        // set up soft AP
        idx = 0;
        UART_transmitString(EUSCI_A2_BASE, AT_CWSAP_CUR);
        Timer32_waitms(2000);
        res = strstr(buffer, "OK");
        if (NULL != res)
        {
            UART_transmitString(EUSCI_A0_BASE, "03  ACK'd\r\n");
        } else
        {
            err = 3;
            UART_transmitString(EUSCI_A0_BASE, "03  NOT ACK'd\r\n");
        }
    }
    if (!err) {
       //allow multiple connections
       idx = 0;
       UART_transmitString(EUSCI_A2_BASE, AT_CIPMUX);
       Timer32_waitms(2000);
       res = strstr(buffer, "OK");
       if (NULL != res)
       {
           UART_transmitString(EUSCI_A0_BASE, "03  ACK'd\r\n");
       } else
       {
           err = 3;
           UART_transmitString(EUSCI_A0_BASE, "03  NOT ACK'd\r\n");
       }
   }
    if (!err) {
        //create TCP server connection on port 80
        idx = 0;
        UART_transmitString(EUSCI_A2_BASE, AT_CIPSERVER);
        Timer32_waitms(2000);
        res = strstr(buffer, "OK");
        if (NULL != res)
        {
            UART_transmitString(EUSCI_A0_BASE, "03  ACK'd\r\n");
        } else
        {
            err = 3;
            UART_transmitString(EUSCI_A0_BASE, "03  NOT ACK'd\r\n");
        }
    }





    if (!err) {
        //get IP
        idx = 0;
        UART_transmitString(EUSCI_A2_BASE, AT_CIFSR);
        Timer32_waitms(2000);
        res = strstr(buffer, "OK");
        if (NULL != res)
        {
            UART_transmitString(EUSCI_A0_BASE, "03  ACK'd\r\n");
        } else
        {
            err = 3;
            UART_transmitString(EUSCI_A0_BASE, "03  NOT ACK'd\r\n");
        }
    }


#endif


    while(1)
    {
#if ESP_MODE == STATION_MODE || ESP_MODE == STATION_PLUS_SOFT_MODE
        if (visual_indication){
            visual_indication = 0;
            printTimeandDate();
        }
        if (59 < second_count)
        {
            second_count = 0;
            currentTime = RTC_read();
            sprintf(time_and_date, "%d/%d/%d %d:%d:%d\r\n", currentTime.year,
                    currentTime.month, currentTime.dayOfmonth, currentTime.hours,
                    currentTime.minutes, currentTime.seconds);

            UART_transmitString(EUSCI_A0_BASE, time_and_date);

            updateTimeandDate();
        }

        if((second_count % 20) == 0){
            //Retrieve sensor values from BME280
            res = BME280_Read(&dev, &compensated_data);

            //Format pressure measurement
            float temp_pressure = ( (float) compensated_data.pressure )/ 100.0;

            //convert from kPa to mmHg
            temp_pressure *= ( 0.760 / 101325.0);

            //scale by 1000 for mmHg. Conversion leaves the value in decimal notation
            //Save current formated measurement
            BME_Senosr.pressure = temp_pressure * 1000;

            //Correct for altitude
            BME_Senosr.pressure += 17;

            //Save formated humidity measurement
            BME_Senosr.humidity = compensated_data.humidity/1000.0; //Convert value to percentage

            //Convert from degrees C to F and save
            BME_Senosr.temperature = (((compensated_data.temperature / 100.0) * (9.0/5.0)) + 32.0);

            //This new data is compared with past trends.
            update_totals();

            //Send new values to the screen
            updateDataDisplay();
        }
#elif ESP_MODE == SOFT_AP_MODE
        if(1){
            ;
        }
#endif
    }
}


void RTC_ISR(void)
{
    uint32_t status;

    // get status and clear interrupt flag
    status = MAP_RTC_C_getEnabledInterruptStatus();
    MAP_RTC_C_clearInterruptFlag(status);

    // increment counter to keep time
    if (status & RTC_C_CLOCK_READ_READY_INTERRUPT)
    {
        second_count++;
        visual_indication = 1;
    }

}

void UARTA0_ISR(void)
{
    uint8_t byte;

    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A0_BASE);
    MAP_UART_clearInterruptFlag(EUSCI_A0_BASE, status);

    if (status & EUSCI_A_UART_RECEIVE_INTERRUPT)
    {
        byte = MAP_UART_receiveData(EUSCI_A0_BASE);
        MAP_UART_transmitData(EUSCI_A2_BASE, byte);
    }
}

void UARTA2_ISR(void)
{
    uint8_t byte;

    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A2_BASE);
    MAP_UART_clearInterruptFlag(EUSCI_A2_BASE, status);

    if (status & EUSCI_A_UART_RECEIVE_INTERRUPT)
    {
        byte = MAP_UART_receiveData(EUSCI_A2_BASE);
        MAP_UART_transmitData(EUSCI_A0_BASE, byte);
        // add byte to the rx buffer
        buffer[idx++] = (char) byte;
        if (BUFFER_LENGTH == idx)
        {
            idx = 0;
        }
    }
}