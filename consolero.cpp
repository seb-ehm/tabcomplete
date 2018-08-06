#include "consolero.h"

void ErrorExit(LPTSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message and exit the process

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	ExitProcess(dw);
}



Consolero::Consolero()
{
	hStdin = GetStdHandle(STD_INPUT_HANDLE);
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

	//disable mouse input capture
	DWORD dwConsoleMode;
	GetConsoleMode(hStdin, &m_OldConsoleMode);
	SetConsoleMode(hStdin, (~ENABLE_MOUSE_INPUT) & m_OldConsoleMode);
	DWORD dwConsoleModeMod;
	GetConsoleMode(hStdin, &dwConsoleModeMod);
}

Consolero::~Consolero()
{
	//reset Console mode
	SetConsoleMode(hStdin, m_OldConsoleMode);
}

void Consolero::Cin()
{
	const int bufferSize = 128;
	INPUT_RECORD irInBuf[bufferSize];
	unsigned long cNumRead;
	CONSOLE_SCREEN_BUFFER_INFO lConsoleScreenBufferInfo;
	while (1) {
		ReadConsoleInput(hStdin, irInBuf, bufferSize, &cNumRead);

		for (unsigned int i = 0; i< cNumRead; i++)
		{
			switch (irInBuf[i].EventType)
			{
			case KEY_EVENT:
				KEY_EVENT_RECORD & event = irInBuf[i].Event.KeyEvent;
				for (unsigned int j = 0; j < event.wRepeatCount; j++) {
					if (event.bKeyDown) {
						if (event.uChar.UnicodeChar == '\t') {
							printf("TAB");
						}
						else {

							
							if (event.wVirtualKeyCode == 8) { //Backspace
								Backspace();
							}

							else if (iswprint(event.uChar.UnicodeChar)) {
								printf("%c", event.uChar.UnicodeChar);
							}
							else {
								printf("|?|");
							}
							//printf("*%u*", irInBuf[i].Event.KeyEvent.wVirtualKeyCode);
							//printf("x%ux", irInBuf[i].Event.KeyEvent.uChar.AsciiChar);
							//printf("%u", lConsoleScreenBufferInfo.dwCursorPosition.X);
						}
					}
				}
			};

		}

	}
}

void Consolero::Backspace()
{
	CONSOLE_SCREEN_BUFFER_INFO lConsoleScreenBufferInfo;
	GetConsoleScreenBufferInfo(hStdout, &lConsoleScreenBufferInfo);
	COORD currentCoord = lConsoleScreenBufferInfo.dwCursorPosition;
	CHAR_INFO space;
	space.Char.AsciiChar = ' ';
	space.Attributes = 0;// FOREGROUND_BLUE | FOREGROUND_GREEN |
						 //FOREGROUND_INTENSITY;
	short newX = currentCoord.X - 1;
	short newY = currentCoord.Y;
	COORD newCoord = { newX, newY };
	SMALL_RECT consoleWriteArea = { newX, newY, newX, newY };
	if (!WriteConsoleOutput(hStdout, &space, { 1,1 }, { 0,0 }, &consoleWriteArea)) {
		ErrorExit(TEXT("WCO"));
	};
	SetConsoleCursorPosition(hStdout, newCoord);

}
