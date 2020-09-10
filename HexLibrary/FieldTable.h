#pragma once
#include "RuntimeAlias.h"
namespace RTC
{
	class Type;
}

namespace RTC
{
	enum class FieldStorage :UInt16
	{
		Static,
		NonStatic,
		ThreadLocal
	};

	struct FieldSlot
	{
		static constexpr Int32 OffsetUnSet = -1;
		RTString FieldName;
		Int32 Offset;
		Int32 FieldSize;
		Int16 Token;
		FieldStorage FieldStorageType;
		Type* FieldType;
	};

	class FieldTable
	{
		FieldSlot* mNonStaticField;
		FieldSlot* mStaticField;
		FieldSlot* mTLSField;
		Type* mOwningType;
		FieldTable* mParent;
	public:

	};
}