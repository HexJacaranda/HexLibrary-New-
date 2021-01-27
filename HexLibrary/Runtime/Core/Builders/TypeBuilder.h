#pragma once
#include "..\..\RuntimeAlias.h"
namespace RTC
{
	class Type;
}
namespace RTB
{
	class TypeBuilder
	{
	public:
		static Type* BuildFrom(Int8* rawBytes);
	};
}