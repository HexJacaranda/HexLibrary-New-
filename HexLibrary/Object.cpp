#include "Object.h"
#include "ArrayObject.h"
#include "MethodTable.h"
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

inline RT::UInt32 RTO::Object::GetObjectSize() const
{
    if (mType->IsArray())
    {
        ArrayObject* array = (ArrayObject*)this;
        return array->GetCount() * array->GetType()->GetSingleTypeArgument()->GetBaseSize();
    }
    return GetType()->GetBaseSize();
}
