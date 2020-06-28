#pragma once
namespace RTD
{
	class TypeDescriptor;
}
namespace RTC
{
	class MethodTable;
}
namespace HL::System::Runtime::Core
{
	//To represent all kinds of types
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
		TypeHandle(EEClass* Class);
		TypeHandle(RTD::TypeDescriptor* Type) {
			mAsTypeDescriptor = Type;
		}
		RTD::TypeDescriptor* AsTypeDescriptor()const {
			return (RTD::TypeDescriptor*)(mAsInt & ~2);
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
			return (mAsInt & 2) == 0;
		}
		bool IsTypeDescriptor()const {
			return !IsUnsharedMT();
		}
		MethodTable* GetMethodTable();
	};


}