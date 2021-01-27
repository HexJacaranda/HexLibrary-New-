#include "ArrayObject.h"
#include "..\Type\Type.h"

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

inline bool RTO::ArrayObject::IsMultiDimensionalArray() const
{
    return mDimension > 1;
}

inline bool RTO::ArrayObject::IsSZArray() const
{
    return mDimension == 1 && mLowerBounds == 0;
}
