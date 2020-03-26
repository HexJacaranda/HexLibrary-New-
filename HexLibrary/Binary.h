#pragma once
namespace HL::System::Runtime::JIT::Binary
{
	class BinaryWriter
	{
	public:
		template<class IntT>
		static constexpr inline Int8* WriteIntByBigEndianess(Int8* where,IntT value) {
			Int8* bytes = (Int8*)&value;
			constexpr size_t total = sizeof(value) >> 2;
			for (size_t i = 0; i < total; i++)
				*where++ = bytes[i];
			return where;
		}
		template<class IntT>
		static constexpr inline Int8* WriteIntByLittleEndianess(Int8* where, IntT value) {
			Int8* bytes = (Int8*)&value;
			constexpr size_t total = sizeof(value) >> 2;
			for (size_t i = 1; i <= total; i++)
				*where++ = bytes[total - i];
			return where;
		}
	};
}