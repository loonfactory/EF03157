#ifndef EF03157_H
#define EF03157_H

#include <Arduino.h>
#include <SoftwareSerial.h>

#define TIMEOUT 3000
#define CONNECTIONTIMOUT 21000

#define    WEP           1
#define    WAP_PSK       2
#define    WAP2_PSK      3
#define    WAP_WAP2_PSK  4

#define    UDP     0
#define    TCP     1

#define    STA     1
#define    AP      2
#define    AP_STA  3

#endif




class EF03157
{
private:
    SoftwareSerial ef03157Serial;
public:

    EF03157(int RX, int TX);
    EF03157(int RX, int TX, String ssid, String pwd);
	//Initialize port
	bool Initialize(String ssid, String pwd, byte mode = STA, byte chl = 1, byte ecn = 2);
	bool ipConfig(byte type, String addr, int port, boolean a = 0, byte id = 0);
	
	bool Write(String str);  //send data in sigle connection mode
	bool Send(byte id, String str);  //send data int multiple connection mode
        
    char ReadByte();
	int Read(char *buf);
	
    //String begin(void);
    /*=================WIFI Function Command=================*/
    void Reset(void);    //reset the module
	bool confMode(byte a);   //set the working mode of module
	boolean confJAP(String ssid , String pwd);    //set the name and password of wifi 
	boolean confSAP(String ssid , String pwd , byte chl , byte ecn);       //set the parametter of SSID, password, channel, encryption in AP mode.
	
    String showMode(void);   //inquire the current mode of wifi module
    String showAP(void);   //show the list of wifi hotspot
    String showJAP(void);  //show the name of current wifi access port
    boolean quitAP(void);    //quit the connection of current wifi
    String showSAP(void);     //show the parameter of ssid, password, channel, encryption in AP mode

    /*================TCP/IP commands================*/
    String showStatus(void);    //inquire the connection status
    String showMux(void);       //show the current connection mode(sigle or multiple)
    boolean confMux(boolean a);    //set the connection mode(sigle:0 or multiple:1)
    bool Open(String addr, int port, byte type = TCP);   //create new tcp or udp connection (sigle connection mode)
    bool Open(byte id, byte type, String addr, int port);   //create new tcp or udp connection (multiple connection mode)(id:0-4) 
    void closeMux(void);   //close tcp or udp (sigle connection mode)
    void closeMux(byte id); //close tcp or udp (multiple connection mode)
    String showIP(void);    //show the current ip address
    boolean confServer(byte mode, int port);  //set the parameter of server
	
	String m_rev;

};
