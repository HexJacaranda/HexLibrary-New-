#include "TypeDescriptor.h"

RTD::ConcreteGenericTypeDescriptor::ConcreteGenericTypeDescriptor(CoreType Type, MethodTable* MT, TypeHandle* Args, Int32 Count)
	:RTD::TypeDescriptor(Type), pMT(MT), mArgs(Args), mCount(Count) {}

RTD::SingleParameterTypeDescriptor::SingleParameterTypeDescriptor(CoreType Type, MethodTable* MT, TypeHandle Handle)
	: RTD::TypeDescriptor(Type), pMT(MT), pHandle(Handle) {}

RTC::TypeHandle RTD::SingleParameterTypeDescriptor::GetParameterTypeHandle()
{
	return pHandle;
}

RTC::MethodTable* RTD::TypeDescriptor::GetMethodTable()
{
	return ((SingleParameterTypeDescriptor*)this)->pMT;
}

RTD::PartiallyOpenGenericTypeDescriptor::PartiallyOpenGenericTypeDescriptor(CoreType Type, MethodTable* CanonicalMT, TypeHandle* Args, Int32* FilledIndices, Int32 Count)
	:RTD::TypeDescriptor(Type), pCannocialMT(CanonicalMT), mArgs(Args), mFilledIndices(FilledIndices), mFilledCount(Count) {}
