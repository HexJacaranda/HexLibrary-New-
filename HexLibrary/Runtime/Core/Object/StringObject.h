#pragma once
#include "..\..\RuntimeAlias.h"
#include "Object.h"
namespace RTC
{
	class Type;
}
namespace RTO
{
	class StringObject :public Object
	{
	private:
		UInt32 mCount;
	public:
		ForcedInline UInt32 GetCount()const;
		ForcedInline Int8* GetContent()const;
	};
}