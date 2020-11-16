#pragma once
#include "RuntimeAlias.h"
namespace RTC
{
	class MethodTable;
	class FieldTable;
	class InterfaceDispatchTable;
	class AssemblyContext;
}

namespace RTC
{
	class TypeFlag {
	public:
		//Reserved for 0 - 3 bit
		static constexpr UInt32 CategoryMask = 0x0000000F;
		static constexpr UInt32 ValueType = 0x00000001;
		static constexpr UInt32 ReferenceType = 0x00000002;
		static constexpr UInt32 RefStruct = 0x00000003;

		//Reserved for 4 - 11 bit
		static constexpr UInt32 DetailedMask = 0x00000FF0;
		static constexpr UInt32 PrimitiveType = 0x00000010;
		static constexpr UInt32 Object = 0x00000020;
		static constexpr UInt32 String = 0x00000030;
		static constexpr UInt32 Array = 0x00000040;
	};

	class TypeLoadingLevel {
	public:
		//Reserved for 12 - 15 bit
		static constexpr UInt32 Mask = 0x0000F000;
		static constexpr UInt32 NotYet = 0x00001000;
		static constexpr UInt32 Partially = 0x00002000;
		static constexpr UInt32 Almost = 0x00003000;
		static constexpr UInt32 Fully = 0x00004000;
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
		InterfaceDispatchTable* mInterfaceTable;
		AssemblyContext* mContext;
		union {
			Type** mTypeArguments;
			//Inline storage for single type
			Type* mSingleTypeArgument;
		};
		Int16 mTypeArgumentsTotal;
		Int16 mTypeArgumentsCount;
	public:
		//Get basic size for type
		inline Int32 GetBaseSize()const;
		inline bool IsPrimitiveType()const;
		inline bool IsOpenType()const;
		inline bool IsClosedType()const;
		inline bool IsArray()const;
		inline bool IsString()const;
		inline bool IsCanonicalType()const;
		inline MethodTable* GetMethodTable()const;
		inline FieldTable* GetFieldTable()const;
		inline InterfaceDispatchTable* GetInterfaceDispatchTable()const;
		inline RTString GetTypeName()const;
		inline UInt32 GetLoadingLevel()const;

		void LoadFully();
	};
}