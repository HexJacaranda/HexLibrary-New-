#include "Object.h"
#include "ArrayBaseObject.h"

RT::UInt32 RTO::Object::GetComponentCount()
{
    return ((ArrayBaseObject*)this)->GetCount();
}
