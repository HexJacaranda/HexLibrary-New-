#pragma once
namespace HL
{
	namespace System
	{
		namespace Collection
		{
			namespace Generic
			{
				template<class U>
				class Array :public Linq::LinqBase<U*, const U*, Array<U>>, Memory::AllocatorSupporter
				{
					U* m_objects = nullptr;
					size_t m_count = 0;
					void From(Array const&other)
					{
						m_objects = static_cast<U*>(Allocator->Alloc(sizeof(U)*m_count));
						Memory::ZeroMemory(m_objects, sizeof(U)*m_count);
						if (Utility::IsPod<U>::Value)
							Memory::MemoryCopy(m_objects, other.m_objects, sizeof(U)*m_count);
						else
							Memory::Construct(m_objects, other.m_objects, m_count);
					}
				public:
					Array(Memory::IAllocator*Allocator = Memory::DefaultAllocator::Instance())
						:Memory::AllocatorSupporter(Allocator) {}
					template<typename = std::enable_if_t<std::is_default_constructible_v<U>,int>>
					Array(size_t Size, Memory::IAllocator* Allocator = Memory::DefaultAllocator::Instance())
						:Memory::AllocatorSupporter(Allocator), m_count(Size) {
						if (Size > 0)
						{
							m_objects = static_cast<U*>(Allocator->Alloc(sizeof(U)*m_count));
							Memory::ZeroMemory(m_objects, sizeof(U)*m_count);
							Memory::ConstructWith(m_objects, m_count);
						}
						else
							m_objects = nullptr;
					}
					template<typename = std::enable_if_t<std::is_copy_constructible_v<U>, int>>
					Array(U const& DefaultValue, size_t Size, Memory::IAllocator* Allocator = Memory::DefaultAllocator::Instance())
						:Memory::AllocatorSupporter(Allocator), m_count(Size) {
						if (Size > 0)
						{
							m_objects = static_cast<U*>(Allocator->Alloc(sizeof(U)*m_count));
							Memory::ZeroMemory(m_objects, sizeof(U)*m_count);
							Memory::ConstructWith(m_objects, m_count, DefaultValue);
						}
						else
							m_objects = nullptr;
					}
					Array(Array const&rhs)
						:Memory::AllocatorSupporter(rhs.Allocator), m_count(rhs.m_count) {
						From(rhs);
					}
					Array(Array &&lhs)noexcept
						:Memory::AllocatorSupporter(lhs.Allocator),m_count(lhs.m_count){
						this->m_objects = lhs.m_objects;
						lhs.m_objects = nullptr;
						lhs.m_count = 0;
					}
					Array(std::initializer_list<U> const&list) 
						:Memory::AllocatorSupporter(Memory::DefaultAllocator::Instance()) {
						m_count = list.size();
						m_objects = static_cast<U*>(Allocator->Alloc(sizeof(U)*m_count));
						Memory::Construct(m_objects, list.begin(), m_count);
					}
					Array& operator=(Array const&rhs) {
						this->Clear();
						From(rhs);
						return *this;
					}
					Array& operator=(Array &&lhs) {
						this->Clear();
						Utility::move_assign(m_objects, lhs.m_objects);
						Utility::move_assign(m_count, lhs.m_count);
						return *this;
					}
					void AdjustTo(size_t NewSize) {
						if (NewSize < m_count && !Utility::IsPod<U>::Value)
							for (size_t i = NewSize; i < m_count; ++i)
								Memory::Destruct(m_objects + i);
						size_t copy_size = NewSize < m_count ? NewSize : m_count;
						U* new_block = static_cast<U*>(Allocator->Alloc(sizeof(U)*NewSize));
						Memory::ZeroMemory(new_block, sizeof(U)*NewSize);
						Memory::MemoryCopy(new_block, m_objects, sizeof(U)*copy_size);
						Allocator->Collect(m_objects);
						m_objects = new_block;
						m_count = NewSize;
					}
					void Clear() {
						if (this->m_objects != nullptr)
						{
							if (!Utility::IsPod<U>::Value)
								for (size_t i = 0; i < m_count; ++i)
									Memory::Destruct(m_objects + i);
							Allocator->Collect(this->m_objects);
							this->m_objects = nullptr;
							this->m_count = 0;
						}
					}
					inline U& operator[](size_t Index)noexcept {
						return m_objects[Index];
					}
					inline U const& operator[](size_t Index)const noexcept {
						return m_objects[Index];
					}
					inline U* begin()noexcept {
						return this->m_objects;
					}
					inline U const* begin()const noexcept {
						return this->m_objects;
					}
					inline U* end()noexcept {
						return this->m_objects + m_count;
					}
					inline U const* end()const noexcept {
						return this->m_objects + m_count;
					}
					inline size_t Count()const noexcept {
						return m_count;
					}
					~Array() {
						Clear();
					}
				};
			}
		}
	}
}