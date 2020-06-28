#pragma once
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
		EEClass* GetClass()const {
			return GetMethodTable()->GetEEClass();
		}
	};

	class ArrayObject : public Object
	{
		Object* AllocateArray(Int32 elementType, Int32* args, Int32 argsCount, bool LOHAllocation);
	private:
		Int32 mComponentCount;
	public:
		bool IsMultiDimensionalArray() const {
			return GetMethodTable()->GetNormalizedElementType() == ElementType::Array;
		}
		const Int32* GetBoundsPtr()const
		{
			if (IsMultiDimensionalArray())
			{
				const Int32* ret = (const Int32*)(this + 1);
				if (GetMethodTable()->HasSharedMethodTable())
					ret = (const Int32*)(((Int8*)ret) + sizeof(IntPtr));
				return ret;
			}
			else
				return &mComponentCount;
		}
		const Int32* GetLowerBoundsPtr()const
		{
			static Int32 zeroBased = 0;
			if (IsMultiDimensionalArray())
				return GetBoundsPtr() + 0;
			return &zeroBased;
		}
	};
}