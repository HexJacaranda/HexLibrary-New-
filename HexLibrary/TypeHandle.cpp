#include "RuntimeAlias.h"
#include "MethodTable.h"
#include "TypeHandle.h"
#include "TypeDescriptor.h"
#include "EEClass.h"

RTC::TypeHandle::TypeHandle(RTC::EEClass* Class)
{
	
}

RTC::MethodTable* RTC::TypeHandle::GetMethodTable()
{
	if (IsUnsharedMT())
		return AsMethodTable();
	else
		return AsTypeDescriptor()->GetMethodTable();
}
