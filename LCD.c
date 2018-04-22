/*
 * LCD.c
 *
 *  Created on: Feb 10, 2018
 *      Author: joe
 */

/* DriverLib Includes */
#include "driverlib.h"

#include "LCD.h"
#include "ST7735.h"
#include "delays.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#if DISPLAY_METHOD == LCD || DISPLAY_METHOD == LCD_AND_GOOGLE_SHEETS
#include "weatherUnderground.txt"
#endif
#include "RTC_driver.h"


uint16_t grid_color = ST7735_CYAN;
uint16_t menu_text_color = ST7735_YELLOW;
uint16_t highlight_text_color = ST7735_CYAN;

uint8_t LCD_Rotation = 1;


#define increaseing_arrow  24
#define decreaseing_arrow  25
#define no_change_arrow    61

volatile display_cell BME_Sensor={
                0,      //x_start
                0,      //x_finish
                0,      //y_start
                0,      //y_finish
                " ",    //display title
                71.5,    //temp
                22.5,    //humidity
                760,     //Pressure
                0,       //total temp
                0,       //total humidity
                0,       //total pressure
                0,       //measurement count
                no_change_arrow,
                no_change_arrow,
                no_change_arrow
};
extern volatile int second_count;

RTC_C_Calendar time;

void LCD_init(void){
    ST7735_InitR(INITR_REDTAB); // initialize LCD controller IC
    ST7735_SetRotation(LCD_Rotation);
}

void create_data_display(void){
    ST7735_DrawFastHLine(0,25,160,grid_color);
    ST7735_DrawString2(20,30,"BME",menu_text_color,ST7735_BLACK);
    //ST7735_DrawString2(100,30,"I(mA)",menu_text_color,ST7735_BLACK);
    ST7735_DrawFastHLine(0,45,90,grid_color);
    ST7735_DrawFastVLine(90,25,83,grid_color);
    ST7735_DrawString2(0,50,"T",menu_text_color,ST7735_BLACK);
    ST7735_DrawString2(0,70,"H",menu_text_color,ST7735_BLACK);

    ST7735_DrawBitmap(110, 65, weatherUnderground, 40, 40);


    ST7735_DrawString2(0,90,"P",menu_text_color,ST7735_BLACK);
    ST7735_DrawFastHLine(0,108,160,grid_color);
    //ST7735_DrawString2(20,110,"Vout = ",menu_text_color,ST7735_BLACK);
}

void updateDataDisplay(void){
    char data[12];
    //print temp
    sprintf(data,"%02.0f%cF%c",BME_Sensor.temperature,247,BME_Sensor.temp_change_direction);
    TenMsDelay(1);
    ST7735_DrawString2(20,50,data,menu_text_color,ST7735_BLACK);

    //print humidity
    sprintf(data,"%02.0f%%%c",BME_Sensor.humidity,BME_Sensor.humidity_change_direction);
    TenMsDelay(1);
    ST7735_DrawString2(20,70,data,menu_text_color,ST7735_BLACK);

    //print bp
    sprintf(data,"%2.1f%c",BME_Sensor.pressure,BME_Sensor.pressure_change_direction);
    TenMsDelay(1);
    ST7735_DrawString2(20,90,data,menu_text_color,ST7735_BLACK);
}

void update_power_display(float outV, float outI, float inI, float batI){
    char data[12];
    //print input current
    sprintf(data,"%02.1f",inI);
    TenMsDelay(1);
    ST7735_DrawString2(100,50,data,menu_text_color,ST7735_BLACK);

    //print output current
    sprintf(data,"%02.1f",outI);
    TenMsDelay(1);
    ST7735_DrawString2(100,70,data,menu_text_color,ST7735_BLACK);

    //print battery current
    sprintf(data,"%2.1f",batI);
    TenMsDelay(1);
    ST7735_DrawString2(100,90,data,menu_text_color,ST7735_BLACK);

    //print output voltage
    sprintf(data,"%2.1fV",outV);
    TenMsDelay(1);
    ST7735_DrawString2(95,110,data,menu_text_color,ST7735_BLACK);
}

void update_totals(void){
    //increments measurement totals and count for average comparisons
    //Also the direction of change is set

    BME_Sensor.measurement_count++;
    BME_Sensor.temp_total+=BME_Sensor.temperature;
    BME_Sensor.pressure_total+=BME_Sensor.pressure;
    BME_Sensor.humidity_total+=BME_Sensor.humidity;

    BME_Sensor.temp_change_direction=getChangeOrientation(BME_Sensor.temp_total,
                                                          BME_Sensor.measurement_count,
                                                          BME_Sensor.temperature);
    BME_Sensor.humidity_change_direction=getChangeOrientation(BME_Sensor.humidity_total,
                                                          BME_Sensor.measurement_count,
                                                          BME_Sensor.humidity);
    BME_Sensor.pressure_change_direction=getChangeOrientation(BME_Sensor.pressure_total,
                                                          BME_Sensor.measurement_count,
                                                          BME_Sensor.pressure);
}


int getChangeOrientation(float total, int count, float current){
    float avg = total / count;
    if(current > avg){
        return increaseing_arrow;
    }else if(current < avg){
        return decreaseing_arrow;
    }else{
        return no_change_arrow;
    }
}


void printTimeandDate(void){
    char temp[10];

    sprintf(temp,"%02d %02d",time.hours,time.minutes);

    ST7735_DrawString2(0,5,temp,menu_text_color,ST7735_BLACK);
    int i = time.year % 2000;
    sprintf(temp,"%02d/%02d/%d",time.month,time.dayOfmonth,i);
    ST7735_DrawString2(70,5,temp,menu_text_color,ST7735_BLACK);
}

void updateIndicator(int on){
    if(on){
        ST7735_DrawString2(22,5,":",menu_text_color,ST7735_BLACK);
    }
    else{
        ST7735_DrawString2(22,5," ",menu_text_color,ST7735_BLACK);
    }
}

void updateTimeandDate(void){
    time = RTC_read();
    printTimeandDate();
}
