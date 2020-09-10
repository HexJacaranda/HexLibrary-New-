#pragma once
#include "RuntimeAlias.h"
namespace RTC
{
	class Type;
}
namespace RTC
{
	struct MethodSlot
	{

		UInt8* Entry;
	};
	class MethodTable
	{
		Type* mType;
		MethodTable* mParent;
	};
}