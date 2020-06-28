#pragma once
#include "TypeDescriptor.h"
namespace HL::System::Runtime::Core
{
	class TypeHandle
	{
	private:
		union {
			IntPtr mAsInt;
			void* mAsPtr;
			MethodTable* mAsMethodTable;
			Descriptor::TypeDescriptor* mAsTypeDescriptor;
		};
	public:
		Descriptor::TypeDescriptor* AsTypeDescriptor()const {
			return (Descriptor::TypeDescriptor*)(mAsInt & ~2);
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
		bool IsNull() const {
			return mAsPtr == nullptr;
		}
		__forceinline bool IsUnsharedMT()const {
			return mAsInt & 2 == 0;
		}
		bool IsTypeDescriptor()const {
			return !IsUnsharedMT();
		}
		MethodTable* GetMethodTable();
	};


}