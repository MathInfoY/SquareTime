// SquareTime.cpp : Defines the entry point for the application.
//

/*
#using <mscorlib.dll>
#using <System.dll>

using namespace System;
using namespace System::Diagnostics;
using namespace System::ComponentModel;
*/

#include <tchar.h>

#include "stdafx.h"
#include <iostream>
#include "psapi.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HWND hwndApp;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK EnumWindowsProc(HWND hWnd, long lParam);

HBITMAP Test(int x, int y, int cx,int cy);

void PrintProcessNameAndID(DWORD processID);
void SendMessage_Interface(CString strDataToSend);
void Test_2();


const int IC_INITSTART = 0;
const int IC_STARTRUNNING = 1;
const int IC_GETFIRSTHIT = 2;
const int IC_STOPRUNNING = 3;
const int IC_SETEMPTYCASE = 4;
const int IC_NEWGAME = 5;

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_SQUARETIME, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SQUARETIME));

	// Main message loop:
	
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	
	return (int) msg.wParam;

}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SQUARETIME));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_SQUARETIME);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   hwndApp = hWnd;

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	FILE *Output = NULL;
	int wmId, wmEvent;
	COPYDATASTRUCT * cpd = NULL;
	char *buffer;
	char *TimeStampCase;
	unsigned noCase = 0;
	bool stopRunning = false;

	switch (message)
	{
	case WM_COPYDATA :
		cpd = (COPYDATASTRUCT*)lParam;
		buffer = (char *)(cpd->lpData);

		switch (cpd->dwData)
		{
			case IC_INITSTART :
				Log("*** New Game ***","w");
				break;

				// Start Looping
			case IC_STARTRUNNING :
				InitRunning(buffer);	// buffer = path Board file
				break;

				// Get Timestamp for case number
			case IC_GETFIRSTHIT :
				noCase = atoi(buffer);
				TimeStampCase = GetFirstHit(noCase);
				SendMessage_Interface(CString(TimeStampCase));
				break;

				// Stop Looping
			case IC_STOPRUNNING :
				Stop();
				Log("Stopped");
				break;

				// Clear Timestamp for case number
			case IC_SETEMPTYCASE :
				noCase = atoi(buffer);
				SetEmptyCase(noCase);
				break;

			case IC_NEWGAME :
				ResetValues();
				Log("*** New Game ***");
				break;
		}

		break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
//		Test();
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
//	HBITMAP hBm_A, hBm_B;
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
//		main_2();

//		hBm_A = Test(0,1,40,40);
//		hBm_B = Test(0,1,40,40);
/*
		bRes = CompareBitmaps(hBm_A, hBm_B);
		if (bRes) MessageBox(hwndApp, L"Bitmap equal", L"Success", MB_OK);
		else MessageBox(hwndApp, L"Bitmap different", L"Success", MB_OK);
		DeleteObject(hBm_A);
		DeleteObject(hBm_B);
*/
//		SendMessage();
//		Test_2();

	    //C:\Chess
		Test_2();
//		InitRunning("C:\\Chess\\CoordonneesBoard.txt");
//		Sleep(100);
//		Stop();

		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

HBITMAP Test(int x, int y,int cx, int cy)
{
	static int k;

// https://msdn.microsoft.com/en-us/library/windows/desktop/dd183402(v=vs.85).aspx

//	Beep(750, 300);
	HBITMAP hbmScreen;
	BITMAP bmpScreen;
	HDC hdcScreen, hdcMem;
	
// DC de l'ecran primary monitor
	hdcScreen = GetDC(NULL);

// Create a compatible DC which is used in a BitBlt from the window DC
	hdcMem = CreateCompatibleDC(hdcScreen);

// Create a compatible bitmap from the Window DC
	hbmScreen = CreateCompatibleBitmap(hdcScreen, cx, cy);

// Select the compatible bitmap into the compatible memory DC.
	SelectObject(hdcMem, hbmScreen);

// Bit block transfer into our compatible memory DC.
	if (!BitBlt(hdcMem,
		0, 0,
		cx, cy,
		hdcScreen,
		x, y,
		SRCCOPY))
	{
		MessageBox(hwndApp, L"BitBlt has failed", L"Failed", MB_OK);
		goto done;
	}

// Get the BITMAP from the HBITMAP
	GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);

	BITMAPFILEHEADER   bmfHeader;
	BITMAPINFOHEADER   bi;

	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bmpScreen.bmWidth;
	bi.biHeight = bmpScreen.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	DWORD dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;

	// Starting with 32-bit Windows, GlobalAlloc and LocalAlloc are implemented as wrapper functions that 
	// call HeapAlloc using a handle to the process's default heap. Therefore, GlobalAlloc and LocalAlloc 
	// have greater overhead than HeapAlloc.
	HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
	char *lpbitmap = (char *)GlobalLock(hDIB);

	// Gets the "bits" from the bitmap and copies them into a buffer 
	// which is pointed to by lpbitmap.
	GetDIBits(hdcScreen, hbmScreen, 0,
		(UINT)bmpScreen.bmHeight,
		lpbitmap,
		(BITMAPINFO *)&bi, DIB_RGB_COLORS);


	// A file is created, this is where we will save the screen capture.

	HANDLE hFile = CreateFile(L"captureqwsx.bmp",
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);

	// Add the size of the headers to the size of the bitmap to get the total file size
	DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	//Offset to where the actual bitmap bits start.
	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

	//Size of the file
	bmfHeader.bfSize = dwSizeofDIB;

	//bfType must always be BM for Bitmaps
	bmfHeader.bfType = 0x4D42; //BM   

	DWORD dwBytesWritten = 0;
	WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);

	//Unlock and Free the DIB from the heap
	GlobalUnlock(hDIB);
	GlobalFree(hDIB);

	//Close the handle for the file that was created

	//Clean up
done:
//	DeleteObject(hbmScreen);
	DeleteObject(hdcMem);
	ReleaseDC(NULL, hdcScreen);

	return(hbmScreen);
}

bool CompareBitmaps(HBITMAP HBitmapLeft, HBITMAP HBitmapRight)
{
	if (HBitmapLeft == HBitmapRight)
	{
		return true;
	}

	if (NULL == HBitmapLeft || NULL == HBitmapRight)
	{
		return false;
	}

	bool bSame = false;

	HDC hdc = GetDC(NULL);
	BITMAPINFO BitmapInfoLeft = { 0 };
	BITMAPINFO BitmapInfoRight = { 0 };

	BitmapInfoLeft.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	BitmapInfoRight.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

	if (0 != GetDIBits(hdc, HBitmapLeft, 0, 0, NULL, &BitmapInfoLeft, DIB_RGB_COLORS) &&
		0 != GetDIBits(hdc, HBitmapRight, 0, 0, NULL, &BitmapInfoRight, DIB_RGB_COLORS))
	{
		// Compare the BITMAPINFOHEADERs of the two bitmaps

		if (0 == memcmp(&BitmapInfoLeft.bmiHeader, &BitmapInfoRight.bmiHeader,
			sizeof(BITMAPINFOHEADER)))
		{
			// The BITMAPINFOHEADERs are the same so now compare the actual bitmap bits
	 		BYTE *pLeftBits  = new BYTE[BitmapInfoLeft.bmiHeader.biSizeImage];
			BYTE *pRightBits = new BYTE[BitmapInfoRight.bmiHeader.biSizeImage];
			BYTE *pByteLeft = NULL;
			BYTE *pByteRight = NULL;

			PBITMAPINFO pBitmapInfoLeft = &BitmapInfoLeft;
			PBITMAPINFO pBitmapInfoRight = &BitmapInfoRight;

			// calculate the size in BYTEs of the additional

			// memory needed for the bmiColor table

			int AdditionalMemory = 0;
			switch (BitmapInfoLeft.bmiHeader.biBitCount)
			{
			case 1:
				AdditionalMemory = 1 * sizeof(RGBQUAD);
				break;
			case 4:
				AdditionalMemory = 15 * sizeof(RGBQUAD);
				break;
			case 8:
				AdditionalMemory = 255 * sizeof(RGBQUAD);
				break;
			case 16:
			case 32:
				AdditionalMemory = 2 * sizeof(RGBQUAD);
			}

			if (AdditionalMemory)
			{
				// we have to allocate room for the bmiColor table that will be

				// attached to our BITMAPINFO variables

				pByteLeft = new BYTE[sizeof(BITMAPINFO) + AdditionalMemory];
				if (pByteLeft)
				{
					memset(pByteLeft, 0, sizeof(BITMAPINFO) + AdditionalMemory);
					memcpy(pByteLeft, pBitmapInfoLeft, sizeof(BITMAPINFO));
					pBitmapInfoLeft = (PBITMAPINFO)pByteLeft;
				}

				pByteRight = new BYTE[sizeof(BITMAPINFO) + AdditionalMemory];
				if (pByteRight)
				{
					memset(pByteRight, 0, sizeof(BITMAPINFO) + AdditionalMemory);
					memcpy(pByteRight, pBitmapInfoRight, sizeof(BITMAPINFO));
					pBitmapInfoRight = (PBITMAPINFO)pByteRight;
				}
			}

			if (pLeftBits && pRightBits && pBitmapInfoLeft && pBitmapInfoRight)
			{
				// zero out the bitmap bit buffers

				memset(pLeftBits, 0, BitmapInfoLeft.bmiHeader.biSizeImage);
				memset(pRightBits, 0, BitmapInfoRight.bmiHeader.biSizeImage);

				// fill the bit buffers with the actual bitmap bits

				if (0 != GetDIBits(hdc, HBitmapLeft, 0,
					pBitmapInfoLeft->bmiHeader.biHeight, pLeftBits, pBitmapInfoLeft,
					DIB_RGB_COLORS) && 0 != GetDIBits(hdc, HBitmapRight, 0,
					pBitmapInfoRight->bmiHeader.biHeight, pRightBits, pBitmapInfoRight,
					DIB_RGB_COLORS))
				{
					// compare the actual bitmap bits of the two bitmaps

					bSame = 0 == memcmp(pLeftBits, pRightBits,
						pBitmapInfoLeft->bmiHeader.biSizeImage);
				}
			}

			// clean up

			delete[] pLeftBits;
			delete[] pRightBits;
			delete[] pByteLeft;
			delete[] pByteRight;
		}
	}

	ReleaseDC(NULL, hdc);

	return bSame;
}

void Test_2()
{
	// Get the current process.

	EnumWindows(EnumWindowsProc, 0);

}

BOOL CALLBACK EnumWindowsProc(HWND hWnd, long lParam) 
{
	HWND FindW = NULL;
	HWND FindWindowInterface = NULL;
	bool find = true;
	char buff[255] = "Alpha";
	wchar_t* ext = L"Communicating via Messages Demo";
	wchar_t* interfaces = L"Interface";
//	wchar_t* interfaces = L"SquareTime";
	char *answer = NULL;
	wchar_t wtext[20];
	mbstowcs(wtext, buff, (unsigned)(strlen(buff) + 1));//Plus null
	LPWSTR ptr = wtext;
	wchar_t* wcTest = L"test";

	CString  strWindowTitle = _T("Interface");
	CString strDataToSend = _T("This is a message to send");
	

	COPYDATASTRUCT cpd;

	cpd.dwData = -1;
	cpd.cbData = strDataToSend.GetLength() * 2;
	cpd.lpData = (void*)strDataToSend.GetBuffer(cpd.cbData);
	FindWindowInterface = FindWindow(NULL, interfaces);
	SendMessageA(FindWindowInterface, WM_COPYDATA, (WPARAM)hWnd, (LPARAM)&cpd);
	strDataToSend.ReleaseBuffer();
	
	return TRUE;
}


void SendMessage_Interface(CString strDataToSend)
{
	HWND FindWindowInterface = NULL;
	COPYDATASTRUCT cpd;
	wchar_t* interfaces = L"Interface";

	cpd.dwData = IC_GETFIRSTHIT;
	cpd.cbData = strDataToSend.GetLength() * 2;
	cpd.lpData = (void*)strDataToSend.GetBuffer(cpd.cbData);
	FindWindowInterface = FindWindow(NULL, interfaces);
	SendMessageA(FindWindowInterface, WM_COPYDATA, (WPARAM)hwndApp, (LPARAM)&cpd);
	strDataToSend.ReleaseBuffer();
}



