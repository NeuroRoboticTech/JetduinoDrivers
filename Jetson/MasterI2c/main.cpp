/*
=================================================================================
 Name        : i2c-arduino.c
 Version     : 0.1

 Copyright (C) 2012 by Andre Wussow, 2012, desk@binerry.de

 Description :
     Sample of controlling an Arduino connected to Jetson TK1 via I2C.


 References  :
 #######http://binerry.de/post/27128825416/raspberry-pi-with-i2c-arduino-slave

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
#include <dlfcn.h>
#include <utime.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/timeb.h>

#include <exception>
#include <string>
#include <iosfwd>
#include <sstream>
#include <fstream>
#include <iterator>
#include <iostream>
#include <vector>
#include <deque>
#include <stack>
#include <map>
#include <list>
#include <unordered_set>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <cctype>
#include <math.h>
#include <memory.h>
#include <algorithm>
#include <string.h>
#include <time.h>

#include <errno.h>
#include <fcntl.h>
#include <poll.h>

#include <getopt.h>
#include <sys/ioctl.h>

#include <linux/i2c-dev.h>
//#include <linux/i2c.h>

int device1Handle;
int readBytes;
char buffer[7];


// function for testing command
void get_data(int handle, char slave)
{
    char command[3] = {(char) (65+slave), 66, 67};

	std::cout << "sending to slave " << slave << std::endl;
	readBytes = write(handle, command, 3);

	// give arduino some reaction time
	usleep(100000); // 100ms

	// read success
	readBytes = read(handle, buffer, 7);
	if (readBytes != 7)
		std::cout << "Error: Invalid receive!" << std::endl;
	else
        std::cout << "Response: " << buffer << std::endl;
}

int main (void)
{
	// print infos
	printf("Jetson TK1 I2C Arduino Sample\n");
	printf("========================================\n");

	// initialize buffer
	for(int i=0; i<7; i++) {
        buffer[i] = i;
	}

	// address of i2c Arduino device 1
    int device1I2CAddress = 0x01;  // (0x01 = 42)

    std::string i2c_port = "/dev/i2c-0"; //GEN1_I2C
    //std::string i2c_port = "/dev/i2c-1"; //GEN2_I2C
    //std::string i2c_port = "/dev/i2c-2"; //CAM_I2C
    //std::string i2c_port = "/dev/i2c-4"; //PWR_I2C

	// open device on /dev/i2c-0
	if ((device1Handle = open(i2c_port.c_str(), O_RDWR)) < 0) {
		printf("Error: Couldn't open device! %d\n", device1Handle);
		return 1;
	}

	// connect to arduino as i2c master
	if (ioctl(device1Handle, I2C_SLAVE, device1I2CAddress) < 0) {
		printf("Error: Couldn't find arduino on address!\n");
		return 1;
	}

    for(int i=0; i<10; i++) {
        if (write(device1Handle, buffer, 3) != 3) {
            /* ERROR HANDLING: i2c transaction failed */
        }
		usleep(1000000); // 1s
    }

	// close connection and return
	close(device1Handle);
	return 0;
}
