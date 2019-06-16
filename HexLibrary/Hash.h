#pragma once
namespace HL
{
	namespace System
	{
		namespace Hash
		{

			static size_t GetSequenceHash(void*target, size_t size)noexcept
			{
				const unsigned char *_First = (const unsigned char *)target;
#if defined(_WIN64)
				const size_t _FNV_offset_basis = 14695981039346656037ULL;
				const size_t _FNV_prime = 1099511628211ULL;

#else
				const size_t _FNV_offset_basis = 2166136261U;
				const size_t _FNV_prime = 16777619U;
#endif 
				size_t _Val = _FNV_offset_basis;
				for (size_t _Next = 0; _Next < size; ++_Next)
				{
					_Val ^= (size_t)_First[_Next];
					_Val *= _FNV_prime;
				}
				return (_Val);
			}

			class IHash
			{
			public:
				virtual size_t GetHashCode()const = 0;

				template<class U>
				inline static std::enable_if_t<std::is_base_of_v<IHash, U>, size_t> GetHashCode(U const&Object) {
					return Object.GetHashCode();
				}
				template<class U>
				inline static std::enable_if_t<!std::is_base_of_v<IHash, U>, size_t> GetHashCode(U const&Object)noexcept {
					return GetSequenceHash(reinterpret_cast<void*>(const_cast<U*>(&Object)), sizeof(U));
				}
			};
		}
	}
}