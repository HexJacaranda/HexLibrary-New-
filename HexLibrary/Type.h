#pragma once
#include "RuntimeAlias.h"
namespace RTC
{
	class MethodTable;
	class FieldTable;
}

namespace RTC
{
	class TypeFlag {
	public:
		static constexpr UInt32 CategoryMask = 0xF0000000;
		static constexpr UInt32 ValueType = 0x10000000;
		static constexpr UInt32 ReferenceType = 0x20000000;
		static constexpr UInt32 RefStruct = 0x30000000;

		static constexpr UInt32 DetailedMask = 0x0FF00000;
		static constexpr UInt32 PrimitiveType = 0x00100000;
		static constexpr UInt32 Object = 0x00200000;
		static constexpr UInt32 String = 0x00300000;
		static constexpr UInt32 Array = 0x00400000;
	};
	class Type
	{
		RTString mUnicodeName;
		Int32 mId;
		Int32 mBaseSize;
		UInt32 mFlag;
		Type* mParent;
		Type* mPureGenericSource;
		MethodTable* mMethTable;
		FieldTable* mFieldTable;
		Type** mTypeArguments;
		Int16 mTypeArgumentsTotal;
		Int16 mTypeArgumentsCount;
	public:
		inline Int32 GetBaseSize()const;
		inline bool IsPrimitiveType()const;
		inline bool IsOpenType()const;
		inline bool IsClosedType()const;
		inline bool IsArray()const;
		inline bool IsString()const;
		inline bool IsCanonicalType()const;
		inline MethodTable* GetMethodTable()const;
		inline FieldTable* GetFieldTable()const;
		inline RTString GetTypeName()const;
	};
}