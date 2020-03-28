#pragma once
namespace HL::System::Runtime::JIT::Binary
{
	class BinaryHelper
	{
	public:
		template<class IntT>
		static constexpr inline Int8* WriteIntByBigEndianess(Int8* where,IntT value) {
			Int8* bytes = (Int8*)&value;
			for (size_t i = 0; i < sizeof(IntT); i++)
				*where++ = bytes[i];
			return where;
		}
		template<class IntT>
		static constexpr inline Int8* WriteIntByLittleEndianess(Int8* where, IntT value) {
			Int8* bytes = (Int8*)&value;
			for (size_t i = 1; i <= sizeof(IntT); i++)
				*where++ = bytes[sizeof(IntT) - i];
			return where;
		}
	};
}