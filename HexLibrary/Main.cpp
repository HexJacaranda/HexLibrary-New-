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

using namespace HL::System::Runtime;
using namespace HL::System::Runtime::JIT::Emit;
using namespace HL::System::Runtime::JIT::Emit::X86;
using namespace HL::System::Runtime::Interfaces;

typedef int(__fastcall *Fn)();


void Main()
{
	Int32 x = 2;
	Int32 y = 0;
	y = x << 1;
	y = x << 4;
	y = x >> 4;
	y = x << x;
	y = x >> x;
	y = x | x;
	y = x | 2;
	y = x & x;
	y = x & 3;
	y = ~x;
	y = x ^ x;
	
}
