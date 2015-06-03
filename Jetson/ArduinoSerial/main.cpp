#include <iostream>
#include "ofSerialLinux.h"

using namespace std;

int main()
{
    ofSerial _port;

    _port.enumerateDevices();
    //bool connected = _port.setup("ttyTHS0", 1000000);  //BR_UART_1
    bool connected = _port.setup("ttyTHS1", 1000000);  //UART2_TXD

    //_port.writeByte('A');

    while(!_port.available()) {}

    while(_port.available())
    {
        char c = _port.readByte();
        std::cout << c;
    }
    std::cout << std::endl;

    std::string data = "This is a test!";
    _port.writeBytes((unsigned char *) data.c_str(), data.length());

    _port.close();

    return 0;
}
