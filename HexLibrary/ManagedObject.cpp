#include "RuntimeAlias.h"
#include "ManagedObject.h"
#include "TypeHandle.h"
#include "TypeDescriptor.h"

RTO::Object* RTO::ArrayObject::AllocateArray
    (TypeHandle elementType, Int32* args, Int32 argsCount, bool LOHAllocation)
{
    RTD::TypeDescriptor* arrayDescriptor = elementType.AsTypeDescriptor();
    MethodTable* arrayMT = arrayDescriptor->GetMethodTable();
    Int64 overflowChecker;


    return nullptr;
}

bool RTO::ArrayObject::IsMultiDimensionalArray() const
{
    return GetMethodTable()->GetNormalizedElementType() == ElementType::Array;
}

const RT::Int32* RTO::ArrayObject::GetBoundsPtr() const
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

RTC::EEClass* RTO::Object::GetClass() const
{
    
}

RT::UInt32 RTO::Object::GetComponentCount()
{
    
}

RT::UInt32 RTO::Object::GetSize()
{
    
}
