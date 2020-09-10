#pragma once
#include "RuntimeAlias.h"
namespace RTC
{
	class Type;
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
		inline Type* GetType();
	};
	using ObjectRef = Object*;
}