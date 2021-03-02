#include "pch.h"
#include "CppUnitTest.h"
#include "../HexLibrary/Runtime/RuntimeAlias.h"
#include "../HexLibrary/Runtime/Core/Exception/RuntimeException.h"
#include "../HexLibrary/Runtime/Core/JIT/Emitter/Emitter.h"
#include "../HexLibrary/Runtime/Core/JIT/Emitter/X86Emitter.h"
#include "../HexLibrary/Runtime/Core/JIT/Emitter/Page/ExecutablePage.h"
#include "../HexLibrary/Runtime/Core/JIT/Emitter/Page/WindowsExecutablePage.h"
#include "../HexLibrary/Include.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Runtime;
using namespace Runtime::Core::JIT::Emit;
using namespace Runtime::Core::JIT::Emit::X86;
using namespace Runtime::Core::Interfaces;
using namespace HL::System::Pointer;
using namespace HL::System::Pointer::Reference;

namespace EmitterUnitTest
{
	TEST_CLASS(EmitterInX86UnitTest)
	{
	public:
		X86Emitter emitter;
		ptr<EmitContext> context = newptr<EmitContext>();
		EmitterInX86UnitTest() {
			emitter.SetEmitContext(context.GetObjectPtr());
		}

		template<class R, class...Args, class Fn>
		R EmitAs(Fn&& fn)
		{
			auto page = newptr<RTIOS2EE::Windows::ExecutablePage>(32);
			emitter.SetExecutablePage(page.GetObjectPtr());
			emitter.StartEmitting();
			std::forward<Fn>(fn)();
			emitter.CommitEmitting();
			using F = R(__fastcall*)(Args...);
			return ((F)page->GetRawPage())();
		}

		TEST_METHOD(SImm2R)
		{
			Assert::AreEqual(10,
				EmitAs<Int32>([this]() {
					emitter.EmitStoreImmediateToRegister(Register::AX, 10, SlotType::Int32);
					emitter.EmitReturn();
					}));
			Logger::WriteMessage(L"mov rax,Imm");
		}

		TEST_METHOD(SR2R)
		{
			Assert::AreEqual(10,
				EmitAs<Int32>([this]() {
					emitter.EmitStoreImmediateToRegister(Register::BX, 10, SlotType::Int32);
					emitter.EmitLoadRegisterToRegister(Register::AX, Register::BX, SlotType::Int32);
					emitter.EmitReturn();
					}));
			Logger::WriteMessage(L"mov eax,ebx");
		}

		TEST_METHOD(SImm2MVR)
		{
			Int32 value = 0;
			EmitAs<void>([&, this]() {
				emitter.EmitStoreImmediateToRegister(Register::AX, (Int64)&value, SlotType::Int32);
				emitter.EmitStoreImmediateToMemoryViaRegister(Register::AX, 10ll, SlotType::Int32);
				emitter.EmitReturn();
				});
			Assert::AreEqual(10, value);
			Logger::WriteMessage(L"mov [eax],Imm");
		}


		TEST_METHOD(SImm2MVImm)
		{
			Int32 value = 0;
			EmitAs<void>([&, this]() {
				emitter.EmitStoreImmediateToMemoryViaImmediate((Int64)&value, 10, SlotType::Int32);
				emitter.EmitReturn();
				});
			Assert::AreEqual(10, value);
			Logger::WriteMessage(L"mov [Addr],Imm");
		}

		TEST_METHOD(SM2RVR)
		{
			Int32 value = 10;
			Assert::AreEqual(10,
				EmitAs<Int32>([&, this]() {
					emitter.EmitStoreImmediateToRegister(Register::BX, (Int64)&value, SlotType::Int32);
					emitter.EmitLoadMemoryToRegisterViaRegister(Register::AX, Register::BX, SlotType::Int32);
					emitter.EmitReturn();
					}));
			Logger::WriteMessage(L"mov eax,[ebx]");
		}


		TEST_METHOD(SM2RVImm)
		{
			Int32 value = 10;
			Assert::AreEqual(10,
				EmitAs<Int32>([&, this]() {
					emitter.EmitLoadMemoryToRegisterViaImmediate(Register::AX, (Int64)&value, SlotType::Int32);
					emitter.EmitReturn();
					}));
			Logger::WriteMessage(L"mov eax,[Imm]");
		}

		TEST_METHOD(AddR2R)
		{
			Assert::AreEqual(2,
				EmitAs<Int32>([&, this]() {
					emitter.EmitStoreImmediateToRegister(Register::AX, 1, SlotType::Int32);
					emitter.EmitStoreImmediateToRegister(Register::BX, 1, SlotType::Int32);
					emitter.EmitAddRegisterToRegister(Register::AX, Register::BX, SlotType::Int32, ArithmeticType::Signed);
					emitter.EmitReturn();
					}));
			Logger::WriteMessage(L"add eax,ebx");
		}

		TEST_METHOD(AddImm2R)
		{
			Assert::AreEqual(2,
				EmitAs<Int32>([&, this]() {
					emitter.EmitStoreImmediateToRegister(Register::AX, 1, SlotType::Int32);
					emitter.EmitAddImmediateToRegister(Register::AX, 1, SlotType::Int32, ArithmeticType::Signed);
					emitter.EmitReturn();
					}));
			Logger::WriteMessage(L"add eax,Imm");
		}

		TEST_METHOD(SubR2R)
		{
			Assert::AreEqual(0,
				EmitAs<Int32>([&, this]() {
					emitter.EmitStoreImmediateToRegister(Register::AX, 1, SlotType::Int32);
					emitter.EmitStoreImmediateToRegister(Register::BX, 1, SlotType::Int32);
					emitter.EmitSubRegisterToRegister(Register::AX, Register::BX, SlotType::Int32, ArithmeticType::Signed);
					emitter.EmitReturn();
					}));
			Logger::WriteMessage(L"sub eax,ebx");
		}

		TEST_METHOD(SubImm2R)
		{
			Assert::AreEqual(0,
				EmitAs<Int32>([&, this]() {
					emitter.EmitStoreImmediateToRegister(Register::AX, 1, SlotType::Int32);
					emitter.EmitSubImmediateToRegister(Register::AX, 1, SlotType::Int32, ArithmeticType::Signed);
					emitter.EmitReturn();
					}));
			Logger::WriteMessage(L"sub eax,Imm");
		}

		TEST_METHOD(MulR2R)
		{
			Assert::AreEqual(4,
				EmitAs<Int32>([&, this]() {
					emitter.EmitStoreImmediateToRegister(Register::AX, 2, SlotType::Int32);
					emitter.EmitStoreImmediateToRegister(Register::BX, 2, SlotType::Int32);
					emitter.EmitMulRegisterToRegister(Register::AX, Register::BX, SlotType::Int32, ArithmeticType::Signed);
					emitter.EmitReturn();
					}));
			Logger::WriteMessage(L"mul eax,ebx");
		}

		TEST_METHOD(MulImm2R)
		{
			Assert::AreEqual(4,
				EmitAs<Int32>([&, this]() {
					emitter.EmitStoreImmediateToRegister(Register::AX, 2, SlotType::Int32);
					emitter.EmitMulImmediateToRegister(Register::AX, 2, SlotType::Int32, ArithmeticType::Signed);
					emitter.EmitReturn();
					}));
			Logger::WriteMessage(L"mul eax,Imm");
		}

		TEST_METHOD(DivR2R)
		{
			Assert::AreEqual(2,
				EmitAs<Int32>([&, this]() {
					emitter.EmitStoreImmediateToRegister(Register::AX, 4, SlotType::Int32);
					emitter.EmitStoreImmediateToRegister(Register::BX, 2, SlotType::Int32);
					emitter.EmitDivRegisterToRegister(Register::AX, Register::BX, SlotType::Int32, ArithmeticType::Signed);
					emitter.EmitReturn();
					}));
			Logger::WriteMessage(L"div eax,ebx");
		}

		TEST_METHOD(DivImm2R)
		{
			Assert::AreEqual(2,
				EmitAs<Int32>([&, this]() {
					emitter.EmitStoreImmediateToRegister(Register::AX, 4, SlotType::Int32);
					emitter.EmitDivImmediateToRegister(Register::AX, 2, SlotType::Int32, ArithmeticType::Signed);
					emitter.EmitReturn();
					}));
			Logger::WriteMessage(L"div eax,Imm");
		}

		TEST_METHOD(PushImm)
		{
			Assert::AreEqual(2,
				EmitAs<Int32>([&, this]() {
					emitter.EmitPushImmediate(2, SlotType::Int32);
					emitter.EmitPopToRegister(Register::AX, SlotType::Int32);
					emitter.EmitReturn();
					}));
			Logger::WriteMessage(L"push Imm");
		}

		TEST_METHOD(PushR)
		{
			Assert::AreEqual(2,
				EmitAs<Int32>([&, this]() {
					emitter.EmitStoreImmediateToRegister(Register::BX, 2, SlotType::Int32);
					emitter.EmitPushRegister(Register::BX, SlotType::Int32);
					emitter.EmitPopToRegister(Register::AX, SlotType::Int32);
					emitter.EmitReturn();
					}));
			Logger::WriteMessage(L"push ebx");
		}

		TEST_METHOD(PushMVImm)
		{
			Int32 value = 2;
			Assert::AreEqual(2,
				EmitAs<Int32>([&, this]() {
					emitter.EmitPushMemoryViaImmediate((Int64)&value, SlotType::Int32);
					emitter.EmitPopToRegister(Register::AX, SlotType::Int32);
					emitter.EmitReturn();
					}));
			Logger::WriteMessage(L"push [Addr]");
		}

		TEST_METHOD(PushMVR)
		{
			Int32 value = 2;
			Assert::AreEqual(2,
				EmitAs<Int32>([&, this]() {
					emitter.EmitStoreImmediateToRegister(Register::AX, (Int64)&value, SlotType::Int32);
					emitter.EmitPushMemoryViaRegister(Register::AX, SlotType::Int32);
					emitter.EmitPopToRegister(Register::AX, SlotType::Int32);
					emitter.EmitReturn();
					}));
			Logger::WriteMessage(L"push [eax]");
		}

		TEST_METHOD(Pop2R)
		{
			Assert::AreEqual(2,
				EmitAs<Int32>([&, this]() {
					emitter.EmitPushImmediate(2, SlotType::Int32);
					emitter.EmitPopToRegister(Register::AX, SlotType::Int32);
					emitter.EmitReturn();
					}));
			Logger::WriteMessage(L"pop eax");
		}

		TEST_METHOD(Pop2MVImm)
		{
			Int32 value = 0;
			EmitAs<void>([&, this]() {
				emitter.EmitPushImmediate(2, SlotType::Int32);
				emitter.EmitPopToMemoryViaImmediate((Int64)&value, SlotType::Int32);
				emitter.EmitReturn();
				});
			Assert::AreEqual(2, value);
			Logger::WriteMessage(L"pop [Addr]");
		}

		TEST_METHOD(Pop2MVR)
		{
			Int32 value = 0;
			EmitAs<void>([&, this]() {
				emitter.EmitPushImmediate(2, SlotType::Int32);
				emitter.EmitStoreImmediateToRegister(Register::AX, (Int64)&value, SlotType::Int32);
				emitter.EmitPopToMemoryViaRegister(Register::AX, SlotType::Int32);
				emitter.EmitReturn();
				});
			Assert::AreEqual(2, value);
			Logger::WriteMessage(L"pop [eax]");
		}

		TEST_METHOD(LShiftRBImm)
		{
			Assert::AreEqual(0b100, EmitAs<Int32>([&, this]() {
				emitter.EmitStoreImmediateToRegister(Register::AX, 0b001, SlotType::Int32);
				emitter.EmitLShiftRegisterByImmediateTimes(Register::AX, 2, SlotType::Int8, ArithmeticType::Signed);
				emitter.EmitReturn();
				}));
			Logger::WriteMessage(L"shl eax, 2");
		}

		TEST_METHOD(LShiftRB1)
		{
			Assert::AreEqual(0b10, EmitAs<Int32>([&, this]() {
				emitter.EmitStoreImmediateToRegister(Register::AX, 0b01, SlotType::Int32);
				emitter.EmitLShiftRegisterByImmediateTimes(Register::AX, 1, SlotType::Int8, ArithmeticType::Signed);
				emitter.EmitReturn();
				}));
			Logger::WriteMessage(L"shl eax, 1");
		}

		TEST_METHOD(LShiftRBR)
		{
			Assert::AreEqual(0b10, EmitAs<Int32>([&, this]() {
				emitter.EmitStoreImmediateToRegister(Register::AX, 0b01, SlotType::Int32);
				emitter.EmitStoreImmediateToRegister(Register::BX, 1, SlotType::Int32);
				emitter.EmitLShiftRegisterByRegisterTimes(Register::AX, Register::BX, SlotType::Int32, ArithmeticType::Signed);
				emitter.EmitReturn();
				}));
			Logger::WriteMessage(L"shl eax, ebx");
		}

		TEST_METHOD(RShiftRBImm)
		{
			Assert::AreEqual(0b001, EmitAs<Int32>([&, this]() {
				emitter.EmitStoreImmediateToRegister(Register::AX, 0b100, SlotType::Int32);
				emitter.EmitRShiftRegisterByImmediateTimes(Register::AX, 2, SlotType::Int8, ArithmeticType::Signed);
				emitter.EmitReturn();
				}));
			Logger::WriteMessage(L"shr eax, 2");
		}

		TEST_METHOD(RShiftRB1)
		{
			Assert::AreEqual(0b01, EmitAs<Int32>([&, this]() {
				emitter.EmitStoreImmediateToRegister(Register::AX, 0b10, SlotType::Int32);
				emitter.EmitRShiftRegisterByImmediateTimes(Register::AX, 1, SlotType::Int8, ArithmeticType::Signed);
				emitter.EmitReturn();
				}));
			Logger::WriteMessage(L"shr eax, 1");
		}

		TEST_METHOD(RShiftRBR)
		{
			Assert::AreEqual(0b01, EmitAs<Int32>([&, this]() {
				emitter.EmitStoreImmediateToRegister(Register::AX, 0b10, SlotType::Int32);
				emitter.EmitStoreImmediateToRegister(Register::BX, 1, SlotType::Int32);
				emitter.EmitRShiftRegisterByRegisterTimes(Register::AX, Register::BX, SlotType::Int32, ArithmeticType::Signed);
				emitter.EmitReturn();
				}));
			Logger::WriteMessage(L"shr eax, ebx");
		}

		TEST_METHOD(JmpShortImm)
		{
			Assert::AreEqual(2, EmitAs<Int32>([&, this]() {
				auto entry = emitter.EmitJmpViaImmediate(0, SlotType::Int8, RedirectSemantic::Relative);
				auto nowPosition = emitter.CurrentPosition();
				emitter.EmitStoreImmediateToRegister(Register::AX, 1, SlotType::Int32);
				emitter.EmitReturn();
				emitter.UpdateFlowControlTo((Int64)emitter.CurrentPosition() - nowPosition, entry);
				emitter.EmitStoreImmediateToRegister(Register::AX, 2, SlotType::Int32);
				emitter.EmitReturn();
				}));
			Logger::WriteMessage(L"jmp short imm");
		}

		TEST_METHOD(Jcc)
		{
			Assert::AreEqual(2, EmitAs<Int32>([&, this]() {
				emitter.EmitStoreImmediateToRegister(Register::AX, 0, SlotType::Int32);
				emitter.EmitCompareRegisterWithImmediate(Register::AX, 0, SlotType::Int32);
				auto entry = emitter.EmitJcc(Condition::EqualTo, 0, SlotType::Int32, RedirectSemantic::Relative);
				auto nowPosition = emitter.CurrentPosition();
				emitter.EmitStoreImmediateToRegister(Register::AX, 1, SlotType::Int32);
				emitter.EmitReturn();
				emitter.UpdateFlowControlTo((Int64)emitter.CurrentPosition() - nowPosition, entry);
				emitter.EmitStoreImmediateToRegister(Register::AX, 2, SlotType::Int32);
				emitter.EmitReturn();
				}));
			Logger::WriteMessage(L"je short imm");

			Assert::AreEqual(2, EmitAs<Int32>([&, this]() {
				emitter.EmitStoreImmediateToRegister(Register::AX, 0, SlotType::Int32);
				emitter.EmitCompareRegisterWithImmediate(Register::AX, -1, SlotType::Int32);
				auto entry = emitter.EmitJcc(Condition::EqualTo, 0, SlotType::Int32, RedirectSemantic::Relative);
				auto nowPosition = emitter.CurrentPosition();
				emitter.EmitStoreImmediateToRegister(Register::AX, 1, SlotType::Int32);
				emitter.EmitReturn();
				emitter.UpdateFlowControlTo((Int64)emitter.CurrentPosition() - nowPosition, entry);
				emitter.EmitStoreImmediateToRegister(Register::AX, 2, SlotType::Int32);
				emitter.EmitReturn();
				}));
			Logger::WriteMessage(L"jg short imm");

			Assert::AreEqual(2, EmitAs<Int32>([&, this]() {
				emitter.EmitStoreImmediateToRegister(Register::AX, 0, SlotType::Int32);
				emitter.EmitCompareRegisterWithImmediate(Register::AX, 1, SlotType::Int32);
				auto entry = emitter.EmitJcc(Condition::EqualTo, 0, SlotType::Int32, RedirectSemantic::Relative);
				auto nowPosition = emitter.CurrentPosition();
				emitter.EmitStoreImmediateToRegister(Register::AX, 1, SlotType::Int32);
				emitter.EmitReturn();
				emitter.UpdateFlowControlTo((Int64)emitter.CurrentPosition() - nowPosition, entry);
				emitter.EmitStoreImmediateToRegister(Register::AX, 2, SlotType::Int32);
				emitter.EmitReturn();
				}));
			Logger::WriteMessage(L"jl short imm");

			Assert::AreEqual(2, EmitAs<Int32>([&, this]() {
				emitter.EmitStoreImmediateToRegister(Register::AX, 0, SlotType::Int32);
				emitter.EmitCompareRegisterWithImmediate(Register::AX, 0, SlotType::Int32);
				auto entry = emitter.EmitJcc(Condition::EqualTo, 0, SlotType::Int32, RedirectSemantic::Relative);
				auto nowPosition = emitter.CurrentPosition();
				emitter.EmitStoreImmediateToRegister(Register::AX, 1, SlotType::Int32);
				emitter.EmitReturn();
				emitter.UpdateFlowControlTo((Int64)emitter.CurrentPosition() - nowPosition, entry);
				emitter.EmitStoreImmediateToRegister(Register::AX, 2, SlotType::Int32);
				emitter.EmitReturn();
				}));
			Logger::WriteMessage(L"jge short imm");

			Assert::AreEqual(2, EmitAs<Int32>([&, this]() {
				emitter.EmitStoreImmediateToRegister(Register::AX, 0, SlotType::Int32);
				emitter.EmitCompareRegisterWithImmediate(Register::AX, 0, SlotType::Int32);
				auto entry = emitter.EmitJcc(Condition::EqualTo, 0, SlotType::Int32, RedirectSemantic::Relative);
				auto nowPosition = emitter.CurrentPosition();
				emitter.EmitStoreImmediateToRegister(Register::AX, 1, SlotType::Int32);
				emitter.EmitReturn();
				emitter.UpdateFlowControlTo((Int64)emitter.CurrentPosition() - nowPosition, entry);
				emitter.EmitStoreImmediateToRegister(Register::AX, 2, SlotType::Int32);
				emitter.EmitReturn();
				}));
			Logger::WriteMessage(L"jle short imm");

			Assert::AreEqual(2, EmitAs<Int32>([&, this]() {
				emitter.EmitStoreImmediateToRegister(Register::AX, 0, SlotType::Int32);
				emitter.EmitCompareRegisterWithImmediate(Register::AX, 3, SlotType::Int32);
				auto entry = emitter.EmitJcc(Condition::EqualTo, 0, SlotType::Int32, RedirectSemantic::Relative);
				auto nowPosition = emitter.CurrentPosition();
				emitter.EmitStoreImmediateToRegister(Register::AX, 1, SlotType::Int32);
				emitter.EmitReturn();
				emitter.UpdateFlowControlTo((Int64)emitter.CurrentPosition() - nowPosition, entry);
				emitter.EmitStoreImmediateToRegister(Register::AX, 2, SlotType::Int32);
				emitter.EmitReturn();
				}));
			Logger::WriteMessage(L"jne short imm");
		}
	};
}