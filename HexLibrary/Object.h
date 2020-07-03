#pragma once
#include "RuntimeAlias.h"

namespace RTC
{
	class MethodTable;
	class EEClass;
	class TypeHandle;
}

namespace RTO
{
	static constexpr Int32 BigObjectThreshold = 0xFFFF;
	static constexpr UInt32 BigObjectThresholdU = 0xFFFFu;
	
	class Object
	{
	private:
		MethodTable* pMethodTable;
	protected:
		Object() { pMethodTable = nullptr; }
		~Object() {}
	public:
		MethodTable* GetMethodTable()const {
			return pMethodTable;
		}
		void SetMethodTable(MethodTable* value) {
			pMethodTable = value;
		}
		UInt32 GetComponentCount();
	};
	using ObjectRef = Object*;
}