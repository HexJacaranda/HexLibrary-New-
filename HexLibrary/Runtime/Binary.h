#pragma once
#include "RuntimeAlias.h"
#include <stdlib.h>
namespace RTC
{
#define SwapInt16(Value) _byteswap_ushort(*(unsigned short*)&Value)
#define SwapInt32(Value) _byteswap_ulong(*(unsigned long*)&Value)
#define SwapInt64(Value) _byteswap_uint64(*(unsigned long long*)&Value)

	enum class Endianness
	{
		Big,
		Little
	};
	class Binary
	{
		static Endianness Host;	
	public:
		template<class IntT>
		static void WriteByLittleEndianness(Int8* Target, IntT Value);
		template<class IntT>
		static void WriteByBigEndianness(Int8* Target, IntT Value);
	};

	Endianness Binary::Host = Endianness::Little;

	template<class IntT>
	inline void Binary::WriteByLittleEndianness(Int8* Target, IntT Value)
	{
		if (Host == Endianness::Little)
			*(IntT*)Target = Value;
		else
		{
			switch (sizeof(IntT))
			{
				case sizeof(UInt8) : *Target = *(UInt8*)&Value; break;
				case sizeof(UInt16) : *(UInt16*)Target = SwapInt16(Value); break;
				case sizeof(UInt32) : *(UInt32*)Target = SwapInt32(Value); break;
				case sizeof(UInt64) : *(UInt64*)Target = SwapInt64(Value); break;
			}
		}
	}
	template<class IntT>
	inline void Binary::WriteByBigEndianness(Int8* Target, IntT Value)
	{
		if (Host == Endianness::Big)
			*(IntT*)Target = Value;
		else
		{
			switch (sizeof(IntT))
			{
				case sizeof(UInt8) : *Target = *(UInt8*)&Value; break;
				case sizeof(UInt16) : *(UInt16*)Target = SwapInt16(Value); break;
				case sizeof(UInt32) : *(UInt32*)Target = SwapInt32(Value); break;
				case sizeof(UInt64) : *(UInt64*)Target = SwapInt64(Value); break;
			}
		}
	}
}