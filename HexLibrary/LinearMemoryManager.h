#pragma once
namespace HL
{
	namespace System
	{
		namespace Collection
		{
			class AllocationFailedException :Exception::IException
			{
			public:
				AllocationFailedException(): Exception::IException(L"Memory allocation failed") {}
			};
			//线性内存管理器
			template<class U>
			class LinearMemoryManager :public Memory::AllocatorSupporter
			{
				U* m_data = nullptr;
				size_t m_total_count = 0;
				size_t m_used_count = 0;
				float m_increasing_factor = 2.0f;

				void Resize(size_t Count, bool AllowGrowth = true) {
					size_t to = 0;
					if (AllowGrowth)
						to = (size_t)((m_total_count + Count)*m_increasing_factor);
					else
						to = m_total_count + Count;
					void*block = Allocator->Alloc(sizeof(U)*to);
					if (block == nullptr&&AllowGrowth)
					{
						to = m_total_count + Count;
						block = Allocator->Alloc(sizeof(U)*to);
						if (block == nullptr)
							Exception::Throw<AllocationFailedException>();
					}
					Memory::MemoryCopy(block, this->m_data, sizeof(U)*m_total_count);
					Allocator->Collect(this->m_data);
					m_total_count = to;
					this->m_data = static_cast<U*>(block);
				}
				inline void CheckSize(size_t New)
				{
					if (m_used_count + New > m_total_count)
						Resize(New);
				}
				void Construct(size_t Capacity) {
					m_data = (U*)Allocator->Alloc(sizeof(U)*Capacity);
					if (m_data == nullptr)
						Exception::Throw<AllocationFailedException>();
					this->m_total_count = Capacity;
				}
				void ConstructFrom(U const*From, size_t Count) {
					Construct(Count);
					this->m_used_count = Count;
					if (Utility::IsPod<U>::Value)
						Memory::MemoryCopy(m_data, From, sizeof(U)*Count);
					else
						Memory::Construct(m_data, From, Count);
				}
			public:
				LinearMemoryManager(Memory::IAllocator*Allocator = Memory::DefaultAllocator::Instance())
					:Memory::AllocatorSupporter(Allocator) {}
				LinearMemoryManager(size_t Capacity,Memory::IAllocator*Allocator = Memory::DefaultAllocator::Instance())
					:Memory::AllocatorSupporter(Allocator) {
					Construct(Capacity);
				}
				LinearMemoryManager(LinearMemoryManager const&rhs) 
					:Memory::AllocatorSupporter(rhs.Allocator) {
					this->m_increasing_factor = rhs.m_increasing_factor;
					if (rhs.m_used_count == 0)
						return;
					ConstructFrom(rhs.m_data, rhs.m_used_count);
				}
				LinearMemoryManager(LinearMemoryManager && lhs)noexcept :Memory::AllocatorSupporter(nullptr) {
					Utility::move_assign(this->Allocator, lhs.Allocator);
					Utility::move_assign(this->m_data, lhs.m_data);
					Utility::move_assign(this->m_total_count, lhs.m_total_count);
					Utility::move_assign(this->m_used_count, lhs.m_used_count);
					Utility::move_assign(this->m_increasing_factor, lhs.m_increasing_factor);
				}
				LinearMemoryManager&operator=(LinearMemoryManager const&rhs) {
					this->Empty();
					this->Allocator = rhs.Allocator;
					this->m_increasing_factor = rhs.m_increasing_factor;
					if (rhs.m_used_count == 0)
						return *this;
					ConstructFrom(rhs.m_data, rhs.m_used_count);
					return *this;
				}
				LinearMemoryManager&operator=(LinearMemoryManager &&lhs)noexcept {
					this->Empty();
					Utility::move_assign(this->Allocator, lhs.Allocator);
					Utility::move_assign(this->m_data, lhs.m_data);
					Utility::move_assign(this->m_total_count, lhs.m_total_count);
					Utility::move_assign(this->m_used_count, lhs.m_used_count);
					Utility::move_assign(this->m_increasing_factor, lhs.m_increasing_factor);
					return *this;
				}
				void Append(U const&Target) {
					CheckSize(1);
					Memory::PlacementNew(m_data + m_used_count, Target);
					m_used_count++;
				}
				void Append(U && Target) {
					CheckSize(1);
					Memory::PlacementNew(m_data + m_used_count, std::forward<U>(Target));
					m_used_count++;
				}
				void Append(U const*From, size_t Count) {
					CheckSize(Count);
					if (Utility::IsPod<U>::Value)
						Memory::MemoryCopy(m_data + m_used_count, From, Count * sizeof(U));
					else
						Memory::Construct(m_data + m_used_count, From, Count);
					m_used_count += Count;
				}
				void Remove(size_t Base, size_t Top) {
					if (!Utility::IsPod<U>::Value)
						for (size_t i = Base; i < Top; ++i)
							Memory::Destruct(m_data + i);
					Memory::MemoryCopy(m_data + Base, m_data + Top, sizeof(U)*(m_used_count - Top));
					m_used_count -= (Top - Base);
				}
				inline U const* GetDataPtr()const {
					return this->m_data;
				}
				inline U* GetDataPtr() {
					return this->m_data;
				}

				inline size_t UsedCount()const {
					return this->m_used_count;
				}
				inline void UsedCount(size_t Count) {
					this->m_used_count = Count;
				}
				inline size_t MaxCount()const {
					return this->m_total_count;
				}
				inline void MaxCount(size_t Count) {
					this->m_total_count = Count;
				}
				inline float IncFactor()const {
					return this->m_increasing_factor;
				}
				inline void IncFactor(float Value) {
					this->m_increasing_factor = Value;
				}
				inline void Shrink() {
					if (this->m_total_count == this->m_used_count || this->m_used_count == 0)
						return;
					U* new_block = static_cast<U*>(Allocator->Alloc(sizeof(U)*this->m_used_count));
					Memory::MemoryCopy(new_block, this->m_data, sizeof(U)*this->m_used_count);
					Allocator->Collect(this->m_data);
					this->m_data = new_block;
					this->m_total_count = this->m_used_count;
				}
				inline U& At(size_t Index) {
					return m_data[Index];
				}
				inline U const& At(size_t Index)const {
					return m_data[Index];
				}
				void Empty() {
					if (m_data != nullptr)
					{
						if (!Utility::IsPod<U>::Value)
							for (size_t index = 0; index < m_used_count; ++index)
								Memory::Destruct(m_data + index);
						Allocator->Collect(m_data);
						m_data = nullptr;
					}
					m_used_count = 0;
					m_total_count = 0;
				}
				~LinearMemoryManager() {
					Empty();
				}
			};
		}
	}
}