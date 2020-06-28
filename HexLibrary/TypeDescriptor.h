#pragma once
#include "NormalizedType.h"
#include "MethodTable.h"
#include "TypeHandle.h"

namespace HL::System::Runtime::Core::Descriptor
{
	class TypeDescriptor
	{
	public:
		TypeDescriptor(ElementType type) {
			mType = type;
		}
		ElementType GetNormalizedElementType() {
			return mType;
		}
		MethodTable* GetMethodTable();
	protected:
		ElementType mType;
	};

	class ParameterizedTypeDescritpor : public TypeDescriptor
	{
	public:
		ParameterizedTypeDescritpor(ElementType type, MethodTable* methodTable, TypeHandle argument)
			:TypeDescriptor(type), mTemplateMT(methodTable), mArg(argument) {}	

		MethodTable* mTemplateMT;
		TypeHandle mArg;
	};

}