#include "Type.h"
#include "AssemblyContext.h"
#include "CoreTypes.h"

inline RT::Int32 RTC::Type::GetBaseSize() const
{
    return mBaseSize;
}

inline bool RTC::Type::IsPrimitiveType() const
{
    return GetCoreType() < CoreTypes::Struct;
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
    return GetCoreType() == CoreTypes::Array;
}

inline bool RTC::Type::IsString() const
{
    return GetCoreType() == CoreTypes::String;
}

inline RT::UInt8 RTC::Type::GetCoreType() const
{
    return ((mFlag & TypeFlag::DetailedMask) >> 4);
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

inline RTC::Type** RTC::Type::GetTypeArguments() const
{
    return mTypeArguments;
}

inline RTC::Type* RTC::Type::GetSingleTypeArgument() const
{
    return mSingleTypeArgument;
}

void RTC::Type::PrepareType(UInt32 loadingLevel)
{

}
