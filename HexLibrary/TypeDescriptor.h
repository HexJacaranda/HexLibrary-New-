#pragma once
#include "RuntimeAlias.h"
#include "StandardType.h"
#include "TypeHandle.h"

namespace RTC
{
	class MethodTable;
}
namespace RTD
{
	class ArrayDescriptor;
}

namespace RTD
{
	//Used to describe type where a single MethodTable* cannot express
	class TypeDescriptor
	{
	protected:
		CoreType mType;
	public:
		TypeDescriptor(CoreType type) :mType(type) {}
		CoreType GetCoreType() {
			return mType;
		}
		//Cast this to array desc
		ArrayDescriptor* AsArray() {
			return (ArrayDescriptor*)this;
		}
		//This may result in returning canonical MT for PartiallyOpenGenericTypeDescriptor
		MethodTable* GetMethodTable();
	};

	//Provide description for generic type(fully concrete)
	class ConcreteGenericTypeDescriptor : public TypeDescriptor
	{
	public:
		//MethodTable must go fisrt so that GetCanonicalMethodTable() can get it.
		MethodTable* pMT;
		TypeHandle* mArgs;
		Int32 mCount;	
		ConcreteGenericTypeDescriptor(CoreType Type, MethodTable* MT, TypeHandle* Args, Int32 Count);
	};

	//Provide description for partially open generic type like Dictionary<string,>
	class PartiallyOpenGenericTypeDescriptor : public TypeDescriptor
	{
	public:
		//MethodTable must go fisrt so that GetCanonicalMethodTable() can get it.
		MethodTable* pCannocialMT;
		TypeHandle* mArgs;
		Int32 mFilledCount;
		Int32* mFilledIndices;
		PartiallyOpenGenericTypeDescriptor(CoreType Type, MethodTable* CanonicalMT, TypeHandle* Args, Int32* FilledIndices, Int32 Count);
	};

	//Specialized generic type for only one parameter(only used for System.Array, Reference, Pointer)
	class SingleParameterTypeDescriptor : public TypeDescriptor
	{
	public:
		//MethodTable must go fisrt so that GetCanonicalMethodTable() can get it.
		MethodTable* pMT;
		TypeHandle pHandle;
		SingleParameterTypeDescriptor(CoreType Type, MethodTable* MT, TypeHandle Handle);
		TypeHandle GetParameterTypeHandle();
	};

	//Specialized for function pointer
	class FunctionPointerDescriptor :public TypeDescriptor
	{
	public:
		FunctionPointerDescriptor(CoreType Type) :TypeDescriptor(Type) {}
	};
}