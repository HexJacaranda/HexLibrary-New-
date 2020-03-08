#include <iostream>
#include <vector>
#include <iomanip>
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
	__asm
	{
		add word ptr[ebx], 0x12345678
	}
}

