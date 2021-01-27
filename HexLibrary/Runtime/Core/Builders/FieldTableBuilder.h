#pragma once
#include "..\..\RuntimeAlias.h"
namespace RTC
{
	class FieldTable;
}
namespace RTB
{
	class FieldTableBuilder
	{
	public:
		static FieldTable* BuildFrom(Int8* rawBytes);
	};
}