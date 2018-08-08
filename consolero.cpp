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

	//set initial coordinate
	CONSOLE_SCREEN_BUFFER_INFO lConsoleScreenBufferInfo;
	GetConsoleScreenBufferInfo(hStdout, &lConsoleScreenBufferInfo);
	m_InitialCoord = lConsoleScreenBufferInfo.dwCursorPosition;

	m_currentLine.cursorPosition = 0;
	m_currentLine.start = m_InitialCoord;
	CHAR_INFO space;
	space.Char.AsciiChar = ' ';
	space.Attributes = 0;
	m_currentLine.content.push_back(space);
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
		m_currentLine.moveCursor(-1);
	}
	else if (keyEvent.wVirtualKeyCode == 38) // Arrow Up
	{
		
	}
	else if (keyEvent.wVirtualKeyCode == 39) // Arrow Right
	{
		m_currentLine.moveCursor(+1);
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
		m_currentLine.content.at(m_currentLine.cursorPosition).Char.UnicodeChar = keyEvent.uChar.UnicodeChar;
		m_currentLine.content.at(m_currentLine.cursorPosition).Attributes = FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED;
		m_currentLine.cursorPosition += 1;
		if (m_currentLine.cursorPosition == m_currentLine.content.size()) {
			CHAR_INFO space;
			space.Char.AsciiChar = ' ';
			space.Attributes = 0;
			m_currentLine.content.push_back(space);
		}
		//TODO: Currently works as if "Insert" was activated on keyboard
		//printf("%c", keyEvent.uChar.UnicodeChar);
	}
	else {
		printf("|?|");
	}
	displayLine(m_currentLine);

;
}

bool Consolero::moveCursor(SHORT linearChange)
{
	CONSOLE_SCREEN_BUFFER_INFO lConsoleScreenBufferInfo;
	GetConsoleScreenBufferInfo(hStdout, &lConsoleScreenBufferInfo);
	COORD currentCoord = lConsoleScreenBufferInfo.dwCursorPosition;
	short delta = 1;
	if (linearChange < 0) {
		delta = -1;
	}
	while (linearChange != 0) {

	}

	return false;
}

void Consolero::displayLine(const ConsoleLine & line)
{
	CONSOLE_SCREEN_BUFFER_INFO lConsoleScreenBufferInfo;
	GetConsoleScreenBufferInfo(hStdout, &lConsoleScreenBufferInfo);

	COORD& bufferSize = lConsoleScreenBufferInfo.dwSize;
	COORD current = line.start;

	for (int i = 0; i < line.content.size(); ++i) {
		SMALL_RECT consoleWriteArea = { current.X, current.Y, current.X, current.Y };
		WriteConsoleOutput(hStdout, &line.content.at(i), { 1,1 }, { 0,0 }, &consoleWriteArea);
		if (i == line.cursorPosition) {
			SetConsoleCursorPosition(hStdout, current);
		}

		if (current.X == bufferSize.X - 1) {
			current.Y = current.Y + 1;
			current.X = 0;
		}
		else {
			current.X += 1;
		}

		// TODO: Scroll Buffer when Y-Max is reached!
	}
}

void Consolero::Backspace()
{
	auto& cursorPosition = m_currentLine.cursorPosition;
	if (cursorPosition > 0) {
		m_currentLine.content.erase(m_currentLine.content.begin() + cursorPosition - 1);
	}
	cursorPosition -= 1;
}
