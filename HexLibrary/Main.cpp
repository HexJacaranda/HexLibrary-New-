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

template<int N,int Divisor>
struct IsPrime
{
	static constexpr bool Value = N % Divisor == 0 ? false : IsPrime<N, Divisor - 1>::Value;
};

template<int N>
struct IsPrime<N, 1>
{
	static constexpr bool Value = true;
};

template<int Count,int N,int TargetCount>
struct NthPrimeHelper
{
	static constexpr int Prime = IsPrime<N, N - 1>::Value ? 
		NthPrimeHelper<Count + 1, N + 1, TargetCount>::Prime:
		NthPrimeHelper<Count, N + 1, TargetCount>::Prime;
};


template<int Same,int N>
struct NthPrimeHelper<Same, N, Same>
{
	static constexpr int Prime = N - 1;
};

template<int N>
struct NthPrime
{
	static constexpr int Prime = NthPrimeHelper<0, 2, N>::Prime;
};

void Main()
{

}
