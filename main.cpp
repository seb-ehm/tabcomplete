#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <Tchar.h>
#include <iostream>
#include <cctype>
#include <strsafe.h>



int main(int argc, _TCHAR* argv[]) {
	const int bufferSize = 128;
	INPUT_RECORD irInBuf[bufferSize];
	unsigned long cNumRead;

	// Set up the handles for reading/writing:

	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

	// Change the window title:
	SetConsoleTitle(TEXT("Win32 Console Control Demo"));
	
	
	CONSOLE_SCREEN_BUFFER_INFO lConsoleScreenBufferInfo;
	//disable mouse input capture
	DWORD dwConsoleMode;
	GetConsoleMode(hStdin, &dwConsoleMode);
	SetConsoleMode(hStdin, (~ENABLE_MOUSE_INPUT) & dwConsoleMode);

	while (1) {
		ReadConsoleInput(hStdin, irInBuf, bufferSize, &cNumRead);  

			for (unsigned int i = 0; i< cNumRead; i++)
			{
				switch (irInBuf[i].EventType)
				{
				case KEY_EVENT:
					KEY_EVENT_RECORD& event = irInBuf[i].Event.KeyEvent;
					for (unsigned int j = 0; j < event.wRepeatCount; j++) {
						if (event.bKeyDown) {
							if (event.uChar.UnicodeChar == '\t') {
								printf("TAB");
							}
							else {
							
								GetConsoleScreenBufferInfo(hStdout, &lConsoleScreenBufferInfo);
								COORD currentCoord = lConsoleScreenBufferInfo.dwCursorPosition;
								if (event.wVirtualKeyCode == 8) { //Backspace
									CHAR_INFO space;
								    space.Char.AsciiChar=' ';
									space.Attributes = 0 ;// FOREGROUND_BLUE | FOREGROUND_GREEN |
										//FOREGROUND_INTENSITY;
									short newX = currentCoord.X-1;
									short newY = currentCoord.Y;
									COORD newCoord = { newX, newY };
									SMALL_RECT consoleWriteArea = { newX, newY, newX, newY };
									if (!WriteConsoleOutput(hStdout, &space, { 1,1 }, { 0,0 }, &consoleWriteArea)) {
										ErrorExit(TEXT("WCO"));
									};
									SetConsoleCursorPosition(hStdout, newCoord);
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