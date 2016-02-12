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
#define IDC_STATIC9 208

#define IDC_BUTTON 300

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// The main window class name

TCHAR m_pszPortName[5] = {"COM1"}; //default value to fill edit box upon start

stm100 controller; //The stm100 deposition controller object

//handles to the main window and the static child windows
HWND hwnd;
HWND hStatic[9];
HWND hButton;

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	FilmProp(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	CommSet(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    Options(HWND, UINT, WPARAM, LPARAM);

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

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW^WS_THICKFRAME,
      100, 100, 225, 216, NULL, NULL, hInstance, NULL);

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

			controller.OpenPort(m_pszPortName);

			/* Creating all the static child windows for the 
			   data that is polled from the STM100 controller */
			hStatic[0] = CreateWindowEx(NULL,"Static","",WS_CHILD | WS_VISIBLE,0,0,125,128,hWnd,(HMENU)IDC_STATIC1,GetModuleHandle(NULL),NULL);
			hStatic[1] = CreateWindowEx(NULL,"Static","",WS_CHILD | WS_VISIBLE,125,0,100,16,hWnd,(HMENU)IDC_STATIC2,GetModuleHandle(NULL),NULL);
			hStatic[2] = CreateWindowEx(NULL,"Static","",WS_CHILD | WS_VISIBLE,125,16,100,16,hWnd,(HMENU)IDC_STATIC3,GetModuleHandle(NULL),NULL);
			hStatic[3] = CreateWindowEx(NULL,"Static","",WS_CHILD | WS_VISIBLE,125,32,100,16,hWnd,(HMENU)IDC_STATIC4,GetModuleHandle(NULL),NULL);
			hStatic[4] = CreateWindowEx(NULL,"Static","",WS_CHILD | WS_VISIBLE,125,48,100,16,hWnd,(HMENU)IDC_STATIC5,GetModuleHandle(NULL),NULL);
			hStatic[5] = CreateWindowEx(NULL,"Static","",WS_CHILD | WS_VISIBLE,125,64,100,16,hWnd,(HMENU)IDC_STATIC6,GetModuleHandle(NULL),NULL);
			hStatic[6] = CreateWindowEx(NULL,"Static","",WS_CHILD | WS_VISIBLE,125,80,100,16,hWnd,(HMENU)IDC_STATIC7,GetModuleHandle(NULL),NULL);
			hStatic[7] = CreateWindowEx(NULL,"Static","",WS_CHILD | WS_VISIBLE,125,96,100,16,hWnd,(HMENU)IDC_STATIC8,GetModuleHandle(NULL),NULL);
			hStatic[8] = CreateWindowEx(NULL,"Static","",WS_CHILD | WS_VISIBLE,125,112,100,16,hWnd,(HMENU)IDC_STATIC9,GetModuleHandle(NULL),NULL);
			SetWindowText(hStatic[0],TEXT("Rate:\nAverage Rate:\nThickness:\nDensity:\nZ-Factor:\nRemaining Life:\nFrequency:\nTime:"));

			hButton = CreateWindowEx(NULL,"Button","Zero Thickness",WS_CHILD | WS_VISIBLE,5,130,210,30,hWnd,(HMENU)IDC_BUTTON,GetModuleHandle(NULL),NULL);

			//update timer for the displayed values
			SetTimer(hWnd,2,500,NULL);

			break;
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDC_BUTTON:
					controller.ZeroThicknessTimer();
					break;
				case ID_SETTINGS_FILMPROPERTIES:
					DialogBox(hInst, (LPCTSTR)IDD_FPROP, hWnd, (DLGPROC)FilmProp);
					break;
				case ID_SETTINGS_OPTIONS:
					//OPTIONS CURRENTLY NOT SUPPORTED
					//DialogBox(hInst, (LPCTSTR)IDD_OPTIONS, hWnd, (DLGPROC)Options);
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
			//setting the color of the static windows
			SetBkMode((HDC)wParam,TRANSPARENT);
			return (INT_PTR)(HBRUSH)GetStockObject(WHITE_BRUSH);
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			RECT rt;
			GetClientRect(hWnd, &rt);

			//nothing done here

			EndPaint(hWnd, &ps);
			break;
		case WM_TIMER:

			//update the displayed values

			sprintf(szBuf,"%05.1f A/s",controller.GetRate());
			SetWindowText(hStatic[1],szBuf);

			tm current_time = controller.GetTime();
			sprintf(szBuf,"%06f A/s",controller.GetThickness()/(current_time.tm_min*60.0f+current_time.tm_sec));
			SetWindowText(hStatic[2],szBuf);

			sprintf(szBuf,"%05.3f KA",controller.GetThickness()/1000);
			SetWindowText(hStatic[3],szBuf);

			sprintf(szBuf,"%06.3f g/cc",controller.GetDensity());
			SetWindowText(hStatic[4],szBuf);

			sprintf(szBuf,"%05.3f",controller.GetZFactor());
			SetWindowText(hStatic[5],szBuf);

			sprintf(szBuf,"%04.1f%%",controller.GetLife());
			SetWindowText(hStatic[6],szBuf);

			sprintf(szBuf,"%d Hz",controller.GetFreq());
			SetWindowText(hStatic[7],szBuf);

			sprintf(szBuf,"%02d:%02d",current_time.tm_min,current_time.tm_sec);
			SetWindowText(hStatic[8],szBuf);

			break;
		case WM_DESTROY:
			//close communications before closing program
			controller.ClosePort();
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
	float newdensity, newzfactor;

	switch (message)
	{
		case WM_INITDIALOG:

				sprintf(szBuf,"%05.2f",controller.GetDensity());
				SetDlgItemText(hDlg,IDC_EDIT1,szBuf);

				sprintf(szBuf,"%05.3f",controller.GetZFactor());
				SetDlgItemText(hDlg,IDC_EDIT2,szBuf);

				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK){

				//get density value from edit box
				GetDlgItemText(hDlg,IDC_EDIT1,szBuf,6);
				sscanf(szBuf,"%f",&newdensity);

				//make sure density is reasonable value
				if(newdensity >= 0.5 && newdensity <= 99.99){

					//send the controller the new density value
					controller.SetDensity(newdensity);

				}else{

					MessageBox(hDlg,"Invalid value for density!","Error",MB_OK);
					return TRUE;

				}

				//get the Z-factor value from the edit box
				GetDlgItemText(hDlg,IDC_EDIT2,szBuf,6);
				sscanf(szBuf,"%f",&newzfactor);

				//make sure Z-factor is a reasonable value
				if(newzfactor >= 0.1 && newzfactor <= 9.999){

					//send the controller the new Z-factor value
					controller.SetZFactor(newzfactor);

				}else{

					MessageBox(hDlg,"Invalid value for Z-Factor!","Error",MB_OK);
					return TRUE;

				}

				EndDialog(hDlg, LOWORD(wParam));
				
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

			//set the edit box with the default com port
			SetDlgItemText(hDlg,IDC_EDIT1,m_pszPortName);

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

//callback for the dialog box that sets the options
//CURRENTLY NOT SUPPRTED
LRESULT CALLBACK Options(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
		case WM_INITDIALOG:

				SetDlgItemText(hDlg,IDC_NUM_SAMPLES,TEXT("5"));

				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK){
				char numberText[16];

				GetDlgItemText(hDlg,IDC_NUM_SAMPLES,numberText,16);
				//avg_samples = atoi(numberText);

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
