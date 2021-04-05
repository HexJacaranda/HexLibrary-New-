#pragma once
#include "..\..\RuntimeAlias.h"

namespace RTJ
{
	/// <summary>
	/// Number of opcodes may increases with time. Currently we support up to 255 different opcodes(8 bit).
	/// <para>1. The opcode of value 0 is reserved for extension </para>
	/// <para>2. Other opcodes are all one-byte length</para>
	/// <para>The encoding of instruction:</para>
	/// <para>[OpCode(8 Bit)][Other data]</para>
	/// <para>Necessary data followed:</para>
	/// <para>[OpCode][Number of consumed stack items(4 Bit)][Number of produced stack items(4 Bit)]</para>
	/// <para>Specially, value 0b1111 in slot means variable stack items, which depends on the context.</para>
	/// </summary>
	class OpCodes
	{
	public:
		static constexpr UInt8 LdFld = 0x01;
		static constexpr UInt8 LdFldBae = 0xF1;

		static constexpr UInt8 StFld = 0x02;
		static constexpr UInt8 StFldBae = 0x1F;

		static constexpr UInt8 LdLoc = 0x03;
		static constexpr UInt8 LdLocBae = 0x01;

		static constexpr UInt8 StLoc = 0x04;
		static constexpr UInt8 StLocBae = 0x10;

		static constexpr UInt8 LdArg = 0x05;
		static constexpr UInt8 LdArgBae = 0x01;

		static constexpr UInt8 StArg = 0x06;
		static constexpr UInt8 StArgBae = 0x10;

		static constexpr UInt8 LdFn = 0x07;
		static constexpr UInt8 LdFnBae = 0x01;

		static constexpr UInt8 LdFldA = 0x08;
		static constexpr UInt8 LdFldABae = 0xF1;

		static constexpr UInt8 LdLocA = 0x09;
		static constexpr UInt8 LdLocABae = 0x01;

		static constexpr UInt8 StTA = 0x0A;
		static constexpr UInt8 StTABae = 0x11;

		static constexpr UInt8 Add = 0x0B;
		static constexpr UInt8 AddBae = 0x21;

		static constexpr UInt8 Sub = 0x0C;
		static constexpr UInt8 SubBae = 0x21;

		static constexpr UInt8 Mul = 0x0D;
		static constexpr UInt8 MulBae = 0x21;

		static constexpr UInt8 Div = 0x0E;
		static constexpr UInt8 DivBae = 0x21;

		static constexpr UInt8 Conv = 0x0F;
		static constexpr UInt8 DivBae = 0x11;

		static constexpr UInt8 Call = 0x10;
		static constexpr UInt8 CallBae = 0xFF;

		static constexpr UInt8 CallVirt = 0x11;
		static constexpr UInt8 CallVirtBae = 0xFF;

		static constexpr UInt8 Ret = 0x12;
		static constexpr UInt8 RetBae = 0x11;

		static constexpr UInt8 Jcc = 0x13;
		static constexpr UInt8 JccBae = 0x10;

		static constexpr UInt8 Jmp = 0x14;
		static constexpr UInt8 JccBae = 0x00;

		static constexpr UInt8 Throw = 0x15;
		static constexpr UInt8 ThrowBae = 0x00;

		static constexpr UInt8 Try = 0x16;
		static constexpr UInt8 TryBae = 0x00;

		static constexpr UInt8 Catch = 0x17;
		static constexpr UInt8 CatchBae = 0x00;

		static constexpr UInt8 Finally = 0x18;
		static constexpr UInt8 FinallyBae = 0x00;

		static constexpr UInt8 LdElem = 0x19;
		static constexpr UInt8 LdElemBae = 0x21;

		static constexpr UInt8 StElem = 0x1A;
		static constexpr UInt8 StElemBae = 0x30;

		static constexpr UInt8 LdElemA = 0x1B;
		static constexpr UInt8 LdElemABae = 0x21;
	};

	/// <summary>
	/// Condition constant is encoded in 4 bits following the stack item slot.
	/// </summary>
	class OpCodeJccConstant
	{
	public:
		static constexpr UInt8 EqualTo = 0b0000;
		static constexpr UInt8 NotEqualTo = 0b0001;
		static constexpr UInt8 GreaterThan = 0b0010;
		static constexpr UInt8 LessThan = 0b0011;
		static constexpr UInt8 GreaterThanOrEqualTo = 0b0100;
		static constexpr UInt8 LessThanOrEqualTo = 0b0101;

		/// <summary>
		/// Offset integer of up to 15 bytes is supported.
		/// </summary>
		static constexpr UInt8 OffsetSizeMask = 0b1111;
	};

	/// <summary>
	/// Indicating opcode operand type.
	/// </summary>
	class OpCodeOperandTypeConstant
	{
	public:
		static constexpr UInt8 I1 = 0x00;
		static constexpr UInt8 I2 = 0x01;
		static constexpr UInt8 I4 = 0x02;
		static constexpr UInt8 I8 = 0x03;

		static constexpr UInt8 U1 = 0x04;
		static constexpr UInt8 U2 = 0x05;
		static constexpr UInt8 U4 = 0x06;
		static constexpr UInt8 U8 = 0x07;

		/// <summary>
		/// Half
		/// </summary>
		static constexpr UInt8 R2 = 0x08;
		/// <summary>
		/// Single
		/// </summary>
		static constexpr UInt8 R4 = 0x09;
		/// <summary>
		/// Double
		/// </summary>
		static constexpr UInt8 R8 = 0x0A;
	};
}
