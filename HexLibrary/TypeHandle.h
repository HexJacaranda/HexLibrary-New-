#pragma once
#include "RuntimeAlias.h"
namespace RTC
{
	class MethodTable;
}
namespace RTD
{
	class TypeDescriptor;
}
namespace RTC
{
	//TypeHandle represents and accommodates all kinds of type 
	//representation (i.e. MethodTable,TypeDescriptor,...)
	class TypeHandle
	{
	private:
		union {
			IntPtr mAsInt;
			void* mAsPtr;
			MethodTable* mAsMethodTable;
			RTD::TypeDescriptor* mAsTypeDescriptor;
		};
	public:
		TypeHandle() {
			mAsPtr = nullptr;
		}
		TypeHandle(MethodTable* MT) {
			mAsMethodTable = MT;
		}
		TypeHandle(RTD::TypeDescriptor* Type) {
			mAsTypeDescriptor = Type;
		}
		IntPtr AsInt()const {
			return mAsInt;
		}
		void* AsPtr()const {
			return mAsPtr;
		}
		MethodTable* AsMethodTable()const {
			return mAsMethodTable;
		}
		RTD::TypeDescriptor* AsTypeDescriptor()const {
			return mAsTypeDescriptor;
		}
		bool IsNull() const {
			return mAsPtr == nullptr;
		}
	};
}