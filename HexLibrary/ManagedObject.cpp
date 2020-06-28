#include "RuntimeAlias.h"
#include "ManagedObject.h"
#include "TypeHandle.h"
#include "TypeDescriptor.h"

RTO::Object* RTO::ArrayObject::AllocateArray
    (RTC::TypeHandle elementType, Int32* args, Int32 argsCount, bool LOHAllocation)
{
    RTD::TypeDescriptor* arrayDescriptor = elementType.AsTypeDescriptor();
    MethodTable* arrayMT = arrayDescriptor->GetMethodTable();
    Int64 overflowChecker;


    return nullptr;
}

RT::UInt32 RTO::Object::GetComponentCount()
{
    
}

RT::UInt32 RTO::Object::GetSize()
{
    
}
