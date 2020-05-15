#pragma once
namespace HL
{
	namespace System
	{
		namespace Utility
		{
			template<class R>struct RemoveReference { typedef R T; };
			template<class R>struct RemoveReference<R&> { typedef R T; };
			template<class R>struct RemoveReference<R const&> { typedef R const T; };
			template<class R>struct RemoveReference<R volatile&> { typedef R volatile T; };
			template<class R>struct RemoveReference<R const volatile&> { typedef R const volatile T; };
			template<class R>struct RemoveReference<R&&> { typedef R T; };
#ifdef _USE_RIGHT_VALUE_REFERENCE
			template<class R>struct RemoveReference<R const&&> { typedef R const T; };
			template<class R>struct RemoveReference<R volatile&&> { typedef R volatile T; };
			template<class R>struct RemoveReference<R const volatile&&> { typedef R const volatile T; };
#endif
			template<class R>struct RemoveConst { typedef R  T; };
			template<class R>struct RemoveConst<R const> { typedef R  T; };
			template<class R>struct RemoveConst<R const&> { typedef R&  T; };
			template<class R>struct RemoveConst<R volatile const> { typedef R volatile  T; };
			template<class R>struct RemoveConst<R volatile const&> { typedef R volatile&  T; };
#ifdef _USE_RIGHT_VALUE_REFERENCE
			template<class R>struct RemoveConst<R const&&> { typedef R&& T; };
			template<class R>struct RemoveConst<R volatile const&&> { typedef R volatile&& T; };
#endif
			template<class R>struct RemoveVolatile { typedef R T; };
			template<class R>struct RemoveVolatile<R volatile> { typedef R T; };
			template<class R>struct RemoveVolatile<R volatile&> { typedef R& T; };
			template<class R>struct RemoveVolatile<R volatile const> { typedef R const T; };
			template<class R>struct RemoveVolatile<R volatile const&> { typedef R const& T; };
#ifdef _USE_RIGHT_VALUE_REFERENCE
			template<class R>struct RemoveVolatile<R volatile&&> { typedef R && T; };
			template<class R>struct RemoveVolatile<R volatile const&&> { typedef R const&& T; };
#endif
			template<class L, class R>
			struct IsSame {
				static constexpr bool Value = false;
			};
			template<class U>
			struct IsSame<U, U> {
				static constexpr bool Value = true;
			};

			template<class UDT>
			struct IsPod {
				static constexpr bool Value = std::is_trivially_copyable<UDT>::value;
			};

			template<class U>inline long ___IsClassOrUnionTest(void(U::* p)()) { return 0; }
			template<class U>inline char ___IsClassOrUnionTest(...) { return 0; }
			template<class U>inline long ___IsBaseOfTest(U* p) { return 0; }
			template<class U>inline char ___IsBaseOfTest(...) { return 0; }
			template<class Base, class Derived>struct IsBaseOf
			{
			private:
				typedef typename RemoveVolatile<typename RemoveConst<typename RemoveReference<Base>::T>::T>::T B;
				typedef typename RemoveVolatile<typename RemoveConst<typename RemoveReference<Derived>::T>::T>::T D;
			public:
				static constexpr bool Value = sizeof(___IsClassOrUnionTest<Base>(0)) == sizeof(long) && sizeof(___IsBaseOfTest<B>((D*)0)) == sizeof(long);
			};

			template<class type>
			struct is_callable
			{
			private:
				template<typename U, typename...Args>
				static auto test(int) -> decltype(std::declval<U>()(std::declval<Args>()...), std::true_type());
				template<typename U>
				static std::false_type test(...);
			public:
				static constexpr bool value = std::is_same<decltype(test<type>(0)), std::true_type>::value;
			};

			template<class type,class...args>
			struct callable_return
			{
				typedef decltype(std::declval<type>()(std::declval<args>()...)) return_type;
			};

			template<class type,class...args>
			using callable_return_t = typename callable_return<type, args...>::return_type;

			template<class type>
			struct has_indexer
			{
			private:
				template<typename U, typename IndexT>
				static auto test(int) -> decltype((std::declval<U>())[std::declval<IndexT>()], std::true_type());
				template<typename U>
				static std::false_type test(...);
			public:
				static constexpr bool value = std::is_same<decltype(test<type>(0)), std::true_type>::value;
			};

			template<class Target>
			inline constexpr void move_assign(Target& left, Target& right, typename std::enable_if_t<!std::is_trivially_copyable_v<Target>, int>* = 0)
				noexcept(std::is_nothrow_move_assignable_v<Target>) {
				left = std::move(right);
			}
			template<class Target>
			inline constexpr void move_assign(Target& left, Target& right, typename std::enable_if_t<std::is_trivially_copyable_v<Target>, int>* = 0)
				noexcept(std::is_nothrow_move_assignable_v<Target>) {
				left = std::move(right);
				right = { 0 };
			}
			template<class...Args>
			inline constexpr void Eval(Args&&...) {}
		}
	}
}
