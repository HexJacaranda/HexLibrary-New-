#pragma once
#include "RuntimeAlias.h"
#include "TypeDescriptor.h"
#include "TypeHandle.h"
namespace RTC
{
	class MethodTable;
}
namespace RTD
{
	class ArrayDescriptor :public SingleParameterTypeDescriptor
	{
	public:
		ArrayDescriptor(MethodTable* ArrayMT, TypeHandle ElementType);
		UInt32 GetRank();
	};
}