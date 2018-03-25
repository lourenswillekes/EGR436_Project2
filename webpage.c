/*
 * webpage.c
 *
 *  Created on: Mar 25, 2018
 *      Author: joe
 */

#include "webpage.h"
#include <string.h>
#include <stdio.h>

char *webpage_html_start = "<!DOCTYPE html><html> <body> <center>"
        "<h1>Set the color of LED light:<br></h1>"
        "<form action=\"\" method=\"get\">"
        "<p style=\"color:black;font-size:28px\"> LED color:<br> </p>"
        "<p style=\"color:red;font-size:24px\">"
        "<label for=\"red\">Red</label>"
        "<input type=\"checkbox\" name=\"red\"> </p>"
        "<p style=\"color:green;font-size:24px\">"
        "<label for=\"green\">Green</label>"
        "<input type=\"checkbox\" name=\"green\"> </p>"
        "<p style=\"color:blue;font-size:24px\">"
        "<label for=\"blue\">Blue</label>"
        "<input type=\"checkbox\" name=\"blue\"> </p>"
        "<p style=\"color:black;font-size:28px\">Turn Light:"
        "<input type=\"radio\" name=\"OnOff\" value=\"On\"> On"
        "<input type=\"radio\" name=\"OnOff\" value=\"Off\" checked> Off<br></p>"
        "<fieldset>"
        "<legend style=\"color:black;font-size:28px\">Environmental variables: </legend>"
        "<p style=\"color:black;font-size:28px\">";

//seperated from webpage_html_start by data section that needs to be formated
char *webpage_html_end = "</fieldset> <br> <input type=\"submit\" value=\"Submit\"> </form> </center> </body> </html>";

void formatHTMLPage(char *msg, float temperature, float humidity, float pressure){
    char temp[100];
    sprintf(temp,"Temperature: %02f<br>Humidity: %02f%%<br>Pressure: %3.1f mmHg<br></p>",temperature,humidity,pressure);
    msg = strcat(webpage_html_start,temp);
    msg = strcat(msg,webpage_html_end);
}



