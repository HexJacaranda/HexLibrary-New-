#pragma once
#include "NormalizedType.h"
namespace HL::System::Runtime::Core
{
	class ArrayClass :public EEClass
	{
		union
		{
			struct {
				UInt8 mRankBegin;
				UInt8 mElementTypeBegin;
			}offset;
			struct {
				UInt8 mRank : 8;
				ElementType mElementType : 8;
			};
		};
		TypeHandle mElementTypeHandle;

	};
}