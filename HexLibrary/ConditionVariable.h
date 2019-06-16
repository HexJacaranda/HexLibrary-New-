#pragma once
namespace HL
{
	namespace System
	{
		namespace Threading
		{
			//条件变量
			class ConditionVariable {
				CONDITION_VARIABLE m_condition_variable;
			public:
				typedef CONDITION_VARIABLE* NativeHandle;

				ConditionVariable()noexcept {
					InitializeConditionVariable(&m_condition_variable);
				}
				ConditionVariable(ConditionVariable&&) = delete;;
				ConditionVariable(ConditionVariable const&) = delete;
				ConditionVariable& operator=(ConditionVariable const&) = delete;
				ConditionVariable& operator=(ConditionVariable&&) = delete;

				inline void WakeUpAll()noexcept {
					WakeAllConditionVariable(&m_condition_variable);
				}
				inline void WakeUpSingle()noexcept {
					WakeConditionVariable(&m_condition_variable);
				}
				inline void Wait(Lock::RWLock &root, Span miliseconds)noexcept {
					root.AcquireRead();
					SleepConditionVariableSRW(&m_condition_variable, root.GetNativeHandle(), miliseconds, 0);
					root.ReleaseRead();
				}
				inline void Wait(Lock::RWLock &root)noexcept {
					root.AcquireRead();
					SleepConditionVariableSRW(&m_condition_variable, root.GetNativeHandle(), NativeThread::Infinite, 0);
					root.ReleaseRead();
				}
				inline void Wait(Lock::CriticalSection &root, Span miliseconds)noexcept {
					root.Lock();
					SleepConditionVariableCS(&m_condition_variable, root.GetNativeHandle(), miliseconds);
					root.Unlock();
				}
				inline void Wait(Lock::CriticalSection &root)noexcept {
					root.Lock();
					SleepConditionVariableCS(&m_condition_variable, root.GetNativeHandle(), NativeThread::Infinite);
					root.Unlock();
				}
				inline NativeHandle GetNativeHandle()noexcept {
					return &m_condition_variable;
				}
			};
		}
	}
}