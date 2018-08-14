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
	m_currentLine.addSpace();

    //Set keyboard codepage according to system settings
    auto systemCodepage = GetACP();
    SetConsoleOutputCP(systemCodepage);
    SetConsoleCP(systemCodepage);
}

Consolero::~Consolero()
{
	//reset Console mode
	SetConsoleMode(hStdin, m_OldConsoleMode);
}

std::string Consolero::Cin()
{
	const int bufferSize = 128;
	INPUT_RECORD irInBuf[bufferSize];
	unsigned long cNumRead;
    bool lineFinished = false;
	while (!lineFinished) {
		ReadConsoleInput(hStdin, irInBuf, bufferSize, &cNumRead);

		for (unsigned int i = 0; i < cNumRead; i++)
		{
			switch (irInBuf[i].EventType)
			{
			case KEY_EVENT:
				KEY_EVENT_RECORD & event = irInBuf[i].Event.KeyEvent;
				for (unsigned int j = 0; j < event.wRepeatCount; j++) {
					if (event.bKeyDown) {
						lineFinished = handleKeyEvent(event);
					}
				}
			}
		};

	}

    return m_currentLine.content;

}


bool Consolero::handleKeyEvent(const KEY_EVENT_RECORD& keyEvent)
{
    bool lineFinished = false;
	if (keyEvent.wVirtualKeyCode == 9) //Tab
	{
		printf("TAB");
	}
    else if (keyEvent.wVirtualKeyCode == 0x0D) // enter
    {
        lineFinished = true;
        return lineFinished;
       
    }
	else if (keyEvent.wVirtualKeyCode == 8) // Backspace
	{
		backspace();
	}
	else if (keyEvent.wVirtualKeyCode == 27) // ESC
	{
		escape();
	}
	else if (keyEvent.wVirtualKeyCode == 46) // DEL
	{
		del();
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
	else if (keyEvent.wVirtualKeyCode == 0x0024) // Pos1
	{
		m_currentLine.cursorPosition = 0;
	}
	else if (keyEvent.wVirtualKeyCode == 0x0023) // END
	{
		m_currentLine.cursorPosition = m_currentLine.content.size()-1;
	}
	else if (keyEvent.wVirtualKeyCode == 112) // F1
	{
	}
	else if (iswprint(keyEvent.uChar.UnicodeChar)) {
        
        std::wstring wstr{ keyEvent.uChar.UnicodeChar };
        std::string converted = wstringToString(wstr);
        m_currentLine.content.insert(m_currentLine.cursorPosition, converted);
       
		//TODO: Enable insert mode

		m_currentLine.cursorPosition += 1;
		if (m_currentLine.cursorPosition == m_currentLine.content.size()) {
			m_currentLine.addSpace();
		}
	}
	else {
		printf("?");
	}
	displayLine(m_currentLine);
    return lineFinished;
;
}

void Consolero::displayLine(const ConsoleLine & line)
{
	CONSOLE_SCREEN_BUFFER_INFO lConsoleScreenBufferInfo;
	GetConsoleScreenBufferInfo(hStdout, &lConsoleScreenBufferInfo);

	COORD& bufferSize = lConsoleScreenBufferInfo.dwSize;
	COORD current = line.start;

	for (int i = 0; i < line.content.size(); ++i) { //TODO: Replace character-wise loop with line-wise loop
		SMALL_RECT consoleWriteArea = { current.X, current.Y, current.X, current.Y };
        std::wstring output = stringToWstring(std::string(&line.content[i], 1));

        CHAR_INFO newChar;
        newChar.Char.UnicodeChar = output[0]; //TODO: This is going to cause problems when output.size()>1!
        newChar.Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;

		WriteConsoleOutput(hStdout, &newChar, { 1,1 }, { 0,0 }, &consoleWriteArea);
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

void Consolero::clearLine(ConsoleLine & line)
{
	for (int i = 0; i < line.content.size(); ++i) {
		line.content[i] = ' ';
	}

}

std::string Consolero::wstringToString(std::wstring w)
{
    auto systemCodepage = GetACP();
    int size = WideCharToMultiByte(systemCodepage, 0, &w[0], static_cast<int>(w.size()), NULL, 0, NULL, NULL);
    std::string converted(size, 0);
    WideCharToMultiByte(systemCodepage, 0, &w[0], static_cast<int>(w.size()), &converted[0], size, NULL, NULL);
    return converted;

}

std::wstring Consolero::stringToWstring(std::string c)
{
    auto systemCodepage = GetACP();
    int size = MultiByteToWideChar(systemCodepage, 0, &c[0], static_cast<int>(c.size()), NULL, 0);
    std::wstring converted(size, 0);
    MultiByteToWideChar(systemCodepage, 0, &c[0], static_cast<int>(c.size()), &converted[0], size);
    return converted;

}

void Consolero::backspace()
{
	auto& cursorPosition = m_currentLine.cursorPosition;
	if (cursorPosition > 0) {
		m_currentLine.content.erase(m_currentLine.content.begin() + cursorPosition - 1);
        cursorPosition -= 1;
	}
	
}

void Consolero::escape()
{
	clearLine(m_currentLine);
	displayLine(m_currentLine);
	m_currentLine.content.clear();
	m_currentLine.addSpace();
	m_currentLine.cursorPosition = 0;

}

void Consolero::del()
{
	auto& cursorPosition = m_currentLine.cursorPosition;
	if ( cursorPosition < m_currentLine.content.size() -1 ) {
		m_currentLine.content.erase(m_currentLine.content.begin() + cursorPosition);
	}
	//cursorPosition -= 1;
}
