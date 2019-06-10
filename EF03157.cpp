#include "EF03157.h"

EF03157::EF03157(int8_t rx, int8_t tx) : EF03157(rx, tx, 100) {}
EF03157::EF03157(int8_t rx, int8_t tx, int bufSize) : _ser(rx, tx) {
	setBufferSize(bufSize);
}
EF03157::~EF03157(){
	free(_buf);
}

int EF03157::readLine(){
	return readLine(_buf, _bufSize);
}

int EF03157::readLine(char *buf, int length){	

	if(buf == nullptr) return -1;
	if(length < 0) return -1;
	
	int i = -1;

	if(_ser.isListening() == false) 
		_ser.listen();

    unsigned long t = millis() + _timeout;	
	do
	{ 
		int ch = _ser.read();
        if(ch == -1) continue;		
		if(ch == '\r' || ch == '\n'){
			delay(2);
			if(_ser.peek() == '\n') _ser.read();				
			break;
		}
		buf[++i] = ch;

	} while (i < length && millis() < t);	
	
	buf[++i] = 0;
	return i;
}

void EF03157::write(const char *str){	
	_ser.write(str);
}
void EF03157::write(String str){
	_ser.write(str.c_str());
}
/*
void EF03157::write(const __FlashStringHelper* str){	
	_ser.write(str);
}
*/
void EF03157::writeLine(){
	_ser.write('\r');
	_ser.write('\n');
}
void EF03157::writeLine(char c){
	_ser.write(c);
	writeLine();
}
void EF03157::writeLine(String& str){	
	_ser.write(str.c_str());
	writeLine();
}
/*
void EF03157::writeLine(const __FlashStringHelper* str){
	_ser.write(str);
	writeLine();
}
*/
void EF03157::writeLine(const char *str){	
	_ser.write(str);
	writeLine();
}

bool EF03157::CWMode(int mode){
	if(mode > 3 || mode <= 0) return false;
	write("AT+CWMODE=");
	writeLine(mode + '0');
	bool result = readLine();
	String s("Error");
	if(result){		
		result != s.equals(_buf);
		readLine();
	}
	return result;
}
bool EF03157::setServer(bool mode){
	write("AT+CIPSERVER=");
	writeLine((mode != 0) + '0');
	readLine();
	readLine();
	return  _buf[0] == 'O' && _buf[1] == 'K';
}
bool EF03157::setServer(bool mode, int port){
	_ser.write("AT+CIPSERVER=");
	_ser.write((mode != 0) + '0');
	_ser.write(',');
	_ser.write(String(port).c_str());
	readLine();
	readLine();
	return  _buf[0] == 'O' && _buf[1] == 'K';
}

bool EF03157::setBufferSize(int size) { return setBufferSize(size, false);  }
bool EF03157::setBufferSize(int size, bool freeFirst){
	bool result;
	if(freeFirst){
		free(_buf);
		_buf = (char*)malloc(sizeof(char) *  size );
		result = _buf != nullptr;
		size = result ? size : -1;
	}
	else{
		char* temp = (char*)malloc(sizeof(char) *  size );
		free(_buf);		
		result = temp != nullptr;
		if(result){
			_buf = temp;
			_bufSize = size;			
		} 		
	}	
	return result;
}

int EF03157::getBufferSize(){return _bufSize;}

bool EF03157::begin(){
	return begin(9600);	
}

bool EF03157::begin(unsigned long t){
	_ser.begin(t);
	_ser.setTimeout(1000);
	while (_ser.available() != 0) _ser.read();

	writeLine("AT");
	delay(10);

	if(_ser.available() > 0){
		_echo = readLine();		
		while (_ser.available() != 0) _ser.read();
		return true;
	}

	return false;	
}

bool EF03157::echoOn() {  return setEcho(true); }
bool EF03157::echoOff() { return setEcho(false); }

bool EF03157::setEcho(bool set){
	if(_echo == set) return true;

	_ser.write("ATE");
	writeLine((set != 0) + '0');
	String ok = "OK";
	if(_echo){
		readLine();
		readLine();
	}
	readLine();
	readLine();	
	bool r = ok.equals(_buf);
	if(r) _echo = set;
	return r;
}

bool EF03157::getEcho(){ return _echo; }

bool EF03157::reset(){
	while (_ser.available()) _ser.read();
	writeLine("AT+RST");		
	delay(10);
	if(_echo){
		readLine();
		readLine();
	}
	readLine();
	if(readLine())
	{		
		while (_ser.available() == 0) ;			
		_echo = true;
		delay(500);
		while (_ser.available()) _ser.read();
		
		return true;			
	}		
	return false;
}

String EF03157::version(){
	writeLine("AT+GMR");
	if(_echo){
		readLine();
		readLine();
	}
	readLine(); // {version} CR LK
	String v(_buf);
	readLine(); // CR LF;
	readLine(); // OK CR LF
	return v;	
}

bool EF03157::setAP(){
	return CWMode(2);
}
bool EF03157::setStation(){
	return CWMode(1);
}
bool EF03157::setApStation(){
	return CWMode(3);
}
bool EF03157::ConnectAP(String ssid, String pwd){
	return ConnectAP(ssid.c_str(), pwd.c_str());
}
bool EF03157::ConnectAP(const char* ssid, const char* pwd){
	write("AT+CWJAP=\"");
	write(ssid);
	write("\",\"");
	write(pwd);
	writeLine('\"');
	if(_echo){
		readLine(); 
		readLine();
	}
	if(readLine()){		
		readLine();
		return _buf[0] == 'O' && _buf[1] == 'K';
	}
	return false;
}
bool EF03157::Connected(){
	write("AT+CWJAP?");
	int retry = 0;
	while (retry++ < 10)
	{
		if(readLine() == false) continue;
		
		readLine();
		String er = "Error";
		if(er.equals(_buf)) return false;
		else return true;					
	}
		
}
String EF03157::getIP(){
	writeLine("AT+CIFSR");
	if(_echo){
		readLine();
		readLine();
	}
	readLine();
	String ip(_buf);
	readLine();
	readLine();

	return ip;
}

bool EF03157::MultConnection(int mux){
	if(mux < 0) return false;
	if(mux > 4) return false; // up to 4	
	bool result = false;
	write("AT+CIPMUX=");
	writeLine(mux + '0');	
	if(readLine()){
		readLine();
		result = _buf[0] == 'O' && _buf[1] == 'K';	
		if(result) _mux = mux;
	}
	return result;
}
bool EF03157::serverInit(){
	if(_mux < 1) MultConnection(1);		
	return setServer(1);
}
bool EF03157::serverInit(int port){
	if(_mux < 1) MultConnection(1);	
	return setServer(1, port);
}
