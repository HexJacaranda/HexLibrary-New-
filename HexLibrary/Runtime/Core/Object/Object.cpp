#include "Object.h"
#include "ArrayObject.h"
#include "..\MethodTable.h"
#include "..\Type\Type.h"
#include "ObjectStorage.h"

inline RTC::Type* RTO::Object::GetType()const
{
    return mType;
}

inline RT::UInt32 RTO::Object::GetObjectSize() const
{
    if (mType->IsArray())
    {
        ArrayObject* array = (ArrayObject*)this;
        return array->GetCount() * array->GetElementType()->GetBaseSize();
    }
    return GetType()->GetBaseSize();
}

inline RTO::ObjectStorage* RTO::Object::GetStorage() const
{
    return (RTO::ObjectStorage*)this - 1;
}
