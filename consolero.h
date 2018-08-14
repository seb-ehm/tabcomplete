#ifndef CONSOLERO_INCLUDE
#define CONSOLERO_INCLUDE
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <Tchar.h>
#include <iostream>
#include <cctype>
#include <strsafe.h>
#include <string>

void ErrorExit(LPTSTR lpszFunction);

struct ConsoleLine {
	COORD start;
	std::string content;
	std::string::size_type cursorPosition;

	void moveCursor(SHORT steps) {
		if (steps > 0) {
			cursorPosition = (cursorPosition + steps >= content.size()-1 ? content.size()-1 : cursorPosition + steps);
		}
		else
		{
			cursorPosition = (cursorPosition + steps >= 0 ? cursorPosition + steps : 0);
		}
	}

	void addSpace() {
		//CHAR_INFO space;
		//space.Char.UnicodeChar = 0x20; //space
		//space.Attributes = 0;
		content.push_back(' ');
	}
};

class Consolero {
public:
	Consolero();
	~Consolero();

	std::string Cin();

private:
	bool handleKeyEvent(const KEY_EVENT_RECORD& keyEvent);
	void displayLine(const ConsoleLine& line);
	void clearLine(ConsoleLine& line);

    std::string  wstringToString(std::wstring w);
    std::wstring stringToWstring(std::string c);

	void backspace();
	void escape();
	void del();

	HANDLE hStdin;
	HANDLE hStdout;
	DWORD  m_OldConsoleMode;
	COORD  m_InitialCoord;
	ConsoleLine m_currentLine;

};


#endif