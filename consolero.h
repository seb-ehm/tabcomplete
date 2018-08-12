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

void ErrorExit(LPTSTR lpszFunction);

struct ConsoleLine {
	COORD start;
	std::vector<CHAR_INFO> content;
	std::vector<CHAR_INFO>::size_type cursorPosition;

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
		CHAR_INFO space;
		space.Char.AsciiChar = ' ';
		space.Attributes = 0;
		content.push_back(space);
	}
};

class Consolero {
public:
	Consolero();
	~Consolero();

	void Cin();

private:
	void handleKeyEvent(const KEY_EVENT_RECORD& keyEvent);
	void displayLine(const ConsoleLine& line);
	void clearLine(ConsoleLine& line);
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