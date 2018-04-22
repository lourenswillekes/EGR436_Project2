/*
 * LCD.h
 *
 *  Created on: Feb 10, 2018
 *      Author: joe
 */

#ifndef LCD_H_
#define LCD_H_

typedef struct display_cell{
    /*Box dimensions*/
    uint16_t x_start;
    uint16_t x_finish;
    uint16_t y_start;
    uint16_t y_finish;

    /*Display Title*/
    char *display_title;

    /*Display Data*/
    float temperature;
    float humidity;
    float pressure;

    /*Used to compute change over hour period*/
    float temp_total;
    float humidity_total;
    float pressure_total;

    int measurement_count;

    int temp_change_direction;
    int humidity_change_direction;
    int pressure_change_direction;
}display_cell;

void LCD_init(void);

void printTimeandDate(void);

void updateTimeandDate(void);

void create_data_display(void);

void updateDataDisplay(void);

void update_totals(void);

int getChangeOrientation(float total, int count, float current);

void updateIndicator(int on);

void update_power_display(float inVolt, float outI, float inI, float batI);

#endif /* LCD_H_ */
