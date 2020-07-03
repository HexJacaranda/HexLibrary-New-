#include "ArrayDescriptor.h"
#include "MethodTable.h"

RTD::ArrayDescriptor::ArrayDescriptor(MethodTable* ArrayMT, TypeHandle ElementType)
	:SingleParameterTypeDescriptor(ArrayMT->GetCoreType(), ArrayMT, ElementType) {}

RT::UInt32 RTD::ArrayDescriptor::GetRank()
{
	return UInt32();
}

