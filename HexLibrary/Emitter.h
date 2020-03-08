#pragma once
namespace HL::System::Runtime::JIT::Emit
{
	using Int8 = __int8;
	using Int16 = __int16;
	using Int32 = __int32;
	using Int64 = __int64;

	enum class SlotType
	{
		Int8,
		Int16,
		Int32,
		Int64,
		Float,
		Double
	};

	enum class Condition
	{
		EqualTo,
		GreaterThan,
		LeassThan,
		GreaterThanOrEqualTo,
		LessThanOrEqualTo
	};

	static inline constexpr Int64 FloatAsImmediate(float target) {
		return (Int64)(*(Int32*)&target);
	}
	static inline constexpr Int64 FloatAsImmediate(double target) {
		return (*(Int64*)&target);
	}

	class INativeEmitter
	{
	public:
		virtual void EmitStoreImmediateToRegister(Int8 Register, SlotType, Int64 Imm) = 0;
		virtual void EmitStoreImmediateToMemoryViaRegister(Int8 Register, SlotType, Int64 Imm) = 0;
		virtual void EmitStoreImmediateToMemoryViaImmediate(Int64 AddressImm, SlotType, Int64 Imm) = 0;
		virtual void EmitStoreRegisterToMemoryViaImmediate(Int8 Register, SlotType Slot, Int64 AddressImm) = 0;
		virtual void EmitStoreRegisterToMemoryViaRegister(Int8 SourceRegister, SlotType Slot, Int8 AddressRegister) = 0;

		virtual void EmitLoadRegisterToRegister(Int8 DestinationRegister, Int8 SourceRegister, SlotType) = 0;
		virtual void EmitLoadMemoryToRegisterViaImmediate(Int8 Register, SlotType, Int64 AddressImm) = 0;
		virtual void EmitLoadMemoryToRegisterViaRegister(Int8 DestinationRegister, SlotType, Int8 SourceRegister) = 0;

		virtual void EmitAddRegisterToRegister(Int8 DestinationRegister, Int8 SourceRegister, SlotType) = 0;
		virtual void EmitAddImmediateToRegister(Int8 Register, SlotType, Int64 Imm) = 0;

		virtual void EmitSubRegisterToRegister(Int8 DestinationRegister, Int8 SourceRegister, SlotType) = 0;
		virtual void EmitSubImmediateToRegister(Int8 Register, SlotType, Int64 Imm, SlotType) = 0;

		virtual void EmitMulRegisterToRegister(Int8 DestinationRegister, Int8 SourceRegister, SlotType) = 0;
		virtual void EmitMulImmediateToRegister(Int8 Register, SlotType, Int64 Imm, SlotType) = 0;
		
		virtual void EmitDivRegisterToRegister(Int8 DestinationRegister, Int8 SourceRegister, SlotType) = 0;
		virtual void EmitDivImmediateToRegister(Int8 Register, SlotType, Int64 Imm, SlotType) = 0;

		virtual void EmitCompareRegisterWithMemory(Int8 Register, Int64 Address, SlotType) = 0;
		virtual void EmitCompareRegisterWithRegister(Int8 LeftRegister, Int8 RightRegister) = 0;
		virtual void EmitCompareRegisterWithImmediate(Int8 Register, Int64 Imm, SlotType) = 0;

		virtual void EmitPop() = 0;
		virtual void EmitReturn() = 0;
		virtual void EmitJmpViaImmediate(Int64 Imm, SlotType) = 0;
		virtual void EmitJmpViaRegister(Int8 Register, SlotType) = 0;
		virtual void EmitJcc(Condition, Int64 Imm, SlotType) = 0;
	};
}