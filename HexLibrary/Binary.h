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
		static void WriteByLittleEndianess(Int8* Target, IntT Value);
		template<class IntT>
		static void WriteByBigEndianess(Int8* Target, IntT Value);
	};

	template<class IntT>
	inline void Binary::WriteByLittleEndianess(Int8* Target, IntT Value)
	{
		if (Host == Endianness::Little)
			*(IntT*)Target = Value;
		else
		{
			switch (sizeof(IntT))
			{
				case sizeof(Int8) : *Target = Value; break;
				case sizeof(Int16) : *(Int16*)Target = SwapInt16(Value); break;
				case sizeof(Int32) : *(Int32*)Target = SwapInt32(Value); break;
				case sizeof(Int64) : *(Int64*)Target = SwapInt64(Value); break;
			}
		}
	}
	template<class IntT>
	inline void Binary::WriteByBigEndianess(Int8* Target, IntT Value)
	{
		if (Host == Endianness::Big)
			*(IntT*)Target = Value;
		else
		{
			switch (sizeof(IntT))
			{
				case sizeof(Int8) : *Target = Value; break;
				case sizeof(Int16) : *(Int16*)Target = SwapInt16(Value); break;
				case sizeof(Int32) : *(Int32*)Target = SwapInt32(Value); break;
				case sizeof(Int64) : *(Int64*)Target = SwapInt64(Value); break;
			}
		}
	}
}