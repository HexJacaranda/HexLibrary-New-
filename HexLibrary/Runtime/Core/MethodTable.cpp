#include "MethodTable.h"

inline RTC::Type* RTC::MethodTable::GetOwningType() const
{
    return mType;
}

inline RTC::MethodSlotBundle* RTC::MethodTable::GetStartBundle() const
{
    return (RTC::MethodSlotBundle*)(this + 1);
}

inline RTC::MethodSlot* RTC::MethodTable::GetSlotAt(Int32 index) const
{
    Int32 slotIndex = index % MethodSlotBundle::SlotCount;
    Int32 bundleIndex = index / MethodSlotBundle::SlotCount;
    return (GetStartBundle() + bundleIndex)->Slots + slotIndex;
}
