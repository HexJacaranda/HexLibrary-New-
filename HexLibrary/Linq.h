#pragma once
namespace HL
{
	namespace System
	{
		namespace Linq
		{
			template<class IteratorT,class FilterT>
			class WhereIterator
			{
				IteratorT m_iterator;
				IteratorT m_end;
				FilterT m_filter;
			public:
				template<class FunctorT>
				WhereIterator(IteratorT const&begin, IteratorT const&end, FunctorT &&filter)
					:m_iterator(begin),m_end(end),m_filter(std::forward<FunctorT>(filter)){
					if (begin == end)
						return;
					if (!m_filter(*m_iterator))
						this->operator++();
				}
				WhereIterator&operator++() {
					for (;;)
					{
						++m_iterator;
						if (m_filter(*m_iterator) || m_iterator == m_end)
							break;
					}
					return *this;
				}
				inline auto operator*()noexcept ->decltype(*m_iterator) {
					return *m_iterator;
				}
				inline bool operator==(WhereIterator const&rhs)const{
					return rhs.m_iterator == this->m_iterator;
				}
				inline bool operator!=(WhereIterator const&rhs)const{
					return rhs.m_iterator != this->m_iterator;
				}
				~WhereIterator() {}
			};
			template<class IteratorT,class PipeT>
			class SelectIterator
			{
				IteratorT m_iterator;
				PipeT m_pipe;
			public:
				template<class FunctorT>
				SelectIterator(IteratorT const&iterator,FunctorT&& pipe)
					:m_iterator(iterator),m_pipe(std::forward<FunctorT>(pipe)){
				}
				inline SelectIterator&operator++() {
					++m_iterator;
					return *this;
				}
				inline constexpr auto operator*()noexcept ->decltype(m_pipe(*m_iterator)) {
					return m_pipe(*m_iterator);
				}
				inline bool operator==(SelectIterator const&rhs)const {
					return rhs.m_iterator == this->m_iterator;
				}
				inline bool operator!=(SelectIterator const&rhs)const {
					return rhs.m_iterator != this->m_iterator;
				}
				~SelectIterator() {}
			};
			template<class IteratorT>
			class OrderIterator
			{
				typedef decltype(&((IteratorT*)nullptr)->operator*()) PtrType;
				Collection::LinearMemoryManager<PtrType> m_ptrs;
				size_t m_index = 0;
			public:
				OrderIterator(Collection::LinearMemoryManager<PtrType>&&sorted)
					:m_ptrs(std::move(sorted)){
				}
				typedef decltype(((IteratorT*)nullptr)->operator*()) UnRefenceType;
				inline UnRefenceType operator*() {
					return *m_ptrs.At(m_index);
				}
				OrderIterator& operator++() {
					m_index++;
					return *this;
				}
				inline bool operator==(OrderIterator const&other)const {
					return this->m_index == other.m_index;
				}
				inline bool operator!=(OrderIterator const&other)const {
					return this->m_index != other.m_index;
				}
			};


			template<class IteratorT>
			class LinqResult :public LinqBase<IteratorT, LinqResult<IteratorT>>
			{
				IteratorT m_begin;
				IteratorT m_end;
			public:
				LinqResult(IteratorT&&begin, IteratorT&&end) :m_begin(std::move(begin)), m_end(std::move(end)) {}
				inline IteratorT& begin()noexcept {
					return m_begin;
				}
				inline IteratorT& end()noexcept {
					return m_end;
				}
				virtual ~LinqResult(){}
			};

			template<class IteratorT,class Derived>
			class LinqBase
			{
			public:
				inline IteratorT begin()noexcept{
					return static_cast<Derived*>(this)->begin();
				}
				inline IteratorT end()noexcept {
					return static_cast<Derived*>(this)->end();
				}
				template<class FunctorT>
				LinqResult<WhereIterator<IteratorT, FunctorT>> where(FunctorT&&filter)
				{
					typedef WhereIterator<IteratorT, FunctorT> IteratorType;
					typedef LinqResult<IteratorType> ReturnType;
					return ReturnType(IteratorType(begin(), end(), std::forward<FunctorT>(filter)), IteratorType(end(), end(), std::forward<FunctorT>(filter)));
				}
				template<class FunctorT>
				LinqResult<SelectIterator<IteratorT, FunctorT>> select(FunctorT&&pipe) {
					typedef SelectIterator<IteratorT, FunctorT> IteratorType;
					typedef LinqResult<IteratorType> ReturnType;
					return ReturnType(IteratorType(begin(), std::forward<FunctorT>(pipe)), IteratorType(end(), std::forward<FunctorT>(pipe)));
				}
				virtual ~LinqBase() {}
			};
		}
	}
}