#include "ArrayObject.h"
#include "Type.h"

ForcedInline RT::UInt32 RTO::ArrayObject::GetCount() const
{
    return mCount;
}

ForcedInline RT::Int8* RTO::ArrayObject::GetElementAddress() const
{
    return (Int8*)(this + 1);
}

inline RTC::Type* RTO::ArrayObject::GetElementType() const
{
    return GetType()->GetSingleTypeArgument();
}
