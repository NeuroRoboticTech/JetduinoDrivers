
#include "ofSerialLinux.h"
#include "ofTypes.h"

#if defined( TARGET_OSX ) || defined( TARGET_LINUX )

#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/serial.h>
#include <getopt.h>

#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <algorithm>

#pragma comment (lib, "Setupapi.lib")

//----------------------------------------------------------------
ofSerial::ofSerial(){

    port_fd = -1;
	port_is_open = 0;
	baud_rate = 38400;	// default baud rate
}

//----------------------------------------------------------------
ofSerial::~ofSerial(){

	close();
}

//----------------------------------------------------------------
static bool isDeviceArduino( ofSerialDeviceInfo & A ){
	//TODO - this should be ofStringInString
	return ( strstr(A.getDeviceName().c_str(), "usbserial") != NULL ||
		 strstr(A.getDeviceName().c_str(), "usbmodem") != NULL );
}

//----------------------------------------------------------------
void ofSerial::buildDeviceList(){

	deviceType = "serial";
	devices.clear();

	std::vector <std::string> prefixMatch;

    prefixMatch.push_back("ttyS");
    prefixMatch.push_back("ttyUSB");
    prefixMatch.push_back("rfc");

	DIR *dir;
	struct dirent *entry;
	dir = opendir("/dev");

	std::string deviceName	= "";
	int deviceCount		= 0;

	if (dir == NULL){
		std::cout << "buildDeviceList(): error listing devices in /dev" << "\r\n";
	} else {
		//for each device
		while((entry = readdir(dir)) != NULL){
			deviceName = (char *)entry->d_name;

			//we go through the prefixes
			for(int k = 0; k < (int)prefixMatch.size(); k++){
				//if the device name is longer than the prefix
				if( deviceName.size() > prefixMatch[k].size() ){
					//do they match ?
					if( deviceName.substr(0, prefixMatch[k].size()) == prefixMatch[k].c_str() ){
						devices.push_back(ofSerialDeviceInfo("/dev/"+deviceName, deviceName, deviceCount));
						deviceCount++;
						break;
					}
				}
			}
		}
		closedir(dir);
	}

	//here we sort the device to have the aruino ones first.
	partition(devices.begin(), devices.end(), isDeviceArduino);
	//we are reordering the device ids. too!
	for(int k = 0; k < (int)devices.size(); k++){
		devices[k].deviceID = k;
	}

	bHaveEnumeratedDevices = true;
}


//----------------------------------------------------------------
void ofSerial::listDevices(){
	buildDeviceList();
	for(int k = 0; k < (int)devices.size(); k++){
		std::cout << "[" << devices[k].getDeviceID() << "] = "<< devices[k].getDeviceName().c_str() << "\r\n";
	}
}

//----------------------------------------------------------------
std::vector <ofSerialDeviceInfo> ofSerial::getDeviceList(){
	buildDeviceList();
	return devices;
}

//----------------------------------------------------------------
void ofSerial::enumerateDevices(){
	listDevices();
}

//----------------------------------------------------------------
void ofSerial::close(){

    if (port_is_open){
        std::cout << "closing serial" << "\r\n";
        drain();
        flush(true, false);
        port_is_open = false;
        port_name = "";
        tcsetattr(port_fd, TCSANOW, &settings_orig);
        ::close(port_fd);
    }

}

//----------------------------------------------------------------
bool ofSerial::setup(){
	return setup(0,9600);		// the first one, at 9600 is a good choice...
}

//----------------------------------------------------------------
bool ofSerial::setup(int deviceNumber, int baud){

	buildDeviceList();
	if( deviceNumber < (int)devices.size() ){
		return setup(devices[deviceNumber].devicePath, baud);
	}else{
		std::cout << "couldn't find device " << deviceNumber << ", only " << devices.size() << " devices found" << "\r\n";
		return false;
	}

}

//----------------------------------------------------------------
bool ofSerial::setup(std::string portName, int baud){

	close();

	std::cout << "setup serial" << "\r\n";
	port_is_open = false;


    //lets account for the name being passed in instead of the device path
    if( portName.size() > 5 && portName.substr(0, 5) != "/dev/" ){
        portName = "/dev/" + portName;
    }

    baud_rate = baud;

    std::cout << "opening " << portName << " @ " << baud << " bps" << "\r\n";
	struct serial_struct kernel_serial_settings;
	int bits;
	port_fd = open(portName.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (port_fd < 0) {
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
	if (ioctl(port_fd, TIOCMGET, &bits) < 0) {
		::close(port_fd);
		error_msg = "Unable to query serial port signals";
        std::cout << error_msg << "\r\n";
		return false;
	}
	bits &= ~(TIOCM_DTR | TIOCM_RTS);
	if (ioctl(port_fd, TIOCMSET, &bits) < 0) {
		::close(port_fd);
		error_msg = "Unable to control serial port signals";
        std::cout << error_msg << "\r\n";
		return false;
	}
	if (tcgetattr(port_fd, &settings_orig) != 0) {
		::close(port_fd);
		error_msg = "Unable to query serial port settings (perhaps not a serial port)";
        std::cout << error_msg << "\r\n";
		return false;
	}
	memset(&settings, 0, sizeof(settings));
	settings.c_iflag = IGNBRK | IGNPAR;
	settings.c_cflag = CS8 | CREAD | HUPCL | CLOCAL;
	setBaud(baud_rate);
	if (ioctl(port_fd, TIOCGSERIAL, &kernel_serial_settings) == 0) {
		kernel_serial_settings.flags |= ASYNC_LOW_LATENCY;
		ioctl(port_fd, TIOCSSERIAL, &kernel_serial_settings);
	}
	tcflush(port_fd, TCIFLUSH);

	port_name = portName;
	port_is_open = true;
    std::cout << "opened " << portName << " sucessfully @ " << baud << " bps" << "\r\n";
	return true;
}

// set the baud rate
bool ofSerial::setBaud(int baud)
{
	if (baud <= 0) return false;
	std::cout << "set_baud: " << baud << "\n";
	baud_rate = baud;
	if (port_fd < 0) return false;
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
			return false;
		}
	}
	cfsetospeed(&settings, spd);
	cfsetispeed(&settings, spd);
	if (tcsetattr(port_fd, TCSANOW, &settings) < 0) return false;

	return true;
}

bool isInteger(const std::string & s)
{
   if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false ;

   char * p ;
   strtol(s.c_str(), &p, 10) ;

   return (*p == 0) ;
}

bool ofSerial::setBaud(const std::string& baud_str)
{
	unsigned long b;
	if (!isInteger(baud_str)) return false;
	return setBaud((int)b);
}

//----------------------------------------------------------------
int ofSerial::writeBytes(unsigned char * buffer, int length){

	if (!port_is_open){
		std::cout << "writeBytes(): serial not inited" << "\r\n";
		return OF_SERIAL_ERROR;
	}

	int n, written=0;
	fd_set wfds;
	struct timeval tv;
	while (written < length) {
		n = write(port_fd, (const char *)buffer + written, length - written);
		if (n < 0 && (errno == EAGAIN || errno == EINTR)) n = 0;
		//std::cout << "Write, n = " <<  n << "\n";
		if (n < 0) return -1;
		if (n > 0) {
			written += n;
		} else {
			tv.tv_sec = 10;
			tv.tv_usec = 0;
			FD_ZERO(&wfds);
			FD_SET(port_fd, &wfds);
			n = select(port_fd+1, NULL, &wfds, NULL, &tv);
			if (n < 0 && errno == EINTR) n = 1;
			if (n <= 0) return -1;
		}
	}
	return written;
}

//----------------------------------------------------------------
int ofSerial::readBytes(unsigned char * buffer, int length){

	if (!port_is_open){
		std::cout << "readBytes(): serial not inited" << "\r\n";
		return OF_SERIAL_ERROR;
	}

    int nRead = read(port_fd, buffer, length);
    if(nRead < 0){
        if ( errno == EAGAIN )
            return OF_SERIAL_NO_DATA;
        //std::cout << "readBytes(): couldn't read from port: " << errno << " " << strerror(errno) << "\r\n";
        return OF_SERIAL_ERROR;
    }
    return nRead;
}

//----------------------------------------------------------------
bool ofSerial::writeByte(unsigned char singleByte){

	if (!port_is_open){
		std::cout << "writeByte(): serial not inited" << "\r\n";
		//return OF_SERIAL_ERROR; // this looks wrong.
		return false;
	}

	unsigned char tmpByte[1];
	tmpByte[0] = singleByte;

    int numWritten = 0;
    numWritten = write(port_fd, tmpByte, 1);
    if(numWritten <= 0 ){
        if ( errno == EAGAIN )
            return 0;
         //std::cout << "writeByte(): couldn't write to port: " << errno << " " << strerror(errno) << "\r\n";
         //return OF_SERIAL_ERROR; // this looks wrong.
         return false;
    }
    //std::cout << "wrote byte" << "\r\n";

    return (numWritten > 0 ? true : false);
}

//----------------------------------------------------------------
int ofSerial::readByte(){

	if (!port_is_open){
		std::cout << "readByte(): serial not inited" << "\r\n";
		return OF_SERIAL_ERROR;
	}

	unsigned char tmpByte[1];
	memset(tmpByte, 0, 1);

    int nRead = read(port_fd, tmpByte, 1);
    if(nRead < 0){
        if ( errno == EAGAIN )
            return OF_SERIAL_NO_DATA;
        std::cout << "readByte(): couldn't read from port: " << errno << " " << strerror(errno) << "\r\n";
        return OF_SERIAL_ERROR;
    }
    if(nRead == 0)
        return OF_SERIAL_NO_DATA;

	return (int)(tmpByte[0]);
}


//----------------------------------------------------------------
void ofSerial::flush(bool flushIn, bool flushOut){

	if (!port_is_open){
		std::cout << "flush(): serial not inited" << "\r\n";
		return;
	}

	int flushType = 0;

    if( flushIn && flushOut) flushType = TCIOFLUSH;
    else if(flushIn) flushType = TCIFLUSH;
    else if(flushOut) flushType = TCOFLUSH;
    else return;

    tcflush(port_fd, flushType);
}

void ofSerial::drain(){
    if (!port_is_open){
		std::cout << "drain(): serial not inited" << "\r\n";
		return;
    }

    tcdrain( port_fd );
}

//-------------------------------------------------------------
int ofSerial::available(){

	if (!port_is_open){
		std::cout << "available(): serial not inited" << "\r\n";
		return OF_SERIAL_ERROR;
	}

	int numBytes = 0;

    ioctl(port_fd,FIONREAD,&numBytes);

	return numBytes;
}

bool ofSerial::isInitialized() const{
	return port_is_open;
}

#endif
