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
				class List :public Linq::LinqBase<U*,const U*, List<U>>
				{
					LinearMemoryManager<U> m_memory;
				public:
					List(Memory::IAllocator* Allocator = Memory::DefaultAllocator::Instance()) :m_memory(Allocator) {
					}
					List(std::initializer_list<U> const&InitList) :m_memory(InitList.size(), Memory::DefaultAllocator::Instance()) {
						for (U const&var : InitList)
							m_memory.Append(var);
					}
					List(size_t Capactiy, Memory::IAllocator* Allocator = Memory::DefaultAllocator::Instance()) :m_memory(Capactiy, Allocator) {
					}
					List(List const&) = default;
					List(List&&)noexcept = default;
					List& operator=(List const&) = default;
					List& operator=(List&&) = default;
					inline void Add(U const&Target) {
						m_memory.Append(Target);
					}
					inline void Add(U && Target) {
						m_memory.Append(std::move(Target));
					}
					inline void Add(U const*Array, size_t Size) {
						m_memory.Append(Array, Size);
					}
					inline void Add(const List<U>& Array) {
						m_memory.Append(Array.m_memory.GetDataPtr(), Array.Count());
					}
					template<class IteratorT>
					inline void AddRange(IteratorT&& begin, IteratorT&& end) {
						for (; begin != end; ++begin)
							m_memory.Append(*begin);
					}
					inline size_t IndexOf(U const& Key)const noexcept {
						return Algorithm::IndexOf(m_memory.GetDataPtr(), m_memory.UsedCount(), Key);
					}
					template<class FilterT>
					inline void RemoveIf(FilterT&&filter) {
						size_t top = m_memory.UsedCount();
						for (size_t i = 0; i < top; ++i)
							if (std::forward<FilterT>(filter)(m_memory.At(i)))
							{
								m_memory.Remove(i, i + 1);
								top--;
							}
					}
					template<class ComparatorT>
					size_t IndexOfMax(ComparatorT&&comparator)const {
						if (m_memory.UsedCount() == 0)
							return std::numeric_limits<size_t>::max();
						U const* base = m_memory.GetDataPtr();
						U const* key = base;
						U const* iterator = base + 1;
						U const* end = base + m_memory.UsedCount();
						for (; iterator != end; ++iterator)
							if (std::forward<ComparatorT>(comparator)(*iterator, *key))
								key = iterator;
						return key - base;
					}
					template<class ComparatorT>
					size_t IndexOfMin(ComparatorT&&comparator)const {
						if (m_memory.UsedCount() == 0)
							return std::numeric_limits<size_t>::max();
						U const* base = m_memory.GetDataPtr();
						U const* key = base;
						U const* iterator = base + 1;
						U const* end = base + m_memory.UsedCount();
						for (; iterator != end; ++iterator)
							if (std::forward<ComparatorT>(comparator)(*iterator, *key))
								key = iterator;
						return key - base;
					}
					size_t IndexOfMax()const {
						if (m_memory.UsedCount() == 0)
							return std::numeric_limits<size_t>::max();
						U const* base = m_memory.GetDataPtr();
						U const* key = base;
						U const* iterator = base + 1;
						U const* end = base + m_memory.UsedCount();
						for (; iterator != end; ++iterator)
							if (*iterator > *key)
								key = iterator;
						return key - base;
					}
					size_t IndexOfMin()const {
						if (m_memory.UsedCount() == 0)
							return std::numeric_limits<size_t>::max();
						U const* base = m_memory.GetDataPtr();
						U const* key = base;
						U const* iterator = base + 1;
						U const* end = base + m_memory.UsedCount();
						for (; iterator != end; ++iterator)
							if (*iterator < *key)
								key = iterator;
						return key - base;
					}
					List<U> SubSequence(size_t base, size_t top) {
						List<U> ret(top - base);
						ret.Add(m_memory.GetDataPtr() + base, top - base);
						return std::move(ret);
					}
					inline void ShrinkToFit() {
						m_memory.Shrink();
					}
					inline void RemoveAt(size_t Index) {
						m_memory.Remove(Index, Index + 1);
					}
					template<class Filter>
					bool Exists(Filter&& filter)const {
						U const* iterator = m_memory.GetDataPtr();
						U const* end = iterator + m_memory.UsedCount();
						for (; iterator != end; ++iterator)
							if (std::forward<Filter>(filter)(*iterator))
								return true;
						return false;
					}
					inline U* begin() {
						return this->m_memory.GetDataPtr();
					}
					inline U const* begin()const {
						return this->m_memory.GetDataPtr();
					}
					inline U* end() {
						return this->m_memory.GetDataPtr() + this->Count();
					}
					inline U const* end()const {
						return this->m_memory.GetDataPtr() + this->Count();
					}
					inline U&operator[](size_t Index) {
						return m_memory.At(Index);
					}
					inline U const&operator[](size_t Index)const {
						return m_memory.At(Index);
					}
					inline size_t Count()const noexcept {
						return this->m_memory.UsedCount();
					}
					inline size_t Capacity()const {
						return this->m_memory.MaxCount();
					}
					inline void Clear() {
						m_memory.Empty();
					}
					~List() {
						this->Clear();
					}
				};
			}
		}
		namespace Pointer
		{
			template<class U>
			struct IteratorInterface<Collection::Generic::List<U>>
			{
				typedef U* IteratorT;
				typedef const U* ConstIteratorT;
			};
		}
	}
}