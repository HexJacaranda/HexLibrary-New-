#pragma once
#include "RuntimeAlias.h"
#include "Object.h"
#include "TypeHandle.h"

namespace RTO
{
	class ArrayBaseObject :public Object
	{
		Int32 mCount;
		TypeHandle mElementType;
	public:
		Int32 GetCount() {
			return mCount;
		}
		TypeHandle GetElementType()const {
			return mElementType;
		}
		void SetElementType(TypeHandle value) {
			mElementType = value;
		}
		bool IsSZArray()const;
		Int32* GetBoundaries() {
			if (IsSZArray())
				return  &mCount;
			else
				return (Int32*)(this + 1);
		}
	public:
		//Allocate Array in GC Heap
		//Args are [LowerBounds1,Count1,LowerBounds2,Count2,...]
		static ObjectRef AllocateArray(TypeHandle ArrayType, Int32* Args, Int32 DimensionCount);
	};
}
