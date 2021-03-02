#include "ObjectStorage.h"
#include "Object.h"

RTO::Object* RTO::ObjectStorage::GetObject() const
{
    return (RTO::Object*)(this + 1);
}
