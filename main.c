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

#include "ADC_driver.h"
#include "RTC_driver.h"
#include "Timer32_driver.h"
#include "UART_driver.h"
#include "LCD.h"
#include "environment_sensor.h"     //BME280 library


#define BUFFER_LENGTH 512
#define ENABLED  1
#define DISABLED 0
#define BATTERY_ENABLED DISABLED

//Designate Display Method
#define LCD                     0
#define GOOGLE_SHEETS           1
#define LCD_AND_GOOGLE_SHEETS   2

#define DISPLAY_METHOD          LCD_AND_GOOGLE_SHEETS

//Determines Wi-Fi Credentials
#define J 0
#define L 1
#define USER J

#if USER == J
const char *AT_WIFI = "AT+CWJAP=\"Samsung Galaxy S7 9448\",\"clke5086\"\r\n";
#elif USER==L
const char *AT_WIFI = "AT+CWJAP=\"Verizon-SM-G935V-B089\",\"brgf962^\"\r\n";
#endif

const char *AT_NIST = "AT+CIPSTART=\"TCP\",\"time.nist.gov\",13\r\n";
const char *AT_MODE = "AT+CWMODE_CUR=3\r\n";

volatile char buffer[BUFFER_LENGTH];
volatile uint8_t idx = 0;
volatile uint8_t response_complete = 0;

char *stockAPIKey = "R4D8SF5DHSXF8RGN";

volatile int second_count = 0;

int visual_indication = 0;

//structures defined in LCD module
extern volatile display_cell BME_Senosr;

//BME280 variables
int res;
int read_sensor = 0;
struct bme280_dev dev;
struct bme280_data compensated_data;

float output_voltage = 3.3;
float output_current = 20.3;
float input_current = 22.3;
float battery_current = 18.2;

int warning_count = 0;

uint8_t getBMEData(void);
int ESP8266CmdOut(int cmdID, const char *cmdOut, char *response, int postCmdWait, int errorResponseWait, bool retry);
void send_Warning_Messages(float value);
void upload_to_googlesheets(void);





volatile double voltage;
volatile double current;



int main(void)
{
    int j;
    char *res = NULL;
    uint8_t result;
    int err = 0;
    int invalid = 1;
    int success;

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
    // TODO: change this to CLK_driver.c, also run off external crystal
    MAP_CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);
    MAP_CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    // initialize ADC
    ADC_init();

#if DISPLAY_METHOD == LCD || DISPLAY_METHOD == LCD_AND_GOOGLE_SHEETS
    // initialize LCD
    LCD_init();
#endif

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
    //BME280_Init(&dev);

    // initialize uart
    UARTA0_init();
    UARTA2_init();

    // enable interrupts
    MAP_Interrupt_enableMaster();




    uint16_t mem0, mem1;
    while(1)
    {

        ADC_read(&mem0, &mem1);
        voltage = ((mem0 / 16384.0) * 3.3);
        current = (((mem1 - 8192) / 8192.0) * 3.3);
        Timer32_waitms(500);

    }





    /* Wi-Fi Module Pin setup */
    //WIFI EN
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN1);

    //WIFI RST (Pulled Up)
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN3);

    //Enable Module
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN1);

    //Set mode
    success = ESP8266CmdOut(1, AT_MODE, "OK", 2000, 100, TRUE);

    //Connect to Wi-Fi
    success = ESP8266CmdOut(2, AT_WIFI, "OK", 6000, 200, TRUE);

    //Query for time from NIST
    success = ESP8266CmdOut(3, AT_NIST, "IPD", 2000, 5000, TRUE);

    if(success){
        res = strstr(buffer, "IPD,51:");
        res += 8; // move to start of time and date
        sscanf(res, "%d %d-%d-%d %d:%d:%d", &julian, &year, &month, &day, &hour, &minute, &second);
        hour = (hour + 20) % 24;
    }else{
        //set default time
        year = 7777;
        month = 7;
        day = 1;
        hour = 7;
        minute = 7;
        second = 0;
    }

    // set time and date
    currentTime.year = year;
    currentTime.month = month;
    currentTime.dayOfmonth = day;
    currentTime.hours = hour;
    currentTime.minutes = minute;
    currentTime.seconds = second;
    RTC_setFromCalendar(&currentTime);

#if DISPLAY_METHOD == LCD || DISPLAY_METHOD == LCD_AND_GOOGLE_SHEETS
    //update LCD
    Timer32_waitms(100);
    create_data_display();
    Timer32_waitms(100);
    updateTimeandDate();

    Timer32_waitms(20);
    result = getBMEData();
    updateDataDisplay();
    update_power_display(output_voltage,output_current,input_current,battery_current);
#endif

    while(1)
    {
#if DISPLAY_METHOD == LCD || DISPLAY_METHOD == LCD_AND_GOOGLE_SHEETS
        //Update LCD Displayed Time
        if (59 < second_count)
        {
            second_count = 0;
            currentTime = RTC_read();
            updateTimeandDate();

            update_power_display(output_voltage,output_current,input_current,battery_current);
        }
#endif

#if DISPLAY_METHOD == GOOGLE_SHEETS || DISPLAY_METHOD == LCD_AND_GOOGLE_SHEETS
        //Push new data to google sheets
        if((second_count % 30) == 0){
            // connect to Google pushingbox API
            upload_to_googlesheets();
        }
#endif
        if((second_count % 20) == 0){
            result = getBMEData();
#if DISPLAY_METHOD == LCD || DISPLAY_METHOD == LCD_AND_GOOGLE_SHEETS
            //This new data is compared with past trends.
            update_totals();

            //Send new values to the screen
            updateDataDisplay();

            if(BME_Senosr.humidity > 70.0){
                if(warning_count == 0){
                    send_Warning_Messages(BME_Senosr.humidity);
                }else{
                    if(warning_count > 6){
                        warning_count = 0;
                    }
                }
                warning_count++;
            }
#endif
        }
    }
}
int ESP8266CmdOut(int cmdID, const char *cmdOut, char *response, int postCmdWait, int errorResponseWait, bool retry){
    bool successful = 0;
    char *res = NULL;
    char out[20];

    //Max retry 5 times
    int attempt_count = 0;

    while(!successful && attempt_count < 5){
        idx = 0;
        UART_transmitString(EUSCI_A2_BASE, cmdOut);
        Timer32_waitms(postCmdWait);
        res = strstr(buffer, response);
        if (NULL != res)
        {
            sprintf(out,"%i--Successful\r\n",cmdID);
            UART_transmitString(EUSCI_A0_BASE, out);

            successful = TRUE;
        } else
        {
            sprintf(out,"%i--Failed\r\n",cmdID);
            UART_transmitString(EUSCI_A0_BASE, out);

            if(!retry){
                break;
            }else{
                Timer32_waitms(errorResponseWait); //give the user time to setup wifi
            }
        }
        attempt_count++;
    }
    return successful;
}
void upload_to_googlesheets(void){
    char ESP8266String[300];
    char PostSensorData[2000];
    int success = 0;

    sprintf(PostSensorData,"GET "
            "/pushingbox?devid=v12285A95612A4A0&ID=%d&BatteryEnabled=%d"
            "&celData=%f&fehrData=%f&Humidity=%f"
            "&Pressure=%f&Vout=%f&Iin=%f&Ibat=%f&Iout=%f"
            " HTTP/1.1\r\nHost: api.pushingbox.com\r\nUser-Agent: ESP8266/1.0\r\nConnection: "
            "close\r\n\r\n",USER,BATTERY_ENABLED,((BME_Senosr.temperature - 32) / 1.8 ),
            BME_Senosr.temperature, BME_Senosr.humidity,BME_Senosr.pressure,output_voltage,
            input_current,battery_current,output_current);

    int formLength=strlen(PostSensorData);

    do{
        strcpy(ESP8266String,"AT+CIPSTART=\"TCP\",\"api.pushingbox.com\",80\r\n");
        success = ESP8266CmdOut(4, ESP8266String, "CONNECT", 3000, 500, TRUE);

        if(success){
            sprintf(ESP8266String, "AT+CIPSEND=%d\r\n",formLength);
            success = ESP8266CmdOut(5, ESP8266String, "OK", 500, 500, TRUE);
        }

        if(success){
            success = ESP8266CmdOut(6, PostSensorData, "SEND OK", 500, 1000, FALSE);
        }
    }while(!success);
}
void send_Warning_Messages(float value){
    // connect to Google pushingbox API

    char ESP8266String[300];
    char PostSensorData[2000];
    int success = 0;

    sprintf(PostSensorData,"GET "
            "/pushingbox?devid=vCA9C23EC5743864&value=%f"
            " HTTP/1.1\r\nHost: api.pushingbox.com\r\nUser-Agent: ESP8266/1.0\r\nConnection: "
            "close\r\n\r\n",value);

    int formLength=strlen(PostSensorData);

    do{
        strcpy(ESP8266String,"AT+CIPSTART=\"TCP\",\"api.pushingbox.com\",80\r\n");
        success = ESP8266CmdOut(4, ESP8266String, "CONNECT", 3000, 500, TRUE);

        if(success){
            sprintf(ESP8266String, "AT+CIPSEND=%d\r\n",formLength);
            success = ESP8266CmdOut(5, ESP8266String, "OK", 500, 500, TRUE);
        }

        if(success){
            success = ESP8266CmdOut(6, PostSensorData, "SEND OK", 500, 1000, FALSE);
        }
    }while(!success);
}

uint8_t getBMEData(void){
    //Retrieve sensor values from BME280
    uint8_t res = BME280_Read(&dev, &compensated_data);

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

    return res;
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
#if DISPLAY_METHOD == LCD || DISPLAY_METHOD == LCD_AND_GOOGLE_SHEETS
        visual_indication = 1;
        updateIndicator((second_count % 2));
#endif
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
