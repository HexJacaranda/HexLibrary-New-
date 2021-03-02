#include "X86Emitter.h"
#include ".\Page\ExecutablePage.h"
#include "..\..\..\Binary.h"

void RTJE::X86::X86Emitter::WriteImmediate(SlotType Slot, Int64 Imm)
{
	switch (Slot)
	{
	case SlotType::Int8:WriteByLittleEndianness((Int8)Imm); break;
	case SlotType::Int16:WriteByLittleEndianness((Int16)Imm); break;
	case SlotType::Int32:
	case SlotType::Float:
		WriteByLittleEndianness((Int32)Imm); break;
	case SlotType::Int64:
	case SlotType::Double:
		WriteByLittleEndianness((Int64)Imm); break;
	}
}

void RTJE::X86::X86Emitter::WriteImmediate(Int8* Target, SlotType Slot, Int64 Imm)
{
	switch (Slot)
	{
	case SlotType::Int8:
		Binary::WriteByLittleEndianness(
		Target, (Int8)Imm); break;
	case SlotType::Int16:Binary::WriteByLittleEndianness(
		Target, (Int16)Imm); break;
	case SlotType::Int32:
	case SlotType::Float:
		Binary::WriteByLittleEndianness(
			Target, (Int32)Imm); break;
	case SlotType::Int64:
	case SlotType::Double:
		Binary::WriteByLittleEndianness(
			Target, (Int64)Imm); break;
	}
}

RT::Int64 RTJE::X86::X86Emitter::ReadAsImmediate(Int8* Target, SlotType Slot)
{
	switch (Slot)
	{
	case SlotType::Int8:return *Target;
	case SlotType::Int16:return *(Int16*)Target;
	case SlotType::Int32:
	case SlotType::Float:
		return *(Int32*)Target;
	case SlotType::Int64:
	case SlotType::Double:
		return *(Int64*)Target;
	}
	RTE::Throw(Text("Invalid slot type when reading immediate!"));
	return *Target;
}

void RTJE::X86::X86Emitter::WriteREX(SlotType Slot)
{
	Int8 REX = X86OpCodes::REX[(int)Slot];
	if (REX != 0)
		Write(REX);
}

inline void RTJE::X86::X86Emitter::NeedFixup(SlotType Slot, AddressFixUpType Type, Int16 Offset)
{
	m_fixups.Add({ GetExecutablePage()->Current(),Slot,Type,Offset });
}

inline void RTJE::X86::X86Emitter::NeedFixup(SlotType Slot, Int32 Index, AddressFixUpType Type, Int16 Offset)
{
	m_fixups.Add({ Index,Slot,Type,Offset });
}

void RTJE::X86::X86Emitter::StartEmitting()
{
}

void RTJE::X86::X86Emitter::CommitEmitting()
{
	Int8* base = GetExecutablePage()->GetRawPage();
	//Fix up work
	{
		for (auto&& fixup : m_fixups)
		{
			Int64 value = ReadAsImmediate(base + fixup.Index, fixup.Slot);
			if (fixup.Type == AddressFixUpType::AbsoluteToRelative)
				value = value - ((Int64)base + (Int64)fixup.Index + (Int64)fixup.Offset);
			else if (fixup.Type == AddressFixUpType::RelativeToAbsolute)
				value = value + ((Int64)base + (Int64)fixup.Index + (Int64)fixup.Offset);
			WriteImmediate(base + fixup.Index, fixup.Slot, value);
		}
	}

	//Fill the remain blank with int3
	{
		Int32 index = GetExecutablePage()->Current();
		Int32 length = GetExecutablePage()->Length();
		for (; index < length; ++index)
			base[index] = X86OpCodes::Pause;
	}
}

void RTJE::X86::X86Emitter::UpdateFlowControlTo(Int64 Imm, FlowControlReplaceEntry const& Entry)
{
	WriteImmediate(Entry.Address, Entry.AllowedSlot, Imm);
}

void RTJE::X86::X86Emitter::EmitStoreImmediateToRegister(Int8 Register, Int64 Imm, SlotType Slot)
{
	WriteREX(Slot);
	Write((Int8)(X86OpCodes::MovImmToR[(int)Slot] + Register));
	WriteImmediate(Slot, Imm);
}

void RTJE::X86::X86Emitter::EmitStoreImmediateToMemoryViaRegister(Int8 Register, Int64 Imm, SlotType Slot)
{
	WriteREX(Slot);
	if (Slot == SlotType::Int64)
	{
		GetEmitContext()->SetRegisterOccupied(Register);
		Int8 ephemeralRegister = GetEmitContext()->AvailableRegister();
		if (ephemeralRegister == -1)
			RTE::Throw(Text("All registers unavailable"));
		EmitStoreImmediateToRegister(ephemeralRegister, Imm, Slot);
		EmitStoreRegisterToMemoryViaRegister(ephemeralRegister, Register, Slot);
		GetEmitContext()->SetRegisterAvailable(Register);
	}
	else
	{
		Write(X86OpCodes::MovImmToM[(int)Slot]);
		Write(ModRM(AddressingMode::RegisterAddressing, 0b000, Register));
		WriteImmediate(Slot, Imm);
	}
}

void RTJE::X86::X86Emitter::EmitStoreImmediateToMemoryViaImmediate(Int64 AddressImm, Int64 Imm, SlotType Slot)
{
	WriteREX(Slot);
	if (Slot == SlotType::Int64)
	{
		Int8 firstEphemeralRegister = GetEmitContext()->AvailableRegister();
		if (firstEphemeralRegister == -1)
			RTE::Throw(Text("All registers unavailable"));
		GetEmitContext()->SetRegisterOccupied(firstEphemeralRegister);
		Int8 secondEphemeralRegister = GetEmitContext()->AvailableRegister();
		if (secondEphemeralRegister == -1)
			RTE::Throw(Text("All registers unavailable"));
		GetEmitContext()->SetRegisterOccupied(secondEphemeralRegister);

		EmitStoreImmediateToRegister(firstEphemeralRegister, Imm, Slot);
		EmitStoreImmediateToRegister(secondEphemeralRegister, AddressImm, Slot);

		EmitStoreRegisterToMemoryViaRegister(firstEphemeralRegister, secondEphemeralRegister, Slot);

		GetEmitContext()->SetRegisterAvailable(firstEphemeralRegister);
		GetEmitContext()->SetRegisterAvailable(secondEphemeralRegister);
	}
	else
	{
		Write(X86OpCodes::MovImmToM[(int)Slot]);
		Write(ModRM(AddressingMode::RegisterAddressing, 0b000, RegisterOrMemory::Offset32bit));
		WriteImmediate(Slot, AddressImm);
		WriteImmediate(Slot, Imm);
	}
}

void RTJE::X86::X86Emitter::EmitStoreRegisterToMemoryViaImmediate(Int8 Register, Int64 AddressImm, SlotType Slot)
{
	if (Register != Register::AX)
		EmitLoadRegisterToRegister(Register::AX, Register, Slot);
	Write(X86OpCodes::MovRToMVImm[(int)Slot]);
	WriteImmediate(Slot, AddressImm);
}

void RTJE::X86::X86Emitter::EmitStoreRegisterToMemoryViaRegister(Int8 SourceRegister, Int8 AddressRegister, SlotType Slot)
{
	WriteREX(Slot);
	Write(X86OpCodes::MovRToMVR[(int)Slot]);
	Write(ModRM(AddressingMode::RegisterAddressing, SourceRegister, AddressRegister));
}

void RTJE::X86::X86Emitter::EmitLoadRegisterToRegister(Int8 DestinationRegister, Int8 SourceRegister, SlotType Slot)
{
	WriteREX(Slot);
	Write(X86OpCodes::MovRToR[(int)Slot]);
	Write(ModRM(AddressingMode::Register, SourceRegister, DestinationRegister));
}

void RTJE::X86::X86Emitter::EmitLoadMemoryToRegisterViaImmediate(Int8 Register, Int64 AddressImm, SlotType Slot)
{
	Write(X86OpCodes::MovMToRVImm[(int)Slot]);
	WriteImmediate(Slot, AddressImm);
	if (Register != Register::AX)
		EmitLoadRegisterToRegister(Register, Register::AX, Slot);
}

void RTJE::X86::X86Emitter::EmitLoadMemoryToRegisterViaRegister(Int8 DestinationRegister, Int8 SourceRegister, SlotType Slot)
{
	WriteREX(Slot);
	Write(X86OpCodes::MovMToRVR[(int)Slot]);
	Write(ModRM(AddressingMode::RegisterAddressing, DestinationRegister, SourceRegister));
}

void RTJE::X86::X86Emitter::EmitAddRegisterToRegister(Int8 DestinationRegister, Int8 SourceRegister, SlotType Slot, ArithmeticType)
{
	WriteREX(Slot);
	Write(X86OpCodes::AddRToR[(int)Slot]);
	Write(ModRM(AddressingMode::Register, SourceRegister, DestinationRegister));
}

void RTJE::X86::X86Emitter::EmitAddImmediateToRegister(Int8 Register, Int64 Imm, SlotType Slot, ArithmeticType)
{
	WriteREX(Slot);
	if (Register == Register::AX)
		Write(X86OpCodes::AddImmToR4AX[(int)Slot]);
	else
	{
		Write(X86OpCodes::AddImmToR[(int)Slot]);
		Write(ModRM(AddressingMode::Register, 0b000, Register));
	}
	WriteImmediate(Slot, Imm);
}

void RTJE::X86::X86Emitter::EmitSubRegisterToRegister(Int8 DestinationRegister, Int8 SourceRegister, SlotType Slot, ArithmeticType)
{
	WriteREX(Slot);
	Write(X86OpCodes::SubRToR[(int)Slot]);
	Write(ModRM(AddressingMode::Register, SourceRegister, DestinationRegister));
}

void RTJE::X86::X86Emitter::EmitSubImmediateToRegister(Int8 Register, Int64 Imm, SlotType Slot, ArithmeticType)
{
	WriteREX(Slot);
	if (Register == Register::AX)
		Write(X86OpCodes::SubImmToR4AX[(int)Slot]);
	else
	{
		Write(X86OpCodes::SubImmToR[(int)Slot]);
		Write(ModRM(AddressingMode::Register, 0b101, Register));
	}
	WriteImmediate(Slot, Imm);
}

void RTJE::X86::X86Emitter::EmitMulRegisterToRegister(Int8 DestinationRegister, Int8 SourceRegister, SlotType Slot, ArithmeticType)
{
	WriteREX(Slot);
	if (DestinationRegister == Register::AX)
	{
		Write(X86OpCodes::MulRToR4AX[(int)Slot]);
		Write(ModRM(AddressingMode::Register, 0b101, SourceRegister));
	}
	else
	{
		Write(X86OpCodes::MulRToR[(int)Slot]);
		Write(ModRM(AddressingMode::Register, DestinationRegister, SourceRegister));
	}
}

void RTJE::X86::X86Emitter::EmitMulImmediateToRegister(Int8 Register, Int64 Imm, SlotType Slot, ArithmeticType)
{
	WriteREX(Slot);
	Write(X86OpCodes::MulImmToR[(int)Slot]);
	Write(ModRM(AddressingMode::Register, Register, Register));
	if (Slot == SlotType::Int64)
		WriteImmediate(SlotType::Int32, Imm);
	else if (Slot == SlotType::Int8)
		WriteImmediate(SlotType::Int16, Imm);
	else
		WriteImmediate(Slot, Imm);
}

void RTJE::X86::X86Emitter::EmitDivRegisterToRegister(Int8 DestinationRegister, Int8 SourceRegister, SlotType Slot, ArithmeticType Type)
{
	GetEmitContext()->SetRegisterOccupied(Register::AX, DestinationRegister);

	if (DestinationRegister != Register::AX)
	{
		if (SourceRegister == Register::AX)
		{
			Int8 ephemeralRegister = GetEmitContext()->AvailableRegister();
			if (ephemeralRegister == -1)
				RTE::Throw(Text("All registers unavailable"));
			EmitLoadRegisterToRegister(ephemeralRegister, Register::AX, Slot);
			SourceRegister = ephemeralRegister;
		}
		EmitLoadRegisterToRegister(Register::AX, DestinationRegister, Slot);
	}

	WriteREX(Slot);
	Write(X86OpCodes::CDQ);
	WriteREX(Slot);
	Write(X86OpCodes::DivRMToR[(int)Slot]);
	if (Type == ArithmeticType::Signed)
		Write(ModRM(AddressingMode::Register, 0b111, SourceRegister));
	else if (Type == ArithmeticType::Unsigned)
		Write(ModRM(AddressingMode::Register, 0b110, SourceRegister));
	if (DestinationRegister != Register::AX)
		EmitLoadRegisterToRegister(DestinationRegister, Register::AX, Slot);

	GetEmitContext()->SetRegisterAvailable(Register::AX, DestinationRegister);
}

void RTJE::X86::X86Emitter::EmitDivImmediateToRegister(Int8 Register, Int64 Imm, SlotType Slot, ArithmeticType Type)
{
	GetEmitContext()->SetRegisterOccupied(Register::AX);
	GetEmitContext()->SetRegisterOccupied(Register);

	Int8 ephemeralRegister = GetEmitContext()->AvailableRegister();
	if (ephemeralRegister == -1)
		RTE::Throw(Text("All registers unavailable"));
	EmitStoreImmediateToRegister(ephemeralRegister, Imm, Slot);

	if (Register != Register::AX)
		EmitLoadRegisterToRegister(Register::AX, Register, Slot);
	WriteREX(Slot);
	Write(X86OpCodes::CDQ);
	WriteREX(Slot);
	Write(X86OpCodes::DivRMToR[(int)Slot]);
	if (Type == ArithmeticType::Signed)
		Write(ModRM(AddressingMode::Register, 0b111, ephemeralRegister));
	else if (Type == ArithmeticType::Unsigned)
		Write(ModRM(AddressingMode::Register, 0b110, ephemeralRegister));

	if (Register != Register::AX)
		EmitLoadRegisterToRegister(Register, Register::AX, Slot);

	GetEmitContext()->SetRegisterAvailable(Register::AX);
	GetEmitContext()->SetRegisterAvailable(Register);
}

void RTJE::X86::X86Emitter::EmitCompareRegisterWithMemoryViaImmediate(Int8 Register, Int64 Address, SlotType Slot)
{
	WriteREX(Slot);
	Write(X86OpCodes::CmpMRWR[(int)Slot]);
	Write(ModRM(AddressingMode::RegisterAddressing, Register, RegisterOrMemory::Offset32bit));
	WriteImmediate(Slot, Address);
}

void RTJE::X86::X86Emitter::EmitCompareRegisterWithMemoryViaRegister(Int8 Register, Int8 AddressRegister, SlotType Slot)
{
	WriteREX(Slot);
	Write(X86OpCodes::CmpMRWR[(int)Slot]);
	Write(ModRM(AddressingMode::RegisterAddressing, Register, AddressRegister));
}

void RTJE::X86::X86Emitter::EmitCompareRegisterWithRegister(Int8 LeftRegister, Int8 RightRegister, SlotType Slot)
{
	WriteREX(Slot);
	Write(X86OpCodes::CmpMRWR[(int)Slot]);
	Write(ModRM(AddressingMode::Register, LeftRegister, RightRegister));
}

void RTJE::X86::X86Emitter::EmitCompareRegisterWithImmediate(Int8 Register, Int64 Imm, SlotType Slot)
{
	WriteREX(Slot);
	if (Register == Register::AX)
		Write(X86OpCodes::CmpImmWR4AX[(int)Slot]);
	else
	{
		Write(X86OpCodes::CmpImmWR[(int)Slot]);
		Write(ModRM(AddressingMode::Register, 0b111, Register));
	}
	if (Slot == SlotType::Int64)
		Slot = SlotType::Int32;
	WriteImmediate(Slot, Imm);
}

void RTJE::X86::X86Emitter::EmitPopToRegister(Int8 Register, SlotType Slot)
{
	if (Slot == SlotType::Int8)
	{
		Slot = SlotType::Int16;
		Context::OperandPromote(GetEmitContext(), Slot);
	}		
	Write((Int8)(X86OpCodes::PopToR[(int)Slot] + Register));
}

void RTJE::X86::X86Emitter::EmitPopToMemoryViaRegister(Int8 Register, SlotType Slot)
{
	if (Slot == SlotType::Int8)
	{
		Slot = SlotType::Int16;
		Context::OperandPromote(GetEmitContext(), Slot);
	}
	Write(X86OpCodes::PopToM[(int)Slot]);
	Write(ModRM(AddressingMode::RegisterAddressing, 0b000, Register));
}

void RTJE::X86::X86Emitter::EmitPopToMemoryViaImmediate(Int64 Imm, SlotType Slot)
{
	if (Slot == SlotType::Int8)
	{
		Slot = SlotType::Int16;
		Context::OperandPromote(GetEmitContext(), Slot);
	}
	
	if (Slot == SlotType::Int64)
	{
		Int8 ephemeralRegister = GetEmitContext()->AvailableRegister();
		if (ephemeralRegister == -1)
			RTE::Throw(Text("All registers unavailable"));
		EmitStoreImmediateToRegister(ephemeralRegister, Imm, Slot);
		Write(X86OpCodes::PopToM[(int)Slot]);
		Write(ModRM(AddressingMode::RegisterAddressing, 0b000, ephemeralRegister));
	}
	else
	{
		Write(X86OpCodes::PopToM[(int)Slot]);
		Write(ModRM(AddressingMode::RegisterAddressing, 0b000, RegisterOrMemory::Offset32bit));
		WriteImmediate(Slot, Imm);
	}
}

void RTJE::X86::X86Emitter::EmitPushRegister(Int8 Register, SlotType Slot)
{
	if (Slot == SlotType::Int8)
	{
		Slot = SlotType::Int16;
		Context::OperandPromote(GetEmitContext(), Slot);
	}
	if (Slot == SlotType::Int16)
		WriteREX(Slot);
	Write((Int8)(X86OpCodes::PushR[(int)Slot] + Register));
}

void RTJE::X86::X86Emitter::EmitPushMemoryViaRegister(Int8 Register, SlotType Slot)
{
	if (Slot == SlotType::Int8)
	{
		Slot = SlotType::Int16;
		Context::OperandPromote(GetEmitContext(), Slot);
	}
	if (Slot == SlotType::Int16)
		WriteREX(Slot);
	Write(X86OpCodes::PushM[(int)Slot]);
	Write(ModRM(AddressingMode::RegisterAddressing, 0b110, Register));
}

void RTJE::X86::X86Emitter::EmitPushMemoryViaImmediate(Int64 Imm, SlotType Slot)
{
	if (Slot == SlotType::Int8)
	{
		Slot = SlotType::Int16;
		Context::OperandPromote(GetEmitContext(), Slot);
	}
	if (Slot == SlotType::Int16)
		WriteREX(Slot);
	if (Slot == SlotType::Int64)
	{
		Int8 ephemeralRegister = GetEmitContext()->AvailableRegister();
		if (ephemeralRegister == -1)
			RTE::Throw(Text("All registers unavailable"));
		EmitStoreImmediateToRegister(ephemeralRegister, Imm, Slot);
		Write(X86OpCodes::PushM[(int)Slot]);
		Write(ModRM(AddressingMode::RegisterAddressing, 0b110, ephemeralRegister));
	}
	else
	{
		Write(X86OpCodes::PushM[(int)Slot]);
		Write(ModRM(AddressingMode::RegisterAddressing, 0b110, RegisterOrMemory::Offset32bit));
		WriteImmediate(Slot, Imm);
	}
}

void RTJE::X86::X86Emitter::EmitPushImmediate(Int64 Imm, SlotType Slot)
{
	if (Slot == SlotType::Int16)
	{
		Slot = SlotType::Int32;
		Context::OperandPromote(GetEmitContext(), Slot);
	}
	if (Slot == SlotType::Int64)
	{
		Int8 ephemeralRegister = GetEmitContext()->AvailableRegister();
		if (ephemeralRegister == -1)
			RTE::Throw(Text("All registers unavailable"));
		EmitStoreImmediateToRegister(ephemeralRegister, Imm, Slot);
		EmitPushRegister(ephemeralRegister, Slot);
	}
	else
	{
		Write(X86OpCodes::PushImm[(int)Slot]);
		WriteImmediate(Slot, Imm);
	}
}

void RTJE::X86::X86Emitter::EmitReturn()
{
	Write(X86OpCodes::Ret[0]);
}

RTJE::FlowControlReplaceEntry RTJE::X86::X86Emitter::EmitJmpViaImmediate(Int64 Imm, SlotType Slot, RedirectSemantic Sem)
{
	if (Sem == RedirectSemantic::Absolute && Slot == SlotType::Int8)
	{
		Slot = SlotType::Int16;
		Context::OperandPromote(GetEmitContext(), Slot);
	}
	else if (Slot == SlotType::Int64)
	{
		Slot = SlotType::Int32;
		Context::OperandSlice(GetEmitContext(), Slot);
	}
	const Int8* OpCodes =
		Sem == RedirectSemantic::Relative ? X86OpCodes::JmpImmRel : X86OpCodes::JmpImmAbs;
	Write(OpCodes[(int)Slot]);
	Int8* address = CurrentPointer();
	WriteImmediate(Slot, Imm);
	return FlowControlReplaceEntry{ address, Slot };
}

void RTJE::X86::X86Emitter::EmitJmpViaRegister(Int8 Register, SlotType Slot, RedirectSemantic Sem)
{
	if (Sem == RedirectSemantic::Relative)
	{
		EmitAddImmediateToRegister(Register, 0, Slot, ArithmeticType::Signed);
		NeedFixup(Slot, GetExecutablePage()->Current() - SlotTypeToSize(Slot), AddressFixUpType::RelativeToAbsolute);
	}
	if (Slot == SlotType::Int8)
	{
		Slot = SlotType::Int16;
		Context::OperandPromote(GetEmitContext(), Slot);
	}
	Write(X86OpCodes::JmpRMAbs[(int)Slot]);
	Write(ModRM(AddressingMode::Register, 0b100, Register));
}

RTJE::FlowControlReplaceEntry RTJE::X86::X86Emitter::EmitJcc(Condition Cond, Int64 Imm, SlotType Slot, RedirectSemantic Sem)
{
	if (Slot == SlotType::Int8 || Slot == SlotType::Int16)
		Context::OperandPromote(GetEmitContext(), SlotType::Int32);
	else if (Slot == SlotType::Int64)
		Context::OperandSlice(GetEmitContext(), SlotType::Int32);
	Write(X86OpCodes::JccImmRel[(int)Cond]);
	if (Sem == RedirectSemantic::Absolute)
		NeedFixup(SlotType::Int32, AddressFixUpType::AbsoluteToRelative);
	Int8* address = CurrentPointer();
	WriteImmediate(SlotType::Int32, Imm);
	return FlowControlReplaceEntry{ address, Slot };
}

void RTJE::X86::X86Emitter::EmitCallViaRegister(Int8 Register, SlotType Slot, RedirectSemantic Sem)
{
	if (Sem == RedirectSemantic::Relative && Slot == SlotType::Int64)
	{
		Context::OperandSlice(GetEmitContext(), SlotType::Int32);
		Slot = SlotType::Int32;
	}
	else if (Slot == SlotType::Int8)
	{
		Context::OperandPromote(GetEmitContext(), SlotType::Int16);
		Slot = SlotType::Int16;
	}
	const Int8* OpCodes =
		Sem == RedirectSemantic::Relative ? X86OpCodes::CallRMRel : X86OpCodes::CallRMAbs;
	Write(OpCodes[(int)Slot]);
	Write(ModRM(AddressingMode::Register, 0b010, Register));
}

void RTJE::X86::X86Emitter::EmitCallViaImmediate(Int64 Imm, SlotType Slot, RedirectSemantic Sem)
{
	if (Sem == RedirectSemantic::Relative && Slot == SlotType::Int64)
	{
		Context::OperandSlice(GetEmitContext(), SlotType::Int32);
		Slot = SlotType::Int32;
	}
	else if (Slot == SlotType::Int8)
	{
		Context::OperandPromote(GetEmitContext(), SlotType::Int16);
		Slot = SlotType::Int16;
	}
	if (Sem == RedirectSemantic::Absolute)
	{
		Write(X86OpCodes::CallRMAbs[(int)Slot]);
		Write(ModRM(AddressingMode::RegisterAddressing, 0b010, RegisterOrMemory::Offset32bit));
	}
	else
		Write(X86OpCodes::CallImmRel[(int)Slot]);
	WriteImmediate(Slot, Imm);
}

void RTJE::X86::X86Emitter::EmitLShiftRegisterByImmediateTimes(Int8 Register, Int64 Imm, SlotType Slot, ArithmeticType)
{
	WriteREX(Slot);
	if (Imm == 1ll)
		Write(X86OpCodes::LShiftB1[(int)Slot]);
	else
		Write(X86OpCodes::LShiftBImm[(int)Slot]);
	Write(ModRM(AddressingMode::Register, 0b100, Register));
	if (Slot != SlotType::Int8)
		Context::OperandSlice(GetEmitContext(), SlotType::Int8);
	if (Imm > 1ll)
		WriteImmediate(SlotType::Int8, (Int8)Imm);
}

void RTJE::X86::X86Emitter::EmitLShiftRegisterByRegisterTimes(Int8 Register, Int8 SourceRegister, SlotType Slot, ArithmeticType)
{
	GetEmitContext()->SetRegisterOccupied(Register::CX);
	if (SourceRegister != Register::CX)
		EmitLoadRegisterToRegister(Register::CX, SourceRegister, SlotType::Int8);
	WriteREX(Slot);
	Write(X86OpCodes::LShiftBR[(int)Slot]);
	Write(ModRM(AddressingMode::Register, 0b100, Register));
	GetEmitContext()->SetRegisterAvailable(Register::CX);
}

void RTJE::X86::X86Emitter::EmitRShiftRegisterByImmediateTimes(Int8 Register, Int64 Imm, SlotType Slot, ArithmeticType)
{
	WriteREX(Slot);
	if (Imm == 1ll)
		Write(X86OpCodes::RShiftB1[(int)Slot]);
	else
		Write(X86OpCodes::RShiftBImm[(int)Slot]);
	Write(ModRM(AddressingMode::Register, 0b101, Register));
	if (Slot != SlotType::Int8)
		Context::OperandSlice(GetEmitContext(), SlotType::Int8);
	if (Imm > 1ll)
		WriteImmediate(SlotType::Int8, (Int8)Imm);
}

void RTJE::X86::X86Emitter::EmitRShiftRegisterByRegisterTimes(Int8 Register, Int8 SourceRegister, SlotType Slot, ArithmeticType)
{
	GetEmitContext()->SetRegisterOccupied(Register::CX);
	if (SourceRegister != Register::CX)
		EmitLoadRegisterToRegister(Register::CX, SourceRegister, SlotType::Int8);
	WriteREX(Slot);
	Write(X86OpCodes::RShiftBR[(int)Slot]);
	Write(ModRM(AddressingMode::Register, 0b101, Register));
	GetEmitContext()->SetRegisterAvailable(Register::CX);
}

void RTJE::X86::X86Emitter::EmitOrRegisterToRegister(Int8 Register, Int8 SourceRegister, SlotType Slot)
{
	WriteREX(Slot);
	Write(X86OpCodes::OrR2R[(int)Slot]);
	Write(ModRM(AddressingMode::Register, SourceRegister, Register));
}

void RTJE::X86::X86Emitter::EmitOrImmediateToRegister(Int8 Register, Int64 Imm, SlotType Slot)
{
	WriteREX(Slot);
	if (Slot == SlotType::Int64)
	{
		Slot = SlotType::Int32;
		Context::OperandSlice(GetEmitContext(), SlotType::Int32);
	}
	if (Register == Register::AX)
		Write(X86OpCodes::OrImm2R4AX[(int)Slot]);
	else
		Write(X86OpCodes::OrImm2R[(int)Slot]);
	Write(ModRM(AddressingMode::Register, 0b001, Register));
	WriteImmediate(Slot, Imm);
}

void RTJE::X86::X86Emitter::EmitAndRegisterToRegister(Int8 Register, Int8 SourceRegister, SlotType Slot)
{
	WriteREX(Slot);
	Write(X86OpCodes::AndR2R[(int)Slot]);
	Write(ModRM(AddressingMode::Register, SourceRegister, Register));
}

void RTJE::X86::X86Emitter::EmitAndImmediateToRegister(Int8 Register, Int64 Imm, SlotType Slot)
{
	WriteREX(Slot);
	if (Slot == SlotType::Int64)
	{
		Slot = SlotType::Int32;
		Context::OperandSlice(GetEmitContext(), SlotType::Int32);
	}
	if (Register == Register::AX)
		Write(X86OpCodes::AndImm2R4AX[(int)Slot]);
	else
		Write(X86OpCodes::AndImm2R[(int)Slot]);
	Write(ModRM(AddressingMode::Register, 0b100, Register));
	WriteImmediate(Slot, Imm);
}

void RTJE::X86::X86Emitter::EmitXorRegisterToRegister(Int8 Register, Int8 SourceRegister, SlotType Slot)
{
	WriteREX(Slot);
	Write(X86OpCodes::XorR2R[(int)Slot]);
	Write(ModRM(AddressingMode::Register, SourceRegister, Register));
}

void RTJE::X86::X86Emitter::EmitXorImmediateToRegister(Int8 Register, Int64 Imm, SlotType Slot)
{
	WriteREX(Slot);
	if (Slot == SlotType::Int64)
	{
		Slot = SlotType::Int32;
		Context::OperandSlice(GetEmitContext(), SlotType::Int32);
	}
	if (Register == Register::AX)
		Write(X86OpCodes::XorImm2R4AX[(int)Slot]);
	else
		Write(X86OpCodes::XorImm2R[(int)Slot]);
	Write(ModRM(AddressingMode::Register, 0b110, Register));
	WriteImmediate(Slot, Imm);
}

void RTJE::X86::X86Emitter::EmitNotRegister(Int8 Register, SlotType Slot)
{
	WriteREX(Slot);
	Write(X86OpCodes::Not[(int)Slot]);
	Write(ModRM(AddressingMode::Register, 0b010, Register));
}

void RTJE::X86::X86Emitter::EmitNotMemoryViaImmediate(Int64 Imm, SlotType Slot)
{
	WriteREX(Slot);
	Write(X86OpCodes::Not[(int)Slot]);
	Write(ModRM(AddressingMode::RegisterAddressing, 0b010, RegisterOrMemory::Offset32bit));
	WriteImmediate(Slot, Imm);
}

void RTJE::X86::X86Emitter::EmitNotMemoryViaRegister(Int8 Register, SlotType Slot)
{
	WriteREX(Slot);
	Write(X86OpCodes::Not[(int)Slot]);
	Write(ModRM(AddressingMode::RegisterAddressing, 0b010, Register));
}
