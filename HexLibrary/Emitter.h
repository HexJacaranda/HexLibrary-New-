#pragma once
namespace HL::System::Runtime::JIT::Emit
{
	class EmitException :public Exception::IException
	{
	public:
		EmitException(Text::WString const& text) :Exception::IException() {}
	};

	enum class SlotType
	{
		Int8,
		Int16,
		Int32,
		Int64,
		Float,
		Double
	};

	enum class ArithmeticType
	{
		Signed,
		Unsigned
	};

	enum class Condition
	{
		EqualTo,
		NotEqualTo,
		GreaterThan,
		LessThan,
		GreaterThanOrEqualTo,
		LessThanOrEqualTo
	};

	enum class RedirectSemantic
	{
		Relative,
		Absolute
	};

	static inline constexpr Int64 FloatAsImmediate(float target) {
		return (Int64)(*(Int32*)&target);
	}
	static inline constexpr Int64 FloatAsImmediate(double target) {
		return (*(Int64*)&target);
	}

	struct EmitContext
	{
		//This field may be used by some implementations to implicitly use extra register
		Int32 GeneralRegisterUsage;
		Int32 CommonFlags;
		//This field is used to indicate the state of emitting
		Int32 EmitState;
		//This field is used to extend the context
		Int8 Appendix[0];	
		//Get the first available register, -1 indicates there's none
		Int8 AvailableRegister()const {
			for (Int8 i = 0; i < sizeof(Int32) << 3; ++i)
				if ((GeneralRegisterUsage >> i) & 1)
					return i;
			return -1;
		}
	};



	class INativeEmitter
	{
	protected:
		Pointer::ptr<Interfaces::OSToEE::IExecutablePage> m_page;
		EmitContext* m_context;
	public:
		virtual void SetEmitContext(EmitContext* value) {
			m_context = value;
		}
		virtual EmitContext* GetEmitContext() {
			return m_context;
		}
		virtual void SetExecutablePage(Pointer::ptr<Interfaces::OSToEE::IExecutablePage> const& value) {
			m_page = value;
		}
		virtual Pointer::ptr<Interfaces::OSToEE::IExecutablePage> const& GetExecutablePage() {
			return m_page;
		}

		virtual void EmitStoreImmediateToRegister(Int8 Register, Int64 Imm, SlotType) = 0;
		virtual void EmitStoreImmediateToMemoryViaRegister(Int8 Register, Int64 Imm, SlotType) = 0;
		virtual void EmitStoreImmediateToMemoryViaImmediate(Int64 AddressImm, Int64 Imm, SlotType) = 0;
		virtual void EmitStoreRegisterToMemoryViaImmediate(Int8 Register, Int64 AddressImm, SlotType) = 0;
		virtual void EmitStoreRegisterToMemoryViaRegister(Int8 SourceRegister, Int8 AddressRegister, SlotType) = 0;

		virtual void EmitLoadRegisterToRegister(Int8 DestinationRegister, Int8 SourceRegister, SlotType) = 0;
		virtual void EmitLoadMemoryToRegisterViaImmediate(Int8 Register, Int64 AddressImm, SlotType) = 0;
		virtual void EmitLoadMemoryToRegisterViaRegister(Int8 DestinationRegister, Int8 SourceRegister, SlotType) = 0;

		virtual void EmitAddRegisterToRegister(Int8 DestinationRegister, Int8 SourceRegister, SlotType, ArithmeticType) = 0;
		virtual void EmitAddImmediateToRegister(Int8 Register, Int64 Imm, ArithmeticType) = 0;

		virtual void EmitSubRegisterToRegister(Int8 DestinationRegister, Int8 SourceRegister, SlotType, ArithmeticType) = 0;
		virtual void EmitSubImmediateToRegister(Int8 Register, Int64 Imm, SlotType, ArithmeticType) = 0;

		virtual void EmitMulRegisterToRegister(Int8 DestinationRegister, Int8 SourceRegister, SlotType, ArithmeticType) = 0;
		virtual void EmitMulImmediateToRegister(Int8 Register, Int64 Imm, SlotType, ArithmeticType) = 0;
		
		virtual void EmitDivRegisterToRegister(Int8 DestinationRegister, Int8 SourceRegister, SlotType, ArithmeticType) = 0;
		virtual void EmitDivImmediateToRegister(Int8 Register, Int64 Imm, SlotType, ArithmeticType) = 0;

		virtual void EmitCompareRegisterWithMemoryViaImmediate(Int8 Register, Int64 Address, SlotType Slot) = 0;
		virtual void EmitCompareRegisterWithMemoryViaRegister(Int8 Register, Int8 AddressRegister, SlotType Slot) = 0;
		virtual void EmitCompareRegisterWithRegister(Int8 LeftRegister, Int8 RightRegister, SlotType) = 0;
		virtual void EmitCompareRegisterWithImmediate(Int8 Register, Int64 Imm, SlotType) = 0;

		virtual void EmitPopToRegister(Int8 Rgister, SlotType Slot) = 0;
		virtual void EmitPopToMemoryViaRegister(Int8 Rgister, SlotType Slot) = 0;
		virtual void EmitPopToMemoryViaImmediate(Int64 Imm, SlotType Slot) = 0;

		virtual void EmitPushRegister(Int8 Register, SlotType Slot) = 0;
		virtual void EmitPushImmediate(Int64 Imm, SlotType Slot) = 0;
		virtual void EmitPushMemoryViaRegister(Int8 Register, SlotType Slot) = 0;
		virtual void EmitPushMemoryViaImmediate(Int64 Imm, SlotType Slot) = 0;

		virtual void EmitReturn() = 0;
		virtual void EmitJmpViaImmediate(Int64 Imm, SlotType, RedirectSemantic) = 0;
		virtual void EmitJmpViaRegister(Int8 Register, SlotType, RedirectSemantic) = 0;
		virtual void EmitJcc(Condition, Int64 Imm, SlotType, RedirectSemantic) = 0;

		virtual void EmitCallViaRegister(Int8 Register, SlotType, RedirectSemantic) = 0;
		virtual void EmitCallViaImmediate(Int64 Imm, SlotType, RedirectSemantic) = 0;
	};
}