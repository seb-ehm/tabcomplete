#ifndef CONSOLERO_INCLUDE
#define CONSOLERO_INCLUDE
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <Tchar.h>
#include <iostream>
#include <cctype>
#include <strsafe.h>

void ErrorExit(LPTSTR lpszFunction);

class Consolero {
public:
	Consolero();
	~Consolero();

	void Cin();

private:
	HANDLE hStdin;
	HANDLE hStdout;
	DWORD  m_OldConsoleMode;
};


#endif