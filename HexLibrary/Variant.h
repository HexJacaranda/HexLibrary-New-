#pragma once
namespace HL
{
	namespace System
	{
		namespace Variance
		{
			template<class...Args>
			struct PickMaxSize;

			template<class T>
			struct PickMaxSize<T>
			{
				enum { Size = sizeof(T) };
			};

			template<class T, class...Args>
			struct PickMaxSize<T, Args...>
			{
				enum { NextSize = PickMaxSize<Args...>::Size };
				enum { Size = NextSize > sizeof(T) ? NextSize : sizeof(T) };
			};

			template<class...Args>
			struct Flag
			{
				//偷懒，variant参数哪有过127个的
				enum { Size = 1 };
				typedef unsigned char IndexT;
			};

			template<int Current, class T, class...Args>
			struct PickIndexAtImpl;

			template<int Current, class T>
			struct PickIndexAtImpl<Current, T>
			{
				enum { Index = -1 };
			};

			template<int Current, class T, class First, class...Rest>
			struct PickIndexAtImpl<Current, T, First, Rest...>
			{
				enum { Index = Utility::IsSame<T, First>::Value ? Current : PickIndexAtImpl<Current + 1, T, Rest...>::Index };
			};

			template<class T, class...Args>
			struct PickIndexAt
			{
				enum { Index = PickIndexAtImpl<0, T, Args...>::Index };
			};

			template<class...Args>
			struct DestructHelper;

			template<class FirstT,class...Rest>
			struct DestructHelper<FirstT, Rest...>
			{
				inline static void Destruct(char* value_ptr, int index) {
					if (index == 0)
						return Memory::Destruct(reinterpret_cast<FirstT*>(value_ptr));
					DestructHelper<Rest...>::Destruct(value_ptr, --index);
				}
			};

			template<class LastT>
			struct DestructHelper<LastT>
			{
				inline static void Destruct(char* value_ptr, int index) {
					if (index == 0)
						Memory::Destruct(reinterpret_cast<LastT*>(value_ptr));
				}
			};

			template<class...Args>
			struct VisitHelper;

			template<class FirstT, class...Rest>
			struct VisitHelper<FirstT, Rest...>
			{
				template<class Fn>
				inline static void Visit(char* value_ptr, int index, Fn&& visitor) {
					if (index == 0)
						std::forward<Fn>(visitor)(*reinterpret_cast<FirstT*>(value_ptr));
					else
						VisitHelper<Rest...>::Visit(value_ptr, --index, std::forward<Fn>(visitor));
				}
			};

			template<class LastT>
			struct VisitHelper<LastT>
			{
				template<class Fn>
				static void Visit(char* value_ptr, int index, Fn&& visitor) {
					if (index == 0)
						std::forward<Fn>(visitor)(*reinterpret_cast<LastT*>(value_ptr));
				}
			};

			template<class...Args>
			class Variant
			{
				typedef typename Flag<Args...>::IndexT IndexT;

				char m_value[PickMaxSize<Args...>::Size + Flag<Args...>::Size];
				inline IndexT const& Index()const {
					return *reinterpret_cast<IndexT*>(m_value + PickMaxSize<Args...>::Size);
				}
				inline IndexT& Index() {
					return *reinterpret_cast<IndexT*>(m_value + PickMaxSize<Args...>::Size);
				}
				template<class T>
				constexpr static int PickIndex() {
					constexpr int index = PickIndexAt<T, Args...>::Index;
					static_assert(index != -1, "no such type");
					return index;
				}
			public:
				Variant() {
					Index() = std::numeric_limits<IndexT>::max();
				}
				template<class T>
				Variant(T&& value) {
					using OriginT = typename Utility::RemoveConst<typename Utility::RemoveReference<T>::T>::T;
					Index() = PickIndex<OriginT>();
					Memory::PlacementNew((OriginT*)m_value, std::forward<T>(value));
				}
				template<class T>
				T& As() {
					constexpr int index = PickIndex<T>();
					if (Index() == index)
						return *reinterpret_cast<T*>(m_value);
					Exception::Throw<Exception::InvalidArgumentException>();
				}
				template<class T>
				T const& As()const {
					constexpr int index = PickIndex<T>();
					if (Index() == index)
						return *reinterpret_cast<const T*>(m_value);
					Exception::Throw<Exception::InvalidArgumentException>();
				}
				template<class T>
				void Set(T && value) {
					using OriginT = typename Utility::RemoveConst<typename Utility::RemoveReference<T>::T>::T;
					constexpr int set_index = PickIndex<OriginT>();
					IndexT index = Index();
					if (index == std::numeric_limits<IndexT>::max())
						Memory::PlacementNew((OriginT*)m_value, std::forward<T>(value));
					else if (set_index == index)
						*reinterpret_cast<OriginT*>(m_value) = std::forward<T>(value);
					else
					{
						DestructHelper<Args...>::Destruct(m_value, index);
						Memory::PlacementNew((OriginT*)m_value, std::forward<T>(value));
					}
					Index() = set_index;
				}
				template<class Fn>
				void Visit(Fn&& receiver) {
					IndexT index = Index();
					if (index != std::numeric_limits<IndexT>::max())
						VisitHelper<Args...>::Visit(m_value, index, std::forward<Fn>(receiver));
				}
				template<class T>
				inline bool IsPresent() {
					return Index() == PickIndex<T>();
				}
				~Variant() {
					IndexT index = Index();
					if (index != std::numeric_limits<IndexT>::max())
						DestructHelper<Args...>::Destruct(m_value, index);
				}
			};
		}
	}
}
