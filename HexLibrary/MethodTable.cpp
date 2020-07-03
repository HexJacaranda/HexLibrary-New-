#include "MethodTable.h"

RTC::CoreType RTC::MethodTable::GetCoreType()
{
    return CoreType();
}

RTC::EEClass* RTC::MethodTable::GetClass()
{
    return nullptr;
}

RT::UInt32 RTC::MethodTable::GetComponentSize()
{
    return UInt32();
}

RT::UInt32 RTC::MethodTable::GetBaseSize()
{
    return UInt32();
}

bool RTC::MethodTable::HasSharedMethodTable()
{
    return false;
}
