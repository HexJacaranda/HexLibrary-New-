#pragma once
namespace HL::System::Runtime::JIT
{
	enum class OpCodeType
	{

	};

	enum class ControlFlow
	{
		Next,
		Branch,
		ConditionalBranch
	};

	enum class OpCodeValue
	{
		//Control Flow
		Nop = 0x00,
		Break = 0x01,
		Jmp = 0x02,
		LtBr = 0x03,
		LtoEqBr = 0x04,
		GtBr = 0x05,
		GtoEqBr = 0x06,
		EqBr = 0x07,
		NEqBr = 0x08,
		Call = 0x09,
		CallVirt = 0x0A,
		Ret = 0x0B,
		//Data Accessing
		LdLoc = 0x0C,
		StLoc = 0x0D,
		LdArg = 0x0E,
		StArg = 0x0F,
		StFld = 0x10,
		LdFld = 0x11,
		StInd = 0x12,
		LdInd = 0x13,
		StEle = 0x14,
		LdEle = 0x15,
		//Type System
		Box = 0x16,
		UnBox = 0x17,
		Cast = 0x18,
		TryCast = 0x19,
		//Operation
		AddI = 0x1A,
		SubI = 0x1B,
		MulI = 0x1C,
		DivI = 0x1D,
		AddF = 0x1E,
		SubF = 0x1F,
		MulF = 0x20,
		DivF = 0x21
	};

	
	struct OpCode
	{	
		constexpr static int ControlFlowMask = 0x0000000F;
		constexpr static int TypeMask = 0x000000F0;
		OpCodeValue Value;
		//
		int Flag;
	};
}
