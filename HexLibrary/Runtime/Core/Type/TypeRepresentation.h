#pragma once
#include "..\..\RuntimeAlias.h"

namespace RTC
{
	class Type;
}

namespace RTD
{
	enum class RepresentationCategory : UInt16
	{
		ReferenceType = 0x0001,
		ValueType = 0x0002,

		Object = 0x0003,
		Array = 0x0004,
		Delegate = 0x0005,

		Bool = 0x0006,
		Byte = 0x0007,
		Char = 0x0008,
		Short = 0x0009,
		Int = 0x000A,
		Long = 0x000B,
		Float = 0x000C,
		Double = 0x000D,

		UByte = 0x000E,
		UShort = 0x000F,
		UInt = 0x0010,
		ULong = 0x0011
	};

	enum class RepresentationModifier : UInt16
	{
		Reference = 0x0001,
		Pointer = 0x0002
	};

	class TypeRepresentation
	{
		UInt32 mFlag;
		Type* mType;
	public:
		TypeRepresentation(RepresentationCategory category,
			RepresentationModifier modifier,
			Type* target);
		inline RepresentationCategory GetCategory()const {
			return (RepresentationCategory)((mFlag & 0xFFFF0000) >> (sizeof(UInt16) * 8));
		}
		inline void SetCategory(RepresentationCategory value) {
			UInt16 target = (UInt16)value;
			mFlag &= 0x0000FFFF;
			mFlag |= (target << (sizeof(UInt16) * 8));
		}
		inline RepresentationModifier GetModifier()const {
			return (RepresentationModifier)(mFlag & 0x0000FFFF);
		}
		inline void SetModifier(RepresentationModifier value) {
			UInt16 target = (UInt16)value;
			mFlag &= 0xFFFF0000;
			mFlag |= target;
		}
	};
}