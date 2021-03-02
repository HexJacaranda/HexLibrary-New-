#pragma once
#pragma warning(disable : 4200)
#include "..\..\..\RuntimeAlias.h"
#include "..\..\..\..\Include.h"

namespace RTIOS2EE
{
	class IExecutablePage;
}
namespace RTJE
{
	struct EmitContext;
}
namespace RTJE
{
	enum class SlotType :Int8
	{
		Int8,
		Int16,
		Int32,
		Int64,
		Float,
		Double
	};

	static constexpr Int32 SlotTypeToSize(SlotType Slot)
	{
		switch (Slot)
		{
		case SlotType::Int8:return sizeof(Int8);
		case SlotType::Int16:return sizeof(Int16); 
		case SlotType::Int32:
		case SlotType::Float:
			return sizeof(Int32);
		default:
		case SlotType::Int64:
		case SlotType::Double:
			return sizeof(Int64);		
		}
	}

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

	enum class EmitState
	{
		OK,
		NotSupported
	};

	/// <summary>
	/// Record the necessary and other emit state
	/// </summary>
	struct EmitContext
	{
		//This field may be used by some implementations to implicitly use extra register
		Int32 GeneralRegisterUsage = 0xFFFFFFFF;
		Int32 CommonFlags = 0;
		//This field is used to indicate the state of emitting
		Int32 EmitState = 0;
		//This field is used to extend the context
		Int8 Appendix[0];	
		//Get the first available register, -1 indicates there's none
		Int8 AvailableRegister()const {
			for (Int8 i = 0; i < sizeof(Int32) << 3; ++i)
				if ((GeneralRegisterUsage >> i) & 1)
					return i;
			return -1;
		}
		template<class...Args>
		inline void SetRegisterAvailable(Args&&...registers)
		{
			HL::System::Utility::Eval(SetRegisterAvailable(registers)...);
		}
		template<class...Args>
		inline void SetRegisterOccupied(Args&&...registers)
		{
			HL::System::Utility::Eval(SetRegisterOccupied(registers)...);
		}
		inline Int32 SetRegisterAvailable(Int8 Register) {
			return GeneralRegisterUsage |= (1 << Register);
		}
		inline Int32 SetRegisterOccupied(Int8 Register) {
			return GeneralRegisterUsage &= ~(1 << Register);
		}
	};

	/// <summary>
	/// For lazy replacement on address in flow control instructions.
	/// </summary>
	struct FlowControlReplaceEntry
	{
		Int8* Address;
		SlotType AllowedSlot;
	};

	class INativeEmitter
	{
	protected:
		RTIOS2EE::IExecutablePage* mPage = nullptr;
		EmitContext* mContext = nullptr;
	public:
		virtual void StartEmitting() = 0;
		virtual void CommitEmitting() = 0;

		virtual void SetEmitContext(EmitContext* value) {
			mContext = value;
		}
		virtual EmitContext* GetEmitContext();
		virtual void SetExecutablePage(RTIOS2EE::IExecutablePage* value);
		virtual RTIOS2EE::IExecutablePage* GetExecutablePage(); 
		virtual Int32 CurrentPosition();
		virtual Int8* CurrentPointer();
		//Flow control fix up writing
		virtual void UpdateFlowControlTo(Int64 Imm, FlowControlReplaceEntry const& Entry) = 0;
		//Memory/Register operation

		virtual void EmitStoreImmediateToRegister(Int8 Register, Int64 Imm, SlotType) = 0;
		virtual void EmitStoreImmediateToMemoryViaRegister(Int8 Register, Int64 Imm, SlotType) = 0;
		virtual void EmitStoreImmediateToMemoryViaImmediate(Int64 AddressImm, Int64 Imm, SlotType) = 0;
		virtual void EmitStoreRegisterToMemoryViaImmediate(Int8 Register, Int64 AddressImm, SlotType) = 0;
		virtual void EmitStoreRegisterToMemoryViaRegister(Int8 SourceRegister, Int8 AddressRegister, SlotType) = 0;

		virtual void EmitLoadRegisterToRegister(Int8 DestinationRegister, Int8 SourceRegister, SlotType) = 0;
		virtual void EmitLoadMemoryToRegisterViaImmediate(Int8 Register, Int64 AddressImm, SlotType) = 0;
		virtual void EmitLoadMemoryToRegisterViaRegister(Int8 DestinationRegister, Int8 SourceRegister, SlotType) = 0;

		//Arithmetic operation

		virtual void EmitAddRegisterToRegister(Int8 DestinationRegister, Int8 SourceRegister, SlotType, ArithmeticType) = 0;
		virtual void EmitAddImmediateToRegister(Int8 Register, Int64 Imm, SlotType, ArithmeticType) = 0;

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

		//Stack operation

		virtual void EmitPopToRegister(Int8 Rgister, SlotType Slot) = 0;
		virtual void EmitPopToMemoryViaRegister(Int8 Rgister, SlotType Slot) = 0;
		virtual void EmitPopToMemoryViaImmediate(Int64 Imm, SlotType Slot) = 0;

		virtual void EmitPushRegister(Int8 Register, SlotType Slot) = 0;
		virtual void EmitPushImmediate(Int64 Imm, SlotType Slot) = 0;
		virtual void EmitPushMemoryViaRegister(Int8 Register, SlotType Slot) = 0;
		virtual void EmitPushMemoryViaImmediate(Int64 Imm, SlotType Slot) = 0;

		//Flow control

		virtual void EmitReturn() = 0;
		virtual FlowControlReplaceEntry EmitJmpViaImmediate(Int64 Imm, SlotType, RedirectSemantic) = 0;
		virtual void EmitJmpViaRegister(Int8 Register, SlotType, RedirectSemantic) = 0;
		virtual FlowControlReplaceEntry EmitJcc(Condition, Int64 Imm, SlotType, RedirectSemantic) = 0;

		virtual void EmitCallViaRegister(Int8 Register, SlotType, RedirectSemantic) = 0;
		virtual void EmitCallViaImmediate(Int64 Imm, SlotType, RedirectSemantic) = 0;

		//Bit operation

		virtual void EmitLShiftRegisterByImmediateTimes(Int8 Register, Int64 Imm, SlotType, ArithmeticType) = 0;
		virtual void EmitLShiftRegisterByRegisterTimes(Int8 Register, Int8 SourceRegister, SlotType, ArithmeticType) = 0;

		virtual void EmitRShiftRegisterByImmediateTimes(Int8 Register, Int64 Imm, SlotType, ArithmeticType) = 0;
		virtual void EmitRShiftRegisterByRegisterTimes(Int8 Register, Int8 SourceRegister, SlotType, ArithmeticType) = 0;

		virtual void EmitOrRegisterToRegister(Int8 Register, Int8 SourceRegister, SlotType) = 0;
		virtual void EmitOrImmediateToRegister(Int8 Register, Int64 Imm, SlotType) = 0;

		virtual void EmitAndRegisterToRegister(Int8 Register, Int8 SourceRegister, SlotType) = 0;
		virtual void EmitAndImmediateToRegister(Int8 Register, Int64 Imm, SlotType) = 0;

		virtual void EmitXorRegisterToRegister(Int8 Register, Int8 SourceRegister, SlotType) = 0;
		virtual void EmitXorImmediateToRegister(Int8 Register, Int64 Imm, SlotType) = 0;

		virtual void EmitNotRegister(Int8 Register, SlotType) = 0;
		virtual void EmitNotMemoryViaImmediate(Int64 Imm, SlotType) = 0;
		virtual void EmitNotMemoryViaRegister(Int8 Register, SlotType) = 0;

		virtual ~INativeEmitter(){}
	};
}