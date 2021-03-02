#pragma once
#include "..\..\..\RuntimeAlias.h"
#include "Emitter.h"
#include "..\..\Exception\RuntimeException.h"
#include "..\..\..\Binary.h"

namespace RTJE::X86
{
	class Prefix
	{
	public:
		static constexpr Int8 Register16bit = 0x66;
		static constexpr Int8 Register32bit = 0x67;
		static constexpr Int8 Register64bit = 0x48;
	};

	class Register
	{
	public:
		static constexpr Int8 AX = 0b000;
		static constexpr Int8 CX = 0b001;
		static constexpr Int8 DX = 0b010;
		static constexpr Int8 BX = 0b011;
		static constexpr Int8 SP = 0b100;
		static constexpr Int8 BP = 0b101;
		static constexpr Int8 SI = 0b110;
		static constexpr Int8 DI = 0b111;
		static constexpr Int8 XMM[] = {
			0b000,0b001,0b010,0b011,0b100,0b101,0b110,0b111
		};
		static constexpr Int8 Shift = 3;
		static constexpr Int8 Empty = 0b000;
	};

	class AddressingMode
	{
	public:
		static constexpr Int8 RegisterAddressing = 0b00;
		static constexpr Int8 RegisterAddressingWithOffsetOn8bit = 0b01;
		static constexpr Int8 RegisterAddressingWithOffsetOn16Or32bit = 0b10;
		static constexpr Int8 Register = 0b11;
		static constexpr Int8 Shift = 6;
	};

	class Context
	{
	public:
		static constexpr Int32 PlatformMask = 0b10;
		static void OperandSlice(EmitContext* context, SlotType To) {
			throw std::exception("Oops! Operand slice hanppened!");
			context->EmitState |= (Int8)To;
		}
		static void OperandPromote(EmitContext* context, SlotType To) {
			context->EmitState |= (Int8)To;
		}
		static bool X86(EmitContext* context) {
			return context->CommonFlags & PlatformMask;
		}
	};

	class RegisterOrMemory :public Register
	{
	public:
		//Enables only when it's not AddressingMode::Register
		static constexpr Int8 SIBAddressing = 0b100;
		//Enables only when it's AddressingMode::RegisterAddressing
		static constexpr Int8 Offset32bit = 0b101;
	};

	class Redirect
	{
	public:
		static constexpr Int8 Near = 0;
		static constexpr Int8 Far = 1;
	};

	static inline constexpr Int8 ModRM(Int8 Mod, Int8 Reg, Int8 RM) {
		return (Mod << AddressingMode::Shift) | (Reg << Register::Shift) | RM;
	}
	static inline constexpr Int8 SIB(Int8 Scale, Int8 Base, Int8 Index) {
		return (Scale << 6) | (Base << 3) | Index;
	}

	class X86OpCodes
	{
		//Imm = immediate
		//R = register
		//V = via
		//M = memory
		//REX = REX prefix
		//4 = for
		//W = with
	public:
		static constexpr Int8 REX[] = { 0,Prefix::Register16bit,0,Prefix::Register64bit };

		static constexpr Int8 MovImmToR[] = { (Int8)0xB0,(Int8)0xB8,(Int8)0xB8,(Int8)0xB8 };

		static constexpr Int8 MovImmToM[] = { (Int8)0xC6,(Int8)0xC7,(Int8)0xC7,(Int8)0xC7 };

		static constexpr Int8 MovRToR[] = { (Int8)0x88,(Int8)0x89,(Int8)0x89,(Int8)0x89 };

		static constexpr Int8 MovRToMVR[] = { (Int8)0x88,(Int8)0x89,(Int8)0x89,(Int8)0x89 };

		static constexpr Int8 MovMToRVImm[] = { (Int8)0xA0,(Int8)0xA1,(Int8)0xA1,(Int8)0xA1 };

		static constexpr Int8 MovRToMVImm[] = { (Int8)0xA2,(Int8)0xA3,(Int8)0xA3,(Int8)0xA3 };

		static constexpr Int8 MovMToRVR[] = { (Int8)0x8A,(Int8)0x8B,(Int8)0x8B,(Int8)0x8B };

		static constexpr Int8 AddRToR[] = { (Int8)0x01,(Int8)0x01,(Int8)0x01,(Int8)0x01 };

		static constexpr Int8 AddImmToR[] = { (Int8)0x81,(Int8)0x82,(Int8)0x82,(Int8)0x82 };

		static constexpr Int8 AddImmToR4AX[] = { (Int8)0x04,(Int8)0x05,(Int8)0x05,(Int8)0x05 };

		static constexpr Int8 SubRToR[] = { (Int8)0x28,(Int8)0x29,(Int8)0x29,(Int8)0x29 };

		static constexpr Int8 SubImmToR[] = { (Int8)0x80,(Int8)0x81,(Int8)0x81,(Int8)0x81 };

		static constexpr Int8 SubImmToR4AX[] = { (Int8)0x2C,(Int8)0x2D,(Int8)0x2D,(Int8)0x2D };

		static constexpr Int8 MulRToR4AX[] = { (Int8)0xF6,(Int8)0xF7,(Int8)0xF7,(Int8)0xF7 };

		static constexpr Int16 MulRToR[] = { (Int16)0x0FAF,(Int16)0x0FAF,(Int16)0x0FAF,(Int16)0x0FAF };

		static constexpr Int8 MulImmToR[] = { (Int8)0x69,(Int8)0x69,(Int8)0x69,(Int8)0x69 };

		static constexpr Int8 DivRMToR[] = { (Int8)0xF6,(Int8)0xF7,(Int8)0xF7,(Int8)0xF7 };

		static constexpr Int8 CDQ = (Int8)0x99;

		static constexpr Int8 CmpImmWR4AX[] = { (Int8)0x3C,(Int8)0x3D,(Int8)0x3D,(Int8)0x3D };

		static constexpr Int8 CmpImmWR[] = { (Int8)0x80,(Int8)0x81,(Int8)0x81,(Int8)0x81 };

		static constexpr Int8 CmpMRWR[] = { (Int8)0x3A,(Int8)0x3B,(Int8)0x3B,(Int8)0x3B };

		static constexpr Int8 PopToM[] = { (Int8)0x8F,(Int8)0x8F,(Int8)0x8F,(Int8)0x8F };

		static constexpr Int8 PopToR[] = { (Int8)0x58,(Int8)0x58,(Int8)0x58,(Int8)0x58 };

		static constexpr Int8 PushM[] = { (Int8)0xFF,(Int8)0xFF,(Int8)0xFF,(Int8)0xFF };

		static constexpr Int8 PushR[] = { (Int8)0x50,(Int8)0x50,(Int8)0x50,(Int8)0x50 };

		static constexpr Int8 PushImm[] = { (Int8)0x6A,(Int8)0x68,(Int8)0x68,(Int8)0x68 };

		static constexpr Int8 Ret[] = { (Int8)0xC3,(Int8)0xCB };

		static constexpr Int8 JmpImmRel[] = { (Int8)0xEB,(Int8)0xE9,(Int8)0xE9 ,(Int8)0xEB };

		static constexpr Int8 JmpImmAbs[] = { (Int8)0xEA,(Int8)0xEA,(Int8)0xEA ,(Int8)0xEA };

		static constexpr Int8 JmpRMAbs[] = { (Int8)0xFF,(Int8)0xFF,(Int8)0xFF ,(Int8)0xFF };

		static constexpr Int16 JccImmRel[] = { (Int16)0x0F84,(Int16)0x0F85,(Int16)0x0F8F,
											   (Int16)0x0F8C,(Int16)0x0F8D,(Int16)0x0F8E };

		static constexpr Int8 CallImmRel[] = { (Int8)0xE8,(Int8)0xE8,(Int8)0xE8 ,(Int8)0xE8 };

		static constexpr Int8 CallRMAbs[] = { (Int8)0xFF,(Int8)0xFF,(Int8)0xFF ,(Int8)0xFF };

		static constexpr Int8 CallRMRel[] = { (Int8)0xE8,(Int8)0xE8,(Int8)0xE8 ,(Int8)0xE8 };

		static constexpr Int8 Pause = (Int8)0xCC;

		static constexpr Int8 LShiftBImm[] = { (Int8)0xC0,(Int8)0xC1, (Int8)0xC1, (Int8)0xC1 };

		static constexpr Int8 LShiftB1[] = { (Int8)0xD0,(Int8)0xD1, (Int8)0xD1, (Int8)0xD1 };

		static constexpr Int8 LShiftBR[] = { (Int8)0xD2,(Int8)0xD3, (Int8)0xD3, (Int8)0xD3 };

		static constexpr Int8 RShiftBImm[] = { (Int8)0xC0,(Int8)0xC1, (Int8)0xC1, (Int8)0xC1 };

		static constexpr Int8 RShiftB1[] = { (Int8)0xD0,(Int8)0xD1, (Int8)0xD1, (Int8)0xD1 };

		static constexpr Int8 RShiftBR[] = { (Int8)0xD2,(Int8)0xD3, (Int8)0xD3, (Int8)0xD3 };

		static constexpr Int8 OrR2R[] = { (Int8)0x08,(Int8)0x09, (Int8)0x09, (Int8)0x09 };

		static constexpr Int8 OrImm2R4AX[] = { (Int8)0x0C,(Int8)0x0D, (Int8)0x0D, (Int8)0x0D };

		static constexpr Int8 OrImm2R[] = { (Int8)0x80,(Int8)0x81, (Int8)0x81, (Int8)0x81 };

		static constexpr Int8 AndR2R[] = { (Int8)0x20,(Int8)0x21, (Int8)0x21, (Int8)0x21 };

		static constexpr Int8 AndImm2R4AX[] = { (Int8)0x24,(Int8)0x25, (Int8)0x25, (Int8)0x25 };

		static constexpr Int8 AndImm2R[] = { (Int8)0x80,(Int8)0x81, (Int8)0x81, (Int8)0x81 };

		static constexpr Int8 XorR2R[] = { (Int8)0x30,(Int8)0x31, (Int8)0x31, (Int8)0x31 };

		static constexpr Int8 XorImm2R4AX[] = { (Int8)0x34,(Int8)0x35, (Int8)0x35, (Int8)0x35 };

		static constexpr Int8 XorImm2R[] = { (Int8)0x80,(Int8)0x81, (Int8)0x81, (Int8)0x81 };

		static constexpr Int8 Not[] = { (Int8)0xF6,(Int8)0xF7, (Int8)0xF7, (Int8)0xF7 };
	};

	enum class AddressFixUpType :Int8
	{
		AbsoluteToRelative,
		RelativeToAbsolute
	};

	struct AddressFixUp
	{
		Int32 Index;
		SlotType Slot;
		AddressFixUpType Type;
		//To indicate the offset from operand to end of the opcode
		Int16 Offset;
	};


	class X86Emitter :public INativeEmitter
	{
		HL::System::Collection::Generic::List<AddressFixUp> m_fixups;

		/// <summary>
		/// Write by big endianness, mostly used for op
		/// </summary>
		/// <typeparam name="IntT"></typeparam>
		/// <param name="value"></param>
		template<class IntT>
		void Write(IntT value) {
			Binary::WriteByBigEndianness(
				GetExecutablePage()->PrepareWrite(sizeof(IntT)), value);
			GetExecutablePage()->CommitWrite(sizeof(IntT));
		}
		template<class IntT>
		void WriteByLittleEndianness(IntT value) {
			Binary::WriteByLittleEndianness(
				GetExecutablePage()->PrepareWrite(sizeof(IntT)), value);
			GetExecutablePage()->CommitWrite(sizeof(IntT));
		}

		void WriteImmediate(SlotType Slot, Int64 Imm);
		static void WriteImmediate(Int8* Target, SlotType Slot, Int64 Imm);
		static Int64 ReadAsImmediate(Int8* Target, SlotType Slot);
		void WriteREX(SlotType Slot);

		inline void NeedFixup(SlotType Slot, AddressFixUpType Type, Int16 Offset = 0);
		inline void NeedFixup(SlotType Slot, Int32 Index, AddressFixUpType Type, Int16 Offset = 0);
	public:
		virtual void StartEmitting();
		virtual void CommitEmitting();
		//Flow control fix up writing
		virtual void UpdateFlowControlTo(Int64 Imm, FlowControlReplaceEntry const& Entry);
		//Memory/Register operation

		virtual void EmitStoreImmediateToRegister(Int8 Register, Int64 Imm, SlotType Slot);
		virtual void EmitStoreImmediateToMemoryViaRegister(Int8 Register, Int64 Imm, SlotType Slot);
		virtual void EmitStoreImmediateToMemoryViaImmediate(Int64 AddressImm, Int64 Imm, SlotType Slot);
		virtual void EmitStoreRegisterToMemoryViaImmediate(Int8 Register, Int64 AddressImm, SlotType Slot);
		virtual void EmitStoreRegisterToMemoryViaRegister(Int8 SourceRegister, Int8 AddressRegister, SlotType Slot);

		virtual void EmitLoadRegisterToRegister(Int8 DestinationRegister, Int8 SourceRegister, SlotType Slot);
		virtual void EmitLoadMemoryToRegisterViaImmediate(Int8 Register, Int64 AddressImm, SlotType Slot);
		virtual void EmitLoadMemoryToRegisterViaRegister(Int8 DestinationRegister, Int8 SourceRegister, SlotType Slot);

		//Arithmetic operation

		virtual void EmitAddRegisterToRegister(Int8 DestinationRegister, Int8 SourceRegister, SlotType Slot, ArithmeticType);
		virtual void EmitAddImmediateToRegister(Int8 Register, Int64 Imm, SlotType Slot, ArithmeticType);

		virtual void EmitSubRegisterToRegister(Int8 DestinationRegister, Int8 SourceRegister, SlotType Slot, ArithmeticType);
		virtual void EmitSubImmediateToRegister(Int8 Register, Int64 Imm, SlotType Slot, ArithmeticType);

		virtual void EmitMulRegisterToRegister(Int8 DestinationRegister, Int8 SourceRegister, SlotType Slot, ArithmeticType);
		virtual void EmitMulImmediateToRegister(Int8 Register, Int64 Imm, SlotType Slot, ArithmeticType);

		virtual void EmitDivRegisterToRegister(Int8 DestinationRegister, Int8 SourceRegister, SlotType Slot, ArithmeticType Type);
		virtual void EmitDivImmediateToRegister(Int8 Register, Int64 Imm, SlotType Slot, ArithmeticType Type);

		virtual void EmitCompareRegisterWithMemoryViaImmediate(Int8 Register, Int64 Address, SlotType Slot);
		virtual void EmitCompareRegisterWithMemoryViaRegister(Int8 Register, Int8 AddressRegister, SlotType Slot);
		virtual void EmitCompareRegisterWithRegister(Int8 LeftRegister, Int8 RightRegister, SlotType Slot);
		virtual void EmitCompareRegisterWithImmediate(Int8 Register, Int64 Imm, SlotType Slot);

		//Stack operation

		virtual void EmitPopToRegister(Int8 Register, SlotType Slot);
		virtual void EmitPopToMemoryViaRegister(Int8 Register, SlotType Slot);
		virtual void EmitPopToMemoryViaImmediate(Int64 Imm, SlotType Slot);

		virtual void EmitPushRegister(Int8 Register, SlotType Slot);
		virtual void EmitPushMemoryViaRegister(Int8 Register, SlotType Slot);
		virtual void EmitPushMemoryViaImmediate(Int64 Imm, SlotType Slot);
		virtual void EmitPushImmediate(Int64 Imm, SlotType Slot);

		//Flow control

		virtual void EmitReturn();

		virtual FlowControlReplaceEntry EmitJmpViaImmediate(Int64 Imm, SlotType Slot, RedirectSemantic Sem);
		virtual void EmitJmpViaRegister(Int8 Register, SlotType Slot, RedirectSemantic Sem);

		virtual FlowControlReplaceEntry EmitJcc(Condition Cond, Int64 Imm, SlotType Slot, RedirectSemantic Sem);

		virtual void EmitCallViaRegister(Int8 Register, SlotType Slot, RedirectSemantic Sem);

		virtual void EmitCallViaImmediate(Int64 Imm, SlotType Slot, RedirectSemantic Sem);

		//Bit operation

		virtual void EmitLShiftRegisterByImmediateTimes(Int8 Register, Int64 Imm, SlotType Slot, ArithmeticType);
		virtual void EmitLShiftRegisterByRegisterTimes(Int8 Register, Int8 SourceRegister, SlotType Slot, ArithmeticType);

		virtual void EmitRShiftRegisterByImmediateTimes(Int8 Register, Int64 Imm, SlotType Slot, ArithmeticType);
		virtual void EmitRShiftRegisterByRegisterTimes(Int8 Register, Int8 SourceRegister, SlotType Slot, ArithmeticType);

		virtual void EmitOrRegisterToRegister(Int8 Register, Int8 SourceRegister, SlotType Slot);
		virtual void EmitOrImmediateToRegister(Int8 Register, Int64 Imm, SlotType Slot);

		virtual void EmitAndRegisterToRegister(Int8 Register, Int8 SourceRegister, SlotType Slot);
		virtual void EmitAndImmediateToRegister(Int8 Register, Int64 Imm, SlotType Slot);

		virtual void EmitXorRegisterToRegister(Int8 Register, Int8 SourceRegister, SlotType Slot);
		virtual void EmitXorImmediateToRegister(Int8 Register, Int64 Imm, SlotType Slot);

		virtual void EmitNotRegister(Int8 Register, SlotType Slot);
		virtual void EmitNotMemoryViaImmediate(Int64 Imm, SlotType Slot);
		virtual void EmitNotMemoryViaRegister(Int8 Register, SlotType Slot);
	};
}