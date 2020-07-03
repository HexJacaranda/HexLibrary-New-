#pragma once
#include "RuntimeAlias.h"
#include "StandardType.h"
namespace RTC
{
	class EEClass;
}
namespace RTC
{
	class MethodTable
	{
	public:
		CoreType GetCoreType();
		EEClass* GetClass();
		UInt32 GetComponentSize();
		UInt32 GetBaseSize();
		bool HasSharedMethodTable();
	};
}