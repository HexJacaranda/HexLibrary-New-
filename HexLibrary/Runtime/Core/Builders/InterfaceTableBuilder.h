#pragma once
#include "..\..\RuntimeAlias.h"
namespace RTC
{
	class InterfaceDispatchTable;
}
namespace RTB
{
	class InterfaceTableBuilder
	{
	public:
		static InterfaceDispatchTable* BuildFrom(Int8* rawBytes);
	};
}