#include "consolero.h"
#include <iostream>
#include <locale>
#include <codecvt>

int main(int argc, _TCHAR* argv[]) {
	
	//int a;
	//std::cin >> a;
	Consolero console;
	auto line = console.Cin();
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;
    std::string convertedString = cv.to_bytes(line);
    std::cout << "\n" << convertedString  << "\n";

}