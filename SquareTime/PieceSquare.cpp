#include "stdafx.h"


PS * g_NoCasePS[65];
unsigned int g_noCase_x[65];
unsigned int g_noCase_y[65];
int g_width;
int g_height;
int g_offset_x;
int g_offset_y;
bool g_stopRunning;
bool g_endLooping;
int g_xLeftTop;
int g_yLeftTop;
struct Parameters *g_param = NULL;
pthread_t g_thread;
SYSTEMTIME g_DefaultTime;

struct Test_Parameters
{
	unsigned i;
	unsigned j;
};

void *Test(void *x_void_ptr);


bool InitRunning(char *pathFile)
{
	bool findFile = false;
	unsigned Board_xLeft, Board_yTop, Board_Width, Board_Height;

	// Arrete le thread si en execution 
	Stop();

	// Lecture des coordonnees a partir du fichier ChessCoordonnees.txt
	findFile = GetCoordBoard(pathFile, Board_xLeft, Board_yTop, Board_Width, Board_Height);
	ConfigPositionBoard(Board_xLeft, Board_yTop, Board_Width, Board_Height);

	// Initialisation
	InitVariables();

	if (findFile)
	{
		Log("Initialisation Succes");

		// Start le Thread
		pthread_create(&g_thread, NULL, RunningSquare_DoWork, g_param);
	}
	else
		Log("Initialisation Failed");


	return(findFile);

}

bool Test_InitRunning(char *pathFile)
{
	FILE *Output;
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	bool findFile = false;
	unsigned Board_xLeft, Board_yTop, Board_Width, Board_Height;
	int x = 0;
	int y = 0;
	struct Test_Parameters *param;
	pthread_t thread;

	param = (struct Test_Parameters *)malloc(sizeof(struct Test_Parameters));

	param->i = 0;
	param->j = 100000;

	Stop();

	findFile = GetCoordBoard(pathFile, Board_xLeft, Board_yTop, Board_Width, Board_Height);

	ConfigPositionBoard(Board_xLeft, Board_yTop, Board_Width, Board_Height);

	InitVariables();

	Output = fopen(g_LogFile, "a");

	if (findFile)
	{
		fprintf(Output, "%d-%d-%d %d:%d:%d\tInitialisation Succes !!\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
		pthread_create(&thread, NULL, Test, param);

		Sleep(12);

		y = param->j;

		if (pthread_join(thread, NULL))
		{
			fprintf(stderr, "Error joining thread\n");
			return false;
		}
	}
	else
		fprintf(Output, "%d-%d-%d %d:%d:%d\tInitialisation Failed !!\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);



	return(findFile);

}

void *Test(void *x_void_ptr)
{
	int k = 0; 
	struct Test_Parameters *x_ptr = (struct Test_Parameters *)x_void_ptr;

	while (x_ptr->i < x_ptr->j)
		x_ptr->i++;

	printf("x increment finished\n");

	return(NULL);
}

// C:\Chess\CoordonneesBoard.txt
bool GetCoordBoard(char *pathFile, unsigned &Board_xLeft, unsigned &Board_yTop, unsigned &Board_Width, unsigned &Board_Height)
{
	FILE *File;
	FILE *Output;
	bool isData = false;
	const char s[2] = " ";
	unsigned noToken = 1;
	unsigned position;

	Board_xLeft = Board_yTop = Board_Width = Board_Height = 0;

	File = fopen(pathFile, "r");

	if (File)
	{
		while (fscanf(File, "%d", &position) != EOF)
			Board_xLeft == 0 ? Board_xLeft = position : Board_yTop == 0 ? Board_yTop = position : Board_Width == 0 ? Board_Width = position : Board_Height = position;
		fclose(File);
		isData = true;
	}
	else
	{
		Output = fopen(g_LogFile, "a");
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);

		fprintf(Output, "%d-%d-%d %d:%d:%d\t Board Not exist\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

	}

	return (isData);
}

void ConfigPositionBoard(unsigned xLeftTop, unsigned yLeftTop, unsigned width, unsigned height)
{
	g_xLeftTop = xLeftTop;            // Coin Bas Gauche Echiquier
	g_yLeftTop = yLeftTop;
	g_width = width;
	g_height = height;
	g_offset_x = width / 16;
	g_offset_y = height / 16;

	for (unsigned i = 1; i <= 64; i++)
		Config_PositionCase(i);
}

void Config_PositionCase(unsigned noCase)
{
	int xCase_A1 = g_xLeftTop + g_offset_x;
	int lenXCase = g_offset_x * 2;

	int yCase_A1 = g_yLeftTop + g_height - g_offset_y;
	int lenYCase = g_offset_y * 2;

	unsigned noRow = 0;

	if (noCase <= 8) noRow = 1;
	else if (noCase <= 16) noRow = 2;
	else if (noCase <= 24) noRow = 3;
	else if (noCase <= 32) noRow = 4;
	else if (noCase <= 40) noRow = 5;
	else if (noCase <= 48) noRow = 6;
	else if (noCase <= 56) noRow = 7;
	else if (noCase <= 64) noRow = 8;

	g_noCase_x[noCase] = xCase_A1 + (noCase - 1) % 8 * lenXCase;
	g_noCase_y[noCase] = yCase_A1 - (noRow - 1) * lenYCase;

}

void InitVariables()
{
	HBITMAP SquareColorBmp = NULL;
	SYSTEMTIME dt;
	unsigned noCase = 1;
	FILE *Output = NULL;

	GetLocalTime(&dt);

	g_DefaultTime = dt;


	for (; noCase <= 64; noCase++)
	{
		SquareColorBmp = TakePictureCaseColor(noCase);

		PS *ps = PieceSquare(noCase, SquareColorBmp, dt);
		g_NoCasePS[noCase] = ps;
	}

	for (noCase = 1; noCase <= 16; noCase++)
		g_NoCasePS[noCase]->isEmpty = false;	

	for (; noCase <= 48; noCase ++)
		g_NoCasePS[noCase]->isEmpty = true;

	while (noCase < 65)
		g_NoCasePS[noCase++]->isEmpty = false;

	
	// Reserver memoire
	if (g_param == NULL)
	{
		g_param = (struct Parameters *)malloc(sizeof(struct Parameters));
		g_param->act = start;
		g_param->isRunning = false;
	}

//	PhotoBoard(17);
//	PhotoBoard(19);
	PhotoBoard();
}

void PhotoBoard()
{
	unsigned i = 1;

	for (i = 1; i <= 64; i++)
	{
		PhotoBoard(i);
	}
}

void PhotoBoard(unsigned noCase)
{
	HDC hdcScreen;
	BITMAP bmpScreen;
	char Filename[20];

	sprintf(Filename, "NoCase_%d.bmp", noCase);

	// convert char * to w_char_t
	wchar_t wFilename[20];
	mbstowcs(wFilename, Filename, strlen(Filename) + 1);//Plus null
	LPWSTR ptr = wFilename;


	if (g_NoCasePS[noCase])
	{
		// DC de l'ecran primary monitor
		hdcScreen = GetDC(NULL);

		// Get the BITMAP from the HBITMAP
		GetObject(g_NoCasePS[noCase]->bmp, sizeof(BITMAP), &bmpScreen);

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
		GetDIBits(hdcScreen, g_NoCasePS[noCase]->bmp, 0,
			(UINT)bmpScreen.bmHeight,
			lpbitmap,
			(BITMAPINFO *)&bi, DIB_RGB_COLORS);


		// A file is created, this is where we will save the screen capture.

		HANDLE hFile = CreateFile(ptr,
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
//	done:
		ReleaseDC(NULL, hdcScreen);
	}
}

void * RunningSquare_DoWork(void *void_ptr)
{
	bool bRes = false;
	HBITMAP hBmp = NULL;
	unsigned i = 0;
	unsigned j = 0;
	SYSTEMTIME dt;
	char buffer[20];

	struct Parameters *ptr = (struct Parameters *)void_ptr;

	Log("Looping Started");

	while (ptr->act != stop)
	{	
		if (ptr->act == start)
		{
			ptr->isRunning = true;

			for (i = 1; i <= 64; i++)
			{
				if (!g_NoCasePS[i]->isEmpty)
					continue;

				hBmp = TakePictureCaseColor(i);

				if (hBmp == NULL)
				{
					i--;
					continue;
				}
				try
				{
					bRes = CompareBitmaps(hBmp, g_NoCasePS[i]->bmp);
				}
				catch (const std::exception&)
				{
					Log("Case Locked (Compare Bitmap)");
					continue;
				}

				if (!bRes)
				{
					try
					{
						DeleteObject(g_NoCasePS[i]->bmp);
						g_NoCasePS[i]->bmp = hBmp;

						// Si Aucune donnée on ajoute timestamp (Valeur par defaut : Heure de l'initialisation variable)
						if (g_NoCasePS[i]->dt.wMinute == g_DefaultTime.wMinute &&
							g_NoCasePS[i]->dt.wSecond == g_DefaultTime.wSecond)
						{
							GetLocalTime(&dt);
							g_NoCasePS[i]->dt = dt;
							sprintf(buffer, "Case = %d", i);
							Log(buffer);
						}
					}
					catch (const std::exception&)
					{
						Log("Case Locked (Delete)");
						continue;
					}
				}
			}
		}
		else if (ptr->act == hold)
		{
			ptr->isRunning = false; 
		}
	}

	Log("Looping Ended");

	return(NULL);
}

HBITMAP TakePictureCaseColor(unsigned noCase)
{
	static int k;
	
	int xPosScreen = g_noCase_x[noCase] - 1;
	int yPosScreen = g_noCase_y[noCase] - 1;

	// https://msdn.microsoft.com/en-us/library/windows/desktop/dd183402(v=vs.85).aspx

	//	Beep(750, 300);
	HBITMAP hbmScreen=NULL;
	BITMAP bmpScreen;
	HDC hdcScreen, hdcMem;

	// DC de l'ecran primary monitor
	hdcScreen = GetDC(NULL);

	// Create a compatible DC which is used in a BitBlt from the window DC
	hdcMem = CreateCompatibleDC(hdcScreen);

	// Create a compatible bitmap from the Window DC
	hbmScreen = CreateCompatibleBitmap(hdcScreen, 2, 2);

	// Select the compatible bitmap into the compatible memory DC.
	SelectObject(hdcMem, hbmScreen);

	// Bit block transfer into our compatible memory DC.
	if (!BitBlt(
				hdcMem,
				0, 0,
				2, 2,
				hdcScreen,
				xPosScreen, yPosScreen,
				SRCCOPY
				)
		)
	{
		goto done;
	}

	// Get the BITMAP from the HBITMAP
	GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);

	//Clean up
	done:
	//	DeleteObject(hbmScreen);
	DeleteObject(hdcMem);
	ReleaseDC(NULL, hdcScreen);
	

	return(hbmScreen);
}



PS * PieceSquare(unsigned short noCase, HBITMAP bmp , SYSTEMTIME dt)
{
	PS *ps = (struct PieceSquare *)malloc(sizeof(struct PieceSquare));

	ps->noCase = noCase;
	ps->bmp = bmp;
	ps->dt = dt;

	return (ps);
}



void Stop()
{
	unsigned noCase = 1;

	if (g_param)
	{
		if (g_param->act != stop)
		{
			g_param->act = stop;

			if (pthread_join(g_thread, NULL))
				Log("Stop Error Thread");
			else
				Log("Stop Thread");

			while (noCase < 65)
			{
				if (g_NoCasePS[noCase])
				{
					if(g_NoCasePS[noCase]->bmp)
						DeleteObject(g_NoCasePS[noCase]->bmp);
					delete(g_NoCasePS[noCase]);
					g_NoCasePS[noCase] = NULL;
				}
				noCase++;
			}

		}
	}
}

/********************************************************/
// 1) Stop Looping
// 2) Reset default values
// 3) Start Looping
/********************************************************/
void ResetValues()
{
	PS *ps = NULL;

	if (g_param)
	{
		if (g_param->isRunning)
			g_param->act = hold;

		while (g_param->isRunning)
			Sleep(500);

		for (unsigned i = 1; i <= 64; i++)
		{
			ps = g_NoCasePS[i];

			if (ps->bmp)
				DeleteObject(ps->bmp);

			ps->dt = g_DefaultTime;

			if (i > 16 && i < 49)
			{
				ps->bmp = TakePictureCaseColor(i);
				ps->isEmpty = true;
			}
			else
			{
				ps->bmp = NULL;
				ps->isEmpty = false;
			}
		}

		g_param->act = start;
	}
}

char * GetFirstHit(unsigned nocase)
{
	if (!g_NoCasePS[nocase]->isEmpty)
		return ((char *)g_DateTime);
	
	return (DateTime(nocase));
}

char * DateTime(unsigned nocase)
{
	unsigned sizechar = 30;

	char * buffer = (char *)malloc((sizechar + 1) * sizeof(char));

	sprintf(buffer, "%-2.2d-%-2.2d-%d %-2.2d:%-2.2d:%-2.2d.%-3.3d", g_NoCasePS[nocase]->dt.wMonth, g_NoCasePS[nocase]->dt.wDay, g_NoCasePS[nocase]->dt.wYear, g_NoCasePS[nocase]->dt.wHour, g_NoCasePS[nocase]->dt.wMinute, g_NoCasePS[nocase]->dt.wSecond, g_NoCasePS[nocase]->dt.wMilliseconds);
	return(buffer);
}

bool SetFirstHit(unsigned nocase)
{
	char buffer[50];

	if (g_NoCasePS[nocase] == NULL)
	{
		sprintf(buffer, "SetFirstHit\t\tAucune piece sur la Case %d", nocase);
		Log(buffer);
		return (false);
	}

	g_NoCasePS[nocase]->dt = g_DefaultTime;

	return(true);
}


bool SetEmptyCase(unsigned noCase)
{
	PS *ps = NULL;
	char buffer[2000];

	try
	{
		ps = g_NoCasePS[noCase];

		if (ps->isEmpty)
		{
			sprintf(buffer, "*** Error SetEmptyCase\t\tCase %d deja vide...***", noCase);
			Log(buffer);
		}
		else
		{
			ps->bmp = TakePictureCaseColor(noCase);
			ps->dt = g_DefaultTime;
			ps->isEmpty = true;

			sprintf(buffer, "Clear Case\t\t %d ", noCase);
			Log(buffer);
		}
	}

	catch (const std::exception& e)
	{
		sprintf(buffer, "SetEmptyCase\t\tCase %d + %s", noCase, (char *)e.what());
		g_NoCasePS[noCase] = NULL;
		Log(buffer);
		return(false);
	}
	
	return (true);
}

void Log(char *message,char * mode)
{
	FILE *Output = NULL;
	SYSTEMTIME lt;
	GetLocalTime(&lt);

	Output = fopen(g_LogFile, mode);

	if (Output)
	{
		// 03-02-2018 09:35:17.885 (C#)
		fprintf(Output, "%-2.2d-%-2.2d-%d %-2.2d:%-2.2d:%-2.2d.%-3.3d\t%s\n", lt.wMonth, lt.wDay,lt.wYear, lt.wHour, lt.wMinute, lt.wSecond, lt.wMilliseconds, message);
		fclose(Output);
	}
}
