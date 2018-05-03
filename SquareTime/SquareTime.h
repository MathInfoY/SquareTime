#pragma once

#include "resource.h"

void ConfigPositionBoard(int xLeftTop, int yLeftTop, int width, int height);
bool CompareBitmaps(HBITMAP HBitmapLeft, HBITMAP HBitmapRight);

struct PieceSquare
{
	unsigned short noCase;  // no de case
	HBITMAP bmp;			// photo de la case
	SYSTEMTIME dt;			// TimeStamp du 1er changement de la case
	bool isEmpty;
};

typedef struct PieceSquare PS;

enum todo { start, stop, hold};

struct Parameters
{
	todo act;
	bool isRunning;
};

// PS * g_NoCasePS[65];

/*
time_t t = time(NULL);
struct tm tm = *localtime(&t);

printf("now: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);m_xLeftTop
*/