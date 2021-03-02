#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <array>
#include "Include.h"



using namespace HL::System::Collection::Generic;
using namespace HL::System::Collection;
using namespace HL::System::Text;
using namespace HL::System::Text::RegularExpression;
using namespace HL::System::Pointer;
using namespace HL::System::Pointer::Reference;
using namespace HL::System;
using namespace HL::System::Threading::Tasks;
using namespace HL::System::GC;

void Main();

int main() {
	Diagnosis::Memory::EnableMemoryLeakReport();
	std::wcout.imbue(std::locale("chs"));
	try {
		Main();
	}
	catch (Exception::IException& e) {
		std::wcout << e.Message() << std::endl;
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	catch (...) {
		std::wcout << L"Non-FCL exception occurs" << std::endl;
	}
	system("pause");
	return 0;
}

void Main()
{
	int x = 0;
	std::cin >> x;
	if (x < 1)
	{
		std::wcout << L"L" << std::endl;
	}
	if (x > 2)
	{
		std::wcout << L"G" << std::endl;
	}
	if (x == 0)
	{
		std::wcout << L"E" << std::endl;
	}

	if (x != 0)
	{
		std::wcout << L"NE" << std::endl;
	}

	if (x <= 0)
	{
		std::wcout << L"LE" << std::endl;
	}

	if (x >= 0)
	{
		std::wcout << L"GE" << std::endl;
	}
}
