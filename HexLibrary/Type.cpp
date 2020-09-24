#include "Type.h"
#include "AssemblyContext.h"

inline RT::Int32 RTC::Type::GetBaseSize() const
{
    return mBaseSize;
}

inline bool RTC::Type::IsPrimitiveType() const
{
    return (mFlag & TypeFlag::DetailedMask) == TypeFlag::PrimitiveType;
}

inline bool RTC::Type::IsOpenType() const
{
    return mTypeArgumentsCount < mTypeArgumentsTotal;
}

inline bool RTC::Type::IsClosedType() const
{
    return mTypeArgumentsCount == mTypeArgumentsTotal;
}

inline bool RTC::Type::IsArray() const
{
    return (mFlag & TypeFlag::DetailedMask) == TypeFlag::Array;
}

inline bool RTC::Type::IsString() const
{
    return (mFlag & TypeFlag::DetailedMask) == TypeFlag::String;
}

inline bool RTC::Type::IsCanonicalType() const
{
    return mPureGenericSource == nullptr;
}

inline RTC::MethodTable* RTC::Type::GetMethodTable() const
{
    return mMethTable;
}

inline RTC::FieldTable* RTC::Type::GetFieldTable() const
{
    return mFieldTable;
}

inline RTC::InterfaceDispatchTable* RTC::Type::GetInterfaceDispatchTable() const
{
    return mInterfaceTable;
}

inline RT::RTString RTC::Type::GetTypeName() const
{
    return mUnicodeName;
}

inline RT::UInt32 RTC::Type::GetLoadingLevel() const
{
    return mFlag & TypeLoadingLevel::Mask;
}

void RTC::Type::LoadFully()
{
    mContext->LoadType(this);
}
