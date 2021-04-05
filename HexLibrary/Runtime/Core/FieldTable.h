#pragma once
#include "..\RuntimeAlias.h"
namespace RTC
{
	class Type;
}

namespace RTC
{
	class FieldStorage
	{
	public:
		//Storage bit mask, 0 - 3 bit reserved
		static constexpr UInt16 Mask = 0x000F;
		static constexpr UInt16 Static = 0x0000;
		static constexpr UInt16 NonStatic = 0x0001;
		static constexpr UInt16 ThreadLocal = 0x0002;
	};

	class FieldProperty
	{
	public:
		//Property bit mask, 4 - 12 bit reserved
		static constexpr UInt16 Mask = 0x0FF0;
		//Is special field
		static constexpr UInt16 Special = 0x0010;
		//Property back up field
		static constexpr UInt16 BackUp = 0x0020;
		//Is hidden
		static constexpr UInt16 Hidden = 0x0040;
	};

	class FieldAccessbility
	{
	public:
		//Accessbility bit mask, 12 - 16 bit reserved
		static constexpr UInt16 Mask = 0xF000;
		static constexpr UInt16 Public = 0x1000;
		static constexpr UInt16 Private = 0x2000;
		static constexpr UInt16 Protected = 0x3000;
		static constexpr UInt16 Internal = 0x4000;
	};

	struct FieldSlot
	{
		static constexpr Int32 OffsetUnSet = -1;
		RTString FieldName;
		//Offset relative to the start of object.
		Int32 Offset;
		//Reference members are always equal to sizeof(void*)
		Int32 FieldSize;
		//Token can be split into <UInt16, UInt16> representation
		//which is useful when JIT need to decide what code to emit
		//according to field information(i.e. WriteBarrier)
		union
		{
			struct {
				UInt16 Index;
				UInt16 Flag;
			};
			UInt32 Token;
		};
		//Field may be generic type when it is not concrete
		//Always 8 bytes in 32 bit and 64 bit mode
		union
		{
			Type* FieldType;
			struct {
				union 
				{
					UInt32 Level;
					UInt32 TypeIndex;
				};
				UInt64 GenericParameterRepresentation;
			};		
		};	
	public:
		inline UInt16 GetStorageType()const;
		inline UInt16 GetProperty()const;
		inline UInt16 GetAccessbility()const;
	};


	class FieldTable
	{
		//Non-Static fields are stored right behind the table
		//and static fields and tls fields may be stored somewhere else.

		FieldSlot* mStaticField;
		FieldSlot* mTLSField;
		Type* mOwningType;
		FieldTable* mParent;
	public:
		FieldSlot* GetSlot(UInt32 token)const;
		inline FieldSlot* GetNonStaticSlot(Int32 index)const;
		inline FieldSlot* GetTLSSlot(Int32 index)const;
		inline FieldSlot* GetStaticSlot(Int32 index)const;
		ForcedInline FieldSlot* GetNonStaticFieldStart()const;
	};
}