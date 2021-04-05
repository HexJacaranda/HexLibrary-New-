#pragma once
#include "..\RuntimeAlias.h"
namespace RTC
{
	class Type;
}
namespace RTC
{
	class SlotType
	{
	public:
		constexpr static UInt8 IL = 0x01;	
		constexpr static UInt8 PreJIT = 0x03;
		constexpr static UInt8 JIT = 0x04;
		constexpr static UInt8 Native = 0x02;
	};

	class CallingConvention
	{
	public:
		constexpr static UInt8 JIT = 0x01;
		constexpr static UInt8 Cdecl = 0x02;
		constexpr static UInt8 StdCall = 0x03;
		constexpr static UInt8 FastCall = 0x04;
		constexpr static UInt8 VectorCall = 0x05;
	};

	struct MethodSlot
	{
		UInt8 CallingConvention;
		UInt8 SlotType;
		UInt16 Flags;
		UInt8* Entry;
	};

	struct MethodSlotBundle
	{
		constexpr static Int32 SlotCount = 4;
		MethodSlot Slots[SlotCount];
	};

	class MethodTable
	{
		Type* mType;
	public:
		inline Type* GetOwningType()const;
		inline MethodSlotBundle* GetStartBundle()const;
		inline MethodSlot* GetSlotAt(Int32 index)const;
	};
}