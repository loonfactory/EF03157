#include"EF03157.h"

EF03157::EF03157(int RX, int TX) : ef03157Serial(RX,TX)
{
    ef03157Serial.begin(9600);
    ef03157Serial.flush();
    ef03157Serial.setTimeout(TIMEOUT);    
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
	ef03157Serial.flush();   
	ef03157Serial.print("AT+CWMODE=" + String(a) + "\r\n");     
	unsigned long start;
	start = millis();
	String data;

    while (millis()-start < TIMEOUT) {    		 
        data += (char)ef03157Serial.read();
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
		DBG_Write_Line("Initialize: FALSE");
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
		
	DBG_Write("Initialize: ");
	DBG_Write_Line(b ? "TRUE" : "FALSE");
	return b;
}

/*************************************************************************

//reboot the wifi module

***************************************************************************/
void EF03157::Reset(void)
{			
	ef03157Serial.println("AT+RST");	

	bool result = ef03157Serial.find("OK");	
		
    if(result)
    {   		
		while(!ef03157Serial.find("[System Ready, Vendor:www.ai-thinker.com]"));
		DBG_Write_Line("Module is ready");  		
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
    ef03157Serial.print("AT+CWJAP=");
    ef03157Serial.print("\"");     //"ssid"
    ef03157Serial.print(ssid);
    ef03157Serial.print("\"");

    ef03157Serial.print(",");

    ef03157Serial.print("\"");      //"pwd"
    ef03157Serial.print(pwd);
    ef03157Serial.println("\"");


    unsigned long start;
	start = millis();
    while (millis() - start < CONNECTIONTIMOUT) {    
		if(ef03157Serial.find("OK"))
        {
			DBG_Write_Line("confSAP: TRUE");
           	return true;
        }       
	}
	DBG_Write_Line("confJAP: FALSE");
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
    ef03157Serial.print("AT+CWSAP=");  
    ef03157Serial.print("\"");     //"ssid"
    ef03157Serial.print(ssid);
    ef03157Serial.print("\"");

    ef03157Serial.print(",");

    ef03157Serial.print("\"");      //"pwd"
    ef03157Serial.print(pwd);
    ef03157Serial.print("\"");

    ef03157Serial.print(",");
    ef03157Serial.print(String(chl));

    ef03157Serial.print(",");
    ef03157Serial.println(String(ecn));
	unsigned long start;
	start = millis();
    while (millis()-start < CONNECTIONTIMOUT) {                            
        if(ef03157Serial.find("OK"))
        {
			DBG_Write_Line("confSAP: TRUE");
           	return true;
        }
    }
	DBG_Write_Line("confSAP: FALSE");
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
	ef03157Serial.print("AT+CIPSTART=");
	switch(type)
	{
	case 0:
		ef03157Serial.print("\"UDP\"");	
		break;
	case 1:
        ef03157Serial.print("\"TCP\"");
		break;		
	}
    ef03157Serial.print(",");
    ef03157Serial.print("\"");
    ef03157Serial.print(addr);
    ef03157Serial.print("\"");
    ef03157Serial.print(",");
	ef03157Serial.println(String(port));
	
    unsigned long start;
	start = millis();
	while (millis()-start < TIMEOUT) 
	{ 
     	if(ef03157Serial.available() > 0)
     	{
     		char a = ef03157Serial.read();
     		data = data + a;
     	}
		 
		if (data.indexOf("OK")!=-1 || data.indexOf("ALREAY CONNECT")!=-1)
    	{
			DBG_Write_Line("open: " + addr );
        	return true;
     	}
	}
	DBG_Write_Line("open: FALSE");
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
    ef03157Serial.print("AT+CIPSEND=");
    ef03157Serial.println(str.length());
    unsigned long start;
	start = millis();
	bool found;
	while (millis()-start < TIMEOUT) {                            
		if(ef03157Serial.find(">"))
		{
			found = true;
           	break;
        }
    }
	if(found)
	{
		ef03157Serial.print(str);
	}
	else
	{
		closeMux();
		return false;
	}

    String data;
    start = millis();
	while(millis() - start < TIMEOUT) {
    	while(ef03157Serial.available())
     	{
     		char a = ef03157Serial.read();
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
	return (char)ef03157Serial.read();
}
