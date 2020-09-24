#include "AssemblyContext.h"

inline RT::Int32 RTC::AssemblyContext::GetId() const
{
    return mId;
}

inline RT::UInt32 RTC::AssemblyContext::GetLoadingType() const
{
    return mFlag & AssemblyContextLoadingType::Mask;
}

void RTC::AssemblyContext::LoadType(Type* value)
{
}
