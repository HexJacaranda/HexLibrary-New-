#include "StringObject.h"

ForcedInline RT::UInt32 RTO::StringObject::GetCount() const
{
    return mCount;
}

ForcedInline RT::Int8* RTO::StringObject::GetContent() const
{
    return (Int8*)(this + 1);
}
