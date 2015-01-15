#include "stdafx.h"
#include "resource.h"

#define MAX_LOADSTRING 100

#define IDC_STATIC1 200
#define IDC_STATIC2 201
#define IDC_STATIC3 202
#define IDC_STATIC4 203
#define IDC_STATIC5 204
#define IDC_STATIC6 205
#define IDC_STATIC7 206
#define IDC_STATIC8 207

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// The main window class name

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	FilmProp(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	CommSet(HWND, UINT, WPARAM, LPARAM);

// Some more global variables
TCHAR m_pszPortName[5];
HANDLE m_hSerialComm;
HANDLE hThread;
DWORD dwThreadId;
DWORD WINAPI pollCOM(LPVOID lpParam);
float rate, thickness, life, density, zfactor;
float newdensity, newzfactor;
int timemin, timesec, freq;
bool setdensityready = false;
bool setzfactorready = false;

//window handles to the main window and the static child windows for the process values
HWND hwnd;
HWND hStatic[8];

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_QCMREAD, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_QCMREAD);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

// MyRegisterClass: registers the main window's class
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_QCMREAD);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_QCMREAD;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

// InitInstance: Instaiates the main program's window
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   if(!DialogBox(hInst, (LPCTSTR)IDD_COMMSET, NULL, (DLGPROC)CommSet))
   {
	   return FALSE;
   }

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      100, 100, 225, 175, NULL, NULL, hInstance, NULL);

   hwnd = hWnd;

   if (!hWnd)
   {
      return FALSE;
   }


   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

// WndProc: the callback function handles all the events from the main window
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	char szBuf[100];

	switch (message) 
	{
		case WM_CREATE:

			m_hSerialComm = CreateFile(m_pszPortName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
			if (m_hSerialComm == INVALID_HANDLE_VALUE) 
			{
				MessageBox(hWnd,"Error Opening COM port","info",MB_OK);
			}else
			{
				hThread = CreateThread(NULL,0,pollCOM,m_hSerialComm,0,&dwThreadId);
			}

			// Creating all the static child windows that hold the values of the data that is polled
			// from the STM100 controller
			hStatic[0] = CreateWindow("Static","",WS_CHILD | WS_VISIBLE,0,0,125,112,hWnd,(HMENU)IDC_STATIC1,GetModuleHandle(NULL),NULL);
			hStatic[1] = CreateWindow("Static","",WS_CHILD | WS_VISIBLE,125,0,100,16,hWnd,(HMENU)IDC_STATIC2,GetModuleHandle(NULL),NULL);
			hStatic[2] = CreateWindow("Static","",WS_CHILD | WS_VISIBLE,125,16,100,16,hWnd,(HMENU)IDC_STATIC3,GetModuleHandle(NULL),NULL);
			hStatic[3] = CreateWindow("Static","",WS_CHILD | WS_VISIBLE,125,32,100,16,hWnd,(HMENU)IDC_STATIC4,GetModuleHandle(NULL),NULL);
			hStatic[4] = CreateWindow("Static","",WS_CHILD | WS_VISIBLE,125,48,100,16,hWnd,(HMENU)IDC_STATIC5,GetModuleHandle(NULL),NULL);
			hStatic[5] = CreateWindow("Static","",WS_CHILD | WS_VISIBLE,125,64,100,16,hWnd,(HMENU)IDC_STATIC6,GetModuleHandle(NULL),NULL);
			hStatic[6] = CreateWindow("Static","",WS_CHILD | WS_VISIBLE,125,80,100,16,hWnd,(HMENU)IDC_STATIC7,GetModuleHandle(NULL),NULL);
			hStatic[7] = CreateWindow("Static","",WS_CHILD | WS_VISIBLE,125,96,100,16,hWnd,(HMENU)IDC_STATIC8,GetModuleHandle(NULL),NULL);
			SetWindowText(hStatic[0],TEXT("Rate:\nThickness:\nDensity:\nZ-Factor:\nRemaining Life:\nFrequency:\nTime:"));

			SetTimer(hWnd,2,500,NULL);

			break;
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case ID_SETTINGS_FILMPROPERTIES:
					DialogBox(hInst, (LPCTSTR)IDD_FPROP, hWnd, (DLGPROC)FilmProp);
					break;
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_CTLCOLORSTATIC:
			SetBkMode((HDC)wParam,TRANSPARENT);
			return (INT_PTR)(HBRUSH)GetStockObject(WHITE_BRUSH);
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			RECT rt;
			GetClientRect(hWnd, &rt);


			EndPaint(hWnd, &ps);
			break;
		case WM_TIMER:

			sprintf(szBuf,"%05.1f A/s",rate);
			SetWindowText(hStatic[1],szBuf);

			sprintf(szBuf,"%05.3f KA",thickness/1000);
			SetWindowText(hStatic[2],szBuf);

			sprintf(szBuf,"%06.3f g/cc",density);
			SetWindowText(hStatic[3],szBuf);

			sprintf(szBuf,"%05.3f",zfactor);
			SetWindowText(hStatic[4],szBuf);

			sprintf(szBuf,"%04.1f%%",life);
			SetWindowText(hStatic[5],szBuf);

			sprintf(szBuf,"%d Hz",freq);
			SetWindowText(hStatic[6],szBuf);

			sprintf(szBuf,"%02d:%02d",timemin,timesec);
			SetWindowText(hStatic[7],szBuf);

			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}

//callback for the dialog box were the user sets the film properties (density and Z-factor)
LRESULT CALLBACK FilmProp(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	char szBuf[20];

	switch (message)
	{
		case WM_INITDIALOG:

				sprintf(szBuf,"%05.2f",density);
				SetDlgItemText(hDlg,IDC_EDIT1,szBuf);

				sprintf(szBuf,"%05.3f",zfactor);
				SetDlgItemText(hDlg,IDC_EDIT2,szBuf);

				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK){

				GetDlgItemText(hDlg,IDC_EDIT1,szBuf,6);
				sscanf(szBuf,"%f",&newdensity);
				if(newdensity >= 0.5 && newdensity <= 99.99){

					GetDlgItemText(hDlg,IDC_EDIT2,szBuf,6);
					sscanf(szBuf,"%f",&newzfactor);
					if(newzfactor >= 0.1 && newzfactor <= 9.999){
						setdensityready = true;
						setzfactorready = true;
						EndDialog(hDlg, LOWORD(wParam));
					}else{
						MessageBox(hDlg,"Invalid value for Z-Factor!","Error",MB_OK);
						return TRUE;
					}

				}else{
					MessageBox(hDlg,"Invalid value for density!","Error",MB_OK);
					return TRUE;
				}

				
			}
			if(LOWORD(wParam) == IDCANCEL) {
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}

//callback for the dialog box that prompts the user for the name of the com port
LRESULT CALLBACK CommSet(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
		case WM_INITDIALOG:

				SetDlgItemText(hDlg,IDC_EDIT1,TEXT("COM1"));

				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK){

				GetDlgItemText(hDlg,IDC_EDIT1,m_pszPortName,5);
				EndDialog(hDlg, 1);


			}
			if(LOWORD(wParam) == IDCANCEL) {
				EndDialog(hDlg, 0);
				return TRUE;
			}
			break;
	}
    return FALSE;
}

//the function that polls the values from the STM100 controller over the serial COM port 
DWORD WINAPI pollCOM(LPVOID lpParam)
{
	HANDLE m_hSerialComm = (HANDLE)lpParam;
	int i;
	unsigned char checksum;
	unsigned long dwNumberOfBytesSent;
	unsigned long dwSize;
	char pszBuf[50];
	char szBuf[50];
	unsigned long dwNumberOfBytesWritten;
			
			//First we must setup the communications
			DCB dcbConfig;

			if(GetCommState(m_hSerialComm, &dcbConfig))
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
			    MessageBox(hwnd,"Error getting COMM state","info",MB_OK);
			}

			if(!SetCommState(m_hSerialComm, &dcbConfig)) MessageBox(hwnd,"Error setting COMM state","info",MB_OK);

			COMMTIMEOUTS commTimeout;

			if(GetCommTimeouts(m_hSerialComm, &commTimeout))
			{
			    commTimeout.ReadIntervalTimeout     = 50;
			    commTimeout.ReadTotalTimeoutConstant     = 50;
			    commTimeout.ReadTotalTimeoutMultiplier     = 50;
			    commTimeout.WriteTotalTimeoutConstant     = 1000 * 5;
			    commTimeout.WriteTotalTimeoutMultiplier = 1000 * 5;
			}

			else
			{
			    MessageBox(hwnd,"Error getting COMM timeouts","info",MB_OK);
			}

			if(!SetCommTimeouts(m_hSerialComm, &commTimeout)) MessageBox(hwnd,"Error setting COMM timeouts","info",MB_OK);


			DWORD dwEventMask;
			DWORD dwIncommingReadSize;
			while(1){

				if(setdensityready){
					dwNumberOfBytesSent = 0;

					dwSize = 10;
					sprintf(szBuf,"E=%05.2f",newdensity);
					
					checksum = 0;
					for(i=0;i<7;i++){
						checksum += szBuf[i];
					}
					sprintf(pszBuf,"%c%c%s%c",char(2),char(7),szBuf,checksum);

					//Setting the density value set by user
					if(WriteFile(m_hSerialComm, pszBuf, dwSize, 
					               &dwNumberOfBytesWritten, NULL) != 0)
					{
					    if(dwNumberOfBytesWritten != dwSize) MessageBox(hwnd,"Error Writing byte code 2","info",MB_OK);
					}

					else
					   MessageBox(hwnd,"Error Writing byte code 1","info",MB_OK);

					//Then we received the acknowledgement
					dwSize = 4;

					if(!SetCommMask(m_hSerialComm, EV_RXCHAR)) MessageBox(hwnd,"Error Setting COMM mask","info",MB_OK);

					if(WaitCommEvent(m_hSerialComm, &dwEventMask, NULL))
					{
			    

					        if(ReadFile(m_hSerialComm, szBuf, dwSize, &dwIncommingReadSize, NULL) != 0)
					       {

					                szBuf[dwIncommingReadSize+1] = char(0);
			      
					        }

					        else
					            MessageBox(hwnd,"Error Reading byte code 2","info",MB_OK);
					}

					else
					{
					    MessageBox(hwnd,"Error Reading COMM","info",MB_OK);
					}

					setdensityready = false;
				}

				if(setzfactorready){
					dwNumberOfBytesSent = 0;

					dwSize = 10;
					sprintf(szBuf,"F=%05.3f",newzfactor);
					
					checksum = 0;
					for(i=0;i<7;i++){
						checksum += szBuf[i];
					}
					sprintf(pszBuf,"%c%c%s%c",char(2),char(7),szBuf,checksum);

					//Setting the density value set by user
					if(WriteFile(m_hSerialComm, pszBuf, dwSize, 
					               &dwNumberOfBytesWritten, NULL) != 0)
					{
					    if(dwNumberOfBytesWritten != dwSize) MessageBox(hwnd,"Error Writing byte code 2","info",MB_OK);
					}

					else
					   MessageBox(hwnd,"Error Writing byte code 1","info",MB_OK);

					//Then we received the acknowledgement
					dwSize = 4;

					if(!SetCommMask(m_hSerialComm, EV_RXCHAR)) MessageBox(hwnd,"Error Setting COMM mask","info",MB_OK);

					if(WaitCommEvent(m_hSerialComm, &dwEventMask, NULL))
					{
			    

					        if(ReadFile(m_hSerialComm, szBuf, dwSize, &dwIncommingReadSize, NULL) != 0)
					       {

					                szBuf[dwIncommingReadSize+1] = char(0);
			      
					        }

					        else
					            MessageBox(hwnd,"Error Reading byte code 2","info",MB_OK);
					}

					else
					{
					    MessageBox(hwnd,"Error Reading COMM","info",MB_OK);
					}

					setzfactorready = false;
				}


			dwNumberOfBytesSent = 0;

			dwSize = 4;
			sprintf(pszBuf,"%c%cTT",char(2),char(1));
			//Getting the process variable which is the current rate
			//Now we send the first message to the controller
			if(WriteFile(m_hSerialComm, pszBuf, dwSize, 
                           &dwNumberOfBytesWritten, NULL) != 0)
			{
			    if(dwNumberOfBytesWritten != dwSize) MessageBox(hwnd,"Error Writing byte code 2","info",MB_OK);
			}

			else
			    MessageBox(hwnd,"Error Writing byte code 1","info",MB_OK);


			//Then we received the acknowledgement
			dwSize = 10;

			if(!SetCommMask(m_hSerialComm, EV_RXCHAR)) MessageBox(hwnd,"Error Setting COMM mask","info",MB_OK);

			if(WaitCommEvent(m_hSerialComm, &dwEventMask, NULL))
			{
			    

			        if(ReadFile(m_hSerialComm, szBuf, dwSize, &dwIncommingReadSize, NULL) != 0)
			       {

			                szBuf[dwIncommingReadSize+1] = char(0);
			      
			        }

			        else
			            MessageBox(hwnd,"Error Reading byte code 2","info",MB_OK);
			}

			else
			{
			    MessageBox(hwnd,"Error Reading COMM","info",MB_OK);
			}


			sscanf(szBuf,"%*c%*c%*c%f%*c",&rate);

			dwNumberOfBytesSent = 0;

			dwSize = 4;
			sprintf(pszBuf,"%c%cSS",char(2),char(1));
			//Getting the process variable which is the current thickness
			//Now we send the first message to the controller
			if(WriteFile(m_hSerialComm, pszBuf, dwSize, 
                           &dwNumberOfBytesWritten, NULL) != 0)
			{
			    if(dwNumberOfBytesWritten != dwSize) MessageBox(hwnd,"Error Writing byte code 2","info",MB_OK);
			}

			else
			    MessageBox(hwnd,"Error Writing byte code 1","info",MB_OK);


			//Then we received the acknowledgement
			dwSize = 12;

			if(!SetCommMask(m_hSerialComm, EV_RXCHAR)) MessageBox(hwnd,"Error Setting COMM mask","info",MB_OK);

			if(WaitCommEvent(m_hSerialComm, &dwEventMask, NULL))
			{
			    

			        if(ReadFile(m_hSerialComm, szBuf, dwSize, &dwIncommingReadSize, NULL) != 0)
			       {

			                szBuf[dwIncommingReadSize+1] = char(0);
			      
			        }

			        else
			            MessageBox(hwnd,"Error Reading byte code 2","info",MB_OK);
			}

			else
			{
			    MessageBox(hwnd,"Error Reading COMM","info",MB_OK);
			}


			sscanf(szBuf,"%*c%*c%*c%f%*c",&thickness);

			dwNumberOfBytesSent = 0;

			dwSize = 4;
			sprintf(pszBuf,"%c%cVV",char(2),char(1));
			//Getting the process variable which is the current thickness
			//Now we send the first message to the controller
			if(WriteFile(m_hSerialComm, pszBuf, dwSize, 
                           &dwNumberOfBytesWritten, NULL) != 0)
			{
			    if(dwNumberOfBytesWritten != dwSize) MessageBox(hwnd,"Error Writing byte code 2","info",MB_OK);
			}

			else
			    MessageBox(hwnd,"Error Writing byte code 1","info",MB_OK);


			//Then we received the acknowledgement
			dwSize = 10;

			if(!SetCommMask(m_hSerialComm, EV_RXCHAR)) MessageBox(hwnd,"Error Setting COMM mask","info",MB_OK);

			if(WaitCommEvent(m_hSerialComm, &dwEventMask, NULL))
			{
			    

			        if(ReadFile(m_hSerialComm, szBuf, dwSize, &dwIncommingReadSize, NULL) != 0)
			       {

			                szBuf[dwIncommingReadSize+1] = char(0);
			      
			        }

			        else
			            MessageBox(hwnd,"Error Reading byte code 2","info",MB_OK);
			}

			else
			{
			    MessageBox(hwnd,"Error Reading COMM","info",MB_OK);
			}


			sscanf(szBuf,"%*c%*c%*c%f%*c",&life);

			dwNumberOfBytesSent = 0;

			dwSize = 4;
			sprintf(pszBuf,"%c%cUU",char(2),char(1));
			//Getting the process variable which is the current thickness
			//Now we send the first message to the controller
			if(WriteFile(m_hSerialComm, pszBuf, dwSize, 
                           &dwNumberOfBytesWritten, NULL) != 0)
			{
			    if(dwNumberOfBytesWritten != dwSize) MessageBox(hwnd,"Error Writing byte code 2","info",MB_OK);
			}

			else
			    MessageBox(hwnd,"Error Writing byte code 1","info",MB_OK);


			//Then we received the acknowledgement
			dwSize = 11;

			if(!SetCommMask(m_hSerialComm, EV_RXCHAR)) MessageBox(hwnd,"Error Setting COMM mask","info",MB_OK);

			if(WaitCommEvent(m_hSerialComm, &dwEventMask, NULL))
			{
			    

			        if(ReadFile(m_hSerialComm, szBuf, dwSize, &dwIncommingReadSize, NULL) != 0)
			       {

			                szBuf[dwIncommingReadSize+1] = char(0);
			      
			        }

			        else
			            MessageBox(hwnd,"Error Reading byte code 2","info",MB_OK);
			}

			else
			{
			    MessageBox(hwnd,"Error Reading COMM","info",MB_OK);
			}


			sscanf(szBuf,"%*c%*c%*c%d%*c",&freq);

			dwNumberOfBytesSent = 0;

			dwSize = 4;
			sprintf(pszBuf,"%c%cWW",char(2),char(1));
			//Getting the process variable which is the current thickness
			//Now we send the first message to the controller
			if(WriteFile(m_hSerialComm, pszBuf, dwSize, 
                           &dwNumberOfBytesWritten, NULL) != 0)
			{
			    if(dwNumberOfBytesWritten != dwSize) MessageBox(hwnd,"Error Writing byte code 2","info",MB_OK);
			}

			else
			    MessageBox(hwnd,"Error Writing byte code 1","info",MB_OK);


			//Then we received the acknowledgement
			dwSize = 10;

			if(!SetCommMask(m_hSerialComm, EV_RXCHAR)) MessageBox(hwnd,"Error Setting COMM mask","info",MB_OK);

			if(WaitCommEvent(m_hSerialComm, &dwEventMask, NULL))
			{
			    

			        if(ReadFile(m_hSerialComm, szBuf, dwSize, &dwIncommingReadSize, NULL) != 0)
			       {

			                szBuf[dwIncommingReadSize+1] = char(0);
			      
			        }

			        else
			            MessageBox(hwnd,"Error Reading byte code 2","info",MB_OK);
			}

			else
			{
			    MessageBox(hwnd,"Error Reading COMM","info",MB_OK);
			}


			sscanf(szBuf,"%*c%*c%*c%*c%d%*c%d%*c",&timemin,&timesec);

			dwNumberOfBytesSent = 0;

			dwSize = 5;
			sprintf(pszBuf,"%c%cE?%c",char(2),char(2),char(69+63));
			//Getting the process variable which is the current thickness
			//Now we send the first message to the controller
			if(WriteFile(m_hSerialComm, pszBuf, dwSize, 
                           &dwNumberOfBytesWritten, NULL) != 0)
			{
			    if(dwNumberOfBytesWritten != dwSize) MessageBox(hwnd,"Error Writing byte code 2","info",MB_OK);
			}

			else
			    MessageBox(hwnd,"Error Writing byte code 1","info",MB_OK);


			//Then we received the acknowledgement
			dwSize = 10;

			if(!SetCommMask(m_hSerialComm, EV_RXCHAR)) MessageBox(hwnd,"Error Setting COMM mask","info",MB_OK);

			if(WaitCommEvent(m_hSerialComm, &dwEventMask, NULL))
			{
			    

			        if(ReadFile(m_hSerialComm, szBuf, dwSize, &dwIncommingReadSize, NULL) != 0)
			       {

			                szBuf[dwIncommingReadSize+1] = char(0);
			      
			        }

			        else
			            MessageBox(hwnd,"Error Reading byte code 2","info",MB_OK);
			}

			else
			{
			    MessageBox(hwnd,"Error Reading COMM","info",MB_OK);
			}

			sscanf(szBuf,"%*c%*c%*c%f%*c",&density);

			dwNumberOfBytesSent = 0;

			dwSize = 5;
			sprintf(pszBuf,"%c%cF?%c",char(2),char(2),char(70+63));
			//Getting the process variable which is the current thickness
			//Now we send the first message to the controller
			if(WriteFile(m_hSerialComm, pszBuf, dwSize, 
                           &dwNumberOfBytesWritten, NULL) != 0)
			{
			    if(dwNumberOfBytesWritten != dwSize) MessageBox(hwnd,"Error Writing byte code 2","info",MB_OK);
			}

			else
			    MessageBox(hwnd,"Error Writing byte code 1","info",MB_OK);


			//Then we received the acknowledgement
			dwSize = 9;

			if(!SetCommMask(m_hSerialComm, EV_RXCHAR)) MessageBox(hwnd,"Error Setting COMM mask","info",MB_OK);

			if(WaitCommEvent(m_hSerialComm, &dwEventMask, NULL))
			{
			    

			        if(ReadFile(m_hSerialComm, szBuf, dwSize, &dwIncommingReadSize, NULL) != 0)
			       {

			                szBuf[dwIncommingReadSize+1] = char(0);
			      
			        }

			        else
			            MessageBox(hwnd,"Error Reading byte code 2","info",MB_OK);
			}

			else
			{
			    MessageBox(hwnd,"Error Reading COMM","info",MB_OK);
			}

			sscanf(szBuf,"%*c%*c%*c%f%*c",&zfactor);

			}

			return 0;
}