PS * PieceSquare(unsigned short noCase, HBITMAP bmp,  SYSTEMTIME dt);
bool GetCoordBoard(char *pathFile, unsigned &Board_xLeft, unsigned &Board_yTop, unsigned &Board_Width, unsigned &Board_Height);
bool Test_InitRunning(char *pathFile);
bool InitRunning(char *pathFile);
bool SetEmptyCase(unsigned nocase);
bool SetFirstHit(unsigned nocase);
void Stop();
void ConfigPositionBoard(unsigned xLeftTop, unsigned yLeftTop, unsigned width, unsigned height);
void Config_PositionCase(unsigned noCase);
void InitVariables();
void PhotoBoard(unsigned noCase);
void PhotoBoard();
void *RunningSquare_DoWork(void *void_ptr);
void Log(char *message, char * mode ="a");
void ResetValues();
char * GetFirstHit(unsigned nocase);
char * DateTime(unsigned nocase);
HBITMAP TakePictureCaseColor(unsigned noCase);


