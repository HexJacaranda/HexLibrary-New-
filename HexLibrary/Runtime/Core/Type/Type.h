#pragma once
#include "..\..\RuntimeAlias.h"
namespace RTC
{
	class MethodTable;
	class FieldTable;
	class InterfaceDispatchTable;
	class AssemblyContext;
}
namespace RTB
{
	class TypeBuilder;
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
		//Use CoreTypes to represent.
		static constexpr UInt32 DetailedMask = 0x00000FF0;
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
		/// <summary>
		/// Unicode name
		/// </summary>
		RTString mUnicodeName;
		UInt32 mToken;
		/// <summary>
		/// The base size of object of this type. Special types like array and string
		/// do not represent the real object size in heap.
		/// </summary>
		Int32 mBaseSize;
		UInt32 mFlag;
		Type* mParent;
		/// <summary>
		/// Only valid when this is an enclosed type.
		/// </summary>
		Type* mEnclosingParent;
		/// <summary>
		/// If this is a half open or concrete generic type, this field points to the  
		/// origin generic type. Otherwise, it's null.
		/// </summary>
		Type* mPureGenericSource;
		/// <summary>
		/// Interfaces that this type implements or reabstracts.
		/// </summary>
		Type** mInterfaces;
		MethodTable* mMethTable;
		FieldTable* mFieldTable;
		InterfaceDispatchTable* mInterfaceTable;
		AssemblyContext* mContext;
		union {
			/// <summary>
			/// If any item is null, then the corresponding type argument is unfilled.
			/// </summary>
			Type** mTypeArguments;
			/// <summary>
			/// Inline storage for single type argument
			/// </summary>
			Type* mSingleTypeArgument;
		};
		/// <summary>
		/// Total number of type arguments
		/// </summary>
		Int16 mTypeArgumentsTotal;
		/// <summary>
		/// The number of filled type arguments
		/// </summary>
		Int16 mTypeArgumentsCount;
	public:
		//Friend to builder
		friend class TypeBuilder;
	public:
		/// <summary>
		/// Get basic size for type
		/// </summary>
		/// <returns></returns>
		inline Int32 GetBaseSize()const;
		inline bool IsPrimitiveType()const;
		inline bool IsOpenType()const;
		inline bool IsClosedType()const;
		inline bool IsArray()const;
		inline bool IsString()const;
		inline UInt8 GetCoreType()const;
		/// <summary>
		/// Is it a normal type or fully open generic type
		/// </summary>
		/// <returns></returns>
		inline bool IsCanonicalType()const;
		inline MethodTable* GetMethodTable()const;
		inline FieldTable* GetFieldTable()const;
		inline InterfaceDispatchTable* GetInterfaceDispatchTable()const;
		inline RTString GetTypeName()const;
		inline UInt32 GetLoadingLevel()const;
		inline Type** GetTypeArguments()const;
		inline Type* GetSingleTypeArgument()const;
		//Load type from context
		void PrepareType(UInt32 loadingLevel);
	};
}