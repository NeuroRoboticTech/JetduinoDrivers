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

int pinCount = 11;
int pins[] = {57, 81, 82, 84, 160, 161, 162, 163, 164, 165, 166};

int main (void)
{
	// print infos
	printf("Jetson Breakout GPIO Sample\n");
	printf("========================================\n");

//81, 82, 84

    printf("Exporting GPIO pins as outputs\r\n");
    for(int i=0; i<pinCount; i++)
    {
        gpio_export(pins[i]);
        gpio_set_dir(pins[i], OUTPUT_PIN);
    }

    for(int loop=0; loop<5; loop++)
    {
        printf("Setting GPIO pins high\r\n");
        //Set the GPIO pins high
        for(int i=0; i<pinCount; i++)
            gpio_set_value(pins[i], true);

        sleep(1);

        printf("Setting GPIO pins low\r\n");
        //Set the GPIO pins low
        for(int i=0; i<pinCount; i++)
            gpio_set_value(pins[i], false);

        sleep(1);
    }

    printf("Unexporting GPIO pins\r\n");
    for(int i=0; i<pinCount; i++)
        gpio_unexport(pins[i]);

	return 0;
}
