#pragma once
#include "RuntimeAlias.h"
namespace RTC
{
	class Type;
	class FieldTable;
	class MethodTable;
}

namespace RTO
{
	static constexpr Int32 BigObjectThreshold = 0xFFFF;
	static constexpr UInt32 BigObjectThresholdU = 0xFFFFu;
	
	class Object
	{
	private:
		Type* mType;
	public:
		inline Type* GetType()const;
		inline FieldTable* GetFieldTable()const;
		inline MethodTable* GetMethodTable()const;
	};
	using ObjectRef = Object*;
}