#pragma once
#include "..\..\RuntimeAlias.h"
#include "Object.h"
namespace RTC
{
	class Type;
}
namespace RTO
{
	class ArrayObject :public Object
	{
	private:
		UInt32 mCount;
		UInt8 mDimension;
		UInt8 mLowerBounds;
		UInt16 mFlags;
		union
		{
			UInt32* mUpperBounds;
			UInt32 mSingleUpperBound;
		};	
	public:
		ForcedInline UInt32 GetCount()const;
		ForcedInline Int8* GetElementAddress()const;
		inline Type* GetElementType()const;
		inline bool IsMultiDimensionalArray()const;
		inline bool IsSZArray()const;
	};
}