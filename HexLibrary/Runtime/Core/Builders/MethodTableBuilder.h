#pragma once
#include "..\..\RuntimeAlias.h"
namespace RTC
{
	class MethodTable;
	class Type;
}

namespace RTB
{
	class MethodTableBuilder
	{
	public:
		static MethodTable* BuildFrom(Int8* rawBytes);
	};
}