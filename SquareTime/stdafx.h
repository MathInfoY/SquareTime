// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <time.h>
#include <io.h>
#include <pthread.h>
#include <atlbase.h>
#include <atlconv.h>
#include <atlstr.h> 
#include <iostream>
#include "SquareTime.h"
#include "PieceSquare.h"

// TODO: reference additional headers your program requires here
const char g_LogFile[] = "C:\\Chess\\SquareTime.Log";
const char g_DateTime[] = "01-01-0001 12:00:00.000";
char g_pathBoardFile[60]; 




