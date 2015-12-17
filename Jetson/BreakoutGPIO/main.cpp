/*
=================================================================================
 Name        : BreakoutGPIO
 Version     : 0.1

 Copyright (C) 2015 by NeuroRoboticTechnologies, LLC, 2015,
 dcofer@NeuroRoboticTech.com

 Description :
     Controlling GPIO on the Jetson TK1 breakout board.

 References  :
http://neurorobotictech.com

================================================================================
This sample is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This sample is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.
================================================================================
*/

#include <iostream>
#include <linux/types.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include "SimpleGPIO.h"

//GPIO Pin Mapping
//PH1  57
//PK1  81
//PK2  82
//PK4  84

//PU0  160
//PU1  161
//PU2  162
//PU3  163
//PU4  164
//PU5  165
//PU6  166


int outPinCount = 6;
int outPins[] = {57, 81, 82, 84, 160, 161};

int inPinCount = 5;
int inPins[] = {162, 163, 164, 165, 166};

/*
int outPinCount = 11;
int outPins[] = {57, 81, 82, 84, 160, 161, 162, 163, 164, 165, 166};

int inPinCount = 0;
int inPins[] = {};
*/

void readGPIO()
{
    printf("Reading GPIO input pins\r\n");
    for(int i=0; i<inPinCount; i++)
    {
        unsigned int val=0;
        if(gpio_get_value(inPins[i], &val) == 0)
            printf("Pin %d: %d\r\n", inPins[i], val);
        else
            printf("Error reading GPIO input pin %d\r\n", inPins[i]);
    }
}

void writeGPIO(bool level)
{
    if(level)
        printf("Setting GPIO output pins high\r\n");
    else
        printf("Setting GPIO output pins low\r\n");

    for(int i=0; i<outPinCount; i++)
        gpio_set_value(outPins[i], level);
}

int main (void)
{
	// print infos
	printf("Jetson Breakout GPIO Sample\n");
	printf("========================================\n");

    printf("Exporting GPIO output pins\r\n");
    for(int i=0; i<outPinCount; i++)
    {
        gpio_export(outPins[i]);
        gpio_set_dir(outPins[i], OUTPUT_PIN);
    }

    printf("Exporting GPIO input pins\r\n");
    for(int i=0; i<inPinCount; i++)
    {
        gpio_export(inPins[i]);
        gpio_set_dir(inPins[i], INPUT_PIN);
    }

    for(int loop=0; loop<5; loop++)
    {
        writeGPIO(true);
        readGPIO();

        sleep(2);

        writeGPIO(false);
        readGPIO();

        sleep(2);
    }

    printf("Unexporting GPIO output pins\r\n");
    for(int i=0; i<outPinCount; i++)
        gpio_unexport(outPins[i]);

    printf("Unexporting GPIO input pins\r\n");
    for(int i=0; i<inPinCount; i++)
        gpio_unexport(inPins[i]);

	return 0;
}
