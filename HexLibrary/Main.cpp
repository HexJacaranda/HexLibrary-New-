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
	y = x << x;
	y = x >> x;
	y = x | x;
	y = x & x;
	y = ~x;
	y = x ^ x;

	//Int32 divider = 4;
	//X86Emitter emitter;
	//auto context = newptr<EmitContext>();
	//auto page = newptr<OSToEE::Windows::ExecutablePage>(32);
	//emitter.SetEmitContext(context.GetObjectPtr());
	//emitter.SetExecutablePage(page);
	//emitter.StartEmitting();

	////mov ebx,80
	//emitter.EmitStoreImmediateToRegister(Register::BX, 80, SlotType::Int32);
	////mov eax,[divider]
	//emitter.EmitLoadMemoryToRegisterViaImmediate(Register::AX, (Int64)&divider, SlotType::Int32);
	////cdq
	////idiv ebx,eax
	//emitter.EmitDivRegisterToRegister(Register::BX, Register::AX, SlotType::Int32, ArithmeticType::Signed);
	////mov eax,ebx
	//emitter.EmitLoadRegisterToRegister(Register::AX, Register::BX, SlotType::Int32);
	////ret
	//emitter.EmitReturn();
	//
	//
	//emitter.CommitEmitting();
	//Fn fn = (Fn)page->GetRawPage();
	//std::wcout << fn() << std::endl;
}

