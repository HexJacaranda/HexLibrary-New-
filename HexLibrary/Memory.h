#pragma once
#include <memory>

namespace HL
{
	namespace System
	{
		namespace Memory
		{
			template<class T,class...Args>
			static inline constexpr T* PlacementNew(T*Target, Args&&...args)noexcept{
				return new(Target)T(std::forward<Args>(args)...);
			}
			template<class T>
			static inline constexpr void Destruct(T*Target)noexcept {
				Target->T::~T();
			}
			template<class T,class...Args>
			static inline constexpr void ConstructWith(T*Target, size_t Length,Args&&...args){
				while (Length-- > 0)
					PlacementNew(Target++, std::forward<Args...>(args)...);
			}
			template<class T,class U>
			static inline constexpr void Construct(T*Target, U const* From, size_t Length) {
				while (Length-- > 0)
					PlacementNew(Target++, *From++);
			}
			template<class U>
			static void ZeroMemory(U*Target) {
				constexpr size_t unit = sizeof(long long);
				long long*ldest = reinterpret_cast<long long*>(Target);
				for (size_t i = 0; i < sizeof(U) / unit; ++i)
					*ldest++ = 0LL;
				char* cdest = reinterpret_cast<char*>(ldest);
				for (size_t i = 0; i < sizeof(U) % unit; ++i)
					*cdest++ = 0;
			}

			class IAllocator
			{
			public:
				virtual void* Alloc(size_t) = 0;
				virtual void Collect(void*) = 0;
			};

			class AllocatorSupporter
			{
			protected:
				IAllocator* Allocator;
			public:
				AllocatorSupporter(IAllocator*Target) :Allocator(Target) {}
				virtual ~AllocatorSupporter(){
					Allocator = nullptr;
				}
			};

			class DefaultAllocator :public IAllocator
			{
			public:
				virtual void* Alloc(size_t Size)final {
					return malloc(Size);
				}
				virtual void Collect(void*Target)final {
					free(Target);
				}
				static DefaultAllocator* Instance() {
					static DefaultAllocator default_allocator;
					return &default_allocator;
				}
			};

			static void MemoryCopy(void*Destination, void const*Source, size_t Size)
			{
				constexpr size_t unit = sizeof(long long);
				long long*ldest = static_cast<long long*>(Destination);
				long long const* lsource = static_cast<long long const*>(Source);
				for (size_t i = 0; i < Size / unit; ++i)
					*ldest++ = *lsource++;
				char* cdest = reinterpret_cast<char*>(ldest);
				char const*csource = reinterpret_cast<char const*>(lsource);
				for (size_t i = 0; i < Size%unit; ++i)
					*cdest++ = *csource++;
			}

			static void ZeroMemory(void*Target, size_t Size) {
				constexpr size_t unit = sizeof(long long);
				long long*ldest = static_cast<long long*>(Target);
				for (size_t i = 0; i < Size / unit; ++i)
					*ldest++ = 0LL;
				char* cdest = reinterpret_cast<char*>(ldest);
				for (size_t i = 0; i < Size%unit; ++i)
					*cdest++ = 0;
			}
		}
	}
}
