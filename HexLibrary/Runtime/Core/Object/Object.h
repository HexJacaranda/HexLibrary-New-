#pragma once
#include "..\..\RuntimeAlias.h"
namespace RTC
{
	class Type;
	class FieldTable;
	class MethodTable;
	class ArrayObject;
}

namespace RTO
{
	class ObjectStorage;
}

namespace RTO
{
	
	static constexpr Int32 BigObjectThreshold = 0xFFFF;
	static constexpr UInt32 BigObjectThresholdU = 0xFFFFu;
	static constexpr Int32 SmallestObjectSize = sizeof(void*) + sizeof(Int);
	class Object
	{
	private:
		Type* mType;
	public:
		inline Type* GetType()const;
		inline UInt32 GetObjectSize()const;
		inline ObjectStorage* GetStorage()const;
	};
	using ObjectRef = Object*;
}