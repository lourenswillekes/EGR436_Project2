/*
 * LCD.h
 *
 *  Created on: Feb 10, 2018
 *      Author: joe
 */

#ifndef LCD_H_
#define LCD_H_

#if 0 == 1
typedef enum
{
    item1 = 0,
    item2 = 1,
    item3 = 2,
    item4 = 3,
    item5 = 4
}menu_items;

typedef enum
{
    DARK = 0,
    TWILIGHT = 1,
    OVERCAST = 2,
    PARTLY_SUNNY = 3,
    SUNNY = 4

}Light_Status;

typedef struct forecast{
    Light_Status light_quality;
    const uint16_t *image;
    uint16_t height;
    uint16_t width;
    uint16_t x;
    uint16_t y;
    /*Values used to determine light range*/
    int maxR;
    int minR;
}forecast;
#endif

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

#if 0 == 1
//typedef struct menu_item menu_item;
typedef struct menu_item{
    /*Menu Item Number*/
    menu_items item_num;   //used to match with enum(menu_items) for selection and click

    /*Option Name*/
    char *name;

    /*LCD Placement*/
    int x_pos;
    int y_pos;

    /*Flagged when cursor is over this item*/
    int selected;

    /*Flagged when the user clicks this item*/
    int clicked;

    /*Counter value required to reach desired frequency*/
    int toggle_period;

    /*Clk Count: used for longer delays*/
    int toggle_count;
}menu_item;

menu_items num_to_menu_item(int x);
Light_Status num_to_enum(int x);


void print_current_status_pic(Light_Status current_status);

#endif

//void create_data_display(void);
void LCD_init(void);
//void updateForecast(int forecast_code);
//void updateDataDisplay(void);

void printTimeandDate(void);
void updateTimeandDate(void);

#if 0 == 1
void update_totals(int update_inside, int update_outside);

int getChangeOrientation(float total, int count, float current);
#endif

#endif /* LCD_H_ */
