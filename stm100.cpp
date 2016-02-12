#include "stdafx.h"

//default constructor
stm100::stm100(){

	setdensityready = false;
	setzfactorready = false;
	zerothicknesstimer = false;

}

//default destructor
stm100::~stm100(){

	ClosePort();

}

//default copy constructor
stm100::stm100(const stm100 & rhs){

	//coming soon
	//adding copy of the current values but not the handles

}

// open the communications with the serial port
bool stm100::OpenPort(const char* port){

	//open the com port file handle
	m_hSerialComm = CreateFile(port, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (m_hSerialComm == INVALID_HANDLE_VALUE) 
	{
		//indicate error
		return false;
	}else
	{
		//start the polling the thread
		Terminate = false;
		m_hThread = CreateThread(NULL,0,pollCOM,this,0,&m_dwThreadId);
		return true;
	}

}

// close the serial port
void stm100::ClosePort(){

	Terminate = true; //signal polling thread to return

	if(m_hSerialComm != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hSerialComm);
		m_hSerialComm = INVALID_HANDLE_VALUE;
	}

}

void stm100::readCom(char pszBuf[],DWORD dwSize, HANDLE hSerialComm){

	DWORD dwEventMask;
	DWORD dwIncommingReadSize;

	if(!SetCommMask(hSerialComm, EV_RXCHAR)) MessageBox(NULL,"Error Setting COMM mask","info",MB_OK);

	if(WaitCommEvent(hSerialComm, &dwEventMask, NULL))
	{
			    
		if(ReadFile(hSerialComm, pszBuf, dwSize, &dwIncommingReadSize, NULL) != 0)
		{

			pszBuf[dwIncommingReadSize+1] = char(0);
				      
		}

		else
			MessageBox(NULL,"Error: Failed to read message","info",MB_OK);
	}

	else
	{
		MessageBox(NULL,"Error: WaitCommEvent returned FALSE","info",MB_OK);
	}

}

void stm100::writeCom(char pszBuf[],DWORD dwSize, HANDLE hSerialComm){

	unsigned long dwNumberOfBytesWritten;

	if(WriteFile(hSerialComm, pszBuf, dwSize, &dwNumberOfBytesWritten, NULL) != 0)
	{
		if(dwNumberOfBytesWritten != dwSize) MessageBox(NULL,"Error Writing: number of bytes written incorrect","info",MB_OK);
	}

	else
		MessageBox(NULL,"Error: Failed to write message","info",MB_OK);

}

float stm100::readRate(HANDLE hSerialComm){

	char pszBuf[16];
	float rate;

	//message to request current rate "<STX><length>T<checksum>"
	sprintf(pszBuf,"%c%cTT",char(2),char(1));

	//Send the message to the controller
	writeCom(pszBuf,4,hSerialComm);

	//Receive the acknowledgement
	readCom(pszBuf,10,hSerialComm);

	//extract the data
	sscanf(pszBuf,"%*c%*c%*c%f%*c",&rate);

	return rate;

}

float stm100::readThickness(HANDLE hSerialComm){

	char pszBuf[16];
	float thickness;

	//message to request current thickness "<STX><length>S<checksum>"
	sprintf(pszBuf,"%c%cSS",char(2),char(1));

	//Send the message to the controller
	writeCom(pszBuf,4,hSerialComm);


	//Receive the acknowledgement
	readCom(pszBuf,12,hSerialComm);

	//extract the data
	sscanf(pszBuf,"%*c%*c%*c%f%*c",&thickness);

	return thickness;

}

float stm100::readDensity(HANDLE hSerialComm){

	char pszBuf[16];
	float density;

	//message to request current density "<STX><length>E?<checksum>"
	sprintf(pszBuf,"%c%cE?%c",char(2),char(2),char(69+63));

	//Send the message to the controller
	writeCom(pszBuf,5,hSerialComm);

	//Receive the acknowledgement
	readCom(pszBuf,10,hSerialComm);

	//extract the data
	sscanf(pszBuf,"%*c%*c%*c%f%*c",&density);

	return density;

}

float stm100::readZFactor(HANDLE hSerialComm){

	char pszBuf[16];
	float zfactor;

	//message to request current Z-factor "<STX><length>F?<checksum>"
	sprintf(pszBuf,"%c%cF?%c",char(2),char(2),char(70+63));

	//Send the message to the controller
	writeCom(pszBuf,5,hSerialComm);

	//Receive the acknowledgement
	readCom(pszBuf,9,hSerialComm);

	//extract the data
	sscanf(pszBuf,"%*c%*c%*c%f%*c",&zfactor);

	return zfactor;

}

float stm100::readLife(HANDLE hSerialComm){

	char pszBuf[16];
	float life;

	//message to request remaining life "<STX><length>V<checksum>"
	sprintf(pszBuf,"%c%cVV",char(2),char(1));

	//Send the message to the controller
	writeCom(pszBuf,4,hSerialComm);


	//Receive the acknowledgement
	readCom(pszBuf,10,hSerialComm);

	//extract the data
	sscanf(pszBuf,"%*c%*c%*c%f%*c",&life);

	return life;

}

int stm100::readFreq(HANDLE hSerialComm){

	char pszBuf[16];
	int freq;

	//message to request current frequency "<STX><length>U<checksum>"
	sprintf(pszBuf,"%c%cUU",char(2),char(1));

	//Send the message to the controller
	writeCom(pszBuf,4,hSerialComm);


	//Receive the acknowledgement
	readCom(pszBuf,11,hSerialComm);

	//extract the data
	sscanf(pszBuf,"%*c%*c%*c%d%*c",&freq);

	return freq;

}

tm stm100::readTime(HANDLE hSerialComm){

	char pszBuf[16];
	tm   time;

	//message to request the counters current time "<STX><length>W<checksum>"
	sprintf(pszBuf,"%c%cWW",char(2),char(1));

	//Send the message to the controller
	writeCom(pszBuf,4,hSerialComm);


	//Receive the acknowledgement
	readCom(pszBuf,10,hSerialComm);

	//extract the data
	sscanf(pszBuf,"%*c%*c%*c%*c%d%*c%d%*c",&time.tm_min,&time.tm_sec);

	return time;

}

void stm100::writeDensity(HANDLE hSerialComm, float density){

	char pszBuf_nochecksum[16], pszBuf[16];
	unsigned char checksum;
	int i;

	//message to set the current density "<STX><length>E=<value><checksum>"
	sprintf(pszBuf_nochecksum,"E=%05.2f",density);
	
	//calculate checksum
	checksum = 0;
	for(i=0;i<7;i++){
		checksum += pszBuf_nochecksum[i];
	}
	sprintf(pszBuf,"%c%c%s%c",char(2),char(7),pszBuf_nochecksum,checksum);

	//Send the message to the controller
	writeCom(pszBuf,10,hSerialComm);

	//Receive the acknowledgement
	readCom(pszBuf,4,hSerialComm);

	//for now do nothing with the response

}

void stm100::writeZFactor(HANDLE hSerialComm, float zfactor){

	char pszBuf_nochecksum[16], pszBuf[16];
	unsigned char checksum;
	int i;

	//message to set the current Z-factor "<STX><length>F=<value><checksum>"
	sprintf(pszBuf_nochecksum,"F=%05.3f",zfactor);

	//calculate checksum
	checksum = 0;
	for(i=0;i<7;i++){
		checksum += pszBuf_nochecksum[i];
	}
	sprintf(pszBuf,"%c%c%s%c",char(2),char(7),pszBuf_nochecksum,checksum);

	//Send the message to the controller
	writeCom(pszBuf,10,hSerialComm);

	//Receive the acknowledgement
	readCom(pszBuf,4,hSerialComm);

	//for now do nothing with the response

}

void stm100::writeZeroThicknessTimer(HANDLE hSerialComm){

	char pszBuf[16];

	//message to zero thicnkess and timer "<STX><length>B<checksum>"
	sprintf(pszBuf,"%c%cBB",char(2),char(1));

	//Send the message to the controller
	writeCom(pszBuf,4,hSerialComm);

	//Receive the acknowledgement
	readCom(pszBuf,4,hSerialComm);

	//for now do nothing with the response

}

void stm100::SetDensity(float new_density){

	//flag density for change
	m_newdensity = new_density;
	setdensityready = true;

}

void stm100::SetZFactor(float new_zfactor){

	//flag z-factor for change
	m_newzfactor = new_zfactor;
	setzfactorready = true;

}

void stm100::ZeroThicknessTimer(){

	//flag the timer and thickness to be zeroed
	zerothicknesstimer = true;

}

void stm100::setDCB(HANDLE hSerialComm){

	DCB dcbConfig;

	if(GetCommState(hSerialComm, &dcbConfig))
	{
	    dcbConfig.BaudRate = 9600;
	    dcbConfig.ByteSize = 8;
	    dcbConfig.Parity = NOPARITY;
	    dcbConfig.StopBits = ONESTOPBIT;
	    dcbConfig.fBinary = TRUE;
	    dcbConfig.fParity = TRUE;
	}

	else
	{
	    MessageBox(NULL,"Error getting COMM state","info",MB_OK);
	}

	if(!SetCommState(hSerialComm, &dcbConfig)){
		MessageBox(NULL,"Error setting COMM state","info",MB_OK);
	}

}

void stm100::setTimeouts(HANDLE hSerialComm){

	COMMTIMEOUTS commTimeout;

	if(GetCommTimeouts(hSerialComm, &commTimeout))
	{
	    commTimeout.ReadIntervalTimeout     = 50;
	    commTimeout.ReadTotalTimeoutConstant     = 50;
	    commTimeout.ReadTotalTimeoutMultiplier     = 50;
	    commTimeout.WriteTotalTimeoutConstant     = 1000 * 5;
	    commTimeout.WriteTotalTimeoutMultiplier = 1000 * 5;
	}

	else
	{
	    MessageBox(NULL,"Error getting COMM timeouts","info",MB_OK);
	}

	if(!SetCommTimeouts(hSerialComm, &commTimeout)){
		MessageBox(NULL,"Error setting COMM timeouts","info",MB_OK);
	}

}

/* This function continuously polls the values from the
   STM100 controller over the serial COM port */
DWORD WINAPI stm100::pollCOM(LPVOID lpParam)
{
	stm100* thisObject = (stm100*)lpParam;
	HANDLE hSerialComm = thisObject->m_hSerialComm;

	//Setup the communications
	setDCB(hSerialComm);
	setTimeouts(hSerialComm);

	// if the Terminate signal (changes to TRUE) is received 
	// the thread exits the loop and returns 0
	while(!thisObject->Terminate){

		if(thisObject->setdensityready){ //if flagged for change set the new value

			writeDensity(hSerialComm,thisObject->m_newdensity);
			thisObject->setdensityready = false;

		}

		if(thisObject->setzfactorready){ //if flagged for change set the new value

			writeZFactor(hSerialComm,thisObject->m_newzfactor);
			thisObject->setzfactorready = false;

		}

		if(thisObject->zerothicknesstimer){ //if flagged for zeroing send the message

			writeZeroThicknessTimer(hSerialComm);
			thisObject->zerothicknesstimer = false;

		}


		//read all the current values from the controller

		thisObject->m_rate      = readRate(hSerialComm);

		thisObject->m_thickness = readThickness(hSerialComm);

		thisObject->m_density   = readDensity(hSerialComm);

		thisObject->m_zfactor   = readZFactor(hSerialComm);

		thisObject->m_life      = readLife(hSerialComm);

		thisObject->m_freq      = readFreq(hSerialComm);

		thisObject->m_time      = readTime(hSerialComm);

	}

	return 0;

}