#pragma once
#include <process.h>
#include <Windows.h>
#include <atomic>

#undef max
#undef GetObject
namespace HL
{
	namespace System
	{
		namespace Threading
		{
			typedef DWORD Span;

			class NativeThread
			{
			public:
				typedef HANDLE Handle;
				//等待无限时间
				static constexpr unsigned long Infinite = 0xFFFFFFFF;
				//睡眠
				inline static void SleepFor(Span Miliseconds) {
					Sleep(Miliseconds);
				}
				//启动线程
				template<class Fun>
				inline static Handle StartNativeThread(Fun Target, size_t StackSize, void*Args)noexcept {
					return (Handle)_beginthread(Target, (unsigned int)StackSize, Args);
				}
				//等待线程
				inline static void WaitFor(Handle Target, Span Miliseconds)noexcept {
					WaitForSingleObject(Target, Miliseconds);
				}
				//挂起线程
				inline static void Suspend(Handle Target)noexcept {
#if _LIB_X64
					Wow64SuspendThread(Target);
#else
					SuspendThread(Target);
#endif
				}
				inline static void Resume(Handle Target)noexcept {
					ResumeThread(Target);
				}
				//关闭线程
				inline static void Close(Handle Target) {
					_endthread();
				}
				inline static unsigned long CurrentThreadID()noexcept {
					return GetCurrentThreadId();
				}
			};
		}
	}
}