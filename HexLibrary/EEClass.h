#pragma once
#include "RuntimeAlias.h"
namespace RTC
{
	class MethodTable;
}
namespace RTC::Loader
{
	class ClassLoader;
}

namespace HL::System::Runtime::Core
{
	class EEClass
	{
		UInt16 mInstaceFieldsCount;
		UInt16 mStaticFieldsCount;
		UInt16 mGCPointersCount;
		UInt16 mVTableSlotsCount;
		UInt16 mMethodSlotsCount;
		UInt16 mDuplicateSlotsCount;
		MethodTable* pMethodTable;
		Loader::ClassLoader* pClassLoader;
		EEClass* pSiblings;
		union 
		{
			EEClass* pChildren;
		};
	public:

	};
}