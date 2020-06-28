#pragma once
#include "NormalizedType.h"
#include "MethodDescritpor.h"
namespace HL::System::Runtime::Core
{
	class ArrayClass :public EEClass
	{
		union
		{
			struct {
				UInt8 mRankBegin;
				UInt8 mElementTypeBegin;
			};
			struct {
				UInt8 mRank : 8;
				ElementType mElementType : 8;
			};
		};
		TypeHandle mElementTypeHandle;
		Descriptor::MethodDescriptor* pElementCtor;
	public:
		UInt32 GetRank() {
			return mRank;
		}
		void SetRank(UInt32 value) {
			mRank = value;
		}
		Descriptor::MethodDescriptor* GetElementCtor() {
			return pElementCtor;
		}
		void SeElementCtor(Descriptor::MethodDescriptor* value) {
			pElementCtor = value;
		}
		ElementType GetElementType() {
			return mElementType;
		}
		void SetElementType(ElementType value) {
			mElementType = value;
		}
		TypeHandle GetElementTypeHandle() {
			return mElementTypeHandle;
		}
		void SetElementTypeHandle(TypeHandle value) {
			mElementTypeHandle = value;
		}
	};
}