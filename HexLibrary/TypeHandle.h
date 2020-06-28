#pragma once
namespace HL::System::Runtime::Core
{
	class TypeHandle
	{
	private:
		union {
			IntPtr mAsInt;
			void* mAsPtr;
			MethodTable* mAsMethodTable;
		};
	public:
	};
}