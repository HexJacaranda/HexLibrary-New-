#pragma once
namespace HL
{
	namespace System
	{
		namespace Threading
		{
			namespace Lock
			{
				class Mutex
				{
					HANDLE m_mutex_ptr = nullptr;
				public:
					typedef HANDLE NativeHandle;
					Mutex()noexcept {
						m_mutex_ptr = CreateMutexW(nullptr, false, nullptr);
					}
					Mutex(Mutex const&) = delete;
					Mutex(Mutex&&) = delete;
					Mutex& operator=(Mutex const&) = delete;
					Mutex& operator=(Mutex&&) = delete;
					~Mutex() {
						if (m_mutex_ptr != nullptr)
						{
							CloseHandle(m_mutex_ptr);
							m_mutex_ptr = nullptr;
						}
					}

					inline void Lock() noexcept{
						WaitForSingleObject(m_mutex_ptr, INFINITE);
					}
					inline void Lock(DWORD miliseconds) noexcept {
						WaitForSingleObject(m_mutex_ptr, miliseconds);
					}
					inline void Unlock()noexcept {
						ReleaseMutex(m_mutex_ptr);
					}
					
					inline NativeHandle GetNativeHandle()const noexcept {
						return m_mutex_ptr;
					}
				};

				class RWLock
				{
					SRWLOCK m_lock;
				public:
					typedef SRWLOCK* NativeHandle;

					RWLock() {
						InitializeSRWLock(&m_lock);
					}
					RWLock(RWLock const&) = delete;
					RWLock(RWLock&&) = delete;
					RWLock& operator=(RWLock const&) = delete;
					RWLock& operator=(RWLock &&) = delete;
					~RWLock() = default;
					inline void AcquireWrite()noexcept {
						AcquireSRWLockExclusive(&m_lock);
					}
					inline void AcquireRead()noexcept {
						AcquireSRWLockShared(&m_lock);
					}
					_Requires_lock_held_(m_lock)
					inline void ReleaseWrite()noexcept {			
						ReleaseSRWLockExclusive(&m_lock);
					}
					inline void ReleaseRead()noexcept {
						ReleaseSRWLockShared(&m_lock);
					}
					inline NativeHandle GetNativeHandle()noexcept {
						return &m_lock;
					}
				};

				class SpinLock
				{
					std::atomic_flag m_flag = ATOMIC_FLAG_INIT;
				public:
					SpinLock() = default;
					SpinLock(SpinLock const&) = delete;
					SpinLock(SpinLock&&) = delete;
					SpinLock& operator=(SpinLock const&) = delete;
					SpinLock& operator=(SpinLock &&) = delete;
					~SpinLock() = default;
					inline void Lock()noexcept {
						while (m_flag.test_and_set(std::memory_order_acquire));
					}
					inline void Unlock()noexcept {
						m_flag.clear(std::memory_order_release);
					}
				};

				class CriticalSection
				{
					CRITICAL_SECTION m_critical_section;
				public:
					typedef CRITICAL_SECTION* NativeHandle;
					CriticalSection()noexcept {
						InitializeCriticalSection(&m_critical_section);
					}
					CriticalSection(CriticalSection const&) = delete;
					CriticalSection(CriticalSection&&) = delete;
					CriticalSection& operator=(CriticalSection const&) = delete;
					CriticalSection& operator=(CriticalSection &&) = delete;
					~CriticalSection() {
						DeleteCriticalSection(&m_critical_section);
					}

					inline void Lock()noexcept {
						EnterCriticalSection(&m_critical_section);
					}
					inline void Unlock()noexcept {
						LeaveCriticalSection(&m_critical_section);
					}
					inline NativeHandle GetNativeHandle()noexcept {
						return &m_critical_section;
					}
				};

				class Interlock
				{
					__declspec(align(4))volatile unsigned long m_thread_id = 0;
					volatile long m_lock_cnt = 0;
					int m_collision_cnt = 0;
				public:
					void Lock() {
						unsigned long id = Threading::NativeThread::CurrentThreadID();
						if (id == m_thread_id)
							m_lock_cnt++;
						else
						{
							int cnt = 0;
							for (;;) {
								++cnt;
								if (InterlockedCompareExchange(&m_thread_id, id, 0) == 0) {
									m_lock_cnt = 1;
									if (cnt > m_collision_cnt)
										m_collision_cnt = cnt;
									return;
								}
								Threading::NativeThread::SleepFor(0);
							}
						}
					}
					void Unlock() {
						unsigned long id = Threading::NativeThread::CurrentThreadID();
						if (id == m_thread_id)
						{
							--m_lock_cnt;
							if (m_lock_cnt == 0)
								m_thread_id = 0;
						}
					}
				};
			}
		}
	}
}