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
		static constexpr UInt8 LdFld = 0b0;
		static constexpr UInt8 StFld = 0b0;

		static constexpr UInt8 LdLoc = 0b0;
		static constexpr UInt8 StLoc = 0b0;

		static constexpr UInt8 LdArg = 0b0;
		static constexpr UInt8 StArg = 0b0;

		static constexpr UInt8 LdFld = 0b0;
		static constexpr UInt8 StFld = 0b0;

		static constexpr UInt8 LdFn = 0b0;

		static constexpr UInt8 LdAddr = 0b0;

		static constexpr UInt8 Add = 0b0;
		static constexpr UInt8 Sub = 0b0;
		static constexpr UInt8 Mul = 0b0;
		static constexpr UInt8 Div = 0b0;

		static constexpr UInt8 Conv = 0b0;

		static constexpr UInt8 Call = 0b0;
		static constexpr UInt8 CallVirt = 0b0;
		static constexpr UInt8 Ret = 0b0;
		static constexpr UInt8 Jcc = 0b0;
		static constexpr UInt8 Jmp = 0b0;
		static constexpr UInt8 Throw = 0b0;
		static constexpr UInt8 Try = 0b0;
		static constexpr UInt8 Catch = 0b0;
		static constexpr UInt8 Finally = 0b0;
	};
}
