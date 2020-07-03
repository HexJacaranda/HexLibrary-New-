#include "ArrayBaseObject.h"
#include "ArrayDescriptor.h"
#include "MethodTable.h"
#include "RuntimeException.h"
#include "GCInterface.h"
#include <memory>
bool RTO::ArrayBaseObject::IsSZArray()const
{
    return GetMethodTable()->GetCoreType() == CoreType::SZArray;
}

RTO::ObjectRef RTO::ArrayBaseObject::AllocateArray(TypeHandle ArrayType, Int32* Args, Int32 DimensionCount)
{
    RTD::ArrayDescriptor* arrayDesc = ArrayType.AsTypeDescriptor()->AsArray();
    MethodTable* arrayMT = arrayDesc->GetMethodTable();
    CoreType kind = arrayDesc->GetCoreType();
    Int64 overflowChecker = 0;
    Int32 elementCount = 0;
    if (kind == CoreType::Array)
    {
        if (arrayDesc->GetRank() != DimensionCount)
            RTE::Throw(Text("Dimension count not match!"));
        for (Int32 i = 0; i < DimensionCount; ++i)
        {
            overflowChecker = (Int64)elementCount * Args[(DimensionCount << 1) + 1];
            elementCount = overflowChecker;
            if (overflowChecker != (Int64)elementCount)
                RTE::Throw(Text("Arithmetic operation overflew:\n\tArray is too big"));
        }
    }
    else if (kind == CoreType::SZArray)
    {
        if (DimensionCount != 1 || arrayDesc->GetRank() != 1)
            RTE::Throw(Text("Dimension count not match!"));
        elementCount = Args[0];
    }
    else
    {
        //Should never reach here
        RTE::Throw(Text("Invalid core type!"));
    }

    overflowChecker = (Int64)elementCount * arrayMT->GetComponentSize() + arrayMT->GetBaseSize();
    UInt32 size = (UInt32)overflowChecker;
    if (overflowChecker != (Int64)size)
        RTE::Throw(Text("Arithmetic operation overflew:\n\tArray is too big"));
    ArrayBaseObject* ret = nullptr;
    if (size < BigObjectThresholdU)
        ret = (ArrayBaseObject*)RTGC::Allocate(size);
    else
        ret = (ArrayBaseObject*)RTGC::AllocateOnLOH(size);

    ret->mCount = elementCount;
    if (kind == CoreType::Array)
    {
        Int32* boundaries = ret->GetBoundaries();
        if (memcpy_s(boundaries,
            DimensionCount * sizeof(Int32) << 1,
            Args,
            DimensionCount * sizeof(Int32) << 1))
            RTE::Throw(Text("Memory copy failed!"));
    }
    
    ret->SetElementType(arrayDesc->GetParameterTypeHandle());
    return ret;
}
