#pragma once
#include <chrono>
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
namespace HL
{
	namespace System
	{
		namespace Diagnosis
		{
			class time
			{
			public:
				template<class time_t, class target_t, class...Args>
				static long long measure_time(target_t&&target, size_t cnt, Args&&...args)
				{
					long long total = 0;
					for (size_t i = 0; i < cnt; ++i)
					{
						std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
						std::forward<target_t>(target)(std::forward<Args>(args)...);
						std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
						time_t time = std::chrono::duration_cast<time_t>(end - begin);
						total += time.count();
					}
					return total / cnt;
				}
			};

			class memory
			{
			public:
				inline static void enable_memory_leak_report()noexcept
				{
					_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
				}
				inline static void break_allocation_at(long flag)noexcept 
				{
					_CrtSetBreakAlloc(flag);
				}
			};
		}
	}
}