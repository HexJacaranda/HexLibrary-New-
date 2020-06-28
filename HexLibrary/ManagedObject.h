#pragma once

namespace RTC
{
	class MethodTable;
	class EEClass;
	class TypeHandle;
}

namespace HL::System::Runtime::Core::ManagedObject
{
	class Object
	{
	private:
		MethodTable* pMethodTable;
	protected:
		Object() {}
		~Object() {}
	public:
		MethodTable* GetMethodTable()const {
			return pMethodTable;
		}
		void SetMethodTable(MethodTable* value) {
			pMethodTable = value;
		}
		EEClass* GetClass()const;
		UInt32 GetComponentCount();
		UInt32 GetSize();
	};

	class ArrayObject : public Object
	{
		Object* AllocateArray(TypeHandle elementType, Int32* args, Int32 argsCount, bool LOHAllocation);
	private:
		Int32 mComponentCount;
	public:
		bool IsMultiDimensionalArray()const;
		const Int32* GetBoundsPtr()const;
		const Int32* GetLowerBoundsPtr()const
		{
			static Int32 zeroBased = 0;
			if (IsMultiDimensionalArray())
				return GetBoundsPtr() + 0;
			return &zeroBased;
		}
	};
}