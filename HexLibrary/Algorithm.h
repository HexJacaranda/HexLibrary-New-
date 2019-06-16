#pragma once
namespace HL
{
	namespace System
	{
		namespace Algorithm
		{
			template<class U, class T>
			static inline bool Any(U const&Target, T const&First)noexcept {
				return Target == First;
			}
			template<class U, class T, class...Args>
			static inline bool Any(U const&Target, T const&First, Args const&...Rest)noexcept {
				if (Target == First)
					return true;
				return Any(Target, Rest...);
			}
			template<class U>
			static size_t IndexOf(const U*Objects, size_t Count, U const&Key)noexcept {
				for (size_t i = 0; i < Count; ++i)
					if (Objects[i] == Key)
						return i;
				return std::numeric_limits<size_t>::max();
			}
		}
	}
}