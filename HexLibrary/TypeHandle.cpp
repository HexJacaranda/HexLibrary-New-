#include "RuntimeAlias.h"
#include "MethodTable.h"
#include "TypeHandle.h"

RTC::MethodTable* RTC::TypeHandle::GetMethodTable()
{
	if (IsUnsharedMT())
		return AsMethodTable();
	else
		return AsTypeDescriptor()->GetMethodTable();

}
