#pragma ones

#ifndef EF03157_H
#define EF03157_H

#include <SoftwareSerial.h>

class EF03157
{
private:
    SoftwareSerial _ser;

    bool _isServer;
    int8_t _mux;
    bool _echo = true;
    char* _buf;
    int _bufSize;
    unsigned long _timeout = 1000;

    void write(const char * str);
    void write(String str);
    //void write(const __FlashStringHelper* str);
    void writeLine();
    void writeLine(int n);
    void writeLine(char c);
    void writeLine(const char *str);
    //void writeLine(const __FlashStringHelper* str);
    void writeLine(String& str);
    int readLine();
    int Comparable(const char *left, const char *right);

    // 1 = Station, 2 = AP, 3 = Station + AP
    bool CWMode(int mode);
    bool setServer(bool mode);  
    bool setServer(bool mode,int port);

public:
    EF03157(int8_t rx, int8_t tx, int bufSize);
    EF03157(int8_t rx, int8_t tx);
    ~EF03157();    

    bool begin();
    bool begin(unsigned long t);
    void setTimeout(unsigned long timeout) {_ser.setTimeout(timeout);}; 
    unsigned long getTimeout(void) { return _ser.getTimeout(); }
    bool setBufferSize(int size);
    bool setBufferSize(int size, bool freeFirst);
    int getBufferSize();

    
    bool echoOn();
    bool echoOff();
    bool setEcho(bool set);    
    bool getEcho();

    bool reset();
    String version();

    bool setAP();
    bool setStation();
    bool setApStation();

    bool ConnectAP(String ssid, String pwd);
    bool ConnectAP(const char* ssid, const char* pwd);
    bool Connected();

    String getIP();

    bool MultConnection(int mux);
    bool serverInit();
    bool serverInit(int port);
    bool setServerTimeout(int sec);
    unsigned long getServerTimeout();

    int readLine(char* buf, int length);
}; 
#endif
