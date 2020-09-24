#include "Object.h"
#include "Type.h"

inline RTC::Type* RTO::Object::GetType()const
{
    return mType;
}

inline RTC::FieldTable* RTO::Object::GetFieldTable()const
{
    return mType->GetFieldTable();
}

inline RTC::MethodTable* RTO::Object::GetMethodTable()const
{
    return mType->GetMethodTable();
}
