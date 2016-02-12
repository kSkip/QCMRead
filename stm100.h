#include <windows.h>
#include <cstdio>
#include <ctime>

#ifndef STM100_H
#define STM100_H

class stm100{

	public:
		//constructors
		stm100();
		~stm100();
		stm100(const stm100 & rhs);

		//get methods
		float GetRate()     {return m_rate;}
		float GetThickness(){return m_thickness;}
		float GetDensity()  {return m_density;}
		float GetZFactor()  {return m_zfactor;}
		float GetLife()     {return m_life;}
		int   GetFreq()     {return m_freq;}
		tm    GetTime()     {return m_time;}

		//set methods
		void SetDensity(float new_density);
		void SetZFactor(float new_zfactor);

		void ZeroThicknessTimer();

		//open/close functions for the serial port
		bool OpenPort(const char* port);
		void ClosePort();

	private:

		//the process value member variables
		float m_rate, m_thickness, m_life, m_density, m_zfactor;
		int m_freq;
		tm m_time;

		//set values
		float m_newdensity;
		float m_newzfactor;

		bool setdensityready;
		bool setzfactorready;

		bool zerothicknesstimer;

		//functions and members that handle communications
		HANDLE m_hThread;
		DWORD m_dwThreadId;
		HANDLE m_hSerialComm;
		bool Terminate;
		static DWORD WINAPI pollCOM(LPVOID lpParam);
		static void setDCB(HANDLE hSerialComm);
		static void setTimeouts(HANDLE hSearialComm);

		static void readCom(char pszBuf[], DWORD dwSize, HANDLE hSerialComm);
		static void writeCom(char pszBuf[], DWORD dwSize, HANDLE hSerialComm);

		static float readRate(HANDLE hSerialComm);
		static float readThickness(HANDLE hSerialComm);
		static float readLife(HANDLE hSerialComm);
		static float readDensity(HANDLE hSerialComm);
		static float readZFactor(HANDLE hSerialComm);
		static int   readFreq(HANDLE hSerialComm);
		static tm    readTime(HANDLE hSerialComm);

		static void writeDensity(HANDLE hSerialComm, float density);
		static void writeZFactor(HANDLE hSerialComm, float zfactor);
		static void writeZeroThicknessTimer(HANDLE hSerialComm);

};

#endif