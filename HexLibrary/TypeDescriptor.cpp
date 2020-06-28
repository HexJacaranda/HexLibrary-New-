#include "TypeDescriptor.h"
#include "RuntimeAlias.h"

RTC::MethodTable* RTD::TypeDescriptor::GetMethodTable()
{
    return ((ParameterizedTypeDescritpor*)this)->mTemplateMT;
}
