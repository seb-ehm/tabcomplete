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

		for (unsigned int i = 0; i < cNumRead; i++)
		{
			switch (irInBuf[i].EventType)
			{
			case KEY_EVENT:
				KEY_EVENT_RECORD & event = irInBuf[i].Event.KeyEvent;
				for (unsigned int j = 0; j < event.wRepeatCount; j++) {
					//printf("*%u*", irInBuf[i].Event.KeyEvent.wVirtualKeyCode);
					if (event.bKeyDown) {
						handleKeyEvent(event);
						//printf("*%u*", irInBuf[i].Event.KeyEvent.wVirtualKeyCode);
						//printf("x%ux", irInBuf[i].Event.KeyEvent.uChar.AsciiChar);
						//printf("%u", lConsoleScreenBufferInfo.dwCursorPosition.X)
					}
				}
			}
		};

	}

}


void Consolero::handleKeyEvent(const KEY_EVENT_RECORD& keyEvent)
{
	if (keyEvent.wVirtualKeyCode == 9) //Tab
	{
		printf("TAB");
	}
	else if (keyEvent.wVirtualKeyCode == 8) // Backspace
	{
		Backspace();
	}
	else if (keyEvent.wVirtualKeyCode == 27) // ESC
	{
	}
	else if (keyEvent.wVirtualKeyCode == 46) // DEL
	{
	}
	else if (keyEvent.wVirtualKeyCode == 27) // ESC
	{
	}
	else if (keyEvent.wVirtualKeyCode == 37) // Arrow Left
	{
	}
	else if (keyEvent.wVirtualKeyCode == 38) // Arrow Up
	{
	}
	else if (keyEvent.wVirtualKeyCode == 39) // Arrow Right
	{
	}
	else if (keyEvent.wVirtualKeyCode == 40) // Arrow Down
	{
	}
	else if (keyEvent.wVirtualKeyCode == 120) // Pos1
	{
	}
	else if (keyEvent.wVirtualKeyCode == 121) // END
	{
	}
	else if (keyEvent.wVirtualKeyCode == 112) // F1
	{
	}
	else if (iswprint(keyEvent.uChar.UnicodeChar)) {
		printf("%c", keyEvent.uChar.UnicodeChar);
	}
	else {
		printf("|?|");
	}

;
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
