#pragma once
namespace HL::System::Runtime::Core
{
	class MethodTable
	{
	public:
		UInt32 mFlags;
		UInt32 mBasicSize;
		EEClass* pEEClass;
		union
		{
			void** pInterfaceMap;
		};
		UInt16 mInterfaceCount;
		ElementType mNormalizedType;

		void* pModule;

		UInt16 mCCtorSlot;
		UInt16 mDefaultCtorSlot;
		UInt32 mSlots;
		Int8* mVTable[1];
	public:
		EEClass* GetEEClass() 
		{
			return pEEClass;
		}
		ElementType GetNormalizedElementType() {
			return ElementType::Object;
		}
		bool HasSharedMethodTable() {
			return false;
		}
	};
}