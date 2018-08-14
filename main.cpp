#include "consolero.h"
#include <iostream>
#include <locale>
#include <codecvt>

int main(int argc, _TCHAR* argv[]) {
	
	//int a;
	//std::cin >> a;
	Consolero console;
	auto line = console.Cin();
    std::cout << "\n" << line << "\n";
    //std::wstring_convert<std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>> cv;
    //std::string convertedString = cv.to_bytes(line);
    //std::cout << "\n" << convertedString  << "\n";

}