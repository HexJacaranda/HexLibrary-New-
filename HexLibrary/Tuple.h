#pragma once
namespace HL
{
	namespace System
	{
		namespace Collection
		{
			template<size_t Index,class FirstT,class...Args>
			struct TupleTraits
			{
				typedef typename TupleTraits<Index - 1, Args...>::BaseType BaseType;
				typedef typename TupleTraits<Index - 1, Args...>::TargetType TargetType;
			};

			template<class FirstT,class...Args>
			struct TupleTraits<0, FirstT, Args...>
			{
				typedef Tuple<FirstT, Args...> BaseType;
				typedef FirstT TargetType;
			};

			template<>
			class Tuple<>
			{
			public:
				Tuple() {}
				~Tuple() {}
			};
			template<>
			class Tuple<void>
			{
			public:
				Tuple() {}
				~Tuple() {}
			};

			//ิชื้
			template<class T, class...Rest>
			class Tuple<T, Rest...> :private Tuple<Rest...>
			{
			public:
				T Value;
				template<typename = std::enable_if_t<std::is_default_constructible_v<T>,int>>
				Tuple() noexcept(std::is_nothrow_default_constructible_v<T>)
					:Value() {}
				template<class FirstT,class...Others, typename = std::enable_if_t<std::is_constructible_v<T,FirstT const&>,int>>
				constexpr Tuple(Tuple<FirstT, Others...>const&tuple) noexcept(std::is_nothrow_constructible_v<T, FirstT const&>)
					: Value(tuple.Value), Tuple<Rest...>(tuple.GetRest()) {}
				template<class FirstT,class...Others, typename = std::enable_if_t<std::is_constructible_v<T,FirstT>,int>>
				constexpr Tuple(FirstT&& first,Others&&...others) noexcept(std::is_nothrow_constructible_v<T,FirstT>)
					: Value(std::forward<FirstT>(first)), Tuple<Rest...>(std::forward<Others>(others)...) {}
				~Tuple() = default;
				Tuple(Tuple<T, Rest...>const&) = default;
				Tuple(Tuple<T, Rest...>&&) = default;
				Tuple& operator=(Tuple const&) = default;
				Tuple& operator=(Tuple&&) = default;

				constexpr inline Tuple<Rest...>&GetRest()noexcept {
					return (Tuple<Rest...>&)(*this);
				}
				constexpr inline Tuple<Rest...> const&GetRest()const noexcept {
					return (Tuple<Rest...>&)(*this);
				}
				template<size_t index>
				constexpr inline typename TupleTraits<index,T,Rest...>::TargetType & GetAt()noexcept{
					return ((typename TupleTraits<index, T, Rest...>::BaseType*)this)->Value;
				}
				template<size_t index>
				constexpr inline typename TupleTraits<index, T, Rest...>::TargetType const&GetAt()const noexcept{
					return ((typename TupleTraits<index, T, Rest...>::BaseType const*)this)->Value;
				}
			};
		}
	}
}