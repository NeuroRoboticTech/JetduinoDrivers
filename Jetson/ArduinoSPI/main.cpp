/*
=================================================================================
 Name        : i2c-arduino.c
 Version     : 0.1

 Copyright (C) 2012 by Andre Wussow, 2012, desk@binerry.de

 Description :
     Sample of controlling an Arduino connected to Raspberry Pi via I2C.

	 Recommended connection (http://www.raspberrypi.org/archives/384):
	 Arduino pins      I2C-Shifter      Raspberry Pi
	 GND                                P06  - GND
	 5V                5V
	 SDA               SDA2
	 SCL               SCL2
	                   3V3              P01 - 3.3V
					   SDA1             P03 - SDA
					   SCL1             P05 - SCL

 References  :
 http://binerry.de/post/27128825416/raspberry-pi-with-i2c-arduino-slave

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
#include <linux/spi/spidev.h>
#include "SimpleGPIO.h"

int deviceHandle;
int readBytes;
#define BUFF_SIZE 10
unsigned char inBuffer[BUFF_SIZE];
unsigned char outBuffer[BUFF_SIZE];

void testCommand(char command[3], char identifier[5]);

timespec diff(timespec start, timespec end)
{
	timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}

static void dumpstat(const char *name, int fd)
{
	__u8	mode, lsb, bitsr=8, bitsw;
	__u32	speed=25000000;

	if (ioctl(fd, SPI_IOC_RD_MODE, &mode) < 0) {
		perror("SPI rd_mode");
		return;
	}
	if (ioctl(fd, SPI_IOC_RD_LSB_FIRST, &lsb) < 0) {
		perror("SPI rd_lsb_fist");
		return;
	}
	if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bitsr) < 0) {
		perror("SPI bits_per_word");
		return;
	}
	if (ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bitsr) < 0) {
		perror("SPI bits_per_word");
		return;
	}
	if (ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bitsw) < 0) {
		perror("SPI bits_per_word");
		return;
	}
	if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) > 0) {
		perror("SPI write max_speed_hz");
		return;
    }
	if (ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed) < 0) {
		perror("SPI max_speed_hz");
		return;
	}

	printf("%s: spi mode %d, %d bitsr, %d bitsw, %sper word, %d Hz max\n",
		name, mode, bitsr, bitsw, lsb ? "(lsb first) " : "", speed);
}

void do_msg(int fd, unsigned char out, int len)
{
	struct spi_ioc_transfer	xfer[1];
	unsigned char		txbuf[32], rxbuf[32], *bp;
	int			status;

	memset(xfer, 0, sizeof xfer);
	memset(txbuf, out, sizeof txbuf);
	memset(rxbuf, 0, sizeof rxbuf);

	if (len > (int) sizeof txbuf)
		len = sizeof txbuf;

	//txbuf[0] = out;
	xfer[0].tx_buf = (unsigned long) txbuf;
	xfer[0].rx_buf = (unsigned long) rxbuf;
	xfer[0].len = len;

	status = ioctl(fd, SPI_IOC_MESSAGE(1), xfer);
	if (status < 0) {
		perror("SPI_IOC_MESSAGE");
		return;
	}

	printf("response(%2d, %2d): ", len, status);
	for (bp = rxbuf; len; len--)
		printf(" %02x", *bp++);
	printf("\n");
}
/*
void do_msg(int fd, unsigned char out, int len)
{
	struct spi_ioc_transfer	xfer[2];
	unsigned char		txbuf[32], rxbuf[32], *bp;
	int			status;

	memset(xfer, 0, sizeof xfer);
	memset(txbuf, out, sizeof txbuf);
	memset(rxbuf, 0, sizeof rxbuf);

	if (len > (int) sizeof txbuf)
		len = sizeof txbuf;

	//txbuf[0] = out;
	xfer[0].tx_buf = (unsigned long) txbuf;
	xfer[0].len = len;

	xfer[1].rx_buf = (unsigned long) rxbuf;
	xfer[1].len = len;

	status = ioctl(fd, SPI_IOC_MESSAGE(2), xfer);
	if (status < 0) {
		perror("SPI_IOC_MESSAGE");
		return;
	}

	printf("response(%2d, %2d): ", len, status);
	for (bp = rxbuf; len; len--)
		printf(" %02x", *bp++);
	printf("\n");
}
*/
/*
void do_msg(int fd, unsigned int len)
{
	struct spi_ioc_transfer	xfer[2];
	int			status;

	if (len > sizeof outBuffer)
		len = sizeof outBuffer;

	xfer[0].tx_buf = (unsigned long) outBuffer;
	xfer[0].len = len;
	//xfer[0].cs_change = 1;
	//xfer[0].delay_usecs = 1;

	xfer[1].rx_buf = (unsigned long) inBuffer;
	xfer[1].len = len;
	//xfer[1].cs_change = 1;
	//xfer[1].delay_usecs = 1;

	status = ioctl(fd, SPI_IOC_MESSAGE(2), xfer);
	if (status < 0) {
		perror("SPI_IOC_MESSAGE");
		return;
	}

	printf("response(%2d, %2d): ", len, status);
	for (unsigned char *bp = inBuffer; len; len--)
		printf(" %02x", *bp++);
	printf("\n");
}
*/
int main (void)
{
    timespec startTime, endTime;

    std::ofstream myfile;
    myfile.open ("Data.txt");

	// print infos
	printf("SPI Arduino Sample\n");
	printf("========================================\n");

    //printf("Exporting pin 57\r\n");
    //gpio_export(57);

    //gpio_set_dir(57, OUTPUT_PIN);

    //Keep the chipselect high till we want to write something
    //gpio_set_value(57, true);

	// initialize buffer
	for(int i=0; i<BUFF_SIZE; i++)
    {
        outBuffer[i] = 65+i; //65+i;
        inBuffer[i] = 0;
    }
    //outBuffer[1] = 0;//+= 1;

	// open device on /dev/i2c-0
	if ((deviceHandle = open("/dev/spidev0.0", O_RDWR)) < 0) {
	//if ((deviceHandle0 = open("/dev/i2c-0", O_RDWR)) < 0) {
		printf("Error: Couldn't open device 0! %d\n", deviceHandle);
		return 1;
	}

    dumpstat("spidev0.0", deviceHandle);

    //Lower chip select
    //gpio_set_value(57, false);

    double dblTotal = 0;
    //for(int iIter=0; iIter<12; iIter++)
    //{
        clock_gettime(CLOCK_REALTIME, &startTime);

        //readBytes = write(deviceHandle, outBuffer, 10);
        //readBytes = write(deviceHandle, outBuffer, 2);

        // Send out data
        //for(int i=0; i<4; i++)
        //{
        //    readBytes = write(deviceHandle, outBuffer, 1);
        //     //outBuffer[0] += 1;
        //}
        //readBytes = write(deviceHandle, outBuffer, 1);
        //readBytes = write(deviceHandle, outBuffer+1, 1);

        // Recieve data
        //char iVal = 100;
        //for(int i=0; i<64; i++)
        //{
            do_msg(deviceHandle, 65, BUFF_SIZE);
        //readBytes = read(deviceHandle, inBuffer, 10);
        //    //iVal = outBuffer[0];
        //}


        clock_gettime(CLOCK_REALTIME, &endTime);
        double dbltime = diff(startTime,endTime).tv_sec + (double) (diff(startTime,endTime).tv_nsec/1e9);
        std::cout<<dbltime<<"\n";
        dblTotal += dbltime;

        myfile << dbltime << "\n";
    //}

    double dblAvg = dblTotal/10;
    std::cout<< "Average: " << dblAvg <<"\n";

    //Raise chip select again
    //gpio_set_value(57, true);

	// close connection and return
	close(deviceHandle);
	//close(deviceHandle1);

	myfile.close();

	return 0;
}
