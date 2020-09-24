#include "FieldTable.h"

inline RT::UInt16 RTC::FieldSlot::GetStorageType() const
{
	return Flag & FieldStorage::Mask;
}

inline RT::UInt16 RTC::FieldSlot::GetProperty() const
{
	return Flag & FieldProperty::Mask;
}

inline RT::UInt16 RTC::FieldSlot::GetAccessbility() const
{
	return Flag & FieldAccessbility::Mask;
}

RTC::FieldSlot* RTC::FieldTable::GetSlot(UInt32 token)const
{
	UInt32 index = (token & 0xFFFF0000) >> 16;
	UInt32 flag = token & 0x0000FFFF;
	switch (flag & FieldStorage::Mask)
	{
	case FieldStorage::Static:      return mStaticField + index;
	case FieldStorage::NonStatic:   return GetNonStaticFieldStart() + index;
	case FieldStorage::ThreadLocal: return mTLSField + index;
	default:
		break;
	}
}

inline RTC::FieldSlot* RTC::FieldTable::GetNonStaticSlot(Int32 index)const
{
	return GetNonStaticFieldStart() + index;
}

inline RTC::FieldSlot* RTC::FieldTable::GetTLSSlot(Int32 index)const
{
	return mTLSField + index;
}

inline RTC::FieldSlot* RTC::FieldTable::GetStaticSlot(Int32 index)const
{
	return mStaticField + index;
}

ForcedInline RTC::FieldSlot* RTC::FieldTable::GetNonStaticFieldStart()const
{
	return (RTC::FieldSlot*)(this + 1);
}
