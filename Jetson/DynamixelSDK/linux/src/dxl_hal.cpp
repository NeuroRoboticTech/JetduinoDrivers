#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <linux/serial.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <iostream>
#include <string>
#include <errno.h>

#include "dxl_hal.h"
#include "gpio.h"

int	gSocket_fd	= -1;
long	glStartTime	= 0;
float	gfRcvWaitTime	= 0.0f;
float	gfByteTransTime	= 0.0f;

char	gDeviceName[20];

int gCtrlGPIO = -1;
int gBaudRate = -1;
std::string gPortName;

// set the baud rate
int dxl_setBaud(struct termios &settings, int baud)
{
	if (baud <= 0) return false;
	//std::cout << "set_baud: " << baud << "\n";
	//baud_rate = baud;
	if (gSocket_fd < 0) return false;
	speed_t spd;
	switch (baud) {
		case 230400:	spd = B230400;	break;
		case 115200:	spd = B115200;	break;
		case 57600:	spd = B57600;	break;
		case 38400:	spd = B38400;	break;
		case 19200:	spd = B19200;	break;
		case 9600:	spd = B9600;	break;
		case 4800:	spd = B4800;	break;
		case 2400:	spd = B2400;	break;
		case 1800:	spd = B1800;	break;
		case 1200:	spd = B1200;	break;
		case 600:	spd = B600;	break;
		case 300:	spd = B300;	break;
		case 200:	spd = B200;	break;
		case 150:	spd = B150;	break;
		case 134:	spd = B134;	break;
		case 110:	spd = B110;	break;
		case 75:	spd = B75;	break;
		case 50:	spd = B50;	break;
		#ifdef B460800
		case 460800:	spd = B460800;	break;
		#endif
		#ifdef B500000
		case 500000:	spd = B500000;	break;
		#endif
		#ifdef B576000
		case 576000:	spd = B576000;	break;
		#endif
		#ifdef B921600
		case 921600:	spd = B921600;	break;
		#endif
		#ifdef B1000000
		case 1000000:	spd = B1000000;	break;
		#endif
		#ifdef B1152000
		case 1152000:	spd = B1152000;	break;
		#endif
		#ifdef B1500000
		case 1500000:	spd = B1500000;	break;
		#endif
		#ifdef B2000000
		case 2000000:	spd = B2000000;	break;
		#endif
		#ifdef B2500000
		case 2500000:	spd = B2500000;	break;
		#endif
		#ifdef B3000000
		case 3000000:	spd = B3000000;	break;
		#endif
		#ifdef B3500000
		case 3500000:	spd = B3500000;	break;
		#endif
		#ifdef B4000000
		case 4000000:	spd = B4000000;	break;
		#endif
		#ifdef B7200
		case 7200:	spd = B7200;	break;
		#endif
		#ifdef B14400
		case 14400:	spd = B14400;	break;
		#endif
		#ifdef B28800
		case 28800:	spd = B28800;	break;
		#endif
		#ifdef B76800
		case 76800:	spd = B76800;	break;
		#endif
		default: {
			return 0;
		}
	}
	cfsetospeed(&settings, spd);
	cfsetispeed(&settings, spd);
	if (tcsetattr(gSocket_fd, TCSANOW, &settings) < 0) return 0;

	return 1;
}


//----------------------------------------------------------------
int dxl_setup_serial(std::string portName, int baud){

	dxl_hal_close();

	std::cout << "setup serial" << "\r\n";
    gBaudRate = baud;

    //lets account for the name being passed in instead of the device path
    if( portName.size() > 5 && portName.substr(0, 5) != "/dev/" ){
        portName = "/dev/" + portName;
    }

    std::cout << "opening " << portName << " @ " << baud << " bps" << "\r\n";
	struct serial_struct kernel_serial_settings;
	int bits;
	std::string error_msg;
	gSocket_fd = open(portName.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (gSocket_fd < 0) {
		if (errno == EACCES) {
			error_msg = "Unable to access " + portName + ", insufficient permission";
			// TODO: we could look at the permission bits and owner
			// to make a better message here
		} else if (errno == EISDIR) {
			error_msg = "Unable to open " + portName +
				", Object is a directory, not a serial port";
		} else if (errno == ENODEV || errno == ENXIO) {
			error_msg = "Unable to open " + portName +
				", Serial port hardware not installed";
		} else if (errno == ENOENT) {
			error_msg = "Unable to open " + portName +
				", Device name does not exist";
		} else {
			error_msg = "Unable to open " + portName;
		}

        std::cout << error_msg << "\r\n";
		return false;
	}
	if (ioctl(gSocket_fd, TIOCMGET, &bits) < 0) {
		::close(gSocket_fd);
		error_msg = "Unable to query serial port signals";
        std::cout << error_msg << "\r\n";
		return false;
	}
	bits &= ~(TIOCM_DTR | TIOCM_RTS);
	if (ioctl(gSocket_fd, TIOCMSET, &bits) < 0) {
		::close(gSocket_fd);
		error_msg = "Unable to control serial port signals";
        std::cout << error_msg << "\r\n";
		return false;
	}
	struct termios settings_orig;
	struct termios settings;
	if (tcgetattr(gSocket_fd, &settings_orig) != 0) {
		::close(gSocket_fd);
		error_msg = "Unable to query serial port settings (perhaps not a serial port)";
        std::cout << error_msg << "\r\n";
		return false;
	}
	memset(&settings, 0, sizeof(settings));
	settings.c_iflag = IGNBRK | IGNPAR;
	settings.c_cflag = CS8 | CREAD | HUPCL | CLOCAL;
	dxl_setBaud(settings, gBaudRate);
	if (ioctl(gSocket_fd, TIOCGSERIAL, &kernel_serial_settings) == 0) {
		kernel_serial_settings.flags |= ASYNC_LOW_LATENCY;
		ioctl(gSocket_fd, TIOCSSERIAL, &kernel_serial_settings);
	}
	tcflush(gSocket_fd, TCIFLUSH);

	gPortName = portName;
    std::cout << "opened " << portName << " sucessfully @ " << baud << " bps" << "\r\n";
	return true;
}

int dxl_setup_gpio(int gpio_ctrl)
{
    if(gpio_ctrl < 0)
    {
        gCtrlGPIO = -1;
        return 1;
    }

    //If we are using a GPIO control line then export it here, set it to output and pull it low.
    gCtrlGPIO = gpio_ctrl;
    if(!gpio_export(gCtrlGPIO) )
    {
		fprintf(stderr, "export GPIO error: %d\n", gCtrlGPIO);
		return 0;
    }

    if(!gpio_set_dir(gCtrlGPIO, OUTPUT_PIN) )
    {
		fprintf(stderr, "Set Direction GPIO error: %d\n", gCtrlGPIO);
		return 0;
    }

    if(!gpio_set_value(gCtrlGPIO, LOW) )
    {
		fprintf(stderr, "Set Value GPIO error: %d\n", gCtrlGPIO);
		return 0;
    }

    return 1;
}

int dxl_hal_open(char *dev_name, float baudrate, int gpio_ctrl)
{
    if(!dxl_setup_serial(dev_name, baudrate))
        return 0;

    if(!dxl_setup_gpio(gpio_ctrl))
    {
        dxl_hal_close();
        return 0;
    }

    return 1;
}

/*
{
	struct termios newtio;
	struct serial_struct serinfo;
    int val = 0;

	strcpy(gDeviceName, dev_name);
	memset(&newtio, 0, sizeof(newtio));
	dxl_hal_close();

	if((gSocket_fd = open(gDeviceName, O_RDWR|O_NOCTTY|O_NONBLOCK)) < 0) {
		fprintf(stderr, "device open error: %s\n", dev_name);
		goto DXL_HAL_OPEN_ERROR;
	}

	newtio.c_cflag		= B38400|CS8|CLOCAL|CREAD;
	newtio.c_iflag		= IGNPAR;
	newtio.c_oflag		= 0;
	newtio.c_lflag		= 0;
	newtio.c_cc[VTIME]	= 0;	// time-out 값 (TIME * 0.1초) 0 : disable
	newtio.c_cc[VMIN]	= 0;	// MIN 은 read 가 return 되기 위한 최소 문자 개수

	tcflush(gSocket_fd, TCIFLUSH);
	tcsetattr(gSocket_fd, TCSANOW, &newtio);

	if(gSocket_fd == -1)
		return 0;

	if(ioctl(gSocket_fd, TIOCGSERIAL, &serinfo) < 0) {
		fprintf(stderr, "Cannot get serial info\n");
		return 0;
	}

    dxl_setBaud(newtio, baudrate);
	//serinfo.flags &= ~ASYNC_SPD_MASK;
	//serinfo.flags |= ASYNC_SPD_CUST;
	//serinfo.custom_divisor = serinfo.baud_base / baudrate;

    val = ioctl(gSocket_fd, TIOCSSERIAL, &serinfo);
	if(val < 0) {
		fprintf(stderr, "Cannot set serial info\n");
		return 0;
	}

	dxl_hal_close();

	gfByteTransTime = (float)((1000.0f / baudrate) * 12.0f);

	strcpy(gDeviceName, dev_name);
	memset(&newtio, 0, sizeof(newtio));
	dxl_hal_close();

	if((gSocket_fd = open(gDeviceName, O_RDWR|O_NOCTTY|O_NONBLOCK)) < 0) {
		fprintf(stderr, "device open error: %s\n", dev_name);
		goto DXL_HAL_OPEN_ERROR;
	}

	newtio.c_cflag		= B38400|CS8|CLOCAL|CREAD;
	newtio.c_iflag		= IGNPAR;
	newtio.c_oflag		= 0;
	newtio.c_lflag		= 0;
	newtio.c_cc[VTIME]	= 0;	// time-out 값 (TIME * 0.1초) 0 : disable
	newtio.c_cc[VMIN]	= 0;	// MIN 은 read 가 return 되기 위한 최소 문자 개수

	tcflush(gSocket_fd, TCIFLUSH);
	tcsetattr(gSocket_fd, TCSANOW, &newtio);


	return 1;

DXL_HAL_OPEN_ERROR:
	dxl_hal_close();
	return 0;
}
*/

void dxl_hal_close()
{
	if(gSocket_fd != -1)
		close(gSocket_fd);
	gSocket_fd = -1;

    if(gCtrlGPIO >= 0)
        gpio_unexport(gCtrlGPIO);
    gCtrlGPIO = -1;
}

int dxl_hal_set_baud( float baudrate )
{
	struct serial_struct serinfo;

	if(gSocket_fd == -1)
		return 0;

	if(ioctl(gSocket_fd, TIOCGSERIAL, &serinfo) < 0) {
		fprintf(stderr, "Cannot get serial info\n");
		return 0;
	}

	serinfo.flags &= ~ASYNC_SPD_MASK;
	serinfo.flags |= ASYNC_SPD_CUST;
	serinfo.custom_divisor = serinfo.baud_base / baudrate;

	if(ioctl(gSocket_fd, TIOCSSERIAL, &serinfo) < 0) {
		fprintf(stderr, "Cannot set serial info\n");
		return 0;
	}

	//dxl_hal_close();
	//dxl_hal_open(gDeviceName, baudrate);

	gfByteTransTime = (float)((1000.0f / baudrate) * 12.0f);
	return 1;
}

void dxl_hal_clear(void)
{
	tcflush(gSocket_fd, TCIFLUSH);
}

int dxl_hal_tx( unsigned char *pPacket, int numPacket )
{
    if(gCtrlGPIO >= 0 && !gpio_set_value(gCtrlGPIO, HIGH) )
    {
		fprintf(stderr, "Set Value GPIO error: %d\n", gCtrlGPIO);
		return 0;
    }

	int ret = write(gSocket_fd, pPacket, numPacket);

    if(gCtrlGPIO >= 0 && !gpio_set_value(gCtrlGPIO, LOW) )
    {
        fprintf(stderr, "Set Value GPIO error: %d\n", gCtrlGPIO);
        return 0;
    }

    return ret;
}

int dxl_hal_rx( unsigned char *pPacket, int numPacket )
{
	memset(pPacket, 0, numPacket);
	return read(gSocket_fd, pPacket, numPacket);
}

static inline long myclock()
{
	struct timeval tv;
	gettimeofday (&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void dxl_hal_set_timeout( int NumRcvByte )
{
	glStartTime = myclock();
	gfRcvWaitTime = (float)(gfByteTransTime*(float)NumRcvByte + 5.0f);
}

int dxl_hal_timeout(void)
{
	long time;

	time = myclock() - glStartTime;

	if(time > gfRcvWaitTime)
		return 1;
	else if(time < 0)
		glStartTime = myclock();

	return 0;
}
