#include"EF03157.h"

EF03157::EF03157(int RX, int TX) : _ser(RX,TX)
{
    _ser.begin(9600);
    _ser.flush();
    _ser.setTimeout(TIMEOUT);    
}

EF03157::EF03157(int RX, int TX, String ssid, String pwd) : EF03157(RX,TX)
{  
    this->Initialize(ssid, pwd);
}


/*************************************************************************
//configure the operation mode

	a:	
		1	-	Station
		2	-	AP
		3	-	AP+Station
		
	return:
		true	-	successfully
		false	-	unsuccessfully

***************************************************************************/
bool EF03157::confMode(byte a)
{    
	bool flag = false;
	_ser.flush();   
	_ser.print("AT+CWMODE=" + String(a) + "\r\n");     
	unsigned long start;
	start = millis();
	String data;

    while (millis()-start < TIMEOUT) {    		 
        data += (char)_ser.read();
        if (data.indexOf("OK") != -1 || data.indexOf("no change") != -1)
        {
            flag = true;
            break;
        }	    	  
	}
    return flag;
}


/*************************************************************************
//Initialize port

	mode:	setting operation mode
		STA: 	Station
		AP:	 	Access Point
		AT_STA:	Access Point & Station

	chl:	channel number
	ecn:	encryption
		OPEN          0
		WEP           1
		WAP_PSK       2
		WAP2_PSK      3
		WAP_WAP2_PSK  4		

	return:
		true	-	successfully
		false	-	unsuccessfully

***************************************************************************/
bool EF03157::Initialize(String ssid, String pwd, byte mode, byte chl, byte ecn)
{
	bool b = confMode(mode);
			
    if (!b)
    {
        return false; 
    }        
			
	Reset();

    switch(b)
    {
        case STA:
            b &= confJAP(ssid, pwd);
            break;
        case AP:
            b &= confSAP(ssid, pwd, chl, ecn);        
            break;
        case AP_STA:
            b &= confJAP(ssid, pwd);
		    b &= confSAP(ssid, pwd, chl, ecn);
            break;
	}
		
	return b;
}

/*************************************************************************

//reboot the wifi module

***************************************************************************/
void EF03157::Reset(void)
{			
	_ser.println("AT+RST");	

	bool result = _ser.find("OK");	
		
    if(result)
    {   		
		while(!_ser.find("[System Ready, Vendor:www.ai-thinker.com]"));
    }
	
	return result;
}

/*************************************************************************
//configure the SSID and password of the access port
		
		return:
		true	-	successfully
		false	-	unsuccessfully
		

***************************************************************************/
bool EF03157::confJAP(String ssid , String pwd)
{
	//Exp: AT+CWJAP="wifi-1","12345678"	
    _ser.print("AT+CWJAP=");
    _ser.print("\"");     //"ssid"
    _ser.print(ssid);
    _ser.print("\"");

    _ser.print(",");

    _ser.print("\"");      //"pwd"
    _ser.print(pwd);
    _ser.println("\"");


    unsigned long start;
	start = millis();
    while (millis() - start < CONNECTIONTIMOUT) {    
		if(_ser.find("OK"))
        {
           	return true;
        }       
	}
	return false;
}

/*************************************************************************
//configure the parameter of ssid, password, channel, encryption in AP mode
		
		return:
			true	-	successfully
			false	-	unsuccessfully

***************************************************************************/

bool EF03157::confSAP(String ssid , String pwd , byte chl , byte ecn)
{
    _ser.print("AT+CWSAP=");  
    _ser.print("\"");     //"ssid"
    _ser.print(ssid);
    _ser.print("\"");

    _ser.print(",");

    _ser.print("\"");      //"pwd"
    _ser.print(pwd);
    _ser.print("\"");

    _ser.print(",");
    _ser.print(String(chl));

    _ser.print(",");
    _ser.println(String(ecn));
	unsigned long start;
	start = millis();
    while (millis()-start < CONNECTIONTIMOUT) {                            
        if(_ser.find("OK"))
        {
           	return true;
        }
    }
	return false;
}

/*************************************************************************
//Set up tcp or udp connection	(signle connection mode)

	type:	tcp or udp
	
	addr:	ip address
	
	port:	port number
		

	return:
		true	-	successfully
		false	-	unsuccessfully

***************************************************************************/
bool EF03157::Open(String addr, int port, byte type)
{
	String data;
	_ser.print("AT+CIPSTART=");
	switch(type)
	{
	case 0:
		_ser.print("\"UDP\"");	
		break;
	case 1:
        _ser.print("\"TCP\"");
		break;		
	}
    _ser.print(",");
    _ser.print("\"");
    _ser.print(addr);
    _ser.print("\"");
    _ser.print(",");
	_ser.println(String(port));
	
    unsigned long start;
	start = millis();
	while (millis()-start < TIMEOUT) 
	{ 
     	if(_ser.available() > 0)
     	{
     		char a = _ser.read();
     		data = data + a;
     	}
		 
		if (data.indexOf("OK")!=-1 || data.indexOf("ALREAY CONNECT")!=-1)
    	{
        	return true;
     	}
	}
  	return false;
}

/*************************************************************************
//send data in sigle connection mode

	str:	string of message

	return:
		true	-	successfully
		false	-	unsuccessfully

***************************************************************************/
bool EF03157::Write(String str)
{
    _ser.print("AT+CIPSEND=");
    _ser.println(str.length());
    unsigned long start;
	start = millis();
	bool found;
	while (millis()-start < TIMEOUT) {                            
		if(_ser.find(">"))
		{
			found = true;
           	break;
        }
    }
	if(found)
	{
		_ser.print(str);
	}
	else
	{
		closeMux();
		return false;
	}

    String data;
    start = millis();
	while(millis() - start < TIMEOUT) {
    	while(_ser.available())
     	{
     		char a = _ser.read();
     		data=data+a;
     	}
     	if (data.indexOf("SEND OK")!=-1)
     	{
         	return true;
     	}
  	}
  	return false;
}

char EF03157::ReadByte()
{
	return (char)_ser.read();
}
